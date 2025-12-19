/*
 * (c) 2025, Infineon Technologies AG, or an affiliate of Infineon
 * Technologies AG. All rights reserved.
 * This software, associated documentation and materials ("Software") is
 * owned by Infineon Technologies AG or one of its affiliates ("Infineon")
 * and is protected by and subject to worldwide patent protection, worldwide
 * copyright laws, and international treaty provisions. Therefore, you may use
 * this Software only as provided in the license agreement accompanying the
 * software package from which you obtained this Software. If no license
 * agreement applies, then any use, reproduction, modification, translation, or
 * compilation of this Software is prohibited without the express written
 * permission of Infineon.
 *
 * Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
 * IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
 * THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
 * SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
 * Infineon reserves the right to make changes to the Software without notice.
 * You are responsible for properly designing, programming, and testing the
 * functionality and safety of your intended application of the Software, as
 * well as complying with any legal requirements related to its use. Infineon
 * does not guarantee that the Software will be free from intrusion, data theft
 * or loss, or other breaches ("Security Breaches"), and Infineon shall have
 * no liability arising out of any Security Breaches. Unless otherwise
 * explicitly approved by Infineon, the Software may not be used in any
 * application where a failure of the Product or any consequences of the use
 * thereof can reasonably be expected to result in personal injury.
 */

/** @file wifi_utility.c
 *  @brief This file contains the definition of Wi-Fi commands and implementation of the
 *  command handlers.
 */
#ifndef DISABLE_COMMAND_CONSOLE_WIFI
#include "command_console.h"
#include "wifi_utility.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cy_wcm.h"
#include "whd.h"
#include "whd_wifi_api.h"
#include "whd_wlioctl.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/
#define INITIALISER_IPV4_ADDRESS1(addr_var, addr_val) addr_var = {CY_WCM_IP_VER_V4, {.v4 = (uint32_t)(addr_val)}}
#define MAKE_IPV4_ADDRESS1(a, b, c, d)      ((((uint32_t) d) << 24) | (((uint32_t) c) << 16) | (((uint32_t) b) << 8) |((uint32_t) a))

#define CMD_CONSOLE_SSID_MAX_LENGTH         33 /* 32 is what 802.11 defines as longest possible name; +1 for the \0 */
#define CMD_CONSOLE_PASS_MAX_LENGTH         64

#ifndef CMD_CONSOLE_MAX_WIFI_RETRY_COUNT
#define CMD_CONSOLE_MAX_WIFI_RETRY_COUNT    5
#endif

#define CMD_CONSOLE_PING_TIMEOUT_DEFAULT    10000
#define CMD_CONSOLE_IPV4_ADDR_SIZE          4

#if defined(__ICCARM__)
#define WIFI_WEAK_FUNC            __WEAK
#elif defined(__GNUC__) || defined(__clang__) || defined(__CC_ARM)
#define WIFI_WEAK_FUNC            __attribute__((weak))
#else
#define WIFI_WEAK_FUNC           __attribute__((weak))
#endif

#define MAX_WHD_INTERFACE                   2

/******************************************************
 *               External Function Declarations
 ******************************************************/
extern char *strtok_r( char *, const char *, char ** );

/******************************************************
 *               Function Declarations
 ******************************************************/

/* Wi-Fi commands */
int join               (int argc, char* argv[], tlv_buffer_t** data);
int leave              (int argc, char* argv[], tlv_buffer_t** data);
int scan               (int argc, char* argv[], tlv_buffer_t** data);
int ping               (int argc, char* argv[], tlv_buffer_t** data);
int get_rssi           (int argc, char* argv[], tlv_buffer_t** data);
int start_ap           (int argc, char* argv[], tlv_buffer_t** data);
int stop_ap            (int argc, char* argv[], tlv_buffer_t** data);
int get_sta_ifconfig   (int argc, char* argv[], tlv_buffer_t** data);
int wifi_fw_ver        (int argc, char* argv[], tlv_buffer_t** data);
int wifi_status        (int argc, char* argv[], tlv_buffer_t** data);
#if defined(COMPONENT_PSE84) || defined(COMPONENT_55900)
int wifi_country_set   (int argc, char* argv[], tlv_buffer_t** data);
int wifi_country_read  (int argc, char* argv[], tlv_buffer_t** data);
int wifi_country_list  (int argc, char* argv[], tlv_buffer_t** data);
#endif

/******************************************************
 *                    Constants
 ******************************************************/

