/*
 * Copyright 2020, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software"), is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
 *
 */
#include <events/mbed_events.h>
#include <mbed.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "command_console.h"
#include "l2c_api.h"
#include "l2c_handler.h"
#include "gap/AdvertisingDataParser.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/
#define LE_COC_NAME_LEN           6
#define L2CAP_COC_PSM_USED        19
#define L2CAP_COC_MTU             100

/******************************************************
 *                    Constants
 ******************************************************/
// LE COC connection pending or not
static bool le_coc_connect_pending = true;

// LE COC connection ID
static uint16_t le_coc_conn_id = DM_CONN_ID_NONE;

// LE COC MTU
static int le_coc_mtu = L2CAP_COC_MTU;

// LE COC PSM
static int le_coc_psm = L2CAP_COC_PSM_USED;

// LE COC Register ID
static l2cCocRegId_t l2c_coc_regId = L2C_COC_REG_ID_NONE;

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/
static EventQueue event_queue(/* event count */ 10 * EVENTS_EVENT_SIZE);
/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
int handle_ble_on( int argc, char *argv[], tlv_buffer_t** data );
int handle_ble_off( int argc, char *argv[], tlv_buffer_t** data );
int handle_start_scan( int argc, char *argv[], tlv_buffer_t** data );
int handle_stop_scan( int argc, char *argv[], tlv_buffer_t** data );
int handle_start_adv( int argc, char *argv[], tlv_buffer_t** data );
int handle_stop_adv( int argc, char *argv[], tlv_buffer_t** data );
int handle_connect( int argc, char *argv[], tlv_buffer_t** data );
int handle_disconnect( int argc, char *argv[], tlv_buffer_t** data );
int handle_coc_init( int argc, char *argv[], tlv_buffer_t** data );
int handle_coc_adv( int argc, char *argv[], tlv_buffer_t** data );
int handle_coc_scan_connect( int argc, char *argv[], tlv_buffer_t** data );
int handle_coc_disconnect( int argc, char *argv[], tlv_buffer_t** data );
int handle_coc_send_data( int argc, char *argv[], tlv_buffer_t** data );
int handle_coc_stop_data( int argc, char *argv[], tlv_buffer_t** data );
int handle_coc_throughput( int argc, char *argv[], tlv_buffer_t** data );
/******************************************************
 *               Variables Definitions
 ******************************************************/
#define BT_COEX_COMMANDS \
    { "bt_on"  ,               handle_ble_on,           0, NULL, NULL, (char*) "",   (char*) "Turn on BLE \n\t [NOTE]  Command Not Supported, BLE is turned On as part of during application init"}, \
    { "bt_off"  ,              handle_ble_off,          0, NULL, NULL, (char*) "",   (char*) "Turn off BLE \n\t [NOTE] Command Not Supported"}, \
    { "ble_start_scan",        handle_start_scan,       0, NULL, NULL, (char*) "",   (char*) "start LE Scan"}, \
    { "ble_stop_scan",         handle_stop_scan,        0, NULL, NULL, (char*) "",   (char*) "stop LE Scan"}, \
    { "ble_start_adv",         handle_start_adv,        0, NULL, NULL, (char*) "",   (char*) "Start Advertisement"}, \
    { "ble_stop_adv",          handle_stop_adv,         0, NULL, NULL, (char*) "",   (char*) "Stop  Advertisement"}, \
    { "ble_coc_init",          handle_coc_init,         0, NULL, NULL, (char*) "",   (char*) "Initializes LE COC with PSM 19 and MTU 100"}, \
    { "ble_coc_adv",           handle_coc_adv,          0, NULL, NULL, (char*) "",   (char*) "le_coc_adv"}, \
    { "ble_coc_scan_connect",  handle_coc_scan_connect, 0, NULL, NULL, (char*) "",   (char*) "le_coc_scan and connect to a COC server"}, \
    { "ble_coc_disconnect",    handle_coc_disconnect,   0, NULL, NULL, (char*) "",   (char*) "le coc disconnect"}, \
    { "ble_coc_send_start",    handle_coc_send_data,    0, NULL, NULL, (char*) "",   (char*) "le coc send data"}, \
    { "ble_coc_send_stop",     handle_coc_stop_data,    0, NULL, NULL, (char*) "",   (char*) "le coc stop data send"}, \
    { "ble_get_throughput",    handle_coc_throughput,   0, NULL, NULL, (char*) "",   (char*) "Get LE CoC Throughput"}, \

