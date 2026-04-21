#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "places.h"

Place* load_places(const char* map_name) {
    char filepath[100];
    sprintf(filepath, "maps/%s/places.txt", map_name);

    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        printf("Error: No se pudo abrir %s\n", filepath);
        return NULL;
    }

    Place* head = NULL;
    int count = 0;

    // Variables temporals per les 5 columnes
    char id_str[100];
    char name[100];
    char type_str[100];
    double lat, lon;

    // Llegim les 5 columnes
    while (fscanf(file, "%99[^,],%99[^,],%99[^,],%lf,%lf\n", id_str, name, type_str, &lat, &lon) == 5) {
        Place* new_place = (Place*)malloc(sizeof(Place));
        
        // Guardem el nom i les coordenades
        strcpy(new_place->name, name);
        new_place->lat = lat;
        new_place->lon = lon;
        
        new_place->next = head;
        head = new_place;
        
        count++;
    }

    fclose(file);
    printf("%d places loaded\n", count);
    return head;
}

// Funció per buscar un lloc a la llista pel seu nom exacte
Place* find_place(Place* head, const char* search_name) {
    Place* current = head;
    while (current != NULL) {
        if (strcmp(current->name, search_name) == 0) {
            return current; 
        }
        current = current->next;
    }
    return NULL; 
}