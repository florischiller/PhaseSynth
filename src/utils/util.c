#include <stdio.h>
#include <portaudio.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <execinfo.h>

#include "util.h"
#include "../params.h"

void signalHandler(int, siginfo_t *, void *);

int handle_pa_error(PaError err) {
    printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );

    return 0;
}

void signalHandler (int signo, siginfo_t *si, void *data) {
    if (signo == SIGFPE) {
        switch (si->si_code) {
            case FPE_FLTDIV:
                printf ("Got FPE_FLTDIV PID: %ld, UID: %ld\n", (long)si->si_pid, (long)si->si_uid);
                break;
            case FPE_FLTOVF:
                printf ("Got FPE_FLTOVF PID: %ld, UID: %ld\n", (long)si->si_pid, (long)si->si_uid);
                break;
            case FPE_INTDIV:
                printf ("Got FPE_INTDIV PID: %ld, UID: %ld, ADDR: %ld\n", (long)si->si_pid, (long)si->si_uid, (long)si->si_addr);
                break;
            case FPE_INTOVF:
                printf ("Got FPE_INTOVF PID: %ld, UID: %ld\n", (long)si->si_pid, (long)si->si_uid);
                break;
            default:
                printf ("Sending PID: %ld, UID: %ld, CODE: %d\n", (long)si->si_pid, (long)si->si_uid, si->si_code);
                backtrace_symbols_fd(&si->si_addr, 1, 1);
        }
    } else if (signo == SIGSEGV) {
        printf ("Got SIGSEGV PID: %ld, UID: %ld\n", (long)si->si_pid, (long)si->si_uid);
        backtrace_symbols_fd(&si->si_addr, 1, 1);
    } else {
        printf ("Got unknown exception signal PID: %ld, UID: %ld, CODE: %d\n", (long)si->si_pid, (long)si->si_uid, si->si_code);
        backtrace_symbols_fd(&si->si_addr, 1, 1);
    }

    exit(EXIT_FAILURE);
}

void startSignalHandler() {
    struct sigaction act;
	memset (&act, '\0', sizeof(act));
	/* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	act.sa_flags   = SA_SIGINFO;
	act.sa_handler = (void *) &signalHandler;
    sigaction(SIGFPE, &act, NULL);
    sigaction(SIGSEGV, &act, NULL);
}
