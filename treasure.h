#ifndef TREASURE_H
#define TREASURE_H

typedef struct{
    float latitude;
    float longitude;
}Coordinates;

typedef struct{
    int treasure_id;
    char user_name[32];
    Coordinates coordinates;
    char clue[256];
    int value;
}Treasure;

void add(const char hunt_id[]);
void list(char hunt_id[]);
void view(char hunt_id[], int id);
void remove_treasure(char hunt_id[], int id);
void remove_hunt(char hunt_id[]);

#endif