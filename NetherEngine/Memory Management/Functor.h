#ifndef NEInc_Functor_h
#define NEInc_Functor_h

  namespace NE
  {
    
    class Functor : public ManagedMemObj
    {
      
      public:
        virtual void operator ()()=0;
      
    };
    
  }

#endif
