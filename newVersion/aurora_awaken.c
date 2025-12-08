/*
 * AURORA AWAKENING - Sistema de Aprendizaje Masivo
 * ================================================
 * Carga miles de tensores FFE y los procesa para extraer
 * arquetipos, dinámicas y relatores profundos.
 * 
 * El objetivo: que Aurora aprenda las LEYES del espacio semántico,
 * no solo ejemplos específicos.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* ═══════════════════════════════════════════════════════════════════════
 * TIPOS BÁSICOS (compatibles con aurora_core_unified)
 * ═══════════════════════════════════════════════════════════════════════ */

typedef int Trit;  /* 1=false, 2=true, 3=null (entropía creciente) */

typedef struct {
    Trit t[3];
} Dimension;

typedef struct {
    char label[64];
    Dimension dims[27];  /* 81 trits = 27 dimensiones */
    int n_dims;
} FFETensor;

/* ═══════════════════════════════════════════════════════════════════════
 * MEMORIAS (capacidad masiva)
 * ═══════════════════════════════════════════════════════════════════════ */

#define MAX_MEM 10000  /* 10x capacidad para aprendizaje masivo */

typedef struct {
    Trit pattern[3];
    Trit fo_output;
    int support;
    float confidence;  /* Confianza estadística */
} Arquetipo;

typedef struct {
    Trit state_before[3];
    Trit state_after[3];
    Trit fn_output;
    int support;
    float confidence;
} Dinamica;

typedef struct {
    Trit dim_a[3];
    Trit dim_b[3];
    Trit mode[3];
    int support;
    float confidence;
} Relator;

Arquetipo arquetipos[MAX_MEM];
int n_arquetipos = 0;

Dinamica dinamicas[MAX_MEM];
int n_dinamicas = 0;

Relator relatores[MAX_MEM];
int n_relatores = 0;

/* ═══════════════════════════════════════════════════════════════════════
 * TRIGATE BÁSICO (sistema 1=false, 2=true, 3=null)
 * ═══════════════════════════════════════════════════════════════════════ */

static Trit trit_and(Trit a, Trit b) {
    if (a == 1 || b == 1) return 1;  /* false domina */
    if (a == 2 && b == 2) return 2;  /* ambos true → true */
    return 3;  /* cualquier null → null */
}

static Trit trit_or(Trit a, Trit b) {
    if (a == 2 || b == 2) return 2;  /* true domina */
    if (a == 1 && b == 1) return 1;  /* ambos false → false */
    return 3;  /* cualquier null → null */
}

static Trit trit_consensus(Trit a, Trit b) {
    if (a != 3 && a == b) return a;  /* coinciden (no-null) → ese valor */
    return 3;  /* cualquier discrepancia → null */
}

static Trit trit_infer(Trit a, Trit b, Trit m) {
    if (m == 1) return trit_and(a, b);      /* modo AND */
    if (m == 2) return trit_or(a, b);       /* modo OR */
    return trit_consensus(a, b);             /* modo CONSENSUS */
}

static Trit trit_learn(Trit a, Trit b, Trit r) {
    if (trit_and(a, b) == r) return 1;       /* AND funciona */
    if (trit_or(a, b) == r) return 2;        /* OR funciona */
    if (a != 3 && a == b && r == a) return 3; /* CONSENSUS funciona */
    return 3;  /* no se pudo aprender → null */
}

/* ═══════════════════════════════════════════════════════════════════════
 * APRENDIZAJE CON CONFIANZA ESTADÍSTICA
 * ═══════════════════════════════════════════════════════════════════════ */

