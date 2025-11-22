#include <stdio.h>
#include <stdlib.h>

int main(void){
    int **memory = malloc(4096 * sizeof(int *)); //alocação de mem
    if (memory == NULL) return 1;

    for (int i = 0; i < 4096; i++) {
        memory[i] = malloc(12 * sizeof(int));
        if (memory[i] == NULL) return 1; 
        for (int j = 0; j < 12; j++) {
            memory[i][j] = 0;
        }
    }