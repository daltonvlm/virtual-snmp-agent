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

#ifndef VSA_OID_H
#define VSA_OID_H

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#define VSA_OID_NEW_ERROR_MSG "vsa_oid_new() failed"
#define VSA_OID_TO_STR_ERROR_MSG "vsa_oid_to_str() failed"

typedef struct vsa_oid_s vsa_oid_t;

struct vsa_oid_s {
    oid                    *oids;
    size_t                  len;
};

vsa_oid_t              *vsa_oid_new(oid * oids, size_t len);
void                   *vsa_oid_free(vsa_oid_t * tree);
char                   *vsa_oid_to_str(vsa_oid_t * tree);

#endif // VSA_OID_H
