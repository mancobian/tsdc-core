#include <OpenCL/cl.hpp>

__kernel void add_vectors(__global cl_float* A, const __global cl_float* B, const __global cl_float* C, const unsigned cl_uint N)
{
    cl_uint i = get_global_id(0);
    if (i < N)
        A[i] = B[i] + C[i];
}
