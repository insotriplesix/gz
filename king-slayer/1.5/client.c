#include "goodies.h"

/*
 * VARIABLES
 */

char *client_id; 

int ai = 0, pos = 0, turn, WHO,
    current_port = 0, port_num, nports;
int p1[4] = {2, 2, 2, 2}, p2[4] = {3, 3, 3, 3};
int *sockets;

pthread_t music;

/*
 * PRINT MENU
 */

void do_menu(int choice, int color) {
    int rows, cols;
    char buf[100];

    getscreensize(&rows, &cols);
    rows = rows / 2 - 2;
    cols = cols / 2 - 12;
    
    setbgcolor(color);

    switch (choice) {
        case 1: {
            gotoxy(rows, cols + 4);
            write(1, buf, sprintf(buf, "(-- NEW  GAME --)"));
            gotoxy(rows, cols + 4);
        } break;

        case 2: {
            gotoxy(rows + 2, cols + 4);
            write(1, buf, sprintf(buf, "(-- JOIN GAME --)"));
            gotoxy(rows + 2, cols + 4);
        } break;

        case 3: {
            gotoxy(rows + 4, cols + 4);
            write(1, buf, sprintf(buf, "(--   QUIT    --)"));
            gotoxy(rows + 4, cols + 4);
        } break;

        case 4: {
            gotoxy(rows - 3, cols + 4);
            write(1, buf, sprintf(buf, "~KING SLAYER GAME~"));
            gotoxy(rows - 3, cols + 4);
        } break;
        
        case 0: {
            gotoxy(rows + 12, cols - 21);
            if (mute)
                write(1, buf, sprintf(buf, "[SOUNDS: OFF (M)]"));
            else
                write(1, buf, sprintf(buf, "[SOUNDS:  ON (M)]"));
            gotoxy(rows + 12, cols - 21);
        } break;
    }
    setbgcolor(9);
}

/*
 * NAVIGATE THROUGH MENU
 */

int menu() {
    int key, choice = 1;
    mytermsave();
    clrscr();

    do_menu(4, COLOR_MAGENTA);
    do_menu(3, COLOR_WHITE);
    do_menu(2, COLOR_WHITE);
    do_menu(1, COLOR_BLUE);
    do_menu(0, COLOR_WHITE);

    mytermregime(0, 0, 1, 0, 1);

    do {
        readkey((enum keys *) &key);
        switch (key) {
            case _UP:
            do_menu(choice, COLOR_WHITE);
            if (choice > 1)
                --choice;
            else
                choice = 3;
            do_menu(choice, COLOR_BLUE);
            break;

            case _DOWN:
            do_menu(choice, COLOR_WHITE);
            if (choice < 3)
                ++choice;
            else
                choice = 1;
            do_menu(choice, COLOR_BLUE);
            break;
            
            case _M:
            if (mute)
                mute = 0;
            else
                mute = 1;
            do_menu(0, COLOR_WHITE);
            break;
            
            default: break;
        }
    } while (key != _ENTER);
    mytermrestore();
    return choice;
}

/*
 * PRINT MAIN FIELD
 */

int printfield() {
    int i, rows, cols;
    getscreensize(&rows, &cols);
    board (2, 2, rows - 4, cols - 30, 0);
    board (2, cols - 26, rows - 4, 24, 0);
    
    gotoxy(rows - 5, cols - 25);
    printf("                   \n");
    gotoxy(rows - 5, cols - 25);
    printf("SRV: %d (%s)\n", port_num + current_port, server_name(current_port));
    
    gotoxy(rows - 4, cols - 25);
    if (WHO == turn)
        printf("\033[1;33mPlease wait.\n");
    else
        printf("\033[1;33mYour turn!  \n");
    
    gotoxy(rows - 2, cols - 25);
    printf(" \033[1;36mAI:");
    if (ai)
        printf("\033[1;32mON ");
    else
        printf("\033[1;31mOFF");
    printf("\033[0m[N] | \033[1;36mSND:");
    if (!mute)
        printf("\033[1;32mON ");
    else
        printf("\033[1;31mOFF");
    printf("\033[0m[M]\n");    
    
    gotoxy(rows - 3, cols - 25);
    for (i = 1; i < 20; ++i) {
        printA(_ACSHLINE, 3);
    }
    for (i = 0; i < 4; ++i) {
        printbigchar (p2[i], 4, 4 + i * 10, COLOR_WHITE);
        board (3, 3 + i * 10, 8, 8, 0);
    }
    for (i = 0; i < 4; ++i) {
        printbigchar (p1[i], 14, 14 + i * 10, COLOR_WHITE);
        if (i == pos) {
            board (13, 13 + i * 10, 8, 8, 1);
        } else {
            board (13, 13 + i * 10, 8, 8, 0);
        }
    }    
    return 0;
}

