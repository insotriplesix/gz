#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#include "forserver.h"

struct Game {
    int u1;
    int u2;
    int status;
};

std::vector<int> backups;
std::vector<int> users;
std::vector<int> mir(32);
std::vector<Game> games;

int active, listener, nports, server_id, port, panic;
int quits_key_one = 0, quits_key_two = 0;
int quit = 6, nope = 7, ok = 8, join = 9;
unsigned int current_backup = 0;

double current = 0.0, last_five = 0.0;
struct timeval supertimer;

void game_routine(struct Message *mes, int sock) {
    int t = mes->type;
    int byte, ind;
    unsigned i, j;
    switch (t) {
        case 1: // New game.
            struct Game NewGame;
            if (active) {
                for (j = 0; j < mir.size(); ++j)
                    if (mir[j] == sock) break;
                NewGame.u1 = j;
                NewGame.status = 1;
                games.push_back(NewGame);
                std::cout << "[" << server_name(server_id) << " <" << port <<
                    ">] New game [host: " << sock << "]." << std::endl;
                send(sock, &ok, sizeof(int), 0);
                
                if (server_id != nports - 1)
                    send_msg(backups[current_backup], 1, std::to_string(NewGame.u1).c_str());
            } else {
                NewGame.u1 = std::stoi(mes->m);
                NewGame.status = 1;
                games.push_back(NewGame);
                std::cout << "[" << server_name(server_id) << " <" << port <<
                    ">] New game [host: " << sock << "]." << std::endl;
                if (server_id != nports - 1)
                    send_msg(backups[current_backup], 1, mes->m);
            }
            break;
        
        case 2: // Join game.
            for (i = 0; i < games.size(); ++i) {
                if (games[i].status == 1) {
                    games[i].status = 2;
                    if (active) {
                        for (j = 0; j < mir.size(); ++j)
                            if (mir[j] == sock) break;
                        games[i].u2 = j;
                        send(mir[j], &join, sizeof(int), 0);
                        std::cout << "[" << server_name(server_id) << " <"
                            << port << ">] Confirmed to P2 (" << games[i].u2
                            << ")." << std::endl;
                        send(mir[games[i].u1], &join, sizeof(int), 0);
                        std::cout << "[" << server_name(server_id) << " <"
                            << port << ">] Confirmed to P1 (" << games[i].u1
                            << ")." << std::endl;
                        if (server_id != nports - 1)
                            send_msg(backups[current_backup], 2, std::to_string(j).c_str());
                    } else {
                        games[i].u2 = std::stoi(mes->m);
                        if (server_id != nports - 1)
                            send_msg(backups[current_backup], 2, mes->m);
                    }
                    std::cout << "[" << server_name(server_id) << " <"
                        << port << ">] Player 2 has joined a game." << std::endl;
                    break;
                }
            }
            if (i == games.size() && active) {
                for (j = 0; j < mir.size(); ++j)
                    if (mir[j] == sock) break;
                send(mir[j], &nope, sizeof(int), 0);
                std::cout << "[" << server_name(server_id) << " <" << port <<
                    ">] No available games for client " << sock <<
                    ", refused." << std::endl;
            }
            break;
        
        case 3: // Whose turn it is?
            for (i = 0; i < games.size(); ++i) {
                if (games[i].status == 2 &&
                    (mir[games[i].u1] == sock || mir[games[i].u2] == sock)) {
                    if (active) {
                        switch (mes->m[0]) {
                            case 'K': byte = 0; break;
                            case 'S': byte = 1; break;
                            case 'G': byte = 2; break;
                        }
                        if (mir[games[i].u1] == sock) {
                            std::cout << "[" << server_name(server_id) <<
                                " <" << port << ">] Player 1 chose " << card_name(byte) << std::endl;
                            send(mir[games[i].u2], &byte, sizeof(int), 0);
                            send(mir[games[i].u1], &ok, sizeof(int), 0);
                            std::cout << "[" << server_name(server_id) <<
                                " <" << port << ">] Sent OK to P1." << std::endl;
                        } else if (mir[games[i].u2] == sock) {
                            std::cout << "[" << server_name(server_id) <<
                                " <" << port << ">] Player 1 chose " << card_name(byte) << std::endl;
                            send(mir[games[i].u1], &byte, sizeof(int), 0);
                            send(mir[games[i].u2], &ok, sizeof(int), 0);
                            std::cout << "[" << server_name(server_id) <<
                                " <" << port << ">] Sent OK to P2." << std::endl;
                        }
                    }
                    break;
                }
            }
            if (server_id != nports - 1)
                send_msg(backups[current_backup], 3, mes->m);
            break;

        case 4: // Who wins (or maybe continue?).
            for (i = 0; i < games.size(); ++i) {
                if (games[i].status == 2 &&
                    (mir[games[i].u1] == sock || mir[games[i].u2] == sock)) {
                    switch (mes->m[0]) {
                        case 'K': byte = 0; break;
                        case 'S': byte = 1; break;
                        case 'C': byte = 2; break;
                        case 'G': byte = 10; break;
                    }
                    if (active) {
                        send(mir[games[i].u2], &byte, sizeof(int), 0);
                        send(mir[games[i].u1], &ok, sizeof(int), 0);
                        std::cout << "[" << server_name(server_id) << " <"
                            << port << ">] Confirmed 1st player's " << card_name(byte)
                            << " to player 2." << std::endl;
                    }
                    break;
                }
            }
            if (byte != 2) {
                std::cout << "[" << server_name(server_id) << " <" << port <<
                    ">] Endspiel!" << std::endl;
                games.erase(games.begin() + i);
            }
            if (server_id != nports - 1)
                send_msg(backups[current_backup], 4, mes->m);
            break;
            
        case 5: // Server communication.
            gettimeofday(&supertimer, NULL);
            last_five = supertimer.tv_sec + 1E-6 * supertimer.tv_usec;
            break;
        
        case 6: // Client ID.
            ind = std::stoi(mes->m);
            mir[ind] = sock;
            std::cout << "[" << server_name(server_id) << " <" << port <<
                ">] Hello from client " << sock << " with ID: " << ind << std::endl;
            break;
            
        case 7: // Premature quit.
            for (i = 0; i < games.size(); ++i) {
                if (games[i].status == 2 &&
                    (mir[games[i].u1] == sock || mir[games[i].u2] == sock)) {
                    if (active) {
                        if (mir[games[i].u1] == sock) {
                            std::cout << "[" << server_name(server_id) <<
                                " <" << port << ">] Player 1 has quit the game." << std::endl;
                            send(mir[games[i].u1], &ok, sizeof(int), 0);
                            send(mir[games[i].u2], &quit, sizeof(int), 0);
                        } else if (mir[games[i].u2] == sock) {
                            std::cout << "[" << server_name(server_id) <<
                                " <" << port << ">] Player 2 has quit the game." << std::endl;
                            send(mir[games[i].u2], &ok, sizeof(int), 0);
                            send(mir[games[i].u1], &quit, sizeof(int), 0);
                        }
                    }
                    break;
                }
            }
            std::cout << "[" << server_name(server_id) << " <" << port
                << ">] Ending the game." << std::endl;
            games.erase(games.begin() + i);
            if (server_id != nports - 1)
                send_msg(backups[current_backup], 7, mes->m);
            break;
            
        default: // Panic mode on.
            if (t)
                std::cout << "[" << server_name(server_id) << " <" << port
                    << ">] Undefined message passed (type " << t
                    << ") from " << sock << "." << std::endl;
            else
                std::cout << "[" << server_name(server_id) << " <" << port
                    << ">] Type 0 message passed from " << sock <<
                    ". Assuming it's dead, closing connection." << std::endl;
            panic = 1;
            break;
    }
}

