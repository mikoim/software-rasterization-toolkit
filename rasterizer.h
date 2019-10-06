#ifndef RENDER_RASTERIZER_H
#define RENDER_RASTERIZER_H

#include "bitmap.h"
#include "camera.h"
#include "common.h"
#include "polygon.h"

Triangle rasterize(const Camera *camera, Triangle triangle);
void DrawLine(Bitmap *bitmap, Vector v1, Vector v2, const RGBTRIPLE *color);
void DrawTriangle(Bitmap *bitmap, Vector v1, Vector v2, Vector v3, const RGBTRIPLE *color);

#endif // RENDER_RASTERIZER_H
