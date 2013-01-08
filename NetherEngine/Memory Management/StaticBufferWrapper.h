// Note: There is no implementation file associated with this header.

#ifndef NEInc_StaticBufferWrapper_h
#define NEInc_StaticBufferWrapper_h

  #ifndef NEInc_NEAssert_h
  #include "NEAssert.h"
  #endif

namespace NE
{
  template<class T, int i>
  class  StaticBufferWrapper : public ManagedMemObj
  {
    DEFINE_THIS_FILE;   // Used by NEAssert
    
    protected:
      T buffer[i];
    
    public:
      inline T& operator [](int index)
      {
        ASSERT(index<i && "Bad index on CMMBlob::[]");
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
