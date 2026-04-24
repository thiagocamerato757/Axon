#ifndef TENSOR_H
#define TENSOR_H

#include "types.h"

#define MAX_TENSOR_DIMS 8


typedef struct {
    f32  *data; // Pointer to the tensor data in memory
    u32  shape[MAX_TENSOR_DIMS]; // Array of dimensions
    u32  size; // Total number of elements in the tensor
    u8   ndim; // Number of dimensions in the tensor being used
} Tensor;

Tensor* tensor_create(u32 *shape, u8 ndim);
void tensor_free(Tensor **tensor);
void tensor_fill(Tensor *tensor, f32 value);
void tensor_ones(Tensor *tensor);
void tensor_zeros(Tensor *tensor);
void tensor_random(Tensor *tensor);
void tensor_random_normal(Tensor *tensor, f32 mean, f32 stddev);
void tensor_print_shape(const Tensor *tensor);
void tensor_print(const Tensor *tensor);
f32 tensor_get(const Tensor *tensor, u32 *indices);
f32 tensor_get1d(const Tensor *tensor, u32 index);
f32 tensor_get2d(const Tensor *tensor, u32 row, u32 col);
void tensor_matmul(Tensor *out, const Tensor *a, const Tensor *b);


#endif // TENSOR_H