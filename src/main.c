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

  // how to import and call a function
  // printf("Factorial of 4 is %d\n", fact(4));

  // uncomment and run "make v" to see how valgrind detects memory leaks
  // createaleak();

  char map_name[50];
  int origin_option;

  // Demanem el nom del mapa
  printf("Enter map name (e.g. 'xs_2' or 'xl_1'): ");
  scanf("%49s", map_name);

  // Carreguem les cases de la memòria
  House *houses_list = load_houses(map_name);
  if (houses_list == NULL) {
    return 1; // Si ha hagut un error, sortim
  }
  // Carreguem els llocs de la memòria
  Place *places_list = load_places(map_name);
  if (places_list == NULL) {
    return 1; // Si ha hagut un error, sortim
  }
  // Mostrar el menú ORIGIN
  printf("\n--- ORIGIN ---\n");
  printf("Where are you? Address (1), Place (2) or Coordinate (3)? ");
  scanf("%d", &origin_option);

  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;

  // Reacciona a l'elecció del usuari
  if (origin_option == 3) {
    printf("Not implemented yet\n");
  } else if (origin_option == 1) {
    printf("Comming soon: Ask for street and number...\n");
  } else if (origin_option == 2) {
    char place_name[100];
    printf("Enter place name (e.g. \"Universitat Pompeu Fabra–Campus del "
           "Poblenou\"): ");

    scanf("%99[^\n]", place_name);

    // Busquem a la llista
    Place *found = find_place(places_list, place_name);

    if (found != NULL) {
      printf("\n    Found at (%f, %f)\n", found->lat, found->lon);
    } else {
      printf("\n    Place not found.\n");
    }
  } else {
    printf("Invalid option.\n");
  }

  return 0;
}