#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csg.h"

CSGSets *CSGPrimitiveSetsCreate() {
  CSGSets *sets = (CSGSets *)calloc(1, sizeof(CSGSets));
  sets->type = CSG_Sets;
  sets->primitives = LinkedListCreate(NULL);
  return sets;
}

CSGPrimitive *CSGPrimitiveSetsAppend(CSGSets *sets, CSGPrimitive *primitive) {
  if (sets->primitives[0].ptr == NULL) {
    sets->primitives[0].ptr = primitive;
  } else {
    LinkedListAppend(sets->primitives, primitive);
  }
  return primitive;
}

bool CSGPrimitiveSetsDestroy(CSGSets *sets) {
  LinkedListDestroy(sets->primitives, true, NULL);
  free(sets);
  return true;
}

CSGTriangle *CSGPrimitiveTriangleCreate(Vector vertexes[3], Vector surfaceNormal) {
  CSGTriangle *triangle = (CSGTriangle *)calloc(1, sizeof(CSGTriangle));
  *triangle = (CSGTriangle){CSG_Triangle, .vertexes = {vertexes[0], vertexes[1], vertexes[2]}, .surfaceNormal = surfaceNormal};
  return triangle;
}

CSGPlane *CSGPrimitivePlaneCreate(Vector vertexes[4], Vector surfaceNormal) {
  CSGPlane *plane = (CSGPlane *)calloc(1, sizeof(CSGPlane));
  *plane = (CSGPlane){CSG_Plane, .vertexes = {vertexes[0], vertexes[1], vertexes[2], vertexes[3]}, .surfaceNormal = surfaceNormal};
  return plane;
}

CSGCube *CSGPrimitiveCubeCreate(Real size) {
  CSGCube *cube = (CSGCube *)calloc(1, sizeof(CSGCube));
  *cube = (CSGCube){CSG_Cube, size};
  return cube;
}

CSGCylinder *CSGPrimitiveCylinderCreate(Real radius, Real height, uint64_t partition) {
  if (partition < 3) {
#ifndef NDEBUG
    fprintf(stderr, "%s: \"partition\" must be more than 3. (%ld >= 3) \n", __FUNCTION_NAME__, partition);
#endif
    return NULL;
  }
  CSGCylinder *cylinder = (CSGCylinder *)calloc(1, sizeof(CSGCylinder));
  *cylinder = (CSGCylinder){CSG_Cylinder, radius, height, partition};
  return cylinder;
}

CSGCylinder *CSGPrimitiveTriangularCreate(Real radius, Real height, uint64_t partition) {
  if (partition < 3) {
#ifndef NDEBUG
    fprintf(stderr, "%s: \"partition\" must be more than 3. (%ld >= 3) \n", __FUNCTION_NAME__, partition);
#endif
    return NULL;
  }
  CSGCylinder *triangular = (CSGCylinder *)calloc(1, sizeof(CSGCylinder));
  *triangular = (CSGTriangular){CSG_Triangular, radius, height, partition};
  return triangular;
}

CSGBall *CSGPrimitiveBallCreate(Real radius, uint64_t partition) {
  if (partition < 3) {
#ifndef NDEBUG
    fprintf(stderr, "%s: \"partition\" must be more than 3. (%ld >= 3) \n", __FUNCTION_NAME__, partition);
#endif
    return NULL;
  }
  CSGBall *ball = (CSGBall *)calloc(1, sizeof(CSGBall));
  *ball = (CSGBall){CSG_Ball, radius, partition};
  return ball;
}

void CSGPrimitivePlaneReduce(CSGSets *sets, const CSGPlane *plane) {
  Vector triangleVertex1[3] = {plane->vertexes[0], plane->vertexes[1], plane->vertexes[2]};
  Vector triangleVertex2[3] = {plane->vertexes[2], plane->vertexes[3], plane->vertexes[0]};

  CSGPrimitiveSetsAppend(sets, (CSGPrimitive *)CSGPrimitiveTriangleCreate(triangleVertex1, plane->surfaceNormal));
  CSGPrimitiveSetsAppend(sets, (CSGPrimitive *)CSGPrimitiveTriangleCreate(triangleVertex2, plane->surfaceNormal));
};

