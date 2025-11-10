// tetraedro.c - Complete Tetrahedron implementation with 4 faces
#include "tetraedro.h"
#include <stdio.h>
#include <string.h>

// ======= INTERNAL UTILITIES =======

static int count_nulls(const Dim* d) {
    return (d->FO == T2) + (d->FN == T2) + (d->ES == T2);
}

static Trit majority3_trit(Trit a, Trit b, Trit c) {
    if (a==b || a==c) return a;
    if (b==c) return b;
    return T2; // empate → null
}

// Deduce trit: dado modo M, un input A y resultado R, ¿qué B se necesita?
static Trit deduce_trit(Trit A, Trit M, Trit R) {
    // Inverso de infer3: A op B = R, conocemos A,M,R → encontrar B
    // M=T0 (OR): R = max(A,B) → B = R si R>=A, sino libre
    // M=T1 (AND): R = min(A,B) → B = R si R<=A, sino libre
    // M=T2 (CONSENSUS): R = majority(A,B,A) → B = R si R!=A, sino libre
    
    if (M == T0) { // OR
        if (R >= A) return R;
        return T2; // no se puede deducir con certeza
    } else if (M == T1) { // AND
        if (R <= A) return R;
        return T2;
    } else { // CONSENSUS
        if (R != A) return R;
        return T2;
    }
}

// ======= SINTETIZADOR (Forward: A,B → M,R,O) =======

TriGateOutput trigate_infer(const Dim* A, const Dim* B, const Dim* O_hint) {
    TriGateOutput out = {0};
    
    // R: resultado de operar A y B (usa modo default OR para síntesis inicial)
    out.R.FO = infer3(A->FO, B->FO, T0); // OR en datos
    out.R.FN = infer3(A->FN, B->FN, T0); // OR en control
    out.R.ES = infer3(A->ES, B->ES, T0); // OR en coordinación
    
    // M: modo aprendido (qué operación relaciona A,B → R)
    out.M = trigate_learn(A, B, &out.R);
    
    // O: orden sugerido (usa hint si existe, sino infiere de estructura)
    if (O_hint) {
        out.O = *O_hint;
    } else {
        // Orden por defecto: prioriza campo con menor entropía
        int e_FO = (A->FO != B->FO) + (B->FO != out.R.FO);
        int e_FN = (A->FN != B->FN) + (B->FN != out.R.FN);
        int e_ES = (A->ES != B->ES) + (B->ES != out.R.ES);
        
        out.O.FO = (e_FO <= e_FN && e_FO <= e_ES) ? T0 : T2;
        out.O.FN = (e_FN <= e_FO && e_FN <= e_ES) ? T0 : T2;
        out.O.ES = (e_ES <= e_FO && e_ES <= e_FN) ? T0 : T2;
    }
    
    return out;
}

// ======= EVOLVER (Refine M based on A,B,R) =======

Dim trigate_learn(const Dim* A, const Dim* B, const Dim* R) {
    Dim M = {0};
    
    // Para cada campo, determina qué operación (AND/OR/CONSENSUS) generó R
    // Si R = AND(A,B) → M=T1; si R = OR(A,B) → M=T0; sino → M=T2
    
    // FO
    Trit and_FO = (A->FO <= B->FO) ? A->FO : B->FO;
    Trit or_FO  = (A->FO >= B->FO) ? A->FO : B->FO;
    if (R->FO == and_FO) M.FO = T1;
    else if (R->FO == or_FO) M.FO = T0;
    else M.FO = T2;
    
    // FN
    Trit and_FN = (A->FN <= B->FN) ? A->FN : B->FN;
    Trit or_FN  = (A->FN >= B->FN) ? A->FN : B->FN;
    if (R->FN == and_FN) M.FN = T1;
    else if (R->FN == or_FN) M.FN = T0;
    else M.FN = T2;
    
    // ES
    Trit and_ES = (A->ES <= B->ES) ? A->ES : B->ES;
    Trit or_ES  = (A->ES >= B->ES) ? A->ES : B->ES;
    if (R->ES == and_ES) M.ES = T1;
    else if (R->ES == or_ES) M.ES = T0;
    else M.ES = T2;
    
    return M;
}

// ======= EXTENDER (Reverse: M,R,O → A,B) =======

DimPair trigate_extend(const Dim* M, const Dim* R, const Dim* O) {
    DimPair pair = {0};
    
    // Reconstruir A y B desde M (modo) y R (resultado)
    // O determina el orden de prioridad (quién se deduce primero)
    
    // Estrategia: 
    // - Si O.FO=T0 (prioridad a FO), A.FO=R.FO, B.FO se deduce
    // - Si O.FO=T1 (equilibrio), A.FO y B.FO se promedian desde R.FO
    // - Si O.FO=T2 (null/indefinido), ambos son R.FO (copia)
    
    // FO
    if (O->FO == T0) {
        pair.A.FO = R->FO;
        pair.B.FO = deduce_trit(pair.A.FO, M->FO, R->FO);
    } else if (O->FO == T1) {
        // Equilibrio: ambos contribuyen por igual
        pair.A.FO = (R->FO == T0) ? T0 : ((R->FO == T1) ? T0 : T1);
        pair.B.FO = (R->FO == T0) ? T0 : ((R->FO == T1) ? T1 : T2);
    } else {
        pair.A.FO = R->FO;
        pair.B.FO = R->FO;
    }
    
    // FN
    if (O->FN == T0) {
        pair.A.FN = R->FN;
        pair.B.FN = deduce_trit(pair.A.FN, M->FN, R->FN);
    } else if (O->FN == T1) {
        pair.A.FN = (R->FN == T0) ? T0 : ((R->FN == T1) ? T0 : T1);
        pair.B.FN = (R->FN == T0) ? T0 : ((R->FN == T1) ? T1 : T2);
    } else {
        pair.A.FN = R->FN;
        pair.B.FN = R->FN;
    }
    
    // ES
    if (O->ES == T0) {
        pair.A.ES = R->ES;
        pair.B.ES = deduce_trit(pair.A.ES, M->ES, R->ES);
    } else if (O->ES == T1) {
        pair.A.ES = (R->ES == T0) ? T0 : ((R->ES == T1) ? T0 : T1);
        pair.B.ES = (R->ES == T0) ? T0 : ((R->ES == T1) ? T1 : T2);
    } else {
        pair.A.ES = R->ES;
        pair.B.ES = R->ES;
    }
    
    return pair;
}

