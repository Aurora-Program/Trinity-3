// aurora_core.c - Aurora model with dynamic context-aware role discovery
#include "aurora_core.h"
#include <stdio.h>
#include <string.h>

// ======= TERNARY UTILITIES =======
static Trit majority3(Trit a, Trit b, Trit c){
    if (a==b || a==c) return a;
    if (b==c) return b;
    return T0; // empate triple: política mínima
}

static int entropy3(Trit a, Trit b, Trit c){
    int d = (a!=b) + (a!=c) + (b!=c);
    return (d==0)?0: (d==2?1:2); // 0=homogéneo, 1=parcial, 2=heterogéneo
}

static Trit or3(Trit a, Trit b){ return (a>=b)?a:b; }
static Trit and3(Trit a, Trit b){ return (a<=b)?a:b; }

// ======= TRIGATE =======
Trit infer3(Trit A, Trit B, Trit mode){
    if (mode==T0) return or3(A,B);
    if (mode==T1) return and3(A,B);
    return majority3(A,B,A); // T2 -> consenso
}

// ======= ROLE CONTEXT ANALYSIS =======
RoleContext analyze_context(const Vector* v){
    RoleContext ctx = {0};
    
    // Entropía por campo
    ctx.entropy_FO = entropy3(v->dim[0].FO, v->dim[1].FO, v->dim[2].FO);
    ctx.entropy_FN = entropy3(v->dim[0].FN, v->dim[1].FN, v->dim[2].FN);
    ctx.entropy_ES = entropy3(v->dim[0].ES, v->dim[1].ES, v->dim[2].ES);
    
    // Sistema estable si todos los campos tienen baja entropía
    ctx.stable = (ctx.entropy_FO <= 1) && (ctx.entropy_FN <= 1) && (ctx.entropy_ES <= 1);
    
    // Ciclos estables: promedio de contadores de estabilidad
    ctx.cycles_stable = (v->stability[0] + v->stability[1] + v->stability[2]) / 3;
    
    return ctx;
}

// ======= DYNAMIC ROLE INFERENCE =======
// Reglas de asignación contextual:
// - Alta entropía FO + baja FN → necesita CTRL (alguien debe decidir modo)
// - Baja entropía FO + alta ES → necesita DATA (procesar info coherente)
// - Alta entropía ES → necesita COORD (reorganizar flujo)
void infer_roles(Vector* v, const RoleContext* ctx){
    // Puntuaciones por dimensión para cada rol (0-10)
    int score_ctrl[3] = {0};
    int score_data[3] = {0};
    int score_coord[3] = {0};
    
    for(int i=0; i<3; i++){
        // CTRL: preferido si FN es estable (puede dictar modo coherente)
        //       o si sistema está inestable (necesita control)
        if (v->dim[i].FN == v->dim[(i+1)%3].FN) score_ctrl[i] += 3; // FN coherente
        if (!ctx->stable) score_ctrl[i] += 2; // sistema necesita control
        if (ctx->entropy_FO > 1) score_ctrl[i] += 2; // datos caóticos
        
        // DATA: preferido si FO tiene valores distintos (diversidad de datos)
        //       y FN está estabilizado (modo claro)
        if (v->dim[i].FO != v->dim[(i+1)%3].FO) score_data[i] += 3; // FO diverso
        if (ctx->entropy_FN == 0) score_data[i] += 3; // modo estable
        if (v->stability[i] > 3) score_data[i] += 1; // dimensión madura
        
        // COORD: preferido si ES varía (puede enrutar dinámicamente)
        //        o si sistema está estable (mantener flujo)
        if (v->dim[i].ES != v->dim[(i+1)%3].ES) score_coord[i] += 3; // ES variado
        if (ctx->stable) score_coord[i] += 2; // sistema necesita coordinación
        if (ctx->entropy_ES > 0) score_coord[i] += 2; // routing dinámico
    }
    
    // Asignación greedy: cada rol se asigna a su mejor candidato
    // (evitando conflictos)
    bool assigned[3] = {false};
    Role new_roles[3] = {CTRL, DATA, COORD}; // default
    
    // Paso 1: asignar CTRL (más crítico)
    int best_ctrl = 0;
    for(int i=1; i<3; i++)
        if (score_ctrl[i] > score_ctrl[best_ctrl]) best_ctrl = i;
    new_roles[best_ctrl] = CTRL;
    assigned[best_ctrl] = true;
    
    // Paso 2: asignar DATA (segundo más crítico)
    int best_data = (best_ctrl==0)?1:0;
    for(int i=0; i<3; i++)
        if (!assigned[i] && score_data[i] > score_data[best_data]) best_data = i;
    new_roles[best_data] = DATA;
    assigned[best_data] = true;
    
    // Paso 3: asignar COORD (el restante)
    for(int i=0; i<3; i++)
        if (!assigned[i]) new_roles[i] = COORD;
    
    // Actualizar roles
    memcpy(v->roles, new_roles, sizeof(new_roles));
}

