#ifndef TEST_H
#define TEST_H
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include "types.h"

// ─────────────────────────────────────────
//  Helpers
// ─────────────────────────────────────────

#define PASS "\033[32m[PASS]\033[0m"
#define FAIL "\033[31m[FAIL]\033[0m"
#define SECTION(name) printf("\n=== %s ===\n", name)

/**
 * This function checks if two floating-point numbers are approximately equal within a specified tolerance.
 * @param a: The first floating-point number
 * @param b: The second floating-point number
 * @param tol: The tolerance for comparison (e.g., 1e-5)
 * @return 1 if the numbers are approximately equal, 0 otherwise
 */
static int float_eq(f32 a, f32 b, f32 tol) {
    return fabsf(a - b) <= tol;
}

// variables for tracking test results
static int tests_passed = 0;
static int tests_failed = 0;

/**
 * This function checks a condition and prints the result of a test case.
 * @param name: A description of the test case
 * @param condition: The condition to check (1 for pass, 0 for fail)
 */
static void check(const char *name, int condition) {
    if (condition) {
        printf("%s %s\n", PASS, name);
        tests_passed++;
    } else {
        printf("%s %s\n", FAIL, name);
        tests_failed++;
    }
}


// ─────────────────────────────────────────
//  Loading animation
// ─────────────────────────────────────────

void loading(const char *section) {
    const char spinner[] = "|/-\\";
    
    printf("\n=== %s ===\n", section);

    // animação
    for (int i = 0; i < 16; i++) {
        printf("\r \033[34m[TESTING]\033[0m %s %c", section, spinner[i % 4]);
        fflush(stdout);
        usleep(90000); 
    }

    printf("\r\033[K");
}
#endif // TEST_H