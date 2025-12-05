#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#include "../datapath.h"

int array_to_int(int arr[16]);
void int_to_array(int value, int arr[16]);
void copy_array(int src[16], int dest[16]);
void print_array(int arr[16]);
void print_array_debug(int arr[16], const char* label);

int bits_to_int(int bits[], int size);
void int_to_bits(int value, int bits[], int size);

int address_to_int(int address[12]);
void int_to_address(int addr, int address[12]);

void copy_data(int dest[16], int src[16]);

int array_to_binary(mic1_register);

#endif