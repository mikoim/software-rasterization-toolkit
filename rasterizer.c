#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rasterizer.h"
#include "transformer.h"

/*
 * NDC: Normalized Device Coordinates
 */

Vector NDCPos2ImagePos(const Camera *camera, const Vector projectionVec) {
  return V((Real)camera->image_width / 2 * (projectionVec.x + 1), (Real)camera->image_height / 2 * (projectionVec.y + 1), -projectionVec.z);
}

Vector ImagePos2NDCPos(const Camera *camera, const Vector imageVec) { return V((Real)-1 + 2 * imageVec.x / camera->image_width, (Real)-1 + 2 * imageVec.y / camera->image_height, -imageVec.z); }

Vector WorldPos2NDCPos(const Camera *camera, const Vector worldVec) {
  Real _worldPos[][1] = {{worldVec.x}, {worldVec.y}, {worldVec.z}, {1}};
  Matrix *worldPos = MatrixFromArray(4, 1, _worldPos);
  Matrix *_ndcPos = MatrixMultiplication(camera->world2ndc, worldPos);
  Real depth = MatrixGetElement(_ndcPos, 3, 0);
  Vector ndcPos = V(-MatrixGetElement(_ndcPos, 0, 0) / depth, -MatrixGetElement(_ndcPos, 1, 0) / depth, MatrixGetElement(_ndcPos, 2, 0)); // NOTE: Negative sign corrects orientation of image
  MatrixDestroy(_ndcPos);
  MatrixDestroy(worldPos);
  return ndcPos;
}

// FIXME: This function will be used for phong shading but it's currently broken or not tested. It requires a depth between camera to surface, unfortunately there is no function implemented to do
// that.
Vector NDCPos2WorldPos(const Camera *camera, const Vector ndcVec, const Real depth) {
  Real _ndcPos[][1] = {{ndcVec.x * depth}, {ndcVec.y * depth}, {ndcVec.z}, {depth}}; // NOTE: WorldPos2NDCPos drop one element "depth" from NDC matrix so we need to estimate it
  Matrix *ndcPos = MatrixFromArray(4, 1, _ndcPos);
  Matrix *_worldPos = MatrixMultiplication(camera->ndc2world, ndcPos);
  Vector worldPos = V(MatrixGetElement(_worldPos, 0, 0), MatrixGetElement(_worldPos, 1, 0), MatrixGetElement(_worldPos, 2, 0));
  MatrixDestroy(_worldPos);
  MatrixDestroy(ndcPos);
  return worldPos;
}

