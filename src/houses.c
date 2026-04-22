#include "houses.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Carregar les cases a la memòria
House *load_houses(const char *map_name) {
  char filepath[100];
  sprintf(filepath, "maps/%s/houses.txt", map_name);

  // Obrir l'arxiu
  FILE *file = fopen(filepath, "r");
  if (file == NULL) {
    printf("Error: No es pot obrir l'arxiu %s\n", filepath);
    return NULL;
  }

  House *head = NULL; // Inici de la llista
  int count = 0;      // Comptador

  char street[100];
  int number;
  double lat, lon;

  // Llegir línia per línia (4 columnes)
  while (fscanf(file, "%99[^,],%d,%lf,%lf\n", street, &number, &lat, &lon) == 4) {
    
    // Crear nova casa
    House *new_house = (House *)malloc(sizeof(House));

    // Omplir dades
    strcpy(new_house->street_name, street);
    new_house->house_number = number;
    new_house->lat = lat;
    new_house->lon = lon;

    // Inserir al principi de la llista
    new_house->next = head;
    head = new_house;

    count++;
  }

  fclose(file);
  printf("%d houses loaded\n", count);
  return head;
}

// Buscar adreça (LAB 2)
void search_address(House *head, const char *search_street, int search_number) {
    char norm_search[200];
    normalize_string(search_street, norm_search);

    House *current = head;
    int street_found = 0;
    int min_dist = 999;
    char suggestion[100] = "";

    // Recórrer la llista
    while (current != NULL) {
        char norm_current[200];
        normalize_string(current->street_name, norm_current);

        // Si coincideix el carrer
        if (strcmp(norm_current, norm_search) == 0) {
            street_found = 1;
            
            // Si també coincideix el número
            if (current->house_number == search_number) {
                printf("\n    Found at (%f, %f)\n", current->lat, current->lon);
                return;
            }
        } 
        // Si no coincideix el carrer, buscar suggeriments
        else if (!street_found) {
            int d = levenshtein(norm_search, norm_current);
            if (d < min_dist) {
                min_dist = d;
                strcpy(suggestion, current->street_name);
            }
        }
        current = current->next;
    }

    // Mostrar resultats d'error
    if (street_found) {
        printf("\n    Street found, but number %d does not exist.\n", search_number);
    } else {
        printf("\n    Street not found.\n");
        if (min_dist < 15) {
            printf("    Did you mean: '%s'?\n", suggestion);
        }
    }
}