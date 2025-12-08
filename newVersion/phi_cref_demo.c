/*
 * newVersion/phi_cref_demo.c
 * Stream natural-language-like tokens and watch a C-like tensor scalar converge
 * Self-contained; does not depend on v3.0 core. Balanced ternary: -1,0,1.
 *
 * Build (from newVersion):
 *   make phi_cref_demo
 * Run:
 *   ./phi_cref_demo   (or .\phi_cref_demo.exe on Windows)
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

// Ternary value: -1 (null/unknown), 0, +1
typedef int Trit; // values in {-1,0,1}

typedef struct { Trit t[3]; } Dimension;
typedef struct { Dimension d[3]; } VectorFFE;

typedef struct {
    Trit FO[3];
    Trit FN[3];
    Trit ES[3];
} TensorFFE;

static Trit agree2(Trit a, Trit b){ return (a==b)? a : -1; }
static Trit tri_consensus(Trit a, Trit b, Trit c){
    if(a==b || a==c) return a; if(b==c) return b; return -1;
}

static Dimension make_dim(Trit a, Trit b, Trit c){ Dimension d={{a,b,c}}; return d; }
static VectorFFE make_vec(Dimension a, Dimension b, Dimension c){ VectorFFE v={a,b,c}; return v; }
static TensorFFE make_tensor_all(Trit v){ TensorFFE t; for(int i=0;i<3;i++){t.FO[i]=v;t.FN[i]=v;t.ES[i]=v;} return t; }

// Tiny character-to-vector encoder
static void char_features(unsigned char ch, Trit out[3]){
    if(ch>='A'&&ch<='Z') ch=(unsigned char)(ch+32);
    int vowel = (ch=='a'||ch=='e'||ch=='i'||ch=='o'||ch=='u');
    int liquid= (ch=='l'||ch=='r');
    out[0] = vowel? 1:0;          // Forma
    out[1] = liquid? 1:0;         // Función
    out[2] = vowel? -1:(liquid?0:1); // Estructura
}

static VectorFFE vec_from_char(unsigned char ch){
    Trit f[3]; char_features(ch,f);
    Dimension d = make_dim(f[0],f[1],f[2]);
    return make_vec(d,d,d); // replicate across dims (simple, consistent)
}

// Minimal transcendence over 3 vectors (per-trit consensus)
static VectorFFE transcender_step(const VectorFFE* A, const VectorFFE* B, const VectorFFE* C){
    VectorFFE out = *A;
    for(int i=0;i<3;i++){
        for(int t=0;t<3;t++) out.d[i].t[t] = tri_consensus(A->d[i].t[t], B->d[i].t[t], C->d[i].t[t]);
    }
    return out;
}

// Build emergent vector for a word by folding sliding windows of size 3
static VectorFFE word_emergent_vector(const char* w){
    int n=(int)strlen(w);
    if(n<=0){ Dimension z=make_dim(-1,-1,-1); return make_vec(z,z,z);} 
    VectorFFE a=vec_from_char(w[0]);
    VectorFFE b=(n>1)?vec_from_char(w[1]):a;
    VectorFFE c=(n>2)?vec_from_char(w[2]):b;
    VectorFFE acc = transcender_step(&a,&b,&c);
    for(int i=1;i<=n-3;i++){
        a=vec_from_char(w[i]); b=vec_from_char(w[i+1]); c=vec_from_char(w[i+2]);
        VectorFFE tmp = transcender_step(&a,&b,&c);
        for(int d=0; d<3; d++) for(int t=0;t<3;t++) acc.d[d].t[t]=agree2(acc.d[d].t[t], tmp.d[d].t[t]);
    }
    return acc;
}

static TensorFFE tensor_from_vector(const VectorFFE* v){
    TensorFFE T = make_tensor_all(-1);
    for(int i=0;i<3;i++){ T.FO[i]=v->d[0].t[i]; T.FN[i]=v->d[1].t[i]; T.ES[i]=v->d[2].t[i]; }
    return T;
}

// Simple synthesize: per-trit consensus, bias toward recent B
static TensorFFE synthesize(const TensorFFE* A, const TensorFFE* B){
    TensorFFE R=*A;
    Trit const* X[3] = {B->FO,B->FN,B->ES};
    Trit*       Y[3] = {R.FO ,R.FN ,R.ES };
    Trit const* Aarr[3]={A->FO,A->FN,A->ES};
    for(int v=0;v<3;v++) for(int i=0;i<3;i++){
        Trit a=Aarr[v][i], b=X[v][i];
        Y[v][i] = (a==b)? a : ((b!=-1)? b : a);
    }
    return R;
}

// Balanced-ternary scalar: interpret 9 trits as digits in base-3 balanced, normalized
static double tensor_balanced_scalar(const TensorFFE* T){
    int w=0; double acc=0.0, scale=1.0; // scale grows as 3^k; normalize at the end
    Trit const* X[3]={T->FO,T->FN,T->ES};
    for(int v=0;v<3;v++) for(int i=0;i<3;i++,w++){
        double pw = pow(3.0,(double)w);
        acc += (double)X[v][i] * pw;
        scale = pw; // last
    }
    // last power was 3^(w-1); normalize by it to map into a compact range
    return acc / (scale>0? scale : 1.0);
}

static void tensor_balanced_digits(const TensorFFE* T, char out[16]){
    int k=0; Trit const* X[3]={T->FO,T->FN,T->ES};
    for(int v=0;v<3;v++) for(int i=0;i<3;i++){
        Trit t=X[v][i]; out[k++]=(t>0?'+':(t==0?'0':'-'));
    }
    out[k]='\0';
}

static void print_tensor(const char* tag, const TensorFFE* T){
    printf("%s FO=[%d,%d,%d] FN=[%d,%d,%d] ES=[%d,%d,%d]\n", tag,
        T->FO[0],T->FO[1],T->FO[2], T->FN[0],T->FN[1],T->FN[2], T->ES[0],T->ES[1],T->ES[2]);
}

int main(){
    const char* stream[] = {
        "ley","orden","armonia","armon\xC3\xADa","luz","camino","recto","verdad",
        "justo","equilibrio","proporcion","via","claro","bien","bello","unidad",
        NULL
    };

    printf("=== Cref Streaming (balanced ternary) ===\n");
    TensorFFE C = make_tensor_all(-1);

    int idx=0; const char* w;
    while((w=stream[idx++])!=NULL){
        VectorFFE v = word_emergent_vector(w);
        TensorFFE T = tensor_from_vector(&v);
        C = (idx==1)? T : synthesize(&C,&T);
        double s = tensor_balanced_scalar(&C);
        char digits[16]; tensor_balanced_digits(&C,digits);
        double phi = (1.0+sqrt(5.0))*0.5;
        printf("%-12s -> scalar=% .9f  digits=%s  |phi-s|=%.9f\n", w, s, digits, fabs(phi-s));
    }

    printf("\nFinal C tensor:\n");
    print_tensor("C", &C);
    char digits[16]; tensor_balanced_digits(&C,digits);
    printf("digits=%s\n", digits);
    return 0;
}
