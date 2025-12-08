/*
 * interactive_cli.c - Bucle interactivo para educar el sistema Aurora (modo previo a "sueño")
 * Principios:
 *  - La inteligencia está en la geometría de tensores (Reglas + Arquetipos + Creencia C)
 *  - Solo trigate (inferir / aprender) y síntesis genérica; sin lógica de dominio
 *  - Menos líneas, máxima reutilización fractal (1→3→9 ideas recurrentes)
 *  - Este bucle permite alimentar ejemplos antes de activar modo sueño (autopoda/apoptosis)
 *
 * Comandos:
 *   learn A B R   -> A,B,R son triples de {1,0,N} (ej: learn 1N0 10N 110)
 *   infer A B     -> infiere R (triple) usando reglas → arquetipos → tensor C
 *   show rules    -> lista compacta primeras reglas
 *   show arch     -> lista arquetipos
 *   show C        -> muestra tensor C
 *   metrics       -> métricas y causa emergente
 *   help          -> ayuda rápida
 *   exit          -> salir
 *
 * Compilar:
 *   gcc -std=c11 -O2 -Wall -Wextra -o interactive_cli interactive_cli.c
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bootstrap_loader.h"

typedef int Trit; /* 1,0,-1 */

/* ===== Núcleo trigate ===== */
static Trit trit_and(Trit a,Trit b){ if(a==0||b==0) return 0; if(a==1&&b==1) return 1; return -1; }
static Trit trit_or(Trit a,Trit b){ if(a==1||b==1) return 1; if(a==0&&b==0) return 0; return -1; }
static Trit trit_consensus(Trit a,Trit b){ if(a!=-1 && a==b) return a; return -1; }
static Trit trigate_infer(Trit a,Trit b,Trit m){ if(m==0) return trit_and(a,b); if(m==1) return trit_or(a,b); return trit_consensus(a,b); }
static Trit trigate_learn(Trit a,Trit b,Trit r){ Trit ra=trit_and(a,b); if(ra==r) return 0; Trit ro=trit_or(a,b); if(ro==r) return 1; if(a!=-1 && a==b && r==a) return -1; return -1; }

static void vec_infer(const Trit A[3],const Trit B[3],const Trit M[3],Trit R[3]){ for(int i=0;i<3;i++) R[i]=trigate_infer(A[i],B[i],M[i]); }
static void vec_learn(const Trit A[3],const Trit B[3],const Trit R[3],Trit M[3]){ for(int i=0;i<3;i++) M[i]=trigate_learn(A[i],B[i],R[i]); }
static int eq3(const Trit x[3],const Trit y[3]){ for(int i=0;i<3;i++) if(x[i]!=y[i]) return 0; return 1; }
static void cp3(Trit d[3],const Trit s[3]){ for(int i=0;i<3;i++) d[i]=s[i]; }

/* ===== Memorias ===== */
typedef struct { Trit A[3]; Trit B[3]; Trit M[3]; int count; } Rule;
typedef struct { Trit pattern_A[3]; Trit pattern_B[3]; Trit pattern_M[3]; int support; } Archetype;
/* Añadimos versión/revisión para incremental */
static unsigned long global_rev = 1; /* inicia en 1 */
static unsigned long last_saved_rev = 0; /* última rev guardada completa */
typedef struct { Trit A[3]; Trit B[3]; Trit M[3]; int count; unsigned long rev; } RuleRev;
typedef struct { Trit pattern_A[3]; Trit pattern_B[3]; Trit pattern_M[3]; int support; unsigned long rev; } ArchRev;

#define MAX_RULES  512
#define MAX_ARCH   64
static RuleRev rules[MAX_RULES]; static int n_rules=0;
static ArchRev archs[MAX_ARCH]; static int n_arch=0;

/* ===== Utilidades ===== */
static const char* ts(Trit t){ return t==1?"1":(t==0?"0":"N"); }
static Trit char_to_trit(char c){ if(c=='1') return 1; if(c=='0') return 0; return -1; }
static int parse_triplet(const char* s,Trit out[3]){ if(strlen(s)<3) return 0; for(int i=0;i<3;i++) out[i]=char_to_trit(s[i]); return 1; }
static void print_vec(const char* label,const Trit v[3]){ printf("%s=[%s,%s,%s]",label,ts(v[0]),ts(v[1]),ts(v[2])); }

/* ===== Mapeo de tokens nombrados (facilidad de uso) =====
 * Cada nombre representa un vector ternario simplificado.
 * Convención mínima inicial (extensible):
 *   V   -> vocal genérica        {1,-1,-1}
 *   C   -> consonante genérica    {0,-1,-1}
 *   FUS -> resultado fusión       {1,1,1}
 *   SIL -> resultado sílaba       {0,0,0}
 *   UNK -> desconocido/null       {-1,-1,-1}
 *   POS -> positivo genérico      {1,1,-1}
 *   NEG -> negativo genérico      {0,0,-1}
 */
