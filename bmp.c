#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"
#include <stdbool.h>


struct bmp_header* read_bmp_header(FILE* stream){
    if (stream == NULL){
        return NULL;
    }

    if(ftell(stream) != 0){
        fseek(stream, 0, SEEK_SET);
    }
    if(fgetc(stream) != 'B' || fgetc(stream) != 'M'){
        return NULL;
    }
    if(fseek(stream, 0, SEEK_SET) != 0){
        return NULL;
    }

    struct bmp_header *image_header = calloc(1, sizeof (struct bmp_header));

    if (fread(image_header, sizeof(struct bmp_header), 1 ,stream) != 1){
        free(image_header);
        return NULL;
    }

    return image_header;
}

struct pixel* read_data(FILE* stream, const struct bmp_header* header){
    if(stream == NULL){
        return NULL;
    }
    if(header == NULL){
        return NULL;
    }

    struct pixel* pixels = calloc(header->image_size, sizeof (struct pixel));

    if(fseek(stream, header->offset, SEEK_SET) != 0){
        free(pixels);
        return NULL;
    }
    if(fread(pixels, header->image_size, 1 , stream) != 1){
        free(pixels);
        return NULL;
    }
    return pixels;
}

struct bmp_image* read_bmp(FILE* stream){

    if (stream == NULL) {
        return NULL;
    }

    struct bmp_image *image = calloc(1, sizeof (struct bmp_image));
    struct bmp_header *header = read_bmp_header(stream);

    if(header == NULL){
        fprintf(stderr, "Error: This is not a BMP file.\n");
        free(image);
        return NULL;
    } else {
        image->header = header;
    }

    struct pixel *pixels = read_data(stream, header);

    if (pixels == NULL){
        fprintf(stderr, "Error: Corrupted BMP file.\n");
        free(image);
        return NULL;
    } else{
        image->data = pixels;
    }
    return image;
}

void free_bmp_image(struct bmp_image* image){
    if(image == NULL){
        return;
    }
    if(image->data != NULL) {
        free(image->data);
    }
    if(image->header != NULL) {
        free(image->header);
    }
    free(image);
}

bool write_bmp(FILE* stream, const struct bmp_image* image){
    if(stream == NULL){
        fprintf(stderr, "Error: Stream is closed.\n");
        return false;
    }
    if (image == NULL || image->header == NULL || image->data == NULL){
        fprintf(stderr, "Error: No image to write.\n");
        return false;
    }



    if(fwrite(image->header, sizeof(struct bmp_header), 1, stream) != 1){
        fprintf(stderr, "Error: Data does not written.\n");
        return false;
    }

    if(fwrite(image->data, sizeof(struct pixel), image->header->image_size , stream) != image->header->image_size){
        fprintf(stderr, "Error: Data does not written.\n");
        return false;
    }

    return true;
}
