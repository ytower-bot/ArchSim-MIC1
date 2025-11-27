#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#include "../datapath.h"

// Funções de conversão entre arrays de bits e inteiros
int array_to_int(int arr[16]);
void int_to_array(int value, int arr[16]);
void copy_array(int src[16], int dest[16]);
void print_array(int arr[16]);
void print_array_debug(int arr[16], const char* label);

// Função legacy (pode ser removida/refatorada)
int array_to_binary(mic1_register);

#endif