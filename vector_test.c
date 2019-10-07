#include <assert.h>

#include "vector.h"

int main() {
  {
    Vector a = VectorNegative(V(70, 94, 10));
    Vector b = V(-70, -94, -10);
    assert(VectorCompare(a, b));
  }
  {
    Vector a = VectorAddition(V(70, 94, 10), V(41, 6, 48));
    Vector b = V(111, 100, 58);
    assert(VectorCompare(a, b));
  }
  {
    Vector a = VectorSubtraction(V(70, 94, 10), V(41, 6, 48));
    Vector b = V(29, 88, -38);
    assert(VectorCompare(a, b));
  }
  {
    Vector a = VectorScalarAddition(V(70, 94, 10), 10);
    Vector b = V(80, 104, 20);
    assert(VectorCompare(a, b));
  }
  {
    Vector a = VectorScalarSubtraction(V(70, 94, 10), 10);
    Vector b = V(60, 84, 0);
    assert(VectorCompare(a, b));
  }
  {
    Vector a = VectorScalarMultiplication(V(70, 94, 10), 10);
    Vector b = V(700, 940, 100);
    assert(VectorCompare(a, b));
  }
  {
    Vector a = VectorScalarDivision(V(70, 90, 10), 10);
    Vector b = V(7, 9, 1);
    assert(VectorCompare(a, b));
  }
  {
    Real a = VectorDotProduct(V(70, 90, 10), V(41, 6, 48));
    Real b = 3890;
    assert(a == b);
  }
  {
    Real a = VectorEuclideanNorm(V(70, 90, 10));
    Real b = 114.455231422596;
    assert(a - b < 0.01 || a - b < -0.01);
  }
  {
    Vector a = VectorL2Normalization(V(70, 90, 10));
    Vector b = V(0.611593, 0.786334, 0.0873704);
    assert(VectorCompareLoose(a, b, 0.01));
  }
  {
    Real a = VectorEuclideanDistance(V(70, 90, 10), V(41, 6, 48));
    Real b = 96.6488;
    assert(a - b < 0.01 || a - b < -0.01);
  }
  {
    bool a = VectorInsideTriangle2D(V(50, 50, 0), V(50, 0, 0), V(0, 100, 0), V(100, 100, 0));
    assert(a);
  }
  {
    bool a = VectorInsideTriangle2D(V(25, 25, 0), V(50, 0, 0), V(0, 100, 0), V(100, 100, 0));
    assert(!a);
  }
  {
    Vector a = VectorBarycentricCoordinateWeight(V(3.33333, 3.33333, 0), V(0, 10, 0), V(0, 0, 0), V(10, 0, 0));
    Vector b = V(0.333, 0.333, 0.333);
    assert(VectorCompareLoose(a, b, 0.01));
  }
}