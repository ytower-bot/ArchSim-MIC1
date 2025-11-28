#include <stdio.h>
#include <string.h>
#include "../../include/connections.h"

int tests_run = 0;
int tests_passed = 0;
int tests_failed = 0;

void print_test(const char* name, int passed) {
    tests_run++;
    if (passed) {
        tests_passed++;
        printf("✓ PASS: %s\n", name);
    } else {
        tests_failed++;
        printf("✗ FAIL: %s\n", name);
    }
}

int arrays_equal(const int* a, const int* b) {
    for (int i = 0; i < 16; i++) {
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}

void test_init_amux() {
    amux a;
    init_amux(&a);
    
    int passed = (a.control_amux == 0);
    print_test("init_amux: control_amux initialized to 0", passed);
}

void test_amux_select_latch_a() {
    amux a;
    mbr mb;
    latch lA;
    alu u;
    
    init_amux(&a);
    
    int latch_data[16] = {0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0};
    int mbr_data[16] =   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    
    for (int i = 0; i < 16; i++) {
        lA.data[i] = latch_data[i];
        mb.data[i] = mbr_data[i];
        u.input_a[i] = 0;
    }
    
    a.control_amux = 0;
    run_amux(&a, &mb, &lA, &u);
    
    int passed = arrays_equal(u.input_a, latch_data);
    print_test("run_amux: control=0 selects Latch A", passed);
}

void test_amux_select_mbr() {
    amux a;
    mbr mb;
    latch lA;
    alu u;
    
    init_amux(&a);
    
    int latch_data[16] = {0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0};
    int mbr_data[16] =   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    
    for (int i = 0; i < 16; i++) {
        lA.data[i] = latch_data[i];
        mb.data[i] = mbr_data[i];
        u.input_a[i] = 0;
    }
    
    a.control_amux = 1;
    run_amux(&a, &mb, &lA, &u);
    
    int passed = arrays_equal(u.input_a, mbr_data);
    print_test("run_amux: control=1 selects MBR", passed);
}

void test_amux_null_pointers() {
    amux a;
    mbr mb;
    latch lA;
    alu u;
    
    init_amux(&a);
    
    run_amux(NULL, &mb, &lA, &u);
    int passed = 1;
    print_test("run_amux: handles NULL amux pointer", passed);
    
    run_amux(&a, NULL, &lA, &u);
    print_test("run_amux: handles NULL mbr pointer", passed);
    
    run_amux(&a, &mb, NULL, &u);
    print_test("run_amux: handles NULL latch pointer", passed);
    
    run_amux(&a, &mb, &lA, NULL);
    print_test("run_amux: handles NULL alu pointer", passed);
}

int main() {
    printf("\n=== AMUX Tests ===\n\n");
    
    test_init_amux();
    test_amux_select_latch_a();
    test_amux_select_mbr();
    test_amux_null_pointers();
    
    printf("\n=== Test Summary ===\n");
    printf("Total:  %d\n", tests_run);
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    
    if (tests_failed == 0) {
        printf("\n✅ All tests passed!\n\n");
        return 0;
    } else {
        printf("\n❌ Some tests failed!\n\n");
        return 1;
    }
}
