// pyramid.c - Implementation of pyramidal knowledge graph
#include "pyramid.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// === Funciones auxiliares privadas ===

static PyramidNode* create_node(int level, int index) {
    PyramidNode* node = (PyramidNode*)calloc(1, sizeof(PyramidNode));
    node->level = level;
    node->index = index;
    node->is_synthesized = false;
    node->parent = NULL;
    node->parent_slot = -1;
    
    for (int i = 0; i < 3; i++) {
        node->sources[i] = NULL;
    }
    
    return node;
}

// === Funciones principales ===

Pyramid* pyramid_create(void) {
    Pyramid* p = (Pyramid*)calloc(1, sizeof(Pyramid));
    p->num_levels = 0;
    p->peak = NULL;
    
    for (int i = 0; i < MAX_PYRAMID_LEVELS; i++) {
        p->nodes_per_level[i] = 0;
        p->levels[i] = NULL;
    }
    
    return p;
}

void pyramid_destroy(Pyramid* p) {
    if (!p) return;
    
    for (int lvl = 0; lvl < p->num_levels; lvl++) {
        if (p->levels[lvl]) {
            for (int i = 0; i < p->nodes_per_level[lvl]; i++) {
                free(p->levels[lvl][i]);
            }
            free(p->levels[lvl]);
        }
    }
    free(p);
}

// === Ciclo Ascendente ===

void pyramid_init_base(Pyramid* p, Vector* base_vectors, int count) {
    // Nivel 0 (base) con 'count' vectores
    p->num_levels = 1;
    p->nodes_per_level[0] = count;
    p->levels[0] = (PyramidNode**)malloc(count * sizeof(PyramidNode*));
    
    for (int i = 0; i < count; i++) {
        PyramidNode* node = create_node(0, i);
        node->vector = base_vectors[i];
        node->is_synthesized = true; // Los nodos base ya están "sintetizados"
        p->levels[0][i] = node;
    }
    
    printf("[Pyramid] Initialized base level with %d vectors\n", count);
}

