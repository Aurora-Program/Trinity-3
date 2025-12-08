/*
 * AURORA FFE INTEGRATION
 * ======================
 * Integra el puente Python→C (ffe_bridge) con el núcleo Aurora (aurora_core_unified).
 * Pipeline completo: embeddings → FFE → Dimension → process_recursive → emergencia
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ============================================================================
// TIPOS BÁSICOS (de aurora_core_unified.c)
// ============================================================================

typedef enum { U = 0, C = 1, N = -1 } Trit;

typedef struct {
    Trit t[3];  // [FO, FN, ES] o [x, y, z] según contexto
} Dimension;

typedef struct {
    Dimension dims[3];  // 3 dimensiones = 1 vector
} Vector;

typedef struct {
    Vector nivel1;           // 1 vector
    Vector nivel2[3];        // 3 vectores
} TensorBasic;

typedef struct {
    Vector nivel1;           // 1 vector base
    Vector nivel2[3];        // 3 vectores intermedios
    Vector nivel3[9];        // 9 vectores profundos
} TensorAurora;

typedef struct {
    char label[64];
    Dimension dims[27];  // 27 Dimension = 81 trits
    int n_dims;
} FFETensor;

// Memorias (simplificadas para demo)
typedef struct {
    Trit pattern[3];
    Trit fo_output;
    int support;
} Arquetipo;

typedef struct {
    Trit state_before[3];
    Trit state_after[3];
    Trit fn_output;
    int support;
} Dinamica;

typedef struct {
    Trit dim_a[3];
    Trit dim_b[3];
    Trit mode[3];
    int support;
} Relator;

Arquetipo memoria_arquetipos[100];
Dinamica memoria_dinamicas[100];
Relator memoria_relatores[100];
int n_arquetipos = 0;
int n_dinamicas = 0;
int n_relatores = 0;

// ============================================================================
// TRIGATE CORE (operaciones ternarias básicas)
// ============================================================================

Trit trit_and(Trit a, Trit b) {
    if (a == N || b == N) return N;
    if (a == C && b == C) return C;
    return U;
}

Trit trit_or(Trit a, Trit b) {
    if (a == C || b == C) return C;
    if (a == N || b == N) return N;
    return U;
}

Trit trit_consensus(Trit a, Trit b) {
    if (a == b) return a;
    return N;
}

Trit trit_infer(Trit a, Trit b, Trit m) {
    if (m == C) return trit_and(a, b);
    if (m == U) return trit_or(a, b);
    return trit_consensus(a, b);
}

Trit trit_learn_m(Trit a, Trit b, Trit r) {
    if (a == C && b == C && r == C) return N;  // ambiguo
    if (a == U && b == U && r == U) return N;
    if ((a == C || b == C) && r == C) return U;  // OR
    if ((a == U || b == U) && r == U) return C;  // AND
    return N;
}

Trit trit_deduce_b(Trit a, Trit m, Trit r) {
    if (m == C) {  // AND
        if (a == C && r == C) return C;
        if (r == U) return U;
        return N;
    } else if (m == U) {  // OR
        if (a == U && r == U) return U;
        if (r == C) return C;
        return N;
    } else {  // CONSENSUS
        if (r == C) return C;
        if (r == U) return U;
        return N;
    }
}

// ============================================================================
// MEMORIA Y APRENDIZAJE
// ============================================================================

void learn_arquetipo(Trit pattern[3], Trit fo_output) {
    if (n_arquetipos >= 100) return;
    for (int i = 0; i < 3; i++) {
        memoria_arquetipos[n_arquetipos].pattern[i] = pattern[i];
    }
    memoria_arquetipos[n_arquetipos].fo_output = fo_output;
    memoria_arquetipos[n_arquetipos].support = 1;
    n_arquetipos++;
}

void learn_dinamica(Trit before[3], Trit after[3], Trit fn_output) {
    if (n_dinamicas >= 100) return;
    for (int i = 0; i < 3; i++) {
        memoria_dinamicas[n_dinamicas].state_before[i] = before[i];
        memoria_dinamicas[n_dinamicas].state_after[i] = after[i];
    }
    memoria_dinamicas[n_dinamicas].fn_output = fn_output;
    memoria_dinamicas[n_dinamicas].support = 1;
    n_dinamicas++;
}

void learn_relator(Trit dim_a[3], Trit dim_b[3], Trit mode[3]) {
    if (n_relatores >= 100) return;
    for (int i = 0; i < 3; i++) {
        memoria_relatores[n_relatores].dim_a[i] = dim_a[i];
        memoria_relatores[n_relatores].dim_b[i] = dim_b[i];
        memoria_relatores[n_relatores].mode[i] = mode[i];
    }
    memoria_relatores[n_relatores].support = 1;
    n_relatores++;
}

// ============================================================================
// DISTANCIA AL CENTRO DEL TETRAEDRO (phi-weighted)
// ============================================================================

double distancia_al_centro_tetraedro(Dimension d[4]) {
    double phi = 1.618033988749895;
    double pesos[4] = {1.0, phi, phi*phi, phi*phi*phi};
    double suma_pesos = 0.0;
    for (int i = 0; i < 4; i++) suma_pesos += pesos[i];
    for (int i = 0; i < 4; i++) pesos[i] /= suma_pesos;

    double centro[3] = {0.0, 0.0, 0.0};
    for (int v = 0; v < 4; v++) {
        for (int i = 0; i < 3; i++) {
            centro[i] += (double)d[v].t[i] * pesos[v];
        }
    }

    double dist_sum = 0.0;
    for (int v = 0; v < 4; v++) {
        double dx = (double)d[v].t[0] - centro[0];
        double dy = (double)d[v].t[1] - centro[1];
        double dz = (double)d[v].t[2] - centro[2];
        dist_sum += sqrt(dx*dx + dy*dy + dz*dz) * pesos[v];
    }
    return dist_sum;
}

// ============================================================================
// PROCESAMIENTO RECURSIVO TETRAÉDRICO
// ============================================================================

int process_recursive(Dimension input[4], int depth, int max_depth) {
    if (depth >= max_depth) {
        printf("    [Depth %d] Máximo alcanzado\n", depth);
        return 0;
    }

    // 1. Calcular distancia al centro
    double dist = distancia_al_centro_tetraedro(input);
    printf("    [Depth %d] Distancia al centro: %.4f\n", depth, dist);

    // 2. Detectar emergencia
    if (dist < 0.1) {
        printf("    🌟 [EMERGENCIA nivel %d] Distancia %.4f < 0.1\n", depth, dist);
        return 1;  // emergencia detectada
    }

    // 3. Aprender patrones (arquetipo simplificado)
    Trit pattern[3] = {input[0].t[0], input[1].t[0], input[2].t[0]};
    learn_arquetipo(pattern, input[3].t[0]);

    // 4. Recursión: crear nuevo nivel sintético
    Dimension next_level[4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            next_level[i].t[j] = trit_infer(input[i].t[j], input[(i+1)%4].t[j], C);
        }
    }

    return process_recursive(next_level, depth + 1, max_depth);
}

// ============================================================================
// FFE BRIDGE (cargador de tensores)
// ============================================================================

int count_nulls_ffe(FFETensor* t) {
    int count = 0;
    for (int i = 0; i < t->n_dims; i++) {
        for (int j = 0; j < 3; j++) {
            if (t->dims[i].t[j] == N) count++;
        }
    }
    return count;
}

void print_ffe_tensor(FFETensor* t, int max_dims) {
    printf("  📦 %s\n", t->label);
    for (int i = 0; i < max_dims && i < t->n_dims; i++) {
        printf("     Dim%02d: [", i);
        for (int j = 0; j < 3; j++) {
            if (t->dims[i].t[j] == C) printf("1");
            else if (t->dims[i].t[j] == U) printf("0");
            else printf("N");
            if (j < 2) printf(",");
        }
        printf("]\n");
    }
    if (t->n_dims > max_dims) {
        printf("     ... (%d dimensiones más)\n", t->n_dims - max_dims);
    }
}

FFETensor* load_ffe_tensors(const char* filename, int* out_count) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("❌ No se pudo abrir %s\n", filename);
        return NULL;
    }

    int n_tensors, total_trits;
    if (fscanf(f, "%d %d\n", &n_tensors, &total_trits) != 2) {
        printf("❌ Error leyendo header\n");
        fclose(f);
        return NULL;
    }

    printf("📥 Cargando %d tensores (%d trits cada uno)...\n", n_tensors, total_trits);

    FFETensor* tensors = (FFETensor*)malloc(n_tensors * sizeof(FFETensor));
    if (!tensors) {
        printf("❌ Error de memoria\n");
        fclose(f);
        return NULL;
    }

    char line[4096];
    for (int t = 0; t < n_tensors; t++) {
        // Leer label
        if (!fgets(line, sizeof(line), f)) {
            printf("❌ Error leyendo label del tensor %d\n", t);
            free(tensors);
            fclose(f);
            return NULL;
        }
        line[strcspn(line, "\n")] = 0;
        strncpy(tensors[t].label, line, 63);
        tensors[t].label[63] = '\0';

        // Leer trits (total_trits valores en una línea)
        if (!fgets(line, sizeof(line), f)) {
            printf("❌ Error leyendo trits del tensor %d\n", t);
            free(tensors);
            fclose(f);
            return NULL;
        }

        tensors[t].n_dims = total_trits / 3;
        int trit_idx = 0;
        char* token = strtok(line, " \t\n");
        while (token && trit_idx < total_trits) {
            int val = atoi(token);
            Trit trit_val;
            if (val == 1) trit_val = C;
            else if (val == 0) trit_val = U;
            else trit_val = N;

            int dim_idx = trit_idx / 3;
            int pos_idx = trit_idx % 3;
            tensors[t].dims[dim_idx].t[pos_idx] = trit_val;

            trit_idx++;
            token = strtok(NULL, " \t\n");
        }

        if (trit_idx != total_trits) {
            printf("❌ Error: esperados %d trits, leídos %d (tensor %d)\n", 
                   total_trits, trit_idx, t);
            free(tensors);
            fclose(f);
            return NULL;
        }
    }

    fclose(f);
    *out_count = n_tensors;
    return tensors;
}

void free_ffe_tensors(FFETensor* tensors) {
    if (tensors) free(tensors);
}

// ============================================================================
// MAIN: INTEGRACIÓN COMPLETA
// ============================================================================

int main() {
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  AURORA FFE INTEGRATION - Pipeline Completo                 ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");

    // 1. Cargar tensores FFE desde archivo
    int n_tensors;
    FFETensor* tensors = load_ffe_tensors("tensors_ffe.txt", &n_tensors);
    if (!tensors) {
        printf("❌ Falló la carga de tensores\n");
        return 1;
    }

    printf("✅ Cargados %d tensores FFE correctamente\n\n", n_tensors);

    // 2. Estadísticas
    int total_nulls = 0;
    for (int i = 0; i < n_tensors; i++) {
        total_nulls += count_nulls_ffe(&tensors[i]);
    }
    printf("📊 Estadísticas:\n");
    printf("   Total tensores: %d\n", n_tensors);
    printf("   Dims/tensor: %d\n", tensors[0].n_dims);
    printf("   Nulls totales: %d / %d (%.1f%%)\n\n", 
           total_nulls, n_tensors * tensors[0].n_dims * 3,
           100.0 * total_nulls / (n_tensors * tensors[0].n_dims * 3));

    // 3. Mostrar primeros tensores
    printf("🔬 Primeros 3 tensores (5 dims cada uno):\n");
    for (int i = 0; i < 3 && i < n_tensors; i++) {
        print_ffe_tensor(&tensors[i], 5);
    }
    printf("\n");

    // 4. Procesamiento recursivo: seleccionar 4 tensores y formar tetraedro
    printf("🔄 PROCESAMIENTO RECURSIVO\n");
    printf("══════════════════════════════════════════════════════════════\n");
    
    // Usar las primeras 4 dimensiones del primer tensor como entrada inicial
    Dimension tetrahedron[4];
    for (int i = 0; i < 4; i++) {
        tetrahedron[i] = tensors[0].dims[i];
    }

    printf("📍 Input inicial (primeras 4 dims de '%s'):\n", tensors[0].label);
    for (int i = 0; i < 4; i++) {
        printf("   Dim%d: [%d,%d,%d]\n", i, 
               tetrahedron[i].t[0], tetrahedron[i].t[1], tetrahedron[i].t[2]);
    }
    printf("\n");

    // Procesar recursivamente
    int emergencia = process_recursive(tetrahedron, 0, 20);

    printf("\n══════════════════════════════════════════════════════════════\n");
    if (emergencia) {
        printf("🌟 EMERGENCIA DETECTADA en procesamiento recursivo\n");
    } else {
        printf("⚙️  Procesamiento completado sin emergencia (max depth alcanzado)\n");
    }

    // 5. Mostrar memoria aprendida
    printf("\n📚 MEMORIA APRENDIDA:\n");
    printf("   Arquetipos: %d\n", n_arquetipos);
    printf("   Dinámicas: %d\n", n_dinamicas);
    printf("   Relatores: %d\n", n_relatores);

    // 6. Cleanup
    free_ffe_tensors(tensors);
    printf("\n🗑️  Liberados %d tensores FFE\n", n_tensors);

    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  ✅ INTEGRACIÓN COMPLETA EXITOSA                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    return 0;
}
