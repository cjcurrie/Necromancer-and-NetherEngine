#ifndef NEInc_ManMemPointer_h
  #define NEInc_ManMemPointer_h

  #include "NEAssert.h"     // NEAssert.h includes Global.h
  #include "ManagedMemObj.h"

  // Note that this class is templated and thus must be defined (inline)
  //  in the header in order for the compiler to portably instantiate it.

  // This smart pointer class is similar to the std::shared_ptr
  // Deconstructors are automatically called when objects are popped off the stack, so this class helps prevent memory leaks

  // This smart pointer class is "intrusive", which means that the data is stored in an outside object of a specific type.
  //  In this case, that type is ManagedMemObj. Intrusive SP's can't work with intrinsic data types (e.g. int, float, char)
  //  because the SP needs the support of a managed memory class. If an instrinsic type object needs to be smart-pointed,
  //  a wrapper class may be created that inherits from both ManagedMemObj and the intrinsic type.


  // ======= IMPORTANT!!! =======
  // Operation 'delete mmPtr : ManMemPointer' is totally illegal because it deletes *mmPtr.
  //  If other ManMemPointers are still referencing *mmPtr, then GarbageCollect will try to
  //  destruct the already-deallocated data once those other pointers all go off the stack.



  // ==================
  //    Declaration
  // ==================

  // First, a helper:

  // ===== check< relationship<T, Base>::exists > =====
  namespace NE
  { 
    /*
      We need a way to guarantee (at compile-time) that T will only be Base or derived from Base.
      For ManMemPointer, Base will be defined as ManMemObj, but this template meta-programming 
       trick can be used to check type inheritance at compile-time for other classes as well.
    */
    
    template<typename T, typename Base>
    class relationship
    {
      // Two different sizes
      typedef char Small;             // 1 byte
      class Big { char dummy[2]; };   // Pointer + 2 chars = 6 bytes
      
      static Small Test(const Base&); // If T is convertible to Base, then this Test() is selected.
      static Big Test(...);           // If not convertible, this Test() is selected.
      
      static T MakeT();               // Nothing is being done here. This is simply an empty, typed expression.
      
      public:
        // The size of the return type of Test(T) will only be small if T is convertible to Base
        enum { exists = sizeof(Test(MakeT())) == sizeof(Small) };
    };
    
    // true OR false check
    template<bool>    struct check;             // If not relationship::exists, then the instance of ManMemPointer
                                                //  will inherit from an undefined struct.
    // Specialized for bool = true
    template<>        struct check<true> {};    // Else, the instance inherits from an empty struct;

    // @TODO: use ASSERT to indicate that a non-Base-derived class was used.
  }
    

  // ====== ManMemPointer =====
  namespace NE
  {
    
    template <typename T>
    // A compiler error on this line such as "Implicit instantiation of undefined template" means
    //  that T does not derive from base class ManagedMemObj and therefore is invalid. The 
    //  check<relationship<,>> format will allow implicit instantiation of the base class,
    //  so long as its not virtual (as ManagedMemObj is).
    class ManMemPointer : public check< relationship<T, ManagedMemObj>::exists >
    {      
      protected:
        T *data;    // T derives from ManagedMemObj, which is virtual.
      
      public:
      
        // Default constructor
        ManMemPointer()
        {
          data = NULL;   // Set the data pointer to zero.
        }
      
      
        // Note: there is no copy constructor that accepts a value parameter - only pointers or references.
      
        // --- Copy constructor for type T* : ManagedMemObj
        // This is invoked when this objects is declared and defined on the same line
        //  e.g. ManMemPointer<myClass : ManagedMemObj> stbw_ptr = new StaticBufferWrapper();
        ManMemPointer(T *other)
        {
          data = other;
          
          if(data != NULL)
            data->AddReference();        // Increase the refCount of new object
        }
      
        // --- Copy by value from other MMPtr
        ManMemPointer(const ManMemPointer<T> &other)    // Copy from another ManMemPointer's address
        {
          data = other;    // this->data and other-> now point to the same place.
          
          if(data != NULL)
            data->AddReference();
        }
      
        // Destructor
        ~ManMemPointer()    // Virtual, so no need to initialize base class
        { 
          if(data != NULL)
            data->ReleaseReference();
        }
      

      
        // --- Assign from pointer to something that derives from ManMemObj
        inline void operator =(T *other)    // You don't want this to be assign-chain-able, so return void
        {
          if(data != NULL)
            data->ReleaseReference();    // Release the old object
          
          data = other;   // data is now the value of the address of other
          
          if(data != NULL)
            data->AddReference();        // Increase the refCount of new object
        }
      
        // Assign from pointer to something that derives from ManMemObj

      
      
        // --- Assign from another smart pointer
        inline void operator =(const ManMemPointer<T> &other)
        {
//          DEFINE_THIS_FILE;
          REQUIRE(data != other);   // msg = "Cannot assign one smart pointer to another when they each point to the same object".
          
          if(data != NULL)
            data->ReleaseReference();
          
          data = other.data;    // data is now the value of other's data pointer
          
          if(data != NULL)
            data->AddReference();
          // Else, this object was assigned a ManMemPointer with no pointer
        }
      
      
      
        // --- Access as a reference
        inline T& operator *() const
        {
//          DEFINE_THIS_FILE;
          REQUIRE(data != 0);     // msg = "Tried to * (reference) a managed-memory smart pointer with NULL value"
          return *data;
        }
      
        // --- Access as a pointer
        inline T* operator ->() const
        {
//          DEFINE_THIS_FILE;
          REQUIRE(data != 0);     // msg = "Tried to -> on a NULL smart pointer"
          return data;
        }
      
      
        // --- Cast to type T*
        inline operator T*() const
        {
          return data;
        }
      
        
        // --- Check if data pointer is valid (not NULL)
        inline bool isValid() const
        {
          return (data != 0);     // Casting an int (pointer) to a bool has a performance cost, so this logical operation is better.
                                  //  We don't use isValid within this method for context clarity.
        }
      
        // Other overloaded operators
        inline bool operator !()
        {
          return (data == 0);
        }
      
      
        // The following four operators do almost the exact same thing, but with differing parameters.
        inline bool operator ==(const ManMemPointer<T> &other) const
        {
          // Note that ->other yields other->data, which is a pointer
          return (data == other.data);
        }
      
        inline bool operator ==(const ManagedMemObj* otherData) const
        {
          return (data == otherData);
        }
      
        inline bool operator !=(const ManMemPointer<T> &other) const
        {
          return !(data == other.data);
        }
      
        inline bool operator !=(const ManagedMemObj* otherData) const
        {
          return !(data == otherData);
        }
    };
    
  }

#endif