// ======= ROLE-BASED WRITE MASKS =======
typedef struct { bool FO, FN, ES; } WriteMask;

static WriteMask get_mask(Role r){
    WriteMask m = {false, false, false};
    if (r == DATA)  m.FO = true;
    if (r == CTRL)  m.FN = true;
    if (r == COORD) m.ES = true;
    return m;
}

static inline void try_set_FO(Dim* d, Trit v, WriteMask m){ if(m.FO) d->FO = v; }
static inline void try_set_FN(Dim* d, Trit v, WriteMask m){ if(m.FN) d->FN = v; }
static inline void try_set_ES(Dim* d, Trit v, WriteMask m){ if(m.ES) d->ES = v; }

// ======= ACTIVE PAIR ROUTING =======
typedef struct { int a, b; } Pair;

static Pair active_pair(const Vector* v){
    // COORD usa ES para elegir par
    int coord = -1;
    for(int i=0; i<3; i++) 
        if (v->roles[i] == COORD) { coord = i; break; }
    
    if (coord == -1) return (Pair){0,1}; // fallback
    
    Trit r = v->dim[coord].ES;
    if (r==T0) return (Pair){0,1};
    if (r==T1) return (Pair){1,2};
    return (Pair){2,0};
}

// ======= FO-STAGE (compute) =======
static FO_Results stage_FO(const Vector* v){
    FO_Results o={0};
    
    // Modo: CTRL dicta mediante FN
    int ctrl = -1;
    for(int i=0; i<3; i++)
        if (v->roles[i] == CTRL) { ctrl = i; break; }
    
    Trit mode = (ctrl>=0)? v->dim[ctrl].FN : T2; // default: consensus
    
    // Enrutado: COORD elige par activo
    Pair p = active_pair(v);
    
    // Datos: siempre desde FO
    Trit FO0=v->dim[0].FO, FO1=v->dim[1].FO, FO2=v->dim[2].FO;
    
    // Solo el par activo se procesa
    o.r01 = (p.a==0 && p.b==1)? infer3(FO0,FO1,mode) : FO0;
    o.r12 = (p.a==1 && p.b==2)? infer3(FO1,FO2,mode) : FO1;
    o.r20 = (p.a==2 && p.b==0)? infer3(FO2,FO0,mode) : FO2;
    
    return o;
}

// ======= M-STAGE (learn & adapt) =======
static Trit learn_mode_simple(Trit a, Trit b, Trit r){
    if (r==and3(a,b)) return T1;
    if (r==or3(a,b))  return T0;
    return T2;
}

