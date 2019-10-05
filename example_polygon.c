#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitmap.h"
#include "matrix.h"
#include "vector.h"

#pragma pack(push)
#pragma pack(1)
typedef struct tagSTLTriangle {
  float normal[3];
  float vertexes[3][3];
  uint16_t attribute;
} STLTriangle;
#pragma pack(pop)

typedef struct tagTriangle {
  Vector normal;
  Vector vertexes[3];
} Triangle;

typedef struct tagPolygon {
  uint32_t triangle;
  Triangle *triangles;
} Polygon;

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

/**
 * eye: Represent "camera position" in world space
 * at: Represent "target position" in world space
 * up_v: Represent "up direction" (unit vector)
 *
 * forward -> (z axis) = (eye - at) / |eye - at|
 * right -> (x axis) = (up_v x forward) / |up_v x forward|
 * up -> (y axis) = forward x right
 * NOTE: x is cross product operator in these formula.
 *
 * Rotation matrix -> r
 * | xx yx zx  0 |
 * | xy yy zy  0 |
 * | xz yz zz  0 |
 * |  0  0  0  1 |
 *
 * Translation matrix -> t
 * |      1      0      0      0 |
 * |      0      1      0      0 |
 * |      0      0      1      0 |
 * | -eye.x -eye.y -eye.z      1 |
 *
 * world2eye = (r.t)^-1
 */
typedef struct tagCamera {
  Vector eye;
  Vector at;
  Vector up_v;

  uint16_t image_width;
  uint16_t image_height;

  Real fov;       // perspective projection
  Real near, far; // both

  Matrix *world2camera;
  Matrix *camera2projection;
  Matrix *world2projection;
} Camera;

Camera *_CameraNew(Vector eye, Vector at, Vector up_v, uint16_t projection_width, uint16_t projection_height, Real near, Real far) {
  Vector z = VectorL2Normalization(VectorSubtraction(eye, at));
  Vector x = VectorL2Normalization(VectorCrossProduct(up_v, z));
  Vector y = VectorCrossProduct(z, x);

  Real ___world2camera[][4] = {{x.x, x.y, x.z, 0}, {y.x, y.y, y.z, 0}, {z.x, z.y, z.z, 0}, {-eye.x, -eye.y, -eye.z, 1}};
  Matrix *__world2camera = MatrixFromArray(4, 4, ___world2camera);
  Matrix *_world2camera = MatrixTranspose(__world2camera);
  Matrix *world2camera = MatrixInverse(_world2camera);
  MatrixDestroy(__world2camera);
  MatrixDestroy(_world2camera);

  Camera *new = calloc(1, sizeof(Camera));
  *new = (Camera){eye, at, up_v, projection_width, projection_height, 0, near, far, world2camera, NULL, NULL};
  return new;
}

Camera *CameraPerspectiveProjection(Vector eye, Vector at, Vector up_v, uint16_t image_width, uint16_t image_height, Real near, Real far, Real fov) {
  Camera *c = _CameraNew(eye, at, up_v, image_width, image_height, near, far);
  c->fov = fov;
  Real scale = 1 / tanl(c->fov * 0.5 * M_PI / 180);
  Real __camera2projection[][4] = {{scale, 0, 0, 0}, {0, scale, 0, 0}, {0, 0, -c->far / (c->far - c->near), -1}, {0, 0, -c->far * c->near / (c->far - c->near), 0}};
  Matrix *_camera2projection = MatrixFromArray(4, 4, __camera2projection);
  Matrix *camera2projection = MatrixTranspose(_camera2projection);
  MatrixDestroy(_camera2projection);
  c->camera2projection = camera2projection;
  c->world2projection = MatrixMultiplication(c->camera2projection, c->world2camera);
  return c;
}

bool CameraDestroy(Camera *camera) {
  free(camera->world2camera);
  free(camera->camera2projection);
  free(camera->world2projection);
  free(camera);
  return true;
}

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

void DrawLine(Bitmap *bitmap, const Vector v1, const Vector v2) {
  Vector unit = VectorL2Normalization(VectorSubtraction(v2, v1));
  Real length = VectorEuclideanDistance(v1, v2);
  Vector cur = v1;
  for (int i = 0; i <= length; ++i) {
    BitmapSetPixelColor(bitmap, cur.x, bitmap->dibHeader.bcHeight - cur.y, BMP_COLOR(255, 0, 0));
    cur = VectorAddition(cur, unit);
  }
}

int main() {
  const int w = 1000;
  const int h = 1000;
  Bitmap *bmp = BitmapNewImage(w, h);

  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      BitmapSetPixelColor(bmp, x, y, BMP_COLOR(0, 0, 0));
    }
  }

  Polygon *polygon = PolygonReadSTL("models/monkey.stl");

  /**
   * OpenGL (Right hand)
   *      y
   *      |
   *      |___ x
   *     /
   *    z
   *
   * DirectX (Left hand)
   *   y  z
   *   | /
   *   |/___ x
   */

  Camera *camera = CameraPerspectiveProjection(V(1, 0, 0), V(0, 0, 0), V(0, 1, 0), w, h, 0.1, 1000, 120);

  MatrixPrint(camera->world2camera);
  MatrixPrint(camera->camera2projection);
  MatrixPrint(camera->world2projection);

  for (uint32_t i = 0; i < polygon->triangle; ++i) {
    Triangle triangle = rasterize(camera, polygon->triangles[i]);
    DrawLine(bmp, triangle.vertexes[0], triangle.vertexes[1]);
    DrawLine(bmp, triangle.vertexes[1], triangle.vertexes[2]);
    DrawLine(bmp, triangle.vertexes[2], triangle.vertexes[0]);
  }

  CameraDestroy(camera);
  PolygonDestroy(polygon);

  BitmapWriteFile(bmp, "polygon.bmp");
  BitmapDestroy(bmp);

  return 0;
}