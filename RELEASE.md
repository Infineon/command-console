# Command console library

## What's included?

See the [README.md](./README.md) for a complete description of the command console library.

## Known issues
| Problem | Workaround |
| ------- | ---------- |
| Running iperf command in client UDP mode with the `-i 1` (interval) option reports a lower throughput. | Run the iPerf without the `-i` (interval) option (or) use an interval value greater than 1 second. |


## Changelog

### v3.0.0

- Minor documentation and BT configuration updates

- Fix added for smartcoex LE disconnection issue

- Introduced ARMC6 toolchain build support for AnyCloud

- Integrated with BTSTACK library version v3.x


### v2.0.0

- Added support of the command console library for the FreeRTOS framework

- Added Wi-Fi diagnostic commands to perform network operations

- Added BT diagnostic commands to perform Bluetooth&reg; LE operations

### v1.0.1

- Documentation updates

- For the IAR toolchain, added a fix to echo the commands as they are typed

### v1.0.0

- This is the first version of the command console library for Mbed OS and AnyCloud

## Supported software and tools

The current version of the library was validated for compatibility with the following software and tools:

| Software and tools                                      | Version |
| :---                                                    | :----:  |
| ModusToolbox&trade; software                            | 2.3.1   |
| PSoC&trade; 6 peripheral driver library (PDL)           | 2.2.0   |
| GCC Compiler                                            | 9.3.1   |
| IAR Compiler (only for AnyCloud)                        | 8.32    |
| Arm® Compiler 6                                         | 6.14    |
| Mbed OS                                                 | 6.2.0   |
