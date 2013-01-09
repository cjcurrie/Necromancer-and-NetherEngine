#ifndef NEInc_ManagedMemObj_h
#define NEInc_ManagedMemObj_h

//  #ifndef NEInc_NEAssert_h
//  #include "NEAssert.h"
//  #endif

  #include <list>

// ==================
//    Declaration
// ==================

  namespace NE
  {
    class ManagedMemObj
    {
      DEFINE_THIS_FILE;   // Used by NEAssert
      
      private:
        static std::list<ManagedMemObj *> liveObjects;
        static std::list<ManagedMemObj *> deadObjects;
        long refCount;
        std::list<ManagedMemObj *>::iterator it_positionInLiveList;
      
      protected:
        ManagedMemObj();
        virtual ~ManagedMemObj();   // because this is virtual, the global deconstructor will be used.
        
      public:
        inline void AddReference();   // These are inline because they're short and we don't want to copy the stack every time they're called
        inline void ReleaseReference();
        static void CollectGarbage();   // Called during program to destroy any dynamic memory
                                        //  that has refCount<=0
        static void CollectRemainingObjects(bool bEmitWarnings=false);    // Called at end of the program to do
                                                                          //  one last cleanup
        virtual unsigned long size()=0;
      
    };
  }

  // This macro is used by classes that inherit from ManagedMemObj and need a quick reference to its own size
  #define AUTO_SIZE unsigned long size(){return sizeof(*this);}


// ====================
//    IMPLEMENTATION
// ====================

  // === Methods ===
  inline void NE::ManagedMemObj::AddReference()
  {
    ++refCount;
  }

  inline void NE::ManagedMemObj::ReleaseReference()
  {
    --refCount;
    
    if(refCount<=0)
    {
      ASSERT( this->it_positionInLiveList == liveObjects.end() );     // msg = "This Managed Memory object has already been moved into the deadObjects list.");
      
      //liveObjects.remove(this);   // remove() does a traversal - bad
      liveObjects.erase(this->it_positionInLiveList);     // Unlike vectors and queues, iterators and pointers to items in lists retain
      //  their validity even after an element of the list has been removed
      this->it_positionInLiveList = liveObjects.end();   // It's useless now
      
      deadObjects.push_back(this);
    }
  }

  void NE::ManagedMemObj::CollectGarbage()
  {
    for( std::list<ManagedMemObj *>::iterator it = deadObjects.begin();
        it!=deadObjects.end();
        it++)
    {
      delete (*it);
    }
    
    deadObjects.clear();
  }

  void NE::ManagedMemObj::CollectRemainingObjects(bool bEmitWarnings)
  {
    CollectGarbage();
    for(std::list<ManagedMemObj*>::iterator it = liveObjects.begin();
        it!=liveObjects.end();
        it++)
    {
      ManagedMemObj *o = (*it);
      if(bEmitWarnings)
      {
        // @TODO: log some kind of error message here for each object that has not been released
        
      }
      
      delete o;
    }
    liveObjects.clear();
  }


  // === Constructors and Destructors ===

  //a 'static initialiser' is needed in one of the source files
  //to give the std::list a definitive presence
  std::list<NE::ManagedMemObj *> NE::ManagedMemObj::liveObjects;
  std::list<NE::ManagedMemObj *> NE::ManagedMemObj::deadObjects;

  // Constructor
  NE::ManagedMemObj::ManagedMemObj()
  {
    liveObjects.push_back(this);
    this->it_positionInLiveList = liveObjects.end();     // So the object can quickly remove itself from the static list
    
    // Initialise refCount to zero
    this->refCount=0;
  }

  // Destructor
  NE::ManagedMemObj::~ManagedMemObj()
  {
    // We add an empty virtual destructor to make sure
    // that the destructors in derived classes work properly.
  }

#endif