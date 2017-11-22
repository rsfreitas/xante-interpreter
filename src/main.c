
/*
 * Description: A libxanter application interpreter.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat May  6 11:43:37 2017
 * Project: xanter
 *
 * Copyright (C) 2017 Rodrigo Freitas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include <collections.h>
#include <libxante.h>

#include "xanter.h"

static void usage(void)
{
    printf("Usage: xanter [OPTIONS]\n");
    printf("A libxante application interpreter.\n\n");
    printf("Options:\n\n");
    printf("  -h, --help                 Shows this help screen.\n");
    printf("  -v, --version              Shows current xanter version.\n");
    printf("  -j [filename]              Indicates the JTF file pathname.\n");
    printf("  -t [filename]              Changes the UI theme.\n");
    printf("  -u [username]              Username to run the application.\n");
    printf("  -p [password]              The user password.\n");
    printf("  -T                         Puts the interpreter into the test mode.\n");
    printf("  -V                         Shows current application version.\n");
    printf("  -N                         Disables the user authentication.\n");
    printf("  -C, --config [pathname]    Creates the application configuration file with\n\
                                its default values, into JTF predefined path\n\
                                or in the specified path.\n\n");

    printf("  -d [path]                  Indicates the path to find the authentication\n\
                                database.\n\n");

    printf("  -D [path]                  Creates a database to use by applications.\n");
    printf("  -J [filename]              Creates the JXDB file from an application.\n");
    printf("  -S [session type]          Indicates the session type of the user login.\n");
    printf("  -M, --multi-instance       Enables the multi-instance feature of the\n\
                                application.\n");

    printf("\n");
    printf("Details:\n\n");
    printf("Session types\n"
           "\t%d -> A continuous login, which will last for every application "
           "started\n\
            after this one.\n"
           "\t%d -> An individual login, which will cause a logout after "
           "closing the\n\
           application.\n", XANTE_SESSION_CONTINUOUS,
           XANTE_SESSION_SINGLE);

    printf("\n");
}

static void version(void)
{
    printf("xanter - Version %d.%d.%d %s\n", MAJOR_VERSION, MINOR_VERSION,
           RELEASE, (BETA == true) ? "beta" : "");
}

static void application_version(xante_t *xpp)
{
    char *version_str = NULL;

    version_str = xante_application_version(xpp);

    if (version_str != NULL) {
        printf("%s\n", version_str);
        free(version_str);
    }
}

static void error_msg(bool run_ui, const char *message)
{
    if (run_ui) {
        xante_dlg_messagebox(NULL, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             "%s", message);
    } else
        printf("%s\n", message);
}

static void create_xante_database(const char *pathname)
{
    cl_init(NULL);

    if (xante_auth_create_database(pathname, true))
        error_msg(false, xante_strerror(xante_get_last_error()));

    cl_uninit();
}

int main(int argc, char **argv)
{
    const char *opt = "hvj:Tt:u:p:VC::ND:J:d:S:M\0";
    struct option long_options[] = {
        { "config",         required_argument, 0, 'C' },
        { "help",           no_argument,       0, 'h' },
        { "version",        no_argument,       0, 'v' },
        { "multi-instance", no_argument,       0, 'M' },
        { 0,                0,                 0, 0   }
    };

    int option, return_value = -1;
    char *jtf_pathname = NULL, *username = NULL, *password = NULL,
         *jxdb_pathname = NULL;
    bool show_application_version = false, run_ui = true,
         create_config_file = false;
    xante_t *xpp = NULL;
    enum xante_session session = XANTE_SESSION_SINGLE;
    enum xante_init_flags flags = XANTE_USE_AUTH | XANTE_USE_MODULE |
                                  XANTE_SINGLE_INSTANCE;

    do {
        option = getopt_long(argc, argv, opt, long_options, NULL);

        switch (option) {
            case 'h': /* usage */
                usage();
                return 1;

            case 'v': /* version */
                version();
                return 2;

            case 'j': /* JTF file */
                jtf_pathname = strdup(optarg);
                break;

            case 'T': /* test mode */
                /* Test mode means the we're going to disable all plugin calls. */
                flags &= ~XANTE_USE_MODULE;
                break;

            case 't': /* theme file */
                xante_env_set_theme(optarg);
                break;

            case 'u': /* username */
                username = strdup(optarg);
                break;

            case 'p': /* password */
                password = strdup(optarg);
                break;

            case 'V': /* application version */
                show_application_version = true;
                flags &= ~XANTE_USE_MODULE;
                break;

            case 'C': /* create default configuration file */
                create_config_file = true;
                run_ui = false;
                flags &= ~XANTE_USE_AUTH;
                flags &= ~XANTE_USE_MODULE;

                if (optarg != NULL)
                    xante_env_set_cfg_path(optarg);

                break;

            case 'N': /* disable database authentication */
                flags &= ~XANTE_USE_AUTH;
                break;

            case 'D': /* create a default (empty) database */
                create_xante_database(optarg);
                return 3;

            case 'J': /* create an intermediate JXDBI file */
                jxdb_pathname = strdup(optarg);
                run_ui = false;
                flags &= ~XANTE_USE_MODULE;
                flags &= ~XANTE_USE_AUTH;
                break;

            case 'd': /* database path */
                xante_env_set_auth_path(optarg);
                break;

            case 'S': /* session type */
                session = atoi(optarg);
                break;

            case 'M': /* disable single instance */
                flags &= ~XANTE_SINGLE_INSTANCE;
                break;

            case '?':
                return -1;
        }
    } while (option != -1);

    if (NULL == jtf_pathname) {
        error_msg(run_ui, cl_tr("A JTF file name should be passed to the "
                                "interpreter."));

        return -1;
    }

    if ((flags & XANTE_USE_AUTH) == XANTE_USE_AUTH) {
        if ((NULL == username) || (NULL == password)) {
            error_msg(run_ui, cl_tr("A username/password must be used to "
                                    "run the application!"));

            return -1;
        }
    }

    xpp = xante_init(argv[0], jtf_pathname, flags, session, username, password);

    if (NULL == xpp) {
        error_msg(run_ui, xante_strerror(xante_get_last_error()));

        /*
         * We need to manually set the return value since the library was not
         * initialized and we can't obtain its exit_value.
         */
        return_value = -1;
        goto release_block;
    }

    if (show_application_version == true) {
        application_version(xpp);
        goto end_block;
    }

    if (jxdb_pathname != NULL) {
        if (xante_auth_export_jxdbi(xpp, jxdb_pathname) < 0)
            error_msg(run_ui, xante_strerror(xante_get_last_error()));

        goto end_block;
    }

    if (create_config_file == true) {
        /*
         * We're going to just create the file. And to do so we force a few
         * internal flags.
         */
        xante_runtime_set_force_config_file_saving(xpp, true);
        xante_runtime_set_show_config_saving_question(xpp, false);
    }

    xante_config_load(xpp);

    if (run_ui == true)
        xante_ui_run(xpp);

    xante_config_write(xpp);

end_block:
    return_value = xante_runtime_exit_value(xpp);
    xante_uninit(xpp);

release_block:
    if (jxdb_pathname != NULL)
        free(jxdb_pathname);

    if (jtf_pathname != NULL)
        free(jtf_pathname);

    if (username != NULL)
        free(username);

    if (password != NULL)
        free(password);

    return return_value;
}

