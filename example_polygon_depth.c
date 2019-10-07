#include "rasterizer.h"

int main() {
  const int w = 1000;
  const int h = 1000;
  Bitmap *bmp = BitmapNewImage(w, h);
  Bitmap *bmpDepth = BitmapNewImage(w, h);

  Polygon *polygon = PolygonReadSTL("models/monkey.stl");

  Camera *camera = CameraPerspectiveProjection(V(1, 0, 0), V(0, 0, 0), V(0, 1, 0), w, h, 0.1, 1000, 120);

  ZBuffer *zbuffer = ZBufferCreate(w, h);

  for (uint32_t i = 0; i < polygon->triangle; ++i) {
    Triangle triangle = rasterize(camera, polygon->triangles[i]);
    DrawTriangle(bmp, triangle.vertexes[0], triangle.vertexes[1], triangle.vertexes[2], BMP_COLOR(255, 0, 0), zbuffer);
  }

  ZBufferExportToImage(zbuffer, bmpDepth);
  ZBufferDestroy(zbuffer);

  CameraDestroy(camera);
  PolygonDestroy(polygon);

  BitmapWriteFile(bmp, "polygon_depth.bmp");
  BitmapDestroy(bmp);

  BitmapWriteFile(bmpDepth, "polygon_depth_buffer.bmp");
  BitmapDestroy(bmpDepth);
  return 0;
}
