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
        // Calculem el punt mig del carrer
        Position mid = midpoint(curr->from_pos, curr->to_pos);
        // Calculem distància de l'usuari al punt mig
        double dist = haversine(user_pos, mid);
        
        // Actualitzem si està més a prop de la anterior
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
            Street* new_s = malloc(sizeof(Street));
            *new_s = *street; 
            new_s->next = curr->connected_streets;
            curr->connected_streets = new_s;
            return;
        }
        curr = curr->next;
    }

    // Si no existeix, creem un nou node
    HashNode* new_node = malloc(sizeof(HashNode));
    new_node->intersection_id = intersection_id;
    
    // Creem la primera còpia del carrer
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

// Funció ràpida (Lab 5)
void print_connected_streets_fast(HashNode** hash_map, Street* current) {
    printf("    From this street segment, you can go to:\n");
    
    // Busquem on acaba el carrer
    unsigned int index = hash_function(current->to_id);
    HashNode* node_l1 = hash_map[index];
    
    // Trobar la intersecció exacta 
    while (node_l1 != NULL && node_l1->intersection_id != current->to_id) {
        node_l1 = node_l1->next;
    }

    if (node_l1 == NULL) return; // Si no hi ha res, sortim

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

// Estructura per guardar la ruta pas a pas
typedef struct path_node {
    Street* street;
    struct path_node* prev;
} PathNode;

// Estructura de la Cua pel BFS
typedef struct queue_node {
    PathNode* path;
    struct queue_node* next;
} QueueNode;

// Estructura molt eficient de visitats
typedef struct visited_node {
    long long intersection_id;
    struct visited_node* next;
} VisitedNode;

// Funció calculate_route
void calculate_route(HashNode** hash_map, Street* start_street, Street* end_street) {
    if (!start_street || !end_street) return;

    // Inicialitzar la Cua i el set de visitats 
    VisitedNode** visited = calloc(HASH_SIZE, sizeof(VisitedNode*));
    QueueNode* front = NULL;
    QueueNode* rear = NULL;

    // Fiquem el primer carrer a la cua
    PathNode* start_path = malloc(sizeof(PathNode));
    start_path->street = start_street;
    start_path->prev = NULL;

    QueueNode* qn = malloc(sizeof(QueueNode));
    qn->path = start_path;
    qn->next = NULL;
    front = rear = qn;

    // Marquem l'inici com a visitat
    unsigned int start_idx = hash_function(start_street->to_id);
    VisitedNode* vn = malloc(sizeof(VisitedNode));
    vn->intersection_id = start_street->to_id;
    vn->next = visited[start_idx];
    visited[start_idx] = vn;

    PathNode* final_path = NULL;

    // Bucle principal BFS
    while (front != NULL) {
        // Traiem l'element de davant de la cua
        QueueNode* curr_qn = front;
        front = front->next;
        if (front == NULL) rear = NULL;

        PathNode* curr_path = curr_qn->path;
        Street* curr_street = curr_path->street;
        free(curr_qn);

        // Hem arribat al destí?
        if (curr_street->to_id == end_street->to_id || curr_street->to_id == end_street->from_id) {
            final_path = curr_path;
            break;
        }

        // Mirem les connexions ràpidament al Hash Map
        unsigned int idx = hash_function(curr_street->to_id);
        HashNode* node = hash_map[idx];
        while (node != NULL && node->intersection_id != curr_street->to_id) {
            node = node->next;
        }

        if (node != NULL) {
            Street* neighbor = node->connected_streets;
            while (neighbor != NULL) {
                // Mirem a O(1) si ja hem visitat la intersecció
                unsigned int n_idx = hash_function(neighbor->to_id);
                VisitedNode* v = visited[n_idx];
                int is_visited = 0;
                while (v) {
                    if (v->intersection_id == neighbor->to_id) {
                        is_visited = 1; break;
                    }
                    v = v->next;
                }

                // Si és nou, l'afegim a la ruta i a la cua
                if (!is_visited) {
                    VisitedNode* new_v = malloc(sizeof(VisitedNode));
                    new_v->intersection_id = neighbor->to_id;
                    new_v->next = visited[n_idx];
                    visited[n_idx] = new_v;

                    PathNode* next_path = malloc(sizeof(PathNode));
                    next_path->street = neighbor;
                    next_path->prev = curr_path;

                    QueueNode* new_qn = malloc(sizeof(QueueNode));
                    new_qn->path = next_path;
                    new_qn->next = NULL;
                    
                    if (rear) {
                        rear->next = new_qn;
                        rear = new_qn;
                    } else {
                        front = rear = new_qn;
                    }
                }
                neighbor = neighbor->next;
            }
        }
    }

    // Imprimir les instruccions clavades al format del professor
    if (final_path != NULL) {
        int len = 0;
        PathNode* curr = final_path;
        while (curr) {
            len++;
            curr = curr->prev;
        }

        // Passem de llista enllaçada inversa a un Array ordenat
        Street** route = malloc(len * sizeof(Street*));
        curr = final_path;
        for (int i = len - 1; i >= 0; i--) {
            route[i] = curr->street;
            curr = curr->prev;
        }

        printf("  Start at %s\n", route[0]->name);

        int i = 1;
        while (i < len) {
            Street* prev_seg = route[i-1];
            Street* curr_seg = route[i];
            
            char* current_name = curr_seg->name;
            double group_length = curr_seg->length;
            
            // Producte vectorial (Cross Product) per saber si girem a l'esquerra o dreta
            double dx1 = prev_seg->to_pos.lon - prev_seg->from_pos.lon;
            double dy1 = prev_seg->to_pos.lat - prev_seg->from_pos.lat;
            double dx2 = curr_seg->to_pos.lon - curr_seg->from_pos.lon;
            double dy2 = curr_seg->to_pos.lat - curr_seg->from_pos.lat;
            
            double cross = (dx1 * dy2) - (dy1 * dx2);
            char* turn_dir = cross > 0 ? "left" : "right";
            
            // Agrupem els trossos de carrer que tinguin exactament el mateix nom
            i++;
            while (i < len && strcmp(route[i]->name, current_name) == 0) {
                group_length += route[i]->length;
                i++;
            }
            
            printf("  Turn %s to %s and continue for %.0fm\n", turn_dir, current_name, group_length);
        }
        printf("  You have arrived to %s\n", route[len-1]->name);
        
        free(route);
    } else {
        printf("  No route found.\n");
    }

    // Alliberar memòria de la taula de visitats
    for (int i = 0; i < HASH_SIZE; i++) {
        VisitedNode* v = visited[i];
        while (v) {
            VisitedNode* next = v->next;
            free(v);
            v = next;
        }
    }
    free(visited);
}