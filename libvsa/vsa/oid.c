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
#include <stdio.h>
#include <stdlib.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include <vsa/log.h>
#include <vsa/oid.h>

vsa_oid_t              *
vsa_oid_new(oid * oids, size_t len)
{
    vsa_oid_t              *tree;

    tree = calloc(1, sizeof (vsa_oid_t));
    if (!tree) {
        vsa_log_debugln("%s", strerror(errno));
        return NULL;
    }
    tree->oids = oids;
    tree->len = len;

    return tree;
}

void                   *
vsa_oid_free(vsa_oid_t * tree)
{
    if (!tree) {
        return NULL;
    }
    free(tree->oids);
    free(tree);

    return NULL;
}

char                   *
vsa_oid_to_str(vsa_oid_t * tree)
{
    char                   *str;

    str = NULL;
    for (size_t i = 0; i < tree->len; i++) {
        char                   *tmp;

        tmp = str;
        if (-1 == asprintf(&str, "%s.%lu", str ? str : "", tree->oids[i])) {
            vsa_log_debugln("%s", VSA_LOG_AS_PRINTF_ERROR_MSG);
            free(tmp);
            return NULL;
        }
        free(tmp);
    }

    return str;
}
