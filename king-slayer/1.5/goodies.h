#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <termios.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#include "forserver.h"

#define MAXBUFF 200

#define _CLRSCR     "\033[H\033[2J"
#define _GOTOXY     "\033[%d;%dH"
#define _BGCOLOR    "\033[4%dm"
#define _ENTERAM    "\033(0"
#define _EXITAM     "\033(B"
#define _ACSBOARD   "a"
#define _ACSVLINE   "x"
#define _ACSHLINE   "q"
#define _ACSVLINE_R "x"
#define _ACSHLINE_R "q"

#define KING   0
#define SLAVE  1
#define GUARD  2
#define BACK   3

int mute = 1, game_stage = 0;

enum colors {
    COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW,
    COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_GREY, COLOR_WHITE
};

enum keys {
    _UP, _DOWN, _LEFT, _RIGHT, _ENTER, _SPACE, _ESC, _BACKSPACE,
    _M, _N, _UNKNOWN, _TIMEOUT
};

struct termios rk_currentTermState;

int getscreensize(int *rows, int *cols) {
    struct winsize ws;
    if (ioctl(1, TIOCGWINSZ, &ws)) return -1;
    *rows = ws.ws_row;
    *cols = ws.ws_col;
    return 0;
}

int setbgcolor(enum colors color) {
    char buf[50];
    int len;

    if ((color < 0) || (color > 9)) return -1;
    len = sprintf(buf, _BGCOLOR, color);
    write(1, buf, len);

    return 0;
}

int gotoxy(int row, int col) {
    char buf[50];
    int len, rows, cols;

    if ((row <= 0) || (col <= 0)) return -1;
    if (getscreensize(&rows, &cols) || row > rows || col > cols)
        return -1;

    len = sprintf(buf, _GOTOXY, row, col);
    write(1, buf, len);
    return 0;
}

int clrscr(void) {
    write(1, _CLRSCR, strlen(_CLRSCR));
    return 0;
}

int readkey(enum keys *key) {
    char ch;
    
    if (!read(0, &ch, 1)) return -1;
    *key = ch;

    switch (ch) {
        case '\177': *key = _BACKSPACE; break;
        case '\n':   *key = _ENTER; break;
        case ' ':    *key = _SPACE; break;
        case 'm':    *key = _M; break;
        case 'n':    *key = _N; break;

        case '\033':
            *key = _ESC;
            if (!read(0, &ch, 1)) return -1;
            if (ch == '[') {
                *key = _UNKNOWN;
                if (!read(0, &ch, 1)) return -1;
                switch (ch) {
                    case 'A': *key = _UP;    break;
                    case 'B': *key = _DOWN;  break;
                    case 'C': *key = _RIGHT; break;
                    case 'D': *key = _LEFT;  break;
                    default: *key = _UNKNOWN;
                }
            }
        break;
    }
    return 0;
}

int readkey_timeout(enum keys *key, int lim) {
    char ch;
    int ready = 0;
    fd_set input_set;
    struct timeval timeout;

    FD_ZERO(&input_set);
    FD_SET(0, &input_set);
    timeout.tv_sec = lim;
    timeout.tv_usec = 0;
    
    ready = select(1, &input_set, NULL, NULL, &timeout);
    
    if (ready == -1) {
        *key = _TIMEOUT;
        return 0;
    }
    
    if (ready) {
        if (!read(0, &ch, 1)) return -1;
        *key = ch;

        switch (ch) {
            case '\177': *key = _BACKSPACE; break;
            case '\n':   *key = _ENTER; break;
            case ' ':    *key = _SPACE; break;
            case 'm':    *key = _M; break;
            case 'n':    *key = _N; break;

            case '\033':
                *key = _ESC;
                if (!read(0, &ch, 1)) return -1;
                if (ch == '[') {
                    *key = _UNKNOWN;
                    if (!read(0, &ch, 1)) return -1;
                    switch (ch) {
                        case 'A': *key = _UP;    break;
                        case 'B': *key = _DOWN;  break;
                        case 'C': *key = _RIGHT; break;
                        case 'D': *key = _LEFT;  break;
                        default: *key = _UNKNOWN;
                    }
                }
            break;
        }
    } else {
        *key = _TIMEOUT;
    }
    return 0;
}

