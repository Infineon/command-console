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

#ifndef IPERF_SOCKETS_H
#define IPERF_SOCKETS_H

#include "iperf_sockets_internal.h"
#include "iperf_utility.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

void sockets_layer_init(void);
void iperf_network_init( void * networkInterface );
int iperf_socket( int protocolFamily, int type, int protocol );
int iperf_select( int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout );
int iperf_close( int sockID);
int iperf_write( int sockID, const char *msg, size_t msgLength );
int iperf_read(int s, void *mem, size_t len);
bool iperf_setsock_blocking (int fd, bool blocking);
int iperf_send(int sockID, const char *msg, size_t msgLength, int flags);

int iperf_getpeername(int sockID, struct sockaddr *remoteAddress, uint32_t *addressLength);
int iperf_getsockname(int sockID, struct sockaddr *localAddress, uint32_t *addressLength);
int iperf_recvfrom(int sockID, char *buffer, size_t buffersize, int flags,struct sockaddr *fromAddr, uint32_t *fromAddrLen);
int iperf_recv(int sockID, void *rcvBuffer, size_t bufferLength, int flags);
int iperf_accept(int sockID, struct sockaddr *ClientAddress, uint32_t *addressLength);
int iperf_listen(int sockID, int backlog);
int iperf_bind(int sockID, struct sockaddr *localAddress, uint32_t addressLength);
int iperf_connect(int sockID, struct sockaddr *remoteAddress, uint32_t addressLength);
int iperf_setsockopt(int sockID, int option_level, int option_name, const void *option_value, uint32_t option_length);
int iperf_getsockopt(int sockID, int option_level, int option_name, void *option_value,uint32_t *option_length);
int iperf_sendto(int sockID, char *msg, size_t msgLength, int flags, struct sockaddr *destAddr, uint32_t destAddrLen);
struct hostent* iperf_gethostbyname(const char *name);
#ifdef __cplusplus
} /*extern "C" */
#endif

#endif //IPERF_SOCKETS_H
