#ifndef RSSD_RESEARCH_OPENCL_RELAX_SHARED_HPP
#define RSSD_RESEARCH_OPENCL_RELAX_SHARED_HPP

#include <cassert>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <rssd/Core.h>
#include "rssdcl/ClBuffer.h"

///
/// Macros
///

#define BEGIN_TRACE(NAME, TIMER) \
std::cout << "[" << NAME << "] -------------------------" << std::endl; \
TIMER->start();

#define END_TRACE(NAME, TIMER) \
TIMER->stop(); \
std::cout << "[" << NAME << "] Completed in: " << TIMER->getSeconds() << " seconds" << std::endl; \
std::cout << "[" << NAME << "] -------------------------" << std::endl << std::endl; \
TIMER->reset();

namespace rssd {
namespace relax {

///
/// Constant Vars
///

struct NormalDistributionTable
{
public:
  static const size_t COLS = 10;
  static const size_t ROWS = 31;
  static const float VALUES[ROWS][COLS]; 

public:
  static float get(
    const size_t col, 
    const size_t row);
}; // struct NormalDistributionTable


///
/// Structs
///

struct point_t
{
public:
  point_t(const float *values) :
    x(values[0]),
    y(values[1]),
    z(values[2]),
    w(values[3])
  {
  }

  point_t(
    const float x = 0.0f,
    const float y = 0.0f,
    const float z = 0.0f,
    const float w = 0.0f) :
    x(x),
    y(y),
    z(z),
    w(w)
  {
  }

public:
  bool operator <(const point_t &rhs) const
  {
    if (this->x == rhs.x)
      if (this->y == rhs.y)
        if (this->z == rhs.z)
          return (this->w < rhs.w);
        else return (this->z < rhs.z);
      else return (this->y < rhs.y);
    return (this->x < rhs.x);
  }

  bool operator ==(const point_t &rhs) const
  {
    return (this->x == rhs.x)
      && (this->y == rhs.y)
      && (this->z == rhs.z)
      && (this->w == rhs.w);
  }

  point_t operator -(const point_t &rhs) const
  {
    return point_t(
      (this->x - rhs.x),
      (this->y - rhs.y),
      (this->z - rhs.z),
      (this->w - rhs.w));
  }

  float operator [](const uint32_t index) const
  {
    assert (index < 4);
    switch (index)
    {
      case 0: return this->x;
      case 1: return this->y;
      case 2: return this->z;
      case 3: return this->w;
      default: break;
    }
    return 0.0f;
  }

  friend std::ostream& operator <<(std::ostream &out, point_t &value)
  {
    out << "{" << value.x << ", "
      << value.y << ", "
      << value.z << ", "
      << value.w << "}";
    return out;
  }

public:
  inline float* get()
  {
    return &this->x;
  }

  void set(
    const float x, 
    const float y = 0.0f,
    const float z = 0.0f,
    const float w = 0.0f)
  {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
  }

  void clear()
  {
    this->x = 0.0f;
    this->y = 0.0f;
    this->z = 0.0f;
    this->w = 0.0f;
  }

  double length() const
  {
    return std::sqrt(
      std::pow((double)this->x, 2)
      + std::pow((double)this->y, 2)
      + std::pow((double)this->z, 2)
      + std::pow((double)this->w, 2));
  }

  double squaredLength() const
  {
    return std::pow((double)this->x, 2)
      + std::pow((double)this->y, 2)
      + std::pow((double)this->z, 2)
      + std::pow((double)this->w, 2);
  }

  double distance(const point_t &rhs) const
  {
    return (*this - rhs).length();
  }

  double squaredDistance(const point_t &rhs) const
  {
    return (*this - rhs).squaredLength();
  }

public:
  float x, y, z, w;
}; // struct point_t

struct marker_t
{
public:
  struct property
  {
    enum type
    {
      NONE = 0,
      ASSIGNED = 1<<0,
      INDEX = 1<<1,
      VALUE = 1<<2,
      POSITION = 1<<3,
      ALL = -1
    }; // enum type
  }; // struct property

public:
  marker_t() :
    assigned(false),
    index(0),
    value(0.0f)
  {}

  marker_t(
    const size_t i, 
    const float v) :
    assigned(false),
    index(i),
    value(v)
  {}

public:
  bool operator <(const marker_t &rhs) const
  {
    return (this->index < rhs.index);
  }

  bool operator ==(const marker_t &rhs) const
  {
    return (this->index == rhs.index);
  }

  bool operator !=(const marker_t &rhs) const
  {
    return !(*this == rhs);
  }

  friend std::ostream& operator <<(std::ostream &out, marker_t &value)
  {
    out << "marker_t {" << '\n'
      << "\tIndex: " << value.index << '\n'
      << "\tValue: " << value.value << '\n'
      << "\tAssigned: " << value.assigned << '\n'
      << "\tPosition: " << value.position << '\n'
      << "}" << std::endl;
    return out;
  }

public:
  void clear()
  {
    this->assigned = false;
    this->index = 0;
    this->value = 0.0f;
    this->position.clear();
  }

