#ifndef NEInc_Dator_h
  #define NEInc_Dator_h

  #include "NEAssert.h"

  #include "BaseDator.h"

  #include <sstream>


  // This Dator is for unitary objects i.e. not a list or array.

  namespace NE
  {
    template<class T>
    class Dator : public BaseDator
    {      
      protected:
        T &target;
        
        /*
         Note: The following types can be converted with toValue() and toString():
         - bool
         - short, ushort, int, uint, long, and ulong
         - float, double, and long double
         - void*
         - basic_streambuf<T>
         - basic_ostream &
         - basic_ios<,>
         _ ios_base &
         
         These types are defined by ostream::operator << overloads
         */
        
        T toValue(std::string &input)
        {
          std::stringstream str;    // stringstream provides a lexical cast, converting between common types and strings.
          str.unsetf(std::ios::skipws);   // Strips whitespace from the leading end of string
          
          T result;
          
          str << input;   // Put input into the buffer
          str >> result;     // Get input from the buffer. Now it's type T
          
          
          ASSERT( !str.fail() );    // msg = "Conversion operation failed. Was the type of target an allowed type?"
          
          return result;
        }
        
        std::string toString(T &input)    // This is the complement to toValue() above
        {
          std::stringstream str;
          str.unsetf(std::ios::skipws);
          
          std::string result;
          
          str << input;
          str >> result;
          
          
          ASSERT( !str.fail() );
          
          return result;
        }
      
      public:
        Dator(const T &t) : target(t) {}
      
        BaseDator &operator =(const std::string &rhsInput)
        {
          // Copy-swap isn't needed here because toValue copy-swaps and returns a value.
          
          target = toValue(rhsInput);
          return *this;
        }
      
        BaseDator &operator +=(std::string &rhsInput)
        {
          target += toValue(rhsInput);
          return *this;
        }
      
        BaseDator &operator -=(std::string &rhsInput)
        {
          target -= toValue(rhsInput);
          return *this;
        }
      
        bool operator ==(const std::string &rhsInput)
        {
          return (rhsInput == (std::string)(*this));
        }
      
        bool operator !=(std::string &rhsInput)
        {
          return (rhsInput != (std::string)(*this));
        }
      
        // Explicit cast to string operator
        operator std::string()
        {
          return toString(target);
        }
        
        bool hasMultipleValues()
        {
          return false;   // Unitary objects do not have multiple values.
        }
        
        AUTO_SIZE;
    };
  }

#endif