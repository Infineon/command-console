# Command Console Library

## What's Included?

See the [README.md](./README.md) for a complete description of the Command Console Library.

## Known Issues
| Problem | Workaround |
| ------- | ---------- |
| Intermittently LE Rx data may not be received when the LE COC connection is initiated by the PSoC® 6 MCU kit. | Disconnect and reconnect from the peer device to resume Rx data. Will be fixed in a future release. |
| Running iperf command in client UDP mode with "-i 1" (interval) option reports lower throughput. | Run the iperf without the -i (interval) option (or) use a interval value greater than 1 second. |
| At times, after running the iperf TCP client command on the device may not return the control back to the user, to enter new command on the console. | Reboot the device to enter the command. |

## Changelog

### v2.0.0

- Added support of the Command Console Library for the Amazon FreeRTOS framework.

- Added Wi-Fi diagnostic commands to perform network operations.

- Added BT diagnostic commands to perform BLE operations.

### v1.0.1

- Documentation updates.

- For IAR toolchain, added a fix to echo the commands as they are typed.

### v1.0.0

- This is the first version of the Command Console Library for Mbed OS and AnyCloud.

## Supported Software and Tools

The current version of the library was validated for compatibility with the following software and tools:

| Software and Tools                                      | Version |
| :---                                                    | :----:  |
| ModusToolbox® software environment                      | 2.2     |
| - ModusToolbox Device Configurator                      | 2.20    |
| - ModusToolbox CapSense Configurator / Tuner tools      | 3.10    |
| PSoC 6 Peripheral Driver Library (PDL)                  | 2.0.0   |
| GCC Compiler                                            | 9.3.1   |
| IAR Compiler (only for AnyCloud)                        | 8.32    |
| Arm® Compiler 6                                         | 6.14    |
| Mbed OS                                                 | 6.2.0   |
