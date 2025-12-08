/*
 * role_discovery_demo.c
 * Demostración visual del concepto clave del Whitepaper 0.4:
 * "No sabes qué dimensión se relaciona con cuál. El sistema debe descubrirlo."
 * 
 * Este demo muestra cómo Aurora descubre los roles FO/FN/ES de forma dinámica,
 * probando permutaciones y minimizando nulls (coherencia geométrica).
 * 
 * Build:
 *   gcc -std=c11 -O2 -Wall -o role_discovery_demo role_discovery_demo.c aurora_core.o
 */

#include "aurora_core.h"
#include <stdio.h>

void print_layout_details(const char* name, const RoleLayout* layout, const VectorFFE_Fractal* v){
    printf("\n%s:\n", name);
    printf("  Mejor asignación encontrada:\n");
    printf("    FO viene de dimensión d[%d]\n", layout->idx_FO);
    printf("    FN viene de dimensión d[%d]\n", layout->idx_FN);
    printf("    ES viene de dimensión d[%d]\n", layout->idx_ES);
    printf("  Nulls tras armonizar: %d\n", layout->nulls_after);
    
    // Mostrar valores concretos en ese layout
    printf("  Valores resultantes:\n");
    printf("    FO = [%s, %s, %s]\n", 
        trit_to_str(v->d[layout->idx_FO].t[0]),
        trit_to_str(v->d[layout->idx_FO].t[1]),
        trit_to_str(v->d[layout->idx_FO].t[2]));
    printf("    FN = [%s, %s, %s]\n", 
        trit_to_str(v->d[layout->idx_FN].t[0]),
        trit_to_str(v->d[layout->idx_FN].t[1]),
        trit_to_str(v->d[layout->idx_FN].t[2]));
    printf("    ES = [%s, %s, %s]\n", 
        trit_to_str(v->d[layout->idx_ES].t[0]),
        trit_to_str(v->d[layout->idx_ES].t[1]),
        trit_to_str(v->d[layout->idx_ES].t[2]));
}