int pyramid_ascend_level(Pyramid* p, int level) {
    if (level >= p->num_levels) {
        printf("[Pyramid] Error: level %d doesn't exist (num_levels=%d)\n", 
               level, p->num_levels);
        return 0;
    }
    
    int num_nodes = p->nodes_per_level[level];
    if (num_nodes < 3) {
        printf("[Pyramid] Level %d has %d nodes (need 3+ for synthesis)\n", 
               level, num_nodes);
        return 0;
    }
    
    // Número de nodos emergentes = num_nodes / 3
    int num_emergent = num_nodes / 3;
    
    printf("[Pyramid] Ascending level %d: %d nodes → %d emergent\n", 
           level, num_nodes, num_emergent);
    
    // Crear el siguiente nivel
    int next_level = level + 1;
    if (next_level >= MAX_PYRAMID_LEVELS) {
        printf("[Pyramid] Error: exceeded MAX_PYRAMID_LEVELS\n");
        return 0;
    }
    
    p->num_levels = next_level + 1;
    p->nodes_per_level[next_level] = num_emergent;
    p->levels[next_level] = (PyramidNode**)malloc(num_emergent * sizeof(PyramidNode*));
    
    // Procesar cada grupo de 3 vectores
    for (int i = 0; i < num_emergent; i++) {
        int base_idx = i * 3;
        
        // Los 3 nodos fuente
        PyramidNode* src_A = p->levels[level][base_idx + 0];
        PyramidNode* src_B = p->levels[level][base_idx + 1];
        PyramidNode* src_C = p->levels[level][base_idx + 2];
        
        printf("  [%d] Processing nodes %d,%d,%d → emergent %d\n", 
               level, base_idx, base_idx+1, base_idx+2, i);
        
        // Crear input para Tetraedro
        TetraedroInput input = {
            .A = src_A->vector,
            .B = src_B->vector,
            .C = src_C->vector
        };
        
        // Procesar con Tetraedro (máx 10 iteraciones)
        TetraedroOutput output = tetraedro_process(&input, 10);
        
        // Crear nodo emergente
        PyramidNode* emergent = create_node(next_level, i);
        emergent->is_synthesized = true;
        emergent->synthesis = output;
        
        // Construir el vector emergente a partir de R, M, O
        // Según el modelo: cada dimensión del emergente viene de las 3 dimensiones R/M/O
        for (int d = 0; d < 3; d++) {
            // Combinar las 3 salidas del Tetraedro para cada dimensión
            emergent->vector.dim[d].FO = output.synthesis.R.FO; // Forma viene de R
            emergent->vector.dim[d].FN = output.synthesis.M.FN; // Función viene de M
            emergent->vector.dim[d].ES = output.synthesis.O.ES; // Estructura viene de O
        }
        
        // Asignar roles iniciales (se pueden ajustar después con aurora_core)
        emergent->vector.roles[0] = DATA;
        emergent->vector.roles[1] = CTRL;
        emergent->vector.roles[2] = COORD;
        
        // Guardar relaciones RRR, MMM, OOO
        emergent->sources[0] = src_A;
        emergent->sources[1] = src_B;
        emergent->sources[2] = src_C;
        
        emergent->RRR[0] = output.synthesis.R;
        emergent->RRR[1] = output.synthesis.R; // Mismo R para las 3 (síntesis única)
        emergent->RRR[2] = output.synthesis.R;
        
        emergent->MMM[0] = output.synthesis.M;
        emergent->MMM[1] = output.synthesis.M;
        emergent->MMM[2] = output.synthesis.M;
        
        emergent->OOO[0] = output.synthesis.O;
        emergent->OOO[1] = output.synthesis.O;
        emergent->OOO[2] = output.synthesis.O;
        
        // Establecer relación parent
        src_A->parent = emergent;
        src_A->parent_slot = 0;
        src_B->parent = emergent;
        src_B->parent_slot = 1;
        src_C->parent = emergent;
        src_C->parent_slot = 2;
        
        p->levels[next_level][i] = emergent;
    }
    
    return num_emergent;
}

void pyramid_ascend_to_peak(Pyramid* p) {
    printf("\n=== ASCENDING TO PEAK ===\n");
    
    int current_level = 0;
    
    while (p->nodes_per_level[current_level] >= 3) {
        int emergent_count = pyramid_ascend_level(p, current_level);
        if (emergent_count == 0) break;
        
        current_level++;
        
        // Si solo queda 1 nodo, es la cima
        if (p->nodes_per_level[current_level] == 1) {
            p->peak = p->levels[current_level][0];
            printf("\n[Pyramid] PEAK REACHED at level %d\n", current_level);
            break;
        }
    }
}

// === Ciclo Descendente ===

void pyramid_descend_from_peak(Pyramid* p) {
    if (!p->peak) {
        printf("[Pyramid] No peak found - cannot descend\n");
        return;
    }
    
    printf("\n=== DESCENDING FROM PEAK ===\n");
    printf("[Pyramid] Starting from level %d\n", p->peak->level);
    
    // Descender desde el nivel justo debajo de la cima
    for (int lvl = p->peak->level - 1; lvl >= 0; lvl--) {
        pyramid_descend_level(p, lvl);
    }
}

