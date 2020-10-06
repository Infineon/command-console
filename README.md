# Command Console Library 

## Introduction
This library provides a framework to add command console support to your application. Support for Wi-Fi iPerf and Bluetooth BLE commands is  bundled with this library.

[Mbed OS Wi-Fi Bluetooth Tester](https://github.com/cypresssemiconductorco/mtb-anycloud-wifi-bluetooth-tester) and [ModusToolbox AnyCloud Wi-Fi Bluetooth Tester](https://github.com/cypresssemiconductorco/mbed-os-wifi-bluetooth-tester) applications are built over this library.

## Features
- Integrates iPerf 2.0 commands. iPerf is a tool for active measurements of the maximum achievable bandwidth on IP networks. 

- Supports TCP client/server and UDP client/server on IPv4. Supports single instance of iPerf client & server. IPv6 support will be added in the future. 

- Supports Bluetooth LE commands including Tx/Rx for measuring BLE throughput.

**Note:** While iPerf provides several other commands, only a subset of commands that are relevant to embedded connectivity use cases is enabled in this library. Refer to the [Supported iPerf commands](#supported-iperf-commands) section.

## Supported Platforms

### AnyCloud

- [PSoC 6 WiFi-BT Prototyping Kit (CY8CPROTO-062-4343W)](https://www.cypress.com/documentation/development-kitsboards/psoc-6-wi-fi-bt-prototyping-kit-cy8cproto-062-4343w)

- [PSoC 62S2 Wi-Fi BT Pioneer Kit (CY8CKIT-062S2-43012)](https://www.cypress.com/documentation/development-kitsboards/psoc-62s2-wi-fi-bt-pioneer-kit-cy8ckit-062s2-43012)

### Mbed OS

- [PSoC 6 WiFi-BT Prototyping Kit (CY8CPROTO-062-4343W)](https://www.cypress.com/documentation/development-kitsboards/psoc-6-wi-fi-bt-prototyping-kit-cy8cproto-062-4343w)

- [PSoC 62S2 Wi-Fi BT Pioneer Kit (CY8CKIT-062S2-43012)](https://www.cypress.com/documentation/development-kitsboards/psoc-62s2-wi-fi-bt-pioneer-kit-cy8ckit-062s2-43012)

## Supported Frameworks

This library supports the following frameworks:

- **AnyCloud Framework:** AnyCloud is a FreeRTOS-based solution. Command Console Library uses the [abstraction-rtos](https://github.com/cypresssemiconductorco/abstraction-rtos) library that provides the RTOS abstraction API and uses the [secure-sockets](https://github.com/cypresssemiconductorco/secure-sockets) library for implementing socket functions. For the BLE functionality, this library uses [bluetooth-freertos](https://github.com/cypresssemiconductorco/bluetooth-freertos).

- **Mbed Framework:** Mbed framework is a Mbed OS-based solution. Command console Library uses the [abstraction-rtos](https://github.com/cypresssemiconductorco/abstraction-rtos) library that provides RTOS abstraction API, and uses the Mbed socket API for implementing socket functions. For the BLE functionality, it uses the Cordio BLE stack in Mbed OS.

## Dependencies
This section provides the list of dependent libraries required for this middleware library to work on AnyCloud and Arm Mbed OS IoT frameworks.

### AnyCloud

- [Wi-Fi Middleware Core](https://github.com/cypresssemiconductorco/wifi-mw-core)

- [bluetooth-freertos](https://github.com/cypresssemiconductorco/bluetooth-freertos)

### Mbed OS
- [Arm Mbed OS 6.2.0 and above](https://os.mbed.com/mbed-os/releases)

- [Connectivity Utilities Library](https://github.com/cypresssemiconductorco/connectivity-utilities)

## Quick Start

This library is supported on both AnyCloud and Mbed OS frameworks. See the following section to build the library in those frameworks.

### AnyCloud

A set of pre-defined configuration files have been bundled with the wifi-mw-core library for FreeRTOS, lwIP, and mbed TLS. Review the configuration and make the required adjustments. See the "Quick Start" section in [README.md](https://github.com/cypresssemiconductorco/wifi-mw-core/blob/master/README.md).

1. Define the following COMPONENTS in the application's Makefile for the HTTP Server Library. For additional information, see the "Quick Start" section in [README.md](https://github.com/cypresssemiconductorco/wifi-mw-core/blob/master/README.md).

  ```
    COMPONENTS=FREERTOS MBEDTLS LWIP SECURE_SOCKETS
  ```

2. To enable Bluetooth the BLE functionality, add `WICED_BLE` to the `COMPONENTS` section in the application's Makefile:

  ```
    COMPONENTS=WICED_BLE
  ```

3. Add the following compiler directives to the `DEFINES` section in the application's Makefile:
 
  ```
    DEFINES=HAVE_SNPRINTF ANYCLOUD
  ```

4. Tune the lwIP configuration parameters depending on the required Wi-Fi throughput and use case.

### Mbed OS
1. Add the following compiler directive to the `macros` section of the *mbed_app.json* file in the application:

   ```
   "macros": ["MBEDOS"]
   ```

2. Add `MBED` to the *components_add* in the code example's JSON file. The JSON file entry would look like as follows:
     ```
       "target.components_add": ["MBED"]
     ```

3. Tune the lwIP configuration parameters depending on the required Wi-Fi throughput and use case.

## Supported iPerf commands
The following commands are supported by the iPerf library integrated into command console. On the console, 'iperf --help' will present the list of supported commands

   Note: -w command is not supported through command line option and it can be only configured by changing network stack configuration.
   > iperf --help
    Usage: iperf [-s|-c host] [options]
       iperf [-h|--help] [-v|--version]

    Client/Server:
     -i, --interval  #         seconds between periodic bandwidth reports
     -l, --len       #[kmKM]   length of buffer in bytes to read or write (Defaults: TCP=128K, v4 UDP=1470, v6 UDP=1450)
     -p, --port      #         server port to listen on/connect to
     -u, --udp                 use UDP rather than TCP

     TCP Window size can be configured through network stack configuration
      For MBED OS:
      1. Modify the tcp window size with pbuf size in mbed_app.json as below
         "lwip.tcp-wnd": (TCP_MSS * 20)
         "lwip.pbuf-pool-size":20
      For AnyCloud:
      1. Add below line in libs/wifi-mw-core/configs/lwipopts.h to modify tcp window size
         #define TCP_WND (TCP_MSS * 20)

    Server specific:
     -s, --server             run in server mode
     -t, --time      #        time in seconds to listen for new connections as well as to receive traffic (default not set)

    Client specific:
     -c, --client    <host>   run in client mode, connecting to <host>
     -n, --num       #[kmgKMG]    number of bytes to transmit (instead of -t)
     -t, --time      #        time in seconds to transmit for (default 10 secs)
    Miscellaneous:
     -x, --reportexclude [CDMSV]   exclude C(connection) D(data) M(multicast) S(settings) V(server) reports
     -y, --reportstyle C      report as a Comma-Separated Values
     -h, --help               print this message and quit
     -v, --version            print version information and quit

*Note: On Mbed OS, iPerf command line option '-l' has a max limit of 60KB (per packet) for data transfer*

## Operation

### Run iPerf client and server against a remote peer device

Enter the following commands after the device boots up and connects to the configured AP.

1. Start iPerf as a TCP server:
   ```
   >iperf -s
   ```
   (On the peer device, start iPerf as a TCP client and send TCP IPv4 data from the client.)

2. Start iPerf as a TCP client:

   ```
	>iperf -c <server_ip_addr>
   ```
   (On the peer device, start iPerf as server. It must be on the same subnet as the client.)

3. Start iPerf as a UDP server:
   ```
   >iperf -s -u
   ```

4. Start iPerf as a UDP client:
   ```
   >iperf -c <server_ip_addr> -u
   ```

### Run BLE commands against a remote peer device
As an example, use the LE CoC application on a CYW20719B2Q40EVB-01 kit as the remote device.

The following BLE commands are supported:

1. Turn on Bluetooth:
   ```
   >bt_on
   ```

2. Turn off Bluetooth:
   ```
   >bt_off
   ```

3. Start BLE advertisement:
   ```
   >ble_start_adv
   ```

4. Stop BLE advertisement:
   ```
   >ble_stop_adv
   ```

5. Start BLE scan:
   ```
   >ble_start_scan
   ```

6. Stop BLE scan:
   ```
   >ble_stop_scan
   ``` 

7. Initialize LE Connection-oriented Channel (CoC):
   ```
   >ble_coc_init
   ```

   (LE CoC will be initialized with an MTU size of 100 and local Protocol Service Multiplexer (PSM) 0x19.)

8. Start LE CoC advertisement:
   ```
   >ble_coc_adv
   ```

9. Scan and connect to a remote BLE device running LE COC app on the CYW20719B2Q40EVB-01 device: 
   ```
   >ble_coc_scan_connect
   ```

10. Disconnect an LE CoC connection:
    ```
    >ble_coc_disconnect
    ```

11. Send LE CoC data:
    ```
    >ble_coc_send_start
    ```

12. Stop sending LE CoC data:
    ```
    >ble_coc_send_stop
    ```

13. Get the throughput: 
    ```
    >ble_get_throughput
    ```

### Add Console Commands in an Application

1. Initialize the command console framework: 
   ```
   cy_command_console_init
   ```

   You should define the commands table and the handler along with the primary library. For example, the WHD library console commands may be placed in `WHD_utils` and located along with the WHD library. The test application will invoke the relevant function in `WHD_utils` for adding the commands.

3. Define the commands as shown in the following example:

   ```
   >#define MY_COMMANDS \
   { "mycmd1", my_command1, 0, NULL, NULL, (char *)"", (char *)"Run mycmd1 --help for usage."}, \
   { "mycmd2", my_command2, 0, NULL, NULL, (char *)"", (char *)"Run mycmd2 --help for usage."}, \
   ```

4. Define the command table:
   ```
   >const cy_command_console_cmd_t my_command_table[] =
	{
		MY_COMMANDS
		CMD_TABLE_END
	};
   ```

5. Implement command handlers when the commands are executed on the console. 
   
   In this example, implement `my_command1` and `my_command2` function handlers.

6. Add the command table to the console:
   ```
   >cy_command_console_add_table( my_command_table )
   ```

7. De-register the command table:
   ```
   >cy_command_console_delete_table( my_command_table )
   ```

## Additional Information
- [Command console RELEASE.md](./RELEASE.md)

- [Command Console API Reference Guide](https://cypresssemiconductorco.github.io/command-console/api_reference_manual/html/index.html)

- [ModusToolbox® Software Environment, Quick Start Guide, Documentation, and Videos](https://www.cypress.com/products/modustoolbox-software-environment)

- [Command Console Version](./version.txt)
