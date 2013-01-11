#ifndef NEInc_ManMemObjFunctor_h
  #define NEInc_ManMemObjFunctor_h

  #include "Functor.h"
  #include "ManMemPointer.h"

  // Note that this Functor class will only template with objects that derive from ManagedMemObj

  namespace NE
  {
    template<ManagedMemObj T>
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
