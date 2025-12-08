#ifndef BOOTSTRAP_LOADER_H
#define BOOTSTRAP_LOADER_H

#include <stddef.h>

/* Representación mínima de palabra bootstrap con triple FFE heurístico (FO,FN,ES) */
typedef struct {
    char word[32];
    int ffe[3]; /* Trits: 1,0,-1 */
} BootWord;

/* Carga lista de palabras (una por línea). Devuelve número cargado. */
size_t load_bootstrap_words(const char* path);
size_t get_bootstrap_count(void);
const BootWord* get_bootstrap_word(size_t i);

#endif /* BOOTSTRAP_LOADER_H */
