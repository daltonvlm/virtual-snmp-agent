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

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <vsa/log.h>
#include <vsa/asn_type.h>

static const struct {
    vsa_asn_type_t          type;
    const char             *name;
    size_t                  len;
} types[] = {
    { VSA_ASN_BIT, "BIT", 3 },
    { VSA_ASN_COUNTER_32, "COUNTER32", 9 },
    { VSA_ASN_COUNTER_64, "COUNTER64", 9 },
    { VSA_ASN_GAUGE_32, "GAUGE32", 7 },
    { VSA_ASN_HEX_STRING, "HEX-STRING", 10 },
    { VSA_ASN_INTEGER, "INTEGER", 7 },
    { VSA_ASN_IP_ADDRESS, "IPADDRESS", 9 },
    { VSA_ASN_NETWORK_ADDRESS, "NETWORK ADDRESS", 15 },
    { VSA_ASN_OCTET_STRING, "OCTETSTRING", 11 },
    { VSA_ASN_OID, "OID", 3 },
    { VSA_ASN_STRING, "STRING", 6 },
    { VSA_ASN_TIMETICKS, "TIMETICKS", 9 },
    { VSA_ASN_UNKNOWN, "UNKNOWN", 0 }
};

vsa_asn_type_t
vsa_asn_type_from_str(const char *str)
{
    for (int i = 0; types[i].len; i++) {
        if (!strncasecmp(str, types[i].name, types[i].len)) {
            return types[i].type;
        }
    }

    // There may be lines with types like: "Wrong Type (should be Gauge32 or Unsigned32)"
    if (strcasestr(str, "gauge32")) {
        return VSA_ASN_GAUGE_32;
    }

    return VSA_ASN_UNKNOWN;
}

char                   *
vsa_asn_type_to_str(vsa_asn_type_t type)
{
    char                   *str;
    int                     i;

    for (i = 0; types[i].len; i++) {
        if (types[i].type == type) {
            break;
        }
    }

    str = strdup(types[i].name);
    if (!str) {
        vsa_log_debugln("%s", strerror(errno));
        return NULL;
    }

    return str;
}
