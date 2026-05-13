#include "../core/tensor.h"
#include "test.h"

// ─────────────────────────────────────────
//  tensor_create / tensor_free
// ─────────────────────────────────────────

void test_tensor_creation_and_deallocation() {
  loading("Tensor Creation & Deallocation");

  // 1D tensor
  u32 shape1[] = {5};
  Tensor *t = tensor_create(shape1, 1);
  check("Create 1D tensor (non-null)", t != NULL);
  check("Correct ndim (1)", t->ndim == 1);
  check("Correct size (5)", t->size == 5);
  check("Shape[0] correct (5)", t->shape[0] == 5);
  check("Unused shape[1] is zero", t->shape[1] == 0);
  check("Unused shape[2] is zero", t->shape[2] == 0);
  check("Data pointer allocated", t->data != NULL);
  tensor_free(&t);
  check("Pointer NULL after free", t == NULL);

  // 2D tensor
  u32 shape2[] = {3, 4};
  Tensor *t2 = tensor_create(shape2, 2);
  check("Create 2D tensor (non-null)", t2 != NULL);
  check("Correct size (3*4=12)", t2->size == 12);
  check("Shape[0] == 3", t2->shape[0] == 3);
  check("Shape[1] == 4", t2->shape[1] == 4);
  check("Unused shape[2] is zero", t2->shape[2] == 0);
  tensor_free(&t2);

  // 3D tensor
  u32 shape3[] = {2, 3, 4};
  Tensor *t3 = tensor_create(shape3, 3);
  check("Create 3D tensor (non-null)", t3 != NULL);
  check("Correct size (2*3*4=24)", t3->size == 24);
  check("Shape[0]==2, [1]==3, [2]==4",
        t3->shape[0] == 2 && t3->shape[1] == 3 && t3->shape[2] == 4);
  tensor_free(&t3);

  // zero-dimension tensor
  u32 shape_zero[] = {0, 4};
  Tensor *tz = tensor_create(shape_zero, 2);
  check("Zero-dim tensor has size 0", tz != NULL && tz->size == 0);
  tensor_free(&tz);

  // double free — must not crash
  tensor_free(&t); // t is already NULL
  check("Double free does not crash", 1);
}

// ─────────────────────────────────────────
//  tensor_fill / tensor_zeros / tensor_ones
// ─────────────────────────────────────────

void test_tensor_fill_operations() {
  loading("Tensor Fill Operations");

  u32 shape[] = {6};
  Tensor *t = tensor_create(shape, 1);

  // fill with constant
  tensor_fill(t, 3.14f);
  int all_pi = 1;
  for (u32 i = 0; i < t->size; i++)
    if (!float_eq(t->data[i], 3.14f, 1e-5f))
      all_pi = 0;
  check("Fill all elements with 3.14", all_pi);

  // zeros
  tensor_zeros(t);
  int all_zero = 1;
  for (u32 i = 0; i < t->size; i++)
    if (!float_eq(t->data[i], 0.0f, 1e-9f))
      all_zero = 0;
  check("Zeros — all elements are 0.0", all_zero);

  // ones
  tensor_ones(t);
  int all_one = 1;
  for (u32 i = 0; i < t->size; i++)
    if (!float_eq(t->data[i], 1.0f, 1e-9f))
      all_one = 0;
  check("Ones — all elements are 1.0", all_one);

  // fill with negative value
  tensor_fill(t, -7.5f);
  int all_neg = 1;
  for (u32 i = 0; i < t->size; i++)
    if (!float_eq(t->data[i], -7.5f, 1e-5f))
      all_neg = 0;
  check("Fill with negative value (-7.5)", all_neg);

  // fill with explicit zero
  tensor_fill(t, 0.0f);
  int all_zero2 = 1;
  for (u32 i = 0; i < t->size; i++)
    if (!float_eq(t->data[i], 0.0f, 1e-9f))
      all_zero2 = 0;
  check("Fill with 0.0f explicitly", all_zero2);

  // ones after zeros — ensure no leftover state
  tensor_zeros(t);
  tensor_ones(t);
  check("Ones after zeros — first elem 1", float_eq(t->data[0], 1.0f, 1e-9f));
  check("Ones after zeros — last elem 1",
        float_eq(t->data[t->size - 1], 1.0f, 1e-9f));

  tensor_free(&t);
}

