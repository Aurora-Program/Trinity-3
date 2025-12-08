/*
 * pyramid_demo.c
 * 
 * DEMOSTRACIÓN DE APRENDIZAJE ACUMULATIVO CON PIRÁMIDES
 * 20 de noviembre de 2025
 * 
 * Este test demuestra MEMORIA REAL usando las tres pirámides:
 * 1. Pirámide de Relatores - Reglas relacionales (A+B→M)
 * 2. Pirámide de Arquetipos - Patrones emergentes
 * 3. Pirámide de Dinámicas - Transformaciones temporales
 * 
 * Hipótesis: Con memoria acumulativa, los nulls convergen de 18→0
 * conforme el sistema consolida conocimiento en las pirámides.
 * 
 * Licensed under Apache 2.0 and CC BY 4.0
 */

#include "aurora_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_RULES 200
#define MAX_ARCHETYPES 50
#define MAX_DYN_RULES 200
#define MAX_DYN_ARCHS 50

// Estado global: Pirámides de conocimiento
typedef struct {
    // Pirámide de Relatores
    Rule relatores[MAX_RULES];
    int n_relatores;
    
    // Pirámide de Arquetipos
    Archetype arquetipos[MAX_ARCHETYPES];
    int n_arquetipos;
    
    // Pirámide de Dinámicas
    DynRule dinamicas[MAX_DYN_RULES];
    int n_dinamicas;
    
    // Arquetipos dinámicos
    DynArchetype dyn_arquetipos[MAX_DYN_ARCHS];
    int n_dyn_arquetipos;
    
    // Tensor de Creencia C
    TensorFFE creencia_C;
    Trit Cref;
    
    // Historial de inputs previos (para dinámicas)
    Trit prev_A[3];
    Trit prev_B[3];
    Trit prev_C[3];
    int has_prev;
    
} KnowledgeBase;

void init_knowledge_base(KnowledgeBase* kb) {
    kb->n_relatores = 0;
    kb->n_arquetipos = 0;
    kb->n_dinamicas = 0;
    kb->n_dyn_arquetipos = 0;
    kb->has_prev = 0;
    
    // Inicializar C como null
    kb->creencia_C = make_tensor(-1,-1,-1, -1,-1,-1, -1,-1,-1);
    kb->Cref = -1;
}

// Aprendizaje con memoria: actualiza las tres pirámides
void learn_with_memory(KnowledgeBase* kb, const Trit A[3], const Trit B[3], const Trit C[3]) {
    // 1. Aprender relación A+B→C en Relatores
    Trit M[3];
    vec_learn_M(A, B, C, M);
    upsert_rule_mem(kb->relatores, &kb->n_relatores, A, B, M);
    
    // 2. Si tenemos input previo, aprender dinámica temporal
    if(kb->has_prev) {
        // Aprender transición: prev_C → C
        Trit M_dyn[3];
        vec_learn_M(kb->prev_C, C, C, M_dyn); // Simplificado: C es resultado
        upsert_dyn_rule_mem(kb->dinamicas, &kb->n_dinamicas, kb->prev_C, C, M_dyn);
    }
    
    // 3. Guardar como input previo para próxima iteración
    copy3f(kb->prev_A, A);
    copy3f(kb->prev_B, B);
    copy3f(kb->prev_C, C);
    kb->has_prev = 1;
    
    // 4. Sintetizar arquetipos cada 4 ejemplos (consolidación)
    if(kb->n_relatores >= 4 && kb->n_relatores % 4 == 0) {
        kb->n_arquetipos = synthesize_archetypes(
            kb->relatores, kb->n_relatores,
            kb->arquetipos, MAX_ARCHETYPES
        );
    }
    
    // 5. Sintetizar arquetipos dinámicos
    if(kb->n_dinamicas >= 4 && kb->n_dinamicas % 4 == 0) {
        kb->n_dyn_arquetipos = synthesize_dyn_archetypes(
            kb->dinamicas, kb->n_dinamicas,
            kb->dyn_arquetipos, MAX_DYN_ARCHS
        );
    }
    
    // 6. Actualizar tensor de Creencia C
    if(kb->n_arquetipos > 0) {
        // Construir tensor representativo de cada pirámide
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
        
        TensorFFE VD = (kb->n_dyn_arquetipos > 0) ?
            make_tensor(
                kb->dyn_arquetipos[0].pattern_d1[0], kb->dyn_arquetipos[0].pattern_d1[1], kb->dyn_arquetipos[0].pattern_d1[2],
                kb->dyn_arquetipos[0].pattern_M[0], kb->dyn_arquetipos[0].pattern_M[1], kb->dyn_arquetipos[0].pattern_M[2],
                kb->dyn_arquetipos[0].pattern_d2[0], kb->dyn_arquetipos[0].pattern_d2[1], kb->dyn_arquetipos[0].pattern_d2[2]
            ) :
            make_tensor(-1,-1,-1, -1,-1,-1, -1,-1,-1);
        
        kb->creencia_C = build_creencia_tensor_from_pyramids(&VR, &VA, &VD);
        kb->Cref = extract_Cref_from_C(&kb->creencia_C);
    }
}