typedef struct { const char* name; Trit v[3]; } NamedToken;
static const NamedToken NAMED[] = {
    {"V",   {1,-1,-1}}, {"C",   {0,-1,-1}}, {"FUS", {1,1,1}},
    {"SIL", {0,0,0}},   {"UNK", {-1,-1,-1}}, {"POS", {1,1,-1}},
    {"NEG", {0,0,-1}},  {"VC",  {1,0,-1}},   {"CV",  {0,1,-1}}
};
static int parse_named(const char* s,Trit out[3]){
    for(size_t i=0;i<sizeof(NAMED)/sizeof(NAMED[0]);i++){
        if(strcmp(s,NAMED[i].name)==0){ out[0]=NAMED[i].v[0]; out[1]=NAMED[i].v[1]; out[2]=NAMED[i].v[2]; return 1; }
    }
    return 0;
}
/* wrapper: intenta primero nombre, luego tripleta cruda (### con 1/0/N) */
static int parse_any(const char* s,Trit out[3]){
    if(parse_named(s,out)) return 1;
    if(strlen(s)>=3){
        int ok=1; for(int i=0;i<3;i++){ char c=s[i]; if(!(c=='1'||c=='0'||c=='N')){ ok=0; break; } }
        if(ok){ for(int i=0;i<3;i++) out[i]=char_to_trit(s[i]); return 1; }
    }
    return 0;
}

/* ===== Upsert regla ===== */
static int find_rule(const Trit A[3],const Trit B[3]){ for(int i=0;i<n_rules;i++) if(eq3(rules[i].A,A)&&eq3(rules[i].B,B)) return i; return -1; }
static void upsert_rule(const Trit A[3],const Trit B[3],const Trit M[3]){
    int k=find_rule(A,B); if(k<0 && n_rules<MAX_RULES){ k=n_rules++; cp3(rules[k].A,A); cp3(rules[k].B,B); for(int i=0;i<3;i++) rules[k].M[i]=-1; rules[k].count=0; rules[k].rev=global_rev++; }
    if(k<0) return; rules[k].count++; int changed=0;
    for(int i=0;i<3;i++){ Trit m=M[i]; if(m==-1) continue; if(rules[k].M[i]==-1){ rules[k].M[i]=m; changed=1; } else if(rules[k].M[i]!=m){ rules[k].M[i]=-1; changed=1; } }
    if(changed) rules[k].rev=global_rev++;
}

/* ===== Síntesis de arquetipos (agrupa por A[0] = 0/1) ===== */
static void rebuild_archetypes(void){
    n_arch=0; for(int phase=0; phase<2 && n_arch<MAX_ARCH; phase++){ Trit target = phase?1:0; int cnt=0; int sumA[3]={0},sumB[3]={0},sumM[3]={0};
        for(int i=0;i<n_rules;i++){ if(rules[i].A[0]==target && rules[i].count>=2){ cnt++; for(int d=0;d<3;d++){ if(rules[i].A[d]!=-1) sumA[d]+=rules[i].A[d]; if(rules[i].B[d]!=-1) sumB[d]+=rules[i].B[d]; if(rules[i].M[d]!=-1) sumM[d]+=rules[i].M[d]; } } }
        if(cnt){ ArchRev *a=&archs[n_arch]; for(int d=0;d<3;d++){ a->pattern_A[d]= (sumA[d]*2>=cnt)?1: (sumA[d]==0?0:-1); a->pattern_B[d]= (sumB[d]*2>=cnt)?1: (sumB[d]==0?0:-1); a->pattern_M[d]= (sumM[d]*2>=cnt)?1: (sumM[d]==0?0:-1); } a->support=cnt; a->rev=global_rev++; n_arch++; }
    }
}

/* ===== Tensor C (creencia) simplificado: consenso FO ← patrones M ===== */
typedef struct { Trit FO[3]; Trit FN[3]; Trit ES[3]; } TensorC;
static TensorC Ctensor; static int C_valid=0;
static int count_nulls(const TensorC* t){ int c=0; for(int i=0;i<3;i++){ if(t->FO[i]==-1) c++; if(t->FN[i]==-1) c++; if(t->ES[i]==-1) c++; } return c; }

static void rebuild_C(void){
    TensorC C={{-1,-1,-1},{-1,-1,-1},{-1,-1,-1}};
    /* FO: consenso sobre M de reglas */
    for(int d=0;d<3;d++){ int c1=0,c0=0; for(int i=0;i<n_rules;i++){ Trit m=rules[i].M[d]; if(m==1) c1++; else if(m==0) c0++; }
        if(c1>=2 && c1>=c0) C.FO[d]=1; else if(c0>=2 && c0>c1) C.FO[d]=0; }
    /* FN: primer arquetipo estable */
    if(n_arch>0){ for(int d=0; d<3; d++) C.FN[d]=archs[0].pattern_M[d]; }
    /* ES: triádico FO/FN + delegación */
    for(int d=0; d<3; d++){ Trit a=C.FO[d],b=C.FN[d]; if(a==b && a!=-1) C.ES[d]=a; else C.ES[d]=-1; }
    Ctensor=C; C_valid=1;
}

