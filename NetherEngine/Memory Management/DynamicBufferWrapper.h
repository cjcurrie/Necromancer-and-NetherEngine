// Note: There is no implementation file associated with this header.

#ifndef NEInc_DynamicBufferWrapper_h
#define NEInc_DynamicBufferWrapper_h

//  #ifndef NEInc_NEAssert_h
//  #include "NEAssert.h"
//  #endif

namespace NE
{
  template<class T>
  class DynamicBufferWrapper : public ManagedMemObj
  {
    DEFINE_THIS_FILE;   // Used by NEAssert
    
    protected:
      unsigned long dataSize;
      T *buffer;
    
    public:
      inline T& operator [](int index)
      {
        ASSERT(index<dataSize);     // msg = "Bad index on CMMDynamicBlob::[]"
        return buffer[index];
      }
    
      inline operator T*()
      {
        return buffer;
      }
    
      // Parameterized constructor
      DynamicBufferWrapper(unsigned long size)
      {
        dataSize = size;
        buffer = new T[size];
        ASSERT(buffer);     // msg = "DynamicBlob buffer could not be created - out of memory?"
      }
    
      ~DynamicBufferWrapper()
      {
        if(buffer)delete[] buffer;
      }
      
      unsigned long size()
      {
        return dataSize+sizeof(this);
      }
    
      inline unsigned long blobSize()
      {
        return dataSize;
      }
  };
}

#endif
