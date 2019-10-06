#include "rasterizer.h"

/**
 * OpenGL (Right hand)
 *      y
 *      |
 *      |___ x
 *     /
 *    z
 *
 * DirectX (Left hand)
 *   y  z
 *   | /
 *   |/___ x
 */

int main() {
  const int w = 1000;
  const int h = 1000;
  Bitmap *bmp = BitmapNewImage(w, h);

  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      BitmapSetPixelColor(bmp, x, y, BMP_COLOR(0, 0, 0));
    }
  }

  Polygon *polygon = PolygonReadSTL("models/monkey.stl");

  Camera *camera = CameraPerspectiveProjection(V(1, 0, 0), V(0, 0, 0), V(0, 1, 0), w, h, 0.1, 1000, 120);

  MatrixPrint(camera->world2camera);
  MatrixPrint(camera->camera2projection);
  MatrixPrint(camera->world2projection);

  for (uint32_t i = 0; i < polygon->triangle; ++i) {
    Triangle triangle = rasterize(camera, polygon->triangles[i]);
    DrawTriangle(bmp, triangle.vertexes[0], triangle.vertexes[1], triangle.vertexes[2], BMP_COLOR(255, 0, 0));
  }

  for (uint32_t i = 0; i < polygon->triangle; ++i) {
    Triangle triangle = rasterize(camera, polygon->triangles[i]);
    DrawLine(bmp, triangle.vertexes[0], triangle.vertexes[1], BMP_COLOR(0, 0, 255));
    DrawLine(bmp, triangle.vertexes[1], triangle.vertexes[2], BMP_COLOR(0, 0, 255));
    DrawLine(bmp, triangle.vertexes[2], triangle.vertexes[0], BMP_COLOR(0, 0, 255));
  }

  CameraDestroy(camera);
  PolygonDestroy(polygon);

  BitmapWriteFile(bmp, "polygon.bmp");
  BitmapDestroy(bmp);

  return 0;
}