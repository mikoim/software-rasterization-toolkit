#ifndef RENDER_RASTERIZER_H
#define RENDER_RASTERIZER_H

#include "bitmap.h"
#include "camera.h"
#include "common.h"
#include "polygon.h"
#include "transformer.h"

typedef struct tagZBuffer {
  uint16_t imageWidth;
  uint16_t imageHeight;
  Real *depths;
} ZBuffer;

Vector NDCPos2ImagePos(const Camera *camera, Vector projectionVec);
Vector ImagePos2NDCPos(const Camera *camera, Vector imageVec);
Vector WorldPos2NDCPos(const Camera *camera, Vector worldVec);
Vector NDCPos2WorldPos(const Camera *camera, Vector ndcVec, Real depth);

Triangle rasterize(const Camera *camera, Triangle triangle);

void DrawLine(Bitmap *bitmap, Vector v1, Vector v2, const RGBTRIPLE *color);
void DrawTriangle(Bitmap *bitmap, Vector v1, Vector v2, Vector v3, const RGBTRIPLE *color, ZBuffer *zbuffer);

ZBuffer *ZBufferCreate(uint16_t imageWidth, uint16_t imageHeight);
Real ZBufferGetDepth(const ZBuffer *zbuffer, uint16_t x, uint16_t y);
bool ZBufferSetDepth(const ZBuffer *zbuffer, uint16_t x, uint16_t y, Real depth);
bool ZBufferTestAndUpdate(ZBuffer *zbuffer, uint16_t x, uint16_t y, Real depth);
bool ZBufferExportToImage(const ZBuffer *zbuffer, Bitmap *bitmap);
bool ZBufferDestroy(ZBuffer *zbuffer);

#endif // RENDER_RASTERIZER_H
