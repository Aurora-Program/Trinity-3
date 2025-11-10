#ifndef AURORA_CORE_H
#define AURORA_CORE_H

#include <stdint.h>
#include <stdbool.h>

// ======= TERNARY BASE =======
typedef enum { T0=0, T1=1, T2=2 } Trit;

// ======= AURORA TYPES =======
typedef struct { 
    Trit FO;  // Forma/Data
    Trit FN;  // Función/Control  
    Trit ES;  // Estructura/Coordinación
} Dim;

typedef enum { 
    CTRL=0,   // Controla modo de operación (escribe FN)
    DATA=1,   // Procesa información (escribe FO)
    COORD=2   // Coordina flujo (escribe ES)
} Role;

typedef struct { 
    Dim dim[3]; 
    Role roles[3];        // Roles actuales (pueden cambiar)
    uint8_t stability[3]; // Contador de estabilidad por dimensión (0-255)
} Vector;

// ======= ROLE CONTEXT =======
typedef struct {
    int entropy_FO;    // Entropía de datos
    int entropy_FN;    // Entropía de control
    int entropy_ES;    // Entropía de coordinación
    bool stable;       // Sistema estable
    int cycles_stable; // Ciclos consecutivos estables
} RoleContext;

// ======= CORE OPERATIONS =======

// Trigate operations (mode determined by FN of CTRL dimension)
Trit infer3(Trit A, Trit B, Trit mode);

// Role inference from context
RoleContext analyze_context(const Vector* v);
void infer_roles(Vector* v, const RoleContext* ctx);

// Full processing step
typedef struct { 
    Trit r01, r12, r20;  // FO-stage results
} FO_Results;

typedef struct { 
    FO_Results fo; 
    Dim emergent; 
    Role old_roles[3];   // Roles antes del paso
    Role new_roles[3];   // Roles después (pueden cambiar)
    bool role_changed;   // Flag: hubo cambio de roles
} StepOut;

StepOut step(Vector* v, unsigned long cycle);

// Utilities
const char* trit_str(Trit t);
const char* role_str(Role r);
void print_vector(const Vector* v, const char* label);

#endif // AURORA_CORE_H