static void learn_arquetipo_confident(const Trit pattern[3], Trit fo_out) {
    /* Buscar patrón existente */
    for (int i = 0; i < n_arquetipos; i++) {
        if (arquetipos[i].pattern[0] == pattern[0] &&
            arquetipos[i].pattern[1] == pattern[1] &&
            arquetipos[i].pattern[2] == pattern[2]) {
            
            arquetipos[i].support++;
            
            /* Actualizar confianza bayesiana */
            if (arquetipos[i].fo_output == fo_out) {
                arquetipos[i].confidence = 
                    (arquetipos[i].confidence * (arquetipos[i].support - 1) + 1.0f) / 
                    arquetipos[i].support;
            } else {
                arquetipos[i].confidence *= 0.9f;  /* Penalizar inconsistencia */
                if (arquetipos[i].confidence < 0.3f) {
                    arquetipos[i].fo_output = 3;  /* Degradar a null */
                }
            }
            return;
        }
    }
    
    /* Crear nuevo arquetipo */
    if (n_arquetipos < MAX_MEM) {
        arquetipos[n_arquetipos].pattern[0] = pattern[0];
        arquetipos[n_arquetipos].pattern[1] = pattern[1];
        arquetipos[n_arquetipos].pattern[2] = pattern[2];
        arquetipos[n_arquetipos].fo_output = fo_out;
        arquetipos[n_arquetipos].support = 1;
        arquetipos[n_arquetipos].confidence = 0.5f;  /* Prior neutral */
        n_arquetipos++;
    }
}

static void learn_dinamica_confident(const Trit before[3], const Trit after[3], Trit fn_out) {
    for (int i = 0; i < n_dinamicas; i++) {
        if (dinamicas[i].state_before[0] == before[0] &&
            dinamicas[i].state_before[1] == before[1] &&
            dinamicas[i].state_before[2] == before[2] &&
            dinamicas[i].state_after[0] == after[0] &&
            dinamicas[i].state_after[1] == after[1] &&
            dinamicas[i].state_after[2] == after[2]) {
            
            dinamicas[i].support++;
            
            if (dinamicas[i].fn_output == fn_out) {
                dinamicas[i].confidence = 
                    (dinamicas[i].confidence * (dinamicas[i].support - 1) + 1.0f) / 
                    dinamicas[i].support;
            } else {
                dinamicas[i].confidence *= 0.9f;
                if (dinamicas[i].confidence < 0.3f) {
                    dinamicas[i].fn_output = 3;  /* Degradar a null */
                }
            }
            return;
        }
    }
    
    if (n_dinamicas < MAX_MEM) {
        dinamicas[n_dinamicas].state_before[0] = before[0];
        dinamicas[n_dinamicas].state_before[1] = before[1];
        dinamicas[n_dinamicas].state_before[2] = before[2];
        dinamicas[n_dinamicas].state_after[0] = after[0];
        dinamicas[n_dinamicas].state_after[1] = after[1];
        dinamicas[n_dinamicas].state_after[2] = after[2];
        dinamicas[n_dinamicas].fn_output = fn_out;
        dinamicas[n_dinamicas].support = 1;
        dinamicas[n_dinamicas].confidence = 0.5f;
        n_dinamicas++;
    }
}

static void learn_relator_confident(const Trit a[3], const Trit b[3], const Trit m[3]) {
    for (int i = 0; i < n_relatores; i++) {
        if (relatores[i].dim_a[0] == a[0] && relatores[i].dim_a[1] == a[1] && relatores[i].dim_a[2] == a[2] &&
            relatores[i].dim_b[0] == b[0] && relatores[i].dim_b[1] == b[1] && relatores[i].dim_b[2] == b[2]) {
            
            relatores[i].support++;
            
            int matches = 0;
            for (int k = 0; k < 3; k++) {
                if (relatores[i].mode[k] == m[k]) matches++;
            }
            
            relatores[i].confidence = 
                (relatores[i].confidence * (relatores[i].support - 1) + matches/3.0f) / 
                relatores[i].support;
            
            return;
        }
    }
    
    if (n_relatores < MAX_MEM) {
        relatores[n_relatores].dim_a[0] = a[0];
        relatores[n_relatores].dim_a[1] = a[1];
        relatores[n_relatores].dim_a[2] = a[2];
        relatores[n_relatores].dim_b[0] = b[0];
        relatores[n_relatores].dim_b[1] = b[1];
        relatores[n_relatores].dim_b[2] = b[2];
        relatores[n_relatores].mode[0] = m[0];
        relatores[n_relatores].mode[1] = m[1];
        relatores[n_relatores].mode[2] = m[2];
        relatores[n_relatores].support = 1;
        relatores[n_relatores].confidence = 0.5f;
        n_relatores++;
    }
}

