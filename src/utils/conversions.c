#include "../../include/utils/conversions.h"
#include <stdio.h>

int array_to_int(int arr[16]) {
    int result = 0;

    if (arr[0] == 1) {

        for (int i = 0; i < 16; i++) {
            result = result * 2 + arr[i];
        }

        result = result - 65536;
    } else {

        for (int i = 0; i < 16; i++) {
            result = result * 2 + arr[i];
        }
    }

    return result;
}

void int_to_array(int value, int arr[16]) {

    unsigned int uvalue;

    if (value < 0) {

        uvalue = (unsigned int)(value + 65536);
    } else {
        uvalue = (unsigned int)value;
    }

    for (int i = 15; i >= 0; i--) {
        arr[i] = uvalue & 1;
        uvalue >>= 1;
    }
}

void copy_array(int src[16], int dest[16]) {
    for (int i = 0; i < 16; i++) {
        dest[i] = src[i];
    }
}

void print_array(int arr[16]) {
    printf("[");
    for (int i = 0; i < 16; i++) {
        printf("%d", arr[i]);
    }
    printf("]");
}

void print_array_debug(int arr[16], const char* label) {
    if (label != NULL) {
        printf("%s: ", label);
    }

    print_array(arr);
    printf(" = %d (decimal)\n", array_to_int(arr));
}

int bits_to_int(int bits[], int size) {

    if (!bits || size <= 0) {
        return 0;
    }

    int result = 0;

    for (int i = 0; i < size; i++) {
        result = result * 2 + bits[i];
    }

    return result;
}

void int_to_bits(int value, int bits[], int size) {

    if (!bits || size <= 0) {
        return;
    }

    for (int i = size - 1; i >= 0; i--) {
        bits[i] = value & 1;
        value = value >> 1;
    }
}

int address_to_int(int address[12]) {

    if (!address) {
        return 0;
    }

    return bits_to_int(address, 12);
}

void int_to_address(int addr, int address[12]) {

    if (!address) {
        return;
    }

    int_to_bits(addr, address, 12);
}

void copy_data(int dest[16], int src[16]) {

    if (!dest || !src) {
        return;
    }

    for (int i = 0; i < 16; i++) {
        dest[i] = src[i];
    }
}
