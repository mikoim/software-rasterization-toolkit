#ifndef RENDER_VECTOR_H
#define RENDER_VECTOR_H

#include "common.h"

typedef struct tagVector {
  Real x, y, z;
} Vector;

#define V(x, y, z)                                                                                                                                                                                     \
  (Vector) { x, y, z }

#define V0 V(0, 0, 0)
#define V1 V(1, 1, 1)

void VectorPrint(Vector v);
bool VectorCompare(Vector v1, Vector v2);
bool VectorCompareLoose(Vector v1, Vector v2, Real error);
Vector VectorNegative(Vector v);
Vector VectorAddition(Vector v1, Vector v2);
Vector VectorSubtraction(Vector v1, Vector v2);
Vector VectorScalarAddition(Vector v, Real value);
Vector VectorScalarSubtraction(Vector v, Real value);
Vector VectorScalarMultiplication(Vector v, Real value);
Vector VectorScalarDivision(Vector v, Real value);
Vector VectorCrossProduct(Vector v1, Vector v2); // 外積
Real VectorDotProduct(Vector v1, Vector v2);     // 内積
Real VectorEuclideanNorm(Vector v);              // L2 norm
Vector VectorL2Normalization(Vector v);          // vector x divided by L2 norm
Real VectorEuclideanDistance(Vector v1, Vector v2);
bool VectorInsideTriangle2D(Vector v, Vector v1, Vector v2, Vector v3);
Vector VectorBarycentricCoordinateWeight(Vector v, Vector v1, Vector v2, Vector v3);
Vector VectorConfine(Vector v, Real min, Real max);

#endif // RENDER_VECTOR_H
