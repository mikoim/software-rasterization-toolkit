#include "bitmap.h"
#include "vector.h"

int main() {
  const int w = 2000;
  const int h = (int)(0.866 * w);
  Bitmap *bmp = BitmapNewImage(w, h);

  Vector vecRed = {(w - 1) / (Real)2, 0, 0};
  Vector vecGreen = {0, h - 1, 0};
  Vector vecBlue = {w - 1, h - 1, 0};

  for (int width = 0; width < w; ++width) {
    for (int height = 0; height < h; ++height) {
      uint8_t r = 255;
      uint8_t g = 255;
      uint8_t b = 255;
      Vector vec = {width, height, 0};
      if (VectorInsideTriangle2D(vec, vecRed, vecGreen, vecBlue)) {
        r = (w - VectorEuclideanDistance(vec, vecRed)) / w * 255;
        g = (w - VectorEuclideanDistance(vec, vecGreen)) / w * 255;
        b = (w - VectorEuclideanDistance(vec, vecBlue)) / w * 255;
      }
      BitmapSetPixelColor(bmp, width, height, BMP_COLOR(r, g, b));
    }
  }

  BitmapWriteFile(bmp, "triangle.bmp");
  BitmapDestroy(bmp);

  return 0;
}