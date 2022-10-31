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

#include <config.h>

#include <error.h>
#include <getopt.h>
#include <stdlib.h>

#include <glib.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/mib_modules.h>

#include <vsa/log.h>
#include <vsa/object.h>
#include <vsa/parser.h>

#define VSA_FILE "VSA_FILE"

char                   *program_invocation_name = PACKAGE_NAME;

void                    object_register_cb(gpointer data, gpointer user_data);
char                   *parse_args(int argc, char *argv[]);
void                    usage(int status);
void                    run(int argc, char *argv[]);

void
object_register_cb(gpointer data, gpointer user_data)
{
    static char             buf[BUFSIZ];
    int                     nchars;
    static unsigned         count = 1;
    unsigned               *pnobjects;

    pnobjects = (unsigned *) user_data;

    nchars = snprintf(buf, sizeof (buf), "%u / %u registered", count, *pnobjects);
    vsa_log_info("%s\r", buf);

    if (-1 == vsa_object_register(data)) {
        char                   *object_str;

        object_str = vsa_object_to_str(data);
        if (!object_str) {
            vsa_log_warnln(VSA_OBJECT_TO_STR_ERROR_MSG);
        }
        vsa_log_warnln(VSA_OBJECT_REGISTER_ERROR_MSG " for '%s'", object_str ? object_str : "(?)");
    }

    if (count == *pnobjects) {
        memset(buf, ' ', nchars);
        vsa_log_infoln("done%s\r", buf);
    }
    count++;
}

char                   *
parse_args(int argc, char *argv[])
{
    char                    c, *mib;
    int                     index;

    struct option           options[] = {
        { "help", no_argument, NULL, 'h' },
        { "version", no_argument, NULL, 'v' },
        { NULL, 0, NULL, 0 }
    };

    mib = getenv(VSA_FILE);
    if (argc < 2 && !mib) {
        vsa_logln(stderr, "missing file name");
        usage(EXIT_FAILURE);
    }

    while ((c = getopt_long(argc, argv, ":hv", options, &index)) != -1) {
        switch (c) {
        case 'h':
            usage(EXIT_SUCCESS);
            break;

        case 'v':
            vsa_logln(stdout, PACKAGE_VERSION);
            exit(EXIT_SUCCESS);

        default:
            vsa_logln(stderr, "invalid option");
            exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        vsa_logln(stderr, "missing file name");
        exit(EXIT_FAILURE);
    }

    return argv[optind];
}

void
usage(int status)
{
    FILE                   *out;

    out = status ? stderr : stdout;

    /* *INDENT-OFF* */
    fprintf(out,
PACKAGE " - run virtual copies of SNMP agents from their SNMP walk outputs\n\n"

"    " PACKAGE " is part of " PACKAGE_FULL_NAME " toolset\n\n\n"


"Usage: " PACKAGE " [OPTION].. [FILE]\n\n"

"The " PACKAGE " program runs virtual copies of SNMP agents from their SNMP walk outputs. After parsing the file containing that\n"
"output, " PACKAGE " starts responding to queries on port 161. It also requires a " PACKAGE ".conf file following the same rules defined\n"
"by net-snmp for snmpd.conf. The search path for the " PACKAGE ".conf file is the same as for the snmpd.conf files.\n\n\n"


"OPTIONS\n\n"

"        -h, --help         Print this help message.\n"
"        -v, --version      Print version.\n\n\n"


"FILE is the name of the file that contains an SNMP walk output. The name can also be passed through the " VSA_FILE " environment\n"
"variable.\n\n\n"


PACKAGE_COPYRIGHT "\n\n"
            );
    /* *INDENT-ON* */
    exit(status);
}

void
run(int argc, char *argv[])
{
    char                   *mib;
    guint                   nobjects;
    GList                  *objects;

    mib = parse_args(argc, argv);

    objects = vsa_parser_parse_mib(mib);
    if (!objects) {
        vsa_log_errorln(VSA_LOG_INTERNAL_ERROR_MSG);
    }
    nobjects = g_list_length(objects);

    snmp_enable_stderrlog();
    init_agent(program_invocation_name);

    vsa_log_infoln("registering objects");
    g_list_foreach(objects, object_register_cb, &nobjects);

    init_snmp(program_invocation_name);
    if (init_master_agent()) {
        vsa_log_errorln("couldn't init master agent");
    }

    vsa_log_infoln("running");
    while (1) {
        agent_check_and_process(0);
    }
    snmp_shutdown(program_invocation_name);
}

int
main(int argc, char *argv[])
{
    run(argc, argv);
    return EXIT_SUCCESS;
}
