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

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#include <vsa/log.h>
#include <vsa/object.h>
#include <vsa/oid.h>
#include <vsa/parser.h>

#define VSA_PARSER_PARSE_OID_GET_IDX_ERROR_MSG "vsa_parser_parse_oid_get_idx() failed"
#define VSA_PARSER_MAKE_OBJECT_ERROR_MSG "vsa_parser_make_object() failed"
#define VSA_PARSER_RSTRIP_ERROR_MSG "vsa_parser_rstrip() failed"
#define VSA_PARSER_FEED_ERROR_MSG "vsa_parser_feed() failed"
#define VSA_PARSER_APPEND_ERROR_MSG "vsa_parser_append() failed"

#define VSA_PARSER_CANNOT_PARSE_LINE_ERROR_MSG "%s: %u: can't parse line"

#define VSA_PARSER_LINE_PATTERN\
    "((?<!\\d)\\.?(?:1|iso)(?:\\.\\d+)+)"\
    "[ =:-]*"\
    "([^:=\"]+)"\
    "[ =:]*"\
    "(.*)"

typedef struct vsa_parser_s vsa_parser_t;

struct vsa_parser_s {
    char                   *oid;
    char                   *type;
    char                   *value;
};

static int              vsa_parser_parse_oid_get_idx(const char *index, oid * poid);
static vsa_object_t    *vsa_parser_make_object(vsa_parser_t * parser);
static char            *vsa_parser_rstrip(const char *str);
static vsa_parser_t    *vsa_parser_cleanup(vsa_parser_t * parser);
static vsa_parser_t    *vsa_parser_feed(vsa_parser_t * parser, const char *oid, const char *type, const char *value);
static vsa_parser_t    *vsa_parser_append(vsa_parser_t * parser, const char *value);
static void             vsa_object_free_cb(void *data);

unsigned char          *
vsa_parser_parse_hex_values(const char *str, size_t *len)
{
    char                   *p;
    unsigned char           value, *values;
    size_t                  len_aux;

    len_aux = 0;
    values = NULL;
    do {
        value = (unsigned char) strtoul(str, &p, 16);
        if (errno) {
            vsa_log_debugln("%s", strerror(errno));
            free(values);
            return NULL;
        }
        if (p == str) {
            break;
        }
        values = realloc(values, ++len_aux * sizeof (*values));
        if (!values) {
            vsa_log_debugln("%s", strerror(errno));
            return NULL;
        }
        values[len_aux - 1] = value;
        str = ++p;
    } while (1);

    *len = len_aux;

    return values;
}

int
vsa_parser_parse_number(const char *str, unsigned long *pvalue)
{
    char                   *p;

    while (*str) {
        *pvalue = strtoul(str, &p, 10);
        if (errno) {
            vsa_log_debugln("%s", strerror(errno));
            return -1;
        }
        if (p != str) {
            return 0;
        }
        str = ++p;
    }

    return -1;
}

static int
vsa_parser_parse_oid_get_idx(const char *index, oid * poid)
{
    char                   *p;
    int                     iso_cmp, is_digit;

    iso_cmp = strncasecmp(index, "iso", 3);
    is_digit = isdigit(*index);

    if (iso_cmp && !is_digit) {
        return -1;
    }

    if (!iso_cmp) {
        *poid = 1;
        return 0;
    }

    *poid = strtoul(index, &p, 10);
    if (errno || p == index) {
        if (errno) {
            vsa_log_debugln("%s", strerror(errno));
        }
        return -1;
    }

    return 0;
}

oid                    *
vsa_parser_parse_oid(const char *str, size_t *len)
{
    char                   *str_cpy, *aux, *p;
    size_t                  len_aux;
    oid                    *oids;

    str_cpy = strdup(str);
    if (!str_cpy) {
        vsa_log_debugln("%s", strerror(errno));
        return NULL;
    }

    oids = NULL;
    len_aux = 0;
    aux = str_cpy;
    while ((p = strtok(aux, "."))) {
        oid                     idx;

        aux = NULL;

        if (vsa_parser_parse_oid_get_idx(p, &idx)) {
            vsa_log_debugln(VSA_PARSER_PARSE_OID_GET_IDX_ERROR_MSG);
            free(str_cpy);
            free(oids);
            return NULL;
        }

        len_aux++;
        oids = realloc(oids, len_aux * sizeof (oid));
        if (!oids) {
            vsa_log_debugln("%s", strerror(errno));
            free(str_cpy);
            return NULL;
        }
        oids[len_aux - 1] = idx;
    }
    free(str_cpy);
    *len = len_aux;

    return oids;
}

static vsa_object_t    *
vsa_parser_make_object(vsa_parser_t * parser)
{
    char                   *rvalue;
    size_t                  len;
    oid                    *oids;
    vsa_asn_type_t          type;
    vsa_oid_t              *tree;
    vsa_object_t           *object;
    vsa_value_t            *value;

    oids = vsa_parser_parse_oid(parser->oid, &len);
    if (!oids) {
        vsa_log_debugln(VSA_PARSER_PARSE_OID_ERROR_MSG);
        return NULL;
    }

    tree = vsa_oid_new(oids, len);
    if (!tree) {
        vsa_log_debugln(VSA_OID_NEW_ERROR_MSG);
        free(oids);
        return NULL;
    }

    type = vsa_asn_type_from_str(parser->type);
    if (VSA_ASN_UNKNOWN == type) {
        vsa_log_debugln(VSA_ASN_TYPE_FROM_STR_ERROR_MSG);
        vsa_oid_free(tree);
        return NULL;
    }

    rvalue = vsa_parser_rstrip(parser->value);
    if (!rvalue) {
        vsa_log_debugln(VSA_PARSER_RSTRIP_ERROR_MSG);
        vsa_oid_free(tree);
        return NULL;
    }

    value = vsa_value_new(type, rvalue);
    if (!value) {
        vsa_log_debugln(VSA_VALUE_NEW_ERROR_MSG);
        free(rvalue);
        vsa_oid_free(tree);
        return NULL;
    }

    object = vsa_object_new(tree, value);
    if (!object) {
        vsa_log_debugln(VSA_OBJECT_NEW_ERROR_MSG);
        vsa_value_free(value);
        free(rvalue);
        vsa_oid_free(tree);
        return NULL;
    }
    free(rvalue);

    return object;
}