const cy_command_console_cmd_t bt_coex_command_table[] =
{
    BT_COEX_COMMANDS
    CMD_TABLE_END
};

// Default LE COC Registration param structure
l2cCocReg_t l2c_coc_reg = {
    .psm = L2CAP_COC_PSM_USED,
    .mps = L2CAP_COC_MTU,
    .mtu = L2CAP_COC_MTU,
    .credits = 4,
    .authoriz = 0,
    .secLevel = 0,
    .role = L2C_COC_ROLE_ACCEPTOR
};

// Data Tx state
static bool le_coc_send_data = false;

// Total No of bytes sent
static uint32_t total_sent_data = 0;
// Timer for Tx
mbed::Timer mbed_sent_data_timer;

// Total No of bytes received
static uint32_t total_received_data = 0;
// Timer for Rx
mbed::Timer mbed_received_data_timer;

// BLE instant class
class BLECoexTest : ble::Gap::EventHandler {
public:
    BLECoexTest(BLE &ble, events::EventQueue &event_queue) :
        _ble(ble),
        _event_queue(event_queue)
         {  }

    // Initialize BLE 
    void start() {
        _ble.gap().setEventHandler(this);
        _ble.init(this, &BLECoexTest::on_init_complete);
        _event_queue.dispatch_forever();
    }
    
    // Shutdown BLE 
    bool stop() {
        if (_ble.hasInitialized())
        {
            if (_ble.shutdown() == BLE_ERROR_NONE)
                return true;
        }
        return false;
    }

    // Start/stop scan
    bool start_scan(int start) {
        if (!_ble.hasInitialized())
            return false;
        if (start)
        {
            // start scanning
            if ( _ble.gap().startScan(ble::scan_duration_t(0),ble::duplicates_filter_t::ENABLE, ble::scan_period_t(0)) == BLE_ERROR_NONE )
                 return true;
        }
        else
        {
            //stop scanning
            if ( _ble.gap().stopScan() == BLE_ERROR_NONE )
                return true;
        }
        return false;
    }

    // Start/stop adv
    bool start_adv(int start) {
        if (!_ble.hasInitialized())
            return false;
        if (start)
        {

            ble_error_t error;

            ble::AdvertisingParameters adv_parameters(
                ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
                true
            );

            adv_parameters.setOwnAddressType(ble::own_address_type_t :: PUBLIC);

            error = _ble.gap().setAdvertisingParameters(
                ble::LEGACY_ADVERTISING_HANDLE,
                adv_parameters
            );

            // set advertising data
            ble::AdvertisingDataBuilder adv_data_builder(_adv_buffer);
            adv_data_builder.setFlags();
            adv_data_builder.setName("LE CoC");
            error = _ble.gap().setAdvertisingPayload(
                    ble::LEGACY_ADVERTISING_HANDLE,
                    adv_data_builder.getAdvertisingData()
                    );
            if (error) {
                printf("_ble.gap().setAdvertisingPayload() failed");
                return false;
            }

            // start Advertisement
            if ( _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE) == BLE_ERROR_NONE )
                 return true;
        }
        else
        {
            //stop Advertisement
            if ( _ble.gap().stopAdvertising(ble::LEGACY_ADVERTISING_HANDLE) == BLE_ERROR_NONE )
                return true;
        }
        return false;
    }

    bool connect()
    {
        printf("scanning and connecting to LE CoC server \n");
        start_scan(1);
        return true;
    }

    // Disconnect
    bool disconnect(int handle)
    {
        if ( _ble.gap().disconnect(handle, ble::local_disconnection_reason_t::USER_TERMINATION) == BLE_ERROR_NONE )
            return true;
        return false;
    }

    /** Callback triggered when the ble initialization process has finished */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *params) {
        if (params->error != BLE_ERROR_NONE) {
            printf("Ble initialization failed.");
            return;
        }
        printf("Bluetooth Initialization Completed \n");
    }
