#ifndef NEInc_DynamicBufferWrapper_h
  #define NEInc_DynamicBufferWrapper_h

  #include "ManagedMemObj.h"

  // Note that this class is templated and thus must be defined (inline)
  //  in the header in order for the compiler to portably instantiate it.

  namespace NE
  {
    template<class T>
    class DynamicBufferWrapper : public ManagedMemObj
    {      
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
