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

/**
 * @file common-inl.h
 *
 * File containing instruments common for all modules.
 */

#ifndef COMMON_INL_H_
#define COMMON_INL_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include <string>

/**
 * A macro to disallow the copy constructor and operator= functions
 * This should be used in the private: declarations for a class.
 *
 * @param TypeName Classname of class that should be protected.
 */
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

/**
 * Macro to print fatal errors.
 *
 * @param message Additional error description.
 * @param error Error code.
 */
#define MSS_FATAL(message, error)                                       \
do {                                                                    \
  syslog(LOG_ERR, "File %s,line %d, %s, %s: %s\n",                      \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message, strerror(error));             \
} while (0);

/**
 * Macro to print fatal messages.
 *
 * @param message Error message.
 */
#define MSS_FATAL_MESSAGE(message)                                      \
do {                                                                    \
  syslog(LOG_EMERG, "File %s,line %d, %s, %s\n",                        \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message);                              \
} while (0);

/**
 * Macro to print errors.
 *
 * @param message Additional error description.
 * @param error Error code.
 */
#define MSS_ERROR(message, error)                                       \
do {                                                                    \
  syslog(LOG_ERR, "File %s,line %d, %s, %s: %s\n",                      \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message, strerror(error));             \
} while (0);

/**
 * Macro to print error messages.
 *
 * @param message Error message.
 */
#define MSS_ERROR_MESSAGE(message)                                      \
do {                                                                    \
  syslog(LOG_ERR, "File %s,line %d, %s, %s\n",                          \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message);                              \
} while (0);

/**
 * Macro to print warnings.
 *
 * @param message Additional error description.
 * @param error Error code.
 */
#define MSS_WARN(message, error)                                        \
do {                                                                    \
  syslog(LOG_WARNING, "File %s,line %d, %s, %s: %s\n",                  \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message, strerror(error));             \
} while (0);

/**
 * Macro to print warning messages.
 *
 * @param message Message.
 */
#define MSS_WARN_MESSAGE(message)                                       \
do {                                                                    \
  syslog(LOG_WARNING, "File %s,line %d, %s, %s\n",                      \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message);                              \
} while (0);

/**
 * Macro to print log information.
 *
 * @param message Additional error description.
 * @param error Error code.
 */
#define MSS_INFO(message, error)                                        \
do {                                                                    \
  syslog(LOG_INFO, "File %s,line %d, %s, %s: %s\n",                     \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message, strerror(error));             \
} while (0);

/**
 * Macro to print log information.
 *
 * @param message Message.
 */
#define MSS_INFO_MESSAGE(message)                                       \
do {                                                                    \
  syslog(LOG_INFO, "File %s,line %d, %s: %s\n",                         \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message);                              \
} while (0);

/**
 * @def MSS_DEBUG_ERROR(message, error)
 * Macro to print debug error description.
 * In release mode print nothing.
 *
 * @param message Additional error description.
 * @param error Error code.
 */
#ifdef MSS_DEBUG
#define MSS_DEBUG_ERROR(message, error)                                 \
do {                                                                    \
  syslog(LOG_ERR, "File %s,line %d, %s, %s: %s\n",                      \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message, strerror(error));             \
} while (0);
#else
#define MSS_DEBUG_ERROR(message, error)                                 \
do {                                                                    \
} while (0);
#endif  // #ifdef MSS_DEBUG

/**
 * @def MSS_DEBUG_MESSAGE(message)
 * Macro to print debug error description.
 * In release mode print nothing.
 *
 * @param message Debug message.
 */
#ifdef MSS_DEBUG
#define MSS_DEBUG_MESSAGE(message)                                      \
do {                                                                    \
  syslog(LOG_DEBUG, "File %s,line %d, %s, %s\n",                        \
         strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__,  \
         __LINE__, __FUNCTION__, message);                              \
} while (0);
#else
#define MSS_DEBUG_MESSAGE(message)                                      \
do {                                                                    \
} while (0);
#endif  // #ifdef MSS_DEBUG

/**
 * Function to copy object to heap.
 *
 * @param obj Object
 *
 * @return Copy of this object, situated in heap.
 */
template <class mType> mType *CopyToHeap(const mType &obj) {
  return new mType(obj);
}

#define LIKELY(x)   __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

/**
 * Read database config file.
 *
 * @param database_name Where to store name of the database.
 * @param database_hostname Where to store hostname of the database.
 * @param database_user Where to store username of the database.
 * @param database_password Where to store password from database.
 * @param database_config_file Path to config file.
 *
 * @return 0 on success, -1 otherwise.
 */
inline int read_database_config(std::string *database_name,
                                std::string *database_hostname,
                                std::string *database_user,
                                std::string *database_password,
                                const char *database_config_file) {
  FILE *fin = fopen(database_config_file, "r");
  if (UNLIKELY(!fin)) {
    MSS_FATAL("fopen", errno);
    return -1;
  }

  char *buf = NULL;
  size_t size = 0;

  // Detect database name
  if (getline(&buf, &size, fin) < 0) {
    MSS_FATAL("getline", errno);
    return -1;
  }
  database_name->assign(buf);
  database_name->erase(database_name->end() - 1);

  // Detect database hostname
  if (getline(&buf, &size, fin) < 0) {
    MSS_FATAL("getline", errno);
    return -1;
  }
  database_hostname->assign(buf);
  database_hostname->erase(database_hostname->end() - 1);

  // Detect database username
  if (getline(&buf, &size, fin) < 0) {
    MSS_FATAL("getline", errno);
    return -1;
  }
  database_user->assign(buf);
  database_user->erase(database_user->end() - 1);

  // Detect database password
  if (getline(&buf, &size, fin) < 0) {
    MSS_FATAL("getline", errno);
    return -1;
  }
  database_password->assign(buf);
  database_password->erase(database_password->end() - 1);

  free(buf);
  fclose(fin);
  return 0;
}

#endif  // COMMON_INL_H_