/* ===== Inferencia guiada ===== */
static int match_arch(const ArchRev* a,const Trit A[3],const Trit B[3]){
    for(int d=0;d<3;d++){ Trit pa=a->pattern_A[d]; Trit pb=a->pattern_B[d]; if(pa!=-1 && pa!=A[d]) return 0; if(pb!=-1 && pb!=B[d]) return 0; }
    return 1;
}

static void infer(const Trit A[3],const Trit B[3],Trit R[3]){
    /* 1. Regla exacta */
    int k=find_rule(A,B); if(k>=0){ vec_infer(A,B,rules[k].M,R); return; }
    /* 2. Arquetipo */
    for(int i=0;i<n_arch;i++){ if(match_arch(&archs[i],A,B)){ vec_infer(A,B,archs[i].pattern_M,R); return; } }
    /* 3. Tensor C fallback (FO interpretado como resultado) */
    if(C_valid){ for(int i=0;i<3;i++) R[i]=Ctensor.FO[i]; return; }
    for(int i=0;i<3;i++) R[i]=-1; /* desconocido */
}

/* ===== Métricas simples ===== */
static float metric_consistency(void){ if(n_rules<2) return 0.0f; int pairs=0,consistent=0; for(int i=0;i<n_rules-1;i++) for(int j=i+1;j<n_rules;j++){ int simA=0; for(int d=0;d<3;d++) if(rules[i].A[d]==rules[j].A[d]) simA++; if(simA>=2){ pairs++; int simM=0; for(int d=0;d<3;d++) if(rules[i].M[d]==rules[j].M[d]) simM++; if(simM>=2) consistent++; } } return pairs? (float)consistent/pairs:0.0f; }
static float metric_separability(void){ if(n_rules==0) return 0.0f; int unique=0; for(int i=0;i<n_rules;i++){ int u=1; for(int j=0;j<i;j++) if(eq3(rules[i].M,rules[j].M)){ u=0; break; } if(u) unique++; } return (float)unique/n_rules; }
static float metric_convergence(void){ if(n_rules==0) return 0.0f; int good=0; for(int i=0;i<n_rules;i++){ int nulls=0; for(int d=0;d<3;d++) if(rules[i].M[d]==-1) nulls++; if(nulls<=1) good++; } return (float)good/n_rules; }

static const char* causa(void){ int nullM=0; for(int i=0;i<n_rules;i++){ for(int d=0;d<3;d++) if(rules[i].M[d]==-1) nullM++; }
    if(nullM > n_rules) return "Falta de información"; if(n_arch==0 && n_rules>4) return "Definiciones incorrectas"; return "Sistema en equilibrio"; }

/* ===== Loop ===== */
static void help(void){
    puts("Comandos: learn A B R | infer A B | load [path] | save [path] | restore [path] | savec [path] | restorec [path] | saveinc [log] | restorelog [log] | sleep [n] | show rules | show arch | show C | metrics | help | exit");
    puts("  load: ingiere palabras bootstrap (pares secuenciales → reglas)");
    puts("  save: persiste reglas/arquetipos/tensor C en archivo (default aurora_state.dat)");
    puts("  restore: carga estado previo desde archivo");
}

/* ===== Persistencia mínima =====
 * Formato texto lineal:
 *   AURORA_STATE v1
 *   R A### B### M### count
 *   K A### B### M### support
 *   C FO### FN### ES### valid
 */
static int save_state(const char* path){
    const char* fn = (path && *path)? path : "aurora_state.dat";
    FILE* f = fopen(fn,"w"); if(!f) return 0;
    fprintf(f,"AURORA_STATE v1\n");
    for(int i=0;i<n_rules;i++){
        fprintf(f,"R %c%c%c %c%c%c %c%c%c %d %lu\n",
            ts(rules[i].A[0])[0],ts(rules[i].A[1])[0],ts(rules[i].A[2])[0],
            ts(rules[i].B[0])[0],ts(rules[i].B[1])[0],ts(rules[i].B[2])[0],
            ts(rules[i].M[0])[0],ts(rules[i].M[1])[0],ts(rules[i].M[2])[0],
            rules[i].count,rules[i].rev);
    }
    for(int i=0;i<n_arch;i++){
        fprintf(f,"K %c%c%c %c%c%c %c%c%c %d %lu\n",
            ts(archs[i].pattern_A[0])[0],ts(archs[i].pattern_A[1])[0],ts(archs[i].pattern_A[2])[0],
            ts(archs[i].pattern_B[0])[0],ts(archs[i].pattern_B[1])[0],ts(archs[i].pattern_B[2])[0],
            ts(archs[i].pattern_M[0])[0],ts(archs[i].pattern_M[1])[0],ts(archs[i].pattern_M[2])[0],
            archs[i].support,archs[i].rev);
    }
    if(C_valid){
        fprintf(f,"C %c%c%c %c%c%c %c%c%c %d\n",
            ts(Ctensor.FO[0])[0],ts(Ctensor.FO[1])[0],ts(Ctensor.FO[2])[0],
            ts(Ctensor.FN[0])[0],ts(Ctensor.FN[1])[0],ts(Ctensor.FN[2])[0],
            ts(Ctensor.ES[0])[0],ts(Ctensor.ES[1])[0],ts(Ctensor.ES[2])[0],
            1);
    }
    fclose(f); return 1;
}

