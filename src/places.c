#include "places.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Carregar llocs a la memòria
Place *load_places(const char *map_name) {
    char filepath[100];
    sprintf(filepath, "maps/%s/places.txt", map_name);

    // Obrir arxiu
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        printf("Error: No es pot obrir %s\n", filepath);
        return NULL;
    }

    Place *head = NULL;
    int count = 0;

    char id_str[100];
    char name[100];
    char type_str[100];
    double lat, lon;

    // Llegir 5 columnes
    while (fscanf(file, "%99[^,],%99[^,],%99[^,],%lf,%lf\n", id_str, name, type_str, &lat, &lon) == 5) {
        // Crear nou lloc
        Place *new_place = (Place *)malloc(sizeof(Place));

        // Guardar dades
        strcpy(new_place->id, id_str);
        strcpy(new_place->name, name);
        strcpy(new_place->type, type_str);
        new_place->lat = lat;
        new_place->lon = lon;

        // Inserir al principi
        new_place->next = head;
        head = new_place;

        count++;
    }

    fclose(file);
    printf("%d places loaded\n", count);
    return head;
}

// Buscar lloc (LAB 3)
void search_place(Place *head, const char *search_name) {
    char norm_search[200];
    normalize_string(search_name, norm_search);

    Place *current = head;
    int min_dist = 999;
    char suggestion[100] = "";

    // Recórrer llista
    while (current != NULL) {
        char norm_current[200];
        normalize_string(current->name, norm_current);
        
        // Coincidència exacta
        if (strcmp(norm_current, norm_search) == 0) {
            printf("\n    Found at (%f, %f)\n", current->lat, current->lon);
            return;
        }
        
        // Calcular Levenshtein per suggeriments
        int d = levenshtein(norm_search, norm_current);
        if (d < min_dist) { 
            min_dist = d; 
            strcpy(suggestion, current->name); 
        }
        
        current = current->next;
    }
    
    // Si no ho troba
    printf("\n    Place not found.\n");
    if (min_dist < 15) {
        printf("    Did you mean: '%s'?\n", suggestion);
    }
}
