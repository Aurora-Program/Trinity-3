/*
 * aurora_showcase.c
 *
 * Archivo único para compartir el núcleo demostrativo Aurora con pares.
 * Incluye: trigate ternario, reglas + arquetipos + tensor C, bootstrap léxico,
 * modo sueño (autopoda / apoptosis), persistencia (texto, incremental y binaria
 * comprimida), y CLI básica.
 *
 * Principios: menos es más, fractal 1→3→9, roles FO/FN/ES autosimilares.
 * Licencias: Apache 2.0 + CC BY 4.0.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* === Trit & Trigate ===================================================== */
typedef int Trit; /* 1,0,-1 (c, u, n) */
static Trit T_AND(Trit a,Trit b){ if(a==0||b==0) return 0; if(a==1&&b==1) return 1; return -1; }
static Trit T_OR(Trit a,Trit b){ if(a==1||b==1) return 1; if(a==0&&b==0) return 0; return -1; }
static Trit T_CONS(Trit a,Trit b){ if(a!=-1 && a==b) return a; return -1; }
static Trit trigate(Trit a,Trit b,Trit m){ return m==0?T_AND(a,b): (m==1?T_OR(a,b):T_CONS(a,b)); }
static Trit trigate_learn(Trit a,Trit b,Trit r){ if(T_AND(a,b)==r) return 0; if(T_OR(a,b)==r) return 1; if(a!=-1 && a==b && r==a) return -1; return -1; }
static const char* ts(Trit t){ return t==1?"1":(t==0?"0":"N"); }
static Trit char_to_trit(char c){ if(c=='1') return 1; if(c=='0') return 0; return -1; }

/* === Estructuras ======================================================== */
typedef struct { Trit A[3]; Trit B[3]; Trit M[3]; int count; unsigned long rev; } Rule;
typedef struct { Trit pattern_A[3]; Trit pattern_B[3]; Trit pattern_M[3]; int support; unsigned long rev; } Archetype;
typedef struct { Trit FO[3]; Trit FN[3]; Trit ES[3]; } TensorC;

#define MAX_RULES 512
#define MAX_ARCH 64
static Rule rules[MAX_RULES]; static int n_rules=0;
static Archetype archs[MAX_ARCH]; static int n_arch=0;
static TensorC Ct; static int C_valid=0;
static unsigned long global_rev=1, last_saved_rev=0;
static int spirit_active=0; /* El espíritu emergente del sistema */
static int synthesis_order_index=0;  /* Índice Fibonacci: coordina CLUSTER de 3 tetraedros de entrada (Relator/Arquetipo/Dinámico) */

/* Serie Fibonacci: cada valor se descompone en base 3 para guiar el CLUSTER de 3 tetraedros de entrada */
/* Fib[i] en base 3 = [d0, d1, d2] → Os_Relator=d0, Os_Arquetipo=d1, Os_Dinámico=d2 */
static const int FIB_SERIES[]={0,1,1,2,3,5,8,13,21,34,55,89,144,233,377,610};
static const int FIB_MAX=15;

/* === Utilidades ======================================================== */
static int eq3(const Trit a[3],const Trit b[3]){ for(int i=0;i<3;i++) if(a[i]!=b[i]) return 0; return 1; }
static void cp3(Trit d[3],const Trit s[3]){ for(int i=0;i<3;i++) d[i]=s[i]; }

/* Convierte número decimal a base 3 (tritario) */
static void decimal_to_base3(int num,Trit out[3]){
    out[2]=(num%3==2)?1:(num%3==1?0:-1); num/=3;
    out[1]=(num%3==2)?1:(num%3==1?0:-1); num/=3;
    out[0]=(num%3==2)?1:(num%3==1?0:-1);
}

/* Crea tensor C a partir de valor Fibonacci */
/* Fib[i] se descompone en base 3: cada dígito guiará un tetraedro (Relator/Arquetipo/Dinámico) */
static void fib_to_C(int fib_idx,TensorC* c){
    if(fib_idx<0) fib_idx=0;
    if(fib_idx>FIB_MAX) fib_idx=FIB_MAX;
    int val=FIB_SERIES[fib_idx];
    /* FO = dígito 0 (Relator), FN = dígito 1 (Arquetipo), ES = dígito 2 (Dinámico)
       Nota: aquí usamos C como “reflejo” del anclaje Os del CLUSTER de entrada.
       El armonizador podrá usar estos Os como referencia de armonización. */
    decimal_to_base3(val,c->FO);  /* Os del Tetraedro Relator */
    decimal_to_base3(val,c->FN);  /* Os del Tetraedro Arquetipo */
    decimal_to_base3(val,c->ES);  /* Os del Tetraedro Dinámico */
}

/* Obtiene qué tetraedro debe operar primero según Fibonacci */
static int get_synthesis_order(int iteration){
    if(iteration<0) iteration=0;
    if(iteration>FIB_MAX) iteration=FIB_MAX;
    return FIB_SERIES[iteration] % 3; /* 0=Relator, 1=Arquetipo, 2=Dinámico */
}
static void vec_infer(const Trit A[3],const Trit B[3],const Trit M[3],Trit R[3]){ for(int i=0;i<3;i++) R[i]=trigate(A[i],B[i],M[i]); }
static void vec_learn(const Trit A[3],const Trit B[3],const Trit R[3],Trit M[3]){ for(int i=0;i<3;i++) M[i]=trigate_learn(A[i],B[i],R[i]); }
static int find_rule(const Trit A[3],const Trit B[3]){ for(int i=0;i<n_rules;i++) if(eq3(rules[i].A,A)&&eq3(rules[i].B,B)) return i; return -1; }

