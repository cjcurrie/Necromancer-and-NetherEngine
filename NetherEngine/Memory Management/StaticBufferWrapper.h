#ifndef NEInc_StaticBufferWrapper_h
  #define NEInc_StaticBufferWrapper_h

  #include "ManagedMemObj.h"

  namespace NE
  {
    template<class T, int i>
    class  StaticBufferWrapper : public ManagedMemObj
    {
      
      protected:
        T buffer[i];
      
      public:
        inline T& operator [](int index)
        {
          ASSERT(index<i);    // msg = "Bad index on CMMBlob::[]"
          return buffer[index];
        }
      
        inline operator T*()
        {
          return buffer;
        }
      
        // Declared in ManagedMemObj, this macro returns the byte-width of this wrapper (sans the buffer data).
        AUTO_SIZE;
    };
  }

#endif