// ─────────────────────────────────────────
//  tensor_random / tensor_random_normal
// ─────────────────────────────────────────

void test_tensor_random_generation() {
  loading("Tensor Random Generation");

  srand(42);

  u32 shape[] = {1000};
  Tensor *t = tensor_create(shape, 1);

  // uniform — all values must be in [0, 1]
  tensor_random(t);
  int in_range = 1;
  for (u32 i = 0; i < t->size; i++)
    if (t->data[i] < 0.0f || t->data[i] > 1.0f)
      in_range = 0;
  check("Uniform — all values in [0, 1]", in_range);

  // uniform — values must not all be equal
  int has_variation = 0;
  for (u32 i = 1; i < t->size; i++)
    if (!float_eq(t->data[i], t->data[0], 1e-5f)) {
      has_variation = 1;
      break;
    }
  check("Uniform — values are not constant", has_variation);

  // uniform — mean should be close to 0.5
  f32 sum_u = 0.0f;
  for (u32 i = 0; i < t->size; i++)
    sum_u += t->data[i];
  f32 mean_u = sum_u / (f32)t->size;
  check("Uniform — mean ≈ 0.5 (±0.05)", float_eq(mean_u, 0.5f, 0.05f));

  // normal — mean should be close to 0
  tensor_random_normal(t, 0.0f, 1.0f);
  f32 sum = 0.0f;
  for (u32 i = 0; i < t->size; i++)
    sum += t->data[i];
  f32 mean = sum / (f32)t->size;
  check("Normal(0,1) — mean ≈ 0 (±0.15)", float_eq(mean, 0.0f, 0.15f));

  // normal — stddev should be close to 1
  f32 var = 0.0f;
  for (u32 i = 0; i < t->size; i++)
    var += (t->data[i] - mean) * (t->data[i] - mean);
  f32 stddev = sqrtf(var / (f32)t->size);
  check("Normal(0,1) — stddev ≈ 1 (±0.1)", float_eq(stddev, 1.0f, 0.1f));

  // normal — custom mean
  tensor_random_normal(t, 5.0f, 0.01f);
  f32 sum2 = 0.0f;
  for (u32 i = 0; i < t->size; i++)
    sum2 += t->data[i];
  f32 mean2 = sum2 / (f32)t->size;
  check("Normal(5, 0.01) — mean ≈ 5 (±0.05)", float_eq(mean2, 5.0f, 0.05f));

  // normal — all values must be within 4 standard deviations (>99.99%)
  tensor_random_normal(t, 0.0f, 1.0f);
  int within_4std = 1;
  for (u32 i = 0; i < t->size; i++)
    if (t->data[i] < -4.0f || t->data[i] > 4.0f)
      within_4std = 0;
  check("Normal(0,1) — values within ±4σ", within_4std);

  // normal — must not produce NaN values
  int no_nan = 1;
  for (u32 i = 0; i < t->size; i++)
    if (t->data[i] != t->data[i])
      no_nan = 0; // NaN != NaN
  check("Normal — no NaN values", no_nan);

  tensor_free(&t);
}

// ─────────────────────────────────────────
//  tensor_get / tensor_get1d / tensor_get2d
// ─────────────────────────────────────────

