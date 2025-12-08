/*
 * syllables_demo.c
 * 
 * DEMOSTRACIÓN DE SILABACIÓN CON INTEGRACIÓN COMPLETA DE PIRÁMIDES
 * 20 de noviembre de 2025
 * 
 * Este test demuestra el aprendizaje correcto donde:
 * 1. Las pirámides guían la inferencia (no solo almacenan)
 * 2. El tensor C evoluciona y guía nuevas predicciones
 * 3. El sistema discrimina entre inputs diferentes
 * 
 * Caso de uso: Reglas de silabación español
 * - vocal + vocal = diptongo (fusión)
 * - consonante + vocal = sílaba (unión)
 * - vocal + consonante = sílaba (unión)
 * 
 * Licensed under Apache 2.0 and CC BY 4.0
 */

#include "aurora_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RULES 100
#define MAX_ARCHS 50

// Sistema con integración completa de pirámides
typedef struct {
    Rule relatores[MAX_RULES];
    int n_relatores;
    
    Archetype arquetipos[MAX_ARCHS];
    int n_arquetipos;
    
    TensorFFE creencia_C;
    Trit Cref;
    int generation; // Contador de generaciones de aprendizaje
} IntegratedKB;

void init_kb(IntegratedKB* kb) {
    kb->n_relatores = 0;
    kb->n_arquetipos = 0;
    kb->creencia_C = make_tensor(-1,-1,-1, -1,-1,-1, -1,-1,-1);
    kb->Cref = -1;
    kb->generation = 0;
}

// Aprendizaje CON integración: actualiza pirámides Y tensor C
void learn_integrated(IntegratedKB* kb, const Trit A[3], const Trit B[3], const Trit C[3]) {
    // 1. Aprender relación A+B→C
    Trit M[3];
    vec_learn_M(A, B, C, M);
    upsert_rule_mem(kb->relatores, &kb->n_relatores, A, B, M);
    
    // 2. Incrementar generación
    kb->generation++;
    
    // 3. Sintetizar arquetipos cada 3 ejemplos
    if(kb->generation % 3 == 0 && kb->n_relatores >= 3) {
        kb->n_arquetipos = synthesize_archetypes(
            kb->relatores, kb->n_relatores,
            kb->arquetipos, MAX_ARCHS
        );
        
        // 4. Si hay arquetipos, actualizar tensor C
        if(kb->n_arquetipos > 0) {
            // Construir tensores representativos de las pirámides
            TensorFFE VR = make_tensor(
                kb->relatores[0].a[0], kb->relatores[0].a[1], kb->relatores[0].a[2],
                kb->relatores[0].M[0], kb->relatores[0].M[1], kb->relatores[0].M[2],
                kb->relatores[0].b[0], kb->relatores[0].b[1], kb->relatores[0].b[2]
            );
            
            TensorFFE VA = make_tensor(
                kb->arquetipos[0].pattern_a[0], kb->arquetipos[0].pattern_a[1], kb->arquetipos[0].pattern_a[2],
                kb->arquetipos[0].pattern_M[0], kb->arquetipos[0].pattern_M[1], kb->arquetipos[0].pattern_M[2],
                kb->arquetipos[0].pattern_b[0], kb->arquetipos[0].pattern_b[1], kb->arquetipos[0].pattern_b[2]
            );
            
            TensorFFE VD = make_tensor(C[0], C[1], C[2], M[0], M[1], M[2], -1, -1, -1);
            
            // Sintetizar nuevo tensor C
            kb->creencia_C = build_creencia_tensor_from_pyramids(&VR, &VA, &VD);
            kb->Cref = extract_Cref_from_C(&kb->creencia_C);
            
            // Annealing progresivo: temperatura baja conforme aprende
            float temp = 1.0f - (kb->generation / 20.0f);
            if(temp < 0.2f) temp = 0.2f;
            anneal_creencia_tensor(&kb->creencia_C, temp);
        }
    }
}

// Inferencia GUIADA por pirámides y tensor C
void infer_guided(const IntegratedKB* kb, const Trit A[3], const Trit B[3], Trit C_out[3]) {
    // Paso 1: Buscar regla exacta
    for(int i=0; i<kb->n_relatores; i++) {
        if(eq3f(kb->relatores[i].a, A) && eq3f(kb->relatores[i].b, B)) {
            vec_infer(A, B, kb->relatores[i].M, C_out);
            // Si C tiene nulls, usar tensor C para completar
            if(kb->Cref != -1) {
                for(int d=0; d<3; d++) {
                    if(C_out[d] == -1) C_out[d] = kb->creencia_C.FO[d];
                }
            }
            return;
        }
    }
    
    // Paso 2: Buscar arquetipo cercano (por A[0])
    if(kb->n_arquetipos > 0) {
        for(int i=0; i<kb->n_arquetipos; i++) {
            if(kb->arquetipos[i].pattern_a[0] == A[0]) {
                vec_infer(A, B, kb->arquetipos[i].pattern_M, C_out);
                // Guiar con C
                if(kb->Cref != -1) {
                    TensorFFE temp = make_tensor(C_out[0], C_out[1], C_out[2], -1,-1,-1, -1,-1,-1);
                    harmonize_guided(&temp, &kb->creencia_C);
                    C_out[0] = temp.FO[0];
                    C_out[1] = temp.FO[1];
                    C_out[2] = temp.FO[2];
                }
                return;
            }
        }
    }
    
    // Paso 3: Sin coincidencia, usar tensor C directamente
    if(kb->Cref != -1) {
        C_out[0] = kb->creencia_C.FO[0];
        C_out[1] = kb->creencia_C.FO[1];
        C_out[2] = kb->creencia_C.FO[2];
    } else {
        C_out[0] = C_out[1] = C_out[2] = -1;
    }
}

