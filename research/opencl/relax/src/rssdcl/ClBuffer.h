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

///
/// @class ClBuffer
///

class ClBuffer
{
public:
  struct Type
  {
    enum Value
    {
      MEMORY = 0,
      SAMPLER,
      LOCAL
    }; // enum Value
  }; // struct Type

public:
  ClBuffer(
    cl::Context &context,
    cl::CommandQueue &command_queue,
    const uint32_t index,
    const uint32_t type,
    const uint32_t num_bytes,
    const uint32_t flags = 0) :
    _id(ClBuffer::UID++),
    _index(index),
    _type(type),
    _size(num_bytes),
    _flags(flags),
    _cl(cl::Buffer(
      context,
      flags,
      num_bytes)),
    _context(context),
    _command_queue(command_queue)
  {
    this->resize();
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
  inline uint32_t getIndex() const { return this->_index; }
  inline void setIndex(const uint32_t value) { this->_index = value; }
  inline uint32_t getFlags() const { return this->_flags; }
  inline byte_v& getData() { return this->_data; }
  inline const byte_v& getData() const { return this->_data; }
  inline cl::Buffer& getCl() { return this->_cl; }
  inline const cl::Buffer& getCl() const { return this->_cl; }
  inline uint32_t getType() const { return this->_type; }
  inline void setType(const uint32_t value) { this->_type = value; }

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
    byte_v::iterator position,
    InputIterator first,
    InputIterator last)
  {
    this->_data.insert(position, first, last);
  }

  cl_uint resize()
  {
    // Allocate raw buffer
    if (this->_type != Type::LOCAL)
      this->_data.resize(this->_size);
    return CL_SUCCESS;
  }

  /// @returns Size of buffer in bytes.
  uint32_t size() const
  {
    if (this->_type != ClBuffer::Type::LOCAL)
      assert (this->_size == this->_data.size());
    return this->_size;
  }

  template <typename T>
  void print(std::ostream &out = std::cout) const
  {
    uint32_t count = this->_data.size() / sizeof(T);
    uint32_t stride = sizeof(T) * sizeof(byte);
    T value;

    out << "ClBuffer (size=" << count << ") = {";
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
    this->_index = rhs._index;
    this->_type = rhs._type;
    this->_size = rhs._size;
    this->_flags = rhs._flags;
    this->_context = rhs._context;
    this->_command_queue = rhs._command_queue;
    this->_cl = rhs._cl;
    this->resize();
    this->_data.assign(rhs._data.begin(), rhs._data.end());
    return true;
  }

protected:
  static uint32_t UID;

protected:
  uint32_t
    _id,
    _index,
    _type,
    _size,
    _flags;
  byte_v _data;
  cl::Buffer _cl;
  cl::Context _context;
  cl::CommandQueue _command_queue;
}; // class ClBuffer

///
/// Typedefs
///

// TYPEDEF_CONTAINERS(ClBuffer);
typedef std::map<uint32_t, ClBuffer> ClBuffer_m;
typedef std::list<ClBuffer> ClBuffer_l;

} // namespace gpgpu
} // namespace rssd

#endif // RSSD_GPGPU_CLBUFFER_H
