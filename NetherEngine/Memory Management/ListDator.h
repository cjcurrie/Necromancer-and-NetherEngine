#ifndef NEInc_ListDator_h
  #define NEInc_ListDator_h

  #include "BaseDator.h"

  #include <sstream>


  // This Dator is for a list of object-wrappers.

  namespace NE
  {
    template<class T>
    class ListDator : public BaseDator
    {
      protected:
        std::list<T> &values;
      
        T toValue(std::string &input)
        {
          std::stringstream str;
          
          str.unsetf(std::ios::skipws);   // Clear whitespace
          
          T result;
          
          str<<input;
          str>>result;
          
          return result;
        }
      
        std::string toString(T &input)
        {
          std::stringstream str;
          str.unsetf(std::ios::skipws);
          
          std::string result;
          
          str<<input;
          str>>result;
          
          return result;
        }
      
      public:
        ListDator(const std::list<T> &v) : values(v) { }
      
        BaseDator &operator =(std::string &s)
        {
          values.clear();
          values.push_back(toValue(s));
          return *this;
        }
      
        BaseDator &operator +=(std::string &s)
        {
          values.push_back(toValue(s));
          return *this;
        }
      
        BaseDator &operator -=(std::string &s)
        {
          values.remove(toValue(s));
          return *this;
        }
      
        bool operator ==(std::string &s)
        {
          return (std::find( values.begin(), values.end(), toValue(s) ) != values.end());
        }
      
        bool operator !=(std::string &s)
        {
          return !((*this) == s);
        }
        
        operator std::string()
        {
          return toString(values.back());
        }
      
        operator std::list<T>&()
        {
          return values;
        }
        
        bool hasMultipleValues()
        { return true; }
        
        AUTO_SIZE;
    };
  }

#endif