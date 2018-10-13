{
  "targets": [{
    "target_name": "spi",
    "include_dirs" : [
      "<!(node -e \"require('nan')\")"
    ],
    "conditions": [[
      '"<!(echo $V)" != "1"', {
        "cflags": [
          "-Wno-deprecated-declarations"
        ]
      }]
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