int mytermsave() {
    if (tcgetattr(1, &rk_currentTermState) == -1) return -1;
    return 0;
}

int mytermrestore() {
    if (tcsetattr(1, TCSANOW, &rk_currentTermState) == -1) return -1;
    return 0;
}

int mytermregime(int regime, int vtime, int vmin, int echo, int sigint) {
    struct termios term;

    if ((regime > 1) || (regime < 0) || (echo > 1) ||
        (echo < 0) || (sigint > 1) || (sigint < 0))
        return 1;

    if ((vtime < 0) || (vmin < 0)) return -1;
    if (tcgetattr(1, &term) == -1) return -1;

    if (regime == 0) term.c_lflag &= (~ICANON);
    else term.c_lflag |= ICANON;

    if (echo == 0) term.c_lflag &= (~ECHO);
    else term.c_lflag |= ECHO;

    if (sigint == 0) term.c_lflag &= (~ISIG);
    else term.c_lflag |= ISIG;

    term.c_cc[VMIN] = vmin;
    term.c_cc[VTIME] = vtime;

    if (tcsetattr(1, TCSANOW, &term) == -1) return -1;

    return 0;
}

int printA(char str[], int symb) {
    char buf[50];
    int len;

    switch (symb) {
        case 0:
            len = sprintf(buf, "\033[30m\033[43m\033(0");
            write(1, buf, len);
            write(1, str, 1);
            len = sprintf(buf, "\033[0m\033(B");
            write(1, buf, len);
            break;
        case 1:
            len = sprintf(buf, "\033[30m\033[45m\033(0");
            write(1, buf, len);
            write(1, str, 1);
            len = sprintf(buf, "\033[0m\033(B");
            write(1, buf, len);
            break;
        case 2:
            len = sprintf(buf, "\033[44m\033(0");
            write(1, buf, len);
            write(1, str, 1);
            len = sprintf(buf, "\033[0m\033(B");
            write(1, buf, len);
            break;
        case 3:
            len = sprintf(buf, "\033[0m\033(0");
            write(1, buf, len);
            write(1, str, 1);
            len = sprintf(buf, "\033[0m\033(B");
            write(1, buf, len);
            break;
        case 4:
            len = sprintf(buf, "\033[33m\033(0");
            write(1, buf, len);
            write(1, str, 1);
            len = sprintf(buf, "\033[0m\033(B");
            write(1, buf, len);
            break;
        case 5:
            len = sprintf(buf, "\033[33m\033(0");
            write(1, buf, len);
            write(1, str, 1);
            len = sprintf(buf, "\033[0m\033(B");
            write(1, buf, len);
            break;
        case 6:
            len = sprintf(buf, "\033[35m\033(0");
            write(1, buf, len);
            write(1, str, 1);
            len = sprintf(buf, "\033[0m\033(B");
            write(1, buf, len);
            break;
        case 7:
            len = sprintf(buf, "\033[35m\033(0");
            write(1, buf, len);
            write(1, str, 1);
            len = sprintf(buf, "\033[0m\033(B");
            write(1, buf, len);
            break;
        case 8:
            len = sprintf(buf, "\033[35m\033(0");
            write(1, buf, len);
            write(1, str, 1);
            len = sprintf(buf, "\033[0m\033(B");
            write(1, buf, len);
            break;
        case 9:
            len = sprintf(buf, "\033[35m\033(0");
            write(1, buf, len);
            write(1, str, 1);
            len = sprintf(buf, "\033[0m\033(B");
            write(1, buf, len);
            break;
        case 10:
            len = sprintf(buf, "\033[35m\033(0");
            write(1, buf, len);
            write(1, str, 1);
            len = sprintf(buf, "\033[0m\033(B");
            write(1, buf, len);
            break;
        case 11:
            len = sprintf(buf, "\033[35m\033(0");
            write(1, buf, len);
            write(1, str, 1);
            len = sprintf(buf, "\033[0m\033(B");
            write(1, buf, len);
            break;
        case 12:
            len = sprintf(buf, "\033[33m\033(0");
            write(1, buf, len);
            write(1, str, 1);
            len = sprintf(buf, "\033[0m\033(B");
            write(1, buf, len);
            break;
        case 13:
            len = sprintf(buf, "\033[33m\033(0");
            write(1, buf, len);
            write(1, str, 1);
            len = sprintf(buf, "\033[0m\033(B");
            write(1, buf, len);
            break;
        case 14:
            len = sprintf(buf, "\033[33m\033(0");
            write(1, buf, len);
            write(1, str, 1);
            len = sprintf(buf, "\033[0m\033(B");
            write(1, buf, len);
            break;
        default:
            break;
    }
    return 0;
}

