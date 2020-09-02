#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "my_socket.h"
#include "my_signal.h"
#include "raw_data.h"

int debug = 0;

/* command line interface */
int usage()
{
    char msg[] = "daqmw-tc-server [-d] [-h] [-p port] [-t const|rec|sin] [-r hz]\n"
                 "-d: debug\n"
                 "-h: print help and exit\n"
                 "-p: port\n"
                 "-t: data type.\n"
                 "    const: constant\n"
                 "    rec:   rectangular\n"
                 "    sin:   sine curve\n"
                 "    default is constant\n"
                 "-r: rate (in Hz)\n"
                 "    default is 1 Hz\n";
    fprintf(stderr, "%s", msg);
    return 0;
}

void sig_chld(int signo)
{
    pid_t pid;
    int   stat;

    while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        ;
    }
    return;
}

int child_proc(int sockfd, int data_rate)
{
    struct raw_data raw_data;

    if (debug) {
        fprintf(stderr, "child_proc\n");
    }

    useconds_t usleep_time = 1000000 / data_rate;
    unsigned int trigger_count = 0;
    for ( ; ; ) {
        raw_data = create_data(trigger_count);
        int n = write(sockfd, &raw_data, sizeof(raw_data));
        if (n < 0) {
            if (errno == ECONNRESET || errno == EPIPE) {
                if (debug) {
                    fprintf(stderr, "daqmw-tc-server: connection reset by client (not an error)\n");
                }
                break; /* exit from for ( ; ; ) loop */
            }
            err(1, "write error in child_proc");
        }
        if (n == 0) {
            err(1, "write returns 0");
        }
        usleep(usleep_time);
        trigger_count += 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int c;
    int port = 1234;
    int data_rate = 1;
    struct sockaddr_in remote;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    while ( (c = getopt(argc, argv, "dhp:t:")) != -1) {
        switch (c) {
            case 'd':
                debug = 1;
                break;
            case 'h':
                usage();
                exit(0);
                break;
            case 'p':
                port = strtol(optarg, NULL, 0);
                break;
            case 'r':
                data_rate = strtol(optarg, NULL, 0);
                break;
            default:
                break;
        }
    }
    argc -= optind;
    argv += optind;

    my_signal(SIGCHLD, sig_chld);
    my_signal(SIGPIPE, SIG_IGN);

    int listenfd = tcp_listen(port);
    if (listenfd < 0) {
        errx(1, "tcp_listen");
    }

    for ( ; ; ) {
        int connfd = accept(listenfd, (struct sockaddr *)&remote, &addr_len);
        if (connfd < 0) {
            err(1, "accept");
        }
        
        int pid = fork();
        if (pid == 0) { /* child */
            if (close(listenfd) < 0) {
                err(1, "close listenfd");
            }
            if  (child_proc(connfd, data_rate) < 0) {
                errx(1, "child_proc");
            }
            exit(0);
        }
        else { /* parent */
            if (close(connfd) < 0) {
                err(1, "close for accept socket of parent");
            }
        }
    }

    return 0;
}
