#include "bitmap.h"

int main() {
  const int w = 2000;
  const int h = 100;
  Bitmap *bmp = BitmapNewImage(w, h);

  for (int width = 0; width < w; ++width) {
    for (int height = 0; height < h; ++height) {
      uint8_t r = 0;
      uint8_t g = 0;
      uint8_t b = 0;

      int boundary = w / 6;
      double low2high = (double)(width % boundary) / boundary;
      double high2low = 1 - low2high;

      if (width < boundary) {
        r = 255;
        g = 255 * low2high;
      } else if (width < boundary * 2) {
        r = 255 * high2low;
        g = 255;
      } else if (width < boundary * 3) {
        g = 255;
        b = 255 * low2high;
      } else if (width < boundary * 4) {
        g = 255 * high2low;
        b = 255;
      } else if (width < boundary * 5) {
        r = 255 * low2high;
        b = 255;
      } else if (width < boundary * 6) {
        r = 255;
        b = 255 * high2low;
      }

      BitmapSetPixelColor(bmp, width, height, BMP_COLOR(r, g, b));
    }
  }

  BitmapWriteFile(bmp, "hue_scale.bmp");
  BitmapDestroy(bmp);

  return 0;
}