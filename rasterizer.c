#include <math.h>

#include "rasterizer.h"

/*
 * NDC: Normalized Device Coordinates
 */

Vector _NDCPos2ImagePos(const Camera *camera, const Vector projectionVec) {
  return V((Real)camera->image_width / 2 * (projectionVec.x + 1), (Real)camera->image_height / 2 * (projectionVec.y + 1), -projectionVec.z);
}

Vector _WorldPos2NDCPos(const Camera *camera, const Vector worldVec) {
  Real _worldPos[][1] = {{worldVec.x}, {worldVec.y}, {worldVec.z}, {1}};
  Matrix *worldPos = MatrixFromArray(4, 1, _worldPos);
  Matrix *_ndcPos = MatrixMultiplication(camera->world2ndc, worldPos);
  Vector ndcPos = V(MatrixGetElement(_ndcPos, 0, 0), MatrixGetElement(_ndcPos, 1, 0), MatrixGetElement(_ndcPos, 2, 0));
  MatrixDestroy(_ndcPos);
  MatrixDestroy(worldPos);
  return ndcPos;
}

Triangle rasterize(const Camera *camera, const Triangle triangle) {
  Triangle new = triangle;
  new.vertexes[0] = _NDCPos2ImagePos(camera, _WorldPos2NDCPos(camera, new.vertexes[0]));
  new.vertexes[1] = _NDCPos2ImagePos(camera, _WorldPos2NDCPos(camera, new.vertexes[1]));
  new.vertexes[2] = _NDCPos2ImagePos(camera, _WorldPos2NDCPos(camera, new.vertexes[2]));
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