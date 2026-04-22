#ifndef HOUSES_H
#define HOUSES_H

// Estructura per a les cases
typedef struct House {
  char street_name[100]; // Nom del carrer
  int house_number;      // Número de la casa
  double lat;            // Latitud
  double lon;            // Longitud
  struct House *next;    // Punter a la següent casa
} House;

// Declaració de funcions
House *load_houses(const char *map_name);
void search_address(House* head, const char* street_name, int number);

#endif