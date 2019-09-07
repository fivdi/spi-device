{
  "targets": [{
    "target_name": "spi",
    "conditions": [[
      'OS == "linux"', {
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
        ],
        "conditions": [[
          '"<!(echo $V)" != "1"', {
            "cflags": [
              "-Wno-deprecated-declarations",
              "-Wno-cast-function-type"
            ]
          }]
        ]
      }]
    ]
  }]
}

