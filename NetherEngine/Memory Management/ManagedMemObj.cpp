#include "ManagedMemObj.h"

#include "NEAssert.h"
DEFINE_THIS_FILE;

#include "Log.h"


// ====================
//    IMPLEMENTATION / definition
// ====================

// === Constructors and Destructors ===

  // A 'static initialiser' is needed in one of the source files
  //  to give the std::list a definitive presence
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
  for( std::list<ManagedMemObj *>::iterator it = deadObjects.begin(); it != deadObjects.end(); it++)
  {
    Log::Get().Write( Log::LOGTO_USER, "Garbaging pointer: %i", *it);
    delete (*it);
  }
  
  deadObjects.clear();
}

void NE::ManagedMemObj::CollectRemainingObjects(bool bEmitWarnings)
{
  CollectGarbage();
  for(std::list<ManagedMemObj*>::iterator it = liveObjects.begin(); it!=liveObjects.end(); it++)
  {
    ManagedMemObj *o = (*it);
    if(bEmitWarnings)
    {
      Log::Get().Write( Log::LOGTO_USER, "ERROR: pointer %i was leaked.", o);
    }
    
    delete o;
  }
  liveObjects.clear();
}