int board(int x1, int y1, int x2, int y2, int is_active) {
    int mx, my, i;
    
    if (getscreensize (&mx, &my)) return -1;
    
    for (i = 1; i <= x2; ++i) {
        gotoxy (x1 + i, y1);
        if (is_active) {
            char buf[32];
            write(1, buf, sprintf(buf, "\033[31m*\033[0m"));
        } else {
            printA(_ACSVLINE, 3);
        }
        gotoxy (x1 + i, y1 + y2 + 1);
        if (is_active) {
            char buf[32];
            write(1, buf, sprintf(buf, "\033[31m*\033[0m"));
        } else {
            printA(_ACSVLINE, 3);
        }
    }

    for (i = 1; i <= y2; ++i) {
        gotoxy (x1, y1 + i);
        if (is_active) {
            char buf[32];
            write(1, buf, sprintf(buf, "\033[31m*\033[0m"));
        } else {
            printA(_ACSHLINE, 3);
        }
        gotoxy (x1 + x2 + 1, y1 + i);
        if (is_active) {
            char buf[32];
            write(1, buf, sprintf(buf, "\033[31m*\033[0m"));
        } else {
            printA(_ACSHLINE, 3);
        }
    }
    return 0;
}    

int getbigcharpos(int big[2], int x, int y, int *value) {
    y = (7 - y);
    if (x < 4) {
        *value = (big[0] >> (x * 8 + y)) & 0x1;
    } else {
        x -= 4;
        *value = (big[1] >> (x * 8 + y)) & 0x1;
    }
    return 0;
}

int printbigchar(int symb, int x, int y, enum colors b) {
    int i, j, value;
    int big[2];
    
    switch (symb) {
        case 0:
            big[0] = 0xB1A9A5FF;        // K top
            big[1] = 0xFFA5A9B1;        // K bot
            break;
        case 1:
            big[0] = 0xBDA1BDFF;        // S top
            big[1] = 0xFFBD85BD;        // S bot
            break;
        case 2:
            big[0] = 0xA1A5BDFF;        // G top
            big[1] = 0xFFBDA5AD;        // G bot
            break;
        case 3:
            big[0] = 0xABD5ABFF;        // roo bash ka
            big[1] = 0xFFD5ABD5;        // roo bash ka
            break;
        case 4:
            big[0] = 0xC1800000;        // left side of crown top
            big[1] = 0xFFFFFFE7;        // ls of crown bot
            break;
        case 5:
            big[0] = 0x83010000;        // right side of crown top
            big[1] = 0xFFFFFFE7;        // rs of crown bot
            break;
        case 6:
            big[0] = 0x3030302;         // left side of sword top
            big[1] = 0x1010703;         // rs of sword bot
            break;
        case 7:
            big[0] = 0xC0800000;        // right side of sword top
            big[1] = 0x8080E0C0;        // rs of sword bot
            break;
        case 8:
            big[0] = 0x3A223A00;        // SL top
            big[1] = 0x3B0A;            // SL bot
            break;
        case 9:
            big[0] = 0x1E120C00;        // LA top
            big[1] = 0xD212;            // LA bot
            break;
        case 10:
            big[0] = 0x11294500;        // YE top
            big[1] = 0x1111;            // YE bot
            break;
        case 11:
            big[0] = 0xDE12DE00;        // ER top
            big[1] = 0xD214;            // ER bot
            break;
        case 12:
            big[0] = 0x61514B00;        // KI top
            big[1] = 0x4B51;            // KI bot
            break;
        case 13:
            big[0] = 0x2A32A200;        // IN top
            big[1] = 0xA226;            // IN bot
            break;
        case 14:
            big[0] = 0x58407800;        // G top
            big[1] = 0x7848;            // G bot
            break;

    }

    gotoxy(x, y);
    setbgcolor(b);

    for (i = 0; i < 8; ++i) {
        for (j = 7; j >= 0; --j) {
            gotoxy(x + i, y + j);
            value = 0;
            getbigcharpos(big, i, j, &value);
            if (value)
                printA(_ACSBOARD, symb);
            else
                printA (" ", symb);
        }
    }

    setbgcolor(9);
    return 0;
}

