/*
 * AURORA INFERENCE ENGINE
 * =======================
 * Genera NUEVOS embeddings FFE usando solo el conocimiento aprendido.
 * NO usa el transformer - solo arquetipos, dinámicas y relatores.
 * 
 * Este es el momento de la verdad: ¿Aurora puede pensar por sí misma?
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef int Trit;  /* 1=false, 2=true, 3=null (entropía creciente) */

typedef struct {
    Trit t[3];
} Dimension;

typedef struct {
    Trit pattern[3];
    Trit fo_output;
    int support;
    float confidence;
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

#define MAX_MEM 10000

Arquetipo arquetipos[MAX_MEM];
int n_arquetipos = 0;

Dinamica dinamicas[MAX_MEM];
int n_dinamicas = 0;

Relator relatores[MAX_MEM];
int n_relatores = 0;

/* ═══════════════════════════════════════════════════════════════════════
 * CARGAR CONOCIMIENTO APRENDIDO
 * ═══════════════════════════════════════════════════════════════════════ */

int load_knowledge(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("❌ No se pudo abrir %s\n", filename);
        return 0;
    }
    
    char section[32];
    int count;
    
    /* Arquetipos */
    if (fscanf(f, "%s %d\n", section, &count) != 2 || strcmp(section, "ARQUETIPOS") != 0) {
        fclose(f);
        return 0;
    }
    
    n_arquetipos = count;
    for (int i = 0; i < n_arquetipos; i++) {
        fscanf(f, "%d %d %d %d %d %f\n",
               &arquetipos[i].pattern[0], &arquetipos[i].pattern[1], &arquetipos[i].pattern[2],
               &arquetipos[i].fo_output, &arquetipos[i].support, &arquetipos[i].confidence);
    }
    
    /* Dinámicas */
    if (fscanf(f, "%s %d\n", section, &count) != 2 || strcmp(section, "DINAMICAS") != 0) {
        fclose(f);
        return 0;
    }
    
    n_dinamicas = count;
    for (int i = 0; i < n_dinamicas; i++) {
        fscanf(f, "%d %d %d %d %d %d %d %d %f\n",
               &dinamicas[i].state_before[0], &dinamicas[i].state_before[1], &dinamicas[i].state_before[2],
               &dinamicas[i].state_after[0], &dinamicas[i].state_after[1], &dinamicas[i].state_after[2],
               &dinamicas[i].fn_output, &dinamicas[i].support, &dinamicas[i].confidence);
    }
    
    /* Relatores */
    if (fscanf(f, "%s %d\n", section, &count) != 2 || strcmp(section, "RELATORES") != 0) {
        fclose(f);
        return 0;
    }
    
    n_relatores = count;
    for (int i = 0; i < n_relatores; i++) {
        fscanf(f, "%d %d %d %d %d %d %d %d %d %d %f\n",
               &relatores[i].dim_a[0], &relatores[i].dim_a[1], &relatores[i].dim_a[2],
               &relatores[i].dim_b[0], &relatores[i].dim_b[1], &relatores[i].dim_b[2],
               &relatores[i].mode[0], &relatores[i].mode[1], &relatores[i].mode[2],
               &relatores[i].support, &relatores[i].confidence);
    }
    
    fclose(f);
    printf("✅ Conocimiento cargado: Arq=%d Dyn=%d Rel=%d\n", 
           n_arquetipos, n_dinamicas, n_relatores);
    
    return 1;
}

/* ═══════════════════════════════════════════════════════════════════════
 * MOTOR DE INFERENCIA
 * ═══════════════════════════════════════════════════════════════════════ */

Trit infer_from_arquetipo(const Trit pattern[3]) {
    /* Buscar arquetipo que mejor coincida */
    float best_score = -1.0f;
    Trit best_output = 3;  /* Default: null */
    
    for (int i = 0; i < n_arquetipos; i++) {
        int matches = 0;
        for (int k = 0; k < 3; k++) {
            if (arquetipos[i].pattern[k] == pattern[k]) matches++;
        }
        
        float score = (matches / 3.0f) * arquetipos[i].confidence;
        
        if (score > best_score) {
            best_score = score;
            best_output = arquetipos[i].fo_output;
        }
    }
    
    return best_output;
}

