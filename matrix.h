#ifndef RENDER_MATRIX_H
#define RENDER_MATRIX_H

#include "common.h"

typedef long double Real;

typedef struct {
  uint32_t rows;
  uint32_t columns;
  Real *matrix; // {{1,2,3},{4,5,6},{7,8,9}} -> {1,2,3,4,5,6,7,8,9}
} Matrix;

Matrix *MatrixZero(uint32_t rows, uint32_t columns);
Matrix *MatrixFromArray(uint32_t rows, uint32_t columns, Real array[rows][columns]);
bool MatrixDestroy(Matrix *a);
bool MatrixCompare(const Matrix *a, const Matrix *b);
Matrix *MatrixClone(const Matrix *a);
void MatrixPrint(const Matrix *a);

Real MatrixGetElement(const Matrix *a, uint32_t rows, uint32_t columns);
bool MatrixSetElement(Matrix *a, uint32_t rows, uint32_t columns, Real value);

Matrix *MatrixIdentity(uint32_t n);

Matrix *MatrixAddition(const Matrix *a, const Matrix *b);
Matrix *MatrixSubtraction(const Matrix *a, const Matrix *b);
Matrix *MatrixMultiplication(const Matrix *a, const Matrix *b);

Matrix *MatrixScalarAddition(const Matrix *a, Real value);
Matrix *MatrixScalarSubtraction(const Matrix *a, Real value);
Matrix *MatrixScalarMultiplication(const Matrix *a, Real value);
Matrix *MatrixScalarDivision(const Matrix *a, Real value);

Real MatrixVectorEuclideanDistance(const Matrix *a, const Matrix *b);
bool MatrixVectorInsideTriangle(const Matrix *p, const Matrix *p0, const Matrix *p1, const Matrix *p2);

typedef struct {
  Real x,y,z;
} Vector;

Real VectorEuclideanDistance(const Vector *a, const Vector *b);
bool VectorInsideTriangle(const Vector *p, const Vector *p0, const Vector *p1, const Vector *p2);

#endif //RENDER_MATRIX_H
