#ifndef NEInc_ManMemPointer_h
#define NEInc_ManMemPointer_h

  #ifndef NEInc_NEAssert_h
  #include "NEAssert.h"
  #endif

// ==================
//    Declaration
// ==================

  namespace NE
  {
    template<class T>
    class ManMemPointer
    {
      DEFINE_THIS_FILE;   // Used by NEAssert
      
      protected:
        T* obj;
      
      public:
      
        // Constructors
        ManMemPointer();
        ManMemPointer(T *o);
        ManMemPointer(const ManMemPointer<T> &p);
        // Destructor
        ~ManMemPointer();

        //Assignement operators - assigning a plain pointer
        inline ManMemPointer<T> operator =(T *o);
        //Assigning another smart pointer
        inline ManMemPointer<T> operator =(const ManMemPointer<T> &p);
      
        //Access as a reference
        inline T& operator *() const;
        //Access as a pointer
        inline T* operator ->() const;
      
        // Convert the ManMemPointer to type T*
        inline operator T*() const;
      
        // Check if pointer is valid (not null)
        inline bool isValid() const;
      
        // Other overloaded operators
        inline bool operator !();
        inline bool operator ==(const ManMemPointer<T> &p) const;
        inline bool operator ==(const T* o) const;
    };
  }

// ====================
//    IMPLEMENTATION
// ====================


// Default constructor
template<class T>
NE::ManMemPointer<T>::ManMemPointer()
{
  this->obj=0;
}

// Constructing with a pointer
template<class T>
NE::ManMemPointer<T>::ManMemPointer(T *o)
{
  obj=0;
  *this=o;
}

// Constructing with another smart pointer (copy constructor)
template<class T>
NE::ManMemPointer<T>::ManMemPointer(const ManMemPointer<T> &p)
{
  obj=0;
  *this=p;
}

// Destructor
template<class T>
NE::ManMemPointer<T>::~ManMemPointer()
{
  if(obj)obj->Release();
}

// Assignement operators - assigning a plain pointer
template<class T>
inline NE::ManMemPointer<T> NE::ManMemPointer<T>::operator =(T *o)
{
  if(obj)obj->Release();
  obj=o;
  if(obj)obj->AddRef();
}

//Assigning another smart pointer
template<class T>
inline NE::ManMemPointer<T> NE::ManMemPointer<T>::operator =(const ManMemPointer<T> &p)
{
  // @TODO: check for self-assignment
  
  if(obj)obj->Release();
  obj=p.obj;
  if(obj)obj->AddRef();
}

//Access as a reference
template<class T>
inline T& NE::ManMemPointer<T>::operator *() const
{
  ASSERT(obj!=0 && "Tried to * on a NULL smart pointer");
  return *obj;
}

//Access as a pointer
template<class T>
inline T* NE::ManMemPointer<T>::operator ->() const
{
  ASSERT(obj!=0 && "Tried to -> on a NULL smart pointer");
  return obj;
}


//Conversion - allow the smart pointer to be automatically
//converted to type T*
template<class T>
inline NE::ManMemPointer<T>::operator T*() const
{
  return obj;
}

// Check for not-equal-to-null
template<class T>
inline bool NE::ManMemPointer<T>::isValid() const
{
  return (obj!=0);
}

// Other overloaded operators
template<class T>
inline bool NE::ManMemPointer<T>::operator !()
{
  return !(obj);
}

template<class T>
inline bool NE::ManMemPointer<T>::operator ==(const ManMemPointer<T> &p) const
{
  return (obj==p.obj);
}

template<class T>
inline bool NE::ManMemPointer<T>::operator ==(const T* o) const
{
  return (obj==o);
}

#endif
