////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file    source/nbfmm/core/solver/solve.cu
/// @brief   Solve system
///
/// @author  Mu Yang <emfomy@gmail.com>
///

#include <nbfmm/core.hpp>

// Solve system
void nbfmm::Solver::solve(
    const int     num_particle,
    const float2* gpuptr_position_origin,
    const float*  gpuptr_weight_origin,
    float2*       gpuptr_effect_origin
) {
  assert(num_particle >= 0 && num_particle <= max_num_particle_);

  predo(num_particle, gpuptr_position_origin, gpuptr_weight_origin);
  p2p(num_particle);
  p2m(num_particle);
  m2m();
  m2l();
  l2l();
  l2p(num_particle);
  postdo(num_particle, gpuptr_effect_origin);
}
