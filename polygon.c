#include <stdio.h>
#include <stdlib.h>

#include "polygon.h"

Polygon *PolygonReadSTL(const char *filename) {
  Polygon *new = calloc(1, sizeof(Polygon));
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL) {
    fprintf(stderr, "%s: can't open %s\n", __FUNCTION_NAME__, filename);
    return NULL;
  }
  fseek(fp, 80, SEEK_CUR); // skip STL header
  fread(&new->triangle, 4, 1, fp);
  new->triangles = (Triangle *)calloc(new->triangle, sizeof(Triangle));
  STLTriangle *t = calloc(new->triangle, sizeof(STLTriangle));
  fread(t, sizeof(STLTriangle), new->triangle, fp);
  for (uint32_t i = 0; i < new->triangle; ++i) {
    new->triangles[i].normal = (Vector){t[i].normal[0], t[i].normal[1], t[i].normal[2]};
    new->triangles[i].vertexes[0] = (Vector){t[i].vertexes[0][0], t[i].vertexes[0][1], t[i].vertexes[0][2]};
    new->triangles[i].vertexes[1] = (Vector){t[i].vertexes[1][0], t[i].vertexes[1][1], t[i].vertexes[1][2]};
    new->triangles[i].vertexes[2] = (Vector){t[i].vertexes[2][0], t[i].vertexes[2][1], t[i].vertexes[2][2]};
  }
  free(t);
  fclose(fp);
  return new;
}

bool PolygonDestroy(Polygon *polygon) {
  free(polygon->triangles);
  free(polygon);
  return true;
}
