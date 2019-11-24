#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"

Matrix *MatrixZero(uint32_t rows, uint32_t columns) {
#ifndef NDEBUG
  if (rows == 0 || columns == 0) {
    fprintf(stderr, "%s: Invalid dimension (%dx%d >= 1x1)\n", __FUNCTION_NAME__, rows, columns);
    return NULL;
  }
#endif
  Matrix *new = (Matrix *)calloc(1, sizeof(Matrix));
  *new = (Matrix){rows, columns, NULL};
  new->matrix = (Real *)calloc(1, sizeof(Real) * rows * columns);
  return new;
}

bool MatrixDestroy(Matrix *a) {
#ifndef NDEBUG
  if (a == NULL) {
    fprintf(stderr, "%s: trying to free null pointer, ignored.\n", __FUNCTION_NAME__);
    return false;
  }
#endif
  free(a->matrix);
  free(a);
  return true; // TODO: implement error handling
}

Real MatrixGetElement(const Matrix *a, uint32_t rows, uint32_t columns) {
#ifndef NDEBUG
  if (a->rows <= rows || a->columns <= columns) {
    fprintf(stderr, "%s: Invalid element indices (rows:%d<%d, columns:%d<%d)\n", __FUNCTION_NAME__, rows, a->rows, columns, a->columns);
    return false;
  }
#endif
  return a->matrix[columns + a->columns * rows];
}

bool MatrixSetElement(Matrix *a, uint32_t rows, uint32_t columns, Real value) {
#ifndef NDEBUG
  if (a->rows <= rows || a->columns <= columns) {
    fprintf(stderr, "%s: Invalid element indices (rows:%d<%d, columns:%d<%d)\n", __FUNCTION_NAME__, rows, a->rows, columns, a->columns);
    return false;
  }
#endif
  a->matrix[columns + a->columns * rows] = value;
  return true;
}

void MatrixPrint(const Matrix *a) {
  printf("{");
  for (uint32_t row = 0; row < a->rows; ++row) {
    printf("{");
    for (uint32_t col = 0; col < a->columns; ++col) {
      printf(col < a->columns - 1 ? "%Lf," : "%Lf", MatrixGetElement(a, row, col));
    }
    printf("%s", row < a->rows - 1 ? "}," : "}");
  }
  printf("}\n");
}

bool MatrixCompare(const Matrix *a, const Matrix *b) {
#ifndef NDEBUG
  if (a->rows != b->rows || a->columns != b->columns) {
    fprintf(stderr, "%s: Invalid dimension (%dx%d == %dx%d)\n", __FUNCTION_NAME__, a->rows, a->columns, b->rows, b->columns);
    return false;
  }
#endif
  return memcmp(a->matrix, b->matrix, sizeof(Real) * a->rows * a->columns) == 0 ? true : false;
}

bool MatrixCompareLoose(const Matrix *a, const Matrix *b, Real error) {
#ifndef NDEBUG
  if (a->rows != b->rows || a->columns != b->columns) {
    fprintf(stderr, "%s: Invalid dimension (%dx%d == %dx%d)\n", __FUNCTION_NAME__, a->rows, a->columns, b->rows, b->columns);
    return false;
  }
#endif
  for (uint32_t row = 0; row < a->rows; ++row) {
    for (uint32_t col = 0; col < a->columns; ++col) {
      if (fabsl(MatrixGetElement(a, row, col) - MatrixGetElement(b, row, col)) >= error) {
        return false;
      }
    }
  }
  return true;
}

Matrix *MatrixClone(const Matrix *a) {
  Matrix *new = MatrixZero(a->rows, a->columns);
  memcpy(new->matrix, a->matrix, sizeof(Real) * new->rows * new->columns);
  return new;
}

Matrix *MatrixFromArray(uint32_t rows, uint32_t columns, const Real array[rows][columns]) {
  Matrix *new = MatrixZero(rows, columns);
  for (uint32_t row = 0; row < rows; ++row) {
    for (uint32_t col = 0; col < columns; ++col) {
      MatrixSetElement(new, row, col, array[row][col]);
    }
  }
  return new;
}

Matrix *MatrixIdentity(uint32_t n) {
#ifndef NDEBUG
  if (n == 0) {
    fprintf(stderr, "%s: Invalid dimension (0x0 >= 1x1)\n", __FUNCTION_NAME__);
    return NULL;
  }
#endif
  Matrix *new = MatrixZero(n, n);
  /*
   * n=1 -> 0
   * n=2 -> 0,3
   * n=3 -> 0,4,8
   * n=4 -> 0,5,10,15
   * n=x -> 0,x+1,2*(x+1),3*(n+1),...
   */
  for (uint32_t index = 0; index < n; ++index) {
    MatrixSetElement(new, index, index, 1);
  }
  return new;
}

Matrix *MatrixTranspose(const Matrix *a) {
  Matrix *new = MatrixZero(a->columns, a->rows);
  for (uint32_t row = 0; row < a->columns; ++row) {
    for (uint32_t col = 0; col < a->rows; ++col) {
      MatrixSetElement(new, row, col, MatrixGetElement(a, col, row));
    }
  }
  return new;
}

