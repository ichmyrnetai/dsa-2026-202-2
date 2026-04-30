#ifndef STREETS_H
#define STREETS_H
#include "position.h"

typedef struct street {
    long long from_id;
    Position from_pos;
    long long to_id;
    Position to_pos;
    double length;
    char name[100];
    struct street* next;
} Street;

Street* load_streets(const char* map_name, int* count);
Street* find_closest_street(Street* head, Position user_pos);
void print_connected_streets(Street* head, Street* current);

#endif