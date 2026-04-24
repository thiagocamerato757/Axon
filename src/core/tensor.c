#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "tensor.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

/**
 * This function creates a new tensor
 * @param shape: An array specifying the size of each dimension of the tensor
 * @param ndim: The number of dimensions in the tensor
 * @return A pointer to the newly created tensor, or NULL if allocation fails
 */
Tensor* tensor_create(u32 *shape, u8 ndim) {
    Tensor *tensor = (Tensor *)malloc(sizeof(Tensor));
    if (!tensor) return NULL; // Allocation failed

    tensor->ndim = ndim;
    tensor->size = 1;

    // Initialize all dimensions to 0
    for (u8 i = 0; i < MAX_TENSOR_DIMS; i++)  tensor->shape[i] = 0;

    // Set the shape and calculate the total size
    for (u8 i = 0; i < ndim; i++) {
        tensor->shape[i] = shape[i];
        tensor->size *= shape[i];
    }

    // Allocate memory for the tensor data
    tensor->data = (f32 *)malloc(tensor->size * sizeof(f32));
    if (!tensor->data) {
        free(tensor);
        return NULL; // Allocation failed
    }

    return tensor;
}

/**
 * This function frees the memory allocated for a tensor
 * @param tensor: A pointer to the tensor to be freed
 */
void tensor_free(Tensor **tensor) {
    if (!tensor || !*tensor) return;

    free((*tensor)->data);
    (*tensor)->data = NULL;

    free(*tensor);
    *tensor = NULL;
}

/**
 * This function fills a tensor with a specific value
 * @param tensor: A pointer to the tensor to be filled
 * @param value: The value to fill the tensor with
 */
void tensor_fill(Tensor *tensor, f32 value) {
    if (!tensor || !tensor->data) return;

    for (u32 i = 0; i < tensor->size; i++) {
        tensor->data[i] = value;
    }
}

/**
 * This function initializes all elements of a tensor to zero
 * @param tensor: A pointer to the tensor to be initialized
 */
void tensor_zeros(Tensor *tensor) {
    if (!tensor || !tensor->data) return;

    memset(tensor->data, 0, tensor->size * sizeof(f32));
}

/**
 * This function initializes all elements of a tensor to one
 * @param tensor: A pointer to the tensor to be initialized
 */
void tensor_ones(Tensor *tensor) {
    tensor_fill(tensor, 1.0f);
}

/**
 * This function fills a tensor with random values between 0 and 1
 * @param tensor: A pointer to the tensor to be filled with random values
 */
void tensor_random(Tensor *tensor) {
    if (!tensor || !tensor->data) return;

    for (u32 i = 0; i < tensor->size; i++) {
        tensor->data[i] = (f32)rand() / (f32)RAND_MAX; // Random float between 0 and 1
    }
}

/**
 * This function fills a tensor with normally distributed random values
 * @param tensor: A pointer to the tensor to be filled with random values
 * @param mean: The mean of the normal distribution
 * @param stddev: The standard deviation of the normal distribution
 */
void tensor_random_normal(Tensor *tensor, f32 mean, f32 stddev) {
    if (!tensor || !tensor->data) return;

    u32 i = 0;
    while (i < tensor->size) {
        f32 u1 = fmaxf((f32)rand() / (f32)RAND_MAX, 1e-7f); // Avoid log(0)
        f32 u2 = fmaxf((f32)rand() / (f32)RAND_MAX, 1e-7f); // Avoid log(0)

        f32 mag = sqrtf(-2.0f * logf(u1));
        f32 z0  = mag * cosf(2.0f * M_PI * u2);
        f32 z1  = mag * sinf(2.0f * M_PI * u2);

        tensor->data[i++] = z0 * stddev + mean;
        if (i < tensor->size)
            tensor->data[i++] = z1 * stddev + mean;
    }
}

/**
 * This function retrieves an element from a tensor given its multi-dimensional indices
 * @param tensor: A pointer to the tensor from which to retrieve the element
 * @param indices: An array of indices corresponding to each dimension of the tensor
 * @return The value of the element at the specified indices, or 0.0f if the tensor is invalid or the indices are out of bounds
 */
f32 tensor_get(const Tensor *tensor, u32 *indices) {
    if (!tensor || !tensor->data) return 0.0f;

    u32 flat = 0;// flat index in the data array
    u32 stride = 1; // stride for calculating the flat index

    // Calculate the flat index from the multi-dimensional indices
    for (i8 i = tensor->ndim - 1; i >= 0; i--) {
        flat   += indices[i] * stride;
        stride *= tensor->shape[i];
    }

    return tensor->data[flat];
}

/**
 * This function retrieves an element from a 1D tensor
 * @param tensor: A pointer to the tensor from which to retrieve the element
 * @param index: The index of the element to retrieve
 * @return The value of the element at the specified index, or 0.0f if the tensor is invalid or the index is out of bounds
 */
f32 tensor_get1d(const Tensor *tensor, u32 index) {
    u32 indices[1] = {index};
    return tensor_get(tensor, indices);
}

/**
 * This function retrieves an element from a 2D tensor
 * @param tensor: A pointer to the tensor from which to retrieve the element
 * @param row: The row index of the element to retrieve
 * @param col: The column index of the element to retrieve
 * @return The value of the element at the specified index, or 0.0f if the tensor is invalid or the index is out of bounds
 */
f32 tensor_get2d(const Tensor *tensor, u32 row, u32 col) {
    u32 indices[2] = {row, col};
    return tensor_get(tensor, indices);
}

/**
 * This function prints the shape of a tensor
 * @param tensor: A pointer to the tensor whose shape is to be printed
 */
void tensor_print_shape(const Tensor *tensor) {
    if (!tensor) return;

    printf("Shape: (");
    for (u8 i = 0; i < tensor->ndim; i++) {
        printf("%u", tensor->shape[i]);
        if (i < tensor->ndim - 1) printf(", ");
    }
    printf(")\n");
}

/**
 * This function prints the contents of a tensor in a readable format
 * @param tensor: A pointer to the tensor to be printed
 */
void tensor_print(const Tensor *tensor) {
    if (!tensor || !tensor->data) return;

    printf("Tensor (shape: ");
    tensor_print_shape(tensor);
    printf("Data:\n");

    for (u32 i = 0; i < tensor->size; i++) {
        printf("%f ", tensor->data[i]);
        if ((i + 1) % tensor->shape[tensor->ndim - 1] == 0) printf("\n");
    }
}

/*
 * This function performs matrix multiplication on two 2D tensors
 * @param out: A pointer to the output tensor
 * @param a: A pointer to the first input tensor
 * @param b: A pointer to the second input tensor
 */
void tensor_matmul(Tensor *out, const Tensor *a, const Tensor *b) {
    if (!out || !a || !b) return;
    if (a->ndim != 2 || b->ndim != 2 || out->ndim != 2) return;
    if (a->shape[1] != b->shape[0]) return;

    u32 rows  = a->shape[0];
    u32 inner = a->shape[1];
    u32 cols  = b->shape[1];

    // Initialize the output tensor to zero before performing the multiplication
    tensor_zeros(out);

    for (u32 i = 0; i < rows; i++) {
        for (u32 k = 0; k < inner; k++) {
            f32 a_ik = tensor_get2d(a, i, k); //
            for (u32 j = 0; j < cols; j++) {
                //acumulates the product of a_ik and the corresponding element in b into the output tensor
                out->data[i * cols + j] += a_ik * b->data[k * cols + j];
            }
        }
    }
}