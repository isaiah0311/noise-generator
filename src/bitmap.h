/**
 * \file bitmap.h
 * \author Isaiah Lateer
 * 
 * Contains data pertaining to bitmap files such as headers and pixel data.
 */

#ifndef NOISE_GENERATOR_BITMAP_HEADER
#define NOISE_GENERATOR_BITMAP_HEADER

#include <stdint.h>

struct bitmap_file_header {
    char signature[2];
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
};

struct bitmap_dib_header {
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t color_planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t raw_data_size;
    int32_t horizontal_resolution;
    int32_t vertical_resolution;
    uint32_t color_table_entries;
    uint32_t important_colors;
};

struct pixel {
    uint8_t b;
    uint8_t g;
    uint8_t r;
};

struct bitmap {
    struct bitmap_file_header file_header;
    struct bitmap_dib_header dib_header;
    struct pixel* pixels;
};

#endif