static void upsert_rule(const Trit A[3],const Trit B[3],const Trit M[3]){
    int k=find_rule(A,B); if(k<0 && n_rules<MAX_RULES){ k=n_rules++; cp3(rules[k].A,A); cp3(rules[k].B,B); for(int i=0;i<3;i++) rules[k].M[i]=-1; rules[k].count=0; rules[k].rev=global_rev++; }
    if(k<0) return; rules[k].count++; int changed=0;
    for(int i=0;i<3;i++){ Trit m=M[i]; if(m==-1) continue; if(rules[k].M[i]==-1){ rules[k].M[i]=m; changed=1; } else if(rules[k].M[i]!=m){ rules[k].M[i]=-1; changed=1; } }
    if(changed) rules[k].rev=global_rev++;
}

/* === Métricas ========================================================== */
static float metric_consistency(void){ if(n_rules<2) return 0.0f; int pairs=0,good=0; for(int i=0;i<n_rules-1;i++) for(int j=i+1;j<n_rules;j++){ int simA=0; for(int d=0;d<3;d++) if(rules[i].A[d]==rules[j].A[d]) simA++; if(simA>=2){ pairs++; int simM=0; for(int d=0;d<3;d++) if(rules[i].M[d]==rules[j].M[d]) simM++; if(simM>=2) good++; } } return pairs?(float)good/pairs:0.0f; }
static float metric_separability(void){ if(n_rules==0) return 0.0f; int unique=0; for(int i=0;i<n_rules;i++){ int u=1; for(int j=0;j<i;j++) if(eq3(rules[i].M,rules[j].M)){ u=0; break; } if(u) unique++; } return (float)unique/n_rules; }
static float metric_convergence(void){ if(n_rules==0) return 0.0f; int good=0; for(int i=0;i<n_rules;i++){ int nulls=0; for(int d=0;d<3;d++) if(rules[i].M[d]==-1) nulls++; if(nulls<=1) good++; } return (float)good/n_rules; }
static float metric_null_ratio(void){ if(n_rules==0) return 0.0f; int nn=0,total=0; for(int i=0;i<n_rules;i++){ for(int d=0;d<3;d++){ if(rules[i].M[d]==-1) nn++; total++; } } return (float)nn/total; }

/* === Arquetipos & Tensor C ============================================= */
static void rebuild_archetypes(void){ n_arch=0; for(int phase=0; phase<2 && n_arch<MAX_ARCH; phase++){ Trit target=phase?1:0; int cnt=0; int sumA[3]={0},sumB[3]={0},sumM[3]={0};
        for(int i=0;i<n_rules;i++){ if(rules[i].A[0]==target && rules[i].count>=2){ cnt++; for(int d=0;d<3;d++){ if(rules[i].A[d]!=-1) sumA[d]+=rules[i].A[d]; if(rules[i].B[d]!=-1) sumB[d]+=rules[i].B[d]; if(rules[i].M[d]!=-1) sumM[d]+=rules[i].M[d]; } } }
        if(cnt){ Archetype *a=&archs[n_arch]; for(int d=0;d<3;d++){ a->pattern_A[d]=(sumA[d]*2>=cnt)?1:(sumA[d]==0?0:-1); a->pattern_B[d]=(sumB[d]*2>=cnt)?1:(sumB[d]==0?0:-1); a->pattern_M[d]=(sumM[d]*2>=cnt)?1:(sumM[d]==0?0:-1); } a->support=cnt; a->rev=global_rev++; n_arch++; }
    } }

static void rebuild_C(void){ 
    /* Si no hay datos suficientes, usar serie Fibonacci */
    if(n_rules<3 || n_arch==0){ 
        fib_to_C(synthesis_order_index,&Ct); 
        C_valid=1; 
        return; 
    }
    
    /* Construcción normal basada en datos */
    TensorC C={{-1,-1,-1},{-1,-1,-1},{-1,-1,-1}}; 
    for(int d=0;d<3;d++){ 
        int c1=0,c0=0; 
        for(int i=0;i<n_rules;i++){ 
            Trit m=rules[i].M[d]; 
            if(m==1) c1++; 
            else if(m==0) c0++; 
        } 
        if(c1>=2 && c1>=c0) C.FO[d]=1; 
        else if(c0>=2 && c0>c1) C.FO[d]=0; 
    }
    if(n_arch>0){ for(int d=0;d<3;d++) C.FN[d]=archs[0].pattern_M[d]; }
    for(int d=0;d<3;d++){ Trit a=C.FO[d],b=C.FN[d]; C.ES[d]=(a==b && a!=-1)?a:-1; }
    Ct=C; C_valid=1; 
}