void pyramid_descend_level(Pyramid* p, int level) {
    printf("\n[Pyramid] Descending level %d (%d nodes)\n", 
           level, p->nodes_per_level[level]);
    
    for (int i = 0; i < p->nodes_per_level[level]; i++) {
        PyramidNode* node = p->levels[level][i];
        
        if (!node->parent) {
            continue; // Nodo sin parent (no puede descender más)
        }
        
        // Usar Extender para reconstruir desde el parent
        PyramidNode* parent = node->parent;
        
        printf("  Node [%d][%d] reconstructing from parent [%d][%d]\n",
               level, i, parent->level, parent->index);
        
        // Para cada dimensión del vector
        for (int d = 0; d < 3; d++) {
            // Obtener M (del nivel superior - parent)
            Dim M = parent->synthesis.synthesis.M;
            
            // Obtener R (almacenado en el nodo)
            Dim R = node->RRR[node->parent_slot];
            
            // Obtener O
            Dim O = node->OOO[node->parent_slot];
            
            // Usar Extender para reconstruir
            DimPair reconstructed = trigate_extend(&M, &R, &O);
            
            // Actualizar la dimensión con la reconstrucción
            // (según la posición en parent_slot)
            if (node->parent_slot == 0) {
                node->vector.dim[d] = reconstructed.A;
            } else if (node->parent_slot == 1) {
                node->vector.dim[d] = reconstructed.B;
            } else {
                // Para slot 2, podemos usar promedio o R directamente
                node->vector.dim[d] = R;
            }
        }
    }
}

// === Knowledge Graph Queries ===

PyramidNode* pyramid_get_node(Pyramid* p, int level, int index) {
    if (level >= p->num_levels || index >= p->nodes_per_level[level]) {
        return NULL;
    }
    return p->levels[level][index];
}

void pyramid_get_RRR(PyramidNode* node, Dim out[3]) {
    for (int i = 0; i < 3; i++) {
        out[i] = node->RRR[i];
    }
}

void pyramid_get_MMM(PyramidNode* node, Dim out[3]) {
    for (int i = 0; i < 3; i++) {
        out[i] = node->MMM[i];
    }
}

void pyramid_get_OOO(PyramidNode* node, Dim out[3]) {
    for (int i = 0; i < 3; i++) {
        out[i] = node->OOO[i];
    }
}

// === Utilidades ===

void pyramid_print(Pyramid* p) {
    printf("\n╔══════════════════════════════════════╗\n");
    printf("║      PYRAMIDAL KNOWLEDGE GRAPH       ║\n");
    printf("╚══════════════════════════════════════╝\n\n");
    
    printf("Total levels: %d\n", p->num_levels);
    if (p->peak) {
        printf("Peak: Level %d, Index %d\n\n", p->peak->level, p->peak->index);
    }
    
    // Imprimir desde la cima hacia abajo
    for (int lvl = p->num_levels - 1; lvl >= 0; lvl--) {
        int indent = (p->num_levels - 1 - lvl) * 2;
        
        printf("%*sLevel %d: %d nodes\n", indent, "", lvl, p->nodes_per_level[lvl]);
        
        for (int i = 0; i < p->nodes_per_level[lvl]; i++) {
            PyramidNode* node = p->levels[lvl][i];
            printf("%*s  [%d] ", indent, "", i);
            
            if (node->sources[0]) {
                printf("← sources:[%d][%d][%d] ", 
                       node->sources[0]->index,
                       node->sources[1]->index,
                       node->sources[2]->index);
            }
            
            if (node->parent) {
                printf("→ parent:[%d][%d]", 
                       node->parent->level, node->parent->index);
            }
            
            printf("\n");
        }
        printf("\n");
    }
}

void pyramid_print_node(PyramidNode* node) {
    printf("\n--- Node [%d][%d] ---\n", node->level, node->index);
    print_vector(&node->vector, "Vector");
    
    if (node->sources[0]) {
        printf("\nSources:\n");
        for (int i = 0; i < 3; i++) {
            printf("  [%d] Level %d, Index %d\n", 
                   i, node->sources[i]->level, node->sources[i]->index);
        }
        
        printf("\nRelations:\n");
        printf("  RRR[0]: FO=%s FN=%s ES=%s\n",
               trit_str(node->RRR[0].FO),
               trit_str(node->RRR[0].FN),
               trit_str(node->RRR[0].ES));
        printf("  MMM[0]: FO=%s FN=%s ES=%s\n",
               trit_str(node->MMM[0].FO),
               trit_str(node->MMM[0].FN),
               trit_str(node->MMM[0].ES));
        printf("  OOO[0]: FO=%s FN=%s ES=%s\n",
               trit_str(node->OOO[0].FO),
               trit_str(node->OOO[0].FN),
               trit_str(node->OOO[0].ES));
    }
    
    if (node->parent) {
        printf("\nParent: [%d][%d] (slot %d)\n", 
               node->parent->level, node->parent->index, node->parent_slot);
    }
}

