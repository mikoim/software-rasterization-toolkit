#include <math.h>
#include <stdio.h>

#include "rasterizer.h"

int main() {
  const int w = 2000;
  const int h = 2000;
  Polygon *polygon = PolygonReadSTL("models/alice.stl");
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic)
#endif
  for (int r = 0; r < 360; ++r) {
    Bitmap *bmp = BitmapNewImage(w, h);
    Camera *camera = CameraPerspectiveProjection(V(sinl((Real)r * M_PI / 180), 0, cosl((Real)r * M_PI / 180)), V(0,0,0), V(0, 1, 0), w, h, 0.1, 1000, 90);

#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic)
#endif
    for (uint32_t i = 0; i < polygon->triangle; ++i) {
      Triangle triangle = rasterize(camera, polygon->triangles[i]);
      DrawLine(bmp, triangle.vertexes[0], triangle.vertexes[1], BMP_COLOR(0, 0, 255));
      DrawLine(bmp, triangle.vertexes[1], triangle.vertexes[2], BMP_COLOR(0, 0, 255));
      DrawLine(bmp, triangle.vertexes[2], triangle.vertexes[0], BMP_COLOR(0, 0, 255));
    }

    CameraDestroy(camera);

    char buf[100];
    sprintf(buf, "polygon_rotation_%d.bmp", r);

    BitmapWriteFile(bmp, buf);
    BitmapDestroy(bmp);
  }
  PolygonDestroy(polygon);
  return 0;
}