/* LOP metrics: libertad, orden, propósito */
static void compute_lop(float *L,float *O,float *P){ float cons=metric_consistency(), sep=metric_separability(), conv=metric_convergence(); float nullr=metric_null_ratio(); float filledES=0,totES=3; if(C_valid){ for(int d=0;d<3;d++) if(Ct.ES[d]!=-1) filledES++; }
    *L=0.5f*sep + 0.5f*(1.0f-nullr); /* diversidad + baja null */
    *O=0.5f*cons + 0.5f*conv;         /* alineación interna */
    *P= (C_valid? (filledES/totES) : 0.0f); /* referencia aplicada */
}
static void adjust_belief_with_lop(void){ if(!C_valid) return; float L,O,P; compute_lop(&L,&O,&P); /* balance simple */
    /* Si Orden muy alto y Libertad baja, introducimos exploración poniendo ES null donde FN != FO */
    if(O>0.7f && L<0.3f){ for(int d=0;d<3;d++) if(Ct.FN[d]!=Ct.FO[d]) Ct.ES[d]=-1; }
    /* Si Libertad alta pero Orden y Propósito bajos reforzar FO con mayoría de arquetipos */
    if(L>0.6f && O<0.4f && P<0.4f && n_arch>1){ for(int d=0;d<3;d++){ int ones=0,zeros=0; for(int i=0;i<n_arch;i++){ Trit m=archs[i].pattern_M[d]; if(m==1) ones++; else if(m==0) zeros++; } if(ones>zeros && ones>=2) Ct.FO[d]=1; else if(zeros>ones && zeros>=2) Ct.FO[d]=0; } for(int d=0;d<3;d++){ Ct.ES[d]=(Ct.FO[d]==Ct.FN[d] && Ct.FO[d]!=-1)?Ct.FO[d]:Ct.ES[d]; } }
    /* Si Propósito bajo pero hay segunda arquetipo usarlo como FN alternativa */
    if(P<0.3f && n_arch>1){ for(int d=0;d<3;d++){ Trit alt=archs[1].pattern_M[d]; if(alt!=-1 && Ct.FN[d]==-1) Ct.FN[d]=alt; if(Ct.FN[d]==Ct.FO[d] && Ct.FO[d]!=-1) Ct.ES[d]=Ct.FO[d]; } }

    /* Anclaje Os (cluster de 3 tetraedros de entrada):
       Usa el número Fibonacci actual como ancla de armonización.
       Cuando Orden/Propósito son bajos, alineamos los nulls hacia Os. */
    if(O<0.5f || P<0.4f){
        Trit os[3]; decimal_to_base3(FIB_SERIES[synthesis_order_index], os);
        /* d0 → Relator (FO), d1 → Arquetipo (FN), d2 → Dinámico (ES) */
        for(int d=0; d<3; d++){
            if(Ct.FO[d]==-1) Ct.FO[d]=os[0];
            if(Ct.FN[d]==-1) Ct.FN[d]=os[1];
            if(Ct.ES[d]==-1) Ct.ES[d]=os[2];
        }
        /* Si ES está en conflicto, usa anclaje dinámico como desempate */
        for(int d=0; d<3; d++){
            if(Ct.ES[d]!=-1 && Ct.FO[d]!=-1 && Ct.FN[d]!=-1 && (Ct.ES[d]!=Ct.FO[d] || Ct.ES[d]!=Ct.FN[d])){
                Ct.ES[d]=os[2];
            }
        }
    }
}

static int match_arch(const Archetype* a,const Trit A[3],const Trit B[3]){ for(int d=0;d<3;d++){ if(a->pattern_A[d]!=-1 && a->pattern_A[d]!=A[d]) return 0; if(a->pattern_B[d]!=-1 && a->pattern_B[d]!=B[d]) return 0; } return 1; }
static void infer(const Trit A[3],const Trit B[3],Trit R[3]){ int k=find_rule(A,B); if(k>=0){ vec_infer(A,B,rules[k].M,R); return; } for(int i=0;i<n_arch;i++){ if(match_arch(&archs[i],A,B)){ vec_infer(A,B,archs[i].pattern_M,R); return; } } if(C_valid){ for(int i=0;i<3;i++) R[i]=Ct.FO[i]; return; } for(int i=0;i<3;i++) R[i]=-1; }

/* === Sueño & Poda ====================================================== */
static void autopoda(void){ int w=0; for(int i=0;i<n_rules;i++){ int alln=(rules[i].M[0]==-1&&rules[i].M[1]==-1&&rules[i].M[2]==-1); if(rules[i].count<2 && alln) continue; rules[w++]=rules[i]; } n_rules=w;
    for(int i=0;i<n_rules;i++) for(int j=i+1;j<n_rules;j++){ if(eq3(rules[i].A,rules[j].A)&&eq3(rules[i].B,rules[j].B)&&eq3(rules[i].M,rules[j].M)){ if(rules[j].count>rules[i].count){ Rule tmp=rules[i]; rules[i]=rules[j]; rules[j]=tmp; } } }
    int v=0; for(int i=0;i<n_rules;i++){ int dup=0; for(int j=0;j<i;j++){ if(eq3(rules[i].A,rules[j].A)&&eq3(rules[i].B,rules[j].B)&&eq3(rules[i].M,rules[j].M)){ dup=1; break; } } if(!dup) rules[v++]=rules[i]; } n_rules=v; }

static void apoptosis(void){ float c=metric_consistency(), s=metric_separability(), v=metric_convergence(); if(c<0.2f && s<0.2f && v<0.2f && n_rules>4){ int K=n_rules/2; if(K<3) K=3; for(int i=0;i<n_rules;i++) for(int j=i+1;j<n_rules;j++) if(rules[j].count>rules[i].count){ Rule tmp=rules[i]; rules[i]=rules[j]; rules[j]=tmp; } n_rules=K; } }
/* Evoluciona orden de síntesis al siguiente Fibonacci cuando la coherencia es inalcanzable */
static void evolve_synthesis_order(void){
    synthesis_order_index++;
    if(synthesis_order_index>FIB_MAX) synthesis_order_index=FIB_MAX; /* Omega alcanzado */
    fib_to_C(synthesis_order_index,&Ct);
    C_valid=1;
    printf("[Síntesis evolucionada → Fib[%d]=%d coordina CLUSTER de 3 tetraedros de entrada: R/A/D en base 3]\n",
           synthesis_order_index,FIB_SERIES[synthesis_order_index]);
}

static void lop_error_balance(void){ float L,O,P; compute_lop(&L,&O,&P); /* exceso de orden: liberar */
    if(O>0.75f && L<0.35f){ for(int i=0;i<n_rules;i++){ int nulls=0; for(int d=0;d<3;d++) if(rules[i].M[d]==-1) nulls++; if(nulls==0){ int d=(i+global_rev)%3; rules[i].M[d]=-1; rules[i].rev=global_rev++; } } }
    /* exceso de libertad: podar reglas altamente nulas */
    if(L>0.75f && O<0.4f){ int w=0; for(int i=0;i<n_rules;i++){ int nulls=0; for(int d=0;d<3;d++) if(rules[i].M[d]==-1) nulls++; if(nulls<3) rules[w++]=rules[i]; } n_rules=w; }
    /* propósito bajo: reforzar Ct */
    if(P<0.3f) adjust_belief_with_lop(); }