static const cy_command_console_cmd_t wifi_command_table[] =
{
    { (char*) "join",               join,             2, NULL, NULL, (char*) "<ssid> <open|owe|wpa_aes|wpa_tkip|wpa2|wpa2_aes|wpa2_aes_sha256|wpa2_tkip|wpa3|wpa3_wpa2> [password] [channel] [band<0=auto,1=5G,2=2.4G,3=6G>]"ESCAPE_SPACE_PROMPT, (char*) "Join an AP.(This command is deprecated and it will be removed in the future. Please use wifi_join command)"},
    { (char*) "leave",              leave,            0, NULL, NULL, (char*) "", (char*) "Leave the connected AP.(This command is deprecated and it will be removed in the future. Please use wifi_leave command)"},
    { (char*) "scan",               scan,             0, NULL, NULL, (char*) "", (char*) "Scan all the Wi-Fi AP in the vicinity.(This command is deprecated and it will be removed in the future. Please use wifi_scan command)"},
    { (char*) "ping",               ping,             0, NULL, NULL, (char*) "<IP address> [timeout(ms)]", (char*) "ping to an IP address.(This command is deprecated and it will be removed in the future. Please use wifi_ping command)"},
    { (char*) "get_rssi",           get_rssi,         0, NULL, NULL, (char*) "", (char*) "Get the received signal strength of the AP (client mode only).(This command is deprecated and it will be removed in the future. Please use wifi_get_rssi command)"},
    { (char*) "wifi_join",          join,             2, NULL, NULL, (char*) "<ssid> <open|owe|wpa_aes|wpa_tkip|wpa2|wpa2_aes|wpa2_aes_sha256|wpa2_tkip|wpa3|wpa3_wpa2> [password] [channel] [band<0=auto,1=5G,2=2.4G,3=6G>]"ESCAPE_SPACE_PROMPT, (char*) "Join an AP."},
    { (char*) "wifi_leave",         leave,            0, NULL, NULL, (char*) "", (char*) "Leave the connected AP."},
    { (char*) "wifi_scan",          scan,             0, NULL, NULL, (char*) "", (char*) "Scan all the Wi-FI AP in the vicinity."},
    { (char*) "wifi_ping",          ping,             0, NULL, NULL, (char*) "<IP address> [timeout(ms)]", (char*) "ping to an IP address"},
    { (char*) "wifi_get_rssi",      get_rssi,         0, NULL, NULL, (char*) "", (char*) "Get the received signal strength of the AP (client mode only)."},
    { (char*) "start_ap",           start_ap,         5, NULL, NULL,
      (char*) "<ssid> <open|wpa2|wpa2_aes|wpa3|wpa3_wpa2|wep|wep_shared> <key> <channel> [band<0=auto,1=5G,2=2.4G,3=6G>]\n-->When any parameter has spaces, use quotes.\n\tE.g. start_ap \"my ssid\" wpa2 \"my wpa2 key \" 11 2.  IP and subnet mask are 192.168.0.1 and 255.255.255.0. Note: <key> is a mandatory parameter, hence for 'open' security type, a dummy value needs to be provided. E.g. start_ap my_ssid open x 11 2 ",
      (char*) "Start AP mode."},
    { (char*) "stop_ap",            stop_ap,          0, NULL, NULL, (char*) "", (char*) "Stop AP mode."},
    { (char*) "get_sta_ifconfig",   get_sta_ifconfig, 0, NULL, NULL, (char*) "", (char*) "Get IP & MAC address of STA."},
    { (char*) "wifi_fw_ver",   wifi_fw_ver, 0, NULL, NULL, (char*) "", (char*) "Get WiFi FW version."},
    { (char*) "wifi_status",  wifi_status, 0, NULL, NULL, (char*) "", (char*) "Get the status of the existing connection"},
#if defined(COMPONENT_PSE84) || defined(COMPONENT_55900)
    { (char*) "wifi_country_set",   wifi_country_set, 1, NULL, NULL, (char*) "<country-code> [rev]", (char*) "Set WiFi Country Code."},
    { (char*) "wifi_country_read",   wifi_country_read, 0, NULL, NULL, (char*) "", (char*) "Get Country code."},
    { (char*) "wifi_country_list",   wifi_country_list, 0, NULL, NULL, (char*) "[band<a=5GHz,b=2.4GHz,s=6GHz>]", (char*) "List available country codes."},
#endif
    CMD_TABLE_END
};

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
static void scan_result_cb(cy_wcm_scan_result_t *result_ptr, void *user_data, cy_wcm_scan_status_t status);

