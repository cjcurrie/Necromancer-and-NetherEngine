#include "ManagedMemObj.h"

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
  
  // Move to one element past the last element, then take a step back to the last element (the one we just added).
  this->it_positionInLiveList = liveObjects.end();
  this->it_positionInLiveList--;
  
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
void NE::ManagedMemObj::CollectGarbage()
{
  for( std::list<ManagedMemObj *>::iterator it = deadObjects.begin(); it != deadObjects.end(); it++)
  {
    Log::Get().Write( Log::LOGTO_CONSOLE, "Garbaging pointer: %i", *it);
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
      Log::Get().Write( Log::LOGTO_CONSOLE, "ERROR: managed memory object of type %s was leaked and caught by CollectRemainingObjects.", typeid(*o).name() );
    }
    
    delete o;
  }
  liveObjects.clear();    // Destroys all objects in liveObjects
}