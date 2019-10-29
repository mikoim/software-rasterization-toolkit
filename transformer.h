#ifndef RENDER_TRANSFORMER_H
#define RENDER_TRANSFORMER_H

#include "matrix.h"
#include "vector.h"

typedef struct tagTransformer {
  Vector location;
  Vector rotation; // radian
  Vector scale;
  Matrix *matrix;
  Matrix *inverseMatrix;
} Transformer;

Transformer *TransformerCreate(Vector location, Vector rotation, Vector scale);
bool TransformerDestroy(Transformer *transformer);
bool TransformerUpdateTransformationMatrix(Transformer *transformer);
Vector TransformerTransform(const Transformer *transformer, Vector point);
Vector TransformerDetransform(const Transformer *transformer, Vector point);

#endif // RENDER_TRANSFORMER_H
