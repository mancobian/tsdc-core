#ifndef RSSD_RESEARCH_OPENCL_RELAX_GPU_HPP
#define RSSD_RESEARCH_OPENCL_RELAX_GPU_HPP

#include "Shared.hpp"

namespace rssd {
namespace relax {
namespace gpu {

///
/// Functions
///

int gpu_main(const size_t num_markers);
cl_uint createMarkerDistances(
  const std::string &kernel_name,
  marker_v &markers,
  Timer *timer);
#if 0
cl_uint createEntryPointBuffers(
  const std::string &kernel_name,
  const marker_v &labels,
  const marker_v &objects,
  gpgpu::ClBuffer_l &local_buffers,
  gpgpu::ClBuffer_l &global_buffers,
  uint32_t &param_index);
cl_uint createMarkerPairCompatibilityBuffers(
  const std::string &kernel_name,
  const marker_v &lhs_markers,
  const marker_v &rhs_markers,
  gpgpu::ClBuffer_l &local_buffers,
  gpgpu::ClBuffer_l &global_buffers,
  uint32_t &param_index);
#endif

} // namespace gpu
} // namespace relax
} // namespace rssd

#endif // RSSD_RESEARCH_OPENCL_RELAX_GPU_HPP