private:
    virtual void onAdvertisingReport(const ble::AdvertisingReportEvent &event)
    {
        ble::address_t addr(event.getPeerAddress());
        ble::AdvertisingDataParser adv_parser(event.getPayload());
        bool le_coc_device_found = false;
        char adv_name[LE_COC_NAME_LEN] = "";

        printf("onAdvertisingReport : rssi %d peer_addr %02x:%02x:%02x:%02x:%02x:%02x \n",event.getRssi(),
                addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);

        /* parse the advertising payload, looking for a discoverable device */
        while (adv_parser.hasNext())
        {
            ble::AdvertisingDataParser::element_t field = adv_parser.next();

            /* skip non discoverable device */
            if (field.type == ble::adv_data_type_t::SHORTENED_LOCAL_NAME)
            {
                if(field.value.size() == LE_COC_NAME_LEN)
                {
                    for(int i=0; i<LE_COC_NAME_LEN; i++)
                    {
                        adv_name[i] = field.value[i];
                    }
                }

                if (strncmp (adv_name,"LE CoC",LE_COC_NAME_LEN) == 0)
                {
                    printf("found LE COC Server \n");
                    le_coc_device_found = true;
                    break;
                }
            }
        }

        if(le_coc_device_found)
         {
             _ble.gap().stopScan();
             if ( _ble.gap().connect(event.getPeerAddressType(), event.getPeerAddress(), ble::ConnectionParameters()) == BLE_ERROR_NONE)
             {
                 printf("gap().connect is successfull \n");
             }
         }

    }
    virtual void onConnectionComplete(const ble::ConnectionCompleteEvent &event)
    {
        if (event.getStatus() == BLE_ERROR_NONE) {
            printf("Connected  \n");
            printf("Handle 0x%x Connection interval 0x%x connection latency 0x%x Supervision timeout 0x%x \n", event.getConnectionHandle(),
                            (unsigned int)event.getConnectionInterval().value(),
                            (unsigned int)event.getConnectionLatency().value(),
                            (unsigned int)event.getSupervisionTimeout().value());

            if(l2c_coc_reg.role == L2C_COC_ROLE_ACCEPTOR)
            {
                printf("le coc role is acceptor \n");
                return;
            }

            // Check is LE COC connection is pending
            if (le_coc_connect_pending && (le_coc_conn_id==L2C_COC_CID_NONE) )
            {
                // Assume we are getting dm_id 1
                int dm_id = 1;
                le_coc_conn_id = L2cCocConnectReq(dm_id, l2c_coc_regId, le_coc_psm);
                if (le_coc_conn_id == L2C_COC_CID_NONE)
                {
                    printf ("Failed to connect LE COC \n");
                }
                le_coc_connect_pending = false;
            }
        }
        else
        {
            printf("Fail to connect \n");
        }
    }
    virtual void onDisconnectionComplete(const ble::DisconnectionCompleteEvent &event)
    {
        printf("Disconnected Handle 0x%x Reason 0x%x \n",event.getConnectionHandle(), event.getReason().value());
    }
private:
    BLE &_ble;
    events::EventQueue &_event_queue;
    uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
};

BLECoexTest *test_coex;

/******************************************************
 *               Function Definitions
 ******************************************************/
// Convert ASCII decimal string to integer 
int ascii_dec_to_int(char *str)
{
    int len = strlen(str);
    int total = 0;
    for (int i=0;i<len;i++)
    {
        if ( str[i] >= '0' && str[i] <= '9')
        {
            total = ((total*10) + (str[i]-'0'));
        }
        else
        {
            return -1;
        }
    }
    return total;
}

/** Schedule processing of events from the BLE middleware in the event queue. */
void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}
int handle_ble_on( int argc, char *argv[], tlv_buffer_t** data )
{
    printf("command not supported - bluetooth is turned on during application init \n");
    return 0;
}

int handle_ble_off( int argc, char *argv[], tlv_buffer_t** data )
{
    test_coex->stop();
    return 0;
}

int handle_stop_scan( int argc, char *argv[], tlv_buffer_t** data )
{
    bool status;
    printf("Stop scan operation \n");
    status = test_coex->start_scan(0);
    printf("[handle_stop_scan] status %d \n",status);
    return 0;
}

int handle_start_scan( int argc, char *argv[], tlv_buffer_t** data )
{
    bool status;
        printf("Starting scan operation \n");
        status = test_coex->start_scan(1);
    printf("[handle_start_scan] status %d \n",status);
    return 0;
}