bool pyramid_validate(Pyramid* p) {
    bool valid = true;
    
    printf("\n=== Validating Pyramid ===\n");
    
    for (int lvl = 1; lvl < p->num_levels; lvl++) {
        for (int i = 0; i < p->nodes_per_level[lvl]; i++) {
            PyramidNode* node = p->levels[lvl][i];
            
            // Verificar que tenga 3 sources
            if (!node->sources[0] || !node->sources[1] || !node->sources[2]) {
                printf("  [%d][%d] ERROR: missing sources\n", lvl, i);
                valid = false;
            }
            
            // Verificar que sources apunten a este nodo como parent
            for (int s = 0; s < 3; s++) {
                if (node->sources[s]->parent != node) {
                    printf("  [%d][%d] ERROR: source %d doesn't point back\n", 
                           lvl, i, s);
                    valid = false;
                }
            }
        }
    }
    
    if (valid) {
        printf("  ✓ Pyramid structure is valid\n");
    }
    
    return valid;
}

// === Flujo Termodinámico ===

int pyramid_entropy(Vector* v) {
    int null_count = 0;
    for (int d = 0; d < 3; d++) {
        if (v->dim[d].FO == T2) null_count++;
        if (v->dim[d].FN == T2) null_count++;
        if (v->dim[d].ES == T2) null_count++;
    }
    return null_count;
}

void pyramid_export_entropy_upward(Pyramid* p) {
    printf("\n=== EXPORTING ENTROPY UPWARD (FO nulls rise) ===\n");
    
    // Procesar desde la base hacia arriba
    for (int lvl = 0; lvl < p->num_levels - 1; lvl++) {
        printf("\n[Level %d] Exporting nulls to level %d:\n", lvl, lvl + 1);
        
        for (int i = 0; i < p->nodes_per_level[lvl]; i++) {
            PyramidNode* node = p->levels[lvl][i];
            
            if (!node->parent) continue;
            
            // Contar nulls en FO de este nodo
            int fo_nulls = 0;
            for (int d = 0; d < 3; d++) {
                if (node->vector.dim[d].FO == T2) fo_nulls++;
            }
            
            if (fo_nulls > 0) {
                // Transferir nulls al parent (nivel superior)
                int parent_dim = node->parent_slot;
                
                printf("  Node [%d][%d] has %d FO nulls → transferring to parent [%d][%d]\n",
                       lvl, i, fo_nulls, node->parent->level, node->parent->index);
                
                // El parent recibe los nulls (aumenta su entropía/adaptabilidad)
                // El nodo actual se vuelve más coherente (reduce nulls)
                for (int d = 0; d < 3; d++) {
                    if (node->vector.dim[d].FO == T2) {
                        // Resolver null en nivel inferior (hacerlo coherente)
                        // Usar el valor del parent como guía
                        Trit parent_val = node->parent->vector.dim[parent_dim].FO;
                        
                        if (parent_val != T2) {
                            // Si parent tiene valor definido, usarlo
                            node->vector.dim[d].FO = parent_val;
                        } else {
                            // Si parent también es null, usar valor por defecto coherente
                            node->vector.dim[d].FO = T0; // Base coherente = T0
                        }
                        
                        // El parent RECIBE el null (aumenta su flexibilidad)
                        node->parent->vector.dim[parent_dim].FO = T2;
                        
                        printf("    dim[%d]: FO null resolved to %s in child, exported to parent\n",
                               d, trit_str(node->vector.dim[d].FO));
                    }
                }
            }
        }
    }
    
    printf("\n[Thermodynamics] Base levels now more coherent, top levels more adaptive\n");
}

