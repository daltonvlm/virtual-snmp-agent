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

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/mib_modules.h>

#include <vsa/log.h>
#include <vsa/object.h>
#include <vsa/oid.h>
#include <vsa/value.h>

vsa_object_t           *
vsa_object_new(vsa_oid_t * tree, vsa_value_t * value)
{
    vsa_object_t           *object;

    object = calloc(1, sizeof (vsa_object_t));
    if (!object) {
        vsa_log_debugln("%s", strerror(errno));
        return NULL;
    }
    object->tree = tree;
    object->value = value;

    return object;
}

void                   *
vsa_object_free(vsa_object_t * object)
{
    if (!object) {
        return NULL;
    }
    vsa_oid_free(object->tree);
    vsa_value_free(object->value);
    free(object);

    return NULL;
}

char                   *
vsa_object_to_str(vsa_object_t * object)
{
    char                   *oids, *value, *str;

    oids = vsa_oid_to_str(object->tree);
    if (!oids) {
        vsa_log_debugln(VSA_OID_TO_STR_ERROR_MSG);
        return NULL;
    }

    value = vsa_value_to_str(object->value);
    if (!value) {
        vsa_log_debugln(VSA_VALUE_TO_STR_ERROR_MSG);
        free(oids);
        return NULL;
    }

    str = NULL;
    if (-1 == asprintf(&str, "%s -> %s", oids, value)) {
        vsa_log_debugln("%s", VSA_LOG_AS_PRINTF_ERROR_MSG);
    }
    free(oids);
    free(value);

    return str;
}

int
vsa_object_register(vsa_object_t * object)
{
    netsnmp_handler_registration *reginfo;
    netsnmp_watcher_info   *watcher_info;

    switch (object->value->type) {
    case VSA_ASN_BIT:
        reginfo =
            netsnmp_create_handler_registration(vsa_oid_to_str(object->tree), NULL, object->tree->oids,
                                                object->tree->len, HANDLER_CAN_RWRITE);
        watcher_info =
            netsnmp_create_watcher_info(object->value->value.hex_value.values, object->value->value.hex_value.len,
                                        ASN_BIT_STR, WATCHER_MAX_SIZE);
        return netsnmp_register_watched_instance(reginfo, watcher_info);
        break;

    case VSA_ASN_COUNTER_32:
        return netsnmp_register_read_only_counter32_instance(vsa_oid_to_str(object->tree), object->tree->oids,
                                                             object->tree->len, &object->value->value.ulong_value,
                                                             NULL);
        break;

    case VSA_ASN_COUNTER_64:
        reginfo =
            netsnmp_create_handler_registration(vsa_oid_to_str(object->tree), NULL, object->tree->oids,
                                                object->tree->len, HANDLER_CAN_RONLY);
        watcher_info =
            netsnmp_create_watcher_info(&object->value->value.counter64_value,
                                        sizeof (object->value->value.counter64_value), ASN_OPAQUE, WATCHER_MAX_SIZE);
        return netsnmp_register_watched_instance(reginfo, watcher_info);
        break;

    case VSA_ASN_GAUGE_32:
        return netsnmp_register_ulong_instance(vsa_oid_to_str(object->tree), object->tree->oids,
                                               object->tree->len, &object->value->value.ulong_value, NULL);
        break;

    case VSA_ASN_HEX_STRING:
        reginfo =
            netsnmp_create_handler_registration(vsa_oid_to_str(object->tree), NULL, object->tree->oids,
                                                object->tree->len, HANDLER_CAN_RWRITE);
        watcher_info =
            netsnmp_create_watcher_info(object->value->value.hex_value.values, object->value->value.hex_value.len,
                                        ASN_OCTET_STR, WATCHER_MAX_SIZE);
        return netsnmp_register_watched_instance(reginfo, watcher_info);
        break;

    case VSA_ASN_INTEGER:
#if defined __x86_64
        return netsnmp_register_long_instance(vsa_oid_to_str(object->tree), object->tree->oids, object->tree->len,
                                             &object->value->value.int_value, NULL);
#else
        return netsnmp_register_int_instance(vsa_oid_to_str(object->tree), object->tree->oids, object->tree->len,
                                             &object->value->value.int_value, NULL);
#endif
        break;

    case VSA_ASN_IP_ADDRESS:
        reginfo =
            netsnmp_create_handler_registration(vsa_oid_to_str(object->tree), NULL, object->tree->oids,
                                                object->tree->len, HANDLER_CAN_RWRITE);
        watcher_info =
            netsnmp_create_watcher_info(&(object->value->value.ip_value), sizeof (object->value->value.ip_value),
                                        ASN_IPADDRESS, WATCHER_FIXED_SIZE);
        return netsnmp_register_watched_instance(reginfo, watcher_info);
        break;

    case VSA_ASN_NETWORK_ADDRESS:
        reginfo =
            netsnmp_create_handler_registration(vsa_oid_to_str(object->tree), NULL, object->tree->oids,
                                                object->tree->len, HANDLER_CAN_RWRITE);
        watcher_info =
            netsnmp_create_watcher_info(object->value->value.hex_value.values, object->value->value.hex_value.len,
                                        ASN_IPADDRESS, WATCHER_FIXED_SIZE);
        return netsnmp_register_watched_instance(reginfo, watcher_info);
        break;

    case VSA_ASN_OCTET_STRING:
    case VSA_ASN_STRING:
        reginfo =
            netsnmp_create_handler_registration(vsa_oid_to_str(object->tree), NULL, object->tree->oids,
                                                object->tree->len, HANDLER_CAN_RWRITE);
        watcher_info =
            netsnmp_create_watcher_info(object->value->value.string_value, strlen(object->value->value.string_value),
                                        ASN_OCTET_STR, WATCHER_MAX_SIZE);
        return netsnmp_register_watched_instance(reginfo, watcher_info);
        break;

    case VSA_ASN_OID:
        reginfo =
            netsnmp_create_handler_registration(vsa_oid_to_str(object->tree), NULL, object->tree->oids,
                                                object->tree->len, HANDLER_CAN_RWRITE);
        watcher_info =
            netsnmp_create_watcher_info(object->value->value.oid_value->oids, object->value->value.oid_value->len,
                                        ASN_OBJECT_ID, WATCHER_MAX_SIZE);
        return netsnmp_register_watched_instance(reginfo, watcher_info);
        break;

    case VSA_ASN_TIMETICKS:
        reginfo =
            netsnmp_create_handler_registration(vsa_oid_to_str(object->tree), NULL, object->tree->oids,
                                                object->tree->len, HANDLER_CAN_RWRITE);
        watcher_info =
            netsnmp_create_watcher_info(&object->value->value.ulong_value, sizeof (object->value->value.ulong_value),
                                        ASN_TIMETICKS, WATCHER_MAX_SIZE);
        return netsnmp_register_watched_instance(reginfo, watcher_info);
        break;

    default:
        break;
    }

    return -1;
}
