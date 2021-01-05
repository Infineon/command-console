# Command Console Library

## Introduction

This library provides a framework to add command console support to your application. Support for Wi-Fi, iPerf, and Bluetooth Low Energy (BLE) commands is bundled with this library.

[ModusToolbox® AnyCloud Wi-Fi Bluetooth Tester](https://github.com/cypresssemiconductorco/mtb-anycloud-wifi-bluetooth-tester), [Mbed OS Wi-Fi Bluetooth Tester](https://github.com/cypresssemiconductorco/mbed-os-wifi-bluetooth-tester), and [AFR Wi-Fi Bluetooth Tester](https://github.com/cypresssemiconductorco/afr-wifi-bluetooth-tester) applications are built over this library.

## Features

- Supports Wi-Fi commands to perform network operations such as scan and join.

- Integrates iPerf 2.0 commands. iPerf is a tool for active measurements of the maximum achievable bandwidth on IP networks.

- Supports TCP client/server and UDP client/server on IPv4. Supports a single instance of iPerf client and server. IPv6 support will be added in the future.

- Supports Bluetooth LE commands including Tx/Rx for measuring BLE throughput.

**Note:** While iPerf provides several other commands, only a subset of commands that are relevant to embedded connectivity use cases is enabled in this library. See the [Supported iPerf Commands](#supported-iperf-commands) section.

## Supported Platforms

### AnyCloud

- [PSoC® 6 WiFi-BT Prototyping Kit (CY8CPROTO-062-4343W)](https://www.cypress.com/documentation/development-kitsboards/psoc-6-wi-fi-bt-prototyping-kit-cy8cproto-062-4343w)

- [PSoC 62S2 Wi-Fi BT Pioneer Kit (CY8CKIT-062S2-43012)](https://www.cypress.com/documentation/development-kitsboards/psoc-62s2-wi-fi-bt-pioneer-kit-cy8ckit-062s2-43012)

### Mbed OS

- [PSoC 6 WiFi-BT Prototyping Kit (CY8CPROTO-062-4343W)](https://www.cypress.com/documentation/development-kitsboards/psoc-6-wi-fi-bt-prototyping-kit-cy8cproto-062-4343w)

- [PSoC 62S2 Wi-Fi BT Pioneer Kit (CY8CKIT-062S2-43012)](https://www.cypress.com/documentation/development-kitsboards/psoc-62s2-wi-fi-bt-pioneer-kit-cy8ckit-062s2-43012)

### Amazon FreeRTOS

- [PSoC 6 WiFi-BT Prototyping Kit (CY8CPROTO-062-4343W)](https://www.cypress.com/documentation/development-kitsboards/psoc-6-wi-fi-bt-prototyping-kit-cy8cproto-062-4343w)

- [PSoC 62S2 Wi-Fi BT Pioneer Kit (CY8CKIT-062S2-43012)](https://www.cypress.com/documentation/development-kitsboards/psoc-62s2-wi-fi-bt-pioneer-kit-cy8ckit-062s2-43012)

## Supported Frameworks

This library supports the following frameworks:

- **AnyCloud Framework:** AnyCloud is an Amazon FreeRTOS-based solution. The Command Console Library uses the [abstraction-rtos](https://github.com/cypresssemiconductorco/abstraction-rtos) library that provides the RTOS abstraction API, and uses the [secure-sockets](https://github.com/cypresssemiconductorco/secure-sockets) library for implementing socket functions. For the BLE functionality, this library uses [bluetooth-freertos](https://github.com/cypresssemiconductorco/bluetooth-freertos).

- **Mbed Framework:** The Mbed framework is an Mbed OS-based solution. The Command Console Library uses the [abstraction-rtos](https://github.com/cypresssemiconductorco/abstraction-rtos) library that provides the RTOS abstraction API, and uses the Mbed socket API for implementing socket functions. For the BLE functionality, it uses the Cordio BLE stack in Mbed OS.

- **Amazon FreeRTOS (AFR) Framework:** AFR is an Amazon FreeRTOS-based solution. The Command Console Library uses the [abstraction-rtos](https://github.com/cypresssemiconductorco/abstraction-rtos) library that provides the RTOS abstraction API, and uses the Amazon FreeRTOS Framework network abstraction and lwIP library for implementing socket functions. For the BLE functionality, this library uses [bluetooth-freertos](https://github.com/cypresssemiconductorco/bluetooth-freertos).

## Dependencies

This section provides the list of dependent libraries required for this middleware library to work on AnyCloud and Arm® Mbed OS IoT frameworks.

### AnyCloud

- [Wi-Fi Connection Manager](https://github.com/cypresssemiconductorco/wifi-connection-manager)

- [bluetooth-freertos](https://github.com/cypresssemiconductorco/bluetooth-freertos)

### Mbed OS

- [Arm Mbed OS 6.2.0 and above](https://os.mbed.com/mbed-os/releases)

- [Connectivity Utilities Library](https://github.com/cypresssemiconductorco/connectivity-utilities)

### Amazon FreeRTOS

- [Amazon FreeRTOS](https://github.com/cypresssemiconductorco/amazon-freertos)

- [bluetooth-freertos](https://github.com/cypresssemiconductorco/bluetooth-freertos)

## Quick Start

This library is supported on AnyCloud, Mbed OS, and Amazon FreeRTOS frameworks. See the following section to build the library in those frameworks.

### AnyCloud

A set of pre-defined configuration files have been bundled with the wifi-mw-core library for Amazon FreeRTOS, lwIP, and mbed TLS. Review the configuration and make the required adjustments. See the "Quick Start" section in [README.md](https://github.com/cypresssemiconductorco/wifi-mw-core/blob/master/README.md).

1. Define the following COMPONENTS in the application's Makefile for the HTTP Server Library. For additional information, see the "Quick Start" section in [README.md](https://github.com/cypresssemiconductorco/wifi-mw-core/blob/master/README.md).

   ```
   COMPONENTS=FREERTOS MBEDTLS LWIP SECURE_SOCKETS
   ```

2. To enable BLE functionality, add `WICED_BLE` to the `COMPONENTS` section in the application's Makefile:

   ```
   COMPONENTS=WICED_BLE
   ```

3. Add the following compiler directives to the `DEFINES` section in the application's Makefile:

   ```
   DEFINES=HAVE_SNPRINTF
   ```

4. Tune the lwIP configuration parameters depending on the required Wi-Fi throughput and use case.

### Mbed OS

1. Add `MBED` to the *components_add* section in the code example's JSON file. The JSON file entry would look like as follows:

   ```
   "target.components_add": ["MBED"]
   ```

2. The BLE Command Utility is supported on Mbed Cordio BLE stack and the WICED® BT Stack.

   - To use the Mbed Cordio stack, add `CORDIO` to the list of components as follows:

      ```
      "target.components_add" : ["MBED", "CORDIO"]
      ```

   - To use the WICED BT stack, do the following:

     - Add the following to the *target_overrides* section of the *mbed_app.json* file:

        ```
        "target.extra_labels_remove": ["CORDIO"],
        "target.features_remove": ["BLE"]
        ```
     - Add `WICED_BLE` in the list of components as follows:

       ```
       "target.components_add" : ["MBED", "WICED_BLE"]
       ```

3. Tune the lwIP configuration parameters depending on the required Wi-Fi throughput and use case.

### Amazon FreeRTOS

1. All configurations for the Command Console library on AFR are set by the values defined in the config files of the application.

   Config files for the afr-wifi-bluetooth-tester application, are in the *afr-wifi-bluetooth-tester/config_files/*.

## Supported iPerf Commands

The following commands are supported by the iPerf library integrated into the Command Console. On the console, type `iperf --help` to list the supported commands.

**Note:** The `-w` command is not supported through the command line option; it can be only configured by changing network stack configuration.

```
> iperf --help
  Usage: iperf [-s|-c host] [options]
         iperf [-h|--help] [-v|--version]

   Client/Server:
   -i, --interval  #         seconds between periodic bandwidth reports
   -l, --len       #[kmKM]   length of buffer in bytes to read or write (Defaults: TCP=128K, v4 UDP=1470, v6 UDP=1450)
   -p, --port      #         server port to listen on/connect to
   -u, --udp                 use UDP rather than TCP
   ```

   *Server-specific:*

   ```
   -s, --server             run in server mode
   -t, --time      #        time in seconds to listen for new connections as well as to receive traffic (default not set)
   ```

   *Client-specific:*

   ```
   -c, --client    <host>   run in client mode, connecting to <host>
   -n, --num       #[kmgKMG]    number of bytes to transmit (instead of -t)
   -t, --time      #        time in seconds to transmit for (default 10 secs)
   ```
   *Miscellaneous:*

   ```
   -x, --reportexclude [CDMSV]   exclude C(connection) D(data) M(multicast) S(settings) V(server) reports
   -y, --reportstyle C      report as a Comma-Separated Values
   -h, --help               print this message and quit
   -v, --version            print version information and quit
   ```
**Note:** On Mbed OS, the iPerf command line option `-l` has a max limit of 60 KB (per packet) for data transfer.

Use the network stack configuration to set the TCP window size:   
**Mbed OS:**

Update the following configurations in *mbed_app.json*:

1. For the TCP server, modify/add the following lines to change the TCP window size with the packet buffer (PBUF) pool size:
   ```
   "lwip.tcp-wnd": (TCP_MSS * 20)
   "lwip.pbuf-pool-size": 20
   ```
2. For the TCP client, modify the following line to change the TCP send buffer size:
   ```
   "lwip.tcp-snd-buf": (TCP_MSS * 20)
   "lwip.memp-num-tcp-seg": 127
   ```
**AnyCloud:**

Update the following configurations in the application's config file (For example, *mtb-anycloud-wifi-bluetooth-tester/lwipopts.h*):

1. For the TCP server, modify/add the following lines to change the TCP window size and PBUF pool size:

   ```
   #define TCP_WND (TCP_MSS * 20)
   #define PBUF_POOL_SIZE 21
   ```
2. For the TCP client, modify the following line to change the TCP send buffer size:

   ```
   #define TCP_SND_BUF (TCP_MSS *20)
   #define MEMP_NUM_TCP_SEG 127
   ```

**Amazon FreeRTOS:**

Update the following configurations in the application's config file (For example, *afr-wifi-bluetooth-tester/config_files/lwipopts.h*):

   1. For the TCP server, modify the following lines to change the TCP window size and PBUF pool size:

      ```
      #define TCP_WND (TCP_MSS * 20)
      #define PBUF_POOL_SIZE 20
      ```
   2. For the TCP client, modify the following line to change the TCP send buffer size:

      ```
      #define TCP_SND_BUF (TCP_MSS * 20)
      ```

## Operation

Use the following Wi-Fi commands to perform network operations. Enter the following commands after the device boots up and the Wi-Fi module is initialized and ready.

1. Scan for nearby networks:
   ```
   >scan
	```

2. Connect to a network AP:
   ```
   >join <ssid> <security type> [password] [channel]
	```

3. Disconnect from the network AP:
   ```
   >leave
	```

4. Get the signal strength / RSSI of the network:
   ```
   >get_rssi
	```

5. Ping another device on the network:
   ```
   >ping <ip address>
    ```
   **Note:** This command is not supported on Mbed.

### Run iPerf Client and Server Against a Remote Peer Device

Enter the following commands after the device boots up and connects to the configured AP.

1. Start iPerf as a TCP server:
   ```
   >iperf -s
   ```
   **Note:** On the peer device, start iPerf as a TCP client and send the TCP IPv4 data from the client.

2. Start iPerf as a TCP client:

   ```
	>iperf -c <server_ip_addr>
   ```
   **Note:** On the peer device, start iPerf as server. It must be on the same subnet as the client.

3. Start iPerf as a UDP server:
   ```
   >iperf -s -u
   ```

4. Start iPerf as a UDP client:
   ```
   >iperf -c <server_ip_addr> -u
   ```

### Run BLE Commands Against a Remote Peer Device

As an example, use the LE Connection-oriented Channel (CoC) application on a CYW20719B2Q40EVB-01 kit as the remote device.

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

   **Note:** LE CoC will be initialized with an MTU size of 100 and local Protocol Service Multiplexer (PSM) 0x19.

8. Start LE CoC advertisement:
   ```
   >ble_coc_adv
   ```

9. Scan and connect to a remote BLE device running the LE CoC app on the CYW20719B2Q40EVB-01 device:
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

14. Get the device address:
    ```
    >bt_get_device_address
    ```
   **Note:** This command is supported only on the WICED BLE stack and not on the Mbed Cordio stack.

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
