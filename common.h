/*
 * Copyright (c) 2013 Morgen Matvey, Yulugin Evgeny and others.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * The names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <string.h>
#include <proc/procps.h>
#include <assert.h>
#include <syslog.h>

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

// Macro to print fatal errors
#define MSS_FATAL(message, error)                                       \
do {                                                                    \
  syslog(LOG_ERR, "File %s,line %d, %s, %s: %s\n",                      \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message, strerror(error));             \
} while (0);

// Macro to print fatal messages
#define MSS_FATAL_MESSAGE(message)                                      \
do {                                                                    \
  syslog(LOG_EMERG, "File %s,line %d, %s, %s\n",                        \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message);                              \
} while (0);

// Macro to print errors
#define MSS_ERROR(message, error)                                       \
do {                                                                    \
  syslog(LOG_ERR, "File %s,line %d, %s, %s: %s\n",                      \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message, strerror(error));             \
} while (0);

// Macro to print error messages
#define MSS_ERROR_MESSAGE(message)                                      \
do {                                                                    \
  syslog(LOG_ERR, "File %s,line %d, %s, %s\n",                          \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message);                              \
} while (0);

// Macro to print warning errors
#define MSS_WARN(message, error)                                        \
do {                                                                    \
  syslog(LOG_WARNING, "File %s,line %d, %s, %s: %s\n",                  \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message, strerror(error));             \
} while (0);

// Macro to print warning messages
#define MSS_WARN_MESSAGE(message)                                       \
do {                                                                    \
  syslog(LOG_WARNING, "File %s,line %d, %s, %s\n",                      \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message);                              \
} while (0);

// Macro to print log info
#define MSS_INFO(message, error)                                        \
do {                                                                    \
  syslog(LOG_INFO, "File %s,line %d, %s, %s: %s\n",                     \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message, strerror(error));             \
} while (0);

// Macro to print log info
#define MSS_INFO_MESSAGE(message, error)                                \
do {                                                                    \
  syslog(LOG_INFO, "File %s,line %d, %s, %s: %s\n",                     \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message, strerror(error));             \
} while (0);

// Macro to print fatal errors
#ifdef MSS_DEBUG
#define MSS_DEBUG_ERROR(message, error)                                 \
do {                                                                    \
  syslog(LOG_ERR, "File %s,line %d, %s, %s: %s\n",                      \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message, strerror(error));             \
} while (0);

// Macro to print fatal messages
#define MSS_DEBUG_MESSAGE(message)                                      \
do {                                                                    \
  syslog(LOG_DEBUG, "File %s,line %d, %s, %s\n",                        \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message);                              \
} while (0);

#else
#define MSS_DEBUG_ERROR(message, error)                                 \
do {                                                                    \
} while (0);

#define MSS_DEBUG_MESSAGE(message)                                      \
do {                                                                    \
} while (0);
#endif  // #ifdef MSS_DEBUG

template <class mType> mType *CopyToHeap(const mType &obj) {
  return new mType(obj);
}

#endif  // COMMON_H_
