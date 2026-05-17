#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[]){
    // if user does not pass in any parameters
    if(argc != 2){
        fprintf(2, "please put in paremeters like: sleep [ticks]\n"); 
        exit(1);
    }
    pause(atoi(argv[1]));
    exit(0);
}