import PackageDescription

let package = Package(
    name: "Irregular",
    targets: [
        Target(name: "CUnicode"),
        Target(name: "Irregular", dependencies: [
          .Target(name: "CUnicode")
        ])
    ]
)

let dylib = Product(name: "Irregular", type: .Library(.Dynamic), modules: "Irregular")
products.append(dylib)
