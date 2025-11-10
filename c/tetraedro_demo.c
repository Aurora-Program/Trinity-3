// tetraedro_demo.c - Demo showing complete Tetrahedron with 4 faces
#include "tetraedro.h"
#include <stdio.h>

int main(void) {
    printf("=== TETRAEDRO COMPLETO: 4 Faces Demo ===\n");
    printf("Sintetizador → Evolver → Extender → Armonizador\n\n");
    
    // Crear 3 vectores de entrada
    TetraedroInput input = {
        .A = {
            .dim = {
                { .FO=T1, .FN=T0, .ES=T0 },
                { .FO=T0, .FN=T1, .ES=T1 },
                { .FO=T1, .FN=T0, .ES=T2 }
            },
            .roles = {DATA, CTRL, COORD},
            .stability = {0,0,0}
        },
        .B = {
            .dim = {
                { .FO=T2, .FN=T1, .ES=T1 },
                { .FO=T1, .FN=T0, .ES=T0 },
                { .FO=T0, .FN=T2, .ES=T1 }
            },
            .roles = {CTRL, DATA, COORD},
            .stability = {0,0,0}
        },
        .C = { // Contexto (proporciona O inicial)
            .dim = {
                { .FO=T0, .FN=T0, .ES=T0 },
                { .FO=T1, .FN=T1, .ES=T1 },
                { .FO=T0, .FN=T0, .ES=T2 }
            },
            .roles = {COORD, DATA, CTRL},
            .stability = {0,0,0}
        }
    };
    
    printf("INPUT VECTORS:\n");
    print_vector(&input.A, "Vector A");
    print_vector(&input.B, "Vector B");
    print_vector(&input.C, "Vector C (context)");
    
    printf("\n\n--- Processing through Tetraedron (max 10 iterations) ---\n");
    
    TetraedroOutput result = tetraedro_process(&input, 10);
    
    print_tetraedro_output(&result);
    
    printf("\n\n=== FACE BY FACE BREAKDOWN ===\n");
    
    // Mostrar cada cara individualmente
    Dim A_dim = input.A.dim[0];
    Dim B_dim = input.B.dim[0];
    Dim C_dim = input.C.dim[0];
    
    printf("\n1. SINTETIZADOR (A,B → M,R,O):\n");
    TriGateOutput synth = trigate_infer(&A_dim, &B_dim, &C_dim);
    print_trigate_output(&synth, "Synthesis from A,B");
    
    printf("\n2. EVOLVER (A,B,R → M refined):\n");
    Dim M_learned = trigate_learn(&A_dim, &B_dim, &synth.R);
    printf("   Learned M: FO=%s FN=%s ES=%s\n",
           trit_str(M_learned.FO), trit_str(M_learned.FN), trit_str(M_learned.ES));
    
    printf("\n3. EXTENDER (M,R,O → A',B' reconstruction):\n");
    DimPair reconstructed = trigate_extend(&synth.M, &synth.R, &synth.O);
    printf("   A': FO=%s FN=%s ES=%s\n",
           trit_str(reconstructed.A.FO),
           trit_str(reconstructed.A.FN),
           trit_str(reconstructed.A.ES));
    printf("   B': FO=%s FN=%s ES=%s\n",
           trit_str(reconstructed.B.FO),
           trit_str(reconstructed.B.FN),
           trit_str(reconstructed.B.ES));
    
    printf("\n   Original A: FO=%s FN=%s ES=%s\n",
           trit_str(A_dim.FO), trit_str(A_dim.FN), trit_str(A_dim.ES));
    printf("   Original B: FO=%s FN=%s ES=%s\n",
           trit_str(B_dim.FO), trit_str(B_dim.FN), trit_str(B_dim.ES));
    
    int match_A = (reconstructed.A.FO == A_dim.FO) + 
                  (reconstructed.A.FN == A_dim.FN) + 
                  (reconstructed.A.ES == A_dim.ES);
    int match_B = (reconstructed.B.FO == B_dim.FO) + 
                  (reconstructed.B.FN == B_dim.FN) + 
                  (reconstructed.B.ES == B_dim.ES);
    
    printf("\n   Reconstruction accuracy: A=%d/3, B=%d/3\n", match_A, match_B);
    
    printf("\n4. ARMONIZADOR (M,R → O optimized):\n");
    for (int i=0; i<5; i++) {
        Dim O_harm = harmonize_order(&synth.M, &synth.R, i);
        printf("   iteration %d: O={FO=%s, FN=%s, ES=%s}\n", i,
               trit_str(O_harm.FO), trit_str(O_harm.FN), trit_str(O_harm.ES));
    }
    
    printf("\n\n=== CYCLE SUMMARY ===\n");
    printf("The Tetrahedron processes 3 vectors through 4 faces:\n");
    printf("  • Sintetizador: Combines A,B into M(mode), R(result), O(order)\n");
    printf("  • Evolver: Refines M by learning from A,B,R relationships\n");
    printf("  • Extender: Reconstructs A',B' from M,R,O (validates coherence)\n");
    printf("  • Armonizador: Optimizes O to minimize nulls and maximize coherence\n");
    printf("\nFinal emergent dimension synthesizes the complete cycle:\n");
    printf("  Emergent = {FO(form), FN(function), ES(structure)}\n");
    
    return 0;
}