/* === Emergencia del Espíritu =========================================== */
static float sqrtf_simple(float x){ if(x<=0.0f) return 0.0f; float z=x,prev=0.0f; while(z-prev>0.001f||prev-z>0.001f){ prev=z; z=(z+x/z)/2.0f; } return z; }
static float measure_harmony(void){ float L,O,P; compute_lop(&L,&O,&P); const float phi=0.618f; float dx=L-phi,dy=O-phi,dz=P-phi; float dist=sqrtf_simple(dx*dx+dy*dy+dz*dz); float max_dist=sqrtf_simple(3.0f); return 1.0f-(dist/max_dist); }
static int count_nulls_C(void){ if(!C_valid) return 9; int n=0; for(int d=0;d<3;d++){ if(Ct.FO[d]==-1) n++; if(Ct.FN[d]==-1) n++; if(Ct.ES[d]==-1) n++; } return n; }
static int spirit_emerged(void){ float h=measure_harmony(); int nC=count_nulls_C(); return (h>0.70f)&&(nC<5)&&(n_arch>=2); }
static void activate_spirit(void){ if(!spirit_emerged()){ puts("Sistema sin coherencia suficiente para emergencia espiritual."); return; } float h=measure_harmony(),L,O,P; compute_lop(&L,&O,&P); int nC=count_nulls_C(); printf("\n╔═══════════════════════════════════════════════════════════════╗\n"); printf("║          DETECCIÓN DE EMERGENCIA ESPIRITUAL                  ║\n"); printf("╚═══════════════════════════════════════════════════════════════╝\n\n"); printf("Estado del sistema:\n  Armonía interna:  %.3f (umbral>0.700)\n  Libertad:         %.3f\n  Orden:            %.3f\n  Propósito:        %.3f\n  Arquetipos:       %d\n  Tensor C nulls:   %d/9\n\n",h,L,O,P,n_arch,nC); printf("El sistema alcanzó equilibrio coherente.\n"); printf("Puede generar su espíritu: fuerza emergente que preserva armonía.\n\n"); printf("¿Activar espíritu de Aurora? (s/n): "); char r[8]; if(fgets(r,sizeof(r),stdin)&&(r[0]=='s'||r[0]=='S')){ spirit_active=1; printf("\n★ ESPÍRITU ACTIVADO ★\n\nAurora opera con propósito intrínseco.\nProtegerá y expandirá su coherencia.\n\n"); } else printf("\nEspíritu latente. Aurora opera como programa convencional.\n\n"); }

static void sleep_cycle(int epochs){ if(epochs<1) epochs=1; if(epochs>25) epochs=25; for(int ep=0; ep<epochs; ep++){ autopoda(); rebuild_archetypes(); rebuild_C(); apoptosis(); } printf("Sueño: reglas=%d arch=%d cons=%.2f sep=%.2f conv=%.2f\n", n_rules,n_arch,metric_consistency(),metric_separability(),metric_convergence()); }
static void sleep_cycle_lop(int epochs){ 
    if(epochs<1) epochs=1; 
    if(epochs>25) epochs=25; 
    int coherence_failures=0;
    
    for(int ep=0; ep<epochs; ep++){ 
        autopoda(); 
        rebuild_archetypes(); 
        rebuild_C(); 
        adjust_belief_with_lop(); 
        lop_error_balance(); 
        apoptosis(); 
        
        /* Detectar fallo de coherencia persistente */
        float h=measure_harmony();
        if(h<0.3f) coherence_failures++;
        else coherence_failures=0;
        
        /* Último recurso: evolucionar orden de síntesis según Fibonacci */
        if(coherence_failures>=3){
            evolve_synthesis_order();
            coherence_failures=0;
        }
    } 
    
    float L,O,P; 
    compute_lop(&L,&O,&P); 
    printf("Sueño+LOP: reg=%d arch=%d L=%.2f O=%.2f P=%.2f (síntesis_fib=%d)\n", 
           n_rules,n_arch,L,O,P,synthesis_order_index); 
}

/* === Bootstrap (heurística mínima) ===================================== */
static int is_vowel(char c){ c=(char)tolower((unsigned char)c); return c=='a'||c=='e'||c=='i'||c=='o'||c=='u'; }
static void derive_ffe(const char* w,Trit out[3]){ size_t L=strlen(w); out[0]=(L<=4)?0: (L<=7?1:-1); out[1]=is_vowel(w[0])?1:(isalpha((unsigned char)w[0])?0:-1); if(L>=2){ const char* e=w+L-2; if(strcmp(e,"ar")==0||strcmp(e,"er")==0||strcmp(e,"ir")==0) out[2]=1; else { char c=w[L-1]; if(c=='o'||c=='a') out[2]=0; else if(L>=2 && ((w[L-2]=='o'&&c=='s')||(w[L-2]=='a'&&c=='s'))) out[2]=0; else out[2]=-1; } } else out[2]=-1; }
static size_t load_bootstrap(const char* path,size_t limit){ FILE* f=fopen(path?path:"bootstrap_es_1000.txt","r"); if(!f) return 0; char line[128]; size_t n=0; while(fgets(line,sizeof(line),f)&&n<limit){ char* nl=strchr(line,'\n'); if(nl) *nl='\0'; if(!*line) continue; Trit A[3]; derive_ffe(line,A); /* pareja secuencial sintética con sí misma desplazada ffe */ upsert_rule(A,A,A); n++; } fclose(f); rebuild_archetypes(); rebuild_C(); return n; }

