/*
 * Copyright (C) 2022 Dalton Martins <daltonvlm@gmail.com>
 *
 * This file is part of vsa.
 *
 * vsa is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * vsa is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with vsa.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef VSA_VALUE_H
#define VSA_VALUE_H

#include <arpa/inet.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/mib_modules.h>

#include <vsa/asn_type.h>

#define VSA_VALUE_NEW_ERROR_MSG "vsa_value_new() failed"
#define VSA_VALUE_TO_STR_ERROR_MSG "vsa_value_to_str() failed"

typedef struct vsa_value_s vsa_value_t;

struct vsa_value_s {
    vsa_asn_type_t          type;
    union {
        char                   *string_value;
#if defined __x86_64
        long                     int_value;
#else
        int                     int_value;
#endif
        unsigned long           ulong_value;
        struct counter64        counter64_value;
        struct {
            unsigned char          *values;
            size_t                  len;
        } hex_value;
        struct in_addr          ip_value;
        vsa_oid_t              *oid_value;
    } value;
};

vsa_value_t            *vsa_value_new(vsa_asn_type_t type, const char *str);
void                   *vsa_value_free(vsa_value_t * value);
char                   *vsa_value_to_str(vsa_value_t * value);

#endif // VSA_VALUE_H
