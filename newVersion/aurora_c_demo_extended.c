/*
 * aurora_c_demo_extended.c
 * Extended C demo: adds deduce and extend helpers plus a CLI mode
 * - Keeps compatibility with the simple demo
 * - Adds a small 'pipeline' function to demonstrate ascend/descend
 *
 * Compile as:
 *   gcc -std=c11 -O2 -o aurora_c_demo_extended aurora_c_demo_extended.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int Trit; // 0,1,-1 (None)

const char* trit_to_str(Trit t) { if (t==-1) return "N"; if (t==0) return "0"; return "1"; }

Trit trit_and(Trit a, Trit b){ if(a==0||b==0) return 0; if(a==1&&b==1) return 1; return -1; }
Trit trit_or(Trit a, Trit b){ if(a==1||b==1) return 1; if(a==0&&b==0) return 0; return -1; }
Trit trit_consensus(Trit a, Trit b){ if(a!=-1 && a==b) return a; return -1; }
Trit trigate_infer(Trit a,Trit b,Trit m){ if(m==0) return trit_and(a,b); if(m==1) return trit_or(a,b); return trit_consensus(a,b); }

typedef struct { Trit R[3]; Trit M[3]; Trit O[3]; } TensorFFE;

TensorFFE make_tensor(int r0,int r1,int r2,int m0,int m1,int m2,int o0,int o1,int o2){ TensorFFE t; t.R[0]=r0; t.R[1]=r1; t.R[2]=r2; t.M[0]=m0; t.M[1]=m1; t.M[2]=m2; t.O[0]=o0; t.O[1]=o1; t.O[2]=o2; return t; }
int count_nulls(const TensorFFE* t){ int c=0; for(int i=0;i<3;i++){ if(t->R[i]==-1) c++; if(t->M[i]==-1) c++; if(t->O[i]==-1) c++; } return c; }
void print_tensor(const char* name,const TensorFFE* t){ printf("%s: R=[%s,%s,%s] M=[%s,%s,%s] O=[%s,%s,%s] nulls=%d\n", name, trit_to_str(t->R[0]),trit_to_str(t->R[1]),trit_to_str(t->R[2]), trit_to_str(t->M[0]),trit_to_str(t->M[1]),trit_to_str(t->M[2]), trit_to_str(t->O[0]),trit_to_str(t->O[1]),trit_to_str(t->O[2]), count_nulls(t)); }

void vec_infer(const Trit A[3],const Trit B[3],const Trit M[3],Trit out[3]){ for(int i=0;i<3;i++) out[i]=trigate_infer(A[i],B[i],M[i]); }

// deduce B from A,M,R (simple rule)
void vec_deduce_b(const Trit A[3], const Trit M[3], const Trit R[3], Trit out[3]){
    for(int i=0;i<3;i++){
        Trit a=A[i]; Trit m=M[i]; Trit r=R[i];
        if(m==0){ if(r==1) out[i]=1; else out[i]=-1; }
        else if(m==1){ if(r==0) out[i]=0; else out[i]=-1; }
        else { out[i]=r; }
    }
}

TensorFFE synthesize(const TensorFFE* A,const TensorFFE* B,const TensorFFE* C){ TensorFFE M1,M2,M3,temp,Ms; vec_infer(A->R,B->R,A->M,M1.R); vec_infer(A->M,B->M,A->O,M1.M); vec_infer(A->O,B->O,(Trit[]){0,0,0},M1.O); vec_infer(B->R,C->R,B->M,M2.R); vec_infer(B->M,C->M,B->O,M2.M); vec_infer(B->O,C->O,(Trit[]){0,0,0},M2.O); vec_infer(C->R,A->R,C->M,M3.R); vec_infer(C->M,A->M,C->O,M3.M); vec_infer(C->O,A->O,(Trit[]){0,0,0},M3.O); vec_infer(M1.R,M2.R,M1.M,temp.R); vec_infer(M1.M,M2.M,M1.O,temp.M); vec_infer(M1.O,M2.O,(Trit[]){0,0,0},temp.O); vec_infer(temp.R,M3.R,temp.M,Ms.R); vec_infer(temp.M,M3.M,temp.O,Ms.M); vec_infer(temp.O,M3.O,(Trit[]){0,0,0},Ms.O); return Ms; }

// extend: project Ms to outputs using seeds
void extend_project(const TensorFFE* Ms, const TensorFFE* seed, TensorFFE* out){ vec_infer(Ms->R, seed->R, Ms->M, out->R); memcpy(out->M, Ms->M, sizeof(out->M)); memcpy(out->O, Ms->O, sizeof(out->O)); }

void simple_pipeline_demo(void){
    TensorFFE A=make_tensor(1,0,1, 0,0,1, 1,1,1);
    TensorFFE B=make_tensor(0,1,0, 1,1,1, 1,1,1);
    TensorFFE C=make_tensor(1,1,0, 0,1,0, 1,1,1);
    print_tensor("A",&A); print_tensor("B",&B); print_tensor("C",&C);
    TensorFFE Ms=synthesize(&A,&B,&C);
    print_tensor("Ms",&Ms);
    TensorFFE seed1=Ms; seed1.R[0]=Ms.R[1]; seed1.R[1]=Ms.R[2]; seed1.R[2]=Ms.R[0];
    TensorFFE out1,out2,out3; extend_project(&Ms,&seed1,&out1); extend_project(&Ms,&seed1,&out2); extend_project(&Ms,&seed1,&out3);
    print_tensor("OUT1",&out1); print_tensor("OUT2",&out2); print_tensor("OUT3",&out3);
}

int main(int argc,char** argv){
    printf("Aurora C extended demo\n");
    if(argc<2){ printf("Usage: aurora_c_demo_extended [demo|pipeline]\nRunning 'demo' by default...\n\n"); simple_pipeline_demo(); return 0; }
    if(strcmp(argv[1],"pipeline")==0){ simple_pipeline_demo(); return 0; }
    if(strcmp(argv[1],"demo")==0){ simple_pipeline_demo(); return 0; }
    printf("Unknown command '%s'\n", argv[1]); return 1; }
