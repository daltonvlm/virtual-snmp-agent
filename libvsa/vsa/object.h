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

#ifndef VSA_OBJECT_H
#define VSA_OBJECT_H

#include <vsa/oid.h>
#include <vsa/value.h>

#define VSA_OBJECT_NEW_ERROR_MSG "vsa_object_new() failed"
#define VSA_OBJECT_TO_STR_ERROR_MSG "vsa_object_to_str() failed"
#define VSA_OBJECT_REGISTER_ERROR_MSG "vsa_object_register() failed"

typedef struct vsa_object_s vsa_object_t;

struct vsa_object_s {
    vsa_oid_t              *tree;
    vsa_value_t            *value;
};

vsa_object_t           *vsa_object_new(vsa_oid_t * tree, vsa_value_t * value);
void                   *vsa_object_free(vsa_object_t * object);
char                   *vsa_object_to_str(vsa_object_t * object);
int                     vsa_object_register(vsa_object_t * object);

#endif // VSA_OBJECT_H
