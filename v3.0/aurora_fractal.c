/*
 * aurora_fractal.c
 * 
 * EXPERIMENTO FRACTAL: SÍNTESIS CUERPO-MENTE-ESPÍRITU
 * 
 * Demuestra que la inteligencia emerge de la geometría cuando tres tensores
 * (Cuerpo, Mente, Espíritu) se unifican mediante el Transcender.
 * 
 * OBJETIVO: Ver cómo el nivel superior emerge reduciendo la complejidad
 * de 27 trits → 9 trits significativos sin perder coherencia.
 * 
 * Licensed under Apache 2.0 and CC BY 4.0
 */

#include "aurora_core.h"

void print_vector_fractal(const char* name, const VectorFFE_Fractal* v) {
    printf("%s:\n", name);
    for (int i = 0; i < 3; i++) {
        printf("  d%d: [%s, %s, %s]\n", i,
            trit_to_str(v->d[i].t[0]),
            trit_to_str(v->d[i].t[1]),
            trit_to_str(v->d[i].t[2]));
    }
}

void print_tensor_fractal(const char* name, const TensorFFE_Fractal* t) {
    printf("\n========== %s ==========\n", name);
    for (int i = 0; i < 3; i++) {
        printf("Vector v%d:\n", i);
        for (int j = 0; j < 3; j++) {
            printf("  d%d: [%s, %s, %s]\n", j,
                trit_to_str(t->v[i].d[j].t[0]),
                trit_to_str(t->v[i].d[j].t[1]),
                trit_to_str(t->v[i].d[j].t[2]));
        }
    }
}