void game_controller() {
    int retval, size;
    fd_set rfds;
    struct timeval tv;
    gettimeofday(&supertimer, NULL);
    current = supertimer.tv_sec + 1E-6 * supertimer.tv_usec;
    last_five = current;
    while (1 < 2) {
        tv.tv_sec = 0, tv.tv_usec = 10000;
        for (unsigned i = 0; i < users.size(); ++i) {
            FD_ZERO(&rfds);
            FD_SET(users[i], &rfds);
            retval = select(users[i] + 1, &rfds, NULL, NULL, &tv);
            if (retval > 0) {
                struct Message *mes;
                mes = (struct Message *)malloc(MSG_HEADER_SIZE);
                recv(users[i], mes, MSG_HEADER_SIZE, 0);
                size = mes->size;
                mes = (struct Message *)realloc(mes, MSG_HEADER_SIZE +
                    size);
                recv(users[i], mes->m, size, 0);
                game_routine(mes, users[i]);
                if (panic) {
                    users.erase(users.begin() + i);
                    panic = 0;
                }
                free(mes);
            }
        }
        if (!active) {
            gettimeofday(&supertimer, NULL);
            current = supertimer.tv_sec + 1E-6 * supertimer.tv_usec;
            if (current - last_five > 0.4) {
                active = 1;
                std::cout << "[" << server_name(server_id) << " <" <<
                    port << ">] IS NOW ACTIVE." << std::endl;
            }
        }
        if (quits_key_one && quits_key_two) {
            break;
        }
    }
    return;
}

