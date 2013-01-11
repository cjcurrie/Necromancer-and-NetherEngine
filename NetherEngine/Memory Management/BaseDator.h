#ifndef NEInc_BaseDator_h
  #define NEInc_BaseDator_h

  #include "ManagedMemObj.h"


  namespace NE
  {
    class BaseDator : public ManagedMemObj
    {
      protected:
        BaseDator(){}
        BaseDator(const BaseDator &b){*this = b;}   // Note that all copy constructors needs must take const arguments in order to work with temporary objects.
      
      public:
      virtual BaseDator &operator =(const std::string &s) = 0;
        virtual BaseDator &operator +=(std::string &s) = 0;
        virtual BaseDator &operator -=(std::string &s) = 0;
        virtual bool operator ==(std::string &s) = 0;
        virtual bool operator !=(std::string &s) = 0;
        
        virtual bool hasMultipleValues() = 0;   // Things like ints and chars are discrete, while things with hasMultipleValues = true are like lists and vectors and stacks.
        
        virtual operator std::string() = 0;
    };
  }

#endif