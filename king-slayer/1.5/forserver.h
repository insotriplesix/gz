#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#define MSG_HEADER_SIZE sizeof(int) * 2

/*
 * Messaging stuff.
 */

struct Message {
    int type;
    int size;
    char m[0];
};

void send_msg(int sock, int type, const char *msg) {
    struct Message *m;
    int size = 0, stat;
    for (; msg[size] != '\0'; ++size);

    m = (struct Message *) malloc (MSG_HEADER_SIZE + size);
    memset(m, 0, MSG_HEADER_SIZE + size);

    m->type = type;
    m->size = size;
    strncpy(m->m, msg, size);

    stat = send(sock, m, MSG_HEADER_SIZE + size, 0);
    if (stat == ECONNRESET)
        return;
    free(m);
}

/*
 * Non-blocking terminal input.
 */
 
#define NB_ENABLE   1
#define NB_DISABLE  0
 
void nonblock(int state) {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    if (state == NB_ENABLE) {
        ttystate.c_lflag &= ~ECHO;
        ttystate.c_lflag &= ~ICANON;
        ttystate.c_cc[VMIN] = 1;
    } else if (state == NB_DISABLE) {
        ttystate.c_lflag |= ECHO;
        ttystate.c_lflag |= ICANON;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

int key_hit() {
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);
    select(STDIN_FILENO + 1, &rfds, NULL, NULL, NULL);
    return FD_ISSET(STDIN_FILENO, &rfds);
}

/*
 * Accept w/ timeout.
 */

int accept_timeout(int sock, int timeout) {
    int retval;
    struct timeval tv;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(sock, &rfds);
    
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    
    retval = select(sock + 1, &rfds, NULL, NULL, &tv);
    if (retval > 0)
        return accept(sock, NULL, NULL);
    return -1;
}

/*
 * Let's check if connection is alive.
 */
 
int is_socket_alive_write(int socket) {
    struct sigaction new_actn, old_actn;
    new_actn.sa_handler = SIG_IGN;
    sigemptyset(&new_actn.sa_mask);
    new_actn.sa_flags = 0;
    sigaction(SIGPIPE, &new_actn, &old_actn);
    
    int stat = write(socket, NULL, 0);
    
    sigaction(SIGPIPE, &old_actn, NULL);
    
    if (stat < 0)
        return 0;
    return 1;
}

/*
 * Useful outputs.
 */

const char* card_name(int card) {
    switch (card) {
        case 0:
            return "KING";
        case 1:
            return "SLAYER";
        case 2:
        case 10:
            return "GUARD";
        default:
            return "UNKNOWN";
    }
}

const char* pseudonym[8] = {
    "ALPHA", "BRAVO", "CHARLIE", "DELTA",
    "ECHO", "FOXTROT", "GOLF", "HOTEL"
};

const char* server_name(int id) {
    if (id >= 0 && id <= 7)
        return pseudonym[id];
    return "UNKNOWN";
}
