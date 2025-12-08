/*
 * aurora_pipeline.c
 * Pipeline mínimo: carga tensores FFE → síntesis emergente fractal.
 * Solo usa código newVersion. Menos es más.
 * Sistema de trits entrópicos: 1=false, 2=true, 3=null.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int Trit; /* 1=false, 2=true, 3=null */

typedef struct { Trit t[3]; } Dimension;
typedef struct { char label[64]; Dimension dims[27]; int n_dims; } FFETensor;

/* Cargador compatible con formato generado por ffe_generator.save_for_c */
static FFETensor* load_ffe(const char* path, int* out_n) {
    FILE* f = fopen(path, "r");
    if(!f){ printf("❌ No archivo %s\n", path); return NULL; }
    int n_tensors, n_trits;
    if(fscanf(f, "%d %d\n", &n_tensors, &n_trits)!=2){ fclose(f); return NULL; }
    FFETensor* arr = (FFETensor*)calloc(n_tensors, sizeof(FFETensor));
    char line[4096];
    for(int i=0;i<n_tensors;i++){
        if(!fgets(line,sizeof(line),f)){ free(arr); fclose(f); return NULL; }
        line[strcspn(line,"\n")] = 0; strncpy(arr[i].label,line,63); arr[i].label[63]='\0';
        if(!fgets(line,sizeof(line),f)){ free(arr); fclose(f); return NULL; }
        char* tok = strtok(line, " \t\n"); int idx=0; int total = n_trits; arr[i].n_dims = total/3;
        while(tok && idx<total){ int v = atoi(tok); int d = idx/3; int p = idx%3; arr[i].dims[d].t[p] = (Trit)v; idx++; tok = strtok(NULL," \t\n"); }
    }
    fclose(f); *out_n = n_tensors; return arr;
}

/* Operaciones trigate básicas */
static Trit trit_and(Trit a,Trit b){ if(a==1||b==1) return 1; if(a==2&&b==2) return 2; return 3; }
static Trit trit_or (Trit a,Trit b){ if(a==2||b==2) return 2; if(a==1&&b==1) return 1; return 3; }
static Trit trit_con(Trit a,Trit b){ if(a!=3 && a==b) return a; return 3; }
static Trit infer(Trit a,Trit b,Trit m){ return m==1?trit_and(a,b): m==2?trit_or(a,b): trit_con(a,b); }

/* Síntesis emergente: de 9 dimensiones → 3 → 1 (fractal 1–3–9) */
static void emergent(const FFETensor* t, Dimension* lvl1, Dimension* lvl2){
    /* Grupos de tres dimensiones base (0..8) */
    for(int g=0; g<3; g++){
        int base = g*3; Dimension out = {{3,3,3}}; /* iniciar en null (entropía máxima) */
        Trit fo = t->dims[base].t[0];
        fo = infer(fo, t->dims[base+1].t[0], 3); /* CONSENSUS en FO */
        fo = infer(fo, t->dims[base+2].t[0], 3);
        Trit fn = infer(t->dims[base].t[1], t->dims[base+1].t[1], 2); /* OR en FN */
        fn = infer(fn, t->dims[base+2].t[1], 2);
        Trit es = infer(t->dims[base].t[2], t->dims[base+1].t[2], 1); /* AND en ES */
        es = infer(es, t->dims[base+2].t[2], 1);
        out.t[0]=fo; out.t[1]=fn; out.t[2]=es; lvl1[g]=out;
    }
    /* Segunda síntesis: combinar las 3 dimensiones emergentes → 1 dimensión superior */
    Trit fo = infer(lvl1[0].t[0], lvl1[1].t[0], 3); fo = infer(fo, lvl1[2].t[0], 3);
    Trit fn = infer(lvl1[0].t[1], lvl1[1].t[1], 2); fn = infer(fn, lvl1[2].t[1], 2);
    Trit es = infer(lvl1[0].t[2], lvl1[1].t[2], 1); es = infer(es, lvl1[2].t[2], 1);
    lvl2[0].t[0]=fo; lvl2[0].t[1]=fn; lvl2[0].t[2]=es;
}

static const char* name_trit(Trit v){ return v==1?"false" : v==2?"true" : "null"; }
static const char* name_fn(Trit v){ return v==1?"AND" : v==2?"OR" : "CONSENSUS"; }
static const char* name_es(Trit v){ return v==1?"orden1" : v==2?"orden2" : "orden3"; }

int main(int argc,char** argv){
    const char* file = (argc>1)?argv[1]:"tensors_ffe.txt";
    int n; FFETensor* tensors = load_ffe(file,&n);
    if(!tensors){ printf("⚠️  No datos\n"); return 1; }
    printf("✅ Cargados %d tensores de %s\n", n, file);
    if(n==0){ free(tensors); return 0; }

    /* Usar primer tensor como ejemplo */
    FFETensor* t = &tensors[0];
    printf("\n🔍 Tensor base: %s (primeras 9 dimensiones)\n", t->label);
    for(int i=0;i<9;i++){
        printf("  D%02d: [%d,%d,%d] → FO=%s FN=%s ES=%s\n", i,
               t->dims[i].t[0], t->dims[i].t[1], t->dims[i].t[2],
               name_trit(t->dims[i].t[0]), name_fn(t->dims[i].t[1]), name_es(t->dims[i].t[2]));
    }

    Dimension lvl1[3]; Dimension lvl2[1];
    emergent(t, lvl1, lvl2);
    printf("\n🌱 Nivel intermedio (3 dimensiones emergentes):\n");
    for(int i=0;i<3;i++){
        printf("  E%02d: [%d,%d,%d] FO=%s FN=%s ES=%s\n", i,
               lvl1[i].t[0], lvl1[i].t[1], lvl1[i].t[2],
               name_trit(lvl1[i].t[0]), name_fn(lvl1[i].t[1]), name_es(lvl1[i].t[2]));
    }
    printf("\n🌌 Emergencia final (1 dimensión superior): [%d,%d,%d] → FO=%s FN=%s ES=%s\n",
           lvl2[0].t[0], lvl2[0].t[1], lvl2[0].t[2],
           name_trit(lvl2[0].t[0]), name_fn(lvl2[0].t[1]), name_es(lvl2[0].t[2]));

    free(tensors);
    printf("\n✅ Pipeline fractal mínimo completado\n");
    return 0;
}
