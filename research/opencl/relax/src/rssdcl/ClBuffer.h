#ifndef RSSD_GPGPU_CLBUFFER_H
#define RSSD_GPGPU_CLBUFFER_H

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <map>
#include <OpenCL/cl.hpp>
#include <rssd/Core.h>

namespace rssd {
namespace gpgpu {

struct sizeof_variant_visitor : boost::static_visitor<uint32_t>
{
  uint32_t operator ()(std::string &value) const
  {
    std::cout << "std::string => sizeof(" << value << "): " << sizeof(cl_char) << std::endl;
    return sizeof(cl_char) * value.size();
  }

  uint32_t operator ()(float32_t value) const
  {
    std::cout << "float32_t => sizeof(" << value << "): " << sizeof(cl_float) << std::endl;
    return sizeof(value);
  }

  uint32_t operator ()(float64_t value) const
  {
    std::cout << "float64_t => sizeof(" << value << "): " << sizeof(cl_float) << std::endl;
    return sizeof(value);
  }

  uint32_t operator ()(int32_t value) const
  {
    std::cout << "int32_t => sizeof(" << value << "): " << sizeof(cl_int) << std::endl;
    return sizeof(value);
  }

  uint32_t operator ()(int64_t value) const
  {
    std::cout << "int64_t => sizeof(" << value << "): " << sizeof(cl_long) << std::endl;
    return sizeof(value);
  }

  uint32_t operator ()(uint32_t value) const
  {
    std::cout << "uint32_t => sizeof(" << value << "): " << sizeof(cl_uint) << std::endl;
    return sizeof(value);
  }

  uint32_t operator ()(uint64_t value) const
  {
    std::cout << "uint64_t => sizeof(" << value << "): " << sizeof(cl_ulong) << std::endl;
    return sizeof(value);
  }

  template <typename T>
  uint32_t operator ()(T &value) const
  {
    std::cout << "T => sizeof(" << value << "): " << sizeof(value) << std::endl;
    return sizeof(value);
  }
}; // struct variant_echo_visitor

///
/// @class ClBuffer
///

class ClBuffer
{
// public:
//  typedef std::vector<variant_t> Byte_v;
public:
    typedef std::vector<byte> Byte_v;

public:
  ClBuffer(
    cl::Context &context,
    cl::CommandQueue &command_queue,
    const uint32_t num_bytes,
    const uint32_t flags = 0) :
    _id(ClBuffer::UID++),
    _flags(flags),
    _context(context),
    _command_queue(command_queue)
  {
    this->resize(num_bytes, flags);
  }

  ClBuffer(const ClBuffer &rhs)
  {
    this->copy(rhs);
  }

  ~ClBuffer()
  {
    this->_data.clear();
  }

public:
  inline uint32_t getId() const { return this->_id; }
  inline uint32_t getFlags() const { return this->_flags; }
  inline Byte_v& getData() { return this->_data; }
  inline const Byte_v& getData() const { return this->_data; }
  inline cl::Buffer& getCl() { return this->_cl; }
  inline const cl::Buffer& getCl() const { return this->_cl; }

public:
  ClBuffer& operator =(const ClBuffer &rhs)
  {
    this->copy(rhs);
    return *this;
  }

  bool operator <(const ClBuffer &rhs) const
  {
    return (this->_id < rhs._id);
  }

  bool operator ==(const ClBuffer &rhs) const
  {
    return (this->_id == rhs._id);
  }

  bool operator !=(const ClBuffer &rhs) const
  {
    return !(this->_id == rhs._id);
  }

  byte& operator [](const uint32_t index)
  {
    assert (index < this->_data.size());
    return this->_data[index];
  }

  const byte& operator [](const uint32_t index) const
  {
    assert (index < this->_data.size());
    return this->_data[index];
  }

  friend std::ostream& operator <<(std::ostream &out, const ClBuffer &buffer)
  {
    out << "ClBuffer = {";
    for (uint32_t i = 0; i < buffer._data.size(); ++i)
    {
      if (i > 0) out << ", ";
      out << buffer._data[i];
    }
    out << "}\n";
    return out;
  }

public:
  template <typename InputIterator>
  void insert(
    Byte_v::iterator position,
    InputIterator first,
    InputIterator last)
  {
    this->_data.insert(position, first, last);
  }

  cl_uint resize(
    const uint32_t num_bytes,
    const uint32_t flags = 0)
  {
    // Allocate raw buffer
    this->_data.resize(num_bytes);

    // Allocate CL buffer
    this->_cl = cl::Buffer(
      this->_context,
      flags,
      num_bytes);

    // Store creation flags
    this->_flags = flags;
    return CL_SUCCESS;
  }

  uint32_t size() const
  {
    return this->_data.size();
  }

  template <typename T>
  void print(std::ostream &out = std::cout) const
  {
    uint32_t count = this->_data.size() / sizeof(T);
    uint32_t stride = sizeof(T) * sizeof(byte);
    T value;

    out << "ClBuffer = {";
    for (uint32_t i = 0; i < count; ++i)
    {
      if (i > 0) out << ", ";
      std::memcpy(&value, &this->_data[i*stride], stride);
      out << value;
    }
    out << "}\n";
    out << std::endl;
  }

protected:
  bool copy(const ClBuffer &rhs)
  {
    if (this == &rhs)
      return false;

    this->_id = rhs._id;
    this->_context = rhs._context;
    this->_command_queue = rhs._command_queue;

    this->resize(rhs._data.size(), rhs._flags);

    this->_data.assign(rhs._data.begin(), rhs._data.end());

    this->_command_queue.enqueueCopyBuffer(
      rhs._cl,
      this->_cl,
      0,
      0,
      rhs.size());
    return true;
  }

protected:
  static uint32_t UID;

protected:
  uint32_t
    _id,
    _flags;
  Byte_v _data;
  cl::Buffer _cl;
  cl::Context _context;
  cl::CommandQueue _command_queue;
}; // class ClBuffer

///
/// Typedefs
///

// TYPEDEF_CONTAINERS(ClBuffer);
typedef std::map<uint32_t, ClBuffer> ClBuffer_m;

} // namespace gpgpu
} // namespace rssd

#endif // RSSD_GPGPU_CLBUFFER_H
