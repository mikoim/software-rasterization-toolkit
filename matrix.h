#ifndef RENDER_MATRIX_H
#define RENDER_MATRIX_H

#include "common.h"

typedef struct tagMatrix {
  uint32_t rows;
  uint32_t columns;
  Real *matrix; // {{1,2,3},{4,5,6},{7,8,9}} -> {1,2,3,4,5,6,7,8,9}
} Matrix;

Matrix *MatrixZero(uint32_t rows, uint32_t columns);
Matrix *MatrixFromArray(uint32_t rows, uint32_t columns, const Real array[rows][columns]);
bool MatrixDestroy(Matrix *a);
bool MatrixCompare(const Matrix *a, const Matrix *b);
bool MatrixCompareLoose(const Matrix *a, const Matrix *b, Real error);
Matrix *MatrixClone(const Matrix *a);
void MatrixPrint(const Matrix *a);

Real MatrixGetElement(const Matrix *a, uint32_t rows, uint32_t columns);
bool MatrixSetElement(Matrix *a, uint32_t rows, uint32_t columns, Real value);

Matrix *MatrixIdentity(uint32_t n);

Matrix *MatrixTranspose(const Matrix *a);
Matrix *MatrixInverse(const Matrix *a);
Matrix *MatrixAddition(const Matrix *a, const Matrix *b);
Matrix *MatrixSubtraction(const Matrix *a, const Matrix *b);
Matrix *MatrixMultiplication(const Matrix *a, const Matrix *b);

Matrix *MatrixScalarAddition(const Matrix *a, Real value);
Matrix *MatrixScalarSubtraction(const Matrix *a, Real value);
Matrix *MatrixScalarMultiplication(const Matrix *a, Real value);
Matrix *MatrixScalarDivision(const Matrix *a, Real value);

#endif // RENDER_MATRIX_H
