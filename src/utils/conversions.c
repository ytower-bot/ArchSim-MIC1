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

/**
 * Converte array de bits de tamanho variável para inteiro.
 * 
 * @param bits Array de bits onde bits[0] é o MSB (Most Significant Bit)
 * @param size Tamanho do array (quantidade de bits)
 * @return Valor inteiro correspondente ao array de bits
 * 
 * Exemplo: bits[] = {0,0,1,0}, size=4 → retorna 2
 *          bits[] = {0,1,0,1}, size=4 → retorna 5
 *          bits[] = {1,1,1,1}, size=4 → retorna 15
 * 
 * Endianess: bit[0] = MSB, bit[size-1] = LSB
 */
int bits_to_int(int bits[], int size) {
    // Validação
    if (!bits || size <= 0) {
        return 0;
    }
    
    int result = 0;
    
    // Iterar do MSB (bits[0]) até LSB (bits[size-1])
    for (int i = 0; i < size; i++) {
        result = result * 2 + bits[i];
    }
    
    return result;
}

/**
 * Converte inteiro para array de bits de tamanho variável.
 * 
 * @param value Valor inteiro a ser convertido
 * @param bits Array de bits que será preenchido (output)
 * @param size Tamanho do array (quantidade de bits)
 * 
 * Exemplo: value=5, size=4 → bits[] = {0,1,0,1}
 *          value=15, size=4 → bits[] = {1,1,1,1}
 *          value=2, size=4 → bits[] = {0,0,1,0}
 * 
 * Endianess: bit[0] = MSB, bit[size-1] = LSB
 */
void int_to_bits(int value, int bits[], int size) {
    // Validação
    if (!bits || size <= 0) {
        return;
    }
    
    // Extrair bits da direita para esquerda (LSB para MSB)
    for (int i = size - 1; i >= 0; i--) {
        bits[i] = value & 1;  // Pega o bit menos significativo
        value = value >> 1;    // Desloca para direita
    }
}

/**
 * Converte endereço de 12 bits para inteiro.
 * 
 * @param address Array de 12 bits representando um endereço
 * @return Valor inteiro de 0 a 4095 (0x000 - 0xFFF)
 * 
 * Exemplo: address[] = {0,0,0,0,0,0,0,0,0,0,0,0} → 0
 *          address[] = {1,1,1,1,1,1,1,1,1,1,1,1} → 4095
 *          address[] = {0,0,0,0,0,0,0,1,0,1,0,0} → 20
 * 
 * Endianess: address[0] = MSB, address[11] = LSB
 */
int address_to_int(int address[12]) {
    // Validação
    if (!address) {
        return 0;
    }
    
    // Usar bits_to_int para converter 12 bits
    return bits_to_int(address, 12);
}

/**
 * Converte inteiro para endereço de 12 bits.
 * 
 * @param addr Valor inteiro de 0 a 4095 (0x000 - 0xFFF)
 * @param address Array de 12 bits que será preenchido (output)
 * 
 * Exemplo: addr=0 → address[] = {0,0,0,0,0,0,0,0,0,0,0,0}
 *          addr=4095 → address[] = {1,1,1,1,1,1,1,1,1,1,1,1}
 *          addr=20 → address[] = {0,0,0,0,0,0,0,1,0,1,0,0}
 * 
 * Endianess: address[0] = MSB, address[11] = LSB
 */
void int_to_address(int addr, int address[12]) {
    // Validação
    if (!address) {
        return;
    }
    
    // Usar int_to_bits para converter para 12 bits
    int_to_bits(addr, address, 12);
}

/**
 * Copia dados de um array de 16 bits para outro.
 * 
 * @param dest Array de destino (16 bits)
 * @param src Array de origem (16 bits)
 * 
 * Implementação: Usa memcpy para performance ou loop manual
 */
void copy_data(int dest[16], int src[16]) {
    // Validação
    if (!dest || !src) {
        return;
    }
    
    // Copiar usando loop (equivalente a copy_array existente)
    for (int i = 0; i < 16; i++) {
        dest[i] = src[i];
    }
}
