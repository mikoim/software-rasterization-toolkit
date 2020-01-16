#include "rasterizer.h"

int main() {
  const int w = 1000;
  const int h = 1000;

  // create empty Bitmap
  Bitmap *bmp = BitmapNewImage(w, h);
  Bitmap *bmpDepth = BitmapNewImage(w, h);

  // load monkey polygon model from STL file
  Polygon *polygon = PolygonReadSTL("models/monkey.stl");

  // define camera
  Camera *camera = CameraPerspectiveProjection(V(1, 0, 0), V(0, 0, 0), V(0, 1, 0), w, h, 0.1, 1000, 120);

  // create empty z-buffer
  ZBuffer *zbuffer = ZBufferCreate(w, h);

  // draw wire-frame
  for (uint32_t i = 0; i < polygon->triangle; ++i) {
    Triangle triangle = rasterize(camera, polygon->triangles[i]);
    DrawTriangle(bmp, triangle.vertexes[0], triangle.vertexes[1], triangle.vertexes[2], BMP_COLOR(255, 0, 0), zbuffer);
  }

  // visualize z-buffer
  ZBufferExportToImage(zbuffer, bmpDepth);

  // save image to Bitmap file
  BitmapWriteFile(bmp, "zbuffer.bmp");

  // save z-buffer image to Bitmap file
  BitmapWriteFile(bmpDepth, "zbuffer_depth.bmp");

  // clean-up
  ZBufferDestroy(zbuffer);
  CameraDestroy(camera);
  PolygonDestroy(polygon);
  BitmapDestroy(bmp);
  BitmapDestroy(bmpDepth);

  return 0;
}
