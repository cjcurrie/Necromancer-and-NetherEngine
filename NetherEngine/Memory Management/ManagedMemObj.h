/*
 
  All memory-managed classes will inherit from ManagedMemObj.
 
*/

#ifndef NEInc_ManagedMemObj_h
  #define NEInc_ManagedMemObj_h

  #include "Global.h"
  #include "NEAssert.h"
  #include "Log.h"

  #include <list>


  // ==================
  //    Declaration
  // ==================

  // This macro is used by classes that inherit from ManagedMemObj and need a quick reference to its own size
  #define AUTO_SIZE inline unsigned long size(){return sizeof(*this);}

  namespace NE
  {
    class ManagedMemObj
    { 
      private:
        static std::list<ManagedMemObj *> liveObjects;
        static std::list<ManagedMemObj *> deadObjects;
      
        // Instance members
        int refCount;
      
        // Cache this object's position within liveObjects so it can be quickly be moved to deadObjects.
        std::list<ManagedMemObj *>::iterator it_positionInLiveList;
      
      protected:
        ManagedMemObj();
        virtual ~ManagedMemObj();   // because this is virtual, the global deconstructor will be used.
      
      public:
        static void CollectGarbage();   // Called during program to destroy any dynamic memory
                                        //  that has refCount<=0
        static void CollectRemainingObjects(bool bEmitWarnings = false);    // Called at end of the program to do
                                                                            //  one last cleanup
        // Instance members
        virtual unsigned long size() = 0;
      
      
        // These are inline because they're short and we don't want to copy the stack every time they're called
        // Note that inline methods MUST be defined in the header
        inline void AddReference()        {
          ++refCount;
        }
        inline void ReleaseReference()
        {
//          DEFINE_THIS_FILE;
          --refCount;
          
          // We have to check the state of the iterator instead of asserting it because the destructor could call this method a bunch of times
          //  on the same object as its pointers go off the stack.
          if(refCount<=0 && this->it_positionInLiveList != liveObjects.end())
          {
            
            //liveObjects.remove(this);   // remove() does a traversal - bad
            liveObjects.erase(this->it_positionInLiveList);     // Unlike vectors and queues, iterators and pointers to items in lists retain
                                                                //  their validity even after an element of the list has been removed
            this->it_positionInLiveList = liveObjects.end();    // end() is one past the last element. It's our null case.
            
            deadObjects.push_back(this);
          }
        }
    };
  }

#endif