Real MatrixDeterminant(const Matrix *a) {
#ifndef NDEBUG
  if (a->rows != a->columns || a->rows == 0 || a->rows > 5) {
    fprintf(stderr, "%s: Unsupported dimension (%dx%d)\n", __FUNCTION_NAME__, a->rows, a->columns);
    return 0;
  }
#endif
  Real *m = a->matrix;
  switch (a->rows) {
  case 1:
    return m[0];
  case 2:
    return -m[1] * m[2] + m[0] * m[3];
  case 3:
    return -m[2] * m[4] * m[6] + m[1] * m[5] * m[6] + m[2] * m[3] * m[7] - m[0] * m[5] * m[7] - m[1] * m[3] * m[8] + m[0] * m[4] * m[8];
  case 4:
    return m[1] * m[7] * m[10] * m[12] - m[1] * m[6] * m[11] * m[12] - m[0] * m[7] * m[10] * m[13] + m[0] * m[6] * m[11] * m[13] - m[1] * m[7] * m[8] * m[14] + m[0] * m[7] * m[9] * m[14] +
           m[1] * m[4] * m[11] * m[14] - m[0] * m[5] * m[11] * m[14] +
           m[3] * (m[6] * m[9] * m[12] - m[5] * m[10] * m[12] - m[6] * m[8] * m[13] + m[4] * m[10] * m[13] + m[5] * m[8] * m[14] - m[4] * m[9] * m[14]) +
           (m[1] * m[6] * m[8] - m[0] * m[6] * m[9] - m[1] * m[4] * m[10] + m[0] * m[5] * m[10]) * m[15] +
           m[2] * (-m[7] * m[9] * m[12] + m[5] * m[11] * m[12] + m[7] * m[8] * m[13] - m[4] * m[11] * m[13] - m[5] * m[8] * m[15] + m[4] * m[9] * m[15]);
  case 5:
    return m[2] * m[9] * m[13] * m[16] * m[20] - m[2] * m[8] * m[14] * m[16] * m[20] - m[1] * m[9] * m[13] * m[17] * m[20] + m[1] * m[8] * m[14] * m[17] * m[20] - m[2] * m[9] * m[11] * m[18] * m[20] +
           m[1] * m[9] * m[12] * m[18] * m[20] + m[2] * m[6] * m[14] * m[18] * m[20] - m[1] * m[7] * m[14] * m[18] * m[20] + m[2] * m[8] * m[11] * m[19] * m[20] - m[1] * m[8] * m[12] * m[19] * m[20] -
           m[2] * m[6] * m[13] * m[19] * m[20] + m[1] * m[7] * m[13] * m[19] * m[20] - m[2] * m[9] * m[13] * m[15] * m[21] + m[2] * m[8] * m[14] * m[15] * m[21] + m[0] * m[9] * m[13] * m[17] * m[21] -
           m[0] * m[8] * m[14] * m[17] * m[21] + m[2] * m[9] * m[10] * m[18] * m[21] - m[0] * m[9] * m[12] * m[18] * m[21] - m[2] * m[5] * m[14] * m[18] * m[21] + m[0] * m[7] * m[14] * m[18] * m[21] -
           m[2] * m[8] * m[10] * m[19] * m[21] + m[0] * m[8] * m[12] * m[19] * m[21] + m[2] * m[5] * m[13] * m[19] * m[21] - m[0] * m[7] * m[13] * m[19] * m[21] + m[1] * m[9] * m[13] * m[15] * m[22] -
           m[1] * m[8] * m[14] * m[15] * m[22] - m[0] * m[9] * m[13] * m[16] * m[22] + m[0] * m[8] * m[14] * m[16] * m[22] - m[1] * m[9] * m[10] * m[18] * m[22] + m[0] * m[9] * m[11] * m[18] * m[22] +
           m[1] * m[5] * m[14] * m[18] * m[22] - m[0] * m[6] * m[14] * m[18] * m[22] + m[1] * m[8] * m[10] * m[19] * m[22] - m[0] * m[8] * m[11] * m[19] * m[22] - m[1] * m[5] * m[13] * m[19] * m[22] +
           m[0] * m[6] * m[13] * m[19] * m[22] + m[2] * m[9] * m[11] * m[15] * m[23] - m[1] * m[9] * m[12] * m[15] * m[23] - m[2] * m[6] * m[14] * m[15] * m[23] + m[1] * m[7] * m[14] * m[15] * m[23] -
           m[2] * m[9] * m[10] * m[16] * m[23] + m[0] * m[9] * m[12] * m[16] * m[23] + m[2] * m[5] * m[14] * m[16] * m[23] - m[0] * m[7] * m[14] * m[16] * m[23] + m[1] * m[9] * m[10] * m[17] * m[23] -
           m[0] * m[9] * m[11] * m[17] * m[23] - m[1] * m[5] * m[14] * m[17] * m[23] + m[0] * m[6] * m[14] * m[17] * m[23] + m[2] * m[6] * m[10] * m[19] * m[23] - m[1] * m[7] * m[10] * m[19] * m[23] -
           m[2] * m[5] * m[11] * m[19] * m[23] + m[0] * m[7] * m[11] * m[19] * m[23] + m[1] * m[5] * m[12] * m[19] * m[23] - m[0] * m[6] * m[12] * m[19] * m[23] +
           m[4] * (m[6] * m[13] * m[17] * m[20] - m[6] * m[12] * m[18] * m[20] - m[5] * m[13] * m[17] * m[21] + m[5] * m[12] * m[18] * m[21] - m[6] * m[13] * m[15] * m[22] +
                   m[5] * m[13] * m[16] * m[22] + m[6] * m[10] * m[18] * m[22] - m[5] * m[11] * m[18] * m[22] +
                   m[8] * (m[12] * m[16] * m[20] - m[11] * m[17] * m[20] - m[12] * m[15] * m[21] + m[10] * m[17] * m[21] + m[11] * m[15] * m[22] - m[10] * m[16] * m[22]) +
                   (m[6] * m[12] * m[15] - m[5] * m[12] * m[16] - m[6] * m[10] * m[17] + m[5] * m[11] * m[17]) * m[23] +
                   m[7] * (-m[13] * m[16] * m[20] + m[11] * m[18] * m[20] + m[13] * m[15] * m[21] - m[10] * m[18] * m[21] - m[11] * m[15] * m[23] + m[10] * m[16] * m[23])) +
           (m[2] * (-m[8] * m[11] * m[15] + m[6] * m[13] * m[15] + m[8] * m[10] * m[16] - m[5] * m[13] * m[16] - m[6] * m[10] * m[18] + m[5] * m[11] * m[18]) +
            m[1] * (m[8] * m[12] * m[15] - m[7] * m[13] * m[15] - m[8] * m[10] * m[17] + m[5] * m[13] * m[17] + m[7] * m[10] * m[18] - m[5] * m[12] * m[18]) +
            m[0] * (-m[8] * m[12] * m[16] + m[7] * m[13] * m[16] + m[8] * m[11] * m[17] - m[6] * m[13] * m[17] - m[7] * m[11] * m[18] + m[6] * m[12] * m[18])) *
               m[24] +
           m[3] * (-m[6] * m[14] * m[17] * m[20] + m[6] * m[12] * m[19] * m[20] + m[5] * m[14] * m[17] * m[21] - m[5] * m[12] * m[19] * m[21] + m[6] * m[14] * m[15] * m[22] -
                   m[5] * m[14] * m[16] * m[22] - m[6] * m[10] * m[19] * m[22] + m[5] * m[11] * m[19] * m[22] +
                   m[9] * (-m[12] * m[16] * m[20] + m[11] * m[17] * m[20] + m[12] * m[15] * m[21] - m[10] * m[17] * m[21] - m[11] * m[15] * m[22] + m[10] * m[16] * m[22]) -
                   m[6] * m[12] * m[15] * m[24] + m[5] * m[12] * m[16] * m[24] + m[6] * m[10] * m[17] * m[24] - m[5] * m[11] * m[17] * m[24] +
                   m[7] * (m[14] * m[16] * m[20] - m[11] * m[19] * m[20] - m[14] * m[15] * m[21] + m[10] * m[19] * m[21] + m[11] * m[15] * m[24] - m[10] * m[16] * m[24]));
  default:
    fprintf(stderr, "%s: Unsupported dimension (%dx%d). BUG!\n", __FUNCTION_NAME__, a->rows, a->columns);
    return 0;
  }
}

