// Note: There is no implementation file associated with this header.

#ifndef NEInc_ObjFunctor_h
#define NEInc_ObjFunctor_h

  #ifndef NEInc_Functor_h
  #include "Functor.h"
  #endif

  namespace NE
  {
    template<class T>
    class ObjFunctor : public Functor
    {
      protected:
        T *obj;
        typedef void (T::*funcType)();
        funcType func;
      
      public:
        AUTO_SIZE;
        
        ObjFunctor(T *o, funcType f)
        {
          obj = o;
          func = f;
        }
        
        void operator ()()
        {
          (obj->*func)();
        }
    };
  }

#endif
