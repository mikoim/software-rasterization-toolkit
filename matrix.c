#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"

Matrix *MatrixZero(uint32_t rows, uint32_t columns) {
  if (rows == 0 || columns == 0) {
    fprintf(stderr, "%s: Invalid dimension (%dx%d >= 1x1)\n", __FUNCTION_NAME__, rows, columns);
    return NULL;
  }
  Matrix *new = (Matrix *)calloc(1, sizeof(Matrix));
  *new = (Matrix){rows, columns, NULL};
  new->matrix = (Real *)calloc(1, sizeof(Real) * rows * columns);
  return new;
}

bool MatrixDestroy(Matrix *a) {
  free(a->matrix);
  free(a);
  return true; // TODO: implement error handling
}

Real MatrixGetElement(const Matrix *a, uint32_t rows, uint32_t columns) {
  if (a->rows <= rows || a->columns <= columns) {
    fprintf(stderr, "%s: Invalid element indices (rows:%d<%d, columns:%d<%d)\n", __FUNCTION_NAME__, rows, a->rows, columns, a->columns);
    return false;
  }
  return a->matrix[columns + a->columns * rows];
}

bool MatrixSetElement(Matrix *a, uint32_t rows, uint32_t columns, Real value) {
  if (a->rows <= rows || a->columns <= columns) {
    fprintf(stderr, "%s: Invalid element indices (rows:%d<%d, columns:%d<%d)\n", __FUNCTION_NAME__, rows, a->rows, columns, a->columns);
    return false;
  }
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
  if (a->rows != b->rows || a->columns != b->columns) {
    fprintf(stderr, "%s: Invalid dimension (%dx%d == %dx%d)\n", __FUNCTION_NAME__, a->rows, a->columns, b->rows, b->columns);
    return false;
  }
  return memcmp(a->matrix, b->matrix, sizeof(Real) * a->rows * a->columns) == 0 ? true : false;
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
  if (n == 0) {
    fprintf(stderr, "%s: Invalid dimension (0x0 >= 1x1)\n", __FUNCTION_NAME__);
    return NULL;
  }
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

Matrix *MatrixAddition(const Matrix *a, const Matrix *b) {
  if (a->rows != b->rows || a->columns != b->columns) {
    fprintf(stderr, "%s: Invalid dimension (%dx%d == %dx%d)\n", __FUNCTION_NAME__, a->rows, a->columns, b->rows, b->columns);
    return NULL;
  }
  Matrix *new = MatrixClone(a);
  for (uint64_t index = 0; index < new->rows * new->columns; ++index) {
    new->matrix[index] += b->matrix[index];
  }
  return new;
}

Matrix *MatrixSubtraction(const Matrix *a, const Matrix *b) {
  if (a->rows != b->rows || a->columns != b->columns) {
    fprintf(stderr, "%s: Invalid dimension (%dx%d == %dx%d)\n", __FUNCTION_NAME__, a->rows, a->columns, b->rows, b->columns);
    return NULL;
  }
  Matrix *new = MatrixClone(a);
  for (uint64_t index = 0; index < new->rows * new->columns; ++index) {
    new->matrix[index] -= b->matrix[index];
  }
  return new;
}

Matrix *MatrixMultiplication(const Matrix *a, const Matrix *b) {
  if (a->columns != b->rows) {
    fprintf(stderr, "%s: Invalid dimension (%dx\"%d\" == \"%d\"x%d)\n", __FUNCTION_NAME__, a->rows, a->columns, b->rows, b->columns);
    return NULL;
  }
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
