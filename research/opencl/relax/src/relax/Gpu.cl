__kernel void test(
  __global const float4 *labels,
  __global float *distances)
{
  int GLOBAL_INDEX = get_global_id(0);
  int GLOBAL_SIZE = get_global_size(0);
  int WORK_GROUP_INDEX = get_local_id(0);
  int WORK_GROUP_SIZE = get_local_size(0);
  int WORK_GROUP_ID = get_group_id(0);
  int NUM_WORK_GROUPS = get_num_groups(0);
  int STRIDE = 4;

  // Get the current label
  float4 current_label = labels[GLOBAL_INDEX];

  // For each label...
  // for (int i = 0; i < STRIDE; ++i)
  {
    // float4 other_label = labels[i];
    //distances[GLOBAL_INDEX * STRIDE + i] = current_label[i];
      // fast_distance(current_label,other_label);
    distances[GLOBAL_INDEX * STRIDE + 0] = current_label.x;
    distances[GLOBAL_INDEX * STRIDE + 1] = current_label.y;
    distances[GLOBAL_INDEX * STRIDE + 2] = current_label.z;
    distances[GLOBAL_INDEX * STRIDE + 3] = current_label.w;
  }
}
