#include "houses.h"
#include "places.h"
#include "utils.h"
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
  printf("*****************\nWelcome to DSA!\n*****************\n");

  char map_name[50];
  int origin_option;

  // Demanar el nom del mapa
  printf("Enter map name (e.g. 'xs_2' or 'xl_1'): ");
  scanf("%49s", map_name);

  // Carregar cases
  House *houses_list = load_houses(map_name);
  if (houses_list == NULL) return 1; 

  // Carregar llocs
  Place *places_list = load_places(map_name);
  if (places_list == NULL) return 1; 

  // Mostrar menú
  printf("\n--- ORIGIN ---\n");
  printf("Where are you? Address (1), Place (2) or Coordinate (3)? ");
  scanf("%d", &origin_option);

  // Netejar buffer
  int c;
  while ((c = getchar()) != '\n' && c != EOF);

  // Gestionar opcions
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
    search_address(houses_list, street_name, house_number);
  } 
  else if (origin_option == 2) {
    char place_name[100];
    
    // Demanar lloc
    printf("Enter place name (e.g. \"Universitat Pompeu Fabra–Campus del Poblenou\"): ");
    scanf("%99[^\n]", place_name); 

    // Buscar lloc (crida a places.c)
    search_place(places_list, place_name);
  } 
  else {
    printf("Invalid option.\n");
  }

  return 0;
}