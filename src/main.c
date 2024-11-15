/**
 * \file main.c
 * \author Isaiah Lateer
 * 
 * Creates a bitmap of generated noise.
 */

#include <math.h>
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
 * Calculates the fade over time.
 * 
 * \param[in] t Time.
 * \return Fade.
 */
double fade(double t) {
    return pow(t, 3.0) * (t * (t * 6.0 - 15.0) + 10.0);
}

/**
 * Calculates the linear interpolation between two values over time.
 * 
 * \param[in] t Value used to interpolate between a and b.
 * \param[in] a Starting value.
 * \param[in] b Ending value.
 * \return Linear interpolation.
 */
double lerp(double t, double a, double b) {
    return (1.0 - t) * a + t * b;
}

/**
 * Calculates the gradient at a given position.
 * 
 * \param[in] hash Hash value.
 * \param[in] x Horizontal position.
 * \param[in] y Vertical position.
 * \param[in] z Depth position.
 * \return Gradient.
 */
double grad(int hash, double x, double y, double z) {
    int h = hash & 15;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

/**
 * Calculates the noise at a given position.
 * 
 * \param[in] x Horizontal position.
 * \param[in] y Vertical position.
 * \param[in] z Depth position.
 */
double perlin_noise(double x, double y, double z) {
    int perm[512] = { 0 };

    for (int i = 0; i < 256; ++i) {
        perm[i] = i;
    }

    shuffle(perm, 256);

    for (int i = 256; i < 512; ++i) {
        perm[i] = perm[i - 256];
    }

    int X = ((int) floor(x)) & 255;
    int Y = ((int) floor(y)) & 255;
    int Z = ((int) floor(y)) & 255;

    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    double u = fade(x);
    double v = fade(y);
    double w = fade(z);

    int A = perm[X] + Y;
    int AA = perm[A] + Z;
    int AB = perm[A + 1] + Z;
    int B = perm[X + 1] + Y;
    int BA = perm[B] + Z;
    int BB = perm[B + 1] + Z;

    return lerp(w, lerp(v, lerp(u, grad(perm[AA], x, y, z), grad(perm[BA],
        x - 1.0, y, z)), lerp(u, grad(perm[AB], x, y - 1.0, z), grad(perm[BB],
        x - 1.0, y - 1.0, z))), lerp(v, lerp(u, grad(perm[AA + 1], x, y,
        z - 1.0), grad(perm[BA + 1], x - 1.0, y, z - 1.0)), lerp(u,
        grad(perm[AB + 1], x, y - 1.0, z - 1.0), grad(perm[BB + 1], x - 1,
        y - 1, z - 1))));
}

/**
 * Entry point for the program.
 * 
 * \return Exit code.
 */
int main() {
    srand((unsigned) time(NULL));

    const unsigned width = 20;
    const unsigned height = 20;
    
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            double noise_x = ((double) x) / ((double) width);
            double noise_y = ((double) y) / ((double) height);
            double noise_value = perlin_noise(noise_x, noise_y, 0.0);

            if (noise_value >= 0.0) {
                printf(" ");
            }

            printf("%.2lf", noise_value);

            if (x < width - 1) {
                printf(" ");
            }
        }

        printf("\n");
    }
    
    return EXIT_SUCCESS;
}
