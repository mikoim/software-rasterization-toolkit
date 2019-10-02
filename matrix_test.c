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
    Real _a[][2] = {{1, 2}, {3, 4}};
    Matrix *a = MatrixFromArray(2, 2, _a);
    Matrix *b = MatrixInverse(a);
    Real _z[][2] = {{-2, 1}, {1.5, -0.5}};
    Matrix *z = MatrixFromArray(2, 2, _z);
    assert(MatrixCompare(b, z));
    MatrixDestroy(a);
    MatrixDestroy(b);
    MatrixDestroy(z);
  }
  {
    Real _a[][3] = {{12, 59, 12}, {78, 28, 92}, {59, 63, 93}};
    Matrix *a = MatrixFromArray(3, 3, _a);
    Matrix *b = MatrixInverse(a);
    Real _z[][3] = {{0.0298614, 0.0442588, -0.047636}, {0.0170823, -0.00381687, 0.00157165}, {-0.0305162, -0.0254925, 0.0399087}};
    Matrix *z = MatrixFromArray(3, 3, _z);
    assert(MatrixCompareLoose(b, z, 0.01));
    MatrixDestroy(a);
    MatrixDestroy(b);
    MatrixDestroy(z);
  }
  {
    Real _a[][4] = {{84, 61, 40, 20}, {66, 97, 36, 5}, {14, 84, 30, 17}, {34, 32, 66, 33}};
    Matrix *a = MatrixFromArray(4, 4, _a);
    Matrix *b = MatrixInverse(a);
    Real _z[][4] = {
        {0.0165518, -0.0012969, -0.00842985, -0.00549223}, {-0.0011845, 0.00197605, 0.0128443, -0.0061983}, {-0.0345384, 0.034493, -0.0257954, 0.0289947}, {0.053172, -0.069566, 0.047821, -0.0160171}};
    Matrix *z = MatrixFromArray(4, 4, _z);
    assert(MatrixCompareLoose(b, z, 0.01));
    MatrixDestroy(a);
    MatrixDestroy(b);
    MatrixDestroy(z);
  }
  {
    Real _a[][5] = {{9, 0, 32, 33, 56}, {25, 16, 9, 68, 100}, {86, 98, 10, 35, 38}, {34, 57, 91, 18, 34}, {90, 91, 93, 12, 31}};
    Matrix *a = MatrixFromArray(5, 5, _a);
    Matrix *b = MatrixInverse(a);
    Real _z[][5] = {{0.198375, -0.114, 0.0641086, -0.042949, -0.022094},
                    {-0.249191, 0.141691, -0.0746726, 0.038531, 0.0423581},
                    {0.132184, -0.0802935, 0.0422365, -0.000378089, -0.0311311},
                    {0.748927, -0.453596, 0.29276, -0.027694, -0.21818},
                    {-0.53089, 0.331501, -0.206958, 0.0234383, 0.14991}};
    Matrix *z = MatrixFromArray(5, 5, _z);
    assert(MatrixCompareLoose(b, z, 0.01));
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