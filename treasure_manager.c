#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "treasure.h"

int main(int argc, char** argv){

    if(strcmp(argv[1], "--add") == 0){
        if(argc != 3){
            perror("wrong number of arguments");
            exit(-1);
        }
        add(argv[2]);
        return 0;
    }

    if(strcmp(argv[1], "--list") == 0){
        if(argc != 3){
            perror("wrong number of arguments");
            exit(-1);
        }
        list(argv[2]);
        return 0;
    }

    if(strcmp(argv[1], "--view") == 0){
        if(argc != 4){
            perror("wrong number of arguments");
            exit(-1);
        }
        view(argv[2], atoi(argv[3]));
        return 0;
    }

    if(strcmp(argv[1], "--remove_hunt") == 0){
        if(argc != 3){
            perror("wrong number of arguments");
            exit(-1);
        }
        remove_hunt(argv[2]);
        return 0;
    }

    if(strcmp(argv[1], "--remove_treasure") == 0){
        if(argc != 4){
            perror("wrong number of arguments");
            exit(-1);
        }
        remove_treasure(argv[2], atoi(argv[3]));
        return 0;
    }

    return 0;
}