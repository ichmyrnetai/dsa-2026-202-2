#ifndef PLACES_H
#define PLACES_H

// Estructura per als llocs
typedef struct Place {
  char id[100];   // ID
  char name[100]; // Nom del lloc
  char type[100]; // Tipus de lloc
  double lat;     // Latitud
  double lon;     // Longitud
  struct Place *next; // Punter al següent lloc
} Place;

// Declaració de funcions
Place *load_places(const char *map_name);
void search_place(Place* head, const char* search_name);

#endif