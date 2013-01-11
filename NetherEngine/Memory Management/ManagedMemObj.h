/*
 
  All memory-managed classes will inherit from ManagedMemObj.
 
*/

#ifndef NEInc_ManagedMemObj_h
  #define NEInc_ManagedMemObj_h

  #include <list>


  // ==================
  //    Declaration
  // ==================

  // This macro is used by classes that inherit from ManagedMemObj and need a quick reference to its own size
  #define AUTO_SIZE unsigned long size(){return sizeof(*this);}

  namespace NE
  {
    class ManagedMemObj
    {
      private:  
        static std::list<ManagedMemObj *> liveObjects;
        static std::list<ManagedMemObj *> deadObjects;
      
        // Instance members
        long refCount;
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
        inline void AddReference();   // These are inline because they're short and we don't want to copy the stack every time they're called
        inline void ReleaseReference();
      
    };
  }

#endif