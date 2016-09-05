/*
    minit - minimalist init implementation for containers
            <https://github.com/chazomaticus/minit>
    Copyright 2014, 2015 Charles Lindsay <chaz@chazomatic.us>

    This software is provided 'as-is', without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from
    the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software in
       a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#if _POSIX_C_SOURCE < 199309L
#error minit requires _POSIX_C_SOURCE >= 199309
#endif

#ifndef DEFAULT_STARTUP
#define DEFAULT_STARTUP "/etc/minit/startup"
#endif
#ifndef DEFAULT_SHUTDOWN
#define DEFAULT_SHUTDOWN "/etc/minit/shutdown"
#endif


static const char *const default_startup = DEFAULT_STARTUP;
static const char *const default_shutdown = DEFAULT_SHUTDOWN;


static void wait_for_child(pid_t child_pid) {
    for(
        pid_t pid;
        ((pid = wait(NULL)) != -1 || errno != ECHILD) && pid != child_pid;
    )
        ;
}

static void wait_for_termination(void) {
    sigset_t receive_set;
    sigemptyset(&receive_set);
    sigaddset(&receive_set, SIGCHLD);
    sigaddset(&receive_set, SIGTERM);
    sigaddset(&receive_set, SIGINT);

    // Wait for a termination signal, ignoring EINTR.
    for(int signal; (signal = sigwaitinfo(&receive_set, NULL)) != SIGTERM
            && signal != SIGINT; ) {
        while(waitpid(-1, NULL, WNOHANG) > 0)
            ;
    }
}

static pid_t run(const char *filename, sigset_t child_mask) {
    pid_t pid = fork();
    if(pid == -1)
        perror("minit: fork");

    if(pid == 0) {
        sigprocmask(SIG_SETMASK, &child_mask, NULL);
        execlp(filename, filename, NULL);

        // Ignore "no such file" errors unless specified by caller.
        if((filename == default_startup || filename == default_shutdown)
                && errno == ENOENT)
            exit(0);
        perror(filename);
        exit(1);
    }

    return pid;
}

static sigset_t block_signals(void) {
    sigset_t block_set;
    sigfillset(&block_set);

    sigset_t old_mask;
    sigprocmask(SIG_SETMASK, &block_set, &old_mask);

    return old_mask;
}

int main(int argc, char *argv[]) {
    sigset_t default_mask = block_signals();

    const char *startup = (argc > 1 && *argv[1] ? argv[1] : default_startup);
    const char *shutdown = (argc > 2 && *argv[2] ? argv[2] : default_shutdown);

    run(startup, default_mask);

    wait_for_termination();

    wait_for_child(run(shutdown, default_mask));

    // If we're running as a regular process (not init), don't kill -1.
    if(getpid() == 1)
        kill(-1, SIGTERM);
    while(wait(NULL) != -1 || errno != ECHILD)
        ;

    return 0;
}
