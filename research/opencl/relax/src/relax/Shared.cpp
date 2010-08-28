#include "relax/Shared.hpp"

using namespace std;
using namespace rssd;
using namespace rssd::relax;

///
/// Constant Vars
///

const float rssd::relax::compatibility_t::RIGID_VIOL_COMPAT = -4.5f;
const float rssd::relax::compatibility_t::RIGID_VIOL_INCOMPAT = 8.5f;
const float rssd::relax::compatibility_t::RATIO_CUTOFF_GLOBAL = 4.75f;

///
/// @struct point_t
///

const point_t point_t::ZERO(0.0f, 0.0f, 0.0f, 0.0f);
const point_t point_t::ONE(1.0f, 1.0f, 1.0f, 1.0f);

///
/// @struct rng
///

float rng::MIN_RANGE = 0.0f;
float rng::MAX_RANGE = 1.0f;

///
/// @struct NormalDistributionTable
///

const float rssd::relax::NormalDistributionTable::VALUES[rssd::relax::NormalDistributionTable::ROWS][rssd::relax::NormalDistributionTable::COLS] =
{
  {0.500,0.504,0.508,0.512,0.516,0.520,0.524,0.528,0.532,0.536 },
  {0.540,0.544,0.548,0.552,0.556,0.560,0.564,0.568,0.571,0.575 }, 
  {0.579,0.583,0.587,0.591,0.595,0.599,0.603,0.606,0.610,0.614 },
  {0.618,0.622,0.626,0.629,0.633,0.637,0.641,0.644,0.648,0.652 },
  {0.655,0.659,0.663,0.666,0.670,0.674,0.677,0.681,0.684,0.688 },
  {0.692,0.695,0.699,0.702,0.705,0.709,0.712,0.716,0.719,0.722 },
  {0.726,0.729,0.732,0.736,0.739,0.742,0.745,0.749,0.752,0.755 },
  {0.758,0.761,0.764,0.767,0.770,0.773,0.776,0.779,0.782,0.785 },
  {0.788,0.791,0.794,0.797,0.800,0.802,0.805,0.808,0.811,0.813 },
  {0.816,0.819,0.821,0.824,0.826,0.829,0.832,0.834,0.837,0.839 },
  {0.841,0.844,0.846,0.849,0.851,0.853,0.855,0.858,0.860,0.862 },
  {0.864,0.867,0.869,0.871,0.873,0.875,0.877,0.879,0.881,0.883 },
  {0.885,0.887,0.889,0.891,0.893,0.894,0.896,0.898,0.900,0.902 },
  {0.903,0.905,0.907,0.908,0.910,0.912,0.913,0.915,0.916,0.918 },
  {0.919,0.921,0.922,0.924,0.925,0.927,0.928,0.929,0.931,0.932 },
  {0.933,0.935,0.936,0.937,0.938,0.939,0.941,0.942,0.943,0.944 },
  {0.945,0.946,0.947,0.948,0.950,0.951,0.952,0.953,0.954,0.955 },
  {0.955,0.956,0.957,0.958,0.959,0.960,0.961,0.962,0.963,0.963 },
  {0.964,0.965,0.966,0.966,0.967,0.968,0.969,0.969,0.970,0.971 },
  {0.971,0.972,0.973,0.973,0.974,0.974,0.975,0.976,0.976,0.977 },
  {0.977,0.978,0.978,0.979,0.979,0.980,0.980,0.981,0.981,0.982 },
  {0.982,0.983,0.983,0.983,0.984,0.984,0.985,0.985,0.985,0.986 },
  {0.986,0.986,0.987,0.987,0.988,0.988,0.988,0.988,0.989,0.989 },
  {0.989,0.990,0.990,0.990,0.990,0.991,0.991,0.991,0.991,0.992 },
  {0.992,0.992,0.992,0.993,0.993,0.993,0.993,0.993,0.993,0.994 },
  {0.994,0.994,0.994,0.994,0.995,0.995,0.995,0.995,0.995,0.995 },
  {0.995,0.996,0.996,0.996,0.996,0.996,0.996,0.996,0.996,0.996 },
  {0.997,0.997,0.997,0.997,0.997,0.997,0.997,0.997,0.997,0.997 },
  {0.997,0.998,0.998,0.998,0.998,0.998,0.998,0.998,0.998,0.998 },
  {0.998,0.998,0.998,0.998,0.998,0.998,0.999,0.999,0.999,0.999 },
  {0.999,0.999,0.999,0.999,0.999,0.999,0.999,0.999,0.999,0.999 } 
}; // NormalDistributionTable::VALUES

float rssd::relax::NormalDistributionTable::get(
  const size_t row,
  const size_t col)
{
  assert (row < NormalDistributionTable::ROWS);
  assert (col < NormalDistributionTable::COLS);
  if ((row >= NormalDistributionTable::ROWS)
    || (col >= NormalDistributionTable::COLS))
    return 0.0f;
  return NormalDistributionTable::VALUES[row][col];
}

bool rssd::relax::generateData(
  rssd::relax::marker_v &objects,
  rssd::relax::marker_v &labels,
  rssd::relax::marker_v &reference_labels,
  const uint32_t num_markers)
{
  for (uint32_t i = 0; i < num_markers; ++i)
  {
    // Create object
    objects.push_back(marker_t());
    objects[i].index = i;
    objects[i].value = rng::get();
    objects[i].position.x = rng::get();
    objects[i].position.y = rng::get();
    objects[i].position.z = rng::get();

    // Create label
    labels.push_back(marker_t());
    labels[i].index = i;
    labels[i].value = rng::get();
    labels[i].position.x = rng::get();
    labels[i].position.y = rng::get();
    labels[i].position.z = rng::get();

    // Create reference label
    reference_labels.push_back(marker_t());
    reference_labels[i].index = i;
    reference_labels[i].value = rng::get();
    reference_labels[i].position.x = rng::get();
    reference_labels[i].position.y = rng::get();
    reference_labels[i].position.z = rng::get();

#if PRINT
    std::cout << "Object #" << i << ": " << objects[i] << std::endl;
    std::cout << "Label #" << i << ": " << labels[i] << std::endl;
    std::cout << "Reference Label #" << i << ": " << reference_labels[i] << std::endl;
#endif
  }
  return true;
}
