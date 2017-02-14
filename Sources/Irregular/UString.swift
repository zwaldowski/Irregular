//
//  UString.swift
//  Irregular
//
//  Created by Zachary Waldowski on 2/5/17.
//

import CUnicode

private extension UText {

    mutating func withMutableBuffer<R>(_ body: (inout UnsafeMutableBufferPointer<UInt16>) throws -> R) rethrows -> R {
        // Currently we are using the p, q, and r fields to get 12 UWords of
        // contiguous storage on 64-bit machines and 6 on 32-bit.  It's not much.
        return try withUnsafeMutablePointer(to: &p) { bufferStart in
            let rawBufferStart = UnsafeRawPointer(bufferStart)
            let capacity = withUnsafeMutablePointer(to: &privP) { bufferLimit in
                (UnsafeRawPointer(bufferLimit).assumingMemoryBound(to: Int8.self) - rawBufferStart.assumingMemoryBound(to: Int8.self)) / MemoryLayout<UInt16>.stride
            }
            let start = rawBufferStart.bindMemory(to: UInt16.self, capacity: capacity)
            let mutableStart = UnsafeMutablePointer(mutating: start)
            var buffer = UnsafeMutableBufferPointer(start: mutableStart, count: capacity)
            return try body(&buffer)
        }
    }

    mutating func setup() {
        withMutableBuffer {
            chunkContents = UnsafePointer($0.baseAddress)
        }
    }

    mutating func validate() {
        withMutableBuffer {
            precondition(chunkContents == UnsafePointer($0.baseAddress), "UText moved!")
        }
    }

}

private protocol UTextable {
    func nativeLength(from text: UnsafePointer<UText>) -> Int64
    
    func access(from text: inout UText, atIndex nativeIndex: Int64, isForward forward: Bool) -> Bool

    func extract(from text: inout UText, start nativeStart: Int64, end nativeLimit: Int64, to destination: inout UnsafeMutableBufferPointer<UInt16>, status: inout UErrorCode) -> Int32

    func mapOffsetToNative(from u: UnsafePointer<UText>) -> Int64
    func mapNativeIndexToUTF16(from u: UnsafePointer<UText>, nativeIndex: Int64) -> Int32
}

private var swiftStringFuncs = UTextFuncs(
    tableSize: numericCast(MemoryLayout<UTextFuncs>.stride),
    reserved1: 0, reserved2: 0, reserved3: 0,
    clone: { (destination, existing, deep, status) in
        guard status.pointee.isSuccess else { return nil }
        precondition(deep == 0, "deep cloning not supported")
        UnsafeMutablePointer(mutating: existing).pointee.validate()
        guard var text = UText.setup(destination, extraSpace: 0, status: status), status.pointee.isSuccess else { return destination }

        text.pointee.providerProperties = existing.pointee.providerProperties.union(.ownsText)

        let context = UnsafeMutablePointer<UTextable>.allocate(capacity: 1)
        context.initialize(from: existing.pointee.context!.assumingMemoryBound(to: UTextable.self), count: 1)
        text.pointee.context = UnsafeMutableRawPointer(context)

        text.pointee.chunkNativeStart = existing.pointee.chunkNativeStart
        text.pointee.chunkNativeLimit = existing.pointee.chunkNativeStart
        text.pointee.chunkOffset = 0
        text.pointee.chunkLength = 0

        text.pointee.pFuncs = existing.pointee.pFuncs
        
        text.pointee.setup()
        text.pointee.validate()
        
        return text
    }, nativeLength: { (text) in
        text.pointee.validate()

        let _self = text.pointee.context!.assumingMemoryBound(to: UTextable.self).pointee
        return _self.nativeLength(from: text)
    }, access: { (text, index, forward) -> Int8 in
        text.pointee.validate()

        let _self = text.pointee.context!.assumingMemoryBound(to: UTextable.self).pointee
        return _self.access(from: &text.pointee, atIndex: index, isForward: forward != 0) ? 1 : 0
    }, extract: { (text, start, limit, dest, destCapacity, status) -> Int32 in
        text.pointee.validate()

        let _self = text.pointee.context!.assumingMemoryBound(to: UTextable.self).pointee
        var destination = UnsafeMutableBufferPointer(start: dest, count: numericCast(destCapacity))
        return _self.extract(from: &text.pointee, start: start, end: limit, to: &destination, status: &status.pointee)
    }, replace: nil, copy: nil, mapOffsetToNative: { (text) in
        UnsafeMutablePointer(mutating: text).pointee.validate()
        let _self = text.pointee.context!.assumingMemoryBound(to: UTextable.self).pointee
        return _self.mapOffsetToNative(from: text)
    }, mapNativeIndexToUTF16: { (text, index) in
        UnsafeMutablePointer(mutating: text).pointee.validate()
        let _self = text.pointee.context!.assumingMemoryBound(to: UTextable.self).pointee
        return _self.mapNativeIndexToUTF16(from: text, nativeIndex: index)
    }, close: { (text) in
        if text.pointee.providerProperties.contains(.ownsText) {
            let ptr = text.pointee.context!.assumingMemoryBound(to: UTextable.self)
            ptr.deinitialize(count: 1)
            ptr.deallocate(capacity: 1)
        }
    },
    spare1: nil, spare2: nil, spare3: nil
)

extension UTextable {