void alive_controller() {
    for (int i = 1; i < nports - server_id; ++i) {
        struct sockaddr_in addr;
        backups.push_back(socket(AF_INET, SOCK_STREAM, 0));
        if (backups[i - 1] < 0) {
            std::cout << "[" << server_name(server_id) << " <" << port <<
                ">] CANNOT INTO SOCKET AT " << port + i << "." << std::endl;
            getchar();
            system("setterm -cursor on");
            return;
        }
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port + i);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        while (!connect(backups[i - 1], (struct sockaddr *)&addr, sizeof(addr)));
        std::cout << "[" << server_name(server_id) << " <" << port <<
            ">] Connected to " << port + i << " (" << server_name(server_id + i) <<
            ") as my backup." << std::endl;
    }
    
    std::cout << "[" << server_name(server_id) << " <" << port <<
        ">] My current backup is at port " << port + current_backup + 1 <<
        " (" << server_name(server_id + current_backup + 1) << ")." << std::endl;
    
    while (1 < 2) {
        usleep(100000);
        if (!is_socket_alive_write(backups[current_backup])) {
            ++current_backup;
            std::cout << "[" << server_name(server_id) << " <" << port <<
                ">] I think " << port + current_backup << " (" <<
                server_name(server_id + current_backup) <<
                ") is dead. Erasing it from my backups..." << std::endl;
            if (current_backup == backups.size()) {
                std::cout << "[" << server_name(server_id) << " <" << port <<
                ">] No more backups for me." << std::endl;
                break;
            }
            std::cout << "[" << server_name(server_id) << " <" << port <<
                ">] My current backup is at port " << port + current_backup + 1 <<
                " (" << server_name(server_id + current_backup + 1) << ")." << std::endl;
        } else {
            send_msg(backups[current_backup], 5, "RLY\0");
        }
        if (quits_key_one && quits_key_two) {
            for (unsigned i = 0; i < backups.size(); ++i)
                shutdown(backups[i], 2);
            break;
        }
    }
    return;
}

void keyboard_controller() {
    char ch;
    nonblock(NB_ENABLE);
    while (1 < 2) {
        usleep(1);
        if (key_hit()) {
            ch = fgetc(stdin);
            if (ch == 'q') {
                quits_key_one = 1;
                break;
            }
        }
    }
    nonblock(NB_DISABLE);
    return;
}

void listen_controller() {
    while (1 < 2) {
        listen(listener, 44);
        int sock = accept_timeout(listener, 4);
        if (sock != -1)
            users.push_back(sock);
        if (quits_key_one) {
            shutdown(listener, 2);
            quits_key_two = 1;
            break;
        }
    }
    return;
}

int main(int argc, char **argv) {
    
    /*
     * Reading arguments
     */
    
    if (argc != 5) {
        printf("Not enough arguments.\n");
        system("setterm -cursor on");
        exit(1);
    }
    
    active = atoi(argv[1]);
    server_id = atoi(argv[2]);
    port = atoi(argv[3]);
    nports = atoi(argv[4]);
    
    system("setterm -cursor off");
    std::cout << "[" << server_name(server_id) << " <" << port <<
        ">] USING PORT " << port << "." << std::endl;
    
    if (active)
        std::cout << "[" << server_name(server_id) << " <" << port <<
            ">] IS ACTIVE." << std::endl;
    else
        std::cout << "[" << server_name(server_id) << " <" << port <<
            ">] IS PASSIVE." << std::endl;
    
    /*
     * Creating a listener socket.
     */
    
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        std::cout << "[" << server_name(server_id) << " <" << port <<
            ">] CANNOT INTO LISTENER SOCKET [ENTER]." << std::endl;
        getchar();
        system("setterm -cursor on");
        exit(2);
    }
    
    std::cout << "[" << server_name(server_id) << " <" << port <<
        ">] Created a listener socket." << std::endl;
    
    /*
     * Creating bindings
     */
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cout << "[" << server_name(server_id) << " <" << port <<
            ">] CANNOT BIND. [ENTER]" << std::endl;
        getchar();
        system("setterm -cursor on");
        exit(3);
    }
    
    /*
     * Starting threads.
     */
    
    std::thread alive_cont;
    if (server_id != nports - 1)
        alive_cont = std::thread(alive_controller);
    
    std::thread listen_cont(listen_controller);
    std::thread game_cont(game_controller);
    std::thread keyboard_cont(keyboard_controller);
    
    std::cout << "[" << server_name(server_id) << " <" << port <<
        ">] Started multithreading stuff." << std::endl;
    std::cout << "[" << server_name(server_id) << " <" << port <<
        ">] Press 'q' to switch me off after you are done." << std::endl;
    std::cout << "[" << server_name(server_id) << " <" << port <<
        ">] DON'T PRESS 'q' DURING THE GAME!" << std::endl;
    
    /*
     * Joining stuff.
     */
    
    keyboard_cont.join();
    std::cout << "[" << server_name(server_id) << " <" << port <<
        ">] Keyboard controller shuts down." << std::endl;
        
    listen_cont.join();
    std::cout << "[" << server_name(server_id) << " <" << port <<
        ">] Listener controller shuts down." << std::endl;
    
    game_cont.join();
    std::cout << "[" << server_name(server_id) << " <" << port <<
        ">] Game controller shuts down." << std::endl;
        
    if (server_id != nports - 1) {
        alive_cont.join();
        std::cout << "[" << server_name(server_id) << " <" << port <<
            ">] Alive controller shuts down." << std::endl;
    }
    
    std::cout << "[" << server_name(server_id) << " <" << port <<
        ">] Main thread shutting down. [ENTER]" << std::endl;
    getchar();
    system("setterm -cursor on");
    return 0;
}
