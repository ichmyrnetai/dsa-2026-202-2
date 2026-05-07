#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "streets.h"

#define EARTH_RADIUS 6371.0
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Converteix de graus a radians
double toRadians(double degree) { 
    return degree * (M_PI / 180.0); 
}

// Calcula la distància entre dues coordenades
double haversine(Position posA, Position posB) {
    double lat1 = toRadians(posA.lat);
    double lon1 = toRadians(posA.lon);
    double lat2 = toRadians(posB.lat);
    double lon2 = toRadians(posB.lon);

    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;
    double a = pow(sin(dLat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dLon / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS * c * 1000.0; 
}

// Calcula punt mig
Position midpoint(Position a, Position b) {
    Position mid;
    // Aproximació
    mid.lat = (a.lat + b.lat) / 2.0;
    mid.lon = (a.lon + b.lon) / 2.0;
    return mid;
}

Street* load_streets(const char* map_name, int* count) {
    char path[150];
    sprintf(path, "maps/%s/streets.txt", map_name);
    FILE* file = fopen(path, "r");
    
    if (!file) {
        printf("Error: No s'ha pogut obrir el fitxer %s\n", path);
        return NULL;
    }

    Street* head = NULL;
    *count = 0;
    char line[512];
    
    // Llegim línea per línea
    while (fgets(line, sizeof(line), file)) {
        Street* s = malloc(sizeof(Street));
        
        int llegits = sscanf(line, "%lld,%lf,%lf,%lld,%lf,%lf,%lf,%[^\n]", 
               &s->from_id, &s->from_pos.lat, &s->from_pos.lon,
               &s->to_id, &s->to_pos.lat, &s->to_pos.lon,
               &s->length, s->name);
               
        if (llegits == 8) {
            // Netegem qualsevol \r o \n del nom del carrer perquè s'imprimeixi bé
            s->name[strcspn(s->name, "\r\n")] = 0;

            s->next = head;
            head = s;
            (*count)++;
        } else {
            free(s);
        }
    }
    
    fclose(file);
    return head;
}

Street* find_closest_street(Street* head, Position user_pos) {
    Street* closest = NULL;
    double min_dist = 999999999.0; 
    Street* curr = head;
    
    while (curr) {
        Position mid = midpoint(curr->from_pos, curr->to_pos);
        double dist = haversine(user_pos, mid);
        
        if (dist <= min_dist) {
            min_dist = dist;
            closest = curr;
        }
        curr = curr->next;
    }
    return closest;
}

void print_connected_streets(Street* head, Street* current) {
    printf("    From this street segment, you can go to:\n");
    
    Street* curr1 = head;
    char printed_l1[10][100];
    int count_l1 = 0;

    // El següent segment
    while (curr1) {
        if (curr1->from_id == current->to_id) {
            
            // Comprovem si ja l'hem imprès abans
            int already_printed_l1 = 0;
            for (int i = 0; i < count_l1; i++) {
                if (strcmp(printed_l1[i], curr1->name) == 0) {
                    already_printed_l1 = 1; break;
                }
            }
            
            if (!already_printed_l1) {
                printf("    - %s\n", curr1->name);
                strcpy(printed_l1[count_l1++], curr1->name);
                
                // Mirem si té carrers creuats
                int has_cross = 0;
                Street* temp = head;
                while (temp) {
                    if (temp->from_id == curr1->to_id && strcmp(temp->name, curr1->name) != 0) {
                        has_cross = 1;
                        break;
                    }
                    temp = temp->next;
                }
                
                
                if (has_cross) {
                    printf("        Which is connected to:\n");
                    Street* curr2 = head;
                    char printed_l2[10][100];
                    int count_l2 = 0;
                    
                    while (curr2) {
                        // Que estigui connectat i el nom sigui diferent
                        if (curr2->from_id == curr1->to_id && strcmp(curr2->name, curr1->name) != 0) {
                            
                            int already_printed_l2 = 0;
                            for (int i = 0; i < count_l2; i++) {
                                if (strcmp(printed_l2[i], curr2->name) == 0) {
                                    already_printed_l2 = 1; break;
                                }
                            }
                            
                            if (!already_printed_l2) {
                                printf("         - %s\n", curr2->name);
                                strcpy(printed_l2[count_l2++], curr2->name);
                            }
                        }
                        curr2 = curr2->next;
                    }
                }
            }
        }
        curr1 = curr1->next;
    }
} 
// Mòdul ID entre mida de la taula
unsigned int hash_function(long long id) {
    return (unsigned int)(id % HASH_SIZE);
}

// Crear una taula Hash buida
HashNode** create_hash_map() {
    HashNode** hash_map = malloc(HASH_SIZE * sizeof(HashNode*));
    for (int i = 0; i < HASH_SIZE; i++) {
        hash_map[i] = NULL;
    }
    return hash_map;
}

// Inserir un carrer al Hash Map
void insert_to_hash_map(HashNode** hash_map, long long intersection_id, Street* street) {
    unsigned int index = hash_function(intersection_id);
    
    // Buscar si la intersecció ja existeix a la llista d'aquest index
    HashNode* curr = hash_map[index];
    while (curr != NULL) {
        if (curr->intersection_id == intersection_id) {
            // La intersecció existeix, afegim el carrer a la seva llista local
            // Fem una còpia del carrer per no trencar la llista original
            Street* new_s = malloc(sizeof(Street));
            *new_s = *street; 
            new_s->next = curr->connected_streets;
            curr->connected_streets = new_s;
            return;
        }
        curr = curr->next;
    }

    // Si la intersecció no existeix, creem un nou node per al Hash Map
    HashNode* new_node = malloc(sizeof(HashNode));
    new_node->intersection_id = intersection_id;
    
    // Creem la primera còpia del carrer per aquesta intersecció
    Street* new_s = malloc(sizeof(Street));
    *new_s = *street;
    new_s->next = NULL;
    new_node->connected_streets = new_s;
    
    // L'afegim a la taula Hash
    new_node->next = hash_map[index];
    hash_map[index] = new_node;
}

// Construir el Hash Map a partir de la llista de carrers
HashNode** build_intersection_graph(Street* streets_list) {
    HashNode** hash_map = create_hash_map();
    Street* curr = streets_list;
    
    while (curr != NULL) {
        insert_to_hash_map(hash_map, curr->from_id, curr);
        curr = curr->next;
    }
    return hash_map;
}

// Funció ràpida
void print_connected_streets_fast(HashNode** hash_map, Street* current) {
    printf("    From this street segment, you can go to:\n");
    
    // Busquem l'ID on acaba el nostre carrer directament al Hash Map
    unsigned int index = hash_function(current->to_id);
    HashNode* node_l1 = hash_map[index];
    
    // Trobar la intersecció exacta 
    while (node_l1 != NULL && node_l1->intersection_id != current->to_id) {
        node_l1 = node_l1->next;
    }

    if (node_l1 == NULL) return; // Si no hi ha res connectat, acabem

    char printed_l1[10][100];
    int count_l1 = 0;

    // Recorrem els carrers que surten d'aquí
    Street* curr1 = node_l1->connected_streets;
    while (curr1) {
        int already_printed_l1 = 0;
        for (int i = 0; i < count_l1; i++) {
            if (strcmp(printed_l1[i], curr1->name) == 0) {
                already_printed_l1 = 1; break;
            }
        }
        
        if (!already_printed_l1) {
            printf("    - %s\n", curr1->name);
            strcpy(printed_l1[count_l1++], curr1->name);
            
            // Busquem les connexions del següent carrer
            unsigned int index2 = hash_function(curr1->to_id);
            HashNode* node_l2 = hash_map[index2];
            
            while (node_l2 != NULL && node_l2->intersection_id != curr1->to_id) {
                node_l2 = node_l2->next;
            }

            if (node_l2 != NULL) {
                int has_cross = 0;
                Street* temp = node_l2->connected_streets;
                while (temp) {
                    if (strcmp(temp->name, curr1->name) != 0) {
                        has_cross = 1; break;
                    }
                    temp = temp->next;
                }
                
                if (has_cross) {
                    printf("        Which is connected to:\n");
                    Street* curr2 = node_l2->connected_streets;
                    char printed_l2[10][100];
                    int count_l2 = 0;
                    
                    while (curr2) {
                        if (strcmp(curr2->name, curr1->name) != 0) {
                            int already_printed_l2 = 0;
                            for (int i = 0; i < count_l2; i++) {
                                if (strcmp(printed_l2[i], curr2->name) == 0) {
                                    already_printed_l2 = 1; break;
                                }
                            }
                            if (!already_printed_l2) {
                                printf("         - %s\n", curr2->name);
                                strcpy(printed_l2[count_l2++], curr2->name);
                            }
                        }
                        curr2 = curr2->next;
                    }
                }
            }
        }
        curr1 = curr1->next;
    }
}