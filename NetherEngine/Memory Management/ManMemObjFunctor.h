#ifndef NEInc_ManMemObjFunctor_h
#define NEInc_ManMemObjFunctor_h

  #ifndef NEInc_ManMemPointer_h
  #include "ManMemPointer.h"
  #endif

namespace NE
{
  template<class T>
  class ManMemObjFunctor : public Functor
  {
    protected:
      ManMemPointer<T> obj;
      typedef int (T::*funcType)();
      funcType func;
    
    public:
      AUTO_SIZE;
      
      ManMemObjFunctor(T *o, funcType f)
      {
        obj = o;
        func = f;
      }
      
      void operator ()()
      { return (obj->*func)(); }
  };
}

#endif
