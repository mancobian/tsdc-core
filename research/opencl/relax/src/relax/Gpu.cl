///
/// @function calculateMarkerDistances1d
/// @param num_markers (__global): Number of input markers.
/// @param pos (__global): Array of 3D positions for input markers.
/// @param differences (__global): Scratch space for marker position difference calculation results.
/// @param distances (__global): Scratch space for marker distance calculation results.
/// @param pos_cache (__local): Local copy of pos array.
/// @note Establish a work group for each marker, resulting in m work groups.
///       In each work-item kernel, loop through each input marker position and
///       calculate the distance and difference between a marker and its comparison marker.
/// @note Assume get_global_id(#) goes from 0..num_markers.
/// @note Assume get_local_id(#) goes from 0..num_labels.
///

__kernel void calculateMarkerDistances1d(
  __global float4 *pos,
  __global float4 *differences,
  __global float *distances,
  __local float4 *pos_cache)
{
  // Cache input marker positions
  const uint global_index = get_global_id(0);
  const uint num_markers = get_global_size(0);
  pos_cache[global_index] = pos[global_index];
  const float4 current_marker = pos_cache[global_index];

  // Sync
  barrier (CLK_LOCAL_MEM_FENCE);

  // Calculate marker distances/differences
  uint result_index = global_index * num_markers;
  for (uint i = 0; i < num_markers; ++i)
  {
    distances[result_index] = fast_distance(current_marker, pos_cache[i]);
    differences[result_index] = current_marker - pos_cache[i];
    ++result_index;
  }

  // Sync
  barrier (CLK_LOCAL_MEM_FENCE);
}