static int restore_state(const char* path){
    const char* fn = (path && *path)? path : "aurora_state.dat";
    FILE* f=fopen(fn,"r"); if(!f) return 0;
    char tag[8]; if(fscanf(f,"%7s", tag)!=1 || strcmp(tag,"AURORA_STATE")!=0){ fclose(f); return 0; }
    char version[8]; fscanf(f,"%7s", version); /* v1 */
    n_rules=0; n_arch=0; C_valid=0;
    char lineType; while(fscanf(f," %c", &lineType)==1){
        if(lineType=='R'){ char a[4],b[4],m[4]; int cnt; unsigned long rev; if(fscanf(f," %3s %3s %3s %d %lu", a,b,m,&cnt,&rev)!=6) break;
            if(n_rules<MAX_RULES){ RuleRev *r=&rules[n_rules++]; for(int i=0;i<3;i++){ r->A[i]=char_to_trit(a[i]); r->B[i]=char_to_trit(b[i]); r->M[i]=char_to_trit(m[i]); } r->count=cnt; r->rev=rev; if(rev>global_rev) global_rev=rev+1; }
        } else if(lineType=='K'){ char a[4],b[4],m[4]; int sup; unsigned long rev; if(fscanf(f," %3s %3s %3s %d %lu", a,b,m,&sup,&rev)!=6) break;
            if(n_arch<MAX_ARCH){ ArchRev *k=&archs[n_arch++]; for(int i=0;i<3;i++){ k->pattern_A[i]=char_to_trit(a[i]); k->pattern_B[i]=char_to_trit(b[i]); k->pattern_M[i]=char_to_trit(m[i]); } k->support=sup; k->rev=rev; if(rev>global_rev) global_rev=rev+1; }
        } else if(lineType=='C'){ char fo[4],fnv[4],es[4]; int v; if(fscanf(f," %3s %3s %3s %d", fo,fnv,es,&v)!=4) break;
            if(v){ for(int i=0;i<3;i++){ Ctensor.FO[i]=char_to_trit(fo[i]); Ctensor.FN[i]=char_to_trit(fnv[i]); Ctensor.ES[i]=char_to_trit(es[i]); } C_valid=1; }
        } else { break; }
    }
    fclose(f);
    return 1;
}

/* ===== Incremental save (solo nuevas revisiones desde last_saved_rev) ===== */
static int save_incremental(const char* path){
    const char* fn = (path && *path)? path : "aurora_state.log";
    FILE* f=fopen(fn,"a"); if(!f) return 0;
    fprintf(f,"INC v1 base=%lu current=%lu\n", last_saved_rev, global_rev);
    for(int i=0;i<n_rules;i++) if(rules[i].rev>last_saved_rev)
        fprintf(f,"RI %c%c%c %c%c%c %c%c%c %d %lu\n",
            ts(rules[i].A[0])[0],ts(rules[i].A[1])[0],ts(rules[i].A[2])[0],
            ts(rules[i].B[0])[0],ts(rules[i].B[1])[0],ts(rules[i].B[2])[0],
            ts(rules[i].M[0])[0],ts(rules[i].M[1])[0],ts(rules[i].M[2])[0],
            rules[i].count,rules[i].rev);
    for(int i=0;i<n_arch;i++) if(archs[i].rev>last_saved_rev)
        fprintf(f,"KI %c%c%c %c%c%c %c%c%c %d %lu\n",
            ts(archs[i].pattern_A[0])[0],ts(archs[i].pattern_A[1])[0],ts(archs[i].pattern_A[2])[0],
            ts(archs[i].pattern_B[0])[0],ts(archs[i].pattern_B[1])[0],ts(archs[i].pattern_B[2])[0],
            ts(archs[i].pattern_M[0])[0],ts(archs[i].pattern_M[1])[0],ts(archs[i].pattern_M[2])[0],
            archs[i].support,archs[i].rev);
    if(C_valid) fprintf(f,"CI %c%c%c %c%c%c %c%c%c %lu\n",
            ts(Ctensor.FO[0])[0],ts(Ctensor.FO[1])[0],ts(Ctensor.FO[2])[0],
            ts(Ctensor.FN[0])[0],ts(Ctensor.FN[1])[0],ts(Ctensor.FN[2])[0],
            ts(Ctensor.ES[0])[0],ts(Ctensor.ES[1])[0],ts(Ctensor.ES[2])[0],global_rev);
    fclose(f); last_saved_rev=global_rev; return 1;
}

