#include "houses.h"
#include "places.h"
#include "utils.h"
#include "position.h"
#include "streets.h"
#include "sample_lib.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void createaleak() {
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}

int main() {
  char map_name[50];
  int origin_option;
  int dest_option;

  // Demanar el nom del mapa
  printf("Enter map name (e.g. 'xs_2' or 'xl_1'): ");
  scanf("%49s", map_name);

  // Carregar cases
  House *houses_list = load_houses(map_name);
  if (houses_list == NULL) return 1; 

  // Carregar llocs
  Place *places_list = load_places(map_name);
  if (places_list == NULL) return 1; 

  // Carregar carrers
  int streets_count = 0;
  Street *streets_list = load_streets(map_name, &streets_count);
  if (streets_list == NULL) return 1;
  printf("%d streets loaded\n", streets_count);

  // Construir el Graf (Hash Map)
  HashNode** intersection_graph = build_intersection_graph(streets_list);

  Position origin_pos;
  int found_origin = 0;
  Position dest_pos;
  int found_dest = 0;

  // Mostrar menú origen
  printf("\n--- ORIGIN ---\n");
  printf("Where are you? Address (1), Place (2) or Coordinate (3)? ");
  scanf("%d", &origin_option);

  // Netejar buffer
  int c;
  while ((c = getchar()) != '\n' && c != EOF);

  // Gestionar opcions origen
  if (origin_option == 3) {
    printf("Not implemented yet\n");
  } 
  else if (origin_option == 1) {
    char street_name[100];
    int house_number;

    // Demanar carrer
    printf("Enter street name (e.g. \"Carrer de Roc Boronat\"): ");
    scanf("%99[^\n]", street_name);
    
    // Demanar número
    printf("Enter street number (e.g. \"138\"): ");
    scanf("%d", &house_number);

    // Buscar adreça (crida a houses.c)
    House *found = search_address(houses_list, street_name, house_number);
    if (found != NULL) {
        origin_pos.lat = found->lat;
        origin_pos.lon = found->lon;
        found_origin = 1;
    }
  } 
  else if (origin_option == 2) {
    char place_name[100];
    
    // Demanar lloc
    printf("Enter place name (e.g. \"Universitat Pompeu Fabra-Campus del Poblenou\" or \"L'Illa Diagonal\"): ");
    scanf("%99[^\n]", place_name); 

    // Buscar lloc (crida a places.c)
    Place *found = search_place(places_list, place_name);
    if (found != NULL) {
        origin_pos.lat = found->lat;
        origin_pos.lon = found->lon;
        found_origin = 1;
    }
  } 
  else {
    printf("Invalid option.\n");
  }

  // Mostrar carrer més proper i connexions (Origen)
  if (found_origin) {
      Street* closest = find_closest_street(streets_list, origin_pos);
      if (closest) {
          printf("    Closest street: %s\n", closest->name);
          printf("    Between %lld (%f, %f) and %lld (%f, %f)\n\n",
                 closest->from_id, closest->from_pos.lat, closest->from_pos.lon,
                 closest->to_id, closest->to_pos.lat, closest->to_pos.lon);
          
          // Aquí cridem a la funció usant el Hash Map
          print_connected_streets_fast(intersection_graph, closest);
      }
  }

  // Mostrar menú destí
  if (found_origin) {
      printf("\n--- DESTINATION ---\n");
      printf("Where do you want to go? Address (1), Place (2) or Coordinate (3)? ");
      scanf("%d", &dest_option);

      // Netejar buffer
      while ((c = getchar()) != '\n' && c != EOF);

      // Gestionar opcions destí
      if (dest_option == 3) {
        printf("Not implemented yet\n");
      } 
      else if (dest_option == 1) {
        char street_name[100];
        int house_number;

        // Demanar carrer
        printf("Enter street name (e.g. \"Carrer de Roc Boronat\"): ");
        scanf("%99[^\n]", street_name);
        
        // Demanar número
        printf("Enter street number (e.g. \"138\"): ");
        scanf("%d", &house_number);

        // Buscar adreça 
        House *found = search_address(houses_list, street_name, house_number);
        if (found) {
            dest_pos.lat = found->lat;
            dest_pos.lon = found->lon;
            found_dest = 1;
        }
      } 
      else if (dest_option == 2) {
        char place_name[100];
        
        // Demanar lloc
        printf("Enter place name (e.g. \"L'Illa Diagonal\"): ");
        scanf("%99[^\n]", place_name); 

        // Buscar lloc 
        Place *found = search_place(places_list, place_name);
        if (found) {
            dest_pos.lat = found->lat;
            dest_pos.lon = found->lon;
            found_dest = 1;
        }
      } 
      else {
        printf("Invalid option.\n");
      }

      // Mostrar carrer més proper al destí
      if (found_dest) {
          Street* closest_dest = find_closest_street(streets_list, dest_pos);
          if (closest_dest) {
              printf("    Closest street: %s\n", closest_dest->name);
              printf("    Between %lld (%f, %f) and %lld (%f, %f)\n\n",
                     closest_dest->from_id, closest_dest->from_pos.lat, closest_dest->from_pos.lon,
                     closest_dest->to_id, closest_dest->to_pos.lat, closest_dest->to_pos.lon);
          }
      }
  }

  // Ruta
  if (found_origin && found_dest) {
      printf("\n--- ROUTE ---\n");
      
      Street* closest_origin = find_closest_street(streets_list, origin_pos);
      Street* closest_dest = find_closest_street(streets_list, dest_pos);
      
      if (closest_origin && closest_dest) {
          // Calculem i imprimim la ruta final fent servir BFS
          calculate_route(intersection_graph, closest_origin, closest_dest);
      } else {
          printf("  Error: No s'ha pogut calcular la ruta.\n");
      }
  }

  return 0;
}