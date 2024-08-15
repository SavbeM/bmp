#include <stdio.h>
#include "transformations.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

uint32_t compute_new_size(const struct bmp_header* header) {
    if (header == NULL) {
        return 0;
    }
    uint32_t row_size = header->width * header->bpp / 8;
    uint32_t padding = (4 - (row_size % 4)) % 4;
    uint32_t pixels_size = (row_size + padding) * header->height;

    return sizeof(struct bmp_header) + pixels_size;
}

size_t row_size_in_bytes(size_t width, size_t bpp) {

    uint32_t row_size = width * bpp / 8;

    return row_size;
}

struct bmp_image *flip_horizontally(const struct bmp_image *image) {
    if (image == NULL) {
        fprintf(stderr, "Error: Image struct is empty");
        return NULL;
    }

    int pixels_count = image->header->height * image->header->width;

    struct bmp_image *transformed = calloc(1, sizeof(struct bmp_image));
    transformed->data = calloc(pixels_count, sizeof(struct pixel));

    transformed->header = calloc(1, sizeof(struct bmp_header));
    *(transformed->header) = *(image->header);

    int ptr = 0;
    for (int row = 0; row < image->header->height; ++row) {
        int reversed_ptr = ptr + image->header->width - 1;
        int current = ptr;
        for (int col = reversed_ptr; col >= current; --col) {
            transformed->data[ptr] = image->data[col];
            ptr++;
        }
    }

    return transformed;
}

struct bmp_image *flip_vertically(const struct bmp_image *image) {
    if (image == NULL) {
        fprintf(stderr, "Error: Image struct is empty");
        return NULL;
    }

    int pixels_count = image->header->height * image->header->width;
    struct bmp_image *transformed = calloc(1, sizeof(struct bmp_image));
    transformed->data = calloc(pixels_count, sizeof(struct pixel));
    transformed->header = calloc(1, sizeof(struct bmp_header));
    *(transformed->header) = *(image->header);

    for (int y = 0; y < image->header->height; y++) {
        for (int x = 0; x < image->header->width; x++) {
            int index = y * image->header->width + x;
            int flipped_index = (image->header->height - y - 1) * image->header->width + x;
            transformed->data[flipped_index] = image->data[index];
        }
    }

    return transformed;
}


struct bmp_image *rotate_left(const struct bmp_image *image) {
    if (image == NULL) {
        fprintf(stderr, "Error: Image struct is empty");
        return NULL;
    }

    int pixels_count = image->header->height * image->header->width;
    struct bmp_image *transformed = calloc(1, sizeof(struct bmp_image));
    transformed->data = calloc(pixels_count, sizeof(struct pixel));

    int width = image->header->width;
    int height = image->header->height;
    transformed->header = calloc(1, sizeof(struct bmp_header));
    *(transformed->header) = *(image->header);
    transformed->header->width = height;
    transformed->header->height = width;
    transformed->header->size = compute_new_size(transformed->header);
    transformed->header->image_size = transformed->header->size - sizeof (struct bmp_header);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            int rotated_index = x * height + (height - y - 1);
            transformed->data[rotated_index] = image->data[index];
        }
    }


    return transformed;
}

struct bmp_image *rotate_right(const struct bmp_image *image) {
    if (image == NULL) {
        fprintf(stderr, "Error: Image struct is empty");
        return NULL;
    }

    int pixels_count = image->header->height * image->header->width;
    struct bmp_image *transformed = calloc(1, sizeof(struct bmp_image));
    transformed->data = calloc(pixels_count, sizeof(struct pixel));
    transformed->header = calloc(1, sizeof(struct bmp_header));
    int width = image->header->width;
    int height = image->header->height;

    *(transformed->header) = *(image->header);
    transformed->header->width = height;
    transformed->header->height = width;
    transformed->header->size = compute_new_size(transformed->header);
    transformed->header->image_size = transformed->header->size - sizeof (struct bmp_header);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            int rotated_index = (width - x - 1) * height + y;
            transformed->data[rotated_index] = image->data[index];
        }
    }

    return transformed;
}

