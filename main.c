#include <stdio.h>
#include "bmp.h"
#include "transformations.h"
#include <stdlib.h>

int main() {
    FILE* stream = fopen("assets/cherry.bmp", "rb");
    struct bmp_image *image = read_bmp(stream);
    fclose(stream);
    FILE * write_stream = fopen("new.bmp", "wb");
    struct bmp_image *rotated_image = rotate_left(image);
    struct bmp_image *rotated_right_image = rotate_right(image);
    struct bmp_image *vertically_image = flip_vertically(image);
    struct bmp_image *horizontally_image = flip_horizontally(image);
    struct bmp_image *scaled_image = scale(image, 2);
    struct bmp_image *crop1_image = crop(image,1,1, 20, 20);
    struct bmp_image *extracted_image = extract(image, "b");
    write_bmp(write_stream, extracted_image);
    free_bmp_image(rotated_image);
    free_bmp_image(rotated_right_image);
    free_bmp_image(vertically_image);
    free_bmp_image(horizontally_image);
    free_bmp_image(scaled_image);
    free_bmp_image(image);
    free_bmp_image(crop1_image);
    free_bmp_image(extracted_image);
    fclose(write_stream);
    return 0;
}