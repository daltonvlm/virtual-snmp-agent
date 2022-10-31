#
# Copyright (C) 2022 Dalton Martins <daltonvlm@gmail.com>
#
# This file is part of Virtual SNMP Agent.
#
# Virtual SNMP Agent is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Virtual SNMP Agent is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Virtual SNMP Agent.  If not, see <http://www.gnu.org/licenses/>.
#

m4_define([VSA_NAME], [vsa])
m4_define([VSA_FULL_NAME], [Virtual SNMP Agent])
m4_define([VSA_VERSION], [1.0])
m4_define([VSA_YEAR], [2022])
m4_define([VSA_AUTHOR], [Dalton Martins])
m4_define([VSA_EMAIL], [daltonvlm@gmail.com])
m4_define([VSA_COPYRIGHT], Copyright (C) VSA_YEAR VSA_AUTHOR (VSA_EMAIL))

AC_PREREQ([2.71])
AC_INIT(m4_defn([VSA_NAME]), m4_defn([VSA_VERSION]), m4_defn([VSA_EMAIL]))

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_SRCDIR([src/vsa.c])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile libvsa/vsa/Makefile src/Makefile pkgconfig/Makefile])
AC_CONFIG_FILES([pkgconfig/vsa.pc])

dnl Variables -------------------------------------------------------------------------------------
[VSA_FULL_NAME]="VSA_FULL_NAME"
AC_SUBST([VSA_FULL_NAME])

[VSA_COPYRIGHT]="VSA_COPYRIGHT"
AC_SUBST([VSA_COPYRIGHT])

VSA_CPPFLAGS="-D_GNU_SOURCE -Wall -Wextra -Werror"
AC_SUBST(VSA_CPPFLAGS)

VSA_DEPS="glib-2.0, netsnmp-agent"
AC_SUBST(VSA_DEPS)
dnl end Variables ---------------------------------------------------------------------------------

dnl Defines ---------------------------------------------------------------------------------------
AC_DEFINE([PACKAGE_FULL_NAME], "VSA_FULL_NAME", [Package Year])
AC_DEFINE([PACKAGE_COPYRIGHT], "VSA_COPYRIGHT", [Package Copyright])
dnl end Defines -----------------------------------------------------------------------------------

dnl Check --enable-docker -------------------------------------------------------------------------
AC_ARG_ENABLE([docker],
              [AS_HELP_STRING([--enable-docker], [run multiple agents in docker containers])],
              [docker_enabled=$enableval],
              [docker_enabled=no])

AS_IF([test "$docker_enabled" = "yes"],
      [
       AC_CHECK_PROG([DOCKER], [docker], [yes], [no])
       AS_IF([test "$DOCKER" == "no"], [AC_MSG_ERROR([docker required])])

       AC_CHECK_PROG([LSOF], [lsof], [yes], [no])
       AS_IF([test "$LSOF" == "no"], [AC_MSG_WARN([consider installing lsof in order to use vsa_docker_manager])])

       AC_CONFIG_FILES([docker/Makefile docker/vsa-docker-manager])
       DOCKER_DIR=docker
       AC_SUBST([DOCKER_DIR])
      ],
      []
     )
dnl end Check --enable-docker ---------------------------------------------------------------------

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_RANLIB

AC_CHECK_PROG([PKG_CONFIG], [pkg-config], [yes], [no])
AS_IF([test "$PKG_CONFIG" == "no"], [AC_MSG_ERROR([pkg-config required])])

# Checks for libraries.
PKG_CHECK_MODULES([VSA_DEPS], [$VSA_DEPS])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strdup strerror strncasecmp strtoul])

AM_PROG_AR
AM_INIT_AUTOMAKE([-Wall -Werror])

AC_OUTPUT