struct bmp_image *crop(const struct bmp_image *image, const uint32_t start_y, const uint32_t start_x, const uint32_t height,const uint32_t width) {
    if (image == NULL || image->header == NULL || image->data == NULL) {
        fprintf(stderr, "Error: Image struct is empty");
        return NULL;
    }

    if (start_y + height > image->header->height || start_x + width > image->header->width) {
        fprintf(stderr, "Error: Selected area is out of range");
        return NULL;
    }

    struct bmp_image *cropped = calloc(1, sizeof(struct bmp_image));


    cropped->header = calloc(1, sizeof(struct bmp_header));

    *(cropped->header) = *(image->header);
    cropped->header->height = height;
    cropped->header->width = width;

    cropped->header->size = compute_new_size(cropped->header);
    cropped->header->image_size = cropped->header->size - sizeof (struct bmp_header);

    cropped->data = calloc(height * width, sizeof(struct pixel));

    size_t row_size = row_size_in_bytes(cropped->header->width, cropped->header->bpp);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < row_size; x++) {
            int cropped_index = y * width + x;
            if(x < width) {
                int index = (start_y + y) * image->header->width + (start_x + x);
                cropped->data[cropped_index] = image->data[index];
            } else {
                cropped->data[cropped_index].green =  0;
                cropped->data[cropped_index].blue = 0;
                cropped->data[cropped_index].red =  0;
            }
        }
    }

    return cropped;
}

struct bmp_image *scale(const struct bmp_image *image, float factor) {
    if (image == NULL || image->header == NULL || image->data == NULL || factor <= 0) {
        fprintf(stderr, "Error: Invalid arguments.");
        return NULL;
    }

    struct bmp_image *scaled = calloc(1, sizeof(struct bmp_image));
    scaled->header = calloc(1, sizeof(struct bmp_header));

    *(scaled->header ) = *(image->header);
    int prevWidth = image->header->width;
    int prevHeight = image->header->height;

    scaled->header->width = round(image->header->width * factor);
    scaled->header->height = round(image->header->height * factor);
    scaled->header->size = compute_new_size(scaled->header);
    scaled->header->image_size = scaled->header->size - sizeof (struct bmp_header);

    scaled->data = calloc(scaled->header->image_size, sizeof(struct pixel));

    int ptr = 0;
    int ptrY = 0;
    for (int y = 0; y < prevHeight; ++y) {
        while (floor(ptrY/factor) == y && ptrY < scaled->header->height){
            int ptrX = 0;
            for (int x = ptrX; x < prevWidth; ++x) {
                    while (floor((ptrX / factor)) ==
                           x && ptrX < scaled->header->width) {
                        int current = (prevWidth * y) + x;
                        scaled->data[ptr] = image->data[current];
                        ptr++;
                        ptrX++;
                }
            }
            ptrY++;
        }
    }

    return scaled;
}



struct bmp_image* extract(const struct bmp_image* image, const char* colors_to_keep){
    if (image == NULL) {
        return NULL;
    }

    if(image->header == NULL || image->data == NULL || colors_to_keep == NULL ){
        return NULL;
    }

    for (int i = 0; i < strlen(colors_to_keep); ++i) {
        if(colors_to_keep[i] != 'r' && colors_to_keep[i] != 'b' && colors_to_keep[i] != 'g'){
            return NULL;
        }
    }

    struct bmp_image *extracted = calloc(1, sizeof(struct bmp_image));
    extracted->header = calloc(1, sizeof(struct bmp_header));
    extracted->data = calloc(image->header->height * image->header->width, sizeof(struct bmp_image));

    *(extracted->header ) = *(image->header);

    int r = 0;
    int g = 0;
    int b = 0;

    for (int i = 0; i < strlen(colors_to_keep); ++i) {
        switch (colors_to_keep[i]) {
           case 'r':
            r = 1;
                break;
            case 'g':
                g = 1;
                break;
            case 'b':
                b = 1;
                break;
            default:
                break;
        }
    }

    for (int i = 0; i < extracted->header->size; ++i) {
        extracted->data[i] = image->data[i];
        if(r == 0){
            extracted->data[i].red = 0;
        }
        if(g == 0){
            extracted->data[i].green = 0;
        }
        if(b == 0){
            extracted->data[i].blue = 0;
        }
    }

    return extracted;
}