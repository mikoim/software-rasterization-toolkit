#ifndef RENDER_VECTOR_H
#define RENDER_VECTOR_H

#include "common.h"

typedef struct {
  Real x, y, z;
} Vector;

void VectorPrint(const Vector *v);
bool VectorCompare(const Vector *v1, const Vector *v2);
bool VectorCompareLoose(const Vector *v1, const Vector *v2, Real error);
Vector VectorAddition(const Vector *v1, const Vector *v2);
Vector VectorSubtraction(const Vector *v1, const Vector *v2);
Vector VectorScalarMultiplication(const Vector *v, Real value);
Vector VectorScalarDivision(const Vector *v, Real value);
Vector VectorCrossProduct(const Vector *v1, const Vector *v2); // 外積
Real VectorDotProduct(const Vector *v1, const Vector *v2);     // 内積
Real VectorEuclideanNorm(const Vector *v);                     // L2 norm
Vector VectorL2Normalization(const Vector *v);                 // vector x divided by L2 norm
Real VectorEuclideanDistance(const Vector *v1, const Vector *v2);
bool VectorInsideTriangle2D(const Vector *v, const Vector *v1, const Vector *v2, const Vector *v3);

#endif // RENDER_VECTOR_H
