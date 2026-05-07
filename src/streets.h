#ifndef STREETS_H
#define STREETS_H
#include "position.h"
#define HASH_SIZE 10007

typedef struct street {
    long long from_id;
    Position from_pos;
    long long to_id;
    Position to_pos;
    double length;
    char name[100];
    struct street* next;
} Street;

// Estructura Hash Map
typedef struct hash_node {
    long long intersection_id; 
    Street* connected_streets; 
    struct hash_node* next;    
} HashNode;

Street* load_streets(const char* map_name, int* count);
Street* find_closest_street(Street* head, Position user_pos);
void print_connected_streets(Street* head, Street* current);

//Funcions Hash Map
HashNode** create_hash_map();
void insert_to_hash_map(HashNode** hash_map, long long from_id, Street* street);
HashNode** build_intersection_graph(Street* streets_list);
void print_connected_streets_fast(HashNode** hash_map, Street* current);

// Càlcul de rutes
void calculate_route(HashNode** hash_map, Street* start_street, Street* end_street);

#endif