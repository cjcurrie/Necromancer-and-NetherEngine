#ifndef NEInc_ProfileLogHandler_h
#define NEInc_ProfileLogHandler_h

  #ifndef NEInc_IProfileOutputHandler_h
  #include "IProfileOutputHandler.h"
  #endif

  #ifndef NEInc_Log_h
  #include "Log.h"
  #endif

  #define IDS_PROFILE_HEADER1 "Min: \tAvg: \tMax:"
  #define IDS_PROFILE_HEADER2 "------------------------------------------------------"
  #define IDS_PROFILE_SAMPLE "%s\t%s\t%s\t%s"

  namespace NE
  {
    class ProfileLogHandler : public IProfilerOutputHandler
    {
      public:
        void BeginOutput();
        void EndOutput();
        void Sample(float fMin, float fAvg, float fMax,
                    int callCount, std::string name, int parentCount);
      };

      void ProfileLogHandler::BeginOutput()
      {
        Log::Get().Write(LOG_APP, IDS_PROFILE_HEADER1);
        Log::Get().Write(LOG_APP, IDS_PROFILE_HEADER2);
      }

      void ProfileLogHandler::EndOutput()
      {
        Log::Get().Write(LOG_APP,"\n");
      }

      void ProfileLogHandler::Sample(float fMin, float fAvg, float fMax,
                                      int callCount, std::string name, int parentCount)
      {
        char namebuf[256], indentedName[256];
        char avg[16], min[16], max[16], num[16];
        
        sprintf(avg, "%3.1f", fAvg);
        sprintf(min, "%3.1f", fMin);
        sprintf(max, "%3.1f", fMax);
        sprintf(num, "%3d",   callCount);
        
        strcpy( indentedName, name.c_str());
        for( int indent=0; indent<parentCount; ++indent )
        {
          sprintf(namebuf, " %s", indentedName);
          strcpy( indentedName, namebuf);
        }
        
        Log::Get().Write(LOG_APP, IDS_PROFILE_SAMPLE, min, avg, max, num, indentedName);
    }
  }

#endif
