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

    int score = 0;

    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s/treasures.dat", argv[1]);

    int file = open(file_path, O_RDONLY);
    Treasure treasure;

    while(read(file, &treasure, sizeof(Treasure)) == sizeof(Treasure)){
        if(strcmp(treasure.user_name, argv[2]) == 0){
            score += treasure.value;
        }
    }

    printf("user: %s\nscore: %d\n", argv[2], score);
}