/*
 * MUSIC STUFF
 */

#define SLP100      usleep(100000)
#define SLP50       usleep(50000)
#define SLP25       usleep(25000)
#define NEXTSTEP    if (game_stage != 0) break
#define FINALSTEP   if (game_stage != 1) break
#define REPEAT      if (game_stage != 2) break

void wm() {
    while (1 < 2)
        if (!mute) break;
}

void lose_beep() {
    system("beep -f 523 -l 200"); SLP50;
    system("beep -f 494 -l 200"); SLP50;
    system("beep -f 466 -l 200"); SLP50;
    system("beep -f 440 -l 400"); SLP50;
    
    system("beep -f 523 -l 20");  SLP25;
    system("beep -f 622 -l 20");  SLP25;
    system("beep -f 784 -l 20");  SLP25;
    system("beep -f 1047 -l 20"); SLP25;
    system("beep -f 1242 -l 40"); SLP25;
    SLP50;
    system("beep -f 523 -l 20"); SLP25;
    system("beep -f 622 -l 20"); SLP25;
    system("beep -f 784 -l 20"); SLP25;
    system("beep -f 1047 -l 20"); SLP25;
    system("beep -f 1242 -l 40"); SLP25;
}

void win_beep() {
    int i;
    for (i = 0; i < 2; ++i) {
        system("beep -f 262 -l 50"); SLP50;
        system("beep -f 392 -l 50"); SLP50;
        system("beep -f 523 -l 50"); SLP50;
    }
    
    system("beep -f 659 -l 200"); SLP50; SLP50;
    
    for (i = 0; i < 2; ++i) {
        system("beep -f 294 -l 50"); SLP50;
        system("beep -f 440 -l 50"); SLP50;
        system("beep -f 583 -l 50"); SLP50;
    }
    
    system("beep -f 784 -l 200"); SLP50; SLP50;
    
    system("beep -f 523 -l 400"); SLP50;
}

