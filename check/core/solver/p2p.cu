////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file    check/core/solver/p2p.cu
/// @brief   Test nbfmm::Solver::p2p
///
/// @author  Mu Yang <emfomy@gmail.com>
///

#include "../solver.hpp"

void TestNbfmmSolver::p2p() {
  cudaError_t cuda_status;

  // Alias vectors
  auto position = random_position;
  auto weight   = random_weight;
  auto index    = random_index;
  auto head     = random_head;

  // Allocate memory
  float2 effect0[num_particle];
  float2 effect[num_particle];

  // Copy input vectors
  cuda_status = cudaMemcpy(solver.gpuptr_position_, position, num_particle * sizeof(float2), cudaMemcpyHostToDevice);
  CPPUNIT_ASSERT(cuda_status == cudaSuccess);
  cuda_status = cudaMemcpy(solver.gpuptr_weight_,   weight,   num_particle * sizeof(float),  cudaMemcpyHostToDevice);
  CPPUNIT_ASSERT(cuda_status == cudaSuccess);
  cuda_status = cudaMemcpy(solver.gpuptr_index_,    index,    num_particle * sizeof(int2),   cudaMemcpyHostToDevice);
  CPPUNIT_ASSERT(cuda_status == cudaSuccess);
  cuda_status = cudaMemcpy(solver.gpuptr_head_,     head,     num_cell_p1  * sizeof(int),    cudaMemcpyHostToDevice);
  CPPUNIT_ASSERT(cuda_status == cudaSuccess);

  // Compute effects
  for ( auto i = 0; i < num_particle; ++i ) {
    effect0[i] = make_float2(0.0f, 0.0f);
    for ( auto j = 0; j < num_particle; ++j ) {
      if ( abs(index[i].x-index[j].x) <= 1 && abs(index[i].y-index[j].y) <= 1 && i != j ) {
        effect0[i] += nbfmm::kernelFunction(position[i], position[j], weight[j]);
      }
    }
  }

  // Run p2p
  solver.p2p(num_particle);

  // Copy output vectors
  cuda_status = cudaMemcpy(effect, solver.gpuptr_effect_, num_particle * sizeof(float2), cudaMemcpyDeviceToHost);
  CPPUNIT_ASSERT(cuda_status == cudaSuccess);

  // Check
  for ( auto i = 0; i < num_particle; ++i ) {
    // printf("\n #%d (%2d, %2d): (%12.4f, %12.4f) * %12.4f -> (%12.4f, %12.4f) | (%12.4f, %12.4f)", i, index[i].x, index[i].y,
    //        cell_position[i].x, cell_position[i].y, cell_weight[i],
    //        cell_effect0[i].x, cell_effect0[i].y, cell_effect[i].x, cell_effect[i].y);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(effect0[i].x, effect[i].x, 1e-4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(effect0[i].y, effect[i].y, 1e-4);
  }
}
