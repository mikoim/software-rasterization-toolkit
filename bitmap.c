#include <stdio.h>
#include <stdlib.h>

#include "bitmap.h"

Bitmap *BitmapNewImage(uint16_t width, uint16_t height) {
    Bitmap *bitmap;

    bitmap = (Bitmap *) calloc(1, sizeof(*bitmap));

    uint32_t headerSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPCOREHEADER);
    uint32_t rowSize = sizeof(RGBTRIPLE) * width;
    // Each row in the Pixel array is padded to a multiple of 4 bytes in size.
    if (rowSize % 4) { rowSize += 4 - rowSize % 4; }
    uint32_t size = rowSize * height;

    bitmap->fileHeader = (BITMAPFILEHEADER) {BMP_MAGIC, headerSize + size, 0, 0, headerSize};
    bitmap->dibHeader = (BITMAPCOREHEADER) {12, width, height, 1, 24};
    bitmap->pixels = (RGBTRIPLE *) calloc(1, size);

    return bitmap;
}

bool BitmapDestroy(Bitmap *bitmap) {
    free(bitmap->pixels);
    free(bitmap);
    return true; // TODO: implement error handler
}

bool BitmapWriteFile(const Bitmap *bitmap, const char *filename) {
    FILE *fp = fopen(filename, "w+");
    fwrite(&bitmap->fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    fwrite(&bitmap->dibHeader, sizeof(BITMAPCOREHEADER), 1, fp);
    fwrite(bitmap->pixels, bitmap->fileHeader.bfSize - bitmap->fileHeader.bfOffBits, 1, fp);
    fclose(fp);
    return true; // TODO: implement error handler
}

bool BitmapSetPixelColor(Bitmap *bitmap, uint16_t x, uint16_t y, const RGBTRIPLE *color) {
    if (bitmap->dibHeader.bcWidth <= x || bitmap->dibHeader.bcHeight <= y) {
        fprintf(stderr, "%s: Invalid pixel indices (x:%d<%d, y:%d<%d)\n", __FUNCTION_NAME__, x, bitmap->dibHeader.bcWidth, y, bitmap->dibHeader.bcHeight);
        return false;
    }
    bitmap->pixels[x + bitmap->dibHeader.bcWidth * (bitmap->dibHeader.bcHeight - y -1)] = *color;
    return true;
}