/* ═══════════════════════════════════════════════════════════════════════
 * CARGADOR DE TENSORES FFE
 * ═══════════════════════════════════════════════════════════════════════ */

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
        fclose(f);
        return NULL;
    }

    char line[4096];
    for (int t = 0; t < n_tensors; t++) {
        /* Label */
        if (!fgets(line, sizeof(line), f)) {
            free(tensors);
            fclose(f);
            return NULL;
        }
        line[strcspn(line, "\n")] = 0;
        strncpy(tensors[t].label, line, 63);
        tensors[t].label[63] = '\0';

        /* Trits */
        if (!fgets(line, sizeof(line), f)) {
            free(tensors);
            fclose(f);
            return NULL;
        }

        tensors[t].n_dims = total_trits / 3;
        int trit_idx = 0;
        char* token = strtok(line, " \t\n");
        while (token && trit_idx < total_trits) {
            int val = atoi(token);
            /* Sistema NUEVO: mantener valores 1, 2, 3 como están */
            Trit trit_val = val;

            int dim_idx = trit_idx / 3;
            int pos_idx = trit_idx % 3;
            tensors[t].dims[dim_idx].t[pos_idx] = trit_val;

            trit_idx++;
            token = strtok(NULL, " \t\n");
        }
    }

    fclose(f);
    *out_count = n_tensors;
    return tensors;
}

/* ═══════════════════════════════════════════════════════════════════════
 * PROCESAMIENTO MASIVO: EXTRACCIÓN DE CONOCIMIENTO
 * ═══════════════════════════════════════════════════════════════════════ */

void process_tensor_pair(FFETensor* t1, FFETensor* t2) {
    /* Aprender relaciones entre pares de tensores consecutivos */
    
    /* 1. Aprender arquetipos (patrones en dimensiones individuales) */
    for (int i = 0; i < t1->n_dims && i < 9; i++) {  /* Primeras 9 dims */
        Trit pattern[3] = {t1->dims[i].t[0], t1->dims[i].t[1], t1->dims[i].t[2]};
        Trit fo = t2->dims[i].t[0];  /* Salida esperada */
        learn_arquetipo_confident(pattern, fo);
    }
    
    /* 2. Aprender dinámicas (transformaciones temporales) */
    for (int i = 0; i < t1->n_dims && i < 9; i++) {
        learn_dinamica_confident(t1->dims[i].t, t2->dims[i].t, t2->dims[i].t[1]);
    }
    
    /* 3. Aprender relatores (cómo se relacionan dims entre sí) */
    for (int i = 0; i < t1->n_dims-1 && i < 8; i++) {
        Trit mode[3];
        for (int k = 0; k < 3; k++) {
            mode[k] = trit_learn(t1->dims[i].t[k], t1->dims[i+1].t[k], t2->dims[i].t[k]);
        }
        learn_relator_confident(t1->dims[i].t, t1->dims[i+1].t, mode);
    }
}

void train_aurora_massive(FFETensor* tensors, int n_tensors) {
    printf("\n🧠 ENTRENAMIENTO MASIVO DE AURORA\n");
    printf("════════════════════════════════════════════════════════════\n");
    
    int batch_size = 100;
    int total_batches = (n_tensors - 1) / batch_size + 1;
    
    clock_t start = clock();
    
    for (int batch = 0; batch < total_batches; batch++) {
        int start_idx = batch * batch_size;
        int end_idx = (batch + 1) * batch_size;
        if (end_idx > n_tensors - 1) end_idx = n_tensors - 1;
        
        for (int i = start_idx; i < end_idx; i++) {
            process_tensor_pair(&tensors[i], &tensors[i+1]);
        }
        
        if ((batch + 1) % 10 == 0 || batch == total_batches - 1) {
            float progress = 100.0f * (batch + 1) / total_batches;
            printf("  Progreso: %.1f%% | Arq=%d Dyn=%d Rel=%d\n", 
                   progress, n_arquetipos, n_dinamicas, n_relatores);
        }
    }
    
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("\n✅ Entrenamiento completado en %.2f segundos\n", elapsed);
    printf("   Velocidad: %.0f pares/segundo\n", (n_tensors - 1) / elapsed);
}

