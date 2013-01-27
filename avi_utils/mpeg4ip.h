 /*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is MPEG4IP.
 * 
 * The Initial Developer of the Original Code is Cisco Systems Inc.
 * Portions created by Cisco Systems Inc. are
 * Copyright (C) Cisco Systems Inc. 2000-2005.  All Rights Reserved.
 * 
 * Contributor(s): 
 *		Dave Mackie		dmackie@cisco.com
 *              Bill May                wmay@cisco.com
 */

#ifndef __MPEG4IP_INCLUDED__
#define __MPEG4IP_INCLUDED__

/*****************************************************************************
 *   UNIX LIKE DEFINES BELOW THIS POINT
 *****************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <sys/param.h>

#ifdef __cplusplus
extern "C" {
#endif
//char *strcasestr(const char *haystack, const char *needle);
#ifdef __cplusplus
}
#endif

#define OPEN_RDWR O_RDWR
#define OPEN_CREAT O_CREAT 
#define OPEN_RDONLY O_RDONLY

#define closesocket close
#define IOSBINARY ios::bin

#if SIZEOF_LONG == 8
#define MAX_UINT64 -1LU
#define D64F "ld"
#define U64F  "lu"
#define X64F "lx"

#define TO_D64(a) (a##L)
#define TO_U64(a) (a##LU)
#else
#define MAX_UINT64 -1LLU
#define D64F "lld"
#define U64F  "llu"
#define X64F "llx"

#define TO_D64(a) (a##LL)
#define TO_U64(a) (a##LLU)
#endif
/*****************************************************************************
 *             Generic type includes used in the whole package               *
 *****************************************************************************/
#define D64  "%"D64F
#define U64  "%"U64F
#define X64 "%"X64F

#define M_LLU TO_U64(1000)
#define M_64 TO_U64(1000)
#define LLU  U64

#include <stdarg.h>
typedef void (*error_msg_func_t)(int loglevel,
				 const char *lib,
				 const char *fmt,
				 va_list ap);
typedef void (*lib_message_func_t)(int loglevel,
				   const char *lib,
				   const char *fmt,
				   ...);

#ifndef HAVE_STRSEP
#ifdef __cplusplus
extern "C" {
#endif
char *strsep(char **strp, const char *delim); 
#ifdef __cplusplus
}
#endif
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef INADDR_NONE
#define INADDR_NONE (-1)
#endif

#define MALLOC_STRUCTURE(a) ((a *)malloc(sizeof(a)))

#define CHECK_AND_FREE(a) if ((a) != NULL) { free((void *)(a)); (a) = NULL;}

#define NUM_ELEMENTS_IN_ARRAY(name) ((sizeof((name))) / (sizeof(*(name))))

#define ADV_SPACE(a) {while (isspace(*(a)) && (*(a) != '\0'))(a)++;}

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef __cplusplus

#ifndef bool
 #if SIZEOF_BOOL == 8
  typedef uint64_t bool;
 #else
   #if SIZEOF_BOOL == 4
    typedef uint32_t bool;
   #else
     #if SIZEOF_BOOL == 2
      typedef uint16_t bool;
     #else
      typedef unsigned char bool;
     #endif
   #endif
 #endif
 #ifndef false
 #define false FALSE
 #endif
 #ifndef true
 #define true TRUE
 #endif
#endif

#endif

#ifndef ROUND
# ifdef HAVE_RINT
# define ROUND(f) rint(f)
# else
# define ROUND(f) (int)(floor((f) + 0.5))
# endif
#endif

#ifndef INT16_MAX
# define INT16_MAX (32767)
#endif
#ifndef INT16_MIN 
# define INT16_MIN (-32767-1)
#endif 

#ifndef UINT32_MAX
# define UINT32_MAX             (4294967295U)
#endif

#ifndef UINT64_MAX
# define UINT64_MAX TO_U64(0xffffffffffffffff)
#endif

typedef enum audio_format_t {
  AUDIO_FMT_U8 = 0,
  AUDIO_FMT_S8,
  AUDIO_FMT_U16LSB,
  AUDIO_FMT_S16LSB,
  AUDIO_FMT_U16MSB,
  AUDIO_FMT_S16MSB,
  AUDIO_FMT_U16,
  AUDIO_FMT_S16,
  AUDIO_FMT_FLOAT,
  AUDIO_FMT_HW_AC3,
} audio_format_t;

#endif /* __MPEG4IP_INCLUDED__ */


