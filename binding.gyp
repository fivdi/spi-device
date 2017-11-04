{
  "targets": [{
    "target_name": "spi",
    "conditions": [[
      "OS == \"linux\"", {
        "cflags": [
          "-Wno-deprecated-declarations"
        ]
      }]
    ],
    "include_dirs" : [
      "<!(node -e \"require('nan')\")"
    ],
    "sources": [
      "./src/spi.cc",
      "./src/spidevice.cc",
      "./src/open.cc",
      "./src/close.cc",
      "./src/transfer.cc",
      "./src/getoptions.cc",
      "./src/setoptions.cc"
    ]
  }]
}

