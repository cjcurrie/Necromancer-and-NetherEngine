#ifndef NEInc_IProfileOutputHandler_h
  #define NEInc_IProfileOutputHandler_h

  #include <iostream>

  namespace NE
  {
    class IProfilerOutputHandler
    {
      public:
        virtual void BeginOutput()=0;
        virtual void Sample(float fMin, float fAvg, float fMax,
                            int callCount, std::string name, int parentCount)=0;
        virtual void EndOutput()=0;
    };
  }

#endif