static int convert_to_wcm_connect_params(int argc, char* argv[], cy_wcm_connect_params_t* connect_params);
static int wifi_utils_str_to_ip(char* ip_str, cy_wcm_ip_address_t* ip_addr);
static int wifi_utils_str_to_band(char* channel_str, char* band_str, cy_wcm_wifi_band_t* band);
static cy_wcm_security_t wifi_utils_str_to_authtype(char* auth_str);
static const char* wifi_utils_authtype_to_str(cy_wcm_security_t sec);

static void print_ip4(uint32_t ip);
static cy_rslt_t start_ap_common(const char *ssid, const char *key, uint8_t channel, cy_wcm_security_t security_type, cy_wcm_custom_ie_info_t *custom_ie, cy_wcm_wifi_band_t band);

/******************************************************
 *               Variables Definitions
 ******************************************************/
static const cy_wcm_ip_setting_t ap_ip_settings =
{
    INITIALISER_IPV4_ADDRESS1(.ip_address, MAKE_IPV4_ADDRESS1(192, 168, 0, 2)),
    INITIALISER_IPV4_ADDRESS1(.netmask, MAKE_IPV4_ADDRESS1(255, 255, 255, 0)),
    INITIALISER_IPV4_ADDRESS1(.gateway, MAKE_IPV4_ADDRESS1(192, 168, 0, 2)),
};

extern whd_interface_t whd_ifs[MAX_WHD_INTERFACE];

/******************************************************
 *               Function Definitions
 ******************************************************/

WIFI_WEAK_FUNC cy_rslt_t wifi_utility_init(void)
{
    cy_rslt_t res;
    cy_wcm_config_t wcm_config;

    cy_command_console_add_table(wifi_command_table);

    wcm_config.interface = CY_WCM_INTERFACE_TYPE_AP_STA;
    res = cy_wcm_init(&wcm_config);
    if(res != CY_RSLT_SUCCESS)
    {
        WIFI_ERROR(("Failed to initialize Wi-Fi module. Res:0x%X\n", (unsigned int)res));
        return res;
    }
    WIFI_INFO(("Wi-Fi module initialized...\n"));

    return res;
}

int join(int argc, char* argv[], tlv_buffer_t** data)
{
    cy_rslt_t res;
    int result;
    cy_wcm_connect_params_t connect_params;
    cy_wcm_ip_address_t ip_addr;
    int retry_count = 0;
    bool wifi_connected = false;

    if(cy_wcm_is_connected_to_ap())
    {
        WIFI_INFO(("Already connected. Leave and join again.\n"));
        return 0;
    }

    result = convert_to_wcm_connect_params(argc, argv, &connect_params);
    if(result != 0)
    {
        return result;
    }

    WIFI_INFO(("Connecting to AP please wait...\n"));
    for(retry_count = 0; retry_count < CMD_CONSOLE_MAX_WIFI_RETRY_COUNT; retry_count++)
    {
        memset(&ip_addr, 0x00, sizeof(cy_wcm_ip_address_t));
        res = cy_wcm_connect_ap(&connect_params, &ip_addr);
        if(res != CY_RSLT_SUCCESS)
        {
            WIFI_INFO(("Failed to join AP [0x%X]. Retrying...\n", (unsigned int)res));
            continue;
        }
        wifi_connected = true;
        WIFI_INFO(("Successfully connected to %s\n", argv[1]));
        print_ip4(ip_addr.ip.v4);
        break;
    }

    if(!wifi_connected)
    {
        WIFI_ERROR(("Exceeded maximum Wi-Fi connection attempts.\n"));
        return -1;
    }

    return 0;
}

int leave(int argc, char* argv[], tlv_buffer_t** data)
{
    cy_rslt_t res;

    if(!cy_wcm_is_connected_to_ap())
    {
        WIFI_INFO(("Failed to leave. Not connected to any AP.\n"));
        return 0;
    }

    res = cy_wcm_disconnect_ap();
    if(res != CY_RSLT_SUCCESS)
    {
        WIFI_ERROR(("Failed to disconnect from AP. Res:0x%X", (unsigned int)res));
        return -1;
    }

    WIFI_INFO(("Disconnected from AP.\n"));

    return 0;
}

int scan(int argc, char* argv[], tlv_buffer_t** data)
{
    cy_rslt_t res;

    WIFI_INFO(("#### Scan Results ####\n\n"));
    WIFI_INFO(("SSID                 Security Type  RSSI(dBm)  Channel BSSID\n"));

    res = cy_wcm_start_scan(scan_result_cb, NULL, NULL);
    if(res != CY_RSLT_SUCCESS && res != CY_RSLT_WCM_SCAN_IN_PROGRESS)
    {
        WIFI_ERROR(("Error while scanning. Res: 0x%X", (unsigned int)res));
        return -1;
    }

    return 0;
}