Triangle rasterize(const Camera *camera, const Triangle triangle) {
  Triangle new = triangle;
  new.vertexes[0] = NDCPos2ImagePos(camera, WorldPos2NDCPos(camera, new.vertexes[0]));
  new.vertexes[1] = NDCPos2ImagePos(camera, WorldPos2NDCPos(camera, new.vertexes[1]));
  new.vertexes[2] = NDCPos2ImagePos(camera, WorldPos2NDCPos(camera, new.vertexes[2]));
  new.surfaceNormal = NDCPos2ImagePos(camera, WorldPos2NDCPos(camera, new.surfaceNormal));
  new.vertexNormals[0] = NDCPos2ImagePos(camera, WorldPos2NDCPos(camera, new.vertexNormals[0]));
  new.vertexNormals[1] = NDCPos2ImagePos(camera, WorldPos2NDCPos(camera, new.vertexNormals[1]));
  new.vertexNormals[2] = NDCPos2ImagePos(camera, WorldPos2NDCPos(camera, new.vertexNormals[2]));
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

void DrawTriangle(Bitmap *bitmap, const Vector v1, const Vector v2, const Vector v3, const RGBTRIPLE *color, ZBuffer *zbuffer) {
  const uint32_t maxX = (uint32_t)fminl(fmaxl(fmaxl(v1.x, fmaxl(v2.x, v3.x)), 0), bitmap->dibHeader.bcWidth - 1);
  const uint32_t minX = (uint32_t)fmaxl(fminl(v1.x, fminl(v2.x, v3.x)), 0);
  const uint32_t maxY = (uint32_t)fminl(fmaxl(fmaxl(v1.y, fmaxl(v2.y, v3.y)), 0), bitmap->dibHeader.bcHeight - 1);
  const uint32_t minY = (uint32_t)fmaxl(fminl(v1.y, fminl(v2.y, v3.y)), 0);

  for (uint32_t y = minY; y <= maxY; ++y) {
    for (uint32_t x = minX; x <= maxX; ++x) {
      if (VectorInsideTriangle2D(V(x, y, 0), v1, v2, v3)) {
        Vector weight = VectorBarycentricCoordinateWeight(V(x, y, 0), v1, v2, v3);
        Real depth = v1.z * weight.x + v2.z * weight.y + v3.z * weight.z;
        if (zbuffer == NULL || ZBufferTestAndUpdate(zbuffer, x, y, depth)) {
          BitmapSetPixelColor(bitmap, x, bitmap->dibHeader.bcHeight - y - 1, color);
        }
      }
    }
  }
}

ZBuffer *ZBufferCreate(uint16_t imageWidth, uint16_t imageHeight) {
  uint32_t bufferLength = imageWidth * imageHeight;
  ZBuffer *zbuffer = (ZBuffer *)calloc(1, sizeof(ZBuffer));
  Real *depths = (Real *)calloc(bufferLength, sizeof(Real));
  zbuffer->imageWidth = imageWidth;
  zbuffer->imageHeight = imageHeight;
  zbuffer->depths = depths;
  for (uint32_t i = 0; i < bufferLength; ++i) {
    zbuffer->depths[i] = DBL_MAX;
  }
  return zbuffer;
}

Real ZBufferGetDepth(const ZBuffer *zbuffer, uint16_t x, uint16_t y) {
  if (zbuffer->imageWidth <= x || zbuffer->imageHeight <= y) {
#ifndef NDEBUG
    fprintf(stderr, "%s: Invalid depth indices (x:%d<%d, y:%d<%d)\n", __FUNCTION_NAME__, x, zbuffer->imageWidth, y, zbuffer->imageHeight);
#endif
    return DBL_MIN;
  }
  Real depth;
  depth = zbuffer->depths[x + zbuffer->imageHeight * y];
  return depth;
}

bool ZBufferSetDepth(const ZBuffer *zbuffer, uint16_t x, uint16_t y, Real depth) {
  if (zbuffer->imageWidth <= x || zbuffer->imageHeight <= y) {
#ifndef NDEBUG
    fprintf(stderr, "%s: Invalid depth indices (x:%d<%d, y:%d<%d)\n", __FUNCTION_NAME__, x, zbuffer->imageWidth, y, zbuffer->imageHeight);
#endif
    return false;
  }
  zbuffer->depths[x + zbuffer->imageHeight * y] = depth;
  return true;
}

bool ZBufferTestAndUpdate(ZBuffer *zbuffer, uint16_t x, uint16_t y, Real depth) {
  const uint16_t imageWidth = zbuffer->imageWidth;
  const uint16_t imageHeight = zbuffer->imageHeight;
  if (imageWidth <= x || imageHeight <= y) {
#ifndef NDEBUG
    fprintf(stderr, "%s: Invalid depth indices (x:%d<%d, y:%d<%d)\n", __FUNCTION_NAME__, x, imageWidth, y, imageHeight);
#endif
    return false;
  }
  Real currentDepth = ZBufferGetDepth(zbuffer, x, y);
  if (currentDepth < depth) {
#ifndef NDEBUG
    fprintf(stderr, "%s: Deeper than current depth (%d, %d) = %Lf < %Lf\n", __FUNCTION_NAME__, x, y, currentDepth, depth);
#endif
    return false;
  }
  return ZBufferSetDepth(zbuffer, x, y, depth);
}

bool ZBufferExportToImage(const ZBuffer *zbuffer, Bitmap *bitmap) {
  const uint16_t bitmapWidth = bitmap->dibHeader.bcWidth;
  const uint16_t bitmapHeight = bitmap->dibHeader.bcHeight;
  const uint16_t imageWidth = zbuffer->imageWidth;
  const uint16_t imageHeight = zbuffer->imageHeight;
  if (imageWidth > bitmapWidth || imageHeight > bitmapHeight) {
#ifndef NDEBUG
    fprintf(stderr, "%s: Invalid image size (x:%d=<%d, y:%d=<%d)\n", __FUNCTION_NAME__, imageWidth, bitmapWidth, imageHeight, bitmapHeight);
#endif
    return false;
  }
  Real maxDepth = DBL_MIN;

  for (uint32_t i = 0; i < imageWidth * imageHeight; ++i) {
    const Real depth = zbuffer->depths[i];
    if (depth != DBL_MAX && depth > maxDepth) {
      maxDepth = depth;
    }
  }

  for (uint16_t x = 0; x < imageWidth; ++x) {
    for (uint16_t y = 0; y < imageHeight; ++y) {
      const Real depth = ZBufferGetDepth(zbuffer, x, y);
      if (depth < 0) { // Negative depth. maybe bug
        BitmapSetPixelColor(bitmap, x, bitmapHeight - y - 1, BMP_COLOR(0, 0, 255));
      } else if (depth == DBL_MAX) { // untouched pixel
        BitmapSetPixelColor(bitmap, x, bitmapHeight - y - 1, BMP_COLOR(255, 0, 0));
      } else {
        BitmapSetPixelColor(bitmap, x, bitmapHeight - y - 1, BMP_GRAY_SCALE(255 - (255 * depth) / maxDepth));
      }
    }
  }

  return true;
}

bool ZBufferDestroy(ZBuffer *zbuffer) {
  if (zbuffer == NULL) {
#ifndef NDEBUG
    fprintf(stderr, "%s: trying to free null pointer, ignored.\n", __FUNCTION_NAME__);
#endif
    return false;
  }
  free(zbuffer->depths);
  free(zbuffer);
  return true;
}