/*
 * SET CARD TO A VALUE
 */

int setfield(int x, int symb) {
    switch (symb) {
        case BACK:  p1[x] = 1; break;
        case KING:  p1[x] = 2; break;
        case SLAVE: p1[x] = 3; break;
        case GUARD: p1[x] = 4; break;
    }
    return 0;
}

/*
 * EDIT A FIELD
 */

int editfield(int act) {
    int key, val = 3;
    unsigned seed;
    if (act) {
        if (ai) 
            readkey_timeout((enum keys *)&key, 4);
        else
            readkey((enum keys *)&key);
    } else {
        readkey_timeout((enum keys *)&key, 1);
    }
    switch (key) {
        case _LEFT:
            if (act) {
                if (pos - 1 >= 0)
                    --pos;
                else
                    pos = 3;
            }
            break;
        case _RIGHT:
            if (act) {
                if (pos + 1 <= 3)
                    ++pos;
                else
                    pos = 0;
            }
            break;
        case _SPACE:
            if (act) {
                val = p1[pos];
                p1[pos] = 3;
            }
            break;
        case _ESC:
            mytermrestore();
            val = -1;
            break;
        case _M:
            if (mute)
                mute = 0;
            else
                mute = 1;
            break;
        case _N:
            if (ai)
                ai = 0;
            else
                ai = 1;
            break;
        case _TIMEOUT:
            if (act) {
                seed = time(NULL);
                do {
                    pos = rand_r(&seed) % 4;
                } while (p1[pos] == 3);
                val = p1[pos];
                p1[pos] = 3;
            }
            break;
        default: break;
    }
    printfield();
    return val;
}