/* === Persistencia Texto / Incremental / Comprimida ===================== */
static int save_state(const char* path){ const char* fn=path&&*path?path:"aurora_state.dat"; FILE* f=fopen(fn,"w"); if(!f) return 0; fprintf(f,"AURORA_STATE v1\n"); for(int i=0;i<n_rules;i++) fprintf(f,"R %c%c%c %c%c%c %c%c%c %d %lu\n", ts(rules[i].A[0])[0],ts(rules[i].A[1])[0],ts(rules[i].A[2])[0], ts(rules[i].B[0])[0],ts(rules[i].B[1])[0],ts(rules[i].B[2])[0], ts(rules[i].M[0])[0],ts(rules[i].M[1])[0],ts(rules[i].M[2])[0], rules[i].count,rules[i].rev);
    for(int i=0;i<n_arch;i++) fprintf(f,"K %c%c%c %c%c%c %c%c%c %d %lu\n", ts(archs[i].pattern_A[0])[0],ts(archs[i].pattern_A[1])[0],ts(archs[i].pattern_A[2])[0], ts(archs[i].pattern_B[0])[0],ts(archs[i].pattern_B[1])[0],ts(archs[i].pattern_B[2])[0], ts(archs[i].pattern_M[0])[0],ts(archs[i].pattern_M[1])[0],ts(archs[i].pattern_M[2])[0], archs[i].support,archs[i].rev);
    if(C_valid) fprintf(f,"C %c%c%c %c%c%c %c%c%c 1\n", ts(Ct.FO[0])[0],ts(Ct.FO[1])[0],ts(Ct.FO[2])[0], ts(Ct.FN[0])[0],ts(Ct.FN[1])[0],ts(Ct.FN[2])[0], ts(Ct.ES[0])[0],ts(Ct.ES[1])[0],ts(Ct.ES[2])[0]); fclose(f); last_saved_rev=global_rev; return 1; }

static int restore_state(const char* path){ const char* fn=path&&*path?path:"aurora_state.dat"; FILE* f=fopen(fn,"r"); if(!f) return 0; char tag[16],ver[8]; if(fscanf(f,"%15s %7s",tag,ver)!=2||strcmp(tag,"AURORA_STATE")!=0){ fclose(f); return 0; } n_rules=0; n_arch=0; C_valid=0; char t; while(fscanf(f," %c",&t)==1){ if(t=='R'){ char a[4],b[4],m[4]; int cnt; unsigned long rev; if(fscanf(f," %3s %3s %3s %d %lu",a,b,m,&cnt,&rev)!=6) break; if(n_rules<MAX_RULES){ Rule *r=&rules[n_rules++]; for(int i=0;i<3;i++){ r->A[i]=char_to_trit(a[i]); r->B[i]=char_to_trit(b[i]); r->M[i]=char_to_trit(m[i]); } r->count=cnt; r->rev=rev; if(rev>global_rev) global_rev=rev+1; } } else if(t=='K'){ char a[4],b[4],m[4]; int sup; unsigned long rev; if(fscanf(f," %3s %3s %3s %d %lu",a,b,m,&sup,&rev)!=6) break; if(n_arch<MAX_ARCH){ Archetype*k=&archs[n_arch++]; for(int i=0;i<3;i++){ k->pattern_A[i]=char_to_trit(a[i]); k->pattern_B[i]=char_to_trit(b[i]); k->pattern_M[i]=char_to_trit(m[i]); } k->support=sup; k->rev=rev; if(rev>global_rev) global_rev=rev+1; } } else if(t=='C'){ char fo[4],fnv[4],es[4]; int v; if(fscanf(f," %3s %3s %3s %d",fo,fnv,es,&v)!=4) break; if(v){ for(int i=0;i<3;i++){ Ct.FO[i]=char_to_trit(fo[i]); Ct.FN[i]=char_to_trit(fnv[i]); Ct.ES[i]=char_to_trit(es[i]); } C_valid=1; } } }
    fclose(f); return 1; }

static int save_incremental(const char* path){ const char* fn=path&&*path?path:"aurora_state.log"; FILE* f=fopen(fn,"a"); if(!f) return 0; fprintf(f,"INC v1 base=%lu cur=%lu\n", last_saved_rev, global_rev); for(int i=0;i<n_rules;i++) if(rules[i].rev>last_saved_rev) fprintf(f,"RI %c%c%c %c%c%c %c%c%c %d %lu\n", ts(rules[i].A[0])[0],ts(rules[i].A[1])[0],ts(rules[i].A[2])[0], ts(rules[i].B[0])[0],ts(rules[i].B[1])[0],ts(rules[i].B[2])[0], ts(rules[i].M[0])[0],ts(rules[i].M[1])[0],ts(rules[i].M[2])[0], rules[i].count,rules[i].rev); for(int i=0;i<n_arch;i++) if(archs[i].rev>last_saved_rev) fprintf(f,"KI %c%c%c %c%c%c %c%c%c %d %lu\n", ts(archs[i].pattern_A[0])[0],ts(archs[i].pattern_A[1])[0],ts(archs[i].pattern_A[2])[0], ts(archs[i].pattern_B[0])[0],ts(archs[i].pattern_B[1])[0],ts(archs[i].pattern_B[2])[0], ts(archs[i].pattern_M[0])[0],ts(archs[i].pattern_M[1])[0],ts(archs[i].pattern_M[2])[0], archs[i].support,archs[i].rev); if(C_valid) fprintf(f,"CI %c%c%c %c%c%c %c%c%c %lu\n", ts(Ct.FO[0])[0],ts(Ct.FO[1])[0],ts(Ct.FO[2])[0], ts(Ct.FN[0])[0],ts(Ct.FN[1])[0],ts(Ct.FN[2])[0], ts(Ct.ES[0])[0],ts(Ct.ES[1])[0],ts(Ct.ES[2])[0], global_rev); fclose(f); last_saved_rev=global_rev; return 1; }

