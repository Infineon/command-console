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

#ifndef CONFIG_H
#define CONFIG_H

/* ===================================================================
 * config.h
 *
 * config.h is derived from config.h.in -- do not edit config.h
 *
 * This contains variables that the configure script checks and
 * then defines or undefines. The source code checks for these
 * variables to know if certain features are present.
 * =================================================================== */

/* Define if threads exist (using pthreads or Win32 threads) */
/* #undef HAVE_THREAD */
/* #undef HAVE_POSIX_THREAD */
/* #undef HAVE_WIN32_THREAD */
/* #undef _REENTRANT */

/* Define if on OSF1 and need special extern "C" around some header files */
/* #undef SPECIAL_OSF1_EXTERN */

/* Define if the strings.h header file exists */

#define HAVE_STRINGS_H

#if defined(__ARMCC_VERSION) && !defined(SSIZE_MAX)
typedef signed   int  ssize_t;  ///< Signed size type, usually encodes negative errors
#endif

#ifndef HAVE_SNPRINTF
#define HAVE_SNPRINTF
#endif

#define HAVE_INT64_T

/* Define if you have these functions. */
/* #define HAVE_SNPRINTF */
/* #undef HAVE_INET_PTON */
/* #undef HAVE_INET_NTOP */

/* #undef HAVE_GETTIMEOFDAY */
#define gettimeofday(tv, timezone)      anycloud_gettimeofday(tv, timezone)

typedef unsigned long useconds_t;

/* #undef HAVE_PTHREAD_CANCEL */
/* #undef HAVE_USLEEP */
/* #undef HAVE_QUAD_SUPPORT */
/* #undef HAVE_PRINTF_QD */

/* standard C++, which isn't always... */
/* #undef bool */

#endif /* CONFIG_H */
