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