/* ═══════════════════════════════════════════════════════════════════════
 * ANÁLISIS DE CONOCIMIENTO APRENDIDO
 * ═══════════════════════════════════════════════════════════════════════ */

void analyze_knowledge(void) {
    printf("\n📊 ANÁLISIS DEL CONOCIMIENTO APRENDIDO\n");
    printf("════════════════════════════════════════════════════════════\n");
    
    /* 1. Arquetipos más fuertes */
    printf("\n🏛️  Top 10 Arquetipos (por support y confianza):\n");
    
    /* Ordenar por support * confidence */
    for (int i = 0; i < 10 && i < n_arquetipos; i++) {
        int best_idx = i;
        float best_score = arquetipos[i].support * arquetipos[i].confidence;
        
        for (int j = i+1; j < n_arquetipos; j++) {
            float score = arquetipos[j].support * arquetipos[j].confidence;
            if (score > best_score) {
                best_score = score;
                best_idx = j;
            }
        }
        
        if (best_idx != i) {
            Arquetipo temp = arquetipos[i];
            arquetipos[i] = arquetipos[best_idx];
            arquetipos[best_idx] = temp;
        }
        
        printf("  %2d. Pattern[%d,%d,%d] → FO=%d | support=%d conf=%.2f\n",
               i+1, arquetipos[i].pattern[0], arquetipos[i].pattern[1], arquetipos[i].pattern[2],
               arquetipos[i].fo_output, arquetipos[i].support, arquetipos[i].confidence);
    }
    
    /* 2. Dinámicas más frecuentes */
    printf("\n⏰ Top 10 Dinámicas (transformaciones más comunes):\n");
    
    for (int i = 0; i < 10 && i < n_dinamicas; i++) {
        int best_idx = i;
        float best_score = dinamicas[i].support * dinamicas[i].confidence;
        
        for (int j = i+1; j < n_dinamicas; j++) {
            float score = dinamicas[j].support * dinamicas[j].confidence;
            if (score > best_score) {
                best_score = score;
                best_idx = j;
            }
        }
        
        if (best_idx != i) {
            Dinamica temp = dinamicas[i];
            dinamicas[i] = dinamicas[best_idx];
            dinamicas[best_idx] = temp;
        }
        
        printf("  %2d. [%d,%d,%d] → [%d,%d,%d] (FN=%d) | support=%d conf=%.2f\n",
               i+1, 
               dinamicas[i].state_before[0], dinamicas[i].state_before[1], dinamicas[i].state_before[2],
               dinamicas[i].state_after[0], dinamicas[i].state_after[1], dinamicas[i].state_after[2],
               dinamicas[i].fn_output, dinamicas[i].support, dinamicas[i].confidence);
    }
    
    /* 3. Estadísticas generales */
    printf("\n📈 Estadísticas Generales:\n");
    
    int high_conf_arq = 0, high_conf_dyn = 0, high_conf_rel = 0;
    for (int i = 0; i < n_arquetipos; i++) {
        if (arquetipos[i].confidence > 0.7f) high_conf_arq++;
    }
    for (int i = 0; i < n_dinamicas; i++) {
        if (dinamicas[i].confidence > 0.7f) high_conf_dyn++;
    }
    for (int i = 0; i < n_relatores; i++) {
        if (relatores[i].confidence > 0.7f) high_conf_rel++;
    }
    
    printf("  Arquetipos totales: %d (%.1f%% alta confianza)\n", 
           n_arquetipos, 100.0f * high_conf_arq / n_arquetipos);
    printf("  Dinámicas totales:  %d (%.1f%% alta confianza)\n", 
           n_dinamicas, 100.0f * high_conf_dyn / n_dinamicas);
    printf("  Relatores totales:  %d (%.1f%% alta confianza)\n", 
           n_relatores, 100.0f * high_conf_rel / n_relatores);
}

