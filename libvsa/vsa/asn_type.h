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

#ifndef VSA_ASN_TYPE_H
#define VSA_ASN_TYPE_H

#define VSA_ASN_TYPE_FROM_STR_ERROR_MSG "vsa_asn_type_from_str() failed"
#define VSA_ASN_TYPE_TO_STR_ERROR_MSG "vsa_asn_type_to_str() failed"

typedef enum vsa_asn_type vsa_asn_type_t;

enum vsa_asn_type {
    VSA_ASN_UNKNOWN,
    VSA_ASN_BIT,
    VSA_ASN_COUNTER_32,
    VSA_ASN_COUNTER_64,
    VSA_ASN_GAUGE_32,
    VSA_ASN_HEX_STRING,
    VSA_ASN_INTEGER,
    VSA_ASN_IP_ADDRESS,
    VSA_ASN_NETWORK_ADDRESS,
    VSA_ASN_OCTET_STRING,
    VSA_ASN_OID,
    VSA_ASN_STRING,
    VSA_ASN_TIMETICKS
};

vsa_asn_type_t          vsa_asn_type_from_str(const char *str);
char                   *vsa_asn_type_to_str(vsa_asn_type_t type);

#endif // VSA_ASN_TYPE_H
