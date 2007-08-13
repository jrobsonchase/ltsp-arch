/*
 * ldm.c
 * LTSP display manager.
 * Manages spawning a session to a server.
 *
 * (c) Scott Balneaves, sbalneav@ltsp.org
 *
 * This software is licensed under the GPL v2 or later.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <sys/wait.h>
#include <glib.h>

#include "ldm.h"

void
spawn_greeter()
{
    char *greet[] = {
        ldminfo.greeter_prog,
        NULL};

    g_spawn_async_with_pipes(NULL, greet, NULL,
                             G_SPAWN_DO_NOT_REAP_CHILD, NULL, NULL,
                             &ldminfo.greeterpid,
                             &ldminfo.greeterwfd,
                             &ldminfo.greeterrfd,
                             NULL, NULL);
}
    
int
get_greeter_string(char *str, int len)
{
    char *p = str;
    int i = 0;
    int ret;

    while(TRUE) {
        if (i == (len - 1))
            break;
        ret = read(ldminfo.greeterrfd, p, 1);
        if (ret < 0)
            return 1;
        if (*p == '\n')
            break;
        p++;
        i++;
    }

    *p = '\0';

    return 0;
}


int
get_userid(char *str, int len)
{
    char *prompt = "prompt <b>Username</b>\n";
    char *p;

    fprintf(ldmlog, "In get_userid\n");
    if (p = getenv("LDM_USERNAME")) {
        scopy(ldminfo.username, p);
        return 0;
    } else {
        write(ldminfo.greeterwfd, prompt, strlen(prompt));
        return get_greeter_string(ldminfo.username, sizeof ldminfo.username);
    }
}
    
int
get_passwd()
{
    char *prompt = "prompt <b>Password</b>\n";
    char *pw = "passwd\n";
    char *p;

    fprintf(ldmlog, "In get_passwd\n");
    if (p = getenv("LDM_PASSWORD")) {
        scopy(ldminfo.password, p);
        return 0;
    } else {
        write(ldminfo.greeterwfd, prompt, strlen(prompt));
        write(ldminfo.greeterwfd, pw, strlen(pw));
        return get_greeter_string(ldminfo.password, sizeof ldminfo.password);
    }
}

void
set_message(char *message)
{
    char password[255];
    char *prompt = "msg ";
    char *pw = "\n";
    char *p;

    write(ldminfo.greeterwfd, prompt, strlen(prompt));
    write(ldminfo.greeterwfd, message, strlen(message));
    write(ldminfo.greeterwfd, pw, strlen(pw));
}

int
get_host()
{
    char *cmd = "hostname\n";

    write(ldminfo.greeterwfd, cmd, strlen(cmd));
    return get_greeter_string(ldminfo.server, sizeof ldminfo.server);
}

int
get_language()
{
    char *cmd = "language\n";
    char lang[LDMSTRSZ];
    int status;

    write(ldminfo.greeterwfd, cmd, strlen(cmd));
    status =  get_greeter_string(lang, sizeof lang);
    if (*(ldminfo.lang) != '\0')
        return 0;                           /* admin has set LDM_LANGUAGE */
    if (strncmp(lang, "None", 4))          /* If "None", use default */
        scopy(ldminfo.lang, lang);
    return status;
}

int
get_session()
{
    char *cmd = "session\n";
    char session[LDMSTRSZ];
    int status;

    write(ldminfo.greeterwfd, cmd, strlen(cmd));
    status =  get_greeter_string(session, sizeof session);
    if (strncmp(session, "None", 4))        /* If "None", use default */
        scopy(ldminfo.session, session);
    return status;
}

void
close_greeter()
{
    kill(ldminfo.greeterpid, SIGTERM);
    ldm_wait(ldminfo.greeterpid);
    close(ldminfo.greeterrfd);
    close(ldminfo.greeterwfd);
    ldminfo.greeterrfd = 0;
    ldminfo.greeterwfd = 0;
    ldminfo.greeterpid = 0;
}