int main(){
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  DEMO: Descubrimiento Dinámico de Roles (Whitepaper 0.4)\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    printf("\nPrincipio clave del Whitepaper:\n");
    printf("  \"Cada tensor contiene Forma, Función y Estructura,\n");
    printf("   pero NO sabemos cuál es cuál hasta analizar relaciones.\n");
    printf("   La semántica depende del CONTEXTO.\"\n");
    
    // Caso 1: Vector con clara estructura (vocal)
    printf("\n\n─── CASO 1: Fonema Vocal 'a' ───\n");
    DimensionFFE d0_vocal = make_dim(1, 0, -1);  // vocal, no-líquida, núcleo
    DimensionFFE d1_vocal = make_dim(1, 1, 0);   // abierta, anterior, ...
    DimensionFFE d2_vocal = make_dim(0, 1, 1);   // ...
    VectorFFE_Fractal v_vocal = make_vec_f(d0_vocal, d1_vocal, d2_vocal);
    
    printf("Dimensiones brutas (sin interpretar):\n");
    printf("  d[0] = [%s, %s, %s]\n", 
        trit_to_str(d0_vocal.t[0]), trit_to_str(d0_vocal.t[1]), trit_to_str(d0_vocal.t[2]));
    printf("  d[1] = [%s, %s, %s]\n", 
        trit_to_str(d1_vocal.t[0]), trit_to_str(d1_vocal.t[1]), trit_to_str(d1_vocal.t[2]));
    printf("  d[2] = [%s, %s, %s]\n", 
        trit_to_str(d2_vocal.t[0]), trit_to_str(d2_vocal.t[1]), trit_to_str(d2_vocal.t[2]));
    
    printf("\nAurora prueba las 6 permutaciones posibles...\n");
    RoleLayout layout_vocal = discover_vector_roles(&v_vocal);
    print_layout_details("RESULTADO", &layout_vocal, &v_vocal);
    
    // Caso 2: Vector con estructura diferente (consonante)
    printf("\n\n─── CASO 2: Fonema Consonante 'k' ───\n");
    DimensionFFE d0_cons = make_dim(0, 0, 1);    // consonante, no-líquida, soporte
    DimensionFFE d1_cons = make_dim(0, -1, 1);   // oclusiva, velar, ...
    DimensionFFE d2_cons = make_dim(1, 0, 0);    // ...
    VectorFFE_Fractal v_cons = make_vec_f(d0_cons, d1_cons, d2_cons);
    
    printf("Dimensiones brutas (sin interpretar):\n");
    printf("  d[0] = [%s, %s, %s]\n", 
        trit_to_str(d0_cons.t[0]), trit_to_str(d0_cons.t[1]), trit_to_str(d0_cons.t[2]));
    printf("  d[1] = [%s, %s, %s]\n", 
        trit_to_str(d1_cons.t[0]), trit_to_str(d1_cons.t[1]), trit_to_str(d1_cons.t[2]));
    printf("  d[2] = [%s, %s, %s]\n", 
        trit_to_str(d2_cons.t[0]), trit_to_str(d2_cons.t[1]), trit_to_str(d2_cons.t[2]));
    
    printf("\nAurora prueba las 6 permutaciones posibles...\n");
    RoleLayout layout_cons = discover_vector_roles(&v_cons);
    print_layout_details("RESULTADO", &layout_cons, &v_cons);
    
    // Caso 3: Vector ambiguo (muchos nulls)
    printf("\n\n─── CASO 3: Vector Ambiguo (contexto desconocido) ───\n");
    DimensionFFE d0_amb = make_dim(-1, 1, -1);
    DimensionFFE d1_amb = make_dim(0, -1, 1);
    DimensionFFE d2_amb = make_dim(-1, 0, -1);
    VectorFFE_Fractal v_amb = make_vec_f(d0_amb, d1_amb, d2_amb);
    
    printf("Dimensiones brutas (sin interpretar):\n");
    printf("  d[0] = [%s, %s, %s]\n", 
        trit_to_str(d0_amb.t[0]), trit_to_str(d0_amb.t[1]), trit_to_str(d0_amb.t[2]));
    printf("  d[1] = [%s, %s, %s]\n", 
        trit_to_str(d1_amb.t[0]), trit_to_str(d1_amb.t[1]), trit_to_str(d1_amb.t[2]));
    printf("  d[2] = [%s, %s, %s]\n", 
        trit_to_str(d2_amb.t[0]), trit_to_str(d2_amb.t[1]), trit_to_str(d2_amb.t[2]));
    
    printf("\nAurora prueba las 6 permutaciones posibles...\n");
    RoleLayout layout_amb = discover_vector_roles(&v_amb);
    print_layout_details("RESULTADO", &layout_amb, &v_amb);
    
    printf("\n\n─── COMPARACIÓN FINAL ───\n");
    printf("Vocal 'a':       FO=d[%d] FN=d[%d] ES=d[%d]  (nulls=%d)\n", 
        layout_vocal.idx_FO, layout_vocal.idx_FN, layout_vocal.idx_ES, layout_vocal.nulls_after);
    printf("Consonante 'k':  FO=d[%d] FN=d[%d] ES=d[%d]  (nulls=%d)\n", 
        layout_cons.idx_FO, layout_cons.idx_FN, layout_cons.idx_ES, layout_cons.nulls_after);
    printf("Vector ambiguo:  FO=d[%d] FN=d[%d] ES=d[%d]  (nulls=%d)\n", 
        layout_amb.idx_FO, layout_amb.idx_FN, layout_amb.idx_ES, layout_amb.nulls_after);
    
    printf("\n\n═══════════════════════════════════════════════════════════════\n");
    printf("  CONCLUSIÓN:\n");
    printf("  • Los roles FO/FN/ES NO están prefijados en el tensor\n");
    printf("  • Aurora los DESCUBRE minimizando nulls (coherencia)\n");
    printf("  • Diferentes contextos → diferentes asignaciones óptimas\n");
    printf("  • El Algoritmo de Dios usa Fibonacci para evitar bucles\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    return 0;
}