static int restore_log(const char* path){
    const char* fn = (path && *path)? path : "aurora_state.log";
    FILE* f=fopen(fn,"r"); if(!f) return 0; char tag[8];
    while(fscanf(f,"%7s", tag)==1){
        if(strcmp(tag,"INC")==0){ unsigned long base,cur; if(fscanf(f," base=%lu current=%lu", &base,&cur)==2){ if(cur>global_rev) global_rev=cur+1; }
        } else if(strcmp(tag,"RI")==0){ char a[4],b[4],m[4]; int cnt; unsigned long rev; if(fscanf(f," %3s %3s %3s %d %lu", a,b,m,&cnt,&rev)!=6) break;
            Trit A[3],B[3],M[3]; for(int i=0;i<3;i++){ A[i]=char_to_trit(a[i]); B[i]=char_to_trit(b[i]); M[i]=char_to_trit(m[i]); }
            upsert_rule(A,B,M); /* rev actualiza dentro */
        } else if(strcmp(tag,"KI")==0){ char a[4],b[4],m[4]; int sup; unsigned long rev; if(fscanf(f," %3s %3s %3s %d %lu", a,b,m,&sup,&rev)!=6) break;
            if(n_arch<MAX_ARCH){ ArchRev *k=&archs[n_arch++]; for(int i=0;i<3;i++){ k->pattern_A[i]=char_to_trit(a[i]); k->pattern_B[i]=char_to_trit(b[i]); k->pattern_M[i]=char_to_trit(m[i]); } k->support=sup; k->rev=rev; if(rev>global_rev) global_rev=rev+1; }
        } else if(strcmp(tag,"CI")==0){ char fo[4],fnv[4],es[4]; unsigned long rev; if(fscanf(f," %3s %3s %3s %lu", fo,fnv,es,&rev)!=4) break;
            for(int i=0;i<3;i++){ Ctensor.FO[i]=char_to_trit(fo[i]); Ctensor.FN[i]=char_to_trit(fnv[i]); Ctensor.ES[i]=char_to_trit(es[i]); } C_valid=1; if(rev>global_rev) global_rev=rev+1; }
    }
    fclose(f); rebuild_archetypes(); rebuild_C(); return 1;
}

/* ===== Compresión simple (snapshot binario) ===== */
static int pack_trit(Trit t){ if(t==1) return 2; if(t==0) return 1; return 0; }
static Trit unpack_trit(int v){ if(v==2) return 1; if(v==1) return 0; return -1; }

static int save_compressed(const char* path){
    const char* fn = (path && *path)? path : "aurora_state.bin"; FILE* f=fopen(fn,"wb"); if(!f) return 0;
    unsigned char header[8] = {'A','S','T','V','2',0,0,0}; fwrite(header,1,8,f);
    fwrite(&n_rules,sizeof(n_rules),1,f); fwrite(&n_arch,sizeof(n_arch),1,f); fwrite(&C_valid,sizeof(C_valid),1,f);
    for(int i=0;i<n_rules;i++){
        unsigned char block[4]={0,0,0,0};
        int bits=0; int acc=0; int vals[9]={ pack_trit(rules[i].A[0]),pack_trit(rules[i].A[1]),pack_trit(rules[i].A[2]),pack_trit(rules[i].B[0]),pack_trit(rules[i].B[1]),pack_trit(rules[i].B[2]),pack_trit(rules[i].M[0]),pack_trit(rules[i].M[1]),pack_trit(rules[i].M[2]) };
        for(int v=0;v<9;v++){ acc |= (vals[v]&0x3) << bits; bits+=2; if(bits>=8){ block[0]=acc & 0xFF; acc >>=8; bits-=8; fwrite(block,1,1,f); } }
        if(bits>0){ block[0]=acc & 0xFF; fwrite(block,1,1,f); }
        fwrite(&rules[i].count,sizeof(rules[i].count),1,f); fwrite(&rules[i].rev,sizeof(rules[i].rev),1,f);
    }
    for(int i=0;i<n_arch;i++){
        unsigned char block[4]={0,0,0,0}; int bits=0; int acc=0; int vals[9]={ pack_trit(archs[i].pattern_A[0]),pack_trit(archs[i].pattern_A[1]),pack_trit(archs[i].pattern_A[2]),pack_trit(archs[i].pattern_B[0]),pack_trit(archs[i].pattern_B[1]),pack_trit(archs[i].pattern_B[2]),pack_trit(archs[i].pattern_M[0]),pack_trit(archs[i].pattern_M[1]),pack_trit(archs[i].pattern_M[2]) };
        for(int v=0;v<9;v++){ acc |= (vals[v]&0x3) << bits; bits+=2; if(bits>=8){ block[0]=acc & 0xFF; acc >>=8; bits-=8; fwrite(block,1,1,f); } }
        if(bits>0){ block[0]=acc & 0xFF; fwrite(block,1,1,f); }
        fwrite(&archs[i].support,sizeof(archs[i].support),1,f); fwrite(&archs[i].rev,sizeof(archs[i].rev),1,f);
    }
    if(C_valid){ for(int i=0;i<3;i++){ unsigned char v=(unsigned char)pack_trit(Ctensor.FO[i]); fwrite(&v,1,1,f); }
        for(int i=0;i<3;i++){ unsigned char v=(unsigned char)pack_trit(Ctensor.FN[i]); fwrite(&v,1,1,f); }
        for(int i=0;i<3;i++){ unsigned char v=(unsigned char)pack_trit(Ctensor.ES[i]); fwrite(&v,1,1,f); }
    }
    fclose(f); return 1;
}

