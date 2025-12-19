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

/*---------------------------------------------------------------
 * Copyright (c) 1999,2000,2001,2002,2003
 * The Board of Trustees of the University of Illinois
 * All Rights Reserved.
 *---------------------------------------------------------------
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software (Iperf) and associated
 * documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 *
 * Redistributions of source code must retain the above
 * copyright notice, this list of conditions and
 * the following disclaimers.
 *
 *
 * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimers in the documentation and/or other materials
 * provided with the distribution.
 *
 *
 * Neither the names of the University of Illinois, NCSA,
 * nor the names of its contributors may be used to endorse
 * or promote products derived from this Software without
 * specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE CONTIBUTORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * ________________________________________________________________
 * National Laboratory for Applied Network Research
 * National Center for Supercomputing Applications
 * University of Illinois at Urbana-Champaign
 * http://www.ncsa.uiuc.edu
 * ________________________________________________________________
 *
 * ReportCSV.c
 * by Kevin Gibbs <kgibbs@nlanr.net>
 *
 * ________________________________________________________________ */

/* IPERF_MODIFIED Start */
#include "iperf_locale.h"
#include "iperf_util.h"
/* IPERF_MODIFIED End */
#include "headers.h"
#include "Settings.hpp"
#include "Reporter.h"
#include "report_CSV.h"


void CSV_stats( Transfer_Info *stats ) {
    // $TIMESTAMP,$ID,$INTERVAL,$BYTE,$SPEED,$JITTER,$LOSS,$PACKET,$%LOSS
    intmax_t speed = (intmax_t) ((stats->TotalLen > 0) ? (((double)stats->TotalLen * 8.0) / (stats->endTime -  stats->startTime)) : 0);
    char timestamp[160];
    int milliseconds;
#ifdef HAVE_CLOCK_GETTIME
    struct timespec t1;
    clock_gettime(CLOCK_REALTIME, &t1);
    /* IPERF_MODIFIED_Start */
    milliseconds = (int)(t1.tv_nsec / 1e6);
    /* IPERF_MODIFIED_End */
#else
    struct timeval t1;
    gettimeofday( &t1, NULL );
    /* IPERF_MODIFIED_Start */
    milliseconds = (int)(t1.tv_usec / 1e3);
    /* IPERF_MODIFIED_End */
#endif

   // localtime is not thread safe.  It's only used by the reporter thread.  Use localtime_r if thread safe is ever needed.
    if (!stats->mEnhanced) {
    /* IPERF_MODIFIED Start */
    strftime(timestamp, 80, "%Y%m%d%H%M%S", localtime((const time_t*)&t1.tv_sec));
    /* IPERF_MODIFIED End */
    } else {
	char  buffer[80];
    /* IPERF_MODIFIED Start */
	strftime(buffer, 80, "%Y%m%d%H%M%S", localtime((const time_t*)&t1.tv_sec));
    /* IPERF_MODIFIED End */
	snprintf(timestamp, 160, "%s.%.3d", buffer, milliseconds);
    }
    if ( stats->mUDP != (char)kMode_Server ) {
        // TCP Reporting
        printf( reportCSV_bw_format,
                timestamp,
                (stats->reserved_delay == NULL ? ",,," : stats->reserved_delay),
                stats->transferID,
                stats->startTime,
                stats->endTime,
                stats->TotalLen,
                speed);
    } else {
        // UDP Reporting
        printf( reportCSV_bw_jitter_loss_format,
                timestamp,
                (stats->reserved_delay == NULL ? ",,," : stats->reserved_delay),
                stats->transferID,
                stats->startTime,
                stats->endTime,
                stats->TotalLen,
                speed,
                stats->jitter*1000.0,
                stats->cntError,
                stats->cntDatagrams,
                (100.0 * stats->cntError) / stats->cntDatagrams, stats->cntOutofOrder );
    }
    if ( stats->free == 1 && stats->reserved_delay != NULL ) {
        free( stats->reserved_delay );
    }
}

void *CSV_peer( Connection_Info *stats, int ID ) {

    // copy the inet_ntop into temp buffers, to avoid overwriting
    char local_addr[ REPORT_ADDRLEN ];
    char remote_addr[ REPORT_ADDRLEN ];
    /* IPERF_MODIFIED Start */
    char *buf = (char*) malloc( REPORT_ADDRLEN * 2 + 10 );
    FAIL( buf == NULL, ( "No memory for buffer buf.\n" ), NULL );
    IPERF_DEBUGF( MEMALLOC_DEBUG | IPERF_DBG_TRACE, IPERF_MEMALLOC_MSG( buf, REPORT_ADDRLEN * 2 + 10 ) );
    /* IPERF_MODIFIED End */
    struct sockaddr *local = ((struct sockaddr*)&stats->local);
    struct sockaddr *peer = ((struct sockaddr*)&stats->peer);

    if ( local->sa_family == AF_INET ) {
        inet_ntop( AF_INET, &((struct sockaddr_in*)local)->sin_addr,
                   local_addr, REPORT_ADDRLEN);
    }
#ifdef HAVE_IPV6
      else {
        inet_ntop( AF_INET6, &((struct sockaddr_in6*)local)->sin6_addr,
                   local_addr, REPORT_ADDRLEN);
    }
#endif

    if ( peer->sa_family == AF_INET ) {
        inet_ntop( AF_INET, &((struct sockaddr_in*)peer)->sin_addr,
                   remote_addr, REPORT_ADDRLEN);
    }
#ifdef HAVE_IPV6
      else {
        inet_ntop( AF_INET6, &((struct sockaddr_in6*)peer)->sin6_addr,
                   remote_addr, REPORT_ADDRLEN);
    }
#endif

    snprintf(buf, REPORT_ADDRLEN*2+10, reportCSV_peer,
             local_addr, ( local->sa_family == AF_INET ?
                          ntohs(((struct sockaddr_in*)local)->sin_port) :
#ifdef HAVE_IPV6
                          ntohs(((struct sockaddr_in6*)local)->sin6_port)),
#else
                          0),
#endif
            remote_addr, ( peer->sa_family == AF_INET ?
                          ntohs(((struct sockaddr_in*)peer)->sin_port) :
#ifdef HAVE_IPV6
                          ntohs(((struct sockaddr_in6*)peer)->sin6_port)));
#else
                          0));
#endif
    return buf;
}

void CSV_serverstats( Connection_Info *conn, Transfer_Info *stats ) {
    stats->reserved_delay = CSV_peer( conn, stats->transferID );
    stats->free = 1;
    CSV_stats( stats );
}

