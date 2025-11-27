#include <stdio.h>

int returnadd(int instr[], int addstart){
    int addresscount = 2048; //valor do bit mais relevante
    int result = 0; //endereço final
    for (int i = addstart; i < 16 ; i++) {
        result += instr[i] * addresscount; //adiciona o valor do bit atual se for 1
        addresscount = addresscount / 2;
    }
    return result;
}