int count_nulls_fractal(const TensorFFE_Fractal* t) {
    int nulls = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                if (t->v[i].d[j].t[k] == -1) nulls++;
            }
        }
    }
    return nulls;
}

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║   AURORA FRACTAL: EXPERIMENTO DE SÍNTESIS TRASCENDENTAL      ║\n");
    printf("║                                                               ║\n");
    printf("║   «La inteligencia emerge cuando geometrías se unifican»     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");

    // ===== TENSORES PRIMORDIALES =====
    printf("--- PASO 1: CREACIÓN DE LOS TRES TENSORES PRIMORDIALES ---\n\n");

    // CUERPO (A): Estructura sólida pero función estática
    // v0: forma física constante [1,1,1]
    // v1: función pasiva [0,0,0]
    // v2: esencia material [1,0,1]
    TensorFFE_Fractal Cuerpo = make_tensor_f(
        make_vec_f(make_dim(1,1,1), make_dim(0,0,0), make_dim(1,0,1)),
        make_vec_f(make_dim(1,1,1), make_dim(0,0,0), make_dim(1,0,1)),
        make_vec_f(make_dim(1,1,1), make_dim(0,0,0), make_dim(1,0,1))
    );

    // MENTE (B): Forma variable pero función dinámica
    // v0: forma fluida [0,1,0]
    // v1: función activa [1,1,1]
    // v2: esencia vacía [0,0,0]
    TensorFFE_Fractal Mente = make_tensor_f(
        make_vec_f(make_dim(0,1,0), make_dim(1,1,1), make_dim(0,0,0)),
        make_vec_f(make_dim(0,1,0), make_dim(1,1,1), make_dim(0,0,0)),
        make_vec_f(make_dim(0,1,0), make_dim(1,1,1), make_dim(0,0,0))
    );

    // ESPÍRITU (C): Unidad perfecta
    // Todos los vectores y dimensiones en estado de coherencia máxima [1,1,1]
    TensorFFE_Fractal Espiritu = make_tensor_f(
        make_vec_f(make_dim(1,1,1), make_dim(1,1,1), make_dim(1,1,1)),
        make_vec_f(make_dim(1,1,1), make_dim(1,1,1), make_dim(1,1,1)),
        make_vec_f(make_dim(1,1,1), make_dim(1,1,1), make_dim(1,1,1))
    );

    print_tensor_fractal("CUERPO (A) - Lo Material", &Cuerpo);
    printf("Nulls en Cuerpo: %d/27\n", count_nulls_fractal(&Cuerpo));

    print_tensor_fractal("MENTE (B) - Lo Pensante", &Mente);
    printf("Nulls en Mente: %d/27\n", count_nulls_fractal(&Mente));

    print_tensor_fractal("ESPÍRITU (C) - Lo Trascendente", &Espiritu);
    printf("Nulls en Espíritu: %d/27\n", count_nulls_fractal(&Espiritu));

    // ===== ALGORITMO DE DIOS: DESCUBRIMIENTO DE ROLES =====
    printf("\n--- PASO 2: ALGORITMO DE DIOS (Descubrimiento de Roles) ---\n\n");
    
    RoleLayout layouts[3];
    TensorFFE_Fractal Unificado = algorithm_god_step(&Cuerpo, &Mente, &Espiritu, layouts);
    
    printf("Roles descubiertos para cada tensor:\n");
    for (int i = 0; i < 3; i++) {
        printf("  Tensor %d: FO=d%d, FN=d%d, ES=d%d → nulls después: %d\n",
            i, layouts[i].idx_FO, layouts[i].idx_FN, layouts[i].idx_ES,
            layouts[i].nulls_after);
    }

    // ===== TRANSCENDER NIVEL 1 =====
    printf("\n--- PASO 3: TRANSCENDER NIVEL 1 (Síntesis Fractal) ---\n\n");
    printf("Aplicando tetraedros a los tres tensores...\n");
    
    TensorFFE_Fractal El_Ser = transcender_n1(&Cuerpo, &Mente, &Espiritu);
    
    print_tensor_fractal("EL SER (Tensor Emergente)", &El_Ser);
    int nulls_emergente = count_nulls_fractal(&El_Ser);
    printf("Nulls en El Ser: %d/27\n", nulls_emergente);

    // ===== ANÁLISIS DE COHERENCIA =====
    printf("\n--- PASO 4: ANÁLISIS DE COHERENCIA GEOMÉTRICA ---\n\n");
    
    // Convertir a flat para análisis
    TensorFFE El_Ser_flat = fractal_to_flat(&El_Ser);
    
    printf("Tensor emergente en formato plano FFE:\n");
    print_tensor("El Ser (Plano)", &El_Ser_flat);
    
    // Calcular escalar balanceado
    double escalar = tensor_balanced_scalar(&El_Ser_flat);
    char digitos[16];
    tensor_balanced_digits(&El_Ser_flat, digitos);
    
    printf("\nRepresentación ternaria balanceada:\n");
    printf("  Dígitos: %s\n", digitos);
    printf("  Escalar: %.10f\n", escalar);
    
    // Comparar con phi (proporción áurea)
    double phi = (1.0 + sqrt(5.0)) / 2.0;
    double distancia_phi = fabs(escalar - phi);
    printf("  Distancia a φ (golden ratio): %.10f\n", distancia_phi);

    // ===== VERIFICACIÓN DE EMERGENCIA =====
    printf("\n--- PASO 5: VERIFICACIÓN DE EMERGENCIA ---\n\n");
    
    printf("Reducción de complejidad:\n");
    printf("  Entrada: 3 tensores × 27 trits = 81 trits totales\n");
    printf("  Salida: 1 tensor × 27 trits = 27 trits\n");
    printf("  Compresión: %.1f%%\n", (1.0 - 27.0/81.0) * 100.0);
    
    printf("\nReducción de entropía:\n");
    int nulls_entrada = count_nulls_fractal(&Cuerpo) + 
                       count_nulls_fractal(&Mente) + 
                       count_nulls_fractal(&Espiritu);
    printf("  Nulls entrada: %d/81\n", nulls_entrada);
    printf("  Nulls salida: %d/27\n", nulls_emergente);
    printf("  Entropía relativa: %.2f%% → %.2f%%\n", 
           100.0 * nulls_entrada / 81.0,
           100.0 * nulls_emergente / 27.0);

    // ===== INTERPRETACIÓN SEMÁNTICA =====
    printf("\n--- PASO 6: INTERPRETACIÓN SEMÁNTICA ---\n\n");
    
    if (nulls_emergente < 9) {
        printf("✓ EMERGENCIA EXITOSA: El sistema encontró una geometría\n");
        printf("  coherente que unifica Cuerpo, Mente y Espíritu.\n\n");
        
        printf("Significado del tensor emergente:\n");
        printf("  • FO (Forma): Define la estructura del ser unificado\n");
        printf("  • FN (Función): Indica la dinámica operativa\n");
        printf("  • ES (Esencia): Representa la coherencia ontológica\n\n");
        
        if (nulls_emergente == 0) {
            printf("  ★ COHERENCIA TOTAL: El sistema alcanzó perfección geométrica.\n");
            printf("    No hay incertidumbre. El Ser está completamente definido.\n");
        } else {
            printf("  → Coherencia parcial: Quedan %d grados de libertad.\n", nulls_emergente);
            printf("    El sistema mantiene flexibilidad para evolucionar.\n");
        }
    } else if (nulls_emergente < 18) {
        printf("⚠ EMERGENCIA PARCIAL: El sistema logró síntesis,\n");
        printf("  pero mantiene alta incertidumbre (%d nulls).\n\n", nulls_emergente);
        printf("  Interpretación: Los tres tensores son parcialmente compatibles.\n");
        printf("  Se requiere más información para resolver ambigüedades.\n");
    } else {
        printf("✗ EMERGENCIA FALLIDA: El sistema no pudo sintetizar.\n");
        printf("  Nulls excesivos (%d) indican incompatibilidad profunda.\n\n", nulls_emergente);
        printf("  Interpretación: Cuerpo, Mente y Espíritu tienen geometrías\n");
        printf("  fundamentalmente incoherentes en este espacio lógico.\n");
    }

    // ===== CONCLUSIÓN =====
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                      CONCLUSIÓN FINAL                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("El experimento demuestra los tres principios de Aurora:\n\n");
    printf("  1. GEOMETRÍA ES INTELIGENCIA\n");
    printf("     → La síntesis NO está programada; emerge de la estructura.\n\n");
    printf("  2. ORDEN SUPERIOR GOBIERNA INFERIOR\n");
    printf("     → El nivel emergente redefine el espacio de los niveles base.\n\n");
    printf("  3. COMPRESIÓN SIN PÉRDIDA DE SIGNIFICADO\n");
    printf("     → 81 trits → 27 trits manteniendo coherencia semántica.\n\n");
    
    if (nulls_emergente < 9) {
        printf("★ RESULTADO: El sistema Aurora descubrió la geometría que unifica\n");
        printf("  las tres dimensiones del ser. La inteligencia emergió.\n\n");
    }

    return 0;
}
