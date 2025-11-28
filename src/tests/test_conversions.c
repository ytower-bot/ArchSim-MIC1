#include <stdio.h>
#include "../../include/utils/conversions.h"

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
    
    printf("\n");
    
    // Teste 5: bits_to_int (tamanho variável)
    printf("Teste 5: bits_to_int (FASE_1)\n");
    printf("------------------------------\n");
    
    int bits4[] = {0,0,1,0}; // 2
    int bits4_2[] = {0,1,0,1}; // 5
    int bits4_3[] = {1,1,1,1}; // 15
    int bits8[] = {1,1,1,1,1,1,1,1}; // 255
    
    printf("bits[4] = {0,0,1,0} → %d (esperado: 2) ", bits_to_int(bits4, 4));
    printf(bits_to_int(bits4, 4) == 2 ? "✓\n" : "✗\n");
    
    printf("bits[4] = {0,1,0,1} → %d (esperado: 5) ", bits_to_int(bits4_2, 4));
    printf(bits_to_int(bits4_2, 4) == 5 ? "✓\n" : "✗\n");
    
    printf("bits[4] = {1,1,1,1} → %d (esperado: 15) ", bits_to_int(bits4_3, 4));
    printf(bits_to_int(bits4_3, 4) == 15 ? "✓\n" : "✗\n");
    
    printf("bits[8] = {1,1,1,1,1,1,1,1} → %d (esperado: 255) ", bits_to_int(bits8, 8));
    printf(bits_to_int(bits8, 8) == 255 ? "✓\n" : "✗\n");
    
    printf("\n");
    
    // Teste 6: int_to_bits (tamanho variável)
    printf("Teste 6: int_to_bits (FASE_1)\n");
    printf("------------------------------\n");
    
    int result_bits[16];
    
    int_to_bits(5, result_bits, 4);
    printf("5 → bits[4] = [%d,%d,%d,%d] (esperado: [0,1,0,1]) ", 
           result_bits[0], result_bits[1], result_bits[2], result_bits[3]);
    printf((result_bits[0]==0 && result_bits[1]==1 && result_bits[2]==0 && result_bits[3]==1) ? "✓\n" : "✗\n");
    
    int_to_bits(15, result_bits, 4);
    printf("15 → bits[4] = [%d,%d,%d,%d] (esperado: [1,1,1,1]) ", 
           result_bits[0], result_bits[1], result_bits[2], result_bits[3]);
    printf((result_bits[0]==1 && result_bits[1]==1 && result_bits[2]==1 && result_bits[3]==1) ? "✓\n" : "✗\n");
    
    int_to_bits(2, result_bits, 4);
    printf("2 → bits[4] = [%d,%d,%d,%d] (esperado: [0,0,1,0]) ", 
           result_bits[0], result_bits[1], result_bits[2], result_bits[3]);
    printf((result_bits[0]==0 && result_bits[1]==0 && result_bits[2]==1 && result_bits[3]==0) ? "✓\n" : "✗\n");
    
    printf("\n");
    
    // Teste 7: address_to_int (12 bits)
    printf("Teste 7: address_to_int (FASE_1)\n");
    printf("---------------------------------\n");
    
    int addr_zero[12] = {0,0,0,0,0,0,0,0,0,0,0,0}; // 0
    int addr_max[12] = {1,1,1,1,1,1,1,1,1,1,1,1}; // 4095
    int addr_20[12] = {0,0,0,0,0,0,0,1,0,1,0,0}; // 20
    
    printf("address[12] = {0,0,0,0,0,0,0,0,0,0,0,0} → %d (esperado: 0) ", address_to_int(addr_zero));
    printf(address_to_int(addr_zero) == 0 ? "✓\n" : "✗\n");
    
    printf("address[12] = {1,1,1,1,1,1,1,1,1,1,1,1} → %d (esperado: 4095) ", address_to_int(addr_max));
    printf(address_to_int(addr_max) == 4095 ? "✓\n" : "✗\n");
    
    printf("address[12] = {0,0,0,0,0,0,0,1,0,1,0,0} → %d (esperado: 20) ", address_to_int(addr_20));
    printf(address_to_int(addr_20) == 20 ? "✓\n" : "✗\n");
    
    printf("\n");
    
    // Teste 8: int_to_address (12 bits)
    printf("Teste 8: int_to_address (FASE_1)\n");
    printf("---------------------------------\n");
    
    int result_addr[12];
    
    int_to_address(0, result_addr);
    printf("0 → address[12] = all zeros ");
    int all_zeros = 1;
    for (int i = 0; i < 12; i++) {
        if (result_addr[i] != 0) all_zeros = 0;
    }
    printf(all_zeros ? "✓\n" : "✗\n");
    
    int_to_address(4095, result_addr);
    printf("4095 → address[12] = all ones ");
    int all_ones = 1;
    for (int i = 0; i < 12; i++) {
        if (result_addr[i] != 1) all_ones = 0;
    }
    printf(all_ones ? "✓\n" : "✗\n");
    
    int_to_address(20, result_addr);
    printf("20 → address[12] = {0,0,0,0,0,0,0,1,0,1,0,0} ");
    int addr_20_correct = (result_addr[0]==0 && result_addr[1]==0 && result_addr[2]==0 && 
                           result_addr[3]==0 && result_addr[4]==0 && result_addr[5]==0 && 
                           result_addr[6]==0 && result_addr[7]==1 && result_addr[8]==0 && 
                           result_addr[9]==1 && result_addr[10]==0 && result_addr[11]==0);
    printf(addr_20_correct ? "✓\n" : "✗\n");
    
    printf("\n");
    
    // Teste 9: copy_data (16 bits)
    printf("Teste 9: copy_data (FASE_1)\n");
    printf("----------------------------\n");
    
    int src_data[16] = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};
    int dest_data[16] = {0};
    
    printf("Antes da cópia - dest: ");
    print_array(dest_data);
    printf("\n");
    
    copy_data(dest_data, src_data);
    
    printf("Após cópia - dest: ");
    print_array(dest_data);
    printf("\n");
    
    int copy_ok = 1;
    for (int i = 0; i < 16; i++) {
        if (dest_data[i] != src_data[i]) copy_ok = 0;
    }
    printf("Cópia correta: %s\n", copy_ok ? "✓" : "✗");
    
    printf("\n═══════════════════════════════════════════════════\n");
    printf("  Testes concluídos!\n");
    printf("═══════════════════════════════════════════════════\n");
    
    return 0;
}
