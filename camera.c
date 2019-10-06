#include <math.h>
#include <stdlib.h>

#include "camera.h"

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
  Real aspect = (Real)c->image_width / c->image_height;
  Real range = c->far - c->near;
  Real __camera2projection[][4] = {{scale/aspect, 0, 0, 0}, {0, scale, 0, 0}, {0, 0, -c->far / range, -1}, {0, 0, -c->far * c->near / range, 0}};
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