#include <assert.h>

#include "matrix.h"

int main() {
  {
    Matrix *a = MatrixZero(2, 3);
    Real _b[][3] = {{0, 0, 0}, {0, 0, 0}};
    Matrix *b = MatrixFromArray(2, 3, _b);
    assert(MatrixCompare(a, b));
    MatrixDestroy(a);
    MatrixDestroy(b);
  }
  {
    Matrix *a = MatrixIdentity(3);
    Real _b[][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    Matrix *b = MatrixFromArray(3, 3, _b);
    assert(MatrixCompare(a, b));
    MatrixDestroy(a);
    MatrixDestroy(b);
  }
  {
    Real _a[][3] = {{1, 2, 3}, {4, 5, 6}};
    Matrix *a = MatrixFromArray(2, 3, _a);
    Matrix *b = MatrixTranspose(a);
    Real _z[][2] = {{1, 4}, {2, 5}, {3, 6}};
    Matrix *z = MatrixFromArray(3, 2, _z);
    assert(MatrixCompare(b, z));
    MatrixDestroy(a);
    MatrixDestroy(b);
    MatrixDestroy(z);
  }
  {
    Matrix *a = MatrixIdentity(3);
    Real _b[][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    Matrix *b = MatrixFromArray(3, 3, _b);
    Matrix *c = MatrixAddition(a, b);
    Real _z[][3] = {{2, 2, 3}, {4, 6, 6}, {7, 8, 10}};
    Matrix *z = MatrixFromArray(3, 3, _z);
    assert(MatrixCompare(c, z));
    MatrixDestroy(a);
    MatrixDestroy(b);
    MatrixDestroy(c);
    MatrixDestroy(z);
  }
  {
    Matrix *a = MatrixIdentity(3);
    Real _b[][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    Matrix *b = MatrixFromArray(3, 3, _b);
    Matrix *c = MatrixSubtraction(a, b);
    Real _z[][3] = {{0, -2, -3}, {-4, -4, -6}, {-7, -8, -8}};
    Matrix *z = MatrixFromArray(3, 3, _z);
    assert(MatrixCompare(c, z));
    MatrixDestroy(a);
    MatrixDestroy(b);
    MatrixDestroy(c);
    MatrixDestroy(z);
  }
  {
    Matrix *a = MatrixIdentity(3);
    Real _b[][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    Matrix *b = MatrixFromArray(3, 3, _b);
    Matrix *c = MatrixMultiplication(a, b);
    Real _z[][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    Matrix *z = MatrixFromArray(3, 3, _z);
    assert(MatrixCompare(c, z));
    MatrixDestroy(a);
    MatrixDestroy(b);
    MatrixDestroy(c);
    MatrixDestroy(z);
  }
  {
    Matrix *a = MatrixIdentity(3);
    Matrix *b = MatrixScalarAddition(a, 1);
    Real _z[][3] = {{2, 1, 1}, {1, 2, 1}, {1, 1, 2}};
    Matrix *z = MatrixFromArray(3, 3, _z);
    assert(MatrixCompare(b, z));
    MatrixDestroy(a);
    MatrixDestroy(b);
    MatrixDestroy(z);
  }
  {
    Matrix *a = MatrixIdentity(3);
    Matrix *b = MatrixScalarSubtraction(a, 1);
    Real _z[][3] = {{0, -1, -1}, {-1, 0, -1}, {-1, -1, 0}};
    Matrix *z = MatrixFromArray(3, 3, _z);
    assert(MatrixCompare(b, z));
    MatrixDestroy(a);
    MatrixDestroy(b);
    MatrixDestroy(z);
  }
  {
    Matrix *a = MatrixIdentity(3);
    Matrix *b = MatrixScalarMultiplication(a, 10);
    Real _z[][3] = {{10, 0, 0}, {0, 10, 0}, {0, 0, 10}};
    Matrix *z = MatrixFromArray(3, 3, _z);
    assert(MatrixCompare(b, z));
    MatrixDestroy(a);
    MatrixDestroy(b);
    MatrixDestroy(z);
  }
  {
    Matrix *a = MatrixIdentity(3);
    Matrix *b = MatrixScalarDivision(a, 2);
    Real _z[][3] = {{0.5, 0, 0}, {0, 0.5, 0}, {0, 0, 0.5}};
    Matrix *z = MatrixFromArray(3, 3, _z);
    assert(MatrixCompare(b, z));
    MatrixDestroy(a);
    MatrixDestroy(b);
    MatrixDestroy(z);
  }
}