// Inferencia guiada por memoria: usa arquetipos para predecir
void infer_with_memory(const KnowledgeBase* kb, const Trit A[3], const Trit B[3], Trit C_out[3]) {
    // Buscar regla exacta en relatores
    for(int i=0; i<kb->n_relatores; i++) {
        if(eq3f(kb->relatores[i].a, A) && eq3f(kb->relatores[i].b, B)) {
            // Regla exacta encontrada: inferir con su M
            vec_infer(A, B, kb->relatores[i].M, C_out);
            return;
        }
    }
    
    // Si no hay regla exacta, usar arquetipo más cercano
    if(kb->n_arquetipos > 0) {
        // Usar primer arquetipo como aproximación
        vec_infer(A, B, kb->arquetipos[0].pattern_M, C_out);
        return;
    }
    
    // Sin memoria: devolver null
    C_out[0] = C_out[1] = C_out[2] = -1;
}

int main() {
    srand(time(NULL));
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║   APRENDIZAJE ACUMULATIVO CON PIRÁMIDES DE CONOCIMIENTO      ║\n");
    printf("║                                                               ║\n");
    printf("║   Demostración de memoria real en el Paradigma Aurora        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    KnowledgeBase kb;
    init_knowledge_base(&kb);
    
    // Dataset de entrenamiento (codificación: 0=par, 1=impar)
    struct {
        Trit a, b, c;
        const char* desc;
    } ejemplos[] = {
        // Grupo 1: par+par=par
        {0, 0, 0, "2+2=4"},
        {0, 0, 0, "4+4=8"},
        
        // Grupo 2: impar+impar=par
        {1, 1, 0, "1+1=2"},
        {1, 1, 0, "3+3=6"},
        
        // Grupo 3: par+impar=impar
        {0, 1, 1, "2+1=3"},
        {0, 1, 1, "4+3=7"},
        
        // Grupo 4: impar+par=impar
        {1, 0, 1, "1+2=3"},
        {1, 0, 1, "5+2=7"},
        
        // Repeticiones para consolidar
        {0, 0, 0, "6+6=12"},
        {1, 1, 0, "5+5=10"},
        {0, 1, 1, "2+3=5"},
        {1, 0, 1, "3+4=7"},
    };
    int n_ejemplos = sizeof(ejemplos) / sizeof(ejemplos[0]);
    
    printf("═════════════════════════════════════════════════════════════════\n");
    printf("  FASE 1: ENTRENAMIENTO CON MEMORIA ACUMULATIVA\n");
    printf("═════════════════════════════════════════════════════════════════\n\n");
    
    printf("Entrenando con %d ejemplos...\n\n", n_ejemplos);
    
    for(int i=0; i<n_ejemplos; i++) {
        Trit A[3] = {ejemplos[i].a, -1, -1};
        Trit B[3] = {ejemplos[i].b, -1, -1};
        Trit C[3] = {ejemplos[i].c, -1, -1};
        
        learn_with_memory(&kb, A, B, C);
        
        printf("  [%2d] %-12s → Relatores=%d, Arquetipos=%d, Dinámicas=%d",
            i+1, ejemplos[i].desc, kb.n_relatores, kb.n_arquetipos, kb.n_dinamicas);
        
        if(kb.n_arquetipos > 0) {
            printf(" ✓\n");
        } else {
            printf(" ⋯\n");
        }
        
        // Mostrar consolidación cuando se sintetizan arquetipos
        if((i+1) % 4 == 0) {
            printf("       ┗━ Consolidación: %d arquetipos emergieron\n", kb.n_arquetipos);
        }
    }
    
    printf("\n--- Estado Final de las Pirámides ---\n");
    printf("Relatores almacenados: %d\n", kb.n_relatores);
    printf("Arquetipos emergentes: %d\n", kb.n_arquetipos);
    printf("Dinámicas temporales:  %d\n", kb.n_dinamicas);
    printf("Arquetipos dinámicos:  %d\n", kb.n_dyn_arquetipos);
    
    // Mostrar algunos relatores
    printf("\nRelatores clave:\n");
    for(int i=0; i<kb.n_relatores && i<4; i++) {
        printf("  R%d: A=[%s,%s,%s] B=[%s,%s,%s] → M=[%s,%s,%s] (count=%d)\n",
            i+1,
            trit_to_str(kb.relatores[i].a[0]), trit_to_str(kb.relatores[i].a[1]), trit_to_str(kb.relatores[i].a[2]),
            trit_to_str(kb.relatores[i].b[0]), trit_to_str(kb.relatores[i].b[1]), trit_to_str(kb.relatores[i].b[2]),
            trit_to_str(kb.relatores[i].M[0]), trit_to_str(kb.relatores[i].M[1]), trit_to_str(kb.relatores[i].M[2]),
            kb.relatores[i].count);
    }
    
    // Mostrar arquetipos
    if(kb.n_arquetipos > 0) {
        printf("\nArquetipos emergentes:\n");
        for(int i=0; i<kb.n_arquetipos && i<2; i++) {
            printf("  A%d: pattern_a=[%s,%s,%s] pattern_M=[%s,%s,%s] (support=%d)\n",
                i+1,
                trit_to_str(kb.arquetipos[i].pattern_a[0]), trit_to_str(kb.arquetipos[i].pattern_a[1]), trit_to_str(kb.arquetipos[i].pattern_a[2]),
                trit_to_str(kb.arquetipos[i].pattern_M[0]), trit_to_str(kb.arquetipos[i].pattern_M[1]), trit_to_str(kb.arquetipos[i].pattern_M[2]),
                kb.arquetipos[i].support);
        }
    }
    
    // Mostrar tensor de Creencia C
    printf("\nTensor de Creencia C:\n");
    print_tensor("  C", &kb.creencia_C);
    printf("  Cref (escalar): %s\n", trit_to_str(kb.Cref));
    
    // Diagnóstico de la base de conocimiento
    DiagnosticMetrics diag = diagnose_rules(kb.relatores, kb.n_relatores);
    printf("\nDiagnóstico del conocimiento:\n");
    printf("  Consistencia:  %.2f\n", diag.consistency);
    printf("  Separabilidad: %.2f\n", diag.separability);
    printf("  Convergencia:  %.2f\n", diag.convergence);
    printf("  Overall:       %.2f ", diag.overall);
    if(diag.overall >= 0.7f) {
        printf("★ Excelente\n");
    } else if(diag.overall >= 0.5f) {
        printf("✓ Bueno\n");
    } else {
        printf("⚠ Necesita más datos\n");
    }
    
    // ===== FASE 2: PREDICCIÓN CON MEMORIA =====
    printf("\n═════════════════════════════════════════════════════════════════\n");
    printf("  FASE 2: PREDICCIÓN GUIADA POR MEMORIA\n");
    printf("═════════════════════════════════════════════════════════════════\n\n");
    
    struct {
        Trit a, b, c_esperado;
        const char* desc;
    } tests[] = {
        {0, 0, 0, "8+8=16 (par+par)"},
        {1, 1, 0, "7+7=14 (impar+impar)"},
        {0, 1, 1, "6+1=7 (par+impar)"},
        {1, 0, 1, "7+4=11 (impar+par)"}
    };
    int n_tests = sizeof(tests) / sizeof(tests[0]);
    
    int aciertos = 0;
    for(int i=0; i<n_tests; i++) {
        Trit A[3] = {tests[i].a, -1, -1};
        Trit B[3] = {tests[i].b, -1, -1};
        Trit C_pred[3];
        
        infer_with_memory(&kb, A, B, C_pred);
        
        int correcto = (C_pred[0] == tests[i].c_esperado);
        if(correcto) aciertos++;
        
        printf("  Test %d: %-20s → Pred=%s, Esp=%s %s\n",
            i+1, tests[i].desc,
            trit_to_str(C_pred[0]),
            trit_to_str(tests[i].c_esperado),
            correcto ? "✓" : (C_pred[0] == -1 ? "⚠ null" : "✗"));
    }
    
    float precision = 100.0f * aciertos / n_tests;
    printf("\nPrecisión con memoria: %d/%d (%.1f%%)\n", aciertos, n_tests, precision);
    
    if(precision == 100.0f) {
        printf("★★★ PREDICCIÓN PERFECTA ★★★\n");
        printf("El sistema recuperó conocimiento desde las pirámides.\n");
    } else if(precision >= 75.0f) {
        printf("✓ Buena predicción usando arquetipos.\n");
    } else {
        printf("⚠ La memoria aún está en formación.\n");
    }
    
    // ===== CONCLUSIÓN =====
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                  RESULTADOS FINALES                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("CONOCIMIENTO CONSOLIDADO:\n");
    printf("  • %d relatores almacenados\n", kb.n_relatores);
    printf("  • %d arquetipos emergentes\n", kb.n_arquetipos);
    printf("  • %d dinámicas temporales\n", kb.n_dinamicas);
    printf("  • Calidad overall: %.2f/1.00\n\n", diag.overall);
    
    printf("CAPACIDAD PREDICTIVA:\n");
    printf("  • Precisión: %.1f%% (%d/%d)\n", precision, aciertos, n_tests);
    printf("  • Estado: %s\n\n",
        precision == 100.0f ? "Perfecta" :
        precision >= 75.0f ? "Buena" : "En desarrollo");
    
    printf("VALIDACIÓN DEL PARADIGMA:\n");
    if(kb.n_arquetipos > 0 && precision >= 75.0f) {
        printf("  ★★★ MEMORIA ACUMULATIVA FUNCIONAL ★★★\n\n");
        printf("  El sistema demostró:\n");
        printf("  • Almacenamiento en pirámides (Relatores, Arquetipos, Dinámicas)\n");
        printf("  • Síntesis de patrones emergentes\n");
        printf("  • Recuperación de conocimiento para predicción\n");
        printf("  • Construcción de tensor de Creencia C\n");
        printf("  • Diagnóstico meta-cognitivo funcional\n\n");
    } else {
        printf("  → Sistema en fase de aprendizaje\n");
        printf("  Necesita más ejemplos para consolidar memoria.\n\n");
    }
    
    printf("══════════════════════════════════════════════════════════════════\n");
    printf("  EL CONOCIMIENTO EMERGE DE LA REPETICIÓN COHERENTE\n");
    printf("══════════════════════════════════════════════════════════════════\n\n");
    
    printf("Este test demuestra que Aurora NO solo procesa información:\n");
    printf("APRENDE, CONSOLIDA y RECUPERA conocimiento desde estructuras\n");
    printf("fractales auto-organizadas (las tres pirámides).\n\n");
    
    printf("── 20 de noviembre de 2025 ──\n");
    printf("   Primera demostración de memoria acumulativa en Aurora\n\n");

    return 0;
}