int handle_start_adv( int argc, char *argv[], tlv_buffer_t** data )
{
    bool status;
    printf("Starting advertisement \n");
        status = test_coex->start_adv(1);
    printf("[handle_start_adv] status %d \n",status);
    return 0;
}

int handle_stop_adv( int argc, char *argv[], tlv_buffer_t** data )
{
    bool status;
    printf("Stop advertisement \n");
        status = test_coex->start_adv(0);
    printf("[handle_start_adv] status %d \n",status);
    return 0;
}

int handle_connect( int argc, char *argv[], tlv_buffer_t** data )
{
    bool status;
    status = test_coex->connect();
    printf("[handle_connect] status %d \n",status);
    return 0;
}

int handle_disconnect( int argc, char *argv[], tlv_buffer_t** data )
{
    bool status;
    int handle_len = strlen(argv[1]);
    int handle_value = 0;
    if ( handle_len <= 0 || handle_len > 4 )
    {
        printf("[handle_disconnect] Invalid Handle value %s \n",argv[1]);
        return 1;
    }

    for (int i=0; i<handle_len; i++)
    {
        char input = argv[1][i];
        uint8_t curr_val = 0;
        if ( (input >= '0') && (input <= '9'))
        {
            curr_val = input-'0';
        }
        else if ( (input >= 'a') && (input <= 'f') )
        {
            curr_val = input-'a';
        }
        else if ( (input >= 'A') && (input <= 'F') )
        {
            curr_val = input-'A';
        }
        handle_value += ( curr_val<< ((handle_len-i-1)*4) );
    }
    status = test_coex->disconnect(handle_value);
    printf("[handle_disconnect] status %d \n",status);
    return 0;
}

void generate_send_coc_data(void)
{
    uint8_t data[1024];
    int     index = 0;
    char    ch = 'a';
    for (index = 0; index < l2c_coc_reg.mtu; index++)
    {
        data[index] = ch;
        if (ch == 'z')
        {
            ch = 'a';
        }
        else
        {
            ch++;
        }
    }
    //printf("Sending data of length %d to %d cid \n",l2c_coc_reg.mtu, le_coc_conn_id);
    total_sent_data += l2c_coc_reg.mtu;
    L2cCocDataReq(le_coc_conn_id, l2c_coc_reg.mtu, data);
}

// LE COC event callback
void l2c_coc_cb (l2cCocEvt_t *msg)
{
    //printf("[l2c_coc_cb] param %d status %d event %d \n",msg->hdr.param, msg->hdr.status, msg->hdr.event);
    switch(msg->hdr.event)
    {
        case L2C_COC_CONNECT_IND:
        {
            printf("[L2C_COC_CONNECT_IND] Connection Id : %d \n",msg->connectInd.cid);
            le_coc_conn_id = msg->connectInd.cid;
        }
        break;
        case L2C_COC_DISCONNECT_IND:
        {
            printf("[L2C_COC_DISCONNECT_IND] Disconnection Id : %d Reason %d \n",msg->disconnectInd.cid, msg->disconnectInd.result);
            le_coc_conn_id = DM_CONN_ID_NONE;
        }
        break;
        case L2C_COC_DATA_IND:
        {
            printf(".");
            // check if its first received packet, reset and start timer
            if (total_received_data == 0)
            {
                mbed_received_data_timer.reset();
                mbed_received_data_timer.start();
            }
            total_received_data+= msg->dataInd.dataLen;
            printf("total_received_data = %ld \n", (long)total_received_data);
        }
        break;
        case L2C_COC_DATA_CNF:
        {
            printf("+");
            if ( (msg->hdr.status == L2C_COC_DATA_SUCCESS) && le_coc_send_data == true)
            {
                // Send again if data sent is success
                generate_send_coc_data();
            }
        }
        break;
        default:
            printf("[l2c_coc_cb] Unhandled event %d \n",msg->hdr.event);
        break;
    }
}

int handle_coc_init( int argc, char *argv[], tlv_buffer_t** data )
{
    printf("Initalising LE COC with PSM 19 and MTU 100 \n");
    L2cCocInit();
    l2c_coc_reg.mtu = le_coc_mtu;
    l2c_coc_reg.psm = le_coc_psm;
    HciSetMaxRxAclLen (l2c_coc_reg.mtu + 5);
    {
        wsfHandlerId_t handlerId = WsfOsSetNextHandler(L2cCocHandler);
        printf("Handled Id %d \n",handlerId);
        L2cCocHandlerInit(handlerId);
    }
    return 0;
}