    func withUText<R>(_ body: (UnsafeMutablePointer<UText>) throws -> R) rethrows -> R {
        var copy: UTextable = self
        return try withUnsafePointer(to: &copy) { pSelf in
            var u = UText(vtable: &swiftStringFuncs, context: UnsafeMutableRawPointer(mutating: pSelf))
            u.setup()
            u.validate()
            return try body(&u)
        }
    }
}

private extension Comparable {
    func clamped(to r: ClosedRange<Self>) -> Self {
        return self < r.lowerBound ? r.lowerBound
            : self > r.upperBound ? r.upperBound
            : self
    }
}

private extension MutableCollection {
    /// Copies elements from `source` into `self`, starting at the beginning of
    /// each.
    ///
    /// - Returns:
    ///
    ///   - `limit`: the first index in `self` that was not copied into, or
    ///     `endIndex` if all elements were assigned.
    ///
    ///   - `remainder`: the subsequence of source that didn't fit into `self`,
    ///     or `source[endIndex...]` if all elements fit.
    @discardableResult
    mutating func copy<Source: Collection>(from source: Source) -> (limit: Index, remainder: Source.SubSequence)
        where Source.SubSequence: Collection, Source.SubSequence.Iterator.Element == Iterator.Element, Source.SubSequence == Source.SubSequence.SubSequence {
        // This method should be optimizable for segmented collections
        var r = source[source.startIndex ..< source.endIndex]
        var i: Index = startIndex
        while i != endIndex {
            guard let e = r.popFirst() else { return (limit: i, remainder: r) }
            self[i] = e
            formIndex(after: &i)
        }
        return (limit: endIndex, remainder: r)
    }
}

extension String.UTF16View: UTextable {

    func nativeLength(from text: UnsafePointer<UText>) -> Int64 {
        return numericCast(count)
    }

    func access(from text: inout UText, atIndex nativeTargetIndex: Int64, isForward forward: Bool) -> Bool {
        text.chunkOffset = 0

        let inBoundsTarget = nativeTargetIndex - (forward ? 0 : 1)
        if (text.chunkNativeStart ..< text.chunkNativeLimit).contains(inBoundsTarget) {
            let start = index(startIndex, offsetBy: numericCast(text.chunkNativeStart))
            let end = index(startIndex, offsetBy: numericCast(nativeTargetIndex))
            text.chunkOffset += numericCast(distance(from: start, to: end))
            return true
        }

        guard let targetIndex = index(startIndex, offsetBy: numericCast(nativeTargetIndex), limitedBy: endIndex) else {
            return false
        }

        text.chunkLength = 0
        text.chunkNativeStart = nativeTargetIndex
        text.chunkNativeLimit = nativeTargetIndex

        text.withMutableBuffer { buffer in
            if forward {
                let chunk = suffix(from: targetIndex).prefix(buffer.count)

                buffer.copy(from: chunk)
                text.chunkLength = numericCast(chunk.count)
                text.chunkNativeLimit = numericCast(distance(from: startIndex, to: chunk.endIndex))
            } else {
                let chunk = prefix(upTo: targetIndex).suffix(buffer.count)

                buffer.copy(from: chunk)
                text.chunkLength = numericCast(chunk.count)
                text.chunkNativeStart = numericCast(distance(from: startIndex, to: chunk.startIndex))
                text.chunkOffset = text.chunkLength
            }
        }

        return true
    }

    func extract(from text: inout UText, start nativeStart: Int64, end nativeLimit: Int64, to destination: inout UnsafeMutableBufferPointer<UInt16>, status: inout UErrorCode) -> Int32 {
        let s = nativeStart.clamped(to: 0 ... numericCast(count))
        let l = nativeLimit.clamped(to: 0 ... numericCast(count))
        text.chunkNativeStart = l
        text.chunkNativeLimit = l
        text.chunkLength = 0

        if s < l { // anything to extract?
            let base = self[
                index(startIndex, offsetBy: numericCast(s)) ..< index(startIndex, offsetBy: numericCast(l))
            ]

            let (limit, remainder) = destination.copy(from: base)

            // Add null termination if it fits
            if limit < destination.endIndex { destination[limit] = 0 }

            // If no overflow, we're done
            if remainder.isEmpty {
                return numericCast(destination.distance(from: destination.startIndex, to: limit))
            }

            // Report the error and account for the overflow length in the return value
            status = .BUFFER_OVERFLOW_ERROR
            return Int32(destination.distance(from: destination.startIndex, to: limit) + remainder.count)
        }
        return 0
    }

    func mapOffsetToNative(from text: UnsafePointer<UText>) -> Int64 {
        let start = index(startIndex, offsetBy: Int(text.pointee.chunkNativeStart + Int64(text.pointee.chunkOffset) + 1))
        return numericCast(distance(from: startIndex, to: start))
    }

    func mapNativeIndexToUTF16(from text: UnsafePointer<UText>, nativeIndex: Int64) -> Int32 {
        let start = index(startIndex, offsetBy: numericCast(text.pointee.chunkNativeStart))
        let end = index(startIndex, offsetBy: numericCast(nativeIndex))
        return numericCast(distance(from: start, to: end))
    }

}

extension String {

    func withUText<R>(_ body: (UnsafeMutablePointer<UText>) throws -> R) rethrows -> R {
        return try utf16.withUText(body)
    }

}
