#include "tensor.h"
#include "test.h"

// ─────────────────────────────────────────
//  tensor_create / tensor_free
// ─────────────────────────────────────────

void test_tensor_creation_and_deallocation() {
    loading("Tensor Creation & Deallocation");

    u32 shape1[] = {5};
    Tensor *t = tensor_create(shape1, 1);

    check("Create 1D tensor (non-null)", t != NULL);
    check("Correct ndim", t->ndim == 1);
    check("Correct size (5)", t->size == 5);
    check("Shape[0] correct", t->shape[0] == 5);
    check("Unused shape index is zero", t->shape[1] == 0);
    check("Data allocated", t->data != NULL);

    tensor_free(&t);
    check("Pointer is NULL after free", t == NULL);

    u32 shape2[] = {3, 4};
    Tensor *t2 = tensor_create(shape2, 2);

    check("Create 2D tensor", t2 != NULL);
    check("Correct size (3x4)", t2->size == 12);
    check("Shape[0] == 3", t2->shape[0] == 3);
    check("Shape[1] == 4", t2->shape[1] == 4);

    tensor_free(&t2);

    u32 shape_zero[] = {0, 4};
    Tensor *t3 = tensor_create(shape_zero, 2);

    check("Zero-dimension tensor has size 0", t3 != NULL && t3->size == 0);

    tensor_free(&t3);

    tensor_free(&t);
    check("Double free does not crash", 1);
}

// ─────────────────────────────────────────
//  fill / zeros / ones
// ─────────────────────────────────────────

void test_tensor_fill_operations() {
    loading("Tensor Fill Operations");

    u32 shape[] = {4};
    Tensor *t = tensor_create(shape, 1);

    tensor_fill(t, 3.14f);

    int all_pi = 1;
    for (u32 i = 0; i < t->size; i++)
        if (!float_eq(t->data[i], 3.14f, 1e-5f)) all_pi = 0;

    check("Fill with constant value (3.14)", all_pi);

    tensor_zeros(t);

    int all_zero = 1;
    for (u32 i = 0; i < t->size; i++)
        if (!float_eq(t->data[i], 0.0f, 1e-9f)) all_zero = 0;

    check("Fill with zeros", all_zero);

    tensor_ones(t);

    int all_one = 1;
    for (u32 i = 0; i < t->size; i++)
        if (!float_eq(t->data[i], 1.0f, 1e-9f)) all_one = 0;

    check("Fill with ones", all_one);

    tensor_fill(t, -2.5f);
    check("Fill with negative value", float_eq(t->data[0], -2.5f, 1e-5f));

    tensor_free(&t);
}

// ─────────────────────────────────────────
//  random
// ─────────────────────────────────────────

void test_tensor_random_generation() {
    loading("Tensor Random Generation");

    srand(42);

    u32 shape[] = {1000};
    Tensor *t = tensor_create(shape, 1);

    tensor_random(t);

    int in_range = 1;
    for (u32 i = 0; i < t->size; i++)
        if (t->data[i] < 0.0f || t->data[i] > 1.0f) in_range = 0;

    check("Uniform random values in [0,1]", in_range);

    int variation = 0;
    for (u32 i = 1; i < t->size; i++)
        if (!float_eq(t->data[i], t->data[0], 1e-5f)) { variation = 1; break; }

    check("Random values are not constant", variation);

    tensor_random_normal(t, 0.0f, 1.0f);

    f32 sum = 0.0f;
    for (u32 i = 0; i < t->size; i++) sum += t->data[i];

    f32 mean = sum / t->size;
    check("Normal distribution mean ≈ 0", float_eq(mean, 0.0f, 0.15f));

    f32 var = 0.0f;
    for (u32 i = 0; i < t->size; i++)
        var += (t->data[i] - mean) * (t->data[i] - mean);

    f32 stddev = sqrtf(var / t->size);
    check("Normal distribution stddev ≈ 1", float_eq(stddev, 1.0f, 0.1f));

    tensor_random_normal(t, 5.0f, 0.01f);

    f32 sum2 = 0.0f;
    for (u32 i = 0; i < t->size; i++) sum2 += t->data[i];

    f32 mean2 = sum2 / t->size;
    check("Normal distribution mean ≈ 5", float_eq(mean2, 5.0f, 0.05f));

    tensor_free(&t);
}

// ─────────────────────────────────────────
//  main
// ─────────────────────────────────────────

int main() {
    printf("╔══════════════════════════════════════╗\n");
    printf("║        TENSOR TEST SUITE             ║\n");
    printf("╚══════════════════════════════════════╝\n");

    test_tensor_creation_and_deallocation();
    test_tensor_fill_operations();
    test_tensor_random_generation();

    printf("\n──────────────────────────────────────\n");
    printf("Result: %d passed | %d failed\n", tests_passed, tests_failed);
    printf("──────────────────────────────────────\n");

    return tests_failed > 0 ? 1 : 0;
}