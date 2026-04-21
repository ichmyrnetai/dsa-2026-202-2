#ifndef HOUSES_H
#define HOUSES_H

// Aquesta és la estructura de House
typedef struct House {
  char street_name[100]; // Nom del carrer
  int house_number;      // Número de la casa
  double lat;            // Latitud
  double lon;            // Longitud
  struct House *next;    // Punter a la següent casa
} House;

// Declarem la funció
House *load_houses(const char *map_name);
void search_address(House* head, const char* street_name, int number);

#endif