int new_game() {
    int card, i, r, c = 0;
    int byte, _byte;
    int rval;
    fd_set rfds;
    struct timeval tv;
    unsigned seed = time(NULL);
    
    clrscr();
    
    /*
     * Send and confirm our HAI.
     */
    
    send_msg(sockets[current_port], 1, "HAI\0");
    printf("Starting on %d...\n", port_num + current_port);
    while (1 < 2) {
        byte = 0;
        tv.tv_sec = 0, tv.tv_usec = 10000;
        FD_ZERO(&rfds);
        FD_SET(sockets[current_port], &rfds);
        rval = select(sockets[current_port] + 1, &rfds, NULL, NULL, NULL);
        if (rval > 0) {
            byte = read(sockets[current_port], &_byte, sizeof(int));
            if (byte > 0) {
                break;
            } else if (byte == 0) {
                ++current_port;
                printf("Port %d is closed, switched to %d.\n",
                    port_num + current_port - 1, port_num + current_port);
                if (current_port == nports) {
                    clrscr();
                    printf("Client shutting down: number of ports exceeded. [ENTER]\n");
                    getchar();
                    return 1;
                }
                send_msg(sockets[current_port], 1, "HAI\0");
            }
        }
    }
    
    /*
     * Wait for a second player.
     */
     
    printf("Waiting for player 2 on %d...\n", port_num + current_port);
    while (1 < 2) {
        byte = 0;
        tv.tv_sec = 0, tv.tv_usec = 10000;
        FD_ZERO(&rfds);
        FD_SET(sockets[current_port], &rfds);
        rval = select(sockets[current_port] + 1, &rfds, NULL, NULL, NULL);
        if (rval > 0) {
            byte = read(sockets[current_port], &_byte, sizeof(int));
            if (byte > 0) {
                break;
            } else if (byte == 0) {
                ++current_port;
                printf("Port %d is closed, switched to %d.\n",
                    port_num + current_port - 1, port_num + current_port);
                if (current_port == nports) {
                    clrscr();
                    printf("Client shutting down: number of ports exceeded. [ENTER]\n");
                    getchar();
                    return 1;
                }
            }
        }
    }
    printf("Confirmed new game on %d, next state...\n", port_num + current_port);
    
    game_stage = 1;
    for (i = 0; i < 4; ++i)
        p1[i] = 2;
    for (i = 0; i < 4; ++i)
        p2[i] = 3;
    r = rand_r(&seed) % 4;
    p1[r] = 0;
    WHO = 0;
    turn = 1;
    pos = 0;
    mytermsave();
    mytermregime(0, 0, 1, 0, 1);
    clrscr();
    printfield();
    
    /*
     * Teh game begins!
     */
    
    while (1 < 2) {
        if (turn) {
            while (1 < 2) {
                card = editfield(1);
                if (card == -1) {
                    send_msg(sockets[current_port], 7, "QUIT\0");
                    while (1 < 2) {
                        byte = 0;
                        tv.tv_sec = 0, tv.tv_usec = 10000;
                        FD_ZERO(&rfds);
                        FD_SET(sockets[current_port], &rfds);
                        rval = select(sockets[current_port] + 1, &rfds, NULL, NULL, &tv);
                        if (rval > 0) {
                            byte = read(sockets[current_port], &_byte, sizeof(int));
                            if (byte > 0) {
                                if (_byte == 6) {
                                    clrscr();
                                    printf("Your opponent has left the game. [ENTER]\n");
                                    getchar();
                                    return 0;
                                }
                                break;
                            } else if (byte == 0) {
                                ++current_port;
                                if (current_port == nports) {
                                    clrscr();
                                    printf("Client shutting down: number of ports exceeded. [ENTER]\n");
                                    getchar();
                                    return 1;
                                }
                                printfield();
                                send_msg(sockets[current_port], 7, "QUIT\0");
                            }
                        }
                    }
                    clrscr();
                    printf("Exiting the current game...\n");
                    sleep(1);
                    return 0;
                } else if (card != 3) {
                    switch (card) {
                        case 0:
                            send_msg(sockets[current_port], 3, "KING\0");
                            break;
                        case 1:
                            send_msg(sockets[current_port], 3, "SLAYER\0");
                            break;
                        case 2:
                            send_msg(sockets[current_port], 3, "GUARD\0");
                            break;
                    }
                    while (1 < 2) {
                        byte = 0;
                        tv.tv_sec = 0, tv.tv_usec = 10000;
                        FD_ZERO(&rfds);
                        FD_SET(sockets[current_port], &rfds);
                        rval = select(sockets[current_port] + 1, &rfds, NULL, NULL, &tv);
                        if (rval > 0) {
                            byte = read(sockets[current_port], &_byte, sizeof(int));
                            if (byte > 0) {
                                if (_byte == 6) {
                                    clrscr();
                                    printf("Your opponent has left the game. [ENTER]\n");
                                    getchar();
                                    return 0;
                                }
                                break;
                            } else if (byte == 0) {
                                ++current_port;
                                if (current_port == nports) {
                                    clrscr();
                                    printf("Client shutting down: number of ports exceeded. [ENTER]\n");
                                    getchar();
                                    return 1;
                                }
                                printfield();
                                switch (card) {
                                    case 0:
                                        send_msg(sockets[current_port], 3, "KING\0");
                                        break;
                                    case 1:
                                        send_msg(sockets[current_port], 3, "SLAYER\0");
                                        break;
                                    case 2:
                                        send_msg(sockets[current_port], 3, "GUARD\0");
                                        break;
                                }
                            }
                        }
                    }
                    turn = 0;
                    printfield();
                    break;
                } else continue;
            }
        } else {
            while (1 < 2) {
                int temp = editfield(0);
                if (temp == -1) {
                    send_msg(sockets[current_port], 7, "QUIT\0");
                    while (1 < 2) {
                        byte = 0;
                        tv.tv_sec = 0, tv.tv_usec = 10000;
                        FD_ZERO(&rfds);
                        FD_SET(sockets[current_port], &rfds);
                        rval = select(sockets[current_port] + 1, &rfds, NULL, NULL, &tv);
                        if (rval > 0) {
                            byte = read(sockets[current_port], &_byte, sizeof(int));
                            if (byte > 0) {
                                if (_byte == 6) {
                                    clrscr();
                                    printf("Your opponent has left the game. [ENTER]\n");
                                    getchar();
                                    return 0;
                                }
                                break;
                            } else if (byte == 0) {
                                ++current_port;
                                if (current_port == nports) {
                                    clrscr();
                                    printf("Client shutting down: number of ports exceeded. [ENTER]\n");
                                    getchar();
                                    return 1;
                                }
                                printfield();
                                send_msg(sockets[current_port], 7, "QUIT\0");
                            }
                        }
                    }
                    clrscr();
                    printf("Exiting the current game...\n");
                    sleep(2);
                    return 0;
                }
                byte = 0;
                tv.tv_sec = 0, tv.tv_usec = 10000;
                FD_ZERO(&rfds);
                FD_SET(sockets[current_port], &rfds);
                rval = select(sockets[current_port] + 1, &rfds, NULL, NULL, &tv);
                if (rval > 0) {
                    byte = read(sockets[current_port], &_byte, sizeof(int));
                    if (byte > 0) {
                        if (_byte == 6) {
                            clrscr();
                            printf("Your opponent has left the game. [ENTER]\n");
                            getchar();
                            return 0;
                        }
                        break;
                    } else if (byte == 0) {
                        ++current_port;
                        if (current_port == nports) {
                            clrscr();
                            printf("Client shutting down: number of ports exceeded. [ENTER]\n");
                            getchar();
                            return 1;
                        }
                        printfield();
                    }
                }
            }
            if (card == 0 && _byte == 1) {
                p2[c++] = _byte;
                printfield();
                sleep(2);
                game_stage = 2;
                clrscr();
                printbigchar(6, 10, 20, COLOR_WHITE);
                printbigchar(7, 10, 28, COLOR_WHITE);
                printbigchar(8, 10, 32, COLOR_WHITE);
                printbigchar(9, 10, 40, COLOR_WHITE);
                printbigchar(10, 10, 47, COLOR_WHITE);
                printbigchar(11, 10, 55, COLOR_WHITE);
                send_msg(sockets[current_port], 4, "SLAYER\0");
                while (1 < 2) {
                    byte = 0;
                    tv.tv_sec = 0, tv.tv_usec = 10000;
                    FD_ZERO(&rfds);
                    FD_SET(sockets[current_port], &rfds);
                    rval = select(sockets[current_port] + 1, &rfds, NULL, NULL, &tv);
                    if (rval > 0) {
                        byte = read(sockets[current_port], &_byte, sizeof(int));
                        if (byte > 0) {
                            if (_byte == 6) {
                                clrscr();
                                printf("Your opponent has left the game. [ENTER]\n");
                                getchar();
                                return 0;
                            }
                            break;
                        } else if (byte == 0) {
                            ++current_port;
                            if (current_port == nports) {
                                clrscr();
                                printf("Client shutting down: number of ports exceeded. [ENTER]\n");
                                getchar();
                                return 1;
                            }
                            send_msg(sockets[current_port], 4, "SLAYER\0");
                        }
                    }
                }
                if (!mute) lose_beep();
                sleep(4);
                mytermrestore();
                clrscr();
                return 0;
            } else if ((card == 0 && _byte == 2) || (card == 2 && _byte == 1)) {
                p2[c++] = _byte;
                printfield();
                sleep(2);
                game_stage = 2;
                clrscr();
                printbigchar(4, 10, 20, COLOR_WHITE);
                printbigchar(5, 10, 28, COLOR_WHITE);
                printbigchar(12, 12, 38, COLOR_WHITE);
                printbigchar(13, 12, 46, COLOR_WHITE);
                printbigchar(14, 12, 53, COLOR_WHITE);
                if (card == 0)
                    send_msg(sockets[current_port], 4, "KING\0");
                else if (card == 2)
                    send_msg(sockets[current_port], 4, "GUARD\0");
                while (1 < 2) {
                    byte = 0;
                    tv.tv_sec = 0, tv.tv_usec = 10000;
                    FD_ZERO(&rfds);
                    FD_SET(sockets[current_port], &rfds);
                    rval = select(sockets[current_port] + 1, &rfds, NULL, NULL, &tv);
                    if (rval > 0) {
                        byte = read(sockets[current_port], &_byte, sizeof(int));
                        if (byte > 0) {
                            if (_byte == 6) {
                                clrscr();
                                printf("Your opponent has left the game. [ENTER]\n");
                                getchar();
                                return 0;
                            }
                            break;
                        } else if (byte == 0) {
                            ++current_port;
                            if (current_port == nports) {
                                clrscr();
                                printf("Client shutting down: number of ports exceeded. [ENTER]\n");
                                getchar();
                                return 1;
                            }
                            if (card == 0)
                                send_msg(sockets[current_port], 4, "KING\0");
                            else if (card == 2)
                                send_msg(sockets[current_port], 4, "GUARD\0");
                        }
                    }
                }
                if (!mute) win_beep();
                sleep(4);
                mytermrestore();
                clrscr();
                return 0;
            } else {
                p2[c++] = _byte;
                printfield();
                send_msg(sockets[current_port], 4, "CONTINUE\0");
                while (1 < 2) {
                    byte = 0;
                    tv.tv_sec = 0, tv.tv_usec = 10000;
                    FD_ZERO(&rfds);
                    FD_SET(sockets[current_port], &rfds);
                    rval = select(sockets[current_port] + 1, &rfds, NULL, NULL, &tv);
                    if (rval > 0) {
                        byte = read(sockets[current_port], &_byte, sizeof(int));
                        if (byte > 0) {
                            if (_byte == 6) {
                                clrscr();
                                printf("Your opponent has left the game. [ENTER]\n");
                                getchar();
                                return 0;
                            }
                            break;
                        } else if (byte == 0) {
                            ++current_port;
                            if (current_port == nports) {
                                clrscr();
                                printf("Client shutting down: number of ports exceeded. [ENTER]\n");
                                getchar();
                                return 1;
                            }
                            printfield();
                            send_msg(sockets[current_port], 4, "CONTINUE\0");
                        }
                    }
                }
                turn = 1;
                printfield();
            }
        }
    }
    return 0;
}