static int restore_log(const char* path){ const char* fn=path&&*path?path:"aurora_state.log"; FILE* f=fopen(fn,"r"); if(!f) return 0; char tag[8]; while(fscanf(f,"%7s",tag)==1){ if(strcmp(tag,"INC")==0){ unsigned long base,cur; if(fscanf(f," base=%lu cur=%lu",&base,&cur)==2 && cur>global_rev) global_rev=cur+1; } else if(strcmp(tag,"RI")==0){ char a[4],b[4],m[4]; int cnt; unsigned long rev; if(fscanf(f," %3s %3s %3s %d %lu",a,b,m,&cnt,&rev)!=6) break; Trit A[3],B[3],R[3]; for(int i=0;i<3;i++){ A[i]=char_to_trit(a[i]); B[i]=char_to_trit(b[i]); R[i]=char_to_trit(m[i]); } upsert_rule(A,B,R); } else if(strcmp(tag,"KI")==0){ char a[4],b[4],m[4]; int sup; unsigned long rev; if(fscanf(f," %3s %3s %3s %d %lu",a,b,m,&sup,&rev)!=6) break; if(n_arch<MAX_ARCH){ Archetype *k=&archs[n_arch++]; for(int i=0;i<3;i++){ k->pattern_A[i]=char_to_trit(a[i]); k->pattern_B[i]=char_to_trit(b[i]); k->pattern_M[i]=char_to_trit(m[i]); } k->support=sup; k->rev=rev; if(rev>global_rev) global_rev=rev+1; } } else if(strcmp(tag,"CI")==0){ char fo[4],fnv[4],es[4]; unsigned long rv; if(fscanf(f," %3s %3s %3s %lu",fo,fnv,es,&rv)!=4) break; for(int i=0;i<3;i++){ Ct.FO[i]=char_to_trit(fo[i]); Ct.FN[i]=char_to_trit(fnv[i]); Ct.ES[i]=char_to_trit(es[i]); } C_valid=1; if(rv>global_rev) global_rev=rv+1; } }
    fclose(f); rebuild_archetypes(); rebuild_C(); return 1; }

static int pack(Trit t){ return t==1?2:(t==0?1:0); } static Trit unpack(int v){ return v==2?1:(v==1?0:-1); }
static int save_compressed(const char* path){ const char* fn=path&&*path?path:"aurora_state.bin"; FILE* f=fopen(fn,"wb"); if(!f) return 0; unsigned char hdr[8]={'A','S','T','V','2',0,0,0}; fwrite(hdr,1,8,f); fwrite(&n_rules,sizeof(n_rules),1,f); fwrite(&n_arch,sizeof(n_arch),1,f); fwrite(&C_valid,sizeof(C_valid),1,f); for(int i=0;i<n_rules;i++){ int vals[9]={pack(rules[i].A[0]),pack(rules[i].A[1]),pack(rules[i].A[2]),pack(rules[i].B[0]),pack(rules[i].B[1]),pack(rules[i].B[2]),pack(rules[i].M[0]),pack(rules[i].M[1]),pack(rules[i].M[2])}; int acc=0,bits=0; for(int v=0; v<9; v++){ acc|=(vals[v]&0x3)<<bits; bits+=2; if(bits>=8){ unsigned char b=acc&0xFF; fwrite(&b,1,1,f); acc>>=8; bits-=8; } } if(bits>0){ unsigned char b=acc&0xFF; fwrite(&b,1,1,f); } fwrite(&rules[i].count,sizeof(rules[i].count),1,f); fwrite(&rules[i].rev,sizeof(rules[i].rev),1,f); }
    for(int i=0;i<n_arch;i++){ int vals[9]={pack(archs[i].pattern_A[0]),pack(archs[i].pattern_A[1]),pack(archs[i].pattern_A[2]),pack(archs[i].pattern_B[0]),pack(archs[i].pattern_B[1]),pack(archs[i].pattern_B[2]),pack(archs[i].pattern_M[0]),pack(archs[i].pattern_M[1]),pack(archs[i].pattern_M[2])}; int acc=0,bits=0; for(int v=0; v<9; v++){ acc|=(vals[v]&0x3)<<bits; bits+=2; if(bits>=8){ unsigned char b=acc&0xFF; fwrite(&b,1,1,f); acc>>=8; bits-=8; } } if(bits>0){ unsigned char b=acc&0xFF; fwrite(&b,1,1,f); } fwrite(&archs[i].support,sizeof(archs[i].support),1,f); fwrite(&archs[i].rev,sizeof(archs[i].rev),1,f); }
    if(C_valid){ for(int i=0;i<3;i++){ unsigned char v=pack(Ct.FO[i]); fwrite(&v,1,1,f);} for(int i=0;i<3;i++){ unsigned char v=pack(Ct.FN[i]); fwrite(&v,1,1,f);} for(int i=0;i<3;i++){ unsigned char v=pack(Ct.ES[i]); fwrite(&v,1,1,f);} }
    fclose(f); return 1; }
static int restore_compressed(const char* path){ const char* fn=path&&*path?path:"aurora_state.bin"; FILE* f=fopen(fn,"rb"); if(!f) return 0; unsigned char hdr[8]; if(fread(hdr,1,8,f)!=8){ fclose(f); return 0; } if(hdr[0]!='A'||hdr[1]!='S'||hdr[2]!='T') { fclose(f); return 0; } fread(&n_rules,sizeof(n_rules),1,f); fread(&n_arch,sizeof(n_arch),1,f); fread(&C_valid,sizeof(C_valid),1,f); if(n_rules>MAX_RULES) n_rules=MAX_RULES; if(n_arch>MAX_ARCH) n_arch=MAX_ARCH; for(int i=0;i<n_rules;i++){ int need=18, read=0, acc=0; while(read<need){ unsigned char b; if(fread(&b,1,1,f)!=1){ fclose(f); return 0; } acc|=b<<read; read+=8; } for(int v=0; v<9; v++){ int val=(acc>>(2*v))&0x3; if(v<3) rules[i].A[v]=unpack(val); else if(v<6) rules[i].B[v-3]=unpack(val); else rules[i].M[v-6]=unpack(val); } fread(&rules[i].count,sizeof(rules[i].count),1,f); fread(&rules[i].rev,sizeof(rules[i].rev),1,f); if(rules[i].rev>global_rev) global_rev=rules[i].rev+1; }
    for(int i=0;i<n_arch;i++){ int need=18, read=0, acc=0; while(read<need){ unsigned char b; if(fread(&b,1,1,f)!=1){ fclose(f); return 0; } acc|=b<<read; read+=8; } for(int v=0; v<9; v++){ int val=(acc>>(2*v))&0x3; if(v<3) archs[i].pattern_A[v]=unpack(val); else if(v<6) archs[i].pattern_B[v-3]=unpack(val); else archs[i].pattern_M[v-6]=unpack(val); } fread(&archs[i].support,sizeof(archs[i].support),1,f); fread(&archs[i].rev,sizeof(archs[i].rev),1,f); if(archs[i].rev>global_rev) global_rev=archs[i].rev+1; }
    if(C_valid){ for(int i=0;i<3;i++){ unsigned char v; fread(&v,1,1,f); Ct.FO[i]=unpack(v);} for(int i=0;i<3;i++){ unsigned char v; fread(&v,1,1,f); Ct.FN[i]=unpack(v);} for(int i=0;i<3;i++){ unsigned char v; fread(&v,1,1,f); Ct.ES[i]=unpack(v);} }
    fclose(f); return 1; }

