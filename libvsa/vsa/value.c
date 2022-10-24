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

#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <vsa/asn_type.h>
#include <vsa/log.h>
#include <vsa/oid.h>
#include <vsa/parser.h>
#include <vsa/value.h>

#define VSA_VALUE_ULONG_SIZE (sizeof(unsigned long))

#define VSA_VALUE_UNKNOWN_TYPE_ERROR_MSG "unknown type value '%d'"

vsa_value_t            *
vsa_value_new(vsa_asn_type_t type, const char *str)
{
    char                   *string_value;
    unsigned char          *hex_values;
    unsigned long           ulong_value;
    size_t                  len;
    oid                    *oids;
    vsa_value_t            *value;

    value = calloc(1, sizeof (vsa_value_t));
    if (!value) {
        vsa_log_debugln("%s", strerror(errno));
        return NULL;
    }

    value->type = type;

    switch (type) {
    case VSA_ASN_BIT:
    case VSA_ASN_HEX_STRING:
    case VSA_ASN_NETWORK_ADDRESS:
        hex_values = vsa_parser_parse_hex_values(str, &len);
        if (!hex_values) {
            vsa_log_debugln(VSA_PARSER_PARSE_HEX_VALUES_ERROR_MSG);
            return vsa_value_free(value);
        }
        value->value.hex_value.values = hex_values;
        value->value.hex_value.len = len;
        break;

    case VSA_ASN_COUNTER_32:
    case VSA_ASN_GAUGE_32:
    case VSA_ASN_TIMETICKS:
        if (-1 == vsa_parser_parse_number(str, &value->value.ulong_value)) {
            vsa_log_debugln(VSA_PARSER_PARSE_NUMBER_ERROR_MSG);
            return vsa_value_free(value);
        }
        break;

    case VSA_ASN_COUNTER_64:
        if (-1 == vsa_parser_parse_number(str, &ulong_value)) {
            vsa_log_debugln(VSA_PARSER_PARSE_NUMBER_ERROR_MSG);
            return vsa_value_free(value);
        }
        value->value.counter64_value.low = ulong_value & ~0;
        value->value.counter64_value.high = ulong_value >> 32 & ~0;
        break;

    case VSA_ASN_INTEGER:
        if (-1 == vsa_parser_parse_number(str, &ulong_value)) {
            vsa_log_debugln(VSA_PARSER_PARSE_NUMBER_ERROR_MSG);
            return vsa_value_free(value);
        }
        value->value.int_value = ulong_value;
        break;

    case VSA_ASN_IP_ADDRESS:
        if (inet_pton(AF_INET, str, &value->value.ip_value) <= 0) {

            if (errno) {
                vsa_log_debugln("%s", strerror(errno));
            } else {
                vsa_log_debugln("invalid address: '%s'", str);
            }
            return vsa_value_free(value);
        }
        break;

    case VSA_ASN_OCTET_STRING:
    case VSA_ASN_STRING:
        string_value = strdup(str);
        if (!string_value) {
            vsa_log_debugln("%s", strerror(errno));
            return vsa_value_free(value);
        }
        value->value.string_value = string_value;
        break;

    case VSA_ASN_OID:
        oids = vsa_parser_parse_oid(str, &len);
        if (!oids) {
            vsa_log_debugln(VSA_PARSER_PARSE_OID_ERROR_MSG);
            return vsa_value_free(value);
        }
        value->value.oid_value = vsa_oid_new(oids, len);
        break;

    default:
        vsa_log_debugln(VSA_VALUE_UNKNOWN_TYPE_ERROR_MSG, type);
        return vsa_value_free(value);
        break;
    }

    return value;
}

void                   *
vsa_value_free(vsa_value_t * value)
{
    if (!value) {
        return NULL;
    }

    switch (value->type) {
    case VSA_ASN_BIT:
    case VSA_ASN_HEX_STRING:
    case VSA_ASN_NETWORK_ADDRESS:
        free(value->value.hex_value.values);
        break;

    case VSA_ASN_OCTET_STRING:
    case VSA_ASN_STRING:
        free(value->value.string_value);
        break;

    case VSA_ASN_OID:
        vsa_oid_free(value->value.oid_value);
        break;
    default:
        break;
    }

    free(value);

    return NULL;
}

char                   *
vsa_value_to_str(vsa_value_t * value)
{
    char                   *str, *type, *tmp;
    char                    address_str[INET_ADDRSTRLEN];
    unsigned char          *values;
    size_t                  len;

    str = NULL;

    switch (value->type) {
    case VSA_ASN_BIT:
    case VSA_ASN_HEX_STRING:
    case VSA_ASN_NETWORK_ADDRESS:
        values = value->value.hex_value.values;
        len = value->value.hex_value.len;
        for (size_t i = 0; i < len; i++) {
            tmp = str;
            if (-1 == asprintf(&str, "%s%s%02X", (str ? str : ""), (str ? " " : ""), values[i])) {
                vsa_log_debugln("%s", VSA_LOG_AS_PRINTF_ERROR_MSG);
                free(tmp);
                return NULL;
            }
            free(tmp);
        }
        break;

    case VSA_ASN_OCTET_STRING:
    case VSA_ASN_STRING:
        str = strdup(value->value.string_value);
        if (!str) {
            vsa_log_debugln("%s", strerror(errno));
            return NULL;
        }
        break;

    case VSA_ASN_COUNTER_32:
    case VSA_ASN_GAUGE_32:
    case VSA_ASN_TIMETICKS:
        if (-1 == asprintf(&str, "%lu", value->value.ulong_value)) {
            vsa_log_debugln("%s", VSA_LOG_AS_PRINTF_ERROR_MSG);
            return NULL;
        }
        break;

    case VSA_ASN_COUNTER_64:
        if (-1 ==
            asprintf(&str, "%lu",
                     value->value.counter64_value.low | (((unsigned long) value->value.counter64_value.high) <<
                                                         (VSA_VALUE_ULONG_SIZE / 2)))) {
            vsa_log_debugln("%s", VSA_LOG_AS_PRINTF_ERROR_MSG);
            return NULL;
        }
        break;

    case VSA_ASN_INTEGER:
        if (-1 == asprintf(&str, "%ld", value->value.int_value)) {
            vsa_log_debugln("%s", VSA_LOG_AS_PRINTF_ERROR_MSG);
            return NULL;
        }
        break;

    case VSA_ASN_IP_ADDRESS:
        if (!inet_ntop(AF_INET, &value->value.ip_value.s_addr, address_str, sizeof (address_str))) {
            vsa_log_debugln("%s", strerror(errno));
            return NULL;
        }
        str = strdup(address_str);
        if (!str) {
            vsa_log_debugln("%s", strerror(errno));
            return NULL;
        }
        break;

    case VSA_ASN_OID:
        str = vsa_oid_to_str(value->value.oid_value);
        if (!str) {
            vsa_log_debugln(VSA_OID_TO_STR_ERROR_MSG);
            return NULL;
        }
        break;

    default:
        vsa_log_debugln(VSA_VALUE_UNKNOWN_TYPE_ERROR_MSG, value->type);
        return NULL;
        break;
    }

    type = vsa_asn_type_to_str(value->type);
    if (!type) {
        vsa_log_debugln(VSA_ASN_TYPE_TO_STR_ERROR_MSG);
        free(str);
        return NULL;
    }

    tmp = str;
    if (-1 == asprintf(&str, "%s -> %s", type, str)) {
        vsa_log_debugln("%s", VSA_LOG_AS_PRINTF_ERROR_MSG);
        free(tmp);
        free(type);
        return NULL;
    }
    free(tmp);
    free(type);

    return str;
}