int ping(int argc, char* argv[], tlv_buffer_t** data)
{
    cy_rslt_t res;
    cy_wcm_ip_address_t ip_addr;
    uint32_t timeout_ms = CMD_CONSOLE_PING_TIMEOUT_DEFAULT;
    uint32_t elapsed_ms;

    if(!cy_wcm_is_connected_to_ap())
    {
        WIFI_INFO(("Not connected to AP.\n"));
        return 0;
    }

    if(argc < 2)
    {
        WIFI_ERROR(("Wrong number of arguments. Command format: ping <ipaddr> [timeout(ms)]\n"));
        return -1;
    }

    if(wifi_utils_str_to_ip(argv[1], &ip_addr) == -1)
    {
        return -1;
    }

    if(argc == 3)
    {
        timeout_ms = atoi(argv[2]);
    }

    res = cy_wcm_ping(CY_WCM_INTERFACE_TYPE_STA, &ip_addr, timeout_ms, &elapsed_ms);
    if(res != CY_RSLT_SUCCESS)
    {
        WIFI_ERROR(("Ping failed. Error: 0x%X\n", (unsigned int)res));
        return -1;
    }

    WIFI_INFO(("Ping successful. Elapsed time: %u (ms)\n", (unsigned int)elapsed_ms));

    return 0;
}

int get_rssi(int argc, char* argv[], tlv_buffer_t** data)
{
    cy_rslt_t res;
    cy_wcm_associated_ap_info_t ap_info;

    res = cy_wcm_get_associated_ap_info(&ap_info);
    if(res != CY_RSLT_SUCCESS)
    {
        WIFI_ERROR(("Failed to get RSSI. Res:0x%X\n", (unsigned int)res));
        return -1;
    }
    WIFI_INFO(("RSSI: %d dBm\n", ap_info.signal_strength));

    return 0;
}

int start_ap(int argc, char* argv[], tlv_buffer_t** data)
{
    char *ssid = argv[1];
    cy_wcm_security_t auth_type = wifi_utils_str_to_authtype(argv[2]);
    char *security_key = argv[3];
    uint8_t channel = atoi(argv[4]);
    cy_wcm_wifi_band_t band = (cy_wcm_wifi_band_t)(atoi(argv[5]));
    cy_rslt_t res;

    res = start_ap_common(ssid, security_key, channel, auth_type, NULL, band);
    if(res != CY_RSLT_SUCCESS)
    {
        WIFI_ERROR(("Failed to start_ap_common. Res:0x%x\n", (unsigned int)res));
        return -1;
    }
    WIFI_INFO(("start_ap successful!\n"));
    return 0;
}

int stop_ap(int argc, char* argv[], tlv_buffer_t** data)
{
    cy_rslt_t res;
    WIFI_INFO(("stop_ap start\n"));
    res = cy_wcm_stop_ap();
    if(res != CY_RSLT_SUCCESS)
    {
        WIFI_ERROR(("Failed to stop_ap. Res:0x%x", (unsigned int)res));
        return -1;
    }

    WIFI_INFO(("stop_ap successful!\n"));
    return 0;
}