  double distance(const marker_t &rhs) const
  {
    return this->position.distance(rhs.position);   
  }

public:
  static uint32_t extract(
    std::vector<marker_t> &markers,
    const uint32_t property,
    rssd::gpgpu::ClBuffer *results_buffer)
  {
    uint32_t
      index = 0,
      stride = 0;

    // Calculate stride
    /// @note Calculate stride in bytes.
    switch (property)
    {
      case marker_t::property::POSITION: { stride = sizeof(float) * 4; break; }
      case marker_t::property::ASSIGNED: { stride = sizeof(bool); break; }
      case marker_t::property::INDEX: { stride = sizeof(uint32_t); break; }
      case marker_t::property::VALUE: { stride = sizeof(float); break; }
      default: { stride = 1; break; }
    }

    rssd::gpgpu::ClBuffer::Byte_v &results = results_buffer->getData();
    results.clear();
    results.resize(markers.size() * stride);
    std::cout << "Results resized to: " << results.size() << " bytes (stride=" << stride << " bytes)" << std::endl;

    for (uint32_t i = 0; i < markers.size(); ++i)
    {
      // Determine element start position
      index = stride * i;
      byte *source = NULL;
      switch (property)
      {
#if 0
        case marker_t::property::ASSIGNED: { results[i] = markers[i].assigned; break; }
        case marker_t::property::INDEX: { results[i] = markers[i].index; break; }
        case marker_t::property::VALUE: { results[i] = markers[i].value; break; }
        case marker_t::property::POSITION:
        {
            for (uint32_t j = 0; j < 4; ++j)
            { results[(i * stride) + j] = markers[i].position[j]; }
            break;
        }
#else
        case marker_t::property::ASSIGNED:
        {
          source = reinterpret_cast<byte*>(&markers[i].assigned);
          break;
        }
        case marker_t::property::INDEX:
        {
          source = reinterpret_cast<byte*>(&markers[i].index);
          break;
        }
        case marker_t::property::VALUE:
        {
          source = reinterpret_cast<byte*>(&markers[i].value);
          break;
        }
        case marker_t::property::POSITION:
        {
          source = reinterpret_cast<byte*>(&markers[i].position.x);
          break;
        }
#endif
        default: return 0;
      }
      std::memcpy(
        &results[i * stride],
        source,
        stride);
    }
    return results.size();
  }

public:
  bool assigned;
  uint32_t index;
  float value;
  point_t position;
}; // struct marker_t

struct marker_pair_t
{
public:
  marker_pair_t()
  {
  }

  marker_pair_t(
    const marker_t &first,
    const marker_t &second) :
    first_marker(first),
    second_marker(second)
  {
  }

  virtual ~marker_pair_t()
  {}

public:
  bool operator <(const marker_pair_t &rhs) const
  {
    if (this->first_marker == rhs.first_marker)
      return (this->second_marker < rhs.second_marker);
    return (this->first_marker < rhs.first_marker);
  }

  /// @note Equivalency is commutative for marker pairs.
  bool operator ==(const marker_pair_t &rhs) const
  {
    return (this->first_marker == rhs.first_marker)
      && (this->second_marker == rhs.second_marker);
  }

  bool operator !=(const marker_pair_t &rhs) const
  {
    return !(*this == rhs);
  }

public:
  void set(
    const size_t first_index,
    const size_t first_value,
    const size_t second_index,
    const size_t second_value)
  {
    this->first_marker = marker_t(first_index, first_value);
    this->second_marker = marker_t(second_index, second_value);
  }

  void set(
    const marker_t &first,
    const marker_t &second)
  {
    this->first_marker = first;
    this->second_marker = second;
  }

  void clear()
  {
    this->first_marker.clear();
    this->second_marker.clear();
  }

public:
  marker_t first_marker, second_marker;
}; // struct marker_pair_t

struct compatibility_t
{
public:
  compatibility_t() :
    compatibility_score(0.0f),
    incompatibility_score(0.0f)
  {}

public:
  bool operator <(const compatibility_t &rhs) const
  {
    if (this->first_pair  != rhs.first_pair)
      return (this->first_pair == rhs.first_pair);
    return (this->second_pair < rhs.second_pair);
  }

  bool operator ==(const compatibility_t &rhs) const
  {
    return (this->first_pair != rhs.first_pair)
      && (this->second_pair != rhs.second_pair);
  }

  bool operator !=(const compatibility_t &rhs) const
  {
    return !(*this == rhs);
  }

public:
  void set(
    const marker_pair_t &first,
    const marker_pair_t &second)
  {
      this->first_pair = first;
      this->second_pair = second;
  }

  void clear()
  {
    this->compatibility_score = 0.0f;
    this->incompatibility_score = 0.0f;
  }

public:
  static const float RIGID_VIOL_COMPAT;
  static const float RIGID_VIOL_INCOMPAT;
  static const float RATIO_CUTOFF_GLOBAL;

public:
  float 
    compatibility_score,
    incompatibility_score;
  marker_pair_t
    first_pair,
    second_pair;
}; // struct compatibility_t

struct rng
{
public:
  static float get()
  {
    rng::seed();
    return (rand() / (static_cast<double>(RAND_MAX) + 1.0))
        * (rng::MAX_RANGE - rng::MIN_RANGE) + rng::MIN_RANGE;
  }

public:
  static float
      MIN_RANGE,
      MAX_RANGE;
  static const size_t SEED = 0xDEADBEEF;

protected:
  static void seed()
  {
    static bool INIT = false;
    if (INIT) return;
    INIT = true;
    srand(rng::SEED);
  }

}; // struct rng

///
/// Typedefs
///

typedef std::vector<float> float_v;
typedef std::vector<marker_t> marker_v;
typedef std::map<size_t, marker_t> marker_m;
typedef std::map<marker_pair_t, float> marker_distance_m;
typedef std::set<compatibility_t> compatibility_s;

///
/// Functions
///

bool generateData(
  marker_v &objects,
  marker_v &labels,
  marker_v &reference_labels,
  const uint32_t num_markers);

} // namespace relax
} // namespace rssd

#endif // RSSD_RESEARCH_OPENCL_RELAX_SHARED_HPP
