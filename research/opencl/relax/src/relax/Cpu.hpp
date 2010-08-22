#ifndef RSSD_RESEARCH_OPENCL_RELAX_CPU_HPP
#define RSSD_RESEARCH_OPENCL_RELAX_CPU_HPP

#include "Shared.hpp"

namespace rssd {
namespace relax {
namespace cpu {

///
/// Functions
///

int cpu_main(const size_t num_markers);
void calculateInterLabelDistances(
  const marker_v &labels,
  marker_distance_m &distances);
void calculateInterObjectDistances(
  const marker_v &objects,
  marker_distance_m &distances);
size_t getUniqueCompatabilityPairs(
  const marker_v &objects,
  const marker_v &labels,
  compatibility_s &compatibilities);
#if 0
void calculateCompatibilityScores(
  const marker_v &objects,
  const marker_v &labels,
  const marker_v &reference_labels);
#endif

} // namespace cpu
} // namespace relax
} // namespace rssd

#endif // RSSD_RESEARCH_OPENCL_RELAX_CPU_HPP