int get_sta_ifconfig(int argc, char* argv[], tlv_buffer_t** data)
{
    cy_rslt_t res, result;
    cy_wcm_ip_address_t ip_addr;
    uint8_t mac[6] = {0};

    res = cy_wcm_get_ip_addr(CY_WCM_INTERFACE_TYPE_STA, &ip_addr);
    if (res != CY_RSLT_SUCCESS)
    {
         WIFI_INFO(("cy_wcm_get_ip_addr failed...! with error: [0x%X]\n", (unsigned int)res));
    }
    else
    {
         print_ip4(ip_addr.ip.v4);
    }
    result = cy_wcm_get_mac_addr(CY_WCM_INTERFACE_TYPE_STA, &mac);
    if (result != CY_RSLT_SUCCESS)
    {
         WIFI_INFO(("STA MAC Address failed result:0x%X\n", (unsigned int)result));
    }
    else
    {
         WIFI_INFO(("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]));
    }
    return 0;
}

static cy_rslt_t start_ap_common(const char *ssid, const char *key, uint8_t channel, cy_wcm_security_t security_type, cy_wcm_custom_ie_info_t *custom_ie, cy_wcm_wifi_band_t band)
{
    cy_rslt_t result;
    cy_wcm_ap_config_t ap_params;
    cy_wcm_ip_address_t ipv4_addr;

    memset(&ap_params, 0, sizeof(cy_wcm_ap_config_t));
    memcpy(ap_params.ap_credentials.SSID, ssid, strlen(ssid) + 1);
    memcpy(ap_params.ap_credentials.password, key, strlen(key) + 1);
    ap_params.ap_credentials.security = security_type;
    ap_params.channel = channel;
    ap_params.band = band;

    ap_params.ip_settings.ip_address = ap_ip_settings.ip_address;
    ap_params.ip_settings.gateway = ap_ip_settings.gateway;
    ap_params.ip_settings.netmask = ap_ip_settings.netmask;
    if (custom_ie)
    {
        WIFI_INFO(("Setting custom IE\n"));
        ap_params.ie_info = custom_ie;
        WIFI_INFO(("custom_ie.subtype = %d \n", ap_params.ie_info->subtype));
        WIFI_INFO(("custom_ie.length = %d \n", ap_params.ie_info->length));
        WIFI_INFO(("custom_ie.ie_packet_mask  = %d \n", ap_params.ie_info->ie_packet_mask));
        WIFI_INFO(("custom_ie->data  = %s \n", (char *)ap_params.ie_info->data));
    }
    result = cy_wcm_start_ap(&ap_params);
    if (result != CY_RSLT_SUCCESS)
    {
        WIFI_INFO(("WCM AP start failed with error: [0x%X]\n", (unsigned int)result));
        return result;
    }
    WIFI_INFO(("WCM AP started\n"));
    result = cy_wcm_get_ip_addr(CY_WCM_INTERFACE_TYPE_AP, &ipv4_addr);
    if (result != CY_RSLT_SUCCESS)
    {
        WIFI_INFO(("cy_wcm_get_ip_addr failed...! with error: [0x%X]\n", (unsigned int)result));
        return result;
    }
    print_ip4(ap_params.ip_settings.ip_address.ip.v4);
    return CY_RSLT_SUCCESS;
}

void scan_result_cb(cy_wcm_scan_result_t *result_ptr, void *user_data, cy_wcm_scan_status_t status)
{
    if(status == CY_WCM_SCAN_INCOMPLETE)
    {
        WIFI_INFO(("%-20s %-14s %-10d %-7d %02X:%02X:%02X:%02X:%02X:%02X",
        result_ptr->SSID,
        wifi_utils_authtype_to_str(result_ptr->security),
        result_ptr->signal_strength,
        result_ptr->channel,
        result_ptr->BSSID[0],
        result_ptr->BSSID[1],
        result_ptr->BSSID[2],
        result_ptr->BSSID[3],
        result_ptr->BSSID[4],
        result_ptr->BSSID[5]));

        if(user_data != NULL)
        {
            WIFI_DEBUG((" message = %s ", (char*)user_data));
        }

#if IE_DATA_PRINTS
        int i = 0;
        WIFI_DEBUG((" ie len = %lu data: ", (unsigned long)result_ptr->ie_len));
        for(i=0; i<result_ptr->ie_len; i++)
        {
            WIFI_DEBUG(("%02x ", result_ptr->ie_ptr[i]));
        }
#endif
        WIFI_INFO(("\n"));
    }
    else if(status == CY_WCM_SCAN_COMPLETE)
    {
        WIFI_INFO(("#### Scan Results END ####\n\n"));
    }
}

int convert_to_wcm_connect_params(int argc, char* argv[], cy_wcm_connect_params_t* connect_params)
{
    memset(connect_params, 0, sizeof(cy_wcm_connect_params_t));

    // command format: join <ssid> <sec type> [key] [channel/band]
    if(argc < 3)
    {
        WIFI_ERROR(("Insufficient number of arguments. Command format: join <ssid> <sec type> [key] [channel/band]\n"));
        return -1;
    }

    if(strlen(argv[1]) >= CMD_CONSOLE_SSID_MAX_LENGTH)
    {
        WIFI_ERROR(("SSID length should be lesser than [%d] bytes.\n", CMD_CONSOLE_SSID_MAX_LENGTH));
        return -1;
    }

    // ssid
    memcpy(connect_params->ap_credentials.SSID, argv[1], strlen(argv[1]) + 1);

    // security type
    connect_params->ap_credentials.security = wifi_utils_str_to_authtype(argv[2]);
    if(connect_params->ap_credentials.security == CY_WCM_SECURITY_UNKNOWN)
    {
        WIFI_ERROR(("Cannot connect due to unsupported auth type.\n"));
        return -1;
    }

    // passkey
    if((connect_params->ap_credentials.security != CY_WCM_SECURITY_OPEN)
#ifdef COMPONENT_WIFI6
        && (connect_params->ap_credentials.security != CY_WCM_SECURITY_OWE)
#endif
    )
    {
        if(argc < 4)
        {
            WIFI_ERROR(("Insufficient number of arguments.\n"));
            return -1;
        }
        if(strlen(argv[3]) >= CMD_CONSOLE_PASS_MAX_LENGTH)
        {
            WIFI_ERROR(("Password length should be lesser than [%d] bytes.\n", CMD_CONSOLE_PASS_MAX_LENGTH));
            return -1;
        }
        memcpy(connect_params->ap_credentials.password, argv[3], strlen(argv[3]) + 1);
    }

    // channel
    if(argc == 5 && wifi_utils_str_to_band(argv[4], NULL, &connect_params->band) == -1)
    {
        return -1;
    }

    // channel/band
    if(argc == 6 && wifi_utils_str_to_band(argv[4], argv[5], &connect_params->band) == -1)
    {
        return -1;
    }
    return 0;
}

int wifi_utils_str_to_ip(char* ip_str, cy_wcm_ip_address_t* ip_addr)
{
    int bytes[CMD_CONSOLE_IPV4_ADDR_SIZE];
    char* p = NULL;
    char* rest = NULL;
    int i = 0;
    char* delimiter = ".";

    if((ip_str == NULL)||(ip_addr == NULL))
    {
        WIFI_ERROR(("Invalid parameter.\n"));
        return -1;
    }
    memset(&bytes, 0x00, sizeof(bytes));
    rest = ip_str;
    p = strtok_r (rest, delimiter, &rest);
    if(p == NULL)
    {
        WIFI_ERROR(("Invalid IP Addr.\n"));
        return -1;
    }
    bytes[i] = atoi(p);
    WIFI_DEBUG(("bytes[%d]:%d\n", i, bytes[i]));
    while(p != NULL)
    {
        ++i;
        p = strtok_r (rest, delimiter, &rest);
        if(i < CMD_CONSOLE_IPV4_ADDR_SIZE)
        {
            if(p == NULL)
            {
                WIFI_ERROR(("Invalid IP Addr.\n"));
                return -1;
            }
            bytes[i] = atoi(p);
            WIFI_DEBUG(("bytes[%d]:%d\n", i, bytes[i]));
        }
    }

    if(i != (CMD_CONSOLE_IPV4_ADDR_SIZE))
    {
        WIFI_ERROR(("Invalid IP Addr.\n"));
        return -1;
    }

    ip_addr->version = CY_WCM_IP_VER_V4;
    ip_addr->ip.v4 = (uint32_t)MAKE_IPV4_ADDRESS1(bytes[0], bytes[1], bytes[2], bytes[3]);

    return 0;
}

int wifi_utils_str_to_band(char* channel_str, char* band_str, cy_wcm_wifi_band_t* band)
{
    int channel = atoi(channel_str);
    int band_arg = CY_WCM_WIFI_BAND_ANY;

    if (band_str != NULL)
    {
        band_arg = atoi(band_str);
    }

    if((band_arg == CY_WCM_WIFI_BAND_ANY || band_arg == CY_WCM_WIFI_BAND_2_4GHZ) &&
        (channel >= 1 && channel <= 14))
    {
        *band = CY_WCM_WIFI_BAND_2_4GHZ;
    }
    else if((band_arg == CY_WCM_WIFI_BAND_ANY || band_arg == CY_WCM_WIFI_BAND_5GHZ) &&
        (channel >= 32 && channel <= 165))
    {
        *band = CY_WCM_WIFI_BAND_5GHZ;
    }
    else if((band_arg == CY_WCM_WIFI_BAND_ANY || band_arg == CY_WCM_WIFI_BAND_6GHZ) &&
        (channel >= 1 && channel <= 233))
    {
        *band = CY_WCM_WIFI_BAND_6GHZ;
    }
    else
    {
        WIFI_ERROR(("Invalid channel(%d) band(%d). Valid values: 1 to 14 - 2.4GHz, 32 to 165 - 5GHz, 1 to 233 - 6GHz\n",
            channel, band_arg));
        return -1;
    }

    return 0;
}

cy_wcm_security_t wifi_utils_str_to_authtype(char* auth_str)
{
    if (strcmp(auth_str, "open") == 0)
    {
        return CY_WCM_SECURITY_OPEN;
    }
    else if(strcmp(auth_str, "wpa2_tkip") == 0)
    {
        return CY_WCM_SECURITY_WPA2_TKIP_PSK;
    }
    else if(strcmp(auth_str, "wpa2_aes") == 0)
    {
        return CY_WCM_SECURITY_WPA2_AES_PSK;
    }
    else if(strcmp(auth_str, "wpa2_aes_sha256") == 0)
    {
        return CY_WCM_SECURITY_WPA2_AES_PSK_SHA256;
    }
    else if(strcmp(auth_str, "wpa2") == 0)
    {
        return CY_WCM_SECURITY_WPA2_MIXED_PSK;
    }
    else if(strcmp(auth_str, "wpa_aes" ) == 0)
    {
        return CY_WCM_SECURITY_WPA_AES_PSK;
    }
    else if(strcmp(auth_str, "wpa2-ft") == 0)
    {
        return CY_WCM_SECURITY_WPA2_FBT_PSK;
    }
    else if (strcmp(auth_str, "wpa_tkip") == 0)
    {
        return CY_WCM_SECURITY_WPA_TKIP_PSK;
    }
    else if(strcmp(auth_str, "wpa3") == 0)
    {
        return CY_WCM_SECURITY_WPA3_SAE;
    }
    else if(strcmp(auth_str, "wpa3_wpa2") == 0)
    {
        return CY_WCM_SECURITY_WPA3_WPA2_PSK;
    }
    else if(strcmp(auth_str, "owe") == 0)
    {
#ifdef COMPONENT_WIFI6
        return CY_WCM_SECURITY_OWE;
#else
        WIFI_ERROR(("OWE is not supported for this platform\r\n"));
        return CY_WCM_SECURITY_UNKNOWN;
#endif
    }
    else
    {
        WIFI_ERROR(("Unsupported auth type: '%s'\r\n", auth_str));
        return CY_WCM_SECURITY_UNKNOWN;
    }
}

const char* wifi_utils_authtype_to_str(cy_wcm_security_t sec)
{
    switch (sec)
    {
        case CY_WCM_SECURITY_OPEN:
            return "open";
        case CY_WCM_SECURITY_WEP_PSK:
        case CY_WCM_SECURITY_WEP_SHARED:
            return "wep";
        case CY_WCM_SECURITY_WPA2_MIXED_PSK:
            return "wpa2";
        case CY_WCM_SECURITY_WPA_AES_PSK:
            return "wpa_aes";
        case CY_WCM_SECURITY_WPA2_AES_PSK_SHA256:
            return "wpa2_aes_sha256";
        case CY_WCM_SECURITY_WPA2_AES_PSK:
            return "wpa2_aes";
        case CY_WCM_SECURITY_WPA_TKIP_PSK:
            return "wpa_tkip";
        case CY_WCM_SECURITY_WPA2_TKIP_PSK:
            return "wpa2_tkip";
        case CY_WCM_SECURITY_WPA2_FBT_PSK:
            return "wpa2-ft";
        case CY_WCM_SECURITY_WPA3_SAE:
            return "wpa3";
        case CY_WCM_SECURITY_WPA3_WPA2_PSK:
            return "wpa3_wpa2";
        case CY_WCM_SECURITY_WPA_AES_ENT:
            return "wpa_aes_ent";
        case CY_WCM_SECURITY_WPA_MIXED_ENT:
            return "wpa_mixed_ent";
        case CY_WCM_SECURITY_WPA2_TKIP_ENT:
            return "wpa2_tkip_ent";
        case CY_WCM_SECURITY_WPA2_AES_ENT:
            return "wpa2_aes_ent";
        case CY_WCM_SECURITY_WPA2_MIXED_ENT:
            return "wpa2_mixed_ent";
        case CY_WCM_SECURITY_WPA2_FBT_ENT:
            return "wpa2_fbt_ent";
#if defined(COMPONENT_PSE84) || defined(COMPONENT_55900)
        case CY_WCM_SECURITY_WPA3_192BIT_ENT:
            return "wpa3_192bit_ent";
        case CY_WCM_SECURITY_WPA3_ENT:
            return "wpa3_aes_gcm_256_ent";
        case CY_WCM_SECURITY_WPA3_ENT_AES_CCMP:
            return "wpa3_aes_ccm_128_ent";
#endif
#ifdef COMPONENT_WIFI6
        case CY_WCM_SECURITY_OWE:
            return "owe";
#endif
        case CY_WCM_SECURITY_UNKNOWN:
            return "Unknown";

        default:
            return "Unsupported";
    }
}

static void print_ip4(uint32_t ip)
{
    unsigned char bytes[CMD_CONSOLE_IPV4_ADDR_SIZE];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;

    WIFI_INFO(("IP Address: %d.%d.%d.%d\n", bytes[0], bytes[1], bytes[2], bytes[3]));
}

int wifi_fw_ver(int argc, char* argv[], tlv_buffer_t** data)
{
    char version[200];

    if (whd_wifi_get_wifi_version(whd_ifs[0], version, sizeof(version)) == WHD_SUCCESS)
    {
        WIFI_INFO(("WLAN Firmware    : %s", version));
    }
    return 0;
}

int wifi_status(int argc, char* argv[], tlv_buffer_t** data)
{
    cy_rslt_t result;
    cy_wcm_associated_ap_info_t ap_info;

    result = cy_wcm_get_associated_ap_info(&ap_info);
    if (result != CY_RSLT_SUCCESS)
    {
        WIFI_ERROR(("Get WiFi Connection status failed\n"));
        return -1;
    }

    WIFI_INFO(("WiFi Status\n"));
    WIFI_INFO(("SSID: %s\n", ap_info.SSID));
    WIFI_INFO(("BSSID: %02X:%02X:%02X:%02X:%02X:%02X\n", ap_info.BSSID[0], ap_info.BSSID[1], ap_info.BSSID[2], ap_info.BSSID[3], ap_info.BSSID[4], ap_info.BSSID[5]));
    WIFI_INFO(("Channel: %d\n", ap_info.channel));
    WIFI_INFO(("Channel Width: %d\n", ap_info.channel_width));
    WIFI_INFO(("Security Type: %s\n", wifi_utils_authtype_to_str(ap_info.security)));
    WIFI_INFO(("RSSI: %d\n", ap_info.signal_strength));

    return 0;
}

#if defined(COMPONENT_PSE84) || defined(COMPONENT_55900)
int wifi_country_set(int argc, char* argv[], tlv_buffer_t** data)
{
    /* wifi_country_set US/0 */
    whd_result_t result;
    char country_abbrev[4];
    int32_t rev;
    whd_country_code_t country_code;

    /* Extract the country abbrev */
    strcpy(country_abbrev, argv[1]);

    if (argc > 2)
    {
        /* Extract Rev information */
        rev = atoi(argv[2]);
    }
    else
    {
        rev = 0;
    }

    /* Form country code */
    country_code = (whd_country_code_t)(MK_CNTRY(country_abbrev[0], country_abbrev[1], rev));
    result = whd_wifi_set_country_code(whd_ifs[0], country_code);

    if (result != WHD_SUCCESS)
    {
        WIFI_ERROR(("Setting country code failed, %x\n", (unsigned int)result));
    }
    return 0;
}


int wifi_country_read(int argc, char* argv[], tlv_buffer_t** data)
{
    whd_result_t result;
    char country[4] = { 0 };

    result = whd_wifi_get_country_code(whd_ifs[0], country);

    if (result == WHD_SUCCESS)
    {
        WIFI_INFO(("%s\n", country));
    }
    else
    {
        WIFI_ERROR(("Failed to retrieve country code: %u", (unsigned int)result));
    }
    return 0;
}

int wifi_country_list(int argc, char* argv[], tlv_buffer_t** data)
{
    whd_result_t result;
    uint32_t band = 0;
    uint32_t count = 0;
    char country_list[512];
    const char* abbrev;

    if (argc == 2)
    {
        /* Band information available from user */
        if (strcmp(argv[1], "a") == 0)
        {
            band = WLC_BAND_5G;
        }
        else if (strcmp(argv[1], "b") == 0)
        {
            band = WLC_BAND_2G;
        }
        else if (strcmp(argv[1], "s") == 0)
        {
            band = WLC_BAND_6G;
        }
        else
        {
            WIFI_ERROR(("Unsupported Band\r\n"));
            return -1;
        }
    }
    result = whd_wifi_get_country_list(whd_ifs[0], band, &count, country_list);

    if (result == WHD_SUCCESS)
    {
        WIFI_INFO(("Supported Country list\r\n");

        );
        for (int i = 0; i < count; i++) {
            abbrev = (char*)&country_list[i * WLC_CNTRY_BUF_SZ];

            WIFI_INFO(("%s\n", abbrev));
        }
    }
    else
    {
        WIFI_ERROR(("Country Retrieve failed\r\n"));
    }

    return 0;
}
#endif

#ifdef __cplusplus
}
#endif
#endif
