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
 * Copyright (c) 2017
 * Broadcom Corporation
 * All Rights Reserved.
 *---------------------------------------------------------------
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated
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
 * Neither the name of Broadcom Coporation,
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
 *
 * checkisoch.cpp
 *
 * Test routine to test the isochronous functions
 *
 * by Robert J. McMahon (rjmcmahon@rjmcmahon.com, bob.mcmahon@broadcom.com)
 * ------------------------------------------------------------------- */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
/* IPERF_MODIFIED Start */
#if 0
/* TODO : Fix me */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "headers.h"
#include "isochronous.hpp"
#include "delay.h"
#include "pdfs.h"
#include "util.h"

static void posttimestamp(int, int);

int main (int argc, char **argv) {
    int c, count=61, frequency=60;
    float mean=1e8;
    float variance=3e7;

    Isochronous::FrameCounter *fc = NULL;
    
    while ((c = getopt(argc, argv, "c:f:m:v:")) != -1) {
        switch (c) {
        case 'c':
            count = atoi(optarg);
            break;
        case 'f':
	    frequency = atoi(optarg);
            break;
	case 'm':
	    mean=byte_atof(optarg);
	    break;
	case 'v':
	    variance=byte_atof(optarg);
	    break;
        case '?':
            fprintf (stderr, "usage: -c <count> -f <frames per second> -m <mean> -v <variance>\n");
            return 1;
        default:
            abort ();
        }
    }
    fc = new Isochronous::FrameCounter(frequency);

    fprintf(stdout,"Timestamping %d times at %d fps\n", count, frequency);
    fflush(stdout);
    while (count-- > 0) {
	if (count == 8) {
	    delay_loop (1000000/frequency + 10);
	}
	fc->wait_tick();
	posttimestamp(count, (round(lognormal(mean,variance)) / (frequency * 8)));
	if (fc->slip) {
	    fprintf(stdout,"Slip occurred\n");
	    fc->slip = 0;
	}
    }
}

void posttimestamp (int count, int bytes) {
    struct timespec t1;
    double timestamp;
    int err;

    err = clock_gettime(CLOCK_REALTIME, &t1);
    if (err < 0) {
        perror("clock_getttime");
    } else {
        timestamp = t1.tv_sec + (t1.tv_nsec / 1000000000.0);
        fprintf(stdout,"%f counter(%d), sending %d bytes\n", timestamp, count, bytes);
    }
    fflush(stdout);
}
#endif
/* IPERF_MODIFIED End */
