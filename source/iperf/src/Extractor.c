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
 * Extractor.cpp
 * by Ajay Tirumala (tirumala@ncsa.uiuc.edu)
 * -------------------------------------------------------------------
 * Extract data from a file, used to measure the transfer rates
 * for various stream formats.
 *
 * E.g. Use a gzipped file to measure the transfer rates for
 * compressed data
 * Use an MPEG file to measure the transfer rates of
 * Multimedia data formats
 * Use a plain BMP file to measure the transfer rates of
 * Uncompressed data
 *
 * This is beneficial especially in measuring bandwidth across WAN
 * links where data compression takes place before data transmission
 * -------------------------------------------------------------------
 */

#include "Extractor.h"

/* IPERF_MODIFIED Start */
#ifndef NO_FILE_IO
/* IPERF_MODIFIED End */

/**
 * Constructor
 * @arg fileName   Name of the file
 * @arg size       Block size for reading
 * Open the file and set the block size
 */
void Extractor_Initialize ( char *fileName, int inSize, thread_Settings *mSettings ) {
    /* IPERF_MODIFIED Start */
    IPERF_DEBUGF( EXTRACTOR_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Initializing an Extractor at file %s of block size %d.\n", fileName, inSize ) );
    /* IPERF_MODIFIED End */

    if ( (mSettings->Extractor_file = fopen (fileName, "rb")) == NULL ) {
        fprintf( stderr, "Unable to open the file stream\n");
        fprintf( stderr, "Will use the default data stream\n");
        return;
    }
    mSettings->Extractor_size =  inSize;
}


/**
 * Constructor
 * @arg fp         File Pointer
 * @arg size       Block size for reading
 * Set the block size,file pointer
 */
void Extractor_InitializeFile ( FILE *fp, int inSize, thread_Settings *mSettings ) {
    mSettings->Extractor_file = fp;
    mSettings->Extractor_size =  inSize;
}


/**
 * Destructor - Close the file
 */
void Extractor_Destroy ( thread_Settings *mSettings ) {
    /* IPERF_MODIFIED Start */
    IPERF_DEBUGF( EXTRACTOR_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Destroying Extractor.\n" ) );
    /* IPERF_MODIFIED End */
    if ( mSettings->Extractor_file != NULL )
        fclose( mSettings->Extractor_file );
}


/*
 * Fetches the next data block from
 * the file
 * @arg block     Pointer to the data read
 * @return        Number of bytes read
 */
int Extractor_getNextDataBlock ( char *data, thread_Settings *mSettings ) {
    /* IPERF_MODIFIED Start */
    IPERF_DEBUGF( EXTRACTOR_DEBUG | IPERF_DBG_TRACE, ( "Fetching the next data block from the Extractor file.\n" ) );
    /* IPERF_MODIFIED End */
    if ( Extractor_canRead( mSettings ) ) {
        return(fread( data, 1, mSettings->Extractor_size,
                      mSettings->Extractor_file ));
    }
    return 0;
}

/**
 * Function which determines whether
 * the file stream is still readable
 * @return boolean    true, if readable; false, if not
 */
int Extractor_canRead ( thread_Settings *mSettings ) {
    return(( mSettings->Extractor_file != NULL )
           && !(feof( mSettings->Extractor_file )));
}

/**
 * This is used to reduce the read size
 * Used in UDP transfer to accomodate the
 * the header (timestamp)
 * @arg delta         Size to reduce
 */
void Extractor_reduceReadSize ( int delta, thread_Settings *mSettings ) {
    /* IPERF_MODIFIED Start */
    IPERF_DEBUGF( EXTRACTOR_DEBUG | IPERF_DBG_TRACE, ( "Reducing the read size of the Extractor from %d to %d.\n", mSettings->Extractor_size, (mSettings->Extractor_size - delta) ) );
    /* IPERF_MODIFIED End */
    mSettings->Extractor_size -= delta;
}

















































/* IPERF_MODIFIED Start */
#endif /* NO_FILE_IO */
/* IPERF_MODIFIED End */
