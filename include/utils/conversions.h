#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#include "../datapath.h"

// Funções de conversão entre arrays de bits e inteiros (16 bits fixos)
int array_to_int(int arr[16]);
void int_to_array(int value, int arr[16]);
void copy_array(int src[16], int dest[16]);
void print_array(int arr[16]);
void print_array_debug(int arr[16], const char* label);

// Funções de conversão de tamanho variável (FASE_1)
int bits_to_int(int bits[], int size);
void int_to_bits(int value, int bits[], int size);

// Funções específicas para endereços de 12 bits (FASE_1)
int address_to_int(int address[12]);
void int_to_address(int addr, int address[12]);

// Função de cópia de dados (FASE_1)
void copy_data(int dest[16], int src[16]);

// Função legacy (pode ser removida/refatorada)
int array_to_binary(mic1_register);

#endif