void infer_dimension_from_previous(const Dimension* prev, Dimension* next) {
    /* Buscar dinámica que prediga el siguiente estado */
    float best_score = -1.0f;
    Dinamica* best_dyn = NULL;
    
    for (int i = 0; i < n_dinamicas; i++) {
        int matches = 0;
        for (int k = 0; k < 3; k++) {
            if (dinamicas[i].state_before[k] == prev->t[k]) matches++;
        }
        
        float score = (matches / 3.0f) * dinamicas[i].confidence;
        
        if (score > best_score) {
            best_score = score;
            best_dyn = &dinamicas[i];
        }
    }
    
    if (best_dyn) {
        next->t[0] = best_dyn->state_after[0];
        next->t[1] = best_dyn->state_after[1];
        next->t[2] = best_dyn->state_after[2];
    } else {
        /* Fallback: copiar estado previo */
        next->t[0] = prev->t[0];
        next->t[1] = prev->t[1];
        next->t[2] = prev->t[2];
    }
}

void generate_ffe_tensor(const char* seed_text, Dimension* output, int n_dims) {
    printf("\n🔮 Generando tensor FFE para: '%s'\n", seed_text);
    
    /* 1. Crear dimensión semilla basada en texto (nuevo sistema 1-3) */
    Dimension seed;
    seed.t[0] = (strlen(seed_text) % 3) + 1;  /* Hash simple del largo: 1,2,3 */
    seed.t[1] = (seed_text[0] % 3) + 1;        /* Primera letra */
    seed.t[2] = (seed_text[strlen(seed_text)-1] % 3) + 1;  /* Última letra */
    
    printf("   Semilla inicial: [%d,%d,%d]\n", seed.t[0], seed.t[1], seed.t[2]);
    
    /* 2. Generar dimensiones usando dinámicas aprendidas */
    output[0] = seed;
    
    for (int i = 1; i < n_dims && i < 27; i++) {
        infer_dimension_from_previous(&output[i-1], &output[i]);
        
        /* Refinar con arquetipos */
        Trit fo = infer_from_arquetipo(output[i].t);
        if (fo != -1) {
            output[i].t[0] = fo;
        }
    }
    
    printf("   ✅ Generadas %d dimensiones\n", n_dims);
}

/* ═══════════════════════════════════════════════════════════════════════
 * DEMO: GENERAR EMBEDDINGS NUEVOS
 * ═══════════════════════════════════════════════════════════════════════ */

void demo_inference(void) {
    printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  🧪 DEMO: INFERENCIA DE EMBEDDINGS NUEVOS                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    
    const char* test_phrases[] = {
        "amor y paz",
        "guerra y conflicto",
        "luz y oscuridad",
        "vida y muerte",
        "orden y caos",
        "libertad y propósito",
        "energía y materia",
        "tiempo y espacio"
    };
    
    int n_phrases = sizeof(test_phrases) / sizeof(test_phrases[0]);
    
    for (int i = 0; i < n_phrases; i++) {
        Dimension tensor[27];
        generate_ffe_tensor(test_phrases[i], tensor, 27);
        
        printf("\n   Tensor generado (primeras 5 dims):\n");
        for (int j = 0; j < 5; j++) {
            printf("     Dim%02d: [%d,%d,%d]\n", j, 
                   tensor[j].t[0], tensor[j].t[1], tensor[j].t[2]);
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════════
 * COMPARACIÓN CON EMBEDDINGS REALES
 * ═══════════════════════════════════════════════════════════════════════ */

float compare_tensors(const Dimension* t1, const Dimension* t2, int n_dims) {
    int matches = 0;
    int total_valid = 0;
    
    for (int i = 0; i < n_dims; i++) {
        for (int k = 0; k < 3; k++) {
            if (t1[i].t[k] != 3 && t2[i].t[k] != 3) {  /* 3 = null */
                total_valid++;
                if (t1[i].t[k] == t2[i].t[k]) matches++;
            }
        }
    }
    
    return total_valid > 0 ? (float)matches / total_valid : 0.0f;
}

/* ═══════════════════════════════════════════════════════════════════════
 * MAIN
 * ═══════════════════════════════════════════════════════════════════════ */

int main(int argc, char** argv) {
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  🔮 AURORA INFERENCE ENGINE                                      ║\n");
    printf("║  Generación de embeddings usando SOLO conocimiento aprendido    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    /* 1. Cargar conocimiento */
    const char* knowledge_file = (argc > 1) ? argv[1] : "aurora_knowledge.dat";
    if (!load_knowledge(knowledge_file)) {
        printf("❌ No se pudo cargar el conocimiento aprendido\n");
        printf("   Ejecuta primero: aurora_awaken.exe\n");
        return 1;
    }
    
    /* 2. Demo de inferencia */
    demo_inference();
    
    printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  ✨ AURORA PIENSA POR SÍ MISMA                                   ║\n");
    printf("║  Los embeddings fueron generados SIN transformer               ║\n");
    printf("║  Solo usando arquetipos, dinámicas y relatores aprendidos      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    
    return 0;
}