void test_tensor_get() {
  loading("Tensor Get (1D / 2D / generic)");

  // 1D tensor: [3.0, 7.0, 2.0]
  u32 shape1[] = {3};
  Tensor *t1 = tensor_create(shape1, 1);
  t1->data[0] = 3.0f;
  t1->data[1] = 7.0f;
  t1->data[2] = 2.0f;

  check("get1d index 0 == 3.0", float_eq(tensor_get1d(t1, 0), 3.0f, 1e-5f));
  check("get1d index 1 == 7.0", float_eq(tensor_get1d(t1, 1), 7.0f, 1e-5f));
  check("get1d index 2 == 2.0", float_eq(tensor_get1d(t1, 2), 2.0f, 1e-5f));

  // generic get must match get1d
  u32 idx1[] = {1};
  check("tensor_get 1D [1] == 7.0",
        float_eq(tensor_get(t1, idx1), 7.0f, 1e-5f));

  tensor_free(&t1);

  // 2D tensor: [[1,2,3],[4,5,6]]  shape [2,3]
  u32 shape2[] = {2, 3};
  Tensor *t2 = tensor_create(shape2, 2);
  f32 vals[] = {1, 2, 3, 4, 5, 6};
  memcpy(t2->data, vals, sizeof(vals));

  check("get2d [0][0] == 1", float_eq(tensor_get2d(t2, 0, 0), 1.0f, 1e-5f));
  check("get2d [0][1] == 2", float_eq(tensor_get2d(t2, 0, 1), 2.0f, 1e-5f));
  check("get2d [0][2] == 3", float_eq(tensor_get2d(t2, 0, 2), 3.0f, 1e-5f));
  check("get2d [1][0] == 4", float_eq(tensor_get2d(t2, 1, 0), 4.0f, 1e-5f));
  check("get2d [1][1] == 5", float_eq(tensor_get2d(t2, 1, 1), 5.0f, 1e-5f));
  check("get2d [1][2] == 6", float_eq(tensor_get2d(t2, 1, 2), 6.0f, 1e-5f));

  // generic get must match get2d
  u32 idx2[] = {1, 2};
  check("tensor_get 2D [1][2] == 6",
        float_eq(tensor_get(t2, idx2), 6.0f, 1e-5f));

  // get2d and tensor_get must return identical values for all indices
  int consistent = 1;
  for (u32 i = 0; i < 2; i++) {
    for (u32 j = 0; j < 3; j++) {
      u32 idx[] = {i, j};
      if (!float_eq(tensor_get2d(t2, i, j), tensor_get(t2, idx), 1e-7f))
        consistent = 0;
    }
  }
  check("get2d and tensor_get consistent for all indices", consistent);

  tensor_free(&t2);
}

// ─────────────────────────────────────────
//  tensor_matmul
// ─────────────────────────────────────────

