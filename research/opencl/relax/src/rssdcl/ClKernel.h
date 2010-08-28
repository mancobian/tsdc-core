#ifndef RSSD_GPGPU_CLKERNEL_H
#define RSSD_GPGPU_CLKERNEL_H

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <map>
#include <OpenCL/cl.hpp>
#include <rssd/Core.h>
#include "ClBuffer.h"

namespace rssd {
namespace gpgpu {

///
/// @class ClKernel
///

class ClKernel
{
public:
  ClKernel(
    const cl::Context &context,
    const cl::CommandQueue &command_queue,
    const cl::Kernel &kernel) :
    _context(context),
    _command_queue(command_queue),
    _kernel(kernel)
  {
  }

  ~ClKernel()
  {
  }

public:
  cl::Kernel& get() { return this->_kernel; }
  cl::KernelFunctor& getFunctor() { return this->_functor; }
  void setFunctor(cl::KernelFunctor &value) { this->_functor = value; }
  const ClBuffer_m& getBuffers() const { return this->_buffers; }
  ClBuffer* getBuffer(const uint32_t buffer_id)
  {
    ClBuffer_m::iterator iter = this->_buffers.find(buffer_id);
    if (iter == this->_buffers.end())
      return NULL;
    return &iter->second;
  }
  const ClBuffer* getBuffer(const uint32_t buffer_id) const
  {
    ClBuffer_m::const_iterator iter = this->_buffers.find(buffer_id);
    if (iter == this->_buffers.end())
      return NULL;
    return &iter->second;
  }

public:
  template <typename T>
  uint32_t createBuffer(
    const uint32_t index,
    const uint32_t type,
    const uint32_t num_elements,
    const uint32_t flags = 0)
  {
    // Create and store new ClBuffer
    ClBuffer buffer(
      this->_context,
      this->_command_queue,
      index,
      type,
      num_elements * sizeof(T),
      flags);
    this->_buffers.insert(std::make_pair(buffer.getId(), buffer));
    return buffer.getId();
  }

  bool destroyBuffer(const uint32_t buffer_id)
  {
    ClBuffer_m::iterator iter = this->_buffers.find(buffer_id);
    if (iter == this->_buffers.end())
      return false;
    this->_buffers.erase(iter);
    return true;
  }

  cl_uint receiveOutput()
  {
    // Local vars
    cl_uint result = CL_SUCCESS;

    // Enqueue write buffer(s)
    ClBuffer_m::iterator
      iter = this->_buffers.begin(),
      end = this->_buffers.end();
    for (; iter != end; ++iter)
    {
      ClBuffer &buffer = iter->second;
      if (buffer.getFlags() & CL_MEM_WRITE_ONLY)
        result &= this->enqueueBuffer(buffer);
    }
    return result;
  }

  cl_uint sendInput(
    const cl::NDRange &work_group_size,
    const cl::NDRange &global_size)
  {
    // Local vars
    cl_uint result = CL_SUCCESS;

    // Enqueue read buffer(s)
    ClBuffer_m::iterator
      iter = this->_buffers.begin(),
      end = this->_buffers.end();
    for (; iter != end; ++iter)
    {
      ClBuffer &buffer = iter->second;
      if ((buffer.getFlags() & CL_MEM_READ_ONLY)
        || (buffer.getType() == ClBuffer::Type::LOCAL))
        result &= this->enqueueBuffer(buffer);
    }

    // Bind kernel
    this->_functor = this->_kernel.bind(
      this->_command_queue,
      global_size,
      work_group_size);
    return result;
  }

protected:
  cl_uint enqueueBuffer(
      ClBuffer &buffer,
      const bool blocking = true,
      const size_t offset = 0)
  {
    // Local vars
    cl_uint result = CL_SUCCESS;

#if 0
    // Get buffer byte length
    const uint32_t BUFFER_SIZE = sizeof(byte) * buffer.size();
    std::cout << "BUFFER SIZE: " << BUFFER_SIZE << " bytes" << std::endl;
    std::cout << "BUFFER COUNT: " << buffer.size() << " elements" << std::endl;
    std::cout << "BUFFER: ";
    buffer.print<float>();
#endif

    // Enqueue buffer in OpenCL command queue
    /// @note The *READ_ONLY/*WriteBuffer combo
    /// seems backwards but it is correct.
    if (buffer.getFlags() & CL_MEM_READ_ONLY)
    {
      this->_command_queue.enqueueWriteBuffer(
          buffer.getCl(),
          blocking ? CL_TRUE : CL_FALSE,
          offset,
          buffer.size(),
          &buffer[0]);
    }
    else if (buffer.getFlags() & CL_MEM_WRITE_ONLY)
    {
      this->_command_queue.enqueueReadBuffer(
          buffer.getCl(),
          blocking ? CL_TRUE : CL_FALSE,
          offset,
          buffer.size(),
          &buffer[0]);
    }
    else if(buffer.getType() == ClBuffer::Type::LOCAL)
    {
      this->_kernel.setArg(
        buffer.getIndex(),
        cl::__local(buffer.size()));
    }
    return result;
  }

protected:
  cl::Context _context;
  cl::CommandQueue _command_queue;
  cl::Kernel _kernel;
  cl::KernelFunctor _functor;
  ClBuffer_m _buffers;
}; // class ClKernel

///
/// Typedefs
///

TYPEDEF_CONTAINERS(ClKernel);

} // namespace gpgpu
} // namespace rssd

#endif // RSSD_GPGPU_CLKERNEL_H
