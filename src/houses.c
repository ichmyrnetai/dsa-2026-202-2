#include "houses.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Funció per llegir l'arxiu i crear la linked list
House *load_houses(const char *map_name) {
  // Construim la ruta de l'arxiu
  char filepath[100];
  sprintf(filepath, "maps/%s/houses.txt", map_name);

  // Obrim l'arxiu en mode lectura
  FILE *file = fopen(filepath, "r");
  if (file == NULL) {
    printf("Error: No se pudo abrir el archivo %s\n", filepath);
    return NULL;
  }

  House *head = NULL; // L'inici de la nostra llista
  int count = 0;      // Per contar cuantes cases carreguem

  // Variables temporals per llegir cada línea
  char street[100];
  int number;
  double lat, lon;

  // Llegim línea per línea fins arribar al final del arxiu
  while (fscanf(file, "%[^,],%d,%lf,%lf\n", street, &number, &lat, &lon) == 4) {

    // Creem una nova casa a partir de l'estructura House
    House *new_house = (House *)malloc(sizeof(House));

    // Amb les dades que acabem de llegir omplim les dades de la nova casa
    // creada
    strcpy(new_house->street_name, street);
    new_house->house_number = number;
    new_house->lat = lat;
    new_house->lon = lon;

    // Posem la casa al principi de la llista
    new_house->next = head;
    head = new_house;

    count++;
  }

  fclose(file);
  printf("%d houses loaded\n", count);
  return head; // Tornem el primer element de la llista
}