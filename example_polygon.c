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

  // create empty Bitmap
  Bitmap *bmp = BitmapNewImage(w, h);

  // load monkey polygon model from STL file
  Polygon *polygon = PolygonReadSTL("models/monkey.stl");

  // define camera
  Camera *camera = CameraPerspectiveProjection(V(1, 0, 0), V(0, 0, 0), V(0, 1, 0), w, h, 0.1, 1000, 120);

  // solid shading
  for (uint32_t i = 0; i < polygon->triangle; ++i) {
    Triangle triangle = rasterize(camera, polygon->triangles[i]);
    DrawTriangle(bmp, triangle.vertexes[0], triangle.vertexes[1], triangle.vertexes[2], BMP_COLOR(255, 0, 0), NULL);
  }

  // draw wire-frame
  for (uint32_t i = 0; i < polygon->triangle; ++i) {
    Triangle triangle = rasterize(camera, polygon->triangles[i]);
    DrawLine(bmp, triangle.vertexes[0], triangle.vertexes[1], BMP_COLOR(0, 0, 255));
    DrawLine(bmp, triangle.vertexes[1], triangle.vertexes[2], BMP_COLOR(0, 0, 255));
    DrawLine(bmp, triangle.vertexes[2], triangle.vertexes[0], BMP_COLOR(0, 0, 255));
  }

  // save image to Bitmap file
  BitmapWriteFile(bmp, "polygon.bmp");

  // clean-up
  CameraDestroy(camera);
  PolygonDestroy(polygon);
  BitmapDestroy(bmp);
  return 0;
}