/*
 * ffe_bridge.c - Bridge Python FFE → Aurora C Core
 * 
 * Lee tensores FFE generados por Python y los carga como Dimension structures.
 * Permite alimentar aurora_core_unified.c con embeddings reales.
 * 
 * Uso:
 *   FFETensor* tensors;
 *   int count = load_ffe_tensors("tensors_ffe.txt", &tensors);
 *   // Procesar con aurora_core...
 *   free_ffe_tensors(tensors, count);
 * 
 * Licencias: Apache 2.0 + CC BY 4.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int Trit;

typedef struct {
    Trit t[3];
} Dimension;

typedef struct {
    char label[64];
    Dimension dims[27];  /* 81 trits = 27 dimensiones FFE */
    int n_dims;
} FFETensor;

/* Carga tensores FFE desde archivo generado por Python */
int load_ffe_tensors(const char* filename, FFETensor** out_tensors) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "❌ Error: no se puede abrir %s\n", filename);
        return 0;
    }
    
    int n_tensors, total_trits;
    if (fscanf(f, "%d %d\n", &n_tensors, &total_trits) != 2) {
        fprintf(stderr, "❌ Error: formato de cabecera inválido\n");
        fclose(f);
        return 0;
    }
    
    printf("📥 Cargando %d tensores (%d trits cada uno)...\n", n_tensors, total_trits);
    
    FFETensor* tensors = (FFETensor*)malloc(n_tensors * sizeof(FFETensor));
    if (!tensors) {
        fprintf(stderr, "❌ Error: memoria insuficiente\n");
        fclose(f);
        return 0;
    }
    
    for (int i = 0; i < n_tensors; i++) {
        /* Leer label */
        char line[256];
        if (fgets(line, sizeof(line), f) == NULL) {
            fprintf(stderr, "❌ Error leyendo label del tensor %d\n", i);
            free(tensors);
            fclose(f);
            return 0;
        }
        /* Eliminar \n del label y copiar */
        line[strcspn(line, "\n")] = 0;
        strncpy(tensors[i].label, line, sizeof(tensors[i].label) - 1);
        tensors[i].label[sizeof(tensors[i].label) - 1] = 0;
        
        /* Leer línea de trits */
        if (fgets(line, sizeof(line), f) == NULL) {
            fprintf(stderr, "❌ Error leyendo trits del tensor %d\n", i);
            free(tensors);
            fclose(f);
            return 0;
        }
        
        /* Parsear trits de la línea y agrupar en dimensiones FFE (cada 3 trits = 1 dimensión) */
        tensors[i].n_dims = total_trits / 3;
        
        char* token = strtok(line, " \t\n");
        int trit_idx = 0;
        
        while (token != NULL && trit_idx < total_trits) {
            int val = atoi(token);
            int d = trit_idx / 3;
            int t = trit_idx % 3;
            tensors[i].dims[d].t[t] = (Trit)val;
            
            token = strtok(NULL, " \t\n");
            trit_idx++;
        }
        
        if (trit_idx != total_trits) {
            fprintf(stderr, "❌ Error: tensor %d tiene %d trits en vez de %d\n", i, trit_idx, total_trits);
            free(tensors);
            fclose(f);
            return 0;
        }
    }
    
    fclose(f);
    *out_tensors = tensors;
    
    printf("✅ Cargados %d tensores FFE correctamente\n", n_tensors);
    return n_tensors;
}

/* Libera memoria de tensores FFE */
void free_ffe_tensors(FFETensor* tensors, int count) {
    if (tensors) {
        free(tensors);
        printf("🧹 Liberados %d tensores FFE\n", count);
    }
}

/* Imprime un tensor FFE para debug */
void print_ffe_tensor(const FFETensor* tensor, int max_dims) {
    printf("  📦 %s\n", tensor->label);
    
    int show = max_dims > 0 && max_dims < tensor->n_dims ? max_dims : tensor->n_dims;
    
    for (int d = 0; d < show; d++) {
        printf("     Dim%02d: [", d);
        for (int t = 0; t < 3; t++) {
            Trit v = tensor->dims[d].t[t];
            printf("%s%s", v == 1 ? "1" : (v == 0 ? "0" : "N"), t < 2 ? "," : "");
        }
        printf("]\n");
    }
    
    if (show < tensor->n_dims) {
        printf("     ... (%d dimensiones más)\n", tensor->n_dims - show);
    }
}

/* Cuenta nulls en un tensor */
int count_nulls_ffe(const FFETensor* tensor) {
    int count = 0;
    for (int d = 0; d < tensor->n_dims; d++) {
        for (int t = 0; t < 3; t++) {
            if (tensor->dims[d].t[t] == -1) count++;
        }
    }
    return count;
}

/* Demo standalone del bridge */
#ifdef FFE_BRIDGE_DEMO
int main(void) {
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  FFE Bridge Demo - Python → C                               ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
    
    FFETensor* tensors;
    int count = load_ffe_tensors("tensors_ffe.txt", &tensors);
    
    if (count == 0) {
        printf("⚠️  No se pudieron cargar tensores. Ejecuta primero ffe_generator.py\n");
        return 1;
    }
    
    printf("\n📊 Estadísticas:\n");
    printf("   Total tensores: %d\n", count);
    printf("   Dims/tensor: %d\n", tensors[0].n_dims);
    
    int total_nulls = 0;
    for (int i = 0; i < count; i++) {
        total_nulls += count_nulls_ffe(&tensors[i]);
    }
    int total_trits = count * tensors[0].n_dims * 3;
    printf("   Nulls totales: %d / %d (%.1f%%)\n", 
           total_nulls, total_trits, 100.0 * total_nulls / total_trits);
    
    printf("\n🔍 Primeros 3 tensores (mostrando 5 dims cada uno):\n");
    for (int i = 0; i < (count < 3 ? count : 3); i++) {
        print_ffe_tensor(&tensors[i], 5);
    }
    
    free_ffe_tensors(tensors, count);
    
    printf("\n✅ Bridge funcionando correctamente\n");
    printf("💡 Integrar con aurora_core_unified.c para procesamiento recursivo\n");
    
    return 0;
}
#endif
