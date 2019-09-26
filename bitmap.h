#ifndef RENDER_BITMAP_H
#define RENDER_BITMAP_H

#include "common.h"

#define BMP_MAGIC 0x4d42
#define BMP_COLOR(r, g, b) &(RGBTRIPLE){b, g, r}

typedef enum {
    BMP_ERROR = -1,
    BMP_OK = 0
} BitmapStatus;

#pragma pack(push)
#pragma pack(1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    uint32_t bcSize;
    uint16_t bcWidth;
    uint16_t bcHeight;
    uint16_t bcPlanes;
    uint16_t bcBitCount;
} BITMAPCOREHEADER;

typedef struct {
    uint8_t rgbtBlue;
    uint8_t rgbtGreen;
    uint8_t rgbtRed;
} RGBTRIPLE;
#pragma pack(pop)

typedef struct {
    BITMAPFILEHEADER fileHeader;
    BITMAPCOREHEADER dibHeader;
    RGBTRIPLE *pixels; // bottom-up
} Bitmap;

Bitmap *BitmapNewImage(uint16_t width, uint16_t height);
BitmapStatus BitmapDestroy(Bitmap *bitmap);
BitmapStatus BitmapWriteFile(const Bitmap *bitmap, const char *filename);
BitmapStatus BitmapSetPixelColor(Bitmap *bitmap, uint16_t x, uint16_t y, const RGBTRIPLE *color);

#endif //RENDER_BITMAP_H
