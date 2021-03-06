#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "polygon.h"
#include "transformer.h"

bool TransformerUpdateTransformationMatrix(Transformer *transformer) {
  // translate
  Real _translate[][4] = {{1, 0, 0, transformer->location.x}, {0, 1, 0, transformer->location.y}, {0, 0, 1, transformer->location.z}, {0, 0, 0, 1}};
  Matrix *translate = MatrixFromArray(4, 4, _translate);

  // scale
  Real _scale[][4] = {{transformer->scale.x, 0, 0, 0}, {0, transformer->scale.y, 0, 0}, {0, 0, transformer->scale.z, 0}, {0, 0, 0, 1}};
  Matrix *scale = MatrixFromArray(4, 4, _scale);

  // rotate X axis
  Real _rotateX[][4] = {
      {1, 0, 0, 0}, {0, cosl((transformer->rotation.x)), -sinl((transformer->rotation.x)), 0}, {0, sinl((transformer->rotation.x)), cosl((transformer->rotation.x)), 0}, {0, 0, 0, 1}};
  Matrix *rotateX = MatrixFromArray(4, 4, _rotateX);

  // rotate Y axis
  Real _rotateY[][4] = {{cosl(transformer->rotation.y), 0, sinl(transformer->rotation.y), 0}, {0, 1, 0, 0}, {-sinl(transformer->rotation.y), 0, cosl(transformer->rotation.y), 0}, {0, 0, 0, 1}};
  Matrix *rotateY = MatrixFromArray(4, 4, _rotateY);

  // rotate Z axis
  Real _rotateZ[][4] = {{cosl(transformer->rotation.z), -sinl(transformer->rotation.z), 0, 0}, {sinl(transformer->rotation.z), cosl(transformer->rotation.z), 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
  Matrix *rotateZ = MatrixFromArray(4, 4, _rotateZ);

  Matrix *_a = MatrixMultiplication(translate, scale);
  Matrix *_b = MatrixMultiplication(_a, rotateX);
  Matrix *_c = MatrixMultiplication(_b, rotateY);
  Matrix *_d = MatrixMultiplication(_c, rotateZ);

  if (transformer->matrix != NULL) {
    MatrixDestroy(transformer->matrix);
  }
  transformer->matrix = _d;

  if (transformer->inverseMatrix != NULL) {
    MatrixDestroy(transformer->inverseMatrix);
  }
  transformer->inverseMatrix = MatrixInverse(_d);

  MatrixDestroy(_c);
  MatrixDestroy(_b);
  MatrixDestroy(_a);

  MatrixDestroy(rotateZ);
  MatrixDestroy(rotateY);
  MatrixDestroy(rotateX);

  MatrixDestroy(scale);
  MatrixDestroy(translate);

  return true;
}

Transformer *TransformerCreate(const Vector location, const Vector rotation, const Vector scale) {
  Transformer *t = (Transformer *)calloc(1, sizeof(Transformer));
  *t = (Transformer){location, rotation, scale, NULL, NULL};
  TransformerUpdateTransformationMatrix(t);
  return t;
}

bool TransformerDestroy(Transformer *transformer) {
  if (transformer == NULL) {
#ifndef NDEBUG
    fprintf(stderr, "%s: trying to free null pointer, ignored.\n", __FUNCTION_NAME__);
#endif
    return false;
  }
  MatrixDestroy(transformer->matrix);
  MatrixDestroy(transformer->inverseMatrix);
  free(transformer);
  return true;
}

Vector TransformerTransformPoint(const Transformer *transformer, const Vector point) {
  Real _prevPoint[][1] = {{point.x}, {point.y}, {point.z}, {1}};
  Matrix *prevPoint = MatrixFromArray(4, 1, _prevPoint);
  Matrix *translatedPoint = MatrixMultiplication(transformer->matrix, prevPoint);
  Vector np = V(MatrixGetElement(translatedPoint, 0, 0), MatrixGetElement(translatedPoint, 1, 0), MatrixGetElement(translatedPoint, 2, 0));
  MatrixDestroy(translatedPoint);
  MatrixDestroy(prevPoint);
  return np;
}

Triangle TransformerTransformTriangle(const Transformer *transformer, const Triangle triangle) {
  if (transformer == NULL) {
#ifndef NDEBUG
    fprintf(stderr, "%s: NULL pointer passed. ignored.\n", __FUNCTION_NAME__);
#endif
    return triangle;
  }
  Triangle new = triangle;
  new.vertexes[0] = TransformerTransformPoint(transformer, new.vertexes[0]);
  new.vertexes[1] = TransformerTransformPoint(transformer, new.vertexes[1]);
  new.vertexes[2] = TransformerTransformPoint(transformer, new.vertexes[2]);
  new.surfaceNormal = VectorTriangleNormal(new.vertexes[0], new.vertexes[1], new.vertexes[2]);
  new.vertexNormals[0] = VectorL2Normalization(TransformerTransformPoint(transformer, new.vertexNormals[0]));
  new.vertexNormals[1] = VectorL2Normalization(TransformerTransformPoint(transformer, new.vertexNormals[1]));
  new.vertexNormals[2] = VectorL2Normalization(TransformerTransformPoint(transformer, new.vertexNormals[2]));
  return new;
}

Vector TransformerDetransform(const Transformer *transformer, const Vector point) {
  Real _translatedPoint[][1] = {{point.x}, {point.y}, {point.z}, {1}};
  Matrix *translatedPoint = MatrixFromArray(4, 1, _translatedPoint);
  Matrix *prevPoint = MatrixMultiplication(transformer->inverseMatrix, translatedPoint);
  Vector np = V(MatrixGetElement(prevPoint, 0, 0), MatrixGetElement(prevPoint, 1, 0), MatrixGetElement(prevPoint, 2, 0));
  MatrixDestroy(prevPoint);
  MatrixDestroy(translatedPoint);
  return np;
}