void CSGPrimitiveCubeReduce(CSGSets *sets, const CSGCube *cube) {
  Real s = cube->size;

  Vector bottomFace[4] = {V(0, 0, 0), V(s, 0, 0), V(s, 0, s), V(0, 0, s)};
  Vector topFace[4] = {V(0, s, 0), V(0, s, s), V(s, s, s), V(s, s, 0)};

  Vector rightSideFace[4] = {V(s, 0, 0), V(s, s, 0), V(s, s, s), V(s, 0, s)};
  Vector leftSideFace[4] = {V(0, 0, s), V(0, s, s), V(0, s, 0), V(0, 0, 0)};

  Vector frontFace[4] = {V(0, 0, s), V(s, 0, s), V(s, s, s), V(0, s, s)};
  Vector backFace[4] = {V(0, 0, 0), V(0, s, 0), V(s, s, 0), V(s, 0, 0)};

  CSGPrimitiveSetsAppend(sets, (CSGPrimitive *)CSGPrimitivePlaneCreate(bottomFace, V(0, -1, 0)));
  CSGPrimitiveSetsAppend(sets, (CSGPrimitive *)CSGPrimitivePlaneCreate(topFace, V(0, 1, 0)));

  CSGPrimitiveSetsAppend(sets, (CSGPrimitive *)CSGPrimitivePlaneCreate(rightSideFace, V(1, 0, 0)));
  CSGPrimitiveSetsAppend(sets, (CSGPrimitive *)CSGPrimitivePlaneCreate(leftSideFace, V(-1, 0, 0)));

  CSGPrimitiveSetsAppend(sets, (CSGPrimitive *)CSGPrimitivePlaneCreate(frontFace, V(0, 0, 1)));
  CSGPrimitiveSetsAppend(sets, (CSGPrimitive *)CSGPrimitivePlaneCreate(backFace, V(0, 0, -1)));
};

void CSGPrimitiveCylinderReduce(CSGSets *sets, const CSGCylinder *cylinder) {
  uint64_t p = cylinder->partition;
  Real h = cylinder->height;
  Real r = cylinder->radius;
  Vector bottomCenter = V0;
  Vector topCenter = V(0, cylinder->height, 0);
  Vector *bottoms = (Vector *)calloc(p, sizeof(Vector));
  Vector *tops = (Vector *)calloc(p, sizeof(Vector));
  for (uint64_t j = 0; j < p; ++j) {
    Real x = 2 * M_PI * j / p;
    Real _sin = sinl(x) * r, _cos = cosl(x) * r;
    bottoms[j] = V(_sin, 0, _cos);
    tops[j] = V(_sin, h, _cos);
  }
  for (uint64_t i = 0; i < p; ++i) {
    Vector vb[3] = {bottoms[i], bottomCenter, bottoms[(i + 1) % p]};
    Vector vt[3] = {tops[(i + 1) % p], topCenter, tops[i]};
    Vector backFace[4] = {tops[(i + 1) % p], tops[i], bottoms[i], bottoms[(i + 1) % p]};
    CSGPrimitiveSetsAppend(sets, (CSGPrimitive *)CSGPrimitiveTriangleCreate(vb, V(0, -1, 0)));
    CSGPrimitiveSetsAppend(sets, (CSGPrimitive *)CSGPrimitiveTriangleCreate(vt, V(0, 1, 0)));
    CSGPrimitiveSetsAppend(sets, (CSGPrimitive *)CSGPrimitivePlaneCreate(backFace, VectorTriangleNormal(backFace[0], backFace[1], backFace[2])));
  }
  free(tops);
  free(bottoms);
};

void CSGPrimitiveTriangularReduce(CSGSets *sets, const CSGCylinder *cylinder) {
  uint64_t p = cylinder->partition;
  Real r = cylinder->radius;
  Vector bottomCenter = V0;
  Vector topCenter = V(0, cylinder->height, 0);
  Vector *bottoms = (Vector *)calloc(p, sizeof(Vector));
  for (uint64_t j = 0; j < p; ++j) {
    Real x = 2 * M_PI * j / p;
    Real _sin = sinl(x) * r, _cos = cosl(x) * r;
    bottoms[j] = V(_sin, 0, _cos);
  }
  for (uint64_t i = 0; i < p; ++i) {
    Vector vb[3] = {bottoms[i], bottomCenter, bottoms[(i + 1) % p]};
    Vector backFace[3] = {bottoms[(i + 1) % p], topCenter, bottoms[i]};
    CSGPrimitiveSetsAppend(sets, (CSGPrimitive *)CSGPrimitiveTriangleCreate(vb, V(0, -1, 0)));
    CSGPrimitiveSetsAppend(sets, (CSGPrimitive *)CSGPrimitiveTriangleCreate(backFace, VectorTriangleNormal(backFace[0], backFace[1], backFace[2])));
  }
  free(bottoms);
};

