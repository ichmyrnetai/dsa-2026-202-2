#ifndef UTILS_H
#define UTILS_H

// Normalitza el text
void normalize_string(const char* input, char* output);

// Calcula la distancia de Levenshtein entre dos texts
int levenshtein(const char *s1, const char *s2);

#endif