void test_tensor_matmul() {
  loading("Tensor Matmul");

  // case 1 — 2x2 @ 2x2
  // A = [[1,2],[3,4]]  B = [[5,6],[7,8]]
  // C = [[19,22],[43,50]]
  u32 s22[] = {2, 2};
  Tensor *a = tensor_create(s22, 2);
  Tensor *b = tensor_create(s22, 2);
  Tensor *c = tensor_create(s22, 2);
  f32 va[] = {1, 2, 3, 4};
  f32 vb[] = {5, 6, 7, 8};
  memcpy(a->data, va, sizeof(va));
  memcpy(b->data, vb, sizeof(vb));
  tensor_matmul(c, a, b);
  check("2x2 matmul — C[0][0] == 19",
        float_eq(tensor_get2d(c, 0, 0), 19.0f, 1e-4f));
  check("2x2 matmul — C[0][1] == 22",
        float_eq(tensor_get2d(c, 0, 1), 22.0f, 1e-4f));
  check("2x2 matmul — C[1][0] == 43",
        float_eq(tensor_get2d(c, 1, 0), 43.0f, 1e-4f));
  check("2x2 matmul — C[1][1] == 50",
        float_eq(tensor_get2d(c, 1, 1), 50.0f, 1e-4f));
  tensor_free(&a);
  tensor_free(&b);
  tensor_free(&c);

  // case 2 — different shapes [2,3] @ [3,4] = [2,4]
  // A = [[1,0,2],[0,3,1]]
  // B = [[1,0,1,0],[2,1,0,1],[0,1,2,1]]
  // C[0] = [1,2,5,2]   C[1] = [6,4,2,4]
  u32 sa[] = {2, 3}, sb[] = {3, 4}, sc[] = {2, 4};
  Tensor *a2 = tensor_create(sa, 2);
  Tensor *b2 = tensor_create(sb, 2);
  Tensor *c2 = tensor_create(sc, 2);
  f32 va2[] = {1, 0, 2, 0, 3, 1};
  f32 vb2[] = {1, 0, 1, 0, 2, 1, 0, 1, 0, 1, 2, 1};
  memcpy(a2->data, va2, sizeof(va2));
  memcpy(b2->data, vb2, sizeof(vb2));
  tensor_matmul(c2, a2, b2);
  check("[2,3]@[3,4] — C[0][0]==1",
        float_eq(tensor_get2d(c2, 0, 0), 1.0f, 1e-4f));
  check("[2,3]@[3,4] — C[0][2]==5",
        float_eq(tensor_get2d(c2, 0, 2), 5.0f, 1e-4f));
  check("[2,3]@[3,4] — C[1][0]==6",
        float_eq(tensor_get2d(c2, 1, 0), 6.0f, 1e-4f));
  check("[2,3]@[3,4] — C[1][3]==4",
        float_eq(tensor_get2d(c2, 1, 3), 4.0f, 1e-4f));
  tensor_free(&a2);
  tensor_free(&b2);
  tensor_free(&c2);

  // case 3 — A @ I = A (identity matrix)
  u32 s33[] = {3, 3};
  Tensor *mat = tensor_create(s33, 2);
  Tensor *eye = tensor_create(s33, 2);
  Tensor *res = tensor_create(s33, 2);
  f32 vm[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  memcpy(mat->data, vm, sizeof(vm));
  tensor_zeros(eye);
  eye->data[0] = 1;
  eye->data[4] = 1;
  eye->data[8] = 1;
  tensor_matmul(res, mat, eye);
  int id_ok = 1;
  for (u32 i = 0; i < mat->size; i++)
    if (!float_eq(res->data[i], mat->data[i], 1e-4f))
      id_ok = 0;
  check("A @ I == A (identity)", id_ok);
  tensor_free(&mat);
  tensor_free(&eye);
  tensor_free(&res);

  // case 4 — A @ 0 = 0 (zero matrix)
  u32 s2[] = {2, 2};
  Tensor *m = tensor_create(s2, 2);
  Tensor *z = tensor_create(s2, 2);
  Tensor *r = tensor_create(s2, 2);
  tensor_fill(m, 5.0f);
  tensor_zeros(z);
  tensor_matmul(r, m, z);
  int zero_ok = 1;
  for (u32 i = 0; i < r->size; i++)
    if (!float_eq(r->data[i], 0.0f, 1e-5f))
      zero_ok = 0;
  check("A @ 0 == 0 (zero matrix)", zero_ok);
  tensor_free(&m);
  tensor_free(&z);
  tensor_free(&r);

  // case 5 — incompatible shapes must not crash or alter out
  u32 sx[] = {2, 3}, sy[] = {4, 2}, sz[] = {2, 2};
  Tensor *x = tensor_create(sx, 2);
  Tensor *y = tensor_create(sy, 2);
  Tensor *zz = tensor_create(sz, 2);
  tensor_zeros(zz);
  tensor_matmul(zz, x, y); // incompatible shape — must be ignored
  int still_zero = 1;
  for (u32 i = 0; i < zz->size; i++)
    if (!float_eq(zz->data[i], 0.0f, 1e-9f))
      still_zero = 0;
  check("Incompatible shapes — out unchanged", still_zero);
  tensor_free(&x);
  tensor_free(&y);
  tensor_free(&zz);
}

// ─────────────────────────────────────────
//  tensor_print_shape (visual)
// ─────────────────────────────────────────

void test_tensor_print_shape() {
  loading("Tensor Print Shape");

  u32 s1[] = {5};
  Tensor *t1 = tensor_create(s1, 1);
  printf("  expected: Shape: (5)\n  got:      ");
  tensor_print_shape(t1);
  check("print_shape 1D does not crash", 1);
  tensor_free(&t1);

  u32 s2[] = {3, 4};
  Tensor *t2 = tensor_create(s2, 2);
  printf("  expected: Shape: (3, 4)\n  got:      ");
  tensor_print_shape(t2);
  check("print_shape 2D does not crash", 1);
  tensor_free(&t2);

  u32 s3[] = {2, 3, 4};
  Tensor *t3 = tensor_create(s3, 3);
  printf("  expected: Shape: (2, 3, 4)\n  got:      ");
  tensor_print_shape(t3);
  check("print_shape 3D does not crash", 1);
  tensor_free(&t3);

  tensor_print_shape(NULL);
  check("print_shape NULL does not crash", 1);
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
  test_tensor_get();
  test_tensor_matmul();
  test_tensor_print_shape();

  printf("\n──────────────────────────────────────\n");
  printf("Result: %d passed | %d failed\n", tests_passed, tests_failed);
  printf("──────────────────────────────────────\n");

  return tests_failed > 0 ? 1 : 0;
}