int join_game() {
    int card, i, r, c = 0;
    int byte, _byte;
    int rval;
    fd_set rfds;
    struct timeval tv;
    unsigned seed = time(NULL);
    
    clrscr();
    
    /*
     * Join the game.
     */
    
    send_msg(sockets[current_port], 2, "HAI\0");
    printf("Joining on %d...\n", port_num + current_port);
    while (1 < 2) {
        byte = 0;
        tv.tv_sec = 0, tv.tv_usec = 10000;
        FD_ZERO(&rfds);
        FD_SET(sockets[current_port], &rfds);
        rval = select(sockets[current_port] + 1, &rfds, NULL, NULL, &tv);
        if (rval > 0) {
            byte = read(sockets[current_port], &_byte, sizeof(int));
            if (byte > 0) {
                if (_byte == 7) {
                    clrscr();
                    printf("No games to join at this moment, try a bit later. [ENTER]\n");
                    getchar();
                    return 0;
                }
                break;
            } else if (byte == 0) {
                ++current_port;
                printf("Port %d is closed, switched to %d.\n",
                    port_num + current_port - 1, port_num + current_port);
                if (current_port == nports) {
                    clrscr();
                    printf("Client shutting down: number of ports exceeded. [ENTER]\n");
                    getchar();
                    return 1;
                }
                send_msg(sockets[current_port], 2, "HAI\0");
            }
        }
    }
    printf("Confirmed join on %d, next state...\n", port_num + current_port);
    
    game_stage = 1;
    for (i = 0; i < 4; ++i)
        p1[i] = 2;
    for (i = 0; i < 4; ++i)
        p2[i] = 3;
    r = rand_r(&seed) % 4;
    p1[r] = 1;
    WHO = 1;
    turn = 1;
    pos = 0;
    mytermsave();
    mytermregime(0, 0, 1, 0, 1);

    clrscr();
    printfield();
    while (1 < 2) {
        if (turn) {
            _byte = 0;
            while (1 < 2) {
                int temp = editfield(0);
                if (temp == -1) {
                    send_msg(sockets[current_port], 7, "QUIT\0");
                    while (1 < 2) {
                        byte = 0;
                        tv.tv_sec = 0, tv.tv_usec = 10000;
                        FD_ZERO(&rfds);
                        FD_SET(sockets[current_port], &rfds);
                        rval = select(sockets[current_port] + 1, &rfds, NULL, NULL, &tv);
                        if (rval > 0) {
                            byte = read(sockets[current_port], &_byte, sizeof(int));
                            if (byte > 0) {
                                if (_byte == 6) {
                                    clrscr();
                                    printf("Your opponent has left the game. [ENTER]\n");
                                    getchar();
                                    return 0;
                                }
                                break;
                            } else if (byte == 0) {
                                ++current_port;
                                if (current_port == nports) {
                                    clrscr();
                                    printf("Client shutting down: number of ports exceeded. [ENTER]\n");
                                    getchar();
                                    return 1;
                                }
                                printfield();
                                send_msg(sockets[current_port], 7, "QUIT\0");
                            }
                        }
                    }
                    clrscr();
                    printf("Exiting the current game...\n");
                    sleep(1);
                    return 0;
                }
                byte = 0;
                tv.tv_sec = 0, tv.tv_usec = 10000;
                FD_ZERO(&rfds);
                FD_SET(sockets[current_port], &rfds);
                rval = select(sockets[current_port] + 1, &rfds, NULL, NULL, &tv);
                if (rval > 0) {
                    byte = read(sockets[current_port], &_byte, sizeof(int));
                    if (byte > 0) {
                        if (_byte == 6) {
                            clrscr();
                            printf("Your opponent has left the game. [ENTER]\n");
                            getchar();
                            return 0;
                        }
                        break;
                    } else if (byte == 0) {
                        ++current_port;
                        if (current_port == nports) {
                            clrscr();
                            printf("Client shutting down: number of ports exceeded. [ENTER]\n");
                            getchar();
                            return 1;
                        }
                        printfield();
                    }
                }
            }
            turn = 0;
            printfield();
        } else {
            while (1 < 2) {
                card = editfield(1);
                if (card == -1) {
                    send_msg(sockets[current_port], 7, "QUIT\0");
                    while (1 < 2) {
                        byte = 0;
                        tv.tv_sec = 0, tv.tv_usec = 10000;
                        FD_ZERO(&rfds);
                        FD_SET(sockets[current_port], &rfds);
                        rval = select(sockets[current_port] + 1, &rfds, NULL, NULL, &tv);
                        if (rval > 0) {
                            byte = read(sockets[current_port], &_byte, sizeof(int));
                            if (byte > 0) {
                                if (_byte == 6) {
                                    clrscr();
                                    printf("Your opponent has left the game. [ENTER]\n");
                                    getchar();
                                    return 0;
                                }
                                break;
                            } else if (byte == 0) {
                                ++current_port;
                                if (current_port == nports) {
                                    clrscr();
                                    printf("Client shutting down: number of ports exceeded. [ENTER]\n");
                                    getchar();
                                    return 1;
                                }
                                printfield();
                                send_msg(sockets[current_port], 7, "QUIT\0");
                            }
                        }
                    }
                    clrscr();
                    printf("Exiting the current game...\n");
                    sleep(1);
                    return 0;
                } else if (card != 3) {
                    switch (card) {
                        case 0:
                            send_msg(sockets[current_port], 3, "KING\0");
                            break;
                        case 1:
                            send_msg(sockets[current_port], 3, "SLAYER\0");
                            break;
                        case 2:
                            send_msg(sockets[current_port], 3, "GUARD\0");
                            break;
                    }
                    while (1 < 2) {
                        byte = 0;
                        tv.tv_sec = 0, tv.tv_usec = 10000;
                        FD_ZERO(&rfds);
                        FD_SET(sockets[current_port], &rfds);
                        rval = select(sockets[current_port] + 1, &rfds, NULL, NULL, &tv);
                        if (rval > 0) {
                            byte = read(sockets[current_port], &_byte, sizeof(int));
                            if (byte > 0) {
                                if (_byte == 6) {
                                    clrscr();
                                    printf("Your opponent has left the game. [ENTER]\n");
                                    getchar();
                                    return 0;
                                }
                                break;
                            } else if (byte == 0) {
                                ++current_port;
                                if (current_port == nports) {
                                    clrscr();
                                    printf("Client shutting down: number of ports exceeded. [ENTER]\n");
                                    getchar();
                                    return 1;
                                }
                                printfield();
                                switch (card) {
                                    case 0:
                                        send_msg(sockets[current_port], 3, "KING\0");
                                        break;
                                    case 1:
                                        send_msg(sockets[current_port], 3, "SLAYER\0");
                                        break;
                                    case 2:
                                        send_msg(sockets[current_port], 3, "GUARD\0");
                                        break;
                                }
                            }
                        }
                    }
                    turn = 1;
                    printfield();
                    break;
                } else continue;
            }
            while (1 < 2) {
                byte = 0;
                tv.tv_sec = 0, tv.tv_usec = 10000;
                FD_ZERO(&rfds);
                FD_SET(sockets[current_port], &rfds);
                rval = select(sockets[current_port] + 1, &rfds, NULL, NULL, &tv);
                if (rval > 0) {
                    byte = read(sockets[current_port], &_byte, sizeof(int));
                    if (byte > 0) {
                        if (_byte == 6) {
                            clrscr();
                            printf("Your opponent has left the game. [ENTER]\n");
                            getchar();
                            return 0;
                        }
                        break;
                    } else if (byte == 0) {
                        ++current_port;
                        if (current_port == nports) {
                            clrscr();
                            printf("Client shutting down: number of ports exceeded. [ENTER]\n");
                            getchar();
                            return 1;
                        }
                    }
                }
            }
            switch (_byte) {
                case 0:
                case 10:
                    if (_byte)
                        p2[c++] = 2;
                    else
                        p2[c++] = 0;
                    printfield();
                    sleep(2);
                    game_stage = 2;
                    clrscr();
                    printbigchar(4, 10, 20, COLOR_WHITE);
                    printbigchar(5, 10, 28, COLOR_WHITE);
                    printbigchar(12, 12, 38, COLOR_WHITE);
                    printbigchar(13, 12, 46, COLOR_WHITE);
                    printbigchar(14, 12, 53, COLOR_WHITE);
                    if (!mute) lose_beep();
                    sleep(4);
                    mytermrestore();
                    clrscr();
                    return 0;
                    break;
                case 1:
                    p2[c++] = 0;
                    printfield();
                    sleep(2);
                    game_stage = 2;
                    clrscr();
                    printbigchar(6, 10, 20, COLOR_WHITE);
                    printbigchar(7, 10, 28, COLOR_WHITE);
                    printbigchar(8, 10, 32, COLOR_WHITE);
                    printbigchar(9, 10, 40, COLOR_WHITE);
                    printbigchar(10, 10, 47, COLOR_WHITE);
                    printbigchar(11, 10, 55, COLOR_WHITE);
                    if (!mute) win_beep();
                    sleep(4);
                    mytermrestore();
                    clrscr();
                    return 0;
                    break;
                case 2:
                    p2[c++] = 2;
                    printfield();
                    sleep(2);
                    break;
            }
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Not enough arguments supplied.\n");
        exit(1);
    }
    
    port_num = atoi(argv[1]);
    nports = atoi(argv[2]);
    client_id = argv[3];
    
    sockets = (int *)malloc(sizeof(int) * nports);
    
    int i, rows, cols;
    getscreensize(&rows, &cols);
    
    // GAME SOCKETS
    
    for (i = 0; i < nports; ++i) {
        struct sockaddr_in addr;
        sockets[i] = socket(AF_INET, SOCK_STREAM, 0);
        if (sockets[i] < 0) {
            printf("Socket error at %d. [ENTER]", port_num + i);
            getchar();
            exit(2);
        }
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_num + i);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        if (connect(sockets[i], (struct sockaddr *)&addr, sizeof(addr))) {
            printf("Failed to connect to %d. [ENTER]", port_num + i);
            getchar();
            exit(3);
        }
        send_msg(sockets[i], 6, client_id);
    }
    system("setterm -cursor off");
    pthread_create(&music, NULL, beeps, NULL);
    int n = 0, ret = 0;
    while (n != 3 && !ret) {
        game_stage = 0;
        n = menu();
        switch (n) {
            case 1:
                ret = new_game();
                break;
            case 2:
                ret = join_game();
                break;
            case 3:
                break;
        }
    }
    
    for (i = 0; i < nports; ++i)
        shutdown(sockets[i], 2);
    
    clrscr();
    printf("Goodbye then, mate!\n");
    mute = 1;
    sleep(1);
    clrscr();
    pthread_cancel(music);
    system("setterm -cursor on");
    return 0;
}
