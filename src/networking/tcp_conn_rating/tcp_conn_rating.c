#include <stdio.h>

enum errcodes {
    ERR_NONE,
    ERR_CMD_LINE_PARAMS,
    ERR_LAST
};

struct config_info {
    struct sockaddr_storage dst_addr;
};

struct config_info cfg_info;

int parse_command_line(int argc, char *argv[])
{
    if (argc < 3) {
        printf("");
        return ERR_CMD_LINE_PARAMS;
    }
    
}

int main(int argc, char *argv[])
{
    return 0;
}