Matrix *MatrixInverse(const Matrix *a) {
#ifndef NDEBUG
  if (a->rows != a->columns || a->rows == 0 || a->rows > 5) {
    fprintf(stderr, "%s: Unsupported dimension (%dx%d)\n", __FUNCTION_NAME__, a->rows, a->columns);
    return NULL;
  }
#endif
  Real *m = a->matrix;
  Real d = MatrixDeterminant(a);
  if (a->rows == 1) {
    Real new[][1] = {{1 / d}};
    return MatrixFromArray(a->rows, a->columns, new);
  } else if (a->rows == 2) {
    Real new[][2] = {{m[3] / d, -m[1] / d}, {-m[2] / d, m[0] / d}};
    return MatrixFromArray(a->rows, a->columns, new);
  } else if (a->rows == 3) {
    Real new[][3] = {{(-m[5] * m[7] + m[4] * m[8]) / d, (m[2] * m[7] - m[1] * m[8]) / d, (-m[2] * m[4] + m[1] * m[5]) / d},
                     {(m[5] * m[6] - m[3] * m[8]) / d, (-m[2] * m[6] + m[0] * m[8]) / d, (m[2] * m[3] - m[0] * m[5]) / d},
                     {(-m[4] * m[6] + m[3] * m[7]) / d, (m[1] * m[6] - m[0] * m[7]) / d, (-m[1] * m[3] + m[0] * m[4]) / d}};
    return MatrixFromArray(a->rows, a->columns, new);
  } else if (a->rows == 4) {
    Real new[][4] = {{(-m[7] * m[10] * m[13] + m[6] * m[11] * m[13] + m[7] * m[9] * m[14] - m[5] * m[11] * m[14] - m[6] * m[9] * m[15] + m[5] * m[10] * m[15]) / d,
                      (m[3] * m[10] * m[13] - m[2] * m[11] * m[13] - m[3] * m[9] * m[14] + m[1] * m[11] * m[14] + m[2] * m[9] * m[15] - m[1] * m[10] * m[15]) / d,
                      (-m[3] * m[6] * m[13] + m[2] * m[7] * m[13] + m[3] * m[5] * m[14] - m[1] * m[7] * m[14] - m[2] * m[5] * m[15] + m[1] * m[6] * m[15]) / d,
                      (m[3] * m[6] * m[9] - m[2] * m[7] * m[9] - m[3] * m[5] * m[10] + m[1] * m[7] * m[10] + m[2] * m[5] * m[11] - m[1] * m[6] * m[11]) / d},
                     {(m[7] * m[10] * m[12] - m[6] * m[11] * m[12] - m[7] * m[8] * m[14] + m[4] * m[11] * m[14] + m[6] * m[8] * m[15] - m[4] * m[10] * m[15]) / d,
                      (-m[3] * m[10] * m[12] + m[2] * m[11] * m[12] + m[3] * m[8] * m[14] - m[0] * m[11] * m[14] - m[2] * m[8] * m[15] + m[0] * m[10] * m[15]) / d,
                      (m[3] * m[6] * m[12] - m[2] * m[7] * m[12] - m[3] * m[4] * m[14] + m[0] * m[7] * m[14] + m[2] * m[4] * m[15] - m[0] * m[6] * m[15]) / d,
                      (-m[3] * m[6] * m[8] + m[2] * m[7] * m[8] + m[3] * m[4] * m[10] - m[0] * m[7] * m[10] - m[2] * m[4] * m[11] + m[0] * m[6] * m[11]) / d},
                     {(-m[7] * m[9] * m[12] + m[5] * m[11] * m[12] + m[7] * m[8] * m[13] - m[4] * m[11] * m[13] - m[5] * m[8] * m[15] + m[4] * m[9] * m[15]) / d,
                      (m[3] * m[9] * m[12] - m[1] * m[11] * m[12] - m[3] * m[8] * m[13] + m[0] * m[11] * m[13] + m[1] * m[8] * m[15] - m[0] * m[9] * m[15]) / d,
                      (-m[3] * m[5] * m[12] + m[1] * m[7] * m[12] + m[3] * m[4] * m[13] - m[0] * m[7] * m[13] - m[1] * m[4] * m[15] + m[0] * m[5] * m[15]) / d,
                      (m[3] * m[5] * m[8] - m[1] * m[7] * m[8] - m[3] * m[4] * m[9] + m[0] * m[7] * m[9] + m[1] * m[4] * m[11] - m[0] * m[5] * m[11]) / d},
                     {(m[6] * m[9] * m[12] - m[5] * m[10] * m[12] - m[6] * m[8] * m[13] + m[4] * m[10] * m[13] + m[5] * m[8] * m[14] - m[4] * m[9] * m[14]) / d,
                      (-m[2] * m[9] * m[12] + m[1] * m[10] * m[12] + m[2] * m[8] * m[13] - m[0] * m[10] * m[13] - m[1] * m[8] * m[14] + m[0] * m[9] * m[14]) / d,
                      (m[2] * m[5] * m[12] - m[1] * m[6] * m[12] - m[2] * m[4] * m[13] + m[0] * m[6] * m[13] + m[1] * m[4] * m[14] - m[0] * m[5] * m[14]) / d,
                      (-m[2] * m[5] * m[8] + m[1] * m[6] * m[8] + m[2] * m[4] * m[9] - m[0] * m[6] * m[9] - m[1] * m[4] * m[10] + m[0] * m[5] * m[10]) / d}};
    return MatrixFromArray(a->rows, a->columns, new);
  } else {
    Real new[][5] = {
        {(m[9] * m[13] * m[17] * m[21] - m[8] * m[14] * m[17] * m[21] - m[9] * m[12] * m[18] * m[21] + m[7] * m[14] * m[18] * m[21] + m[8] * m[12] * m[19] * m[21] - m[7] * m[13] * m[19] * m[21] -
          m[9] * m[13] * m[16] * m[22] + m[8] * m[14] * m[16] * m[22] + m[9] * m[11] * m[18] * m[22] - m[6] * m[14] * m[18] * m[22] - m[8] * m[11] * m[19] * m[22] + m[6] * m[13] * m[19] * m[22] +
          m[9] * m[12] * m[16] * m[23] - m[7] * m[14] * m[16] * m[23] - m[9] * m[11] * m[17] * m[23] + m[6] * m[14] * m[17] * m[23] + m[7] * m[11] * m[19] * m[23] - m[6] * m[12] * m[19] * m[23] -
          m[8] * m[12] * m[16] * m[24] + m[7] * m[13] * m[16] * m[24] + m[8] * m[11] * m[17] * m[24] - m[6] * m[13] * m[17] * m[24] - m[7] * m[11] * m[18] * m[24] + m[6] * m[12] * m[18] * m[24]) /
             d,
         (-m[4] * m[13] * m[17] * m[21] + m[3] * m[14] * m[17] * m[21] + m[4] * m[12] * m[18] * m[21] - m[2] * m[14] * m[18] * m[21] - m[3] * m[12] * m[19] * m[21] + m[2] * m[13] * m[19] * m[21] +
          m[4] * m[13] * m[16] * m[22] - m[3] * m[14] * m[16] * m[22] - m[4] * m[11] * m[18] * m[22] + m[1] * m[14] * m[18] * m[22] + m[3] * m[11] * m[19] * m[22] - m[1] * m[13] * m[19] * m[22] -
          m[4] * m[12] * m[16] * m[23] + m[2] * m[14] * m[16] * m[23] + m[4] * m[11] * m[17] * m[23] - m[1] * m[14] * m[17] * m[23] - m[2] * m[11] * m[19] * m[23] + m[1] * m[12] * m[19] * m[23] +
          m[3] * m[12] * m[16] * m[24] - m[2] * m[13] * m[16] * m[24] - m[3] * m[11] * m[17] * m[24] + m[1] * m[13] * m[17] * m[24] + m[2] * m[11] * m[18] * m[24] - m[1] * m[12] * m[18] * m[24]) /
             d,
         (m[4] * m[8] * m[17] * m[21] - m[3] * m[9] * m[17] * m[21] - m[4] * m[7] * m[18] * m[21] + m[2] * m[9] * m[18] * m[21] + m[3] * m[7] * m[19] * m[21] - m[2] * m[8] * m[19] * m[21] -
          m[4] * m[8] * m[16] * m[22] + m[3] * m[9] * m[16] * m[22] + m[4] * m[6] * m[18] * m[22] - m[1] * m[9] * m[18] * m[22] - m[3] * m[6] * m[19] * m[22] + m[1] * m[8] * m[19] * m[22] +
          m[4] * m[7] * m[16] * m[23] - m[2] * m[9] * m[16] * m[23] - m[4] * m[6] * m[17] * m[23] + m[1] * m[9] * m[17] * m[23] + m[2] * m[6] * m[19] * m[23] - m[1] * m[7] * m[19] * m[23] -
          m[3] * m[7] * m[16] * m[24] + m[2] * m[8] * m[16] * m[24] + m[3] * m[6] * m[17] * m[24] - m[1] * m[8] * m[17] * m[24] - m[2] * m[6] * m[18] * m[24] + m[1] * m[7] * m[18] * m[24]) /
             d,
         (-m[4] * m[8] * m[12] * m[21] + m[3] * m[9] * m[12] * m[21] + m[4] * m[7] * m[13] * m[21] - m[2] * m[9] * m[13] * m[21] - m[3] * m[7] * m[14] * m[21] + m[2] * m[8] * m[14] * m[21] +
          m[4] * m[8] * m[11] * m[22] - m[3] * m[9] * m[11] * m[22] - m[4] * m[6] * m[13] * m[22] + m[1] * m[9] * m[13] * m[22] + m[3] * m[6] * m[14] * m[22] - m[1] * m[8] * m[14] * m[22] -
          m[4] * m[7] * m[11] * m[23] + m[2] * m[9] * m[11] * m[23] + m[4] * m[6] * m[12] * m[23] - m[1] * m[9] * m[12] * m[23] - m[2] * m[6] * m[14] * m[23] + m[1] * m[7] * m[14] * m[23] +
          m[3] * m[7] * m[11] * m[24] - m[2] * m[8] * m[11] * m[24] - m[3] * m[6] * m[12] * m[24] + m[1] * m[8] * m[12] * m[24] + m[2] * m[6] * m[13] * m[24] - m[1] * m[7] * m[13] * m[24]) /
             d,
         (m[4] * m[8] * m[12] * m[16] - m[3] * m[9] * m[12] * m[16] - m[4] * m[7] * m[13] * m[16] + m[2] * m[9] * m[13] * m[16] + m[3] * m[7] * m[14] * m[16] - m[2] * m[8] * m[14] * m[16] -
          m[4] * m[8] * m[11] * m[17] + m[3] * m[9] * m[11] * m[17] + m[4] * m[6] * m[13] * m[17] - m[1] * m[9] * m[13] * m[17] - m[3] * m[6] * m[14] * m[17] + m[1] * m[8] * m[14] * m[17] +
          m[4] * m[7] * m[11] * m[18] - m[2] * m[9] * m[11] * m[18] - m[4] * m[6] * m[12] * m[18] + m[1] * m[9] * m[12] * m[18] + m[2] * m[6] * m[14] * m[18] - m[1] * m[7] * m[14] * m[18] -
          m[3] * m[7] * m[11] * m[19] + m[2] * m[8] * m[11] * m[19] + m[3] * m[6] * m[12] * m[19] - m[1] * m[8] * m[12] * m[19] - m[2] * m[6] * m[13] * m[19] + m[1] * m[7] * m[13] * m[19]) /
             d},
        {(-m[9] * m[13] * m[17] * m[20] + m[8] * m[14] * m[17] * m[20] + m[9] * m[12] * m[18] * m[20] - m[7] * m[14] * m[18] * m[20] - m[8] * m[12] * m[19] * m[20] + m[7] * m[13] * m[19] * m[20] +
          m[9] * m[13] * m[15] * m[22] - m[8] * m[14] * m[15] * m[22] - m[9] * m[10] * m[18] * m[22] + m[5] * m[14] * m[18] * m[22] + m[8] * m[10] * m[19] * m[22] - m[5] * m[13] * m[19] * m[22] -
          m[9] * m[12] * m[15] * m[23] + m[7] * m[14] * m[15] * m[23] + m[9] * m[10] * m[17] * m[23] - m[5] * m[14] * m[17] * m[23] - m[7] * m[10] * m[19] * m[23] + m[5] * m[12] * m[19] * m[23] +
          m[8] * m[12] * m[15] * m[24] - m[7] * m[13] * m[15] * m[24] - m[8] * m[10] * m[17] * m[24] + m[5] * m[13] * m[17] * m[24] + m[7] * m[10] * m[18] * m[24] - m[5] * m[12] * m[18] * m[24]) /
             d,
         (m[4] * m[13] * m[17] * m[20] - m[3] * m[14] * m[17] * m[20] - m[4] * m[12] * m[18] * m[20] + m[2] * m[14] * m[18] * m[20] + m[3] * m[12] * m[19] * m[20] - m[2] * m[13] * m[19] * m[20] -
          m[4] * m[13] * m[15] * m[22] + m[3] * m[14] * m[15] * m[22] + m[4] * m[10] * m[18] * m[22] - m[0] * m[14] * m[18] * m[22] - m[3] * m[10] * m[19] * m[22] + m[0] * m[13] * m[19] * m[22] +
          m[4] * m[12] * m[15] * m[23] - m[2] * m[14] * m[15] * m[23] - m[4] * m[10] * m[17] * m[23] + m[0] * m[14] * m[17] * m[23] + m[2] * m[10] * m[19] * m[23] - m[0] * m[12] * m[19] * m[23] -
          m[3] * m[12] * m[15] * m[24] + m[2] * m[13] * m[15] * m[24] + m[3] * m[10] * m[17] * m[24] - m[0] * m[13] * m[17] * m[24] - m[2] * m[10] * m[18] * m[24] + m[0] * m[12] * m[18] * m[24]) /
             d,
         (-m[4] * m[8] * m[17] * m[20] + m[3] * m[9] * m[17] * m[20] + m[4] * m[7] * m[18] * m[20] - m[2] * m[9] * m[18] * m[20] - m[3] * m[7] * m[19] * m[20] + m[2] * m[8] * m[19] * m[20] +
          m[4] * m[8] * m[15] * m[22] - m[3] * m[9] * m[15] * m[22] - m[4] * m[5] * m[18] * m[22] + m[0] * m[9] * m[18] * m[22] + m[3] * m[5] * m[19] * m[22] - m[0] * m[8] * m[19] * m[22] -
          m[4] * m[7] * m[15] * m[23] + m[2] * m[9] * m[15] * m[23] + m[4] * m[5] * m[17] * m[23] - m[0] * m[9] * m[17] * m[23] - m[2] * m[5] * m[19] * m[23] + m[0] * m[7] * m[19] * m[23] +
          m[3] * m[7] * m[15] * m[24] - m[2] * m[8] * m[15] * m[24] - m[3] * m[5] * m[17] * m[24] + m[0] * m[8] * m[17] * m[24] + m[2] * m[5] * m[18] * m[24] - m[0] * m[7] * m[18] * m[24]) /
             d,
         (m[4] * m[8] * m[12] * m[20] - m[3] * m[9] * m[12] * m[20] - m[4] * m[7] * m[13] * m[20] + m[2] * m[9] * m[13] * m[20] + m[3] * m[7] * m[14] * m[20] - m[2] * m[8] * m[14] * m[20] -
          m[4] * m[8] * m[10] * m[22] + m[3] * m[9] * m[10] * m[22] + m[4] * m[5] * m[13] * m[22] - m[0] * m[9] * m[13] * m[22] - m[3] * m[5] * m[14] * m[22] + m[0] * m[8] * m[14] * m[22] +
          m[4] * m[7] * m[10] * m[23] - m[2] * m[9] * m[10] * m[23] - m[4] * m[5] * m[12] * m[23] + m[0] * m[9] * m[12] * m[23] + m[2] * m[5] * m[14] * m[23] - m[0] * m[7] * m[14] * m[23] -
          m[3] * m[7] * m[10] * m[24] + m[2] * m[8] * m[10] * m[24] + m[3] * m[5] * m[12] * m[24] - m[0] * m[8] * m[12] * m[24] - m[2] * m[5] * m[13] * m[24] + m[0] * m[7] * m[13] * m[24]) /
             d,
         (-m[4] * m[8] * m[12] * m[15] + m[3] * m[9] * m[12] * m[15] + m[4] * m[7] * m[13] * m[15] - m[2] * m[9] * m[13] * m[15] - m[3] * m[7] * m[14] * m[15] + m[2] * m[8] * m[14] * m[15] +
          m[4] * m[8] * m[10] * m[17] - m[3] * m[9] * m[10] * m[17] - m[4] * m[5] * m[13] * m[17] + m[0] * m[9] * m[13] * m[17] + m[3] * m[5] * m[14] * m[17] - m[0] * m[8] * m[14] * m[17] -
          m[4] * m[7] * m[10] * m[18] + m[2] * m[9] * m[10] * m[18] + m[4] * m[5] * m[12] * m[18] - m[0] * m[9] * m[12] * m[18] - m[2] * m[5] * m[14] * m[18] + m[0] * m[7] * m[14] * m[18] +
          m[3] * m[7] * m[10] * m[19] - m[2] * m[8] * m[10] * m[19] - m[3] * m[5] * m[12] * m[19] + m[0] * m[8] * m[12] * m[19] + m[2] * m[5] * m[13] * m[19] - m[0] * m[7] * m[13] * m[19]) /
             d},
        {(m[9] * m[13] * m[16] * m[20] - m[8] * m[14] * m[16] * m[20] - m[9] * m[11] * m[18] * m[20] + m[6] * m[14] * m[18] * m[20] + m[8] * m[11] * m[19] * m[20] - m[6] * m[13] * m[19] * m[20] -
          m[9] * m[13] * m[15] * m[21] + m[8] * m[14] * m[15] * m[21] + m[9] * m[10] * m[18] * m[21] - m[5] * m[14] * m[18] * m[21] - m[8] * m[10] * m[19] * m[21] + m[5] * m[13] * m[19] * m[21] +
          m[9] * m[11] * m[15] * m[23] - m[6] * m[14] * m[15] * m[23] - m[9] * m[10] * m[16] * m[23] + m[5] * m[14] * m[16] * m[23] + m[6] * m[10] * m[19] * m[23] - m[5] * m[11] * m[19] * m[23] -
          m[8] * m[11] * m[15] * m[24] + m[6] * m[13] * m[15] * m[24] + m[8] * m[10] * m[16] * m[24] - m[5] * m[13] * m[16] * m[24] - m[6] * m[10] * m[18] * m[24] + m[5] * m[11] * m[18] * m[24]) /
             d,
         (-m[4] * m[13] * m[16] * m[20] + m[3] * m[14] * m[16] * m[20] + m[4] * m[11] * m[18] * m[20] - m[1] * m[14] * m[18] * m[20] - m[3] * m[11] * m[19] * m[20] + m[1] * m[13] * m[19] * m[20] +
          m[4] * m[13] * m[15] * m[21] - m[3] * m[14] * m[15] * m[21] - m[4] * m[10] * m[18] * m[21] + m[0] * m[14] * m[18] * m[21] + m[3] * m[10] * m[19] * m[21] - m[0] * m[13] * m[19] * m[21] -
          m[4] * m[11] * m[15] * m[23] + m[1] * m[14] * m[15] * m[23] + m[4] * m[10] * m[16] * m[23] - m[0] * m[14] * m[16] * m[23] - m[1] * m[10] * m[19] * m[23] + m[0] * m[11] * m[19] * m[23] +
          m[3] * m[11] * m[15] * m[24] - m[1] * m[13] * m[15] * m[24] - m[3] * m[10] * m[16] * m[24] + m[0] * m[13] * m[16] * m[24] + m[1] * m[10] * m[18] * m[24] - m[0] * m[11] * m[18] * m[24]) /
             d,
         (m[4] * m[8] * m[16] * m[20] - m[3] * m[9] * m[16] * m[20] - m[4] * m[6] * m[18] * m[20] + m[1] * m[9] * m[18] * m[20] + m[3] * m[6] * m[19] * m[20] - m[1] * m[8] * m[19] * m[20] -
          m[4] * m[8] * m[15] * m[21] + m[3] * m[9] * m[15] * m[21] + m[4] * m[5] * m[18] * m[21] - m[0] * m[9] * m[18] * m[21] - m[3] * m[5] * m[19] * m[21] + m[0] * m[8] * m[19] * m[21] +
          m[4] * m[6] * m[15] * m[23] - m[1] * m[9] * m[15] * m[23] - m[4] * m[5] * m[16] * m[23] + m[0] * m[9] * m[16] * m[23] + m[1] * m[5] * m[19] * m[23] - m[0] * m[6] * m[19] * m[23] -
          m[3] * m[6] * m[15] * m[24] + m[1] * m[8] * m[15] * m[24] + m[3] * m[5] * m[16] * m[24] - m[0] * m[8] * m[16] * m[24] - m[1] * m[5] * m[18] * m[24] + m[0] * m[6] * m[18] * m[24]) /
             d,
         (-m[4] * m[8] * m[11] * m[20] + m[3] * m[9] * m[11] * m[20] + m[4] * m[6] * m[13] * m[20] - m[1] * m[9] * m[13] * m[20] - m[3] * m[6] * m[14] * m[20] + m[1] * m[8] * m[14] * m[20] +
          m[4] * m[8] * m[10] * m[21] - m[3] * m[9] * m[10] * m[21] - m[4] * m[5] * m[13] * m[21] + m[0] * m[9] * m[13] * m[21] + m[3] * m[5] * m[14] * m[21] - m[0] * m[8] * m[14] * m[21] -
          m[4] * m[6] * m[10] * m[23] + m[1] * m[9] * m[10] * m[23] + m[4] * m[5] * m[11] * m[23] - m[0] * m[9] * m[11] * m[23] - m[1] * m[5] * m[14] * m[23] + m[0] * m[6] * m[14] * m[23] +
          m[3] * m[6] * m[10] * m[24] - m[1] * m[8] * m[10] * m[24] - m[3] * m[5] * m[11] * m[24] + m[0] * m[8] * m[11] * m[24] + m[1] * m[5] * m[13] * m[24] - m[0] * m[6] * m[13] * m[24]) /
             d,
         (m[4] * m[8] * m[11] * m[15] - m[3] * m[9] * m[11] * m[15] - m[4] * m[6] * m[13] * m[15] + m[1] * m[9] * m[13] * m[15] + m[3] * m[6] * m[14] * m[15] - m[1] * m[8] * m[14] * m[15] -
          m[4] * m[8] * m[10] * m[16] + m[3] * m[9] * m[10] * m[16] + m[4] * m[5] * m[13] * m[16] - m[0] * m[9] * m[13] * m[16] - m[3] * m[5] * m[14] * m[16] + m[0] * m[8] * m[14] * m[16] +
          m[4] * m[6] * m[10] * m[18] - m[1] * m[9] * m[10] * m[18] - m[4] * m[5] * m[11] * m[18] + m[0] * m[9] * m[11] * m[18] + m[1] * m[5] * m[14] * m[18] - m[0] * m[6] * m[14] * m[18] -
          m[3] * m[6] * m[10] * m[19] + m[1] * m[8] * m[10] * m[19] + m[3] * m[5] * m[11] * m[19] - m[0] * m[8] * m[11] * m[19] - m[1] * m[5] * m[13] * m[19] + m[0] * m[6] * m[13] * m[19]) /
             d},
        {(-m[9] * m[12] * m[16] * m[20] + m[7] * m[14] * m[16] * m[20] + m[9] * m[11] * m[17] * m[20] - m[6] * m[14] * m[17] * m[20] - m[7] * m[11] * m[19] * m[20] + m[6] * m[12] * m[19] * m[20] +
          m[9] * m[12] * m[15] * m[21] - m[7] * m[14] * m[15] * m[21] - m[9] * m[10] * m[17] * m[21] + m[5] * m[14] * m[17] * m[21] + m[7] * m[10] * m[19] * m[21] - m[5] * m[12] * m[19] * m[21] -
          m[9] * m[11] * m[15] * m[22] + m[6] * m[14] * m[15] * m[22] + m[9] * m[10] * m[16] * m[22] - m[5] * m[14] * m[16] * m[22] - m[6] * m[10] * m[19] * m[22] + m[5] * m[11] * m[19] * m[22] +
          m[7] * m[11] * m[15] * m[24] - m[6] * m[12] * m[15] * m[24] - m[7] * m[10] * m[16] * m[24] + m[5] * m[12] * m[16] * m[24] + m[6] * m[10] * m[17] * m[24] - m[5] * m[11] * m[17] * m[24]) /
             d,
         (m[4] * m[12] * m[16] * m[20] - m[2] * m[14] * m[16] * m[20] - m[4] * m[11] * m[17] * m[20] + m[1] * m[14] * m[17] * m[20] + m[2] * m[11] * m[19] * m[20] - m[1] * m[12] * m[19] * m[20] -
          m[4] * m[12] * m[15] * m[21] + m[2] * m[14] * m[15] * m[21] + m[4] * m[10] * m[17] * m[21] - m[0] * m[14] * m[17] * m[21] - m[2] * m[10] * m[19] * m[21] + m[0] * m[12] * m[19] * m[21] +
          m[4] * m[11] * m[15] * m[22] - m[1] * m[14] * m[15] * m[22] - m[4] * m[10] * m[16] * m[22] + m[0] * m[14] * m[16] * m[22] + m[1] * m[10] * m[19] * m[22] - m[0] * m[11] * m[19] * m[22] -
          m[2] * m[11] * m[15] * m[24] + m[1] * m[12] * m[15] * m[24] + m[2] * m[10] * m[16] * m[24] - m[0] * m[12] * m[16] * m[24] - m[1] * m[10] * m[17] * m[24] + m[0] * m[11] * m[17] * m[24]) /
             d,
         (-m[4] * m[7] * m[16] * m[20] + m[2] * m[9] * m[16] * m[20] + m[4] * m[6] * m[17] * m[20] - m[1] * m[9] * m[17] * m[20] - m[2] * m[6] * m[19] * m[20] + m[1] * m[7] * m[19] * m[20] +
          m[4] * m[7] * m[15] * m[21] - m[2] * m[9] * m[15] * m[21] - m[4] * m[5] * m[17] * m[21] + m[0] * m[9] * m[17] * m[21] + m[2] * m[5] * m[19] * m[21] - m[0] * m[7] * m[19] * m[21] -
          m[4] * m[6] * m[15] * m[22] + m[1] * m[9] * m[15] * m[22] + m[4] * m[5] * m[16] * m[22] - m[0] * m[9] * m[16] * m[22] - m[1] * m[5] * m[19] * m[22] + m[0] * m[6] * m[19] * m[22] +
          m[2] * m[6] * m[15] * m[24] - m[1] * m[7] * m[15] * m[24] - m[2] * m[5] * m[16] * m[24] + m[0] * m[7] * m[16] * m[24] + m[1] * m[5] * m[17] * m[24] - m[0] * m[6] * m[17] * m[24]) /
             d,
         (m[4] * m[7] * m[11] * m[20] - m[2] * m[9] * m[11] * m[20] - m[4] * m[6] * m[12] * m[20] + m[1] * m[9] * m[12] * m[20] + m[2] * m[6] * m[14] * m[20] - m[1] * m[7] * m[14] * m[20] -
          m[4] * m[7] * m[10] * m[21] + m[2] * m[9] * m[10] * m[21] + m[4] * m[5] * m[12] * m[21] - m[0] * m[9] * m[12] * m[21] - m[2] * m[5] * m[14] * m[21] + m[0] * m[7] * m[14] * m[21] +
          m[4] * m[6] * m[10] * m[22] - m[1] * m[9] * m[10] * m[22] - m[4] * m[5] * m[11] * m[22] + m[0] * m[9] * m[11] * m[22] + m[1] * m[5] * m[14] * m[22] - m[0] * m[6] * m[14] * m[22] -
          m[2] * m[6] * m[10] * m[24] + m[1] * m[7] * m[10] * m[24] + m[2] * m[5] * m[11] * m[24] - m[0] * m[7] * m[11] * m[24] - m[1] * m[5] * m[12] * m[24] + m[0] * m[6] * m[12] * m[24]) /
             d,
         (-m[4] * m[7] * m[11] * m[15] + m[2] * m[9] * m[11] * m[15] + m[4] * m[6] * m[12] * m[15] - m[1] * m[9] * m[12] * m[15] - m[2] * m[6] * m[14] * m[15] + m[1] * m[7] * m[14] * m[15] +
          m[4] * m[7] * m[10] * m[16] - m[2] * m[9] * m[10] * m[16] - m[4] * m[5] * m[12] * m[16] + m[0] * m[9] * m[12] * m[16] + m[2] * m[5] * m[14] * m[16] - m[0] * m[7] * m[14] * m[16] -
          m[4] * m[6] * m[10] * m[17] + m[1] * m[9] * m[10] * m[17] + m[4] * m[5] * m[11] * m[17] - m[0] * m[9] * m[11] * m[17] - m[1] * m[5] * m[14] * m[17] + m[0] * m[6] * m[14] * m[17] +
          m[2] * m[6] * m[10] * m[19] - m[1] * m[7] * m[10] * m[19] - m[2] * m[5] * m[11] * m[19] + m[0] * m[7] * m[11] * m[19] + m[1] * m[5] * m[12] * m[19] - m[0] * m[6] * m[12] * m[19]) /
             d},
        {(m[8] * m[12] * m[16] * m[20] - m[7] * m[13] * m[16] * m[20] - m[8] * m[11] * m[17] * m[20] + m[6] * m[13] * m[17] * m[20] + m[7] * m[11] * m[18] * m[20] - m[6] * m[12] * m[18] * m[20] -
          m[8] * m[12] * m[15] * m[21] + m[7] * m[13] * m[15] * m[21] + m[8] * m[10] * m[17] * m[21] - m[5] * m[13] * m[17] * m[21] - m[7] * m[10] * m[18] * m[21] + m[5] * m[12] * m[18] * m[21] +
          m[8] * m[11] * m[15] * m[22] - m[6] * m[13] * m[15] * m[22] - m[8] * m[10] * m[16] * m[22] + m[5] * m[13] * m[16] * m[22] + m[6] * m[10] * m[18] * m[22] - m[5] * m[11] * m[18] * m[22] -
          m[7] * m[11] * m[15] * m[23] + m[6] * m[12] * m[15] * m[23] + m[7] * m[10] * m[16] * m[23] - m[5] * m[12] * m[16] * m[23] - m[6] * m[10] * m[17] * m[23] + m[5] * m[11] * m[17] * m[23]) /
             d,
         (-m[3] * m[12] * m[16] * m[20] + m[2] * m[13] * m[16] * m[20] + m[3] * m[11] * m[17] * m[20] - m[1] * m[13] * m[17] * m[20] - m[2] * m[11] * m[18] * m[20] + m[1] * m[12] * m[18] * m[20] +
          m[3] * m[12] * m[15] * m[21] - m[2] * m[13] * m[15] * m[21] - m[3] * m[10] * m[17] * m[21] + m[0] * m[13] * m[17] * m[21] + m[2] * m[10] * m[18] * m[21] - m[0] * m[12] * m[18] * m[21] -
          m[3] * m[11] * m[15] * m[22] + m[1] * m[13] * m[15] * m[22] + m[3] * m[10] * m[16] * m[22] - m[0] * m[13] * m[16] * m[22] - m[1] * m[10] * m[18] * m[22] + m[0] * m[11] * m[18] * m[22] +
          m[2] * m[11] * m[15] * m[23] - m[1] * m[12] * m[15] * m[23] - m[2] * m[10] * m[16] * m[23] + m[0] * m[12] * m[16] * m[23] + m[1] * m[10] * m[17] * m[23] - m[0] * m[11] * m[17] * m[23]) /
             d,
         (m[3] * m[7] * m[16] * m[20] - m[2] * m[8] * m[16] * m[20] - m[3] * m[6] * m[17] * m[20] + m[1] * m[8] * m[17] * m[20] + m[2] * m[6] * m[18] * m[20] - m[1] * m[7] * m[18] * m[20] -
          m[3] * m[7] * m[15] * m[21] + m[2] * m[8] * m[15] * m[21] + m[3] * m[5] * m[17] * m[21] - m[0] * m[8] * m[17] * m[21] - m[2] * m[5] * m[18] * m[21] + m[0] * m[7] * m[18] * m[21] +
          m[3] * m[6] * m[15] * m[22] - m[1] * m[8] * m[15] * m[22] - m[3] * m[5] * m[16] * m[22] + m[0] * m[8] * m[16] * m[22] + m[1] * m[5] * m[18] * m[22] - m[0] * m[6] * m[18] * m[22] -
          m[2] * m[6] * m[15] * m[23] + m[1] * m[7] * m[15] * m[23] + m[2] * m[5] * m[16] * m[23] - m[0] * m[7] * m[16] * m[23] - m[1] * m[5] * m[17] * m[23] + m[0] * m[6] * m[17] * m[23]) /
             d,
         (-m[3] * m[7] * m[11] * m[20] + m[2] * m[8] * m[11] * m[20] + m[3] * m[6] * m[12] * m[20] - m[1] * m[8] * m[12] * m[20] - m[2] * m[6] * m[13] * m[20] + m[1] * m[7] * m[13] * m[20] +
          m[3] * m[7] * m[10] * m[21] - m[2] * m[8] * m[10] * m[21] - m[3] * m[5] * m[12] * m[21] + m[0] * m[8] * m[12] * m[21] + m[2] * m[5] * m[13] * m[21] - m[0] * m[7] * m[13] * m[21] -
          m[3] * m[6] * m[10] * m[22] + m[1] * m[8] * m[10] * m[22] + m[3] * m[5] * m[11] * m[22] - m[0] * m[8] * m[11] * m[22] - m[1] * m[5] * m[13] * m[22] + m[0] * m[6] * m[13] * m[22] +
          m[2] * m[6] * m[10] * m[23] - m[1] * m[7] * m[10] * m[23] - m[2] * m[5] * m[11] * m[23] + m[0] * m[7] * m[11] * m[23] + m[1] * m[5] * m[12] * m[23] - m[0] * m[6] * m[12] * m[23]) /
             d,
         (m[3] * m[7] * m[11] * m[15] - m[2] * m[8] * m[11] * m[15] - m[3] * m[6] * m[12] * m[15] + m[1] * m[8] * m[12] * m[15] + m[2] * m[6] * m[13] * m[15] - m[1] * m[7] * m[13] * m[15] -
          m[3] * m[7] * m[10] * m[16] + m[2] * m[8] * m[10] * m[16] + m[3] * m[5] * m[12] * m[16] - m[0] * m[8] * m[12] * m[16] - m[2] * m[5] * m[13] * m[16] + m[0] * m[7] * m[13] * m[16] +
          m[3] * m[6] * m[10] * m[17] - m[1] * m[8] * m[10] * m[17] - m[3] * m[5] * m[11] * m[17] + m[0] * m[8] * m[11] * m[17] + m[1] * m[5] * m[13] * m[17] - m[0] * m[6] * m[13] * m[17] -
          m[2] * m[6] * m[10] * m[18] + m[1] * m[7] * m[10] * m[18] + m[2] * m[5] * m[11] * m[18] - m[0] * m[7] * m[11] * m[18] - m[1] * m[5] * m[12] * m[18] + m[0] * m[6] * m[12] * m[18]) /
             d}};
    return MatrixFromArray(a->rows, a->columns, new);
  }
}

