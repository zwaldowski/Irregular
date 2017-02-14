//
//  Irregular.swift
//  Irregular
//
//  Created by Zachary Waldowski on 2/5/17.
//  Copyright © 2017 Zachary Waldowski. All rights reserved.
//

import Dispatch
import CUnicode

private extension URegularExpression {

    mutating func resetText(options: RegularExpression.MatchingOptions) {
        var unusedBuffer: UInt16 = 0
        var unusedError = UErrorCode.ZERO_ERROR

        setTextString(&unusedBuffer, length: 0, status: &unusedError)
        setUsesTransparentBounds(0, status: &unusedError)
        setUsesAnchoringBounds(1, status: &unusedError)
    }
    
}

public final class RegularExpression {

    public typealias Options = URegularExpression.Options

    public struct Error: Swift.Error {
        let pattern: String
        let code: Int
        let line: Int?
        let offset: Int?

        fileprivate init(pattern: String, code: UErrorCode, line: Int32, offset: Int32) {
            self.pattern = pattern
            self.code = Int(code.rawValue)
            self.line = line <= 0 ? nil : Int(line)
            self.offset = offset < 0 ? nil : Int(offset)
        }

        fileprivate init(pattern: String, code: UErrorCode) {
            self.pattern = pattern
            self.code = Int(code.rawValue)
            self.line = nil
            self.offset = nil
        }
    }

    private enum ReuseState {
        case checkedIn(DispatchSemaphore)
        case checkedOut(MatchingOptions, DispatchSemaphore)
        case cloned

        static var new: ReuseState {
            return .checkedIn(DispatchSemaphore(value: 1))
        }
    }

    private let pattern: String
    private let handle: UnsafeMutablePointer<URegularExpression>
    private let reused: ReuseState

    public init(pattern: String, options: Options = []) throws {
        var parseError = UParseError()
        var status = UErrorCode.ZERO_ERROR
        if let handle = pattern.withUText({ URegularExpression.open(pattern: $0, options: options, errorDetails: &parseError, status: &status) }) {
            self.pattern = pattern
            self.handle = handle
            self.reused = .new
        } else {
            throw Error(pattern: pattern, code: status, line: parseError.line, offset: parseError.offset)
        }
    }

    fileprivate init(pattern: StaticString) throws {
        var parseError = UParseError()
        var status = UErrorCode.ZERO_ERROR

        if let handle = pattern.withUTF8Buffer({ URegularExpression.open(cString: $0.baseAddress, options: [], errorDetails: &parseError, status: &status) }) {
            self.pattern = "\(pattern)"
            self.handle = handle
            self.reused = .new
        } else {
            throw Error(pattern: "\(pattern)", code: status, line: parseError.line, offset: parseError.offset)
        }
    }

    private init(checkingOut original: RegularExpression, options: MatchingOptions, semaphore: DispatchSemaphore) {
        self.pattern = original.pattern
        self.handle = original.handle
        self.reused = .checkedOut(options, semaphore)
    }

    private init(cloning original: RegularExpression) throws {
        var status = UErrorCode.ZERO_ERROR
        if let handle = original.handle.pointee.clone(status: &status) {
            self.pattern = original.pattern
            self.handle = handle
            self.reused = .cloned
        } else {
            throw Error(pattern: original.pattern, code: status)
        }
    }

    deinit {
        switch reused {
        case .checkedIn, .cloned:
            handle.pointee.close()
        case let .checkedOut(options, sema):
            handle.pointee.resetText(options: options)
            sema.signal()
        }
    }

    public struct MatchingOptions: OptionSet {
        public let rawValue: UInt
        public init(rawValue: UInt) {
            self.rawValue = rawValue
        }

        public static let anchored = MatchingOptions(rawValue: 1 << 0)
        public static let withTransparentBounds = MatchingOptions(rawValue: 1 << 1)
        public static let withoutAnchoringBounds = MatchingOptions(rawValue: 1 << 2)
    }

    private func checkOut(options: MatchingOptions) throws -> RegularExpression {
        if case .checkedIn(let sema) = reused, case .success = sema.wait(timeout: .now()) {
            return RegularExpression(checkingOut: self, options: options, semaphore: sema)
        } else {
            return try RegularExpression(cloning: self)
        }
    }