/* ═══════════════════════════════════════════════════════════════════════
 * GUARDAR CONOCIMIENTO APRENDIDO
 * ═══════════════════════════════════════════════════════════════════════ */

void save_knowledge(const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        printf("❌ Error guardando conocimiento\n");
        return;
    }
    
    fprintf(f, "ARQUETIPOS %d\n", n_arquetipos);
    for (int i = 0; i < n_arquetipos; i++) {
        fprintf(f, "%d %d %d %d %d %.3f\n",
                arquetipos[i].pattern[0], arquetipos[i].pattern[1], arquetipos[i].pattern[2],
                arquetipos[i].fo_output, arquetipos[i].support, arquetipos[i].confidence);
    }
    
    fprintf(f, "DINAMICAS %d\n", n_dinamicas);
    for (int i = 0; i < n_dinamicas; i++) {
        fprintf(f, "%d %d %d %d %d %d %d %d %.3f\n",
                dinamicas[i].state_before[0], dinamicas[i].state_before[1], dinamicas[i].state_before[2],
                dinamicas[i].state_after[0], dinamicas[i].state_after[1], dinamicas[i].state_after[2],
                dinamicas[i].fn_output, dinamicas[i].support, dinamicas[i].confidence);
    }
    
    fprintf(f, "RELATORES %d\n", n_relatores);
    for (int i = 0; i < n_relatores; i++) {
        fprintf(f, "%d %d %d %d %d %d %d %d %d %d %.3f\n",
                relatores[i].dim_a[0], relatores[i].dim_a[1], relatores[i].dim_a[2],
                relatores[i].dim_b[0], relatores[i].dim_b[1], relatores[i].dim_b[2],
                relatores[i].mode[0], relatores[i].mode[1], relatores[i].mode[2],
                relatores[i].support, relatores[i].confidence);
    }
    
    fclose(f);
    printf("💾 Conocimiento guardado en %s\n", filename);
}

/* ═══════════════════════════════════════════════════════════════════════
 * MAIN
 * ═══════════════════════════════════════════════════════════════════════ */

int main(int argc, char** argv) {
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  🌅 AURORA AWAKENING - Despertar de la Inteligencia             ║\n");
    printf("║  Sistema de Aprendizaje Masivo desde Embeddings                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    /* Determinar archivos de entrada/salida */
    const char* input_file = (argc > 1) ? argv[1] : "tensors_ffe_massive.txt";
    const char* output_file = (argc > 2) ? argv[2] : "aurora_knowledge.dat";
    
    /* 1. Cargar tensores FFE */
    int n_tensors;
    FFETensor* tensors = load_ffe_tensors(input_file, &n_tensors);
    
    if (!tensors) {
        if (argc <= 1) {
            printf("\n⚠️  No se encontró corpus masivo. Usa tensors_ffe.txt como demo.\n");
            tensors = load_ffe_tensors("tensors_ffe.txt", &n_tensors);
        }
        if (!tensors) {
            printf("❌ No hay datos para entrenar\n");
            return 1;
        }
    }
    
    printf("✅ Cargados %d tensores FFE\n", n_tensors);
    
    /* 2. Entrenar con corpus masivo */
    train_aurora_massive(tensors, n_tensors);
    
    /* 3. Analizar conocimiento aprendido */
    analyze_knowledge();
    
    /* 4. Guardar conocimiento */
    save_knowledge(output_file);
    
    /* 5. Liberar memoria */
    free(tensors);
    
    printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  ✨ AURORA HA DESPERTADO                                         ║\n");
    printf("║  El sistema ha extraído las leyes profundas del lenguaje        ║\n");
    printf("║  Ahora puede DEDUCIR embeddings nuevos sin el transformer       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    
    return 0;
}