Matrix *MatrixAddition(const Matrix *a, const Matrix *b) {
#ifndef NDEBUG
  if (a->rows != b->rows || a->columns != b->columns) {
    fprintf(stderr, "%s: Invalid dimension (%dx%d == %dx%d)\n", __FUNCTION_NAME__, a->rows, a->columns, b->rows, b->columns);
    return NULL;
  }
#endif
  Matrix *new = MatrixClone(a);
  for (uint64_t index = 0; index < new->rows * new->columns; ++index) {
    new->matrix[index] += b->matrix[index];
  }
  return new;
}

Matrix *MatrixSubtraction(const Matrix *a, const Matrix *b) {
#ifndef NDEBUG
  if (a->rows != b->rows || a->columns != b->columns) {
    fprintf(stderr, "%s: Invalid dimension (%dx%d == %dx%d)\n", __FUNCTION_NAME__, a->rows, a->columns, b->rows, b->columns);
    return NULL;
  }
#endif
  Matrix *new = MatrixClone(a);
  for (uint64_t index = 0; index < new->rows * new->columns; ++index) {
    new->matrix[index] -= b->matrix[index];
  }
  return new;
}

Matrix *MatrixMultiplication(const Matrix *a, const Matrix *b) {
#ifndef NDEBUG
  if (a->columns != b->rows) {
    fprintf(stderr, "%s: Invalid dimension (%dx\"%d\" == \"%d\"x%d)\n", __FUNCTION_NAME__, a->rows, a->columns, b->rows, b->columns);
    return NULL;
  }
#endif
  Matrix *new = MatrixZero(a->rows, b->columns);
  for (uint32_t i = 0; i < a->rows; ++i) {
    for (uint32_t j = 0; j < b->columns; ++j) {
      Real sum = 0;
      for (uint32_t k = 0; k < a->columns; ++k) {
        sum += MatrixGetElement(a, i, k) * MatrixGetElement(b, k, j);
      }
      MatrixSetElement(new, i, j, sum);
    }
  }
  return new;
}

Matrix *_MatrixScalarManipulation(const Matrix *a, const char op, Real value) {
  Matrix *new = MatrixClone(a);
  for (uint64_t index = 0; index < new->rows * new->columns; ++index) {
    switch (op) {
    case '+':
      new->matrix[index] += value;
      break;
    case '-':
      new->matrix[index] -= value;
      break;
    case '*':
      new->matrix[index] *= value;
      break;
    case '/':
      new->matrix[index] /= value;
      break;
    default:
      fprintf(stderr, "%s: Invalid operator %c", __FUNCTION_NAME__, op);
      return NULL;
    }
  }
  return new;
}

Matrix *MatrixScalarAddition(const Matrix *a, Real value) { return _MatrixScalarManipulation(a, '+', value); }

Matrix *MatrixScalarSubtraction(const Matrix *a, Real value) { return _MatrixScalarManipulation(a, '-', value); }

Matrix *MatrixScalarMultiplication(const Matrix *a, Real value) { return _MatrixScalarManipulation(a, '*', value); }

Matrix *MatrixScalarDivision(const Matrix *a, Real value) { return _MatrixScalarManipulation(a, '/', value); }