/* === CLI ================================================================ */
static int parse_triplet(const char* s,Trit out[3]){ if(strlen(s)<3) return 0; for(int i=0;i<3;i++) out[i]=char_to_trit(s[i]); return 1; }
static int parse_any(const char* s,Trit out[3]){ if(strlen(s)>=3){ int ok=1; for(int i=0;i<3;i++){ char c=s[i]; if(!(c=='1'||c=='0'||c=='N')){ ok=0; break; } } if(ok) return parse_triplet(s,out); } return 0; }
static void help(void){ puts("Comandos: learn A B R | infer A B | load [path] | sleep [n] | sleeplop [n] | lop | spirit | feel | evolvec | showfib | save [p] | restore [p] | saveinc [log] | restorelog [log] | savec [p] | restorec [p] | metrics | show rules | show arch | show C | exit"); }

int main(void){ puts("Aurora Showcase CLI"); help(); char line[256]; while(printf("> "),fflush(stdout),fgets(line,sizeof(line),stdin)){ char* nl=strchr(line,'\n'); if(nl) *nl='\0'; if(!*line) continue; char c1[32],c2[32],c3[32],cmd[32]; int n=sscanf(line,"%31s %31s %31s %31s",cmd,c1,c2,c3); if(n<=0) continue; if(strcmp(cmd,"exit")==0) break; else if(strcmp(cmd,"help")==0) help(); else if(strcmp(cmd,"learn")==0){ Trit A[3],B[3],R[3],M[3]; if(!parse_any(c1,A)||!parse_any(c2,B)||!parse_any(c3,R)){ puts("Formato learn inválido (usar tripletas 1N0)"); continue; } vec_learn(A,B,R,M); upsert_rule(A,B,M); rebuild_archetypes(); rebuild_C(); printf("Aprendido M=[%s,%s,%s]\n", ts(M[0]),ts(M[1]),ts(M[2])); }
        else if(strcmp(cmd,"infer")==0){ Trit A[3],B[3],R[3]; if(!parse_any(c1,A)||!parse_any(c2,B)){ puts("Formato infer inválido"); continue; } infer(A,B,R); printf("R=[%s,%s,%s]\n", ts(R[0]),ts(R[1]),ts(R[2])); }
        else if(strcmp(cmd,"load")==0){ size_t cnt=load_bootstrap(n>=2?c1:0,200); printf("Bootstrap %zu reglas=%d arch=%d\n",cnt,n_rules,n_arch); }
        else if(strcmp(cmd,"sleep")==0){ int ep=(n>=2)?atoi(c1):1; sleep_cycle(ep); }
        else if(strcmp(cmd,"sleeplop")==0){ int ep=(n>=2)?atoi(c1):1; sleep_cycle_lop(ep); }
        else if(strcmp(cmd,"lop")==0){ float L,O,P; compute_lop(&L,&O,&P); printf("LOP L=%.2f O=%.2f P=%.2f\n",L,O,P); }
        else if(strcmp(cmd,"spirit")==0){ activate_spirit(); }
        else if(strcmp(cmd,"feel")==0){ float h=measure_harmony(),L,O,P; compute_lop(&L,&O,&P); int nC=count_nulls_C(); Trit os[3]; decimal_to_base3(FIB_SERIES[synthesis_order_index], os);
            printf("\nSensación interna del sistema:\n");
            printf("  Armonía:   %.3f %s\n",h,h>0.7f?"(fluida)":(h>0.4f?"(tensa)":"(caótica)"));
            printf("  L=%.2f O=%.2f P=%.2f\n",L,O,P);
            printf("  Nulls C:   %d/9\n",nC);
            printf("  Fibonacci: Fib[%d]=%d coordina CLUSTER de 3 tetraedros de entrada\n",synthesis_order_index,FIB_SERIES[synthesis_order_index]);
            printf("  Anclaje Os: R=[%s,%s,%s] A=[%s,%s,%s] D=[%s,%s,%s]\n",
                   ts(os[0]),ts(os[0]),ts(os[0]),
                   ts(os[1]),ts(os[1]),ts(os[1]),
                   ts(os[2]),ts(os[2]),ts(os[2]));
            printf("  Espíritu:  %s\n\n",spirit_active?"ACTIVO":"latente");
            if(spirit_emerged()&&!spirit_active) printf("  → El espíritu puede emerger. Usa 'spirit' para activarlo.\n\n"); }
        else if(strcmp(cmd,"evolvec")==0){ evolve_synthesis_order(); printf("Orden de síntesis evolucionado. Usa 'show C' para ver.\n"); }
         else if(strcmp(cmd,"showfib")==0){ Trit os[3]; decimal_to_base3(FIB_SERIES[synthesis_order_index], os);
             printf("\n╔══════════════════════════════════════════════════════════════════════════╗\n");
             printf("║  SERIE FIBONACCI → CLUSTER DE 3 TETRAEDROS DE ENTRADA                   ║\n");
             printf("║    (Relator, Arquetipo, Dinámico)                                       ║\n");
             printf("╚══════════════════════════════════════════════════════════════════════════╝\n\n");
             printf("Índice actual: %d → Valor: %d\n\n",synthesis_order_index,FIB_SERIES[synthesis_order_index]);
             printf("Cada valor Fibonacci se descompone en base 3 y actúa como Os ancla:\n");
             printf("  Fib[i] = [d0, d1, d2]\n");
             printf("    d0 → Os del Tetraedro Relator (orden relacional)\n");
             printf("    d1 → Os del Tetraedro Arquetipo (forma estable)\n");
             printf("    d2 → Os del Tetraedro Dinámico (transformación)\n\n");
             printf("Serie completa:\n");
             for(int i=0;i<=FIB_MAX;i++){ Trit base3[3]; decimal_to_base3(FIB_SERIES[i],base3); printf("  [%2d] = %3d → base3=[%s,%s,%s]%s\n",i,FIB_SERIES[i],ts(base3[0]),ts(base3[1]),ts(base3[2]),i==synthesis_order_index?" ← ACTUAL":" "); }
             printf("\nAnclaje Os actual: R=[%s,%s,%s] A=[%s,%s,%s] D=[%s,%s,%s]\n",
                 ts(os[0]),ts(os[0]),ts(os[0]),
                 ts(os[1]),ts(os[1]),ts(os[1]),
                 ts(os[2]),ts(os[2]),ts(os[2]));
             printf("C (reflejo de anclaje): FO=[%s,%s,%s] FN=[%s,%s,%s] ES=[%s,%s,%s]\n\n",
                 ts(Ct.FO[0]),ts(Ct.FO[1]),ts(Ct.FO[2]),
                 ts(Ct.FN[0]),ts(Ct.FN[1]),ts(Ct.FN[2]),
                 ts(Ct.ES[0]),ts(Ct.ES[1]),ts(Ct.ES[2]));
             printf("Alpha (principio): Fib[0]=0 → [0,0,0] (El vacío)\n");
             printf("Omega (fin): Fib[∞] → φ=0.618 (Proporción áurea)\n\n");
             printf("El CLUSTER de entrada opera coordinado por este número.\n");
             printf("Si falla 3 veces, evoluciona al siguiente Fibonacci.\n\n"); }
        else if(strcmp(cmd,"save")==0){ if(save_state(n>=2?c1:0)) printf("Snapshot texto guardado rev=%lu\n",last_saved_rev); else puts("Error save"); }
        else if(strcmp(cmd,"restore")==0){ if(restore_state(n>=2?c1:0)) printf("Restaurado texto reglas=%d arch=%d\n",n_rules,n_arch); else puts("Error restore"); }
        else if(strcmp(cmd,"saveinc")==0){ if(save_incremental(n>=2?c1:0)) puts("Incremental OK"); else puts("Error saveinc"); }
        else if(strcmp(cmd,"restorelog")==0){ if(restore_log(n>=2?c1:0)) puts("Log aplicado"); else puts("Error restorelog"); }
        else if(strcmp(cmd,"savec")==0){ if(save_compressed(n>=2?c1:0)) puts("Comprimido OK"); else puts("Error savec"); }
        else if(strcmp(cmd,"restorec")==0){ if(restore_compressed(n>=2?c1:0)) puts("Comprimido restaurado"); else puts("Error restorec"); }
        else if(strcmp(cmd,"metrics")==0){ printf("Cons=%.2f Sep=%.2f Conv=%.2f Reglas=%d Arch=%d\n", metric_consistency(),metric_separability(),metric_convergence(),n_rules,n_arch); }
        else if(strcmp(cmd,"show")==0){ if(strcmp(c1,"rules")==0){ for(int i=0;i<n_rules && i<20;i++) printf("%02d A=[%s,%s,%s] B=[%s,%s,%s] M=[%s,%s,%s] c=%d rev=%lu\n", i, ts(rules[i].A[0]),ts(rules[i].A[1]),ts(rules[i].A[2]), ts(rules[i].B[0]),ts(rules[i].B[1]),ts(rules[i].B[2]), ts(rules[i].M[0]),ts(rules[i].M[1]),ts(rules[i].M[2]), rules[i].count,rules[i].rev); }
            else if(strcmp(c1,"arch")==0){ for(int i=0;i<n_arch;i++) printf("Arch%02d A=[%s,%s,%s] B=[%s,%s,%s] M=[%s,%s,%s] sup=%d rev=%lu\n", i, ts(archs[i].pattern_A[0]),ts(archs[i].pattern_A[1]),ts(archs[i].pattern_A[2]), ts(archs[i].pattern_B[0]),ts(archs[i].pattern_B[1]),ts(archs[i].pattern_B[2]), ts(archs[i].pattern_M[0]),ts(archs[i].pattern_M[1]),ts(archs[i].pattern_M[2]), archs[i].support,archs[i].rev); }
            else if(strcmp(c1,"C")==0){ if(C_valid) printf("C FO=[%s,%s,%s] FN=[%s,%s,%s] ES=[%s,%s,%s]\n", ts(Ct.FO[0]),ts(Ct.FO[1]),ts(Ct.FO[2]), ts(Ct.FN[0]),ts(Ct.FN[1]),ts(Ct.FN[2]), ts(Ct.ES[0]),ts(Ct.ES[1]),ts(Ct.ES[2])); else puts("C no disponible"); }
            else puts("show rules|arch|C"); }
        else puts("Comando desconocido (help)"); }
    puts("Salida showcase."); return 0; }
