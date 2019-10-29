#include <float.h>
#include <math.h>
#include <stdio.h>

#include "vector.h"

void VectorPrint(const Vector v) { printf("{{%Lf},{%Lf},{%Lf}}\n", v.x, v.y, v.z); }

bool VectorCompare(const Vector v1, const Vector v2) { return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z; }

bool VectorCompareLoose(const Vector v1, const Vector v2, Real error) { return fabsl(v1.x - v2.x) < error && fabsl(v1.y - v2.y) < error && fabsl(v1.z - v2.z) < error; }

Vector VectorNegative(const Vector v) { return (Vector){-v.x, -v.y, -v.z}; }

Vector VectorAddition(const Vector v1, const Vector v2) { return (Vector){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }

Vector VectorSubtraction(const Vector v1, const Vector v2) { return (Vector){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }

Vector VectorScalarAddition(const Vector v, const Real value) { return (Vector){v.x + value, v.y + value, v.z + value}; }

Vector VectorScalarSubtraction(const Vector v, const Real value) { return (Vector){v.x - value, v.y - value, v.z - value}; }

Vector VectorScalarMultiplication(const Vector v, const Real value) { return (Vector){v.x * value, v.y * value, v.z * value}; }

Vector VectorScalarDivision(const Vector v, const Real value) { return (Vector){v.x / value, v.y / value, v.z / value}; }

Vector VectorCrossProduct(const Vector v1, const Vector v2) { return (Vector){-v1.z * v2.y + v1.y * v2.z, v1.z * v2.x - v1.x * v2.z, -v1.y * v2.x + v1.x * v2.y}; }

Real VectorDotProduct(const Vector v1, const Vector v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

Real VectorEuclideanNorm(const Vector v) { return sqrtl(v.x * v.x + v.y * v.y + v.z * v.z); }

Vector VectorL2Normalization(const Vector v) {
  Real norm = VectorEuclideanNorm(v);
  return VectorScalarDivision(v, norm == 0 ? LDBL_MAX : norm);
}

Real VectorEuclideanDistance(const Vector v1, const Vector v2) { return sqrtl((v2.x - v1.x) * (v2.x - v1.x) + (v2.y - v1.y) * (v2.y - v1.y) + (v2.z - v1.z) * (v2.z - v1.z)); }

bool VectorInsideTriangle2D(const Vector v, const Vector v1, const Vector v2, const Vector v3) {
  int8_t sign = v1.x * v2.y + v1.y * v3.x + v2.x * v3.y < v1.y * v2.x + v2.y * v3.x + v1.x * v3.y ? -1 : 1;
  Real a = (v1.y * (v3.x - v.x) + v3.y * v.x - v3.x * v.y + v1.x * (-v3.y + v.y));
  Real b = (v1.y * (v2.x - v.x) + v2.y * v.x - v2.x * v.y + v1.x * (-v2.y + v.y));
  Real c = (v2.y * (v3.x - v.x) + v3.y * v.x - v3.x * v.y + v2.x * (-v3.y + v.y));
  return a * sign > 0 && b * sign < 0 && c * sign < 0;
}

/**
 * https://codeplea.com/triangular-interpolation
 * https://en.wikipedia.org/wiki/Barycentric_coordinate_system
 * @param v
 * @param v1
 * @param v2
 * @param v3
 * @return
 */
Vector VectorBarycentricCoordinateWeight(const Vector v, const Vector v1, const Vector v2, const Vector v3) {
  Real w1 = -((-v2.x * v.y + v3.x * v.y + v.x * v2.y - v3.x * v2.y - v.x * v3.y + v2.x * v3.y) / (v2.x * v1.y - v3.x * v1.y - v1.x * v2.y + v3.x * v2.y + v1.x * v3.y - v2.x * v3.y));
  Real w2 = -((v1.x * v.y - v3.x * v.y - v.x * v1.y + v3.x * v1.y + v.x * v3.y - v1.x * v3.y) / (v2.x * v1.y - v3.x * v1.y - v1.x * v2.y + v3.x * v2.y + v1.x * v3.y - v2.x * v3.y));
  Real w3 = -((-v1.x * v.y + v2.x * v.y + v.x * v1.y - v2.x * v1.y - v.x * v2.y + v1.x * v2.y) / (v2.x * v1.y - v3.x * v1.y - v1.x * v2.y + v3.x * v2.y + v1.x * v3.y - v2.x * v3.y));
  return V(w1, w2, w3);
}

Vector VectorConfine(const Vector v, Real min, Real max) { return V(CONFINE(v.x, min, max), CONFINE(v.y, min, max), CONFINE(v.z, min, max)); }