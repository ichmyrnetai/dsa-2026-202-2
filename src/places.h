#ifndef PLACES_H
#define PLACES_H

typedef struct Place {
    char id[100];           // ID único (el churro de letras y números)
    char name[100];         // Nombre del lugar
    char type[100];         // Tipo (amenity:bank, shop:books, etc.)
    double lat;
    double lon;
    struct Place* next;
} Place;

Place* load_places(const char* map_name);
Place* find_place(Place* head, const char* search_name);

#endif