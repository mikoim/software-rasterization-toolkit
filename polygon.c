#include <stdio.h>
#include <stdlib.h>

#include "polygon.h"
#include "hashdict/hashdict.h"

Polygon *PolygonReadSTL(const char *filename) {
  Polygon *new = calloc(1, sizeof(Polygon));
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL) {
    fprintf(stderr, "%s: can't open %s\n", __FUNCTION_NAME__, filename);
    return NULL;
  }
  fseek(fp, 80, SEEK_CUR);         // skip STL header
  fread(&new->triangle, 4, 1, fp); // read number of triangles
  new->triangles = (Triangle *)calloc(new->triangle, sizeof(Triangle));
  STLTriangle *t = calloc(new->triangle, sizeof(STLTriangle));
  fread(t, sizeof(STLTriangle), new->triangle, fp); // read triangles
  for (uint64_t i = 0; i < new->triangle; ++i) {
    new->triangles[i].surfaceNormal = (Vector){t[i].surfaceNormal[0], t[i].surfaceNormal[1], t[i].surfaceNormal[2]};
    new->triangles[i].vertexes[0] = (Vector){t[i].vertexes[0][0], t[i].vertexes[0][1], t[i].vertexes[0][2]};
    new->triangles[i].vertexes[1] = (Vector){t[i].vertexes[1][0], t[i].vertexes[1][1], t[i].vertexes[1][2]};
    new->triangles[i].vertexes[2] = (Vector){t[i].vertexes[2][0], t[i].vertexes[2][1], t[i].vertexes[2][2]};
  }
  free(t);
  fclose(fp);
  return new;
}

/**
 * Calculate vertex normals
 * Warning: This function is NOT thread safe!
 * @param polygon
 * @return
 */
bool PolygonCalculateVertexNormals(Polygon *polygon) {
  const uint64_t triangle = polygon->triangle; // number of triangles
  const uint64_t vertex = triangle * 3;        // number of total vertexes
  Vector *vertexNormals = (Vector *)calloc(vertex, sizeof(Vector));
  struct dictionary* dic = dic_new(vertex);

  // accumulate surface normal vectors
  uint64_t a = 0;
  for (uint64_t triangleIndex = 0; triangleIndex < triangle; ++triangleIndex) {
    Triangle *t = &polygon->triangles[triangleIndex];
    for (uint64_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex) {
      Vector v = t->vertexes[vertexIndex];
      if (dic_add(dic, &v, sizeof(Vector))) {
        const uint64_t foo = *dic->value;
        vertexNormals[foo] = VectorAddition(vertexNormals[foo], t->surfaceNormal);
      } else {
        *dic->value = a;
        vertexNormals[a] = t->surfaceNormal;
        ++a;
      }
    }
  }

  // set vertex normal vectors
  for (uint64_t triangleIndex = 0; triangleIndex < triangle; ++triangleIndex) {
    Triangle *t = &polygon->triangles[triangleIndex];
    for (uint64_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex) {
      Vector v = t->vertexes[vertexIndex];
      if (dic_find(dic, &v, sizeof(Vector))) {
        const uint64_t foo = *dic->value;
        t->vertexNormals[vertexIndex] = VectorL2Normalization(vertexNormals[foo]);
      } else {
#ifndef NDEBUG
        fprintf(stderr, "%s: accumulated surface normal vector does not exist, may be bug.\n", __FUNCTION_NAME__);
#endif
      }
    }
  }

  dic_delete(dic);
  free(vertexNormals);
  return true;
}

bool PolygonDestroy(Polygon *polygon) {
  if (polygon == NULL) {
#ifndef NDEBUG
    fprintf(stderr, "%s: trying to free null pointer, ignored.\n", __FUNCTION_NAME__);
#endif
    return false;
  }
  free(polygon->triangles);
  free(polygon);
  return true;
}
