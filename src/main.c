
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

#include <collections.h>
#include <libxante.h>

#include "xanter.h"

static void usage(void)
{
    printf("Usage: xanter [OPTIONS]\n");
    printf("A libxante application interpreter.\n\n");
    printf("Options:\n");
    printf(" -h\t\tShows this help screen.\n");
    printf(" -v\t\tShows current xanter version.\n");
    printf(" -j [filename]\tIndicates the JTF file pathname.\n");
    printf(" -t [filename]\tChanges the UI theme.\n");
    printf(" -u [username]\tUsername to run the application.\n");
    printf(" -p [password]\tThe user password.\n");
    printf(" -T\t\tPuts the interpreter into the test mode.\n");
    printf(" -V\t\tShows current application version.\n");
    printf("\n");
}

static void version(void)
{
    printf("xanter - Version %d.%d.%d %s\n", MAJOR_VERSION, MINOR_VERSION,
           RELEASE, (BETA == true) ? "beta" : "");
}

int main(int argc, char **argv)
{
    const char *opt = "hvj:Tt:u:p:V\0";
    int option, ui_return = 0;
    char *jtf_pathname = NULL, *theme_pathname = NULL, *username = NULL,
         *password = NULL, *version_str = NULL;
    bool use_plugin = true, show_application_version = false;
    xante_t *xpp = NULL;

    do {
        option = getopt(argc, argv, opt);

        switch (option) {
            case 'h':
                usage();
                return 1;

            case 'v':
                version();
                return 2;

            case 'j':
                jtf_pathname = strdup(optarg);
                break;

            case 'T':
                /*
                 * Test mode means the we're going to disable all plugin calls.
                 */
                use_plugin = false;
                break;

            case 't':
                theme_pathname = strdup(optarg);
                break;

            case 'u':
                username = strdup(optarg);
                break;

            case 'p':
                password = strdup(optarg);
                break;

            case 'V':
                show_application_version = true;
                use_plugin = false;
                break;

            case '?':
                return -1;
        }
    } while (option != -1);

    /* TODO: This should be moved to the libxante library */
    cl_init(NULL);
    xante_theme_set(theme_pathname);

    if (NULL == jtf_pathname) {
        xante_dlg_messagebox(NULL, XANTE_MSGBOX_ERROR, 0, cl_tr("Error"),
                             cl_tr("A JTF file name should be passed to the "
                                   "interpreter."));

        return -1;
    }

    if ((NULL == username) || (NULL == password)) {
        xante_dlg_messagebox(NULL, XANTE_MSGBOX_ERROR, 0, cl_tr("Error"),
                             cl_tr("A username/password must be used to "
                                   "run the application!"));

        return -1;
    }

    xpp = xante_init(jtf_pathname, use_plugin, username, password);

    if (NULL == xpp) {
        xante_dlg_messagebox(NULL, XANTE_MSGBOX_ERROR, 0, cl_tr("Error"),
                             "%s", xante_strerror(xante_get_last_error()));

        return -1;
    }

    if (show_application_version == true) {
        version_str = xante_dlg_application_version(xpp);

        if (version_str != NULL) {
            printf("%s\n", version_str);
            free(version_str);
        }

        goto end_block;
    }

    xante_load_config(xpp);
    ui_return = xante_ui_run(xpp);
    xante_write_config(xpp, ui_return);

end_block:
    xante_uninit(xpp);
    cl_uninit();

    if (jtf_pathname != NULL)
        free(jtf_pathname);

    if (theme_pathname != NULL)
        free(theme_pathname);

    if (username != NULL)
        free(username);

    if (password != NULL)
        free(password);

    return 0;
}

