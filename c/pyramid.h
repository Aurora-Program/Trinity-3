// pyramid.h - Pyramidal Knowledge Graph for recursive Aurora processing
#ifndef PYRAMID_H
#define PYRAMID_H

#include "tetraedro.h"

// Máximo número de niveles en la pirámide (3^0, 3^1, 3^2, 3^3...)
#define MAX_PYRAMID_LEVELS 4

// Nodo de conocimiento en el grafo piramidal
typedef struct PyramidNode {
    Vector vector;              // Vector sintetizado en este nodo
    int level;                  // Nivel en la pirámide (0=base, aumenta hacia cima)
    int index;                  // Índice dentro del nivel
    
    // Relaciones con los 3 vectores que lo generaron (nivel inferior)
    struct PyramidNode* sources[3];  // Punteros a nodos fuente
    Dim RRR[3];                 // Resultados de cada Tetraedro
    Dim MMM[3];                 // Modos aprendidos
    Dim OOO[3];                 // Órdenes armonizados
    
    // Relación con el nodo que contiene este (nivel superior)
    struct PyramidNode* parent;
    int parent_slot;            // 0, 1, o 2 - posición en el parent
    
    bool is_synthesized;        // Si ya fue procesado
    TetraedroOutput synthesis;  // Resultado del Tetraedro que lo creó
} PyramidNode;

// Estructura completa de la pirámide
typedef struct Pyramid {
    int num_levels;                          // Niveles actuales
    int nodes_per_level[MAX_PYRAMID_LEVELS]; // Cantidad de nodos por nivel
    PyramidNode** levels[MAX_PYRAMID_LEVELS]; // Array de nodos por nivel
    PyramidNode* peak;                       // Nodo cima (síntesis máxima)
} Pyramid;

// === Funciones principales ===

// Crear pirámide vacía
Pyramid* pyramid_create(void);

// Liberar memoria
void pyramid_destroy(Pyramid* p);

// === Ciclo Ascendente ===

// Iniciar con 3, 6, 9... vectores base
void pyramid_init_base(Pyramid* p, Vector* base_vectors, int count);

// Procesar un nivel: cada 3 vectores → 1 emergente
// Retorna el número de vectores emergentes generados
int pyramid_ascend_level(Pyramid* p, int level);

// Ascender completamente hasta la cima
void pyramid_ascend_to_peak(Pyramid* p);

// === Ciclo Descendente ===

// Expandir desde la cima usando Extender
void pyramid_descend_from_peak(Pyramid* p);

// Expandir un nivel específico
void pyramid_descend_level(Pyramid* p, int level);

// === Flujo Termodinámico ===

// Calcular entropía (nulls) de un vector
int pyramid_entropy(Vector* v);

// Exportar nulls hacia arriba (FO): niveles inferiores se vuelven coherentes
void pyramid_export_entropy_upward(Pyramid* p);

// Bajar función aprendida (FN): desde cima hacia base vía Extender
void pyramid_propagate_function_downward(Pyramid* p);

// Rotar orden (ES) hasta encontrar coherencia entre forma y función
void pyramid_harmonize_order(Pyramid* p, int max_iterations);

// === Knowledge Graph Queries ===

// Obtener nodo en nivel/índice específico
PyramidNode* pyramid_get_node(Pyramid* p, int level, int index);

// Obtener las relaciones RRR de un nodo
void pyramid_get_RRR(PyramidNode* node, Dim out[3]);

// Obtener las relaciones MMM de un nodo
void pyramid_get_MMM(PyramidNode* node, Dim out[3]);

// Obtener las relaciones OOO de un nodo
void pyramid_get_OOO(PyramidNode* node, Dim out[3]);

// === Utilidades ===

// Imprimir estructura de la pirámide
void pyramid_print(Pyramid* p);

// Imprimir un nodo específico con sus relaciones
void pyramid_print_node(PyramidNode* node);

// Validar coherencia de la pirámide
bool pyramid_validate(Pyramid* p);

#endif // PYRAMID_H