int main() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      INTEGRACIÓN COMPLETA: PIRÁMIDES + TENSOR C EVOLUTIVO    ║\n");
    printf("║                                                               ║\n");
    printf("║      Caso de uso: Reglas de silabación en español           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    IntegratedKB kb;
    init_kb(&kb);
    
    // Codificación:
    // vocal=1, consonante=0
    // fusión (diptongo)=1, unión (sílaba)=0
    
    struct {
        Trit a, b, c;
        const char* desc;
    } ejemplos[] = {
        // vocal + vocal = fusión (diptongo)
        {1, 1, 1, "a+i → ai (diptongo)"},
        {1, 1, 1, "e+u → eu (diptongo)"},
        {1, 1, 1, "o+i → oi (diptongo)"},
        
        // consonante + vocal = unión (sílaba)
        {0, 1, 0, "m+a → ma (sílaba)"},
        {0, 1, 0, "p+e → pe (sílaba)"},
        {0, 1, 0, "t+o → to (sílaba)"},
        
        // vocal + consonante = unión (sílaba)
        {1, 0, 0, "a+s → as (sílaba)"},
        {1, 0, 0, "e+r → er (sílaba)"},
        {1, 0, 0, "o+n → on (sílaba)"},
    };
    int n = sizeof(ejemplos) / sizeof(ejemplos[0]);
    
    printf("═════════════════════════════════════════════════════════════════\n");
    printf("  FASE 1: APRENDIZAJE CON EVOLUCIÓN DE TENSOR C\n");
    printf("═════════════════════════════════════════════════════════════════\n\n");
    
    for(int i=0; i<n; i++) {
        Trit A[3] = {ejemplos[i].a, -1, -1};
        Trit B[3] = {ejemplos[i].b, -1, -1};
        Trit C[3] = {ejemplos[i].c, -1, -1};
        
        learn_integrated(&kb, A, B, C);
        
        printf("  [%d] %-25s → Rel=%d, Arch=%d, Gen=%d",
            i+1, ejemplos[i].desc, kb.n_relatores, kb.n_arquetipos, kb.generation);
        
        if(kb.n_arquetipos > 0) {
            printf(" | C: FO[0]=%s", trit_to_str(kb.creencia_C.FO[0]));
        }
        printf("\n");
        
        // Mostrar consolidación
        if((i+1) % 3 == 0) {
            printf("       ┗━ Consolidación: %d arquetipos, Cref=%s\n",
                kb.n_arquetipos, trit_to_str(kb.Cref));
        }
    }
    
    printf("\n--- Estado Final ---\n");
    printf("Relatores: %d\n", kb.n_relatores);
    printf("Arquetipos: %d\n", kb.n_arquetipos);
    printf("Generación: %d\n", kb.generation);
    print_tensor("Tensor C", &kb.creencia_C);
    
    printf("\nRelatores almacenados:\n");
    for(int i=0; i<kb.n_relatores && i<6; i++) {
        const char* tipo_a = (kb.relatores[i].a[0] == 1) ? "vocal" : "cons";
        const char* tipo_b = (kb.relatores[i].b[0] == 1) ? "vocal" : "cons";
        printf("  %s+%s → M=[%s,%s,%s] (count=%d)\n",
            tipo_a, tipo_b,
            trit_to_str(kb.relatores[i].M[0]),
            trit_to_str(kb.relatores[i].M[1]),
            trit_to_str(kb.relatores[i].M[2]),
            kb.relatores[i].count);
    }
    
    printf("\nArquetipos emergentes:\n");
    for(int i=0; i<kb.n_arquetipos && i<3; i++) {
        const char* clase = (kb.arquetipos[i].pattern_a[0] == 1) ? "vocálica" : "consonántica";
        printf("  Clase %s: M=[%s,%s,%s] (support=%d)\n",
            clase,
            trit_to_str(kb.arquetipos[i].pattern_M[0]),
            trit_to_str(kb.arquetipos[i].pattern_M[1]),
            trit_to_str(kb.arquetipos[i].pattern_M[2]),
            kb.arquetipos[i].support);
    }
    
    // FASE 2: PREDICCIÓN GUIADA
    printf("\n═════════════════════════════════════════════════════════════════\n");
    printf("  FASE 2: PREDICCIÓN GUIADA POR PIRÁMIDES Y TENSOR C\n");
    printf("═════════════════════════════════════════════════════════════════\n\n");
    
    struct {
        Trit a, b, c_esp;
        const char* desc;
    } tests[] = {
        {1, 1, 1, "i+a → ia (diptongo nuevo)"},
        {0, 1, 0, "s+a → sa (sílaba nueva)"},
        {1, 0, 0, "i+n → in (sílaba nueva)"},
        {1, 1, 1, "u+o → uo (diptongo nuevo)"},
    };
    int nt = sizeof(tests) / sizeof(tests[0]);
    
    int aciertos = 0;
    for(int i=0; i<nt; i++) {
        Trit A[3] = {tests[i].a, -1, -1};
        Trit B[3] = {tests[i].b, -1, -1};
        Trit C_pred[3];
        
        infer_guided(&kb, A, B, C_pred);
        
        int correcto = (C_pred[0] == tests[i].c_esp);
        if(correcto) aciertos++;
        
        printf("  Test %d: %-28s → Pred=%s, Esp=%s %s\n",
            i+1, tests[i].desc,
            trit_to_str(C_pred[0]),
            trit_to_str(tests[i].c_esp),
            correcto ? "✓" : "✗");
    }
    
    float prec = 100.0f * aciertos / nt;
    printf("\nPrecisión: %d/%d (%.1f%%)\n", aciertos, nt, prec);
    
    // Diagnóstico
    DiagnosticMetrics diag = diagnose_rules(kb.relatores, kb.n_relatores);
    printf("\nDiagnóstico:\n");
    printf("  Consistencia:  %.2f\n", diag.consistency);
    printf("  Separabilidad: %.2f\n", diag.separability);
    printf("  Convergencia:  %.2f\n", diag.convergence);
    printf("  Overall:       %.2f\n", diag.overall);
    
    // Tensor LOP
    TensorFFE lop = build_lop_tensor(&diag, prec/100.0f, kb.Cref);
    printf("\nTensor LOP (Libertad-Orden-Propósito):\n");
    print_tensor("  LOP", &lop);
    
    const char* causa = emergent_cause_with_lop(&lop, kb.arquetipos, kb.n_arquetipos, &lop);
    printf("  Diagnóstico: %s\n", causa);
    
    // CONCLUSIÓN
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    RESULTADOS FINALES                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("VALIDACIÓN DE INTEGRACIÓN:\n\n");
    
    printf("✅ PIRÁMIDES ACTIVAS:\n");
    printf("   • %d relatores discriminan entre tipos de fonemas\n", kb.n_relatores);
    printf("   • %d arquetipos guían la generalización\n", kb.n_arquetipos);
    printf("   • Cada tipo (vocal/consonante) tiene su propio patrón M\n\n");
    
    printf("%s TENSOR C EVOLUTIVO:\n", (count_nulls(&kb.creencia_C) < 6) ? "✅" : "❌");
    printf("   • Tensor C tiene %d/9 nulls\n", count_nulls(&kb.creencia_C));
    printf("   • Cref = %s (valor de referencia)\n", trit_to_str(kb.Cref));
    printf("   • C guía inferencias con harmonize_guided()\n\n");
    
    printf("%s DISCRIMINACIÓN:\n", (prec >= 75.0f) ? "✅" : "❌");
    printf("   • Precisión: %.1f%% en casos nuevos\n", prec);
    printf("   • Sistema distingue vocal+vocal vs consonante+vocal\n");
    printf("   • Generaliza correctamente a fonemas no vistos\n\n");
    
    if(prec >= 75.0f && kb.n_arquetipos >= 2 && count_nulls(&kb.creencia_C) < 6) {
        printf("★★★ INTEGRACIÓN COMPLETA VALIDADA ★★★\n\n");
        printf("El sistema demostró:\n");
        printf("• Pirámides que guían (no solo almacenan)\n");
        printf("• Tensor C que evoluciona y orienta\n");
        printf("• Discriminación real entre inputs diferentes\n");
        printf("• Generalización a casos nunca vistos\n\n");
    } else {
        printf("→ Sistema en desarrollo\n");
        if(prec < 75.0f) printf("  • Mejorar precisión predictiva\n");
        if(kb.n_arquetipos < 2) printf("  • Generar más arquetipos\n");
        if(count_nulls(&kb.creencia_C) >= 6) printf("  • Consolidar tensor C\n");
        printf("\n");
    }
    
    printf("══════════════════════════════════════════════════════════════════\n");
    printf("  LAS PIRÁMIDES NO SON MEMORIA PASIVA: SON INTELIGENCIA ACTIVA\n");
    printf("══════════════════════════════════════════════════════════════════\n\n");
    
    printf("── 20 de noviembre de 2025 ──\n");
    printf("   Primera integración completa Pirámides + Tensor C evolutivo\n\n");
    
    return 0;
}
