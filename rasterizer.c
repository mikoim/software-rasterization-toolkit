#include <math.h>

#include "rasterizer.h"

Vector _projection2view(const Camera *camera, const Vector projectionVec) {
  const double slopeX = camera->image_width / 2.0;
  const double slopeY = camera->image_height / 2.0;
  return V(slopeX * (projectionVec.x + 1), slopeY * (projectionVec.y + 1), 0);
}

Vector _world2projection(const Camera *camera, const Vector worldVec) {
  Real _w[][1] = {{worldVec.x}, {worldVec.y}, {worldVec.z}, {1}};
  Matrix *w = MatrixFromArray(4, 1, _w);
  Matrix *p = MatrixMultiplication(camera->world2projection, w);
  Vector pV = V(MatrixGetElement(p, 0, 0), MatrixGetElement(p, 1, 0), MatrixGetElement(p, 2, 0));
  MatrixDestroy(p);
  MatrixDestroy(w);
  return pV;
}

Triangle rasterize(const Camera *camera, const Triangle triangle) {
  Triangle new = triangle;
  new.vertexes[0] = _projection2view(camera, _world2projection(camera, new.vertexes[0]));
  new.vertexes[1] = _projection2view(camera, _world2projection(camera, new.vertexes[1]));
  new.vertexes[2] = _projection2view(camera, _world2projection(camera, new.vertexes[2]));
  return new;
}

void DrawLine(Bitmap *bitmap, const Vector v1, const Vector v2, const RGBTRIPLE *color) {
  Vector unit = VectorL2Normalization(VectorSubtraction(v2, v1));
  Real length = VectorEuclideanDistance(v1, v2);
  Vector cur = v1;
  for (int i = 0; i <= length; ++i) {
    BitmapSetPixelColor(bitmap, cur.x, bitmap->dibHeader.bcHeight - cur.y, color);
    cur = VectorAddition(cur, unit);
  }
}

void DrawTriangle(Bitmap *bitmap, const Vector v1, const Vector v2, const Vector v3, const RGBTRIPLE *color) {
  uint32_t maxX = (uint32_t)fmaxl(v1.x, fmaxl(v2.x, v3.x));
  uint32_t minX = (uint32_t)fminl(v1.x, fminl(v2.x, v3.x));
  uint32_t maxY = (uint32_t)fmaxl(v1.y, fmaxl(v2.y, v3.y));
  uint32_t minY = (uint32_t)fminl(v1.y, fminl(v2.y, v3.y));

  for (uint32_t y = minY; y <= maxY; ++y) {
    for (uint32_t x = minX; x <= maxX; ++x) {
      if (VectorInsideTriangle2D(V(x, y, 0), v1, v2, v3)) {
        BitmapSetPixelColor(bitmap, x, bitmap->dibHeader.bcHeight - y, color);
      }
    }
  }
}