int handle_coc_adv( int argc, char *argv[], tlv_buffer_t** data )
{
    bool status;
    if (l2c_coc_regId != L2C_COC_REG_ID_NONE)
    {
        printf("Deregistering ...\n");
        L2cCocDeregister(l2c_coc_regId);
        l2c_coc_regId = L2C_COC_REG_ID_NONE;
    }

    l2c_coc_reg.role = L2C_COC_ROLE_ACCEPTOR;
    l2c_coc_regId = L2cCocRegister(&l2c_coc_cb, &l2c_coc_reg);
    if ( l2c_coc_regId == L2C_COC_REG_ID_NONE)
    {
        printf(" FAILED to register L2CAP COC \n");
        return 1;
    }
    printf("Register L2CAP COC as ACCEPTOR \n");

    status = test_coex->start_adv(1);
    printf("[handle_coc_adv] status %d %d \n",status,HciGetMaxRxAclLen());
    return 0;
}

int handle_coc_scan_connect( int argc, char *argv[], tlv_buffer_t** data )
{
    if (l2c_coc_regId != L2C_COC_REG_ID_NONE )
    {
        printf("Deregistering ...\n");
        L2cCocDeregister(l2c_coc_regId);
        l2c_coc_regId = L2C_COC_REG_ID_NONE;
    }

    l2c_coc_reg.role = L2C_COC_ROLE_INITIATOR;
    l2c_coc_regId = L2cCocRegister(l2c_coc_cb, &l2c_coc_reg);
    le_coc_connect_pending = true;

    return handle_connect(argc,argv,data);
}

int handle_coc_disconnect( int argc, char *argv[], tlv_buffer_t** data )
{
    L2cCocDisconnectReq(le_coc_conn_id);
    return 0;
}

int handle_coc_send_data( int argc, char *argv[], tlv_buffer_t** data )
{
    printf("handle_coc_send_data \n");
    if (le_coc_send_data != true)
    {
        total_sent_data = 0;
        le_coc_send_data = true;
        generate_send_coc_data();
        mbed_sent_data_timer.reset();
        mbed_sent_data_timer.start();
    }
    else
    {
        printf("Already data send is ON \n");
        return 1;
    }
    return 0;
}

int handle_coc_stop_data( int argc, char *argv[], tlv_buffer_t** data )
{
    printf("handle_coc_stop_data \n");
    if (le_coc_send_data == true)
    {
        le_coc_send_data = false;
        mbed_sent_data_timer.stop();
    }
    else
    {
        printf("Already data send is OFF \n");
        return 1;
    }
    return 0;
}

int handle_coc_throughput( int argc, char *argv[], tlv_buffer_t** data )
{
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(mbed_sent_data_timer.elapsed_time());
    int time_in_us = elapsed_time.count();

    float result = 0;
    if (time_in_us != 0)
    {
        result = ((float)total_sent_data/time_in_us)*8*1000*1000;
        printf("Sent data %ld bytes in time %d microsecond Throughput : %lf bps \n", (long)total_sent_data, time_in_us, result);
        mbed_sent_data_timer.reset();
        total_sent_data = 0;
    }
    else
    {
        printf("Total Sent time is Zero \n");
    }

    elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(mbed_received_data_timer.elapsed_time());
    time_in_us = elapsed_time.count();
    if (time_in_us != 0)
    {
        result = ((float)total_received_data/time_in_us)*8*1000*1000;
        printf("Received data %ld bytes in time %d microsecond Throughput : %lf bps", (long)total_received_data, time_in_us, result);
        mbed_received_data_timer.reset();
        total_received_data = 0;
    }
    else
    {
        printf("Total Receive time is Zero \n");
    }
    return 0;
}

// BT Coex Utility initialization
void bt_utility_init(void)
{
    cy_command_console_add_table ( bt_coex_command_table );

    BLE &ble = BLE::Instance();
    ble.onEventsToProcess(schedule_ble_events);
    static BLECoexTest temp_coex(ble, event_queue);
    test_coex = &temp_coex;
    // TODO : Start should be called when ble_on command is received.
    test_coex->start();
}
#ifdef __cplusplus
}
#endif