static char            *
vsa_parser_rstrip(const char *str)
{
    char                   *p, *aux;
    size_t                  len;

    aux = strdup(str);
    if (!aux) {
        vsa_log_debugln("%s", strerror(errno));
        return NULL;
    }

    len = strlen(aux);
    p = aux + len;
    while (--p >= aux && isspace(*p)) {
        *p = '\0';
    }

    return aux;
}

static vsa_parser_t    *
vsa_parser_cleanup(vsa_parser_t * parser)
{
    free(parser->oid), parser->oid = NULL;
    free(parser->type), parser->type = NULL;
    free(parser->value), parser->value = NULL;

    return parser;
}

static vsa_parser_t    *
vsa_parser_feed(vsa_parser_t * parser, const char *oid, const char *type, const char *value)
{
    vsa_parser_cleanup(parser);

    parser->oid = strdup(oid);
    parser->type = strdup(type);
    parser->value = strdup(value);

    if (!parser->oid || !parser->type || !parser->value) {
        vsa_log_debugln("%s", strerror(errno));
        return NULL;
    }

    return parser;
}

static vsa_parser_t    *
vsa_parser_append(vsa_parser_t * parser, const char *value)
{
    char                   *tmp;

    tmp = parser->value;
    if (-1 == asprintf(&parser->value, "%s%s", tmp, value)) {
        vsa_log_debugln("%s", VSA_LOG_AS_PRINTF_ERROR_MSG);
        return NULL;
    }
    free(tmp);

    return parser;
}

static void
vsa_object_free_cb(void *data)
{
    vsa_object_free((vsa_object_t *) data);
}

GList                  *
vsa_parser_parse_mib(const char *mib_name)
{
    char                   *line;
    unsigned                lineno;
    size_t                  len;
    FILE                   *mib;
    gchar                 **matches;
    GError                 *gerror;
    GList                  *objects;
    GMatchInfo             *match_info;
    static GRegex          *gregex;
    vsa_object_t           *object;
    vsa_parser_t            parser = { NULL, NULL, NULL };

    line = NULL;
    mib = NULL;
    matches = NULL;
    objects = NULL;
    match_info = NULL;
    object = NULL;

    gerror = NULL;
    if (!gregex) {
        gregex = g_regex_new(VSA_PARSER_LINE_PATTERN, G_REGEX_CASELESS | G_REGEX_EXTENDED, 0, &gerror);
        if (!gregex) {
            vsa_log_debugln("%s", gerror->message);
            g_error_free(gerror);
            goto cleanup_and_exit_error;
        }
    }

    mib = fopen(mib_name, "r");
    if (!mib) {
        vsa_log_debugln("%s", strerror(errno));
        goto cleanup_and_exit_error;
    }

    len = 0;
    lineno = 1;
    while (-1 != getline(&line, &len, mib)) {

        if (g_regex_match(gregex, line, 0, &match_info)) {
            matches = g_match_info_fetch_all(match_info);
            if (parser.oid) {
                object = vsa_parser_make_object(&parser);
                if (!object) {
                    vsa_log_warnln("%s: %u: " VSA_PARSER_MAKE_OBJECT_ERROR_MSG, mib_name, lineno);
                } else {
                    objects = g_list_prepend(objects, object);
                }
                vsa_parser_cleanup(&parser);
            }
            if (!vsa_parser_feed(&parser, matches[1], matches[2], matches[3])) {
                vsa_log_debugln(VSA_PARSER_FEED_ERROR_MSG);
                goto cleanup_and_exit_error;
            }
            g_strfreev(matches), matches = NULL;
        } else if (parser.oid) {

            if (!vsa_parser_append(&parser, line)) {
                vsa_log_debugln(VSA_PARSER_APPEND_ERROR_MSG);
                goto cleanup_and_exit_error;
            }
        } else {
            vsa_log_warnln(VSA_PARSER_CANNOT_PARSE_LINE_ERROR_MSG, mib_name, lineno);
        }
        g_match_info_free(match_info), match_info = NULL;
        lineno++;
    }
    if (errno) {
        vsa_log_debugln("%s", strerror(errno));
        goto cleanup_and_exit_error;
    }
    free(line), line = NULL;

    if (parser.oid) {
        object = vsa_parser_make_object(&parser);
        if (!object) {
            vsa_log_warnln("%s: %u: " VSA_PARSER_MAKE_OBJECT_ERROR_MSG, mib_name, lineno);
        } else {
            objects = g_list_prepend(objects, object);
        }
        vsa_parser_cleanup(&parser);
    }

    fclose(mib), mib = NULL;

    return g_list_reverse(objects);

  cleanup_and_exit_error:
    free(line);
    if (mib) {
        fclose(mib);
    }
    if (matches) {
        g_strfreev(matches);
    }
    if (objects) {
        g_list_free_full(g_steal_pointer(&objects), vsa_object_free_cb);
    }
    if (match_info) {
        g_match_info_free(match_info);
    }
    if (object) {
        vsa_object_free(object);
    }
    return NULL;
}
