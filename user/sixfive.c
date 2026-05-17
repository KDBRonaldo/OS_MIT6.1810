#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define MAXNUM 32

char numbuf[MAXNUM];

// judge if a character is a seperator
int is_seperator(char c){
    return strchr("-\r\t\n./,", c) != 0;
}

void emit_if_match(int valid, int len, int value){
    if(valid && len > 0 && (value % 5 == 0 || value % 6 == 0)){
        numbuf[len] = '\0';
        printf("%s\n", numbuf);
    }
}

void sixfive(int fd){
    char c;
    int n;
    int innum = 0;
    int valid = 0;
    int len = 0;
    int value = 0;

    while((n = read(fd, &c, 1)) > 0){
        if(is_seperator(c)){
            if(innum){
                emit_if_match(valid, len, value);
            }
            // reset the meta data
            innum = 0;
            valid = 0;
            len = 0;
            value = 0;
        }
        else if(c >= '0' && c <= '9'){
            if(!innum){
                innum = 1;
                valid = 1;
                len = 0;
                value = 0;
            }
            if(len < MAXNUM - 1){
                numbuf[len++] = c;
                value = value * 10 + (c - '0');
            }
        }
        else{
            innum = 0;
            valid = 0;
            len = 0;
            value = 0;
        }
    }


    emit_if_match(valid, len, value);

    if(n < 0){
        fprintf(2, "sixfive: read error\n");
        exit(1);
    }
}

int main(int argc, char* argv[]){
    if(argc < 2){
        fprintf(2, "false number of parameters\n");
        exit(1);
    }

    int fd, i;

    for(i = 1; i < argc; i++){
        fd = open(argv[i], O_RDONLY); 
        if(fd < 0){
            fprintf(2, "cannot open %s\n", argv[i]);
            exit(1);
        }
        sixfive(fd);
        close(fd);
    }

}
