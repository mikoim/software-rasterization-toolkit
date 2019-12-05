#ifndef RENDER_CSG_H
#define RENDER_CSG_H

#include "linkedlist.h"
#include "polygon.h"

typedef enum { CSG_Sets, CSG_Triangle, CSG_Plane, CSG_Cube, CSG_Cylinder, CSG_Triangular, CSG_Ball } CSGPrimitiveType;

typedef struct tagCSG {
  CSGPrimitiveType type;
  LinkedList *primitives;
} CSGSets;

typedef struct tagCSGPrimitive {
  CSGPrimitiveType type;
} CSGPrimitive;

typedef struct tagCSGTriangle {
  CSGPrimitiveType type;
  Vector vertexes[3];
  Vector surfaceNormal;
} CSGTriangle;

typedef struct tagCSGPlane {
  CSGPrimitiveType type;
  Vector vertexes[4];
  Vector surfaceNormal;
} CSGPlane;

typedef struct tagCSGCube {
  CSGPrimitiveType type;
  Real size;
} CSGCube;

typedef struct tagCSGCylinder {
  CSGPrimitiveType type;
  Real radius;
  Real height;
  uint64_t partition;
} CSGCylinder, CSGTriangular;

typedef struct tagCSGBall {
  CSGPrimitiveType type;
  Real radius;
  uint64_t partition;
} CSGBall;

CSGSets *CSGPrimitiveSetsCreate();
CSGPrimitive *CSGPrimitiveSetsAppend(CSGSets *sets, CSGPrimitive *primitive);
bool CSGPrimitiveSetsDestroy(CSGSets *sets);

CSGTriangle *CSGPrimitiveTriangleCreate(Vector vertexes[3], Vector surfaceNormal);
CSGPlane *CSGPrimitivePlaneCreate(Vector vertexes[4], Vector surfaceNormal);
CSGCube *CSGPrimitiveCubeCreate(Real size);
CSGCylinder *CSGPrimitiveCylinderCreate(Real radius, Real height, uint64_t partition);
CSGCylinder *CSGPrimitiveTriangularCreate(Real radius, Real height, uint64_t partition);
CSGBall *CSGPrimitiveBallCreate(Real radius, uint64_t partition);

void CSGPrimitivePlaneReduce(CSGSets *sets, const CSGPlane *plane);
void CSGPrimitiveCubeReduce(CSGSets *sets, const CSGCube *cube);
void CSGPrimitiveCylinderReduce(CSGSets *sets, const CSGCylinder *cylinder);
void CSGPrimitiveTriangularReduce(CSGSets *sets, const CSGCylinder *cylinder);
void CSGPrimitiveBallReduce(CSGSets *sets, const CSGBall *ball);
bool CSGPrimitiveReduce(CSGSets *sets, const CSGPrimitive *primitive);
void CSGPrimitiveSetsReduce(CSGSets *sets);

Polygon *CSGPrimitiveSetsPolygon(const CSGSets *sets);

#endif // RENDER_CSG_H
