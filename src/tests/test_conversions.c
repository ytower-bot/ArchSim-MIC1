#include <stdio.h>
#include "../include/utils/conversions.h"

int main(void) {
    printf("═══════════════════════════════════════════════════\n");
    printf("  Teste de Funções de Conversão - MIC-1\n");
    printf("═══════════════════════════════════════════════════\n\n");
    
    // Teste 1: Conversão de inteiro para array
    printf("Teste 1: int_to_array\n");
    printf("---------------------\n");
    
    int arr[16];
    
    int_to_array(10, arr);
    printf("10 decimal = ");
    print_array_debug(arr, NULL);
    
    int_to_array(-1, arr);
    printf("-1 decimal = ");
    print_array_debug(arr, NULL);
    
    int_to_array(255, arr);
    printf("255 decimal = ");
    print_array_debug(arr, NULL);
    
    int_to_array(-128, arr);
    printf("-128 decimal = ");
    print_array_debug(arr, NULL);
    
    printf("\n");
    
    // Teste 2: Conversão de array para inteiro
    printf("Teste 2: array_to_int\n");
    printf("---------------------\n");
    
    int test1[16] = {0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0}; // 10
    printf("Array: ");
    print_array_debug(test1, NULL);
    
    int test2[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; // -1
    printf("Array: ");
    print_array_debug(test2, NULL);
    
    int test3[16] = {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1}; // 255
    printf("Array: ");
    print_array_debug(test3, NULL);
    
    int test4[16] = {1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0}; // -128
    printf("Array: ");
    print_array_debug(test4, NULL);
    
    printf("\n");
    
    // Teste 3: Cópia de array
    printf("Teste 3: copy_array\n");
    printf("-------------------\n");
    
    int src[16] = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};
    int dest[16];
    
    printf("Origem: ");
    print_array(src);
    printf("\n");
    
    copy_array(src, dest);
    
    printf("Destino (após cópia): ");
    print_array(dest);
    printf("\n");
    
    printf("\n");
    
    // Teste 4: Round-trip (ida e volta)
    printf("Teste 4: Round-trip (int → array → int)\n");
    printf("---------------------------------------\n");
    
    int valores[] = {0, 1, 10, 100, -1, -10, -100, 255, -255};
    int num_testes = sizeof(valores) / sizeof(valores[0]);
    
    for (int i = 0; i < num_testes; i++) {
        int original = valores[i];
        int_to_array(original, arr);
        int resultado = array_to_int(arr);
        
        printf("%4d → ", original);
        print_array(arr);
        printf(" → %4d ", resultado);
        
        if (original == resultado) {
            printf("✓\n");
        } else {
            printf("✗ (ERRO!)\n");
        }
    }
    
    printf("\n═══════════════════════════════════════════════════\n");
    printf("  Testes concluídos!\n");
    printf("═══════════════════════════════════════════════════\n");
    
    return 0;
}