// ======= ARMONIZADOR (Optimize O to minimize nulls) =======

Dim harmonize_order(const Dim* M, const Dim* R, int iteration) {
    Dim O = {0};
    
    // Heurística Fibonacci para rotación O→M→R
    // iteration mod 3: 0→prioriza FO, 1→prioriza FN, 2→prioriza ES
    int focus = iteration % 3;
    
    // Cuenta nulls en M y R
    int nulls_M = count_nulls(M);
    int nulls_R = count_nulls(R);
    
    // Si hay muchos nulls, rotar orden para forzar coherencia
    if (nulls_M + nulls_R > 3) {
        // Rotación agresiva
        O.FO = (focus == 0) ? T0 : T2;
        O.FN = (focus == 1) ? T0 : T2;
        O.ES = (focus == 2) ? T0 : T2;
    } else {
        // Orden conservador: mantener lo que funciona
        O.FO = (M->FO != T2 && R->FO != T2) ? T0 : T1;
        O.FN = (M->FN != T2 && R->FN != T2) ? T0 : T1;
        O.ES = (M->ES != T2 && R->ES != T2) ? T0 : T1;
    }
    
    return O;
}

// ======= TETRAEDRO FULL CYCLE =======

TetraedroOutput tetraedro_process(const TetraedroInput* input, int max_iterations) {
    TetraedroOutput out = {0};
    
    // Extraer dimensiones de cada vector (simplificación: usamos dim[0])
    Dim A = input->A.dim[0];
    Dim B = input->B.dim[0];
    Dim C = input->C.dim[0]; // contexto (usado para O inicial)
    
    Dim O_current = C; // O inicial desde contexto
    TriGateOutput synthesis;
    
    for (int iter = 0; iter < max_iterations; iter++) {
        // 1. SINTETIZADOR: A,B → M,R,O
        synthesis = trigate_infer(&A, &B, &O_current);
        
        // 2. EVOLVER: refinar M
        Dim M_refined = trigate_learn(&A, &B, &synthesis.R);
        synthesis.M = M_refined;
        
        // 3. EXTENDER: M,R,O → A',B' (validación: ¿podemos reconstruir?)
        DimPair reconstructed = trigate_extend(&synthesis.M, &synthesis.R, &synthesis.O);
        
        // 4. ARMONIZADOR: ajustar O para minimizar nulls
        O_current = harmonize_order(&synthesis.M, &synthesis.R, iter);
        
        // Verificar coherencia: ¿A' ≈ A y B' ≈ B?
        int coherence = 0;
        coherence += (reconstructed.A.FO == A.FO) + (reconstructed.A.FN == A.FN) + (reconstructed.A.ES == A.ES);
        coherence += (reconstructed.B.FO == B.FO) + (reconstructed.B.FN == B.FN) + (reconstructed.B.ES == B.ES);
        
        out.coherence_level = coherence;
        out.null_count = count_nulls(&synthesis.M) + count_nulls(&synthesis.R) + count_nulls(&synthesis.O);
        
        // Convergencia: alta coherencia y pocos nulls
        if (coherence >= 5 && out.null_count <= 1) {
            out.converged = true;
            break;
        }
    }
    
    out.synthesis = synthesis;
    
    // EMERGENCIA: sintetizar (Mₛ, Rₛ, Oₛ) del ciclo completo
    out.emergent.FO = synthesis.R.FO;  // Forma emergente = resultado final
    out.emergent.FN = synthesis.M.FN;  // Función emergente = modo aprendido
    out.emergent.ES = synthesis.O.ES;  // Estructura emergente = orden óptimo
    
    return out;
}

// ======= UTILITIES =======

void print_trigate_output(const TriGateOutput* out, const char* label) {
    printf("\n%s:\n", label);
    printf("  M: FO=%s FN=%s ES=%s\n", 
           trit_str(out->M.FO), trit_str(out->M.FN), trit_str(out->M.ES));
    printf("  R: FO=%s FN=%s ES=%s\n", 
           trit_str(out->R.FO), trit_str(out->R.FN), trit_str(out->R.ES));
    printf("  O: FO=%s FN=%s ES=%s\n", 
           trit_str(out->O.FO), trit_str(out->O.FN), trit_str(out->O.ES));
}

void print_tetraedro_output(const TetraedroOutput* out) {
    printf("\n=== TETRAEDRO OUTPUT ===\n");
    print_trigate_output(&out->synthesis, "Final Synthesis");
    printf("\nEmergent Dimension: FO=%s FN=%s ES=%s\n",
           trit_str(out->emergent.FO),
           trit_str(out->emergent.FN),
           trit_str(out->emergent.ES));
    printf("Coherence level: %d/6\n", out->coherence_level);
    printf("Null count: %d\n", out->null_count);
    printf("Converged: %s\n", out->converged ? "YES" : "NO");
}