void* beeps() {
    wm();
    while (1 < 2) {
        
        /*
         * START MENU
         */
        
        unsigned seed = time(NULL);
        int r = rand_r(&seed) % 3;
        if (r == 0) {
            while (1 < 2) {
                NEXTSTEP;
                int i;
                for (i = 0; i < 2; ++i) {
                    system("beep -f 131 -l 140"); SLP50; wm(); NEXTSTEP;
                    system("beep -f 165 -l 140"); SLP50; wm(); NEXTSTEP;
                    system("beep -f 196 -l 140"); SLP50; wm(); NEXTSTEP;
                    system("beep -f 262 -l 140"); SLP50; wm(); NEXTSTEP;
                } NEXTSTEP;
                for (i = 0; i < 2; ++i) {
                    system("beep -f 165 -l 140"); SLP50; wm(); NEXTSTEP;
                    system("beep -f 208 -l 140"); SLP50; wm(); NEXTSTEP;
                    system("beep -f 247 -l 140"); SLP50; wm(); NEXTSTEP;
                    system("beep -f 330 -l 140"); SLP50; wm(); NEXTSTEP;
                } NEXTSTEP;
                for (i = 0; i < 2; ++i) {
                    system("beep -f 110 -l 140"); SLP50; wm(); NEXTSTEP;
                    system("beep -f 131 -l 140"); SLP50; wm(); NEXTSTEP;
                    system("beep -f 165 -l 140"); SLP50; wm(); NEXTSTEP;
                    system("beep -f 220 -l 140"); SLP50; wm(); NEXTSTEP;
                } NEXTSTEP;
                
                system("beep -f 87 -l 140");  SLP50; wm(); NEXTSTEP;
                system("beep -f 110 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 130 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 174 -l 140"); SLP50; wm(); NEXTSTEP;
                
                system("beep -f 87 -l 140");  SLP50; wm(); NEXTSTEP;
                system("beep -f 104 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 130 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 174 -l 140"); SLP50; wm(); NEXTSTEP;
                
                for (i = 0; i < 2; ++i) {
                    system("beep -f 131 -l 140"); SLP50; wm(); NEXTSTEP;
                    system("beep -f 165 -l 140"); SLP50; wm(); NEXTSTEP;
                    system("beep -f 196 -l 140"); SLP50; wm(); NEXTSTEP;
                    system("beep -f 262 -l 140"); SLP50; wm(); NEXTSTEP;
                } NEXTSTEP;
                for (i = 0; i < 2; ++i) {
                    system("beep -f 98 -l 140");  SLP50; wm(); NEXTSTEP;
                    system("beep -f 123 -l 140"); SLP50; wm(); NEXTSTEP;
                    system("beep -f 147 -l 140"); SLP50; wm(); NEXTSTEP;
                    system("beep -f 196 -l 140"); SLP50; wm(); NEXTSTEP;
                } NEXTSTEP;
                for (i = 0; i < 2; ++i) {
                    system("beep -f 110 -l 140"); SLP50; wm(); NEXTSTEP;
                    system("beep -f 131 -l 140"); SLP50; wm(); NEXTSTEP;
                    system("beep -f 165 -l 140"); SLP50; wm(); NEXTSTEP;
                    system("beep -f 220 -l 140"); SLP50; wm(); NEXTSTEP;
                } NEXTSTEP;
                system("beep -f 87 -l 140");  SLP50; wm(); NEXTSTEP;
                system("beep -f 110 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 130 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 174 -l 140"); SLP50; wm(); NEXTSTEP;
                
                system("beep -f 87 -l 140");  SLP50; wm(); NEXTSTEP;
                system("beep -f 104 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 130 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 174 -l 140"); SLP50; wm(); NEXTSTEP;
            }
        } else if (r == 1) {
            while (1 < 2) {
                system("beep -f 65 -l 280"); SLP50; wm(); NEXTSTEP;
                system("beep -f 65 -l 280"); SLP50; wm(); NEXTSTEP;
                system("beep -f 78 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 78 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 65 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 58 -l 280"); SLP50; wm(); NEXTSTEP;
                system("beep -f 58 -l 280"); SLP50; wm(); NEXTSTEP;
                system("beep -f 49 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 58 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 49 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 58 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 62 -l 140"); SLP50; wm(); NEXTSTEP;
    
                system("beep -f 65 -l 280"); SLP50; wm(); NEXTSTEP;
                system("beep -f 65 -l 280"); SLP50; wm(); NEXTSTEP;
                system("beep -f 98 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 98 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 87 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 58 -l 280"); SLP50; wm(); NEXTSTEP;
                system("beep -f 58 -l 280"); SLP50; wm(); NEXTSTEP;
                system("beep -f 98 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 87 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 78 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 73 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 58 -l 140"); SLP50; wm(); NEXTSTEP;
    
                system("beep -f 52 -l 280"); SLP50; wm(); NEXTSTEP;
                system("beep -f 52 -l 280"); SLP50; wm(); NEXTSTEP;
                system("beep -f 59 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 59 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 59 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 65 -l 280"); SLP50; wm(); NEXTSTEP;
                system("beep -f 65 -l 280"); SLP50; wm(); NEXTSTEP;
                system("beep -f 44 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 44 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 87 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 104 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 131 -l 140"); SLP50; wm(); NEXTSTEP;
    
                system("beep -f 52 -l 280"); SLP50; wm(); NEXTSTEP;
                system("beep -f 52 -l 280"); SLP50; wm(); NEXTSTEP;
                system("beep -f 59 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 59 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 59 -l 140"); SLP50; wm(); NEXTSTEP;
                system("beep -f 65 -l 280"); SLP50; wm(); NEXTSTEP;
                system("beep -f 98 -l 280"); SLP50; wm(); NEXTSTEP;
                system("beep -f 131 -l 280"); SLP100;
                SLP100; SLP100; SLP100; wm(); NEXTSTEP;
            }
        } else {
            while (1 < 2) {
                system("beep -f 65 -l 300"); SLP100; SLP50; wm(); NEXTSTEP;
                system("beep -f 98 -l 150"); SLP100; wm(); NEXTSTEP;
                system("beep -f 98 -l 150"); SLP100; SLP100; SLP100; SLP50; wm(); NEXTSTEP;
                
                system("beep -f 65 -l 150"); SLP100; wm(); NEXTSTEP;
                system("beep -f 78 -l 150"); SLP100; SLP100; SLP100; SLP50; wm(); NEXTSTEP;
                    
                system("beep -f 65 -l 300"); SLP100; SLP50; wm(); NEXTSTEP;
                system("beep -f 98 -l 150"); SLP100; wm(); NEXTSTEP;
                system("beep -f 98 -l 150"); SLP100; wm(); NEXTSTEP;
                system("beep -f 58 -l 150"); SLP100; wm(); NEXTSTEP;
                system("beep -f 73 -l 150"); SLP100; wm(); NEXTSTEP;
                system("beep -f 87 -l 150"); SLP100; SLP100; SLP100; SLP50; wm(); NEXTSTEP;
                
                system("beep -f 65 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 65 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 65 -l 75"); SLP100; SLP50; wm(); NEXTSTEP;
                
                system("beep -f 98 -l 100"); SLP100; SLP50; wm(); NEXTSTEP;
                system("beep -f 98 -l 100"); SLP100; SLP100; SLP100; SLP100; wm(); NEXTSTEP;

                system("beep -f 65 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 65 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 78 -l 150"); SLP100; SLP100; SLP100; SLP50; wm(); NEXTSTEP;
                    
                system("beep -f 65 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 65 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 65 -l 75"); SLP100; SLP50; wm(); NEXTSTEP;
                    
                system("beep -f 98 -l 100"); SLP100; SLP50; wm(); NEXTSTEP;
                system("beep -f 98 -l 100"); SLP100; SLP50; wm(); NEXTSTEP;

                system("beep -f 58 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 58 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 73 -l 150"); SLP100; wm(); NEXTSTEP;
                system("beep -f 87 -l 150"); SLP100; wm(); NEXTSTEP;
                system("beep -f 87 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 73 -l 75"); SLP50; wm(); NEXTSTEP;
                
                system("beep -f 65 -l 300"); SLP100; SLP50; wm(); NEXTSTEP;
                system("beep -f 98 -l 150"); SLP100; wm(); NEXTSTEP;
                system("beep -f 98 -l 150"); SLP100; SLP100; SLP100; SLP50; wm(); NEXTSTEP;
                
                system("beep -f 65 -l 150"); SLP100; wm(); NEXTSTEP;
                system("beep -f 78 -l 150"); SLP100; SLP100; SLP100; SLP50; wm(); NEXTSTEP;
                    
                system("beep -f 65 -l 300"); SLP100; SLP50; wm(); NEXTSTEP;
                system("beep -f 98 -l 150"); SLP100; wm(); NEXTSTEP;
                system("beep -f 98 -l 150"); SLP100; wm(); NEXTSTEP;
                system("beep -f 58 -l 150"); SLP100; wm(); NEXTSTEP;
                system("beep -f 73 -l 150"); SLP100; wm(); NEXTSTEP;
                system("beep -f 87 -l 150"); SLP100; SLP100; SLP100; SLP50; wm(); NEXTSTEP;
                
                system("beep -f 65 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 65 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 65 -l 75"); SLP100; SLP50; wm(); NEXTSTEP;
                
                system("beep -f 98 -l 100"); SLP100; SLP50; wm(); NEXTSTEP;
                system("beep -f 98 -l 100"); SLP100; SLP100; SLP100; SLP100; wm(); NEXTSTEP;

                system("beep -f 65 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 65 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 78 -l 150"); SLP100; SLP100; SLP100; SLP50; wm(); NEXTSTEP;
                
                system("beep -f 52 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 52 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 52 -l 75"); SLP100; SLP50; wm(); NEXTSTEP;
                
                system("beep -f 78 -l 100"); SLP100; SLP50; wm(); NEXTSTEP;
                system("beep -f 78 -l 100"); SLP100; SLP50; wm(); NEXTSTEP;

                system("beep -f 58 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 58 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 73 -l 150"); SLP100; wm(); NEXTSTEP;
                system("beep -f 87 -l 150"); SLP100; wm(); NEXTSTEP;
                system("beep -f 87 -l 75"); SLP50; wm(); NEXTSTEP;
                system("beep -f 73 -l 75"); SLP50; wm(); NEXTSTEP;    
            }
        }
    
        /*
         * GAME SCREEN
         */
        
        while (1 < 2) {
                int i;
                for (i = 0; i < 2; ++i) {
                    system("beep -f 262 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 311 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 392 -l 100"); SLP100; wm(); FINALSTEP;
    
                    system("beep -f 196 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 311 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 392 -l 100"); SLP100; wm(); FINALSTEP;
    
                    system("beep -f 262 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 311 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 392 -l 100"); SLP100; wm(); FINALSTEP;
    
                    system("beep -f 196 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 311 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 392 -l 100"); SLP100; wm(); FINALSTEP;
    
                    system("beep -f 208 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 262 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 311 -l 100"); SLP100; wm(); FINALSTEP;
    
                    system("beep -f 156 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 196 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 233 -l 100"); SLP100; wm(); FINALSTEP;
    
                    system("beep -f 175 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 208 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 262 -l 100"); SLP100; wm(); FINALSTEP;
    
                    system("beep -f 196 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 247 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 294 -l 100"); SLP100; wm(); FINALSTEP;
                } FINALSTEP;
                
                for (i = 0; i < 2; ++i) {
                    system("beep -f 311 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 392 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 466 -l 100"); SLP100; wm(); FINALSTEP;
    
                    system("beep -f 293 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 349 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 466 -l 100"); SLP100; wm(); FINALSTEP;
    
                    system("beep -f 262 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 311 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 392 -l 100"); SLP100; wm(); FINALSTEP;
    
                    system("beep -f 208 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 262 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 311 -l 100"); SLP100; wm(); FINALSTEP;
    
                    system("beep -f 175 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 208 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 262 -l 100"); SLP100; wm(); FINALSTEP;
    
                    system("beep -f 175 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 208 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 262 -l 100"); SLP100; wm(); FINALSTEP;
    
                    system("beep -f 196 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 247 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 294 -l 100"); SLP100; wm(); FINALSTEP;
    
                    system("beep -f 196 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 247 -l 100"); SLP100; wm(); FINALSTEP;
                    system("beep -f 294 -l 100"); SLP100; wm(); FINALSTEP;
                } FINALSTEP;
                
                system("beep -f 262 -l 400"); SLP100; SLP100; wm(); FINALSTEP;
        }
        
        /*
         * WINNER / LOSER SCREEN (SILENCE FOR OTHER BEEPS)
         */
        
        while (1 < 2) {
            REPEAT;
        }
    }
    return 0;
}