void CSGPrimitiveBallReduce(CSGSets *sets, const CSGBall *ball) {
  uint64_t p = ball->partition;
  Real r = ball->radius;
  Real h = r * 2;
  uint64_t roundIndex;

  Vector *bottoms = (Vector *)calloc(p, sizeof(Vector));
  Vector *tops = (Vector *)calloc(p, sizeof(Vector));

  for (uint64_t hemisphereIndex = 0; hemisphereIndex < p; ++hemisphereIndex) {
    for (roundIndex = 0; roundIndex < p; ++roundIndex) {
      Real x, y, z, rad, radSurface;

      rad = 2 * M_PI * roundIndex / p;
      radSurface = M_PI * hemisphereIndex / p;

      x = sinl(rad) * sinl(radSurface) * r;
      y = (cosl(radSurface) + 1) / 2 * h;
      z = cosl(rad) * sinl(radSurface) * r;
      tops[roundIndex] = V(x, y, z);

      radSurface = M_PI * (Real)(hemisphereIndex + 1) / p;

      x = sinl(rad) * sinl(radSurface) * r;
      y = (cosl(radSurface) + 1) / 2 * h;
      z = cosl(rad) * sinl(radSurface) * r;
      bottoms[roundIndex] = V(x, y, z);
    }
    for (roundIndex = 0; roundIndex < p; ++roundIndex) {
      Vector backFace[4] = {tops[(roundIndex + 1) % p], tops[roundIndex], bottoms[roundIndex], bottoms[(roundIndex + 1) % p]};
      // Calculating surface normal, four points must be on same plane.
      CSGPrimitiveSetsAppend(sets, (CSGPrimitive *)CSGPrimitivePlaneCreate(backFace, VectorTriangleNormal(backFace[0], backFace[1], backFace[2])));
    }
  }

  free(tops);
  free(bottoms);
};

/**
 *
 * @param sets
 * @param primitive
 * @return Is CSGSets modified in this function?
 */
bool CSGPrimitiveReduce(CSGSets *sets, const CSGPrimitive *primitive) {
  switch (primitive->type) {
  case CSG_Plane:
    CSGPrimitivePlaneReduce(sets, (const CSGPlane *)primitive);
    break;
  case CSG_Cube:
    CSGPrimitiveCubeReduce(sets, (const CSGCube *)primitive);
    break;
  case CSG_Cylinder:
    CSGPrimitiveCylinderReduce(sets, (const CSGCylinder *)primitive);
    break;
  case CSG_Triangular:
    CSGPrimitiveTriangularReduce(sets, (const CSGTriangular *)primitive);
    break;
  case CSG_Ball:
    CSGPrimitiveBallReduce(sets, (const CSGBall *)primitive);
    break;
  case CSG_Sets:
  case CSG_Triangle:
    return false;
  }
  return true;
}

// FIXME: unnecessary loop
void CSGPrimitiveSetsReduce(CSGSets *sets) {
  LinkedList *prev = NULL, *cur = sets->primitives;

  while (cur != NULL) {
    if (CSGPrimitiveReduce(sets, cur->ptr) == true) {
      LinkedList *next = cur->next;
      // free reduced primitive
      free(cur->ptr);
      if (next != NULL) {
        // copy next node
        cur->ptr = next->ptr;
        cur->next = next->next;
        // free (old) next node
        free(next);
        // decrement root length
        --sets->primitives->length;
      } else if (prev != NULL) {
        --sets->primitives->length;
        prev->next = NULL;
      }
      cur = sets->primitives;
    } else {
      prev = cur;
      cur = cur->next;
    }
  }
}

Polygon *CSGPrimitiveSetsPolygon(const CSGSets *sets) {
  uint64_t triangle = sets->primitives->length;

  Polygon *polygon = (Polygon *)calloc(1, sizeof(Polygon));
  polygon->triangle = triangle;
  polygon->triangles = (Triangle *)calloc(triangle, sizeof(Triangle));

  uint64_t triangleIndex = 0;
  LinkedList *cur = sets->primitives;
  while (cur != NULL) {
    CSGTriangle *s = cur->ptr;
    assert(s->type == CSG_Triangle);
    Triangle *d = &polygon->triangles[triangleIndex];
    memcpy(d->vertexes, s->vertexes, sizeof(Vector) * 3);
    d->surfaceNormal = s->surfaceNormal;
    ++triangleIndex;
    cur = cur->next;
  }

  return polygon;
}