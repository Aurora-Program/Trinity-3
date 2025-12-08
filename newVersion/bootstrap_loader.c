#include "bootstrap_loader.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Heurísticas mínimas para derivar FO/FN/ES: menos es más
 * FO: longitud (<=4 ->0, 5-7 ->1, >=8 -> -1)
 * FN: primera letra vocal ->1, consonante ->0, otro -> -1
 * ES: tipo aproximado: termina en ar/er/ir ->1 (verbo), termina en o/a/os/as ->0, otro -> -1
 */

static BootWord store[1024];
static size_t store_n = 0;

static int is_vowel(char c){ c=(char)tolower((unsigned char)c); return c=='a'||c=='e'||c=='i'||c=='o'||c=='u'; }

static void derive_triple(const char* w,int out[3]){
    size_t L=strlen(w);
    out[0] = (L<=4)?0: (L<=7?1:-1);
    out[1] = is_vowel(w[0])?1: (isalpha((unsigned char)w[0])?0:-1);
    if(L>=2){
        const char* end = w+L-2;
        if((strcmp(end,"ar")==0)||(strcmp(end,"er")==0)||(strcmp(end,"ir")==0)) out[2]=1;
        else if(L>=1){
            char c=w[L-1];
            if(c=='o'||c=='a') out[2]=0;
            else if(L>=2 && ((w[L-2]=='o'&&c=='s')||(w[L-2]=='a'&&c=='s'))) out[2]=0;
            else out[2]=-1;
        } else out[2]=-1;
    } else out[2]=-1;
}

size_t load_bootstrap_words(const char* path){
    FILE* f=fopen(path?path:"bootstrap_es_1000.txt","r");
    if(!f) return 0;
    store_n=0; char line[128];
    while(fgets(line,sizeof(line),f)&&store_n<1024){
        char* nl=strchr(line,'\n'); if(nl) *nl='\0';
        if(!*line) continue;
        BootWord *bw=&store[store_n++];
        strncpy(bw->word,line,sizeof(bw->word)-1); bw->word[sizeof(bw->word)-1]='\0';
        derive_triple(bw->word,bw->ffe);
    }
    fclose(f);
    return store_n;
}

size_t get_bootstrap_count(void){ return store_n; }
const BootWord* get_bootstrap_word(size_t i){ return (i<store_n)? &store[i] : NULL; }
