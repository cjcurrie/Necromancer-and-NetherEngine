#ifndef NEInc_Singleton_h
  #define NEInc_Singleton_h

  #include "NEAssert.h"

  /*
    Note:
      When possible to use, local static objects are MUCH better than singleton patterns because they:
        - Automatically get destructed when they leave the stack (no guarantee with singletons [or
            any heap-allocated object created with the 'new' keyword, for that matter]). 
        - Can't be accessed before they are created. An object declared as a Singleton can't be
            accessed until it's defined, but static objects are available immediately after 
            compile-time.
        - The above properties make static objects easier to debug.
  */

  namespace NE
  {
    template<typename T>
    class Singleton
    { 
      static T* ms_singleton;   // Only one instance of ms_singleton can exist per type.
      
      public:
        Singleton()
        {
          ASSERT(!ms_singleton);    // Can't construct a singleton more than once
          
          // This is a cunning casting trick to get the singleton pointing to the start of
          //the whole, rather than the start of the Singleton part of the object
          unsigned long int offset = (unsigned long int)( ((unsigned long int)((T*)1))
                        - (unsigned long int)((Singleton <T>*)(T*)1) );
          ms_singleton = (T*)((unsigned long int)this + offset);
        }
      
        ~Singleton()
        {
          ASSERT(ms_singleton);   // Can't destruct a singleton that hasn't been constructed...
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
