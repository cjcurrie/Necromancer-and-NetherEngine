#ifndef NEInc_ProfileLogHandler_h
  #define NEInc_ProfileLogHandler_h

  #include "IProfileOutputHandler.h"



  namespace NE
  {
    class ProfileLogHandler : public IProfilerOutputHandler
    {
      const char *IDS_PROFILE_HEADER1 = "Min: \tAvg: \tMax:";
      const char *IDS_PROFILE_HEADER2 = "---------------------------------------";
      const char *IDS_PROFILE_SAMPLE = "%s\t%s\t%s\t%s";
      
      public:
        void BeginOutput();
        void EndOutput();
        void Sample(float fMin, float fAvg, float fMax,
                    int callCount, std::string name, int parentCount);
    };
  }

#endif
