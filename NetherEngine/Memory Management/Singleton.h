
#ifndef NEInc_Singleton_h
#define NEInc_Singleton_h

  #ifndef NEInc_NEAssert_h
  #include "NEAssert.h"
  #endif


  namespace NE
  {
    template<typename T>
    class Singleton
    {
      DEFINE_THIS_FILE;  // used by NEAssert.h to handle errors
      
      static T* ms_singleton;
      
      public:
        Singleton()
        {
          ASSERT(!ms_singleton);
          
          //use a cunning trick to get the singleton pointing to the start of
          //the whole, rather than the start of the Singleton part of the object
          int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
          ms_singleton = (T*)((int)this + offset);
        }
        ~Singleton()
        {
          ASSERT(ms_singleton);
          ms_singleton=0;
        }
        static T& GetSingleton()
        {
          ASSERT(ms_singleton);
          return *ms_singleton;
        }
        static T* GetSingletonPtr()
        {
          ASSERT(ms_singleton);
          return ms_singleton;
        }
    };

    template <typename T> T* Singleton <T>::ms_singleton = 0;
    
  }

#endif
