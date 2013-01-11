#include "ProfileLogHandler.h"

#include "Log.h"


void NE::ProfileLogHandler::BeginOutput()
{
  Log::Get().Write(Log::LOGTO_APP, IDS_PROFILE_HEADER1);
  Log::Get().Write(Log::LOGTO_APP, IDS_PROFILE_HEADER2);
}

void NE::ProfileLogHandler::EndOutput()
{
  Log::Get().Write(Log::LOGTO_APP,"\n");
}

void NE::ProfileLogHandler::Sample(float fMin, float fAvg, float fMax,
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
  
  Log::Get().Write(Log::LOGTO_APP, IDS_PROFILE_SAMPLE, min, avg, max, num, indentedName);
}
