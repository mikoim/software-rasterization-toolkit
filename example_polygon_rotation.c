#include <stdio.h>

#include "rasterizer.h"

int main() {
  const int w = 1000;
  const int h = 1000;

  // load alice polygon model from STL file
  Polygon *polygon = PolygonReadSTL("models/alice.stl");

  // define camera
  Camera *camera = CameraPerspectiveProjection(V(2, 0, 0), V(0, 0, 0), V(0, 1, 0), w, h, 0.1, 1000, 120);

#ifdef _OPENMP
#pragma omp parallel for default(none) schedule(dynamic) shared(camera, polygon)
#endif
  for (int r = 0; r < 360; ++r) {
    // create empty Bitmap image
    Bitmap *bmp = BitmapNewImage(w, h);

    // define transformer to spin the model
    Transformer *transformer = TransformerCreate(V0, V(0, RADIAN(r), 0), V1);

    // draw wire-frame
    for (uint32_t i = 0; i < polygon->triangle; ++i) {
      Triangle triangleWorld = TransformerTransformTriangle(transformer, polygon->triangles[i]);
      Triangle triangle = rasterize(camera, triangleWorld);
      DrawLine(bmp, triangle.vertexes[0], triangle.vertexes[1], BMP_COLOR(0, 0, 255));
      DrawLine(bmp, triangle.vertexes[1], triangle.vertexes[2], BMP_COLOR(0, 0, 255));
      DrawLine(bmp, triangle.vertexes[2], triangle.vertexes[0], BMP_COLOR(0, 0, 255));
    }

    // save image to Bitmap file
    char buf[100];
    sprintf(buf, "polygon_rotation_%d.bmp", r);
    BitmapWriteFile(bmp, buf);

    // clean-up
    BitmapDestroy(bmp);
  }

  CameraDestroy(camera);
  PolygonDestroy(polygon);

  return 0;
}