void pyramid_propagate_function_downward(Pyramid* p) {
    if (!p->peak) {
        printf("[Error] No peak to propagate from\n");
        return;
    }
    
    printf("\n=== PROPAGATING FUNCTION DOWNWARD (FN from peak) ===\n");
    
    // Desde la cima hacia abajo
    for (int lvl = p->num_levels - 1; lvl > 0; lvl--) {
        printf("\n[Level %d] Propagating FN to level %d:\n", lvl, lvl - 1);
        
        for (int i = 0; i < p->nodes_per_level[lvl]; i++) {
            PyramidNode* node = p->levels[lvl][i];
            
            // Este nodo tiene una función aprendida (FN)
            // Debe propagarla a sus 3 fuentes (sources)
            
            for (int s = 0; s < 3; s++) {
                if (!node->sources[s]) continue;
                
                PyramidNode* source = node->sources[s];
                
                printf("  Node [%d][%d] → source [%d][%d]: propagating FN\n",
                       lvl, i, source->level, source->index);
                
                // Usar Extender para reconstruir función
                for (int d = 0; d < 3; d++) {
                    // La función del nivel superior guía al inferior
                    Dim M = node->synthesis.synthesis.M;  // Modo aprendido en nivel superior
                    Dim R = node->RRR[s];                  // Resultado
                    Dim O = node->OOO[s];                  // Orden
                    
                    // Extender calcula qué función debe tener la fuente
                    DimPair reconstructed = trigate_extend(&M, &R, &O);
                    
                    // Actualizar FN de la fuente (función baja desde arriba)
                    if (s == 0) {
                        source->vector.dim[d].FN = reconstructed.A.FN;
                    } else if (s == 1) {
                        source->vector.dim[d].FN = reconstructed.B.FN;
                    } else {
                        // Para la tercera fuente, usar el modo M directamente
                        source->vector.dim[d].FN = M.FN;
                    }
                    
                    printf("    source dim[%d].FN = %s (from parent function)\n",
                           d, trit_str(source->vector.dim[d].FN));
                }
            }
        }
    }
    
    printf("\n[Thermodynamics] Function propagated from peak to base\n");
}

void pyramid_harmonize_order(Pyramid* p, int max_iterations) {
    printf("\n=== HARMONIZING ORDER (ES rotation) ===\n");
    printf("Goal: Find ES configuration where Forma and Función are coherent\n\n");
    
    for (int iter = 0; iter < max_iterations; iter++) {
        printf("Iteration %d:\n", iter);
        
        int total_adjustments = 0;
        
        // Procesar todos los niveles
        for (int lvl = 0; lvl < p->num_levels; lvl++) {
            for (int i = 0; i < p->nodes_per_level[lvl]; i++) {
                PyramidNode* node = p->levels[lvl][i];
                
                // Para cada dimensión, verificar coherencia entre FO y FN
                for (int d = 0; d < 3; d++) {
                    Trit fo = node->vector.dim[d].FO;
                    Trit fn = node->vector.dim[d].FN;
                    Trit es = node->vector.dim[d].ES;
                    
                    // Si hay incoherencia (FO y FN incompatibles)
                    bool incoherent = (fo == T2 && fn != T2) || (fo != T2 && fn == T2);
                    
                    if (incoherent) {
                        // Rotar ES usando Fibonacci
                        int fib_focus = iter % 3;  // 0, 1, 2 (FO, FN, ES)
                        
                        Trit new_es = (Trit)((es + 1) % 3);
                        
                        if (fib_focus == 2) {  // Focus en ES
                            node->vector.dim[d].ES = new_es;
                            total_adjustments++;
                        }
                    }
                }
            }
        }
        
        printf("  Total ES adjustments: %d\n", total_adjustments);
        
        // Si no hay más ajustes, hemos alcanzado coherencia
        if (total_adjustments == 0) {
            printf("\n[Harmony] System reached coherent configuration at iteration %d\n", iter);
            break;
        }
    }
    
    printf("\n[Thermodynamics] Order harmonized across all levels\n");
}