static void stage_M(Vector* v, FO_Results fo){
    WriteMask masks[3];
    for(int i=0; i<3; i++) masks[i] = get_mask(v->roles[i]);
    
    // CTRL ajusta FN
    int ctrl = -1;
    for(int i=0; i<3; i++)
        if (v->roles[i] == CTRL) { ctrl = i; break; }
    
    if (ctrl >= 0) {
        Trit aFN = v->dim[(ctrl+2)%3].FN;
        Trit bFN = v->dim[(ctrl+1)%3].FN;
        Trit r   = (ctrl==0)?fo.r20 : (ctrl==1)?fo.r01 : fo.r12;
        Trit newFN = learn_mode_simple(aFN,bFN,r);
        try_set_FN(&v->dim[ctrl], newFN, masks[ctrl]);
    }
    
    // DATA refina FO
    int data = -1;
    for(int i=0; i<3; i++)
        if (v->roles[i] == DATA) { data = i; break; }
    
    if (data >= 0) {
        Trit updFO = (data==0)?fo.r01 : (data==1)?fo.r12 : fo.r20;
        try_set_FO(&v->dim[data], updFO, masks[data]);
    }
    
    // COORD ajusta ES según entropía mínima
    int coord = -1;
    for(int i=0; i<3; i++)
        if (v->roles[i] == COORD) { coord = i; break; }
    
    if (coord >= 0) {
        int e0 = entropy3(v->dim[0].FO, v->dim[1].FO, fo.r01);
        int e1 = entropy3(v->dim[1].FO, v->dim[2].FO, fo.r12);
        int e2 = entropy3(v->dim[2].FO, v->dim[0].FO, fo.r20);
        Trit nextES = (e0<=e1 && e0<=e2)? T0 : (e1<=e2? T1 : T2);
        try_set_ES(&v->dim[coord], nextES, masks[coord]);
    }
}

// ======= EMERGENCE =======
static Trit emergent_FO(FO_Results fo){
    return majority3(fo.r01, fo.r12, fo.r20);
}

static Trit emergent_FN(const Vector* v, FO_Results fo){
    Trit m = majority3(v->dim[0].FN, v->dim[1].FN, v->dim[2].FN);
    int e = entropy3(fo.r01, fo.r12, fo.r20);
    return (e==2)? T2 : m; // alta entropía → consensus
}

static Trit emergent_ES(const Vector* v){
    int e0 = entropy3(v->dim[0].FO, v->dim[1].FO, v->dim[0].FO);
    int e1 = entropy3(v->dim[1].FO, v->dim[2].FO, v->dim[1].FO);
    int e2 = entropy3(v->dim[2].FO, v->dim[0].FO, v->dim[2].FO);
    return (e0<=e1 && e0<=e2)? T0 : (e1<=e2? T1 : T2);
}

static Dim emergent_dim(const Vector* v, FO_Results fo){
    Dim E;
    E.FO = emergent_FO(fo);
    E.FN = emergent_FN(v,fo);
    E.ES = emergent_ES(v);
    return E;
}

// ======= STABILITY TRACKING =======
static void update_stability(Vector* v, bool changed){
    for(int i=0; i<3; i++){
        if (changed) {
            v->stability[i] = 0; // reset si hubo cambio
        } else {
            if (v->stability[i] < 255) v->stability[i]++;
        }
    }
}

// ======= FULL STEP =======
StepOut step(Vector* v, unsigned long cycle){
    (void)cycle; // puede usarse para heurísticas temporales
    
    StepOut out = {0};
    
    // Guardar roles antiguos
    memcpy(out.old_roles, v->roles, sizeof(v->roles));
    
    // Analizar contexto
    RoleContext ctx = analyze_context(v);
    
    // Inferir nuevos roles si sistema inestable o cada N ciclos
    if (!ctx.stable || ctx.cycles_stable > 5) {
        infer_roles(v, &ctx);
    }
    
    memcpy(out.new_roles, v->roles, sizeof(v->roles));
    out.role_changed = (memcmp(out.old_roles, out.new_roles, sizeof(v->roles)) != 0);
    
    // Procesar
    out.fo = stage_FO(v);
    stage_M(v, out.fo);
    out.emergent = emergent_dim(v, out.fo);
    
    // Actualizar estabilidad
    update_stability(v, out.role_changed);
    
    return out;
}

// ======= UTILITIES =======
const char* trit_str(Trit t){ 
    return (t==T0)?"0":(t==T1)?"1":"2"; 
}

const char* role_str(Role r){
    return (r==CTRL)?"CTRL":(r==DATA)?"DATA":"COORD";
}

void print_vector(const Vector* v, const char* label){
    printf("\n%s:\n", label);
    for(int i=0; i<3; i++){
        printf("  dim%d [%s]: FO=%s FN=%s ES=%s (stable=%d)\n", 
               i, role_str(v->roles[i]),
               trit_str(v->dim[i].FO), 
               trit_str(v->dim[i].FN), 
               trit_str(v->dim[i].ES),
               v->stability[i]);
    }
}
