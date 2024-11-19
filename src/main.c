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

#include <bitmap.h>

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
 * \return Noise value.
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
 * Calculates the noise at a given position at several octaves, taking into
 * account both frequency and amplitude.
 * 
 * \param[in] x Horizontal position.
 * \param[in] y Vertical position.
 * \param[in] z Depth position.
 * \param[in] octaves Number of octaves.
 * \return Noise value.
 */
double fractal_brownian_motion(double x, double y, double z, unsigned octaves) {
    double result = 0.0;
    double maximum = 0.0;
    double frequency = 1.0;
    double amplitude = 1.0;

    for (unsigned i = 0; i < octaves; ++i) {
        result += perlin_noise(x * frequency, y * frequency, z * frequency) *
            amplitude;
        maximum += amplitude;

        frequency *= 2.0;
        amplitude /= 2.0;
    }

    return result / maximum;
}

/**
 * Entry point for the program.
 * 
 * \return Exit code.
 */
int main() {
    srand((unsigned) time(NULL));

    const unsigned width = 100;
    const unsigned height = 100;

    struct bitmap bitmap = { 0 };

    bitmap.file_header.signature[0] = 'B';
    bitmap.file_header.signature[1] = 'M';
    bitmap.file_header.file_size = 54 + (width * height * 3);
    bitmap.file_header.offset = 54;

    bitmap.dib_header.header_size = 40;
    bitmap.dib_header.width = width;
    bitmap.dib_header.height = height;
    bitmap.dib_header.color_planes = 1;
    bitmap.dib_header.bits_per_pixel = 24;
    bitmap.dib_header.horizontal_resolution = 3780;
    bitmap.dib_header.vertical_resolution = 3780;

    bitmap.pixels = malloc(width * height * 3);
    
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            double noise_x = ((double) x) / ((double) width);
            double noise_y = ((double) y) / ((double) height);
            double noise_value = fractal_brownian_motion(noise_x, noise_y, 0.0,
                12);

            struct pixel pixel = {
                (uint8_t) ((noise_value + 1.0) * 128.0),
                (uint8_t) ((noise_value + 1.0) * 128.0),
                (uint8_t) ((noise_value + 1.0) * 128.0)
            };

            bitmap.pixels[x + (y * width)] = pixel;
        }
    }

    FILE* bitmap_file = fopen("bin/noise.bmp", "wb");
    if (!bitmap_file) {
        printf("[ERROR] Failed to create bitmap.\n");
        return EXIT_FAILURE;
    }

    fwrite(bitmap.file_header.signature, 1, 2, bitmap_file);
    fwrite(&bitmap.file_header.file_size, 4, 1, bitmap_file);
    fwrite(&bitmap.file_header.reserved1, 2, 1, bitmap_file);
    fwrite(&bitmap.file_header.reserved1, 2, 1, bitmap_file);
    fwrite(&bitmap.file_header.offset, 4, 1, bitmap_file);

    fwrite(&bitmap.dib_header.header_size, 4, 1, bitmap_file);
    fwrite(&bitmap.dib_header.width, 4, 1, bitmap_file);
    fwrite(&bitmap.dib_header.height, 4, 1, bitmap_file);
    fwrite(&bitmap.dib_header.color_planes, 2, 1, bitmap_file);
    fwrite(&bitmap.dib_header.bits_per_pixel, 2, 1, bitmap_file);
    fwrite(&bitmap.dib_header.compression, 4, 1, bitmap_file);
    fwrite(&bitmap.dib_header.raw_data_size, 4, 1, bitmap_file);
    fwrite(&bitmap.dib_header.horizontal_resolution, 4, 1, bitmap_file);
    fwrite(&bitmap.dib_header.vertical_resolution, 4, 1, bitmap_file);
    fwrite(&bitmap.dib_header.color_table_entries, 4, 1, bitmap_file);
    fwrite(&bitmap.dib_header.important_colors, 4, 1, bitmap_file);

    fwrite(bitmap.pixels, 3, width * height, bitmap_file);

    fclose(bitmap_file);
    free(bitmap.pixels);
    
    return EXIT_SUCCESS;
}
