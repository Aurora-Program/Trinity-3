#include <stdio.h>
#include <string.h>
#include "aurora_core.h"

// Utilería necesaria
int eq3f(const Trit x[3], const Trit y[3]){ 
    for(int i=0;i<3;i++) if(x[i]!=y[i]) return 0; 
    return 1; 
}

void print_vec(const char* label, const Trit v[3]) {
    printf("%s: [%s, %s, %s]\n", label, trit_to_str(v[0]), trit_to_str(v[1]), trit_to_str(v[2]));
}

int main() {
    printf("==================================================\n");
    printf("   AURORA CORE: MOTOR TETRAEDRICO (APRENDIZAJE)\n");
    printf("==================================================\n\n");

    // --- FASE 1: EL MAESTRO ENSEÑA (APRENDIZAJE) ---
    printf("--- FASE 1: OBSERVACION Y DEDUCCION ---\n");
    
    // Ejemplo de entrenamiento (Comportamiento Lógico AND)
    // A: 1 (V), 0 (F), 1 (V)
    // B: 1 (V), 1 (V), 0 (F)
    // R: 1 (V), 0 (F), 0 (F) -> Solo es 1 si ambos son 1
    Trit A_train[3] = {1, 0, 1};
    Trit B_train[3] = {1, 1, 0};
    Trit R_target[3]= {1, 0, 0}; 

    print_vec("Input A", A_train);
    print_vec("Input B", B_train);
    print_vec("Resultado Observado", R_target);

    printf("\n>> Sistema Aurora: 'Analizando causalidad...'\n");
    
    // Usamos el Tetraedro para aprender M (la regla mediadora)
    TetraFaceResult aprendizaje;
    tetra_sintetizador_learn(A_train, B_train, R_target, &aprendizaje);
    
    // En Aurora: M=0 es AND, M=1 es OR, M=-1 es Consensus
    print_vec("Regla Aprendida (M)", aprendizaje.M);

    // Verificación
    if(eq3f(aprendizaje.M, (Trit[]){0,0,0})) {
        printf(">> EUREKA: El sistema ha deducido la logica 'AND' (Interseccion).\n");
    } else {
        printf(">> FALLO: El sistema no entendio la regla.\n");
        return 1;
    }

    // --- FASE 2: EL ALUMNO APLICA (INFERENCIA) ---
    printf("\n--- FASE 2: PREDICCION (EXAMEN) ---\n");
    printf("Ahora probamos con datos NUNCA vistos:\n");

    // Nuevos datos
    Trit A_new[3] = {0, 1, 1};
    Trit B_new[3] = {1, 1, 1};
    // Logica humana: 0&1->0, 1&1->1, 1&1->1 => Esperamos [0, 1, 1]
    
    print_vec("Nuevo Input A", A_new);
    print_vec("Nuevo Input B", B_new);

    // Aplicamos la regla aprendida (aprendizaje.M) a los nuevos datos
    TetraFaceResult prediccion;
    tetra_sintetizador_infer(A_new, B_new, aprendizaje.M, &prediccion);

    print_vec("Prediccion del Sistema (R)", prediccion.R);

    // Validación
    Trit R_esperado[3] = {0, 1, 1};
    if(eq3f(prediccion.R, R_esperado)) {
        printf("\n>> EXITO TOTAL: El sistema predijo el futuro correctamente.\n");
        printf("   Aprendio la regla abstracta y la aplico a una nueva realidad.\n");
    } else {
        printf("\n>> FALLO EN PREDICCION.\n");
    }

    // --- BONUS: EMERGENCIA (O) ---
    // El tetraedro también calcula 'O' (El Observador/Resultado Colapsado)
    printf("\n--- BONUS: COLAPSO DEL OBSERVADOR (O) ---\n");
    print_vec("Vector O (Sintesis final)", prediccion.O);
    // O es el colapso triádico de A, B y R.
    // Si A=0, B=1, R=0 -> Mayoría 0. O debe ser 0.
    
    return 0;
}