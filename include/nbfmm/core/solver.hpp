////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file    include/nbfmm/core/solver.hpp
/// @brief   The definition of the FMM solver.
///
/// @author  Mu Yang <emfomy@gmail.com>
///

#ifndef NBFMM_CORE_SOLVER_HPP_
#define NBFMM_CORE_SOLVER_HPP_

#include <nbfmm/config.hpp>
#include <nbfmm/core/kernel_function.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  The NBFMM namespace.
//
namespace nbfmm {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The NBFMM solver class.
///
class Solver {

#ifdef NBFMM_CHECK
 public:
#else
 protected:
#endif

  /// The number of cell levels.
  const int num_level_;

  /// The number of cells in the base level per side. @n = 2^(#num_level_+1)
  const int base_dim_;

  /// The number of cells in the base level plus 1. @n = #base_dim_^2+1
  const int num_cell_p1_;

  /// The maximum number of particles
  const int max_num_particle_;

  /// The limits of positions. @n [x_min, y_min, x_max, y_max].
  const float4 position_limits_;

  /// The device pointer of particle positions. @n Vector, 1 by #max_num_particle_.
  float2* gpuptr_position_;

  /// The device pointer of particle effects. @n Vector, 1 by #max_num_particle_.
  float2* gpuptr_effect_;

  /// The device pointer of particle weights. @n Vector, 1 by #max_num_particle_.
  float*  gpuptr_weight_;

  /// The device pointer of particle cell indices. @n Vector, 1 by #max_num_particle_.
  int2*   gpuptr_index_;

  /// The device pointer of particle permutation indices. @n Vector, 1 by #max_num_particle_.
  int*    gpuptr_perm_;

  /// The device pointer of starting permutation indices of each cell. @n Vector, 1 by #num_cell_p1_.
  int*    gpuptr_head_;

  /// The device pointer of cell positions. @n Cube, #base_dim_ by #base_dim_ by #num_level_.
  float2* gpuptr_cell_position_;

  /// The device pointer of cell effects. @n Cube, #base_dim_ by #base_dim_ by #num_level_.
  float2* gpuptr_cell_effect_;

  /// The device pointer of cell weights. @n Cube, #base_dim_ by #base_dim_ by #num_level_.
  float*  gpuptr_cell_weight_;

  /// The workspace used in P2M
  float2* gpuptr_buffer_float2_;

  /// The workspace used in P2M
  int2*   gpuptr_buffer_int2_;

 public:

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Default constructor
  ///
  /// @param  num_level         the number of cell levels.
  /// @param  max_num_particle  the maximum number of particles.
  /// @param  position_limits   the limits of positions. [x_min, y_min, x_max, y_max].
  ///
  Solver( const int num_level, const int max_num_particle, const float4 position_limits );

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Default destructor
  ///
  ~Solver();

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Solve system
  ///
  /// @param[in]   num_particle            the number of particles.
  /// @param[in]   gpuptr_position_origin  the device pointer of original particle positions.
  /// @param[in]   gpuptr_weight_origin    the device pointer of original particle weights.
  /// @param[out]  gpuptr_effect_origin    the device pointer of original particle effects.
  ///
  void solve( const int num_particle, const float2* gpuptr_position_origin,
              const float* gpuptr_weight_origin, float2* gpuptr_effect_origin );

#ifdef NBFMM_CHECK
 public:
#else
 protected:
#endif

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Do preliminary works
  ///
  /// @param[in]  num_particle            the number of particles.
  /// @param[in]  gpuptr_position_origin  the device pointer of original particle positions.
  /// @param[in]  gpuptr_weight_origin    the device pointer of original particle weights.
  ///
  /// @post  #gpuptr_position_ (sorted)
  /// @post  #gpuptr_weight_ (sorted)
  /// @post  #gpuptr_index_ (sorted)
  /// @post  #gpuptr_head_
  /// @post  #gpuptr_perm_
  ///
  void predo( const int num_particle, const float2* gpuptr_position_origin, const float* gpuptr_weight_origin );

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Do postliminary works
  ///
  /// @param[in]   num_particle          the number of particles.
  /// @param[out]  gpuptr_effect_origin  the device pointer of original particle effects.
  ///
  /// @pre  #gpuptr_effect_ (sorted, all effects)
  /// @pre  #gpuptr_perm_
  ///
  void postdo( const int num_particle, float2* gpuptr_effect_origin );

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Compute particle to particle
  ///
  /// @param  num_particle  the number of particles.
  ///
  /// @pre   #gpuptr_position_ (sorted)
  /// @pre   #gpuptr_weight_ (sorted)
  /// @pre   #gpuptr_index_ (sorted)
  /// @pre   #gpuptr_head_
  ///
  /// @post  #gpuptr_effect_ (sorted, P2P effects only)
  ///
  void p2p( const int num_particle );

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Compute particle to multipole
  ///
  /// @param  num_particle  the number of particles.
  ///
  /// @pre   #gpuptr_position_ (sorted)
  /// @pre   #gpuptr_weight_ (sorted)
  /// @pre   #gpuptr_index_ (sorted)
  /// @pre   #gpuptr_head_
  ///
  /// @post  #gpuptr_cell_position_ (base level only)
  /// @post  #gpuptr_cell_weight_ (base level only)
  ///
  void p2m( const int num_particle );

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Compute multipole to multipole
  ///
  /// @pre   #gpuptr_cell_position_ (base level only)
  /// @pre   #gpuptr_cell_weight_ (base level only)
  ///
  /// @post  #gpuptr_cell_position_ (all level)
  /// @post  #gpuptr_cell_weight_ (all level)
  ///
  void m2m();

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Compute multipole to local
  ///
  /// @pre   #gpuptr_cell_position_ (all level)
  /// @pre   #gpuptr_cell_weight_ (all level)
  ///
  /// @post  #gpuptr_cell_effect_ (all level)
  ///
  void m2l();

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Compute local to particle
  ///
  /// @pre   #gpuptr_cell_effect_ (all level)
  ///
  /// @post  #gpuptr_cell_effect_ (summed to base level)
  ///
  void l2l();

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Compute local to particle
  ///
  /// @param  num_particle  the number of particles.
  ///
  /// @pre   #gpuptr_effect_ (sorted, P2P effects only)
  /// @pre   #gpuptr_index_ (sorted)
  /// @pre   #gpuptr_cell_effect_ (summed to base level)
  ///
  /// @post  #gpuptr_effect_ (sorted, all effects)
  ///
  void l2p( const int num_particle );

};

}  // namespace nbfmm

#endif  // NBFMM_CORE_SOLVER_HPP_
