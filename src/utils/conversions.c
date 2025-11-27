#include "../../include/utils/conversions.h"
#include <stdio.h>

/**
 * Converte um array de bits (int[16]) para um inteiro usando complemento de 2.
 * 
 * @param arr Array de 16 bits onde arr[0] é o bit mais significativo (sinal)
 * @return Valor inteiro correspondente ao array de bits
 * 
 * Exemplo: [0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0] = 10
 *          [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1] = -1
 */
int array_to_int(int arr[16]) {
    int result = 0;
    
    // Se bit de sinal (arr[0]) for 1, número é negativo
    if (arr[0] == 1) {
        // Para números negativos em complemento de 2:
        // 1. Converter bits para positivo
        // 2. Subtrair de 2^16
        for (int i = 0; i < 16; i++) {
            result = result * 2 + arr[i];
        }
        // Ajustar para complemento de 2 (16 bits com sinal)
        result = result - 65536;  // 2^16
    } else {
        // Número positivo: conversão direta
        for (int i = 0; i < 16; i++) {
            result = result * 2 + arr[i];
        }
    }
    
    return result;
}

/**
 * Converte um inteiro para array de bits (int[16]) usando complemento de 2.
 * 
 * @param value Valor inteiro a ser convertido
 * @param arr Array de 16 bits que será preenchido (output)
 * 
 * Exemplo: 10 -> [0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0]
 *          -1 -> [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]
 */
void int_to_array(int value, int arr[16]) {
    // Para números negativos, converter para representação de complemento de 2
    unsigned int uvalue;
    
    if (value < 0) {
        // Converter para complemento de 2 em 16 bits
        uvalue = (unsigned int)(value + 65536);  // 2^16
    } else {
        uvalue = (unsigned int)value;
    }
    
    // Extrair bits da direita para esquerda
    for (int i = 15; i >= 0; i--) {
        arr[i] = uvalue & 1;  // Pega o bit menos significativo
        uvalue >>= 1;          // Desloca para direita
    }
}

/**
 * Copia um array de bits para outro.
 * 
 * @param src Array de origem (16 bits)
 * @param dest Array de destino (16 bits)
 */
void copy_array(int src[16], int dest[16]) {
    for (int i = 0; i < 16; i++) {
        dest[i] = src[i];
    }
}

/**
 * Imprime um array de bits em formato binário para debug.
 * 
 * @param arr Array de 16 bits a ser impresso
 * 
 * Exemplo de saída: [1010000000000001]
 */
void print_array(int arr[16]) {
    printf("[");
    for (int i = 0; i < 16; i++) {
        printf("%d", arr[i]);
    }
    printf("]");
}

/**
 * Imprime um array de bits com informações detalhadas para debug.
 * 
 * @param arr Array de 16 bits
 * @param label Texto descritivo (pode ser NULL)
 * 
 * Exemplo de saída:
 * Array test: [1010000000000001] = -24575 (decimal)
 */
void print_array_debug(int arr[16], const char* label) {
    if (label != NULL) {
        printf("%s: ", label);
    }
    
    print_array(arr);
    printf(" = %d (decimal)\n", array_to_int(arr));
}
