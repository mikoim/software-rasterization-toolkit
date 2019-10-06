#ifndef RENDER_CAMERA_H
#define RENDER_CAMERA_H

#include "common.h"
#include "matrix.h"
#include "vector.h"

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

Camera *CameraPerspectiveProjection(Vector eye, Vector at, Vector up_v, uint16_t image_width, uint16_t image_height, Real near, Real far, Real fov);
bool CameraDestroy(Camera *camera);

#endif // RENDER_CAMERA_H
