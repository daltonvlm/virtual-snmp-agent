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

#ifndef VSA_PARSER_H
#define VSA_PARSER_H

#include <glib.h>

#define VSA_PARSER_PARSE_HEX_VALUES_ERROR_MSG "vsa_parser_parse_hex_values() failed"
#define VSA_PARSER_PARSE_NUMBER_ERROR_MSG "vsa_parser_parse_number() failed"
#define VSA_PARSER_PARSE_OID_ERROR_MSG "vsa_parser_parse_oid() failed"
#define VSA_PARSER_PARSE_MIB_ERROR_MSG "vsa_parser_parse_mib() failed"

unsigned char          *vsa_parser_parse_hex_values(const char *str, size_t *len);
int                     vsa_parser_parse_number(const char *str, unsigned long *pvalue);
oid                    *vsa_parser_parse_oid(const char *str, size_t *len);
GList                  *vsa_parser_parse_mib(const char *mib_name);

#endif // VSA_PARSER_H
