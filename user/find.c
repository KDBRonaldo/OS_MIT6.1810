#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char* path, char* name);
void find_exe(char* path, char* name, char* cmd, int num_args, char** args);


char* fmtname(char *path)
{
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  return p;
}

int main(int argc, char* argv[]){
    if(argc < 3){
        printf("wrong number of parameters!\n");
        exit(1);
    }
    if(argc == 3){
        find(argv[1], argv[2]); // call find
    }
    else if((strcmp(argv[3], "-exec") == 0) && (argc >= 5)){ // find . wc -exec echo hi
        if(argc == 5){
            find_exe(argv[1], argv[2], argv[4], 0, 0); 
        }
        else{
            int i;
            char *c[100];
            int num_args = 0;
            for(i = 5; i < argc; i++){
                c[i-5] = argv[i];
                num_args++;
            }
            find_exe(argv[1], argv[2], argv[4], num_args, c);
        }
    }
    else{
        printf("invalid command!\n");
        exit(1);
    }

    exit(0);
}

int find_match(char* name, char* target){
    if(strcmp(name, target) == 0){
        return 1;
    }
    return 0;
}

void find(char* path, char* name){
    char buf[512];
    char *p;
    struct dirent de;
    struct stat st;
    struct stat cur_st;
    int fd;
    // open the path
    if((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "cannot open %s\n", path);
        return ;
    }
    // get the information about the path
    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return ;
    }

    switch(st.type){
        case T_DEVICE:
        case T_FILE:
            // if the file name match, printf it
            if(find_match(fmtname(path), name)){
                printf("%s\n", path);
            }
            break;
        // if this is a directory, recursively goes down to find 
        case T_DIR:
            // check if length of the path is too long
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("find: path too long\n");
                break;
            }
            // get the new path
            strcpy(buf, path);
            p = buf + strlen(buf);
            *(p++) = '/';
            // loop read the sub files under this directory
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0 ){ 
                    continue;
                }
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0){ // skip . and ..
                    continue;
                }
                if(stat(buf, &cur_st) < 0){
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                
                switch(cur_st.type){
                    case T_DEVICE:
                    case T_FILE:
                        if(find_match(fmtname(buf), name)){
                            printf("%s\n", buf);
                        }
                        break;
                    case T_DIR:
                        find(buf, name); // do it recursively
                }                

            }
    }
    close(fd);
} 

void find_exe(char* path, char* name, char* cmd, int num_args, char** args){
    char buf[512];
    char *p;
    struct dirent de;
    struct stat st;
    struct stat cur_st;
    int fd;
    // open the path
    if((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "cannot open %s\n", path);
        return ;
    }
    // get the information about the path
    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return ;
    }

    switch(st.type){
        case T_DEVICE:
        case T_FILE:
            // if the file name match, printf it
            if(find_match(fmtname(path), name)){
                int pid;
                int i;
                char *exec_args[100];

                exec_args[0] = cmd;
                for(i = 0; i < num_args; i++){
                    exec_args[i+1] = args[i];
                }
                exec_args[num_args + 1] = path;
                exec_args[num_args + 2] = 0;

                pid = fork();
                // for child process
                if(pid == 0){
                    exec(cmd, exec_args);
                    fprintf(2, "exec %s failed\n", cmd);
                    exit(1);
                }
                else{ // for parent process
                    wait(0);
                }
            }
            break;
        // if this is a directory, recursively goes down to find 
        case T_DIR:
            // check if length of the path is too long
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("find: path too long\n");
                break;
            }
            // get the new path
            strcpy(buf, path);
            p = buf + strlen(buf);
            *(p++) = '/';
            // loop read the sub files under this directory
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0 ){ 
                    continue;
                }
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0){ // skip . and ..
                    continue;
                }
                if(stat(buf, &cur_st) < 0){
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                
                switch(cur_st.type){
                    case T_DEVICE:
                    case T_FILE:
                        if(find_match(fmtname(buf), name)){
                            int pid;
                            int i;
                            char *exec_args[100];

                            exec_args[0] = cmd;
                            for(i = 0; i < num_args; i++){
                                exec_args[i+1] = args[i];
                            }
                            exec_args[num_args + 1] = buf;
                            exec_args[num_args + 2] = 0;

                            pid = fork();
                            // for child process
                            if(pid == 0){
                                exec(cmd, exec_args);
                                fprintf(2, "exec %s failed\n", cmd);
                                exit(1);
                            }
                            else{ // for parent process
                                wait(0);
                            }
                        }
                        break;
                    case T_DIR:
                        find_exe(buf, name, cmd, num_args, args); // do it recursively
                }                

            }
    }
    close(fd);
}