    public func matches(in string: String, options: MatchingOptions = [], range: Range<String.Index>? = nil) throws -> Matches {
        var status = UErrorCode.ZERO_ERROR
        return try string.withUText { (text) -> Matches in
            let regex = try checkOut(options: options)
            regex.handle.pointee.setText(text, status: &status)
            regex.handle.pointee.setUsesTransparentBounds(options.contains(.withTransparentBounds) ? 1 : 0, status: &status)
            regex.handle.pointee.setUsesAnchoringBounds(options.contains(.withoutAnchoringBounds) ? 0 : 1, status: &status)

            if let range = range {
                let start = range.lowerBound.samePosition(in: string.utf16)
                let end = range.upperBound.samePosition(in: string.utf16)
                let regionStart: Int64 = numericCast(string.utf16.distance(from: string.utf16.startIndex, to: start))
                let regionLimit: Int64 = numericCast(string.utf16.distance(from: string.utf16.startIndex, to: end))

                regex.handle.pointee.setRegion(start: regionStart, end: regionLimit, status: &status)
            }

            guard status.isSuccess else {
                throw Error(pattern: pattern, code: status)
            }

            return Matches(base: regex, source: string, options: options)
        }
    }

    public struct Matches: IteratorProtocol, Sequence {

        private let base: RegularExpression
        private let source: String
        private let options: MatchingOptions

        fileprivate init(base: RegularExpression, source: String, options: MatchingOptions) {
            self.base = base
            self.source = source
            self.options = options
        }

        fileprivate var numberOfCaptureGroups: Int {
            var errorCode = UErrorCode.ZERO_ERROR
            let ret = base.handle.pointee.numberOfCaptureGroups(status: &errorCode)
            guard errorCode.isSuccess else { return 1 }
            return Int(ret)
        }

        public mutating func next() -> MatchGroup? {
            var errorCode = UErrorCode.ZERO_ERROR
            guard (options.contains(.anchored) && base.handle.pointee.isLooking(atIndex: -1, status: &errorCode) != 0) ||
                (!options.contains(.anchored) && base.handle.pointee.findNext(status: &errorCode) != 0),
                errorCode.isSuccess else {
                return nil
            }

            return MatchGroup(ranges: (0 ..< numberOfCaptureGroups).map({ (i) in
                let startOffset = base.handle.pointee.startIndex(forGroupAtIndex: Int32(i), status: &errorCode)
                let endOffset = base.handle.pointee.endIndex(forGroupAtIndex: Int32(i), status: &errorCode)
                guard errorCode.isSuccess, startOffset >= 0, endOffset >= startOffset,
                    let start = source.utf16.index(source.utf16.startIndex, offsetBy: Int(startOffset)).samePosition(in: source),
                    let end = source.utf16.index(source.utf16.startIndex, offsetBy: Int(endOffset)).samePosition(in: source) else {
                    return source.endIndex ..< source.endIndex
                }
                return start ..< end
            }), within: source)
        }

    }

    public struct MatchGroup: RandomAccessCollection {

        public typealias Indices = CountableRange<Int>

        private let matchAndCaptures: [Range<String.Index>]
        fileprivate let source: String

        fileprivate init(ranges: [Range<String.Index>], within source: String) {
            self.matchAndCaptures = ranges
            self.source = source
        }

        public var range: Range<String.Index>! {
            return matchAndCaptures[0]
        }

        public var startIndex: Int {
            return 1
        }

        public var endIndex: Int {
            return ranges.count
        }

        public subscript(i: Int) -> String {
            return source[matchAndCaptures[i]]
        }

        public var ranges: ArraySlice<Range<String.Index>> {
            return matchAndCaptures.dropFirst()
        }

    }

}

extension RegularExpression: ExpressibleByStringLiteral {

    public convenience init(unicodeScalarLiteral value: StaticString) {
        try! self.init(pattern: value)
    }

    public convenience init(extendedGraphemeClusterLiteral value: StaticString) {
        try! self.init(pattern: value)
    }

    public convenience init(stringLiteral value: StaticString) {
        try! self.init(pattern: value)
    }

}

extension RegularExpression.MatchGroup: CustomStringConvertible, CustomReflectable {

    public var description: String {
        return source[range]
    }

    public var customMirror: Mirror {
        return Mirror(self, children: [
            "range": range,
            "substring": description
        ], displayStyle: .struct)
    }

}
