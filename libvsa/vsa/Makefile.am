#
# Copyright (C) 2022 Dalton Martins <daltonvlm@gmail.com>
#
# This file is part of vsa.
#
# vsa is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# vsa is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with vsa.  If not, see <http://www.gnu.org/licenses/>.
#

pkginclude_HEADERS = asn_type.h log.h object.h oid.h parser.h value.h
lib_LIBRARIES = libvsa.a
libvsa_a_SOURCES = asn_type.c\
				   object.c\
				   oid.c\
				   parser.c\
				   value.c

AM_CPPFLAGS = $(VSA_CPPFLAGS) $(VSA_DEPS_CFLAGS) -I$(top_srcdir)/libvsa
