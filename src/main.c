/**
 * \file main.c
 * \author Isaiah Lateer
 * 
 * Creates a bitmap of generated noise.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * Swaps two integers.
 * 
 * \param[in] a First value.
 * \param[in] b Second value.
 */
void swap(int* a, int* b) {
    int c = *a;
    *a = *b;
    *b = c;
}

/**
 * Shuffles the values in an integer array.
 * 
 * \param[in] array Array to shuffle.
 * \param[in] size Number of elements in the array.
 */
void shuffle(int* array, size_t size) {
    for (size_t i = size - 1; i > 0; --i) {
        size_t j = rand() % (i + 1);
        swap(&array[i], &array[j]);
    }
}

/**
 * Entry point for the program.
 * 
 * \return Exit code.
 */
int main() {
    srand((unsigned) time(NULL));
    
    int perm[512] = { 0 };

    for (int i = 0; i < 256; ++i) {
        perm[i] = i;
    }

    shuffle(perm, 256);

    for (int i = 256; i < 512; ++i) {
        perm[i] = perm[i - 256];
    }

    for (int i = 0; i < 512; ++i) {
        printf("%3d", perm[i]);

        if (i < 511) {
            printf(",");
        }
        
        if ((i + 1) % 16 == 0) {
            printf("\n");
        } else {
            printf(" ");
        }
    }
    
    return EXIT_SUCCESS;
}
