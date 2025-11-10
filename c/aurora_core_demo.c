// aurora_core_demo.c - Demo showing dynamic role adaptation
#include "aurora_core.h"
#include <stdio.h>

int main(void){
    // Vector inicial con roles arbitrarios
    Vector v = {
        .dim = {
            { .FO=T1, .FN=T0, .ES=T0 },
            { .FO=T2, .FN=T1, .ES=T1 },
            { .FO=T0, .FN=T2, .ES=T2 }
        },
        .roles = {CTRL, DATA, COORD}, // roles iniciales
        .stability = {0, 0, 0}
    };

    printf("=== AURORA CORE: Dynamic Role Discovery Demo ===\n");
    print_vector(&v, "INITIAL STATE");
    
    printf("\n--- Processing cycles (roles adapt to context) ---\n");
    
    for (unsigned long k=0; k<12; k++){
        StepOut so = step(&v, k);
        
        printf("\n>> CYCLE %lu", k);
        
        // Mostrar cambio de roles si ocurrió
        if (so.role_changed) {
            printf(" ** ROLES CHANGED **");
            printf("\n   Old: [%s, %s, %s]", 
                   role_str(so.old_roles[0]),
                   role_str(so.old_roles[1]),
                   role_str(so.old_roles[2]));
            printf("\n   New: [%s, %s, %s]",
                   role_str(so.new_roles[0]),
                   role_str(so.new_roles[1]),
                   role_str(so.new_roles[2]));
        } else {
            printf(" (roles stable: [%s, %s, %s])",
                   role_str(so.new_roles[0]),
                   role_str(so.new_roles[1]),
                   role_str(so.new_roles[2]));
        }
        
        printf("\n   FO-results: r01=%s r12=%s r20=%s",
               trit_str(so.fo.r01), 
               trit_str(so.fo.r12), 
               trit_str(so.fo.r20));
        
        printf("\n   Emergent: FO=%s FN=%s ES=%s",
               trit_str(so.emergent.FO),
               trit_str(so.emergent.FN),
               trit_str(so.emergent.ES));
        
        // Mostrar estado actual cada 3 ciclos
        if ((k+1) % 3 == 0) {
            print_vector(&v, "Current State");
        }
    }
    
    printf("\n\n=== FINAL STATE ===");
    print_vector(&v, "After 12 cycles");
    
    RoleContext final_ctx = analyze_context(&v);
    printf("\nFinal context:");
    printf("\n  entropy_FO=%d, entropy_FN=%d, entropy_ES=%d",
           final_ctx.entropy_FO, final_ctx.entropy_FN, final_ctx.entropy_ES);
    printf("\n  stable=%s, cycles_stable=%d\n",
           final_ctx.stable?"YES":"NO", final_ctx.cycles_stable);
    
    return 0;
}
