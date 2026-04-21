#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "utils.h"

void normalize_string(const char* input, char* output) {
    char temp[200];
    int i = 0;
    
    // 1. Pasar todo a minúsculas
    while (input[i] != '\0' && i < 199) {
        temp[i] = tolower((unsigned char)input[i]);
        i++;
    }
    temp[i] = '\0';

    // 2. Expandir abreviaturas comunes al principio del string
    if (strncmp(temp, "c. ", 3) == 0) {
        sprintf(output, "carrer %s", temp + 3);
    } else if (strncmp(temp, "av. ", 4) == 0) {
        sprintf(output, "avinguda %s", temp + 4);
    } else if (strncmp(temp, "ptge. ", 6) == 0) {
        sprintf(output, "passatge %s", temp + 6);
    } else {
        strcpy(output, temp);
    }
}

int levenshtein(const char *s1, const char *s2) {
    int m = strlen(s1);
    int n = strlen(s2);
    int d[150][150]; // Matriz estática (asumimos textos de < 150 caracteres)

    for (int i = 0; i <= m; i++) d[i][0] = i;
    for (int j = 0; j <= n; j++) d[0][j] = j;

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            int del = d[i - 1][j] + 1;
            int ins = d[i][j - 1] + 1;
            int sub = d[i - 1][j - 1] + cost;

            int min = del < ins ? del : ins;
            d[i][j] = min < sub ? min : sub;
        }
    }
    return d[m][n];
}