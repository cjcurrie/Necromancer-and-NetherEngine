#ifndef NEInc_ManMemPointer_h
  #define NEInc_ManMemPointer_h

  #include "ManagedMemObj.h"

  // Note that this class is templated and thus must be defined (inline)
  //  in the header in order for the compiler to portably instantiate it.

  // This smart pointer class is similar to the std::shared_ptr
  // Deconstructors are automatically called when objects are popped off the stack, so this class helps prevent memory leaks

  // This smart pointer class is "intrusive", which means that the data is stored in an outside object of a specific type.
  //  In this case, that type is ManagedMemObj. Intrusive SP's can't work with intrinsic data types (e.g. int, float, char)
  //  because the SP needs the support of a managed memory class. If an instrinsic type object needs to be smart-pointed,
  //  a wrapper class may be created that inherits from both ManagedMemObj and the intrinsic type.




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
    class ManMemPointer : public check< relationship<T, ManagedMemObj>::exists >
    {
      
      protected:
        T *data;    // T derives from ManagedMemObj, which is virtual.
      
      public:
      
        // Default constructor
        ManMemPointer()
        {
          data = 0;   // Set the data pointer to zero.
        }
      
      
        // Note: there is no copy constructor that accepts a value parameter - only pointers or references.
      
      
        // --- Copy value
        ManMemPointer(ManMemPointer<T> *other)    // Copy from other ManMemPointer
                                                  //  Note this only works when other's T type and this object's T type are the same
        {        
          *data = *other;    // Now this->data's value equals other->data's value, where each is a ManagedMemObj. One can subsequently be changed without affecting the other.
        }
          
      
        // --- Copy reference
        ManMemPointer(const ManMemPointer<T> &other)    // Copy from another ManMemPointer's address
        {
          data = other;    // this->data now points to the address of other's value. The values are now linked.
        }
      
        // Destructor
        ~ManMemPointer()
        {
          if(data)
            data->ReleaseReference();
        }

      
        // --- Assign from pointer to something that derives from ManMemObj
        inline ManMemPointer<T> operator =(const T &other)
        {
//          if(data)
//            ((ManagedMemObj)*data).ReleaseReference();    // Release the old object
          
          data = other;   // data is now the value of the address of other
          
//          if(data)
//            ((ManagedMemObj)*data).AddReference();        // Increase the refCount of new object
          
          return *this;   // Return this object's value
        }
      
      
        // --- Assign from another smart pointer
        inline ManMemPointer<T> operator =(const ManMemPointer<T> &other)
        {
          REQUIRE(data != other);   // msg = "Cannot assign one smart pointer to another when they each point to the same object".
          
//          if(data)
//            data->ManagedMemObj::ReleaseReference();
          
          data = other.data;    // data is now the value of other's data pointer
          
//          if(data)
//            ((ManagedMemObj)*data).AddReference();
          // Else, this object was assigned a ManMemPointer with no pointer
            
          return *this;   // Return this object's value
        }
      
      
      
        // --- Access as a reference
        inline T& operator *() const
        {
          REQUIRE(data != 0);     // msg = "Tried to * (reference) a managed-memory smart pointer with NULL value"
          return *data;
        }
      
        // --- Access as a pointer
        inline T* operator ->() const
        {
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
