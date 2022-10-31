/*
 * Copyright (C) 2022 Dalton Martins <daltonvlm@gmail.com>
 *
 * This file is part of Virtual SNMP Agent.
 *
 * Virtual SNMP Agent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Virtual SNMP Agent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Virtual SNMP Agent.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef VSA_LOG_H
#define VSA_LOG_H

#include <stdio.h>

extern char            *program_invocation_name;

// The asprintf documentation doesn't make it clear whether or not it uses errno.
#define VSA_LOG_AS_PRINTF_ERROR_MSG (errno ? strerror(errno) : "asprintf() error")
#define VSA_LOG_INTERNAL_ERROR_MSG "internal error"

// VSA_LOG
#define vsa_log(stream, fmt, ...)\
        fprintf((stream), "%s: " fmt, program_invocation_name, ##__VA_ARGS__)

#define vsa_logln(stream, fmt, ...)\
        vsa_log((stream), fmt "\n", ##__VA_ARGS__)

// VSA_LOG_DEBUG
#define vsa_log_debug(fmt, ...)\
        vsa_log(stderr, "debug: %s: %s: %d: " fmt,\
                __FILE__, __func__, __LINE__, ##__VA_ARGS__)

#define vsa_log_debugln(fmt, ...)\
        vsa_log_debug(fmt "\n", ##__VA_ARGS__)

#if defined(NVSA_DEBUG) || defined(NVSA_ALL)
#undef vsa_log_debug
#define vsa_log_debug(fmt, ...)
#endif

// VSA_LOG_WARN
#define vsa_log_warn(fmt, ...)\
        vsa_log(stderr, "warn: " fmt, ##__VA_ARGS__)

#define vsa_log_warnln(fmt, ...)\
        vsa_log_warn(fmt "\n", ##__VA_ARGS__)

#if defined(NVSA_WARN) || defined(NVSA_ALL)
#undef vsa_log_warn
#define vsa_log_warn(fmt, ...)
#endif

// VSA_LOG_ERROR
#define vsa_log_error(fmt, ...)\
        do {\
            vsa_log(stderr, "error: " fmt, ##__VA_ARGS__);\
            exit(EXIT_FAILURE);\
        }while(0)

#define vsa_log_errorln(fmt, ...)\
        vsa_log_error(fmt "\n", ##__VA_ARGS__)

// VSA_LOG_INFO
#define vsa_log_info(fmt, ...)\
        do {\
            vsa_log(stdout, fmt, ##__VA_ARGS__);\
            fflush(stdout);\
        } while(0)

#define vsa_log_infoln(fmt, ...)\
        vsa_log_info(fmt "\n", ##__VA_ARGS__)

#endif // VSA_LOG_H
