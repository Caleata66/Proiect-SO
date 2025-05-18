#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "treasure.h"

void add(const char hunt_id[]){
    struct stat st = {0};

    char log_path[256];
    snprintf(log_path, sizeof(log_path), "%s/logged_hunt.txt", hunt_id); 
    int log;

    if(stat(hunt_id, &st) == -1){
        if(mkdir(hunt_id, 0755) == -1){
            perror("Error creating directory");
            exit(-1);
        }
        log = creat(log_path, 0644);
        if(log == -1){
            perror("Error creating logged_hunt");
            exit(-1);
        }
        close(log);

        char sl_path[256];
        snprintf(sl_path, sizeof(sl_path), "logged_hunt-%s", hunt_id);

        symlink(log_path, sl_path);
    }

    Treasure treasure;
    printf("Id: ");
    scanf("%d", &treasure.treasure_id);
    printf("user name: ");
    scanf("%31s", treasure.user_name);
    printf("latitude: ");
    scanf("%f", &treasure.coordinates.latitude);
    printf("longitude: ");
    scanf("%f", &treasure.coordinates.longitude);
    printf("value: ");
    scanf("%d", &treasure.value);
    printf("clue: ");
    fgetc(stdin);
    fgets(treasure.clue, sizeof(treasure.clue), stdin);
    treasure.clue[strlen(treasure.clue) - 1] = '\0';
    

    char treasures_path[256];
    snprintf(treasures_path, sizeof(treasures_path), "%s/treasures.dat", hunt_id);   

    int file = open(treasures_path, O_CREAT | O_APPEND | O_WRONLY, 0744);

    if(write(file, &treasure, sizeof(Treasure)) != sizeof(Treasure)){
        perror("Error writing file");
        exit(-1);
    }

    close(file);

    log = open(log_path, O_APPEND | O_WRONLY);

    dprintf(log, "Added treasure %d\n", treasure.treasure_id);

    close(log);
}

void list(char hunt_id[]){
    printf("%s\n", hunt_id);

    struct stat st = {0};
    if(stat(hunt_id, &st) != 0){
        perror("Stat failed");
        exit(-1);
    }

    printf("%ld\n", st.st_size);

    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s/treasures.dat", hunt_id);

    if(stat(file_path, &st) != 0){
        perror("Stat failed 2");
    }
    printf("%s\n", ctime(&st.st_mtime));

    int file = open(file_path, O_RDONLY);
    Treasure treasure;
    int count = 0;
    while(read(file, &treasure, sizeof(Treasure)) == sizeof(Treasure)){
        printf("Id: %d\n", treasure.treasure_id);
        printf("User name: %s\n", treasure.user_name);
        printf("Latitude: %g\n", treasure.coordinates.latitude);
        printf("Longitude: %g\n", treasure.coordinates.longitude);
        printf("Clue: %s\n", treasure.clue);
        printf("Value: %d\n\n", treasure.value);
        count++;
    }
    printf("Number of treasures: %d\n\n", count);
    close(file);

    char log_path[256];
    snprintf(log_path, sizeof(log_path), "%s/logged_hunt.txt", hunt_id); 
    int log;

    log = open(log_path, O_APPEND | O_WRONLY);
    dprintf(log, "List\n");
    close(log);
}

void view(char hunt_id[], int id){
    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s/treasures.dat", hunt_id);

    int file = open(file_path, O_RDONLY);
    Treasure treasure;
    int flag = 0;
    while(read(file, &treasure, sizeof(Treasure)) == sizeof(Treasure)){
        if(treasure.treasure_id == id){
            printf("Id: %d\n", treasure.treasure_id);
            printf("User name :%s\n", treasure.user_name);
            printf("Latitude: %g\n", treasure.coordinates.latitude);
            printf("Longitude: %g\n", treasure.coordinates.longitude);
            printf("Clue: %s\n", treasure.clue);
            printf("Value: %d\n\n", treasure.value);
            flag = 1;
            break;
        }
    }
    close(file);

    if(flag == 0){
        printf("Id not found\n");
    }

    char log_path[256];
    snprintf(log_path, sizeof(log_path), "%s/logged_hunt.txt", hunt_id); 
    int log;

    log = open(log_path, O_APPEND | O_WRONLY);
    dprintf(log, "View treasure %d\n", id);
    close(log);
}

void remove_treasure(char hunt_id[], int id){
    char treasures_path[256];
    snprintf(treasures_path, sizeof(treasures_path), "%s/treasures.dat", hunt_id);   

    int file = open(treasures_path, O_RDONLY);

    char treasures_path_tmp[256];
    snprintf(treasures_path_tmp, sizeof(treasures_path_tmp), "%s/treasuresTmp.dat", hunt_id);   

    int file_tmp = open(treasures_path_tmp, O_CREAT | O_WRONLY | O_APPEND, 0744);

    Treasure treasure;

    int bytes_read;
    bytes_read = read(file, &treasure, sizeof(Treasure));
    while ( bytes_read > 0) {
        if (bytes_read != sizeof(Treasure)) {
            fprintf(stderr, "Incomplete record read\n");
            break;
        }
        if (treasure.treasure_id != id) {
            write(file_tmp, &treasure, sizeof(Treasure));
        }
        bytes_read = read(file, &treasure, sizeof(Treasure));
    }

    close(file);
    close(file_tmp);

    unlink(treasures_path);
    if(rename(treasures_path_tmp, treasures_path) != 0){
        perror("Error renaming file");
        exit(-1);
    }

    char log_path[256];
    snprintf(log_path, sizeof(log_path), "%s/logged_hunt.txt", hunt_id); 
    int log;

    log = open(log_path, O_APPEND | O_WRONLY);
    dprintf(log, "Remove treasure %d\n", id);
    close(log);
}

void remove_hunt(char hunt_id[]){
    
    char log_path[256];
    snprintf(log_path, sizeof(log_path), "%s/logged_hunt.txt", hunt_id); 
    if(unlink(log_path) == -1){
        perror("Error removing file");
        exit(-1);
    }

    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s/treasures.dat", hunt_id);
    if(unlink(file_path) == -1){
        perror("Error removing file");
        exit(-1);
    }

    

    if(rmdir(hunt_id) == -1){
        perror("Error removing directory");
        exit(-1);
    }
}