static int restore_compressed(const char* path){
    const char* fn = (path && *path)? path : "aurora_state.bin"; FILE* f=fopen(fn,"rb"); if(!f) return 0;
    unsigned char header[8]; if(fread(header,1,8,f)!=8){ fclose(f); return 0; }
    if(header[0]!='A'||header[1]!='S'||header[2]!='T'||header[3]!='V') { fclose(f); return 0; }
    fread(&n_rules,sizeof(n_rules),1,f); fread(&n_arch,sizeof(n_arch),1,f); fread(&C_valid,sizeof(C_valid),1,f);
    if(n_rules>MAX_RULES) n_rules=MAX_RULES; if(n_arch>MAX_ARCH) n_arch=MAX_ARCH;
    for(int i=0;i<n_rules;i++){
        int neededBits=18; int readBits=0; int acc=0; int vals[9]; int idx=0; unsigned char byte;
        while(readBits<neededBits){ if(fread(&byte,1,1,f)!=1){ fclose(f); return 0; } acc |= byte << readBits; readBits+=8; }
        for(int v=0; v<9; v++){ vals[v] = (acc >> (2*v)) & 0x3; }
        rules[i].A[0]=unpack_trit(vals[0]); rules[i].A[1]=unpack_trit(vals[1]); rules[i].A[2]=unpack_trit(vals[2]);
        rules[i].B[0]=unpack_trit(vals[3]); rules[i].B[1]=unpack_trit(vals[4]); rules[i].B[2]=unpack_trit(vals[5]);
        rules[i].M[0]=unpack_trit(vals[6]); rules[i].M[1]=unpack_trit(vals[7]); rules[i].M[2]=unpack_trit(vals[8]);
        fread(&rules[i].count,sizeof(rules[i].count),1,f); fread(&rules[i].rev,sizeof(rules[i].rev),1,f); if(rules[i].rev>global_rev) global_rev=rules[i].rev+1;
    }
    for(int i=0;i<n_arch;i++){
        int neededBits=18; int readBits=0; int acc=0; int vals[9]; unsigned char byte;
        while(readBits<neededBits){ if(fread(&byte,1,1,f)!=1){ fclose(f); return 0; } acc |= byte << readBits; readBits+=8; }
        archs[i].pattern_A[0]=unpack_trit((acc>>0)&0x3); archs[i].pattern_A[1]=unpack_trit((acc>>2)&0x3); archs[i].pattern_A[2]=unpack_trit((acc>>4)&0x3);
        archs[i].pattern_B[0]=unpack_trit((acc>>6)&0x3); archs[i].pattern_B[1]=unpack_trit((acc>>8)&0x3); archs[i].pattern_B[2]=unpack_trit((acc>>10)&0x3);
        archs[i].pattern_M[0]=unpack_trit((acc>>12)&0x3); archs[i].pattern_M[1]=unpack_trit((acc>>14)&0x3); archs[i].pattern_M[2]=unpack_trit((acc>>16)&0x3);
        fread(&archs[i].support,sizeof(archs[i].support),1,f); fread(&archs[i].rev,sizeof(archs[i].rev),1,f); if(archs[i].rev>global_rev) global_rev=archs[i].rev+1;
    }
    if(C_valid){ for(int i=0;i<3;i++){ unsigned char v; fread(&v,1,1,f); Ctensor.FO[i]=unpack_trit(v); }
        for(int i=0;i<3;i++){ unsigned char v; fread(&v,1,1,f); Ctensor.FN[i]=unpack_trit(v); }
        for(int i=0;i<3;i++){ unsigned char v; fread(&v,1,1,f); Ctensor.ES[i]=unpack_trit(v); }
    }
    fclose(f); return 1;
}

/* ===== Modo Sueño: autopoda + apoptosis =====
 * Autopoda: elimina reglas débiles (count<2 y M todo null) y reglas redundantes
 * Apoptosis: si métricas muy bajas tras autopoda, purga dejando solo reglas de mayor soporte
 */
static void autopoda(void){
    int w=0; for(int i=0;i<n_rules;i++){ int alln = (rules[i].M[0]==-1 && rules[i].M[1]==-1 && rules[i].M[2]==-1); if(rules[i].count<2 && alln) continue; rules[w++]=rules[i]; }
    n_rules=w;
    /* eliminar redundancias mismas A,B,M manteniendo mayor count */
    for(int i=0;i<n_rules;i++){
        for(int j=i+1;j<n_rules;j++){
            if(eq3(rules[i].A,rules[j].A)&&eq3(rules[i].B,rules[j].B)&&eq3(rules[i].M,rules[j].M)){
                if(rules[j].count>rules[i].count){ Rule tmp=rules[i]; rules[i]=rules[j]; rules[j]=tmp; }
            }
        }
    }
    int v=0; for(int i=0;i<n_rules;i++){ int duplicate=0; for(int j=0;j<i;j++){ if(eq3(rules[i].A,rules[j].A)&&eq3(rules[i].B,rules[j].B)&&eq3(rules[i].M,rules[j].M)){ duplicate=1; break; } } if(!duplicate) rules[v++]=rules[i]; }
    n_rules=v;
}

static void apoptosis(void){
    float c=metric_consistency(), s=metric_separability(), v=metric_convergence();
    if(c<0.2f && s<0.2f && v<0.2f && n_rules>4){
        /* conservar top K por soporte */
        int K = n_rules/2; if(K<3) K=3; for(int i=0;i<n_rules;i++){ for(int j=i+1;j<n_rules;j++){ if(rules[j].count>rules[i].count){ Rule tmp=rules[i]; rules[i]=rules[j]; rules[j]=tmp; } } }
        n_rules = K;
    }
}

static void sleep_cycle(int epochs){
    if(epochs<1) epochs=1; if(epochs>25) epochs=25;
    for(int ep=0; ep<epochs; ep++){
        autopoda();
        rebuild_archetypes();
        rebuild_C();
        apoptosis();
    }
    float c=metric_consistency(), s=metric_separability(), v=metric_convergence();
    printf("Sueño completado: reglas=%d archs=%d Cons=%.2f Sep=%.2f Conv=%.2f\n", n_rules, n_arch, c,s,v);
}

int main(void){
    puts("Aurora Interactive CLI - educación antes de modo sueño"); help();
    char line[256];
    while(printf("> "), fflush(stdout), fgets(line,sizeof(line),stdin)){
        char* nl=strchr(line,'\n'); if(nl) *nl='\0';
        if(!*line) continue; char cmd[32], t1[32], t2[32], t3[32];
        if(sscanf(line,"%31s %31s %31s %31s", cmd,t1,t2,t3)>=1){
            if(strcmp(cmd,"exit")==0) break;
            else if(strcmp(cmd,"help")==0){ help(); }
            else if(strcmp(cmd,"learn")==0){
                Trit A[3],B[3],R[3],M[3];
                if(!parse_any(t1,A)||!parse_any(t2,B)||!parse_any(t3,R)){ puts("Formato learn inválido (use V/C/FUS/SIL/UNK o tripleta 1N0)"); continue; }
                vec_learn(A,B,R,M); upsert_rule(A,B,M); rebuild_archetypes(); rebuild_C();
                printf("Aprendido: "); print_vec("A",A); printf(" "); print_vec("B",B); printf(" -> R"); print_vec("",R); printf(" M=[%s,%s,%s]\n",ts(M[0]),ts(M[1]),ts(M[2]));
            }
            else if(strcmp(cmd,"infer")==0){ Trit A[3],B[3],R[3]; if(!parse_any(t1,A)||!parse_any(t2,B)){ puts("Formato infer inválido"); continue; } infer(A,B,R); print_vec("R",R); puts(""); }
            else if(strcmp(cmd,"sleep")==0){ int n=1; if(*t1){ n=atoi(t1); } sleep_cycle(n); }
            else if(strcmp(cmd,"load")==0){
                const char* path = (*t1)? t1 : "bootstrap_es_1000.txt";
                size_t cnt = load_bootstrap_words(path);
                if(!cnt){ printf("No se pudo cargar '%s'\n", path); continue; }
                size_t limit = (cnt>200)?200:cnt; /* limitar ingestión masiva inicial */
                for(size_t i=0; i+1<limit; i++){
                    const BootWord* wA = get_bootstrap_word(i);
                    const BootWord* wB = get_bootstrap_word(i+1);
                    Trit A[3] = { wA->ffe[0], wA->ffe[1], wA->ffe[2] };
                    Trit B[3] = { wB->ffe[0], wB->ffe[1], wB->ffe[2] };
                    Trit R[3] = { wB->ffe[0], wB->ffe[1], wB->ffe[2] }; /* resultado heurístico = forma B */
                    Trit M[3]; vec_learn(A,B,R,M); upsert_rule(A,B,M);
                }
                rebuild_archetypes(); rebuild_C();
                printf("Bootstrap cargado: %zu palabras → reglas=%d arch=%d\n", cnt, n_rules, n_arch);
            }
            else if(strcmp(cmd,"save")==0){ const char* path = (*t1)? t1 : "aurora_state.dat"; if(save_state(path)){ last_saved_rev=global_rev; printf("Estado guardado en %s rev=%lu\n", path,last_saved_rev); } else printf("Error guardando %s\n", path); }
            else if(strcmp(cmd,"restore")==0){ const char* path = (*t1)? t1 : "aurora_state.dat"; if(restore_state(path)) printf("Estado restaurado desde %s (reglas=%d arch=%d C=%s rev=%lu)\n", path, n_rules, n_arch, C_valid?"OK":"--", global_rev); else printf("Error restaurando %s\n", path); }
            else if(strcmp(cmd,"saveinc")==0){ const char* path = (*t1)? t1 : "aurora_state.log"; if(save_incremental(path)) printf("Incremental guardado en %s (nuevo base=%lu)\n", path,last_saved_rev); else printf("Error incremental %s\n", path); }
            else if(strcmp(cmd,"restorelog")==0){ const char* path = (*t1)? t1 : "aurora_state.log"; if(restore_log(path)) printf("Log aplicado desde %s (reglas=%d arch=%d rev=%lu)\n", path,n_rules,n_arch,global_rev); else printf("Error log %s\n", path); }
            else if(strcmp(cmd,"savec")==0){ const char* path = (*t1)? t1 : "aurora_state.bin"; if(save_compressed(path)) printf("Snapshot comprimido en %s\n", path); else printf("Error snapshot %s\n", path); }
            else if(strcmp(cmd,"restorec")==0){ const char* path = (*t1)? t1 : "aurora_state.bin"; if(restore_compressed(path)) printf("Restaurado comprimido %s (reglas=%d arch=%d C=%s rev=%lu)\n", path,n_rules,n_arch,C_valid?"OK":"--",global_rev); else printf("Error restaurando comprimido %s\n", path); }
            else if(strcmp(cmd,"show")==0){
                if(strcmp(t1,"rules")==0){ for(int i=0;i<n_rules && i<20;i++){ printf("%02d A=[%s,%s,%s] B=[%s,%s,%s] M=[%s,%s,%s] c=%d\n", i, ts(rules[i].A[0]),ts(rules[i].A[1]),ts(rules[i].A[2]), ts(rules[i].B[0]),ts(rules[i].B[1]),ts(rules[i].B[2]), ts(rules[i].M[0]),ts(rules[i].M[1]),ts(rules[i].M[2]), rules[i].count); } }
                else if(strcmp(t1,"arch")==0){ for(int i=0;i<n_arch;i++){ printf("Arch%02d A=[%s,%s,%s] B=[%s,%s,%s] M=[%s,%s,%s] sup=%d\n", i, ts(archs[i].pattern_A[0]),ts(archs[i].pattern_A[1]),ts(archs[i].pattern_A[2]), ts(archs[i].pattern_B[0]),ts(archs[i].pattern_B[1]),ts(archs[i].pattern_B[2]), ts(archs[i].pattern_M[0]),ts(archs[i].pattern_M[1]),ts(archs[i].pattern_M[2]), archs[i].support); } }
                else if(strcmp(t1,"C")==0){ if(C_valid){ printf("C FO=[%s,%s,%s] FN=[%s,%s,%s] ES=[%s,%s,%s] nulls=%d\n", ts(Ctensor.FO[0]),ts(Ctensor.FO[1]),ts(Ctensor.FO[2]), ts(Ctensor.FN[0]),ts(Ctensor.FN[1]),ts(Ctensor.FN[2]), ts(Ctensor.ES[0]),ts(Ctensor.ES[1]),ts(Ctensor.ES[2]), count_nulls(&Ctensor)); } else puts("C no disponible"); }
                else puts("show rules|arch|C");
            }
            else if(strcmp(cmd,"metrics")==0){ float c=metric_consistency(),s=metric_separability(),v=metric_convergence(); printf("Consistencia=%.2f Separabilidad=%.2f Convergencia=%.2f Reglas=%d Arch=%d Causa=%s\n", c,s,v,n_rules,n_arch,causa()); }
            else { puts("Comando desconocido (use help)"); }
        }
    }
    puts("Salida. Estado final:"); printf("Reglas=%d Arch=%d C=%s\n", n_rules,n_arch,C_valid?"OK":"--");
    return 0;
}
