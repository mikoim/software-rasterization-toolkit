#ifndef RENDER_POLYGON_H
#define RENDER_POLYGON_H

#include "common.h"
#include "vector.h"

#pragma pack(push)
#pragma pack(1)
typedef struct tagSTLTriangle {
  float surfaceNormal[3];
  float vertexes[3][3];
  uint16_t attribute;
} STLTriangle;
#pragma pack(pop)

typedef struct tagTriangle {
  Vector surfaceNormal;
  Vector vertexes[3];
  Vector vertexNormals[3];
} Triangle;

typedef struct tagPolygon {
  uint64_t triangle;
  Triangle *triangles;
} Polygon;

Polygon *PolygonReadSTL(const char *filename);
bool PolygonDestroy(Polygon *polygon);
bool PolygonCalculateVertexNormals(Polygon *polygon);

#endif // RENDER_POLYGON_H
