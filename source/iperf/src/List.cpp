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
 * List.cpp
 * by Kevin Gibbs <kgibbs@ncsa.uiuc.edu>
 * -------------------------------------------------------------------
 */

/* IPERF_MODIFIED Start */
/* As build system looks for all the header files with the name and adding include paths. It includes mutex.h present in some other component
 * Hence files are renamed by appending iperf.
 */
#include "iperf_list.h"
#include "iperf_mutex.h"
/* IPERF_MODIFIED End */
#include "SocketAddr.h"

/*
 * Global List and Mutex variables
 */
Iperf_ListEntry *clients = NULL;
Mutex clients_mutex;

/*
 * Add Entry add to the List
 */
void Iperf_pushback ( Iperf_ListEntry *add, Iperf_ListEntry **root ) {
    add->next = *root;
    *root = add;
}

/*
 * Delete Entry del from the List
 */
void Iperf_delete ( iperf_sockaddr *del, Iperf_ListEntry **root ) {
    Iperf_ListEntry *temp = Iperf_present( del, *root );
    if ( temp != NULL ) {
        if ( temp == *root ) {
            *root = (*root)->next;
        } else {
            Iperf_ListEntry *itr = *root;
            while ( itr->next != NULL ) {
                if ( itr->next == temp ) {
                    itr->next = itr->next->next;
                    break;
                }
                itr = itr->next;
            }
        }
        /* IPERF_MODIFIED Start */
        IPERF_DEBUGF( MEMFREE_DEBUG, IPERF_MEMFREE_MSG( temp ) );
        /* IPERF_MODIFIED End */
        delete temp;
    }
}

/*
 * Destroy the List (cleanup function)
 */
void Iperf_destroy ( Iperf_ListEntry **root ) {
    Iperf_ListEntry *itr1 = *root, *itr2;
    while ( itr1 != NULL ) {
        itr2 = itr1->next;
        /* IPERF_MODIFIED Start */
        IPERF_DEBUGF( MEMFREE_DEBUG, IPERF_MEMFREE_MSG( itr1 ) );

        FREE_PTR( itr1 );
        /* IPERF_MODIFIED End */
        itr1 = itr2;
    }
    *root = NULL;
}

/*
 * Check if the exact Entry find is present
 */
Iperf_ListEntry* Iperf_present ( iperf_sockaddr *find, Iperf_ListEntry *root ) {
    Iperf_ListEntry *itr = root;
    while ( itr != NULL ) {

/* IPERF_MODIFIED Start */
/*
 * BUGFIX: Although it seemed to work fine, the original code seems to
 * be wrong. `itr' is of type `Iperf_ListEntry *' and probably shouldn't
 * be arbitrarily cast to a `sockaddr *'.
 */
#if 0
    if ( SockAddr_are_Equal( (sockaddr*)itr, (sockaddr*)find ) ) {
#else
    if ( SockAddr_are_Equal( (sockaddr*) &(itr->data), (sockaddr*)find ) ) {
#endif
/* IPERF_MODIFIED End */
            return itr;
        }
        itr = itr->next;
    }
    return NULL;
}

/*
 * Check if a Entry find is in the List or if any
 * Entry exists that has the same host as the
 * Entry find
 */
Iperf_ListEntry* Iperf_hostpresent ( iperf_sockaddr *find, Iperf_ListEntry *root ) {
    Iperf_ListEntry *itr = root;
    while ( itr != NULL ) {
    /* IPERF_MODIFIED Start */
        /*
         * BUGFIX: Although it seemed to work fine, the original code seems to
         * be wrong. `itr' is of type `Iperf_ListEntry *' and probably shouldn't
         * be arbitrarily cast to a `sockaddr *'.
         */
#if 0
        if ( SockAddr_Hostare_Equal( (sockaddr*)itr, (sockaddr*)find ) ) {
#else
        if ( SockAddr_Hostare_Equal( (sockaddr*) &(itr->data), (sockaddr*)find ) ) {
#endif
    /* IPERF_MODIFIED End */
            return itr;
        }
        itr = itr->next;
    }
    return NULL;
}
