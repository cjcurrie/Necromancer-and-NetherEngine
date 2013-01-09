#ifndef NEInc_Log_h
#include "Log.h"
#endif

//#ifndef Inc_iostream
#include <iostream>
//#define Inc_iostream
//#endif

// ====================
//    IMPLEMENTATION
// ====================

NE::Log::Log()
{
  //the constructor doesn't do anything, but we need
  //it for our singleton to work correctly
}

NE::Log &NE::Log::Get()
{
  static Log log;   // This uses the constructor, which is empty.
  return log;
}

bool NE::Log::Init()
{
  appLog.open("applicationLog.txt");
  clientLog.open("clientLog.txt");
  serverLog.open("serverlog.txt");
  //user errors get logged to client
  
  //load the strings file
  if(!LoadStrings())return false;
  
  return true;
}

// Writes an integer
void NE::Log::Write(int target_enum, unsigned long msgID, ...)
{
  // va_list is mapped to char* in the header
  va_list args;
  // va_start is a macro
  
  va_start(args, msgID);
  char szBuf[1024];
  vsprintf(szBuf,logStrings[msgID].c_str(),args);
  Write(target_enum,szBuf);
}

#define LOG_PREPEND_DATEANDTIME
#ifdef LOG_PREPEND_DATEANDTIME
  #include <ctime>
#endif

// Writes a string
void NE::Log::Write(int target_enum, const char *msg, ...)   // Last parameter is variable-argument; data is dealt with at the byte level
{
  // Initialize a list of arguments to be pumped through the vsprintf formatter.
  va_list argList;
  va_start(argList, msg);
  
  char buffer[1024];    // Contains the result to be outputted.
  vsprintf(buffer, msg, argList);
  
  #ifdef LOG_PREPEND_DATEANDTIME
    #define TBUFF_MAXSIZE 32
    // Prepend the time
    char timeBuffer[TBUFF_MAXSIZE];
  
  
  time_t rawtime = time(NULL);
  
  strftime (timeBuffer, TBUFF_MAXSIZE , "[%x %I:%M%p%z]\n", localtime(&rawtime) );
  #endif
  
  if(target_enum & LOG_APP)    // Note the bitwise operation
  {
    #ifdef LOG_PREPEND_DATEANDTIME
        appLog << timeBuffer;
    #endif
    
    appLog << buffer << "\n";
    
    #ifdef NE_DEBUG    // Note that NE_DEBUG, like NOASSERT, is set in main.*
          std::cout << "should be outputting";
    appLog.flush();
    #endif
  }
  
  if(target_enum & LOG_CLIENT)
  {
    #ifdef LOG_PREPEND_DATEANDTIME
        clientLog << timeBuffer;
    #endif
    
    clientLog << buffer << "\n";
    
    #ifdef NE_DEBUG
      clientLog.flush();
    #endif
  }
  
  if(target_enum & LOG_SERVER)
  {
    #ifdef LOG_PREPEND_DATEANDTIME
        serverLog << timeBuffer;
    #endif
    
    serverLog << buffer << "\n";
    
    #ifdef NE_DEBUG
      serverLog.flush();
    #endif
  }
  
  if(target_enum & LOG_USER)
  {
    #ifdef LOG_PREPEND_DATEANDTIME
        std::cout << timeBuffer;
    #endif
    
    // @TODO: give the user some kind of output, such as a pop-up? (system-dependent)
    // MessageBox(NULL,szBuf,"Message",MB_OK);
    std::cout << buffer << std::endl;
  }
  
  //va_end(argList);    // Release the variable-argument list macro
}


// Load the strings from a previous log into this current log. Note that
//  any Write() operation will overwrite the entire contents of the log.
bool NE::Log::LoadStrings()
{
  std::ifstream inStream("strings.txt");
  
  if( !inStream.is_open() )
  {
    return false;
  }
  
  unsigned long int index=0;
  
  while( !inStream.eof() )
  {
    char szBuf[1024];
    inStream.getline(szBuf,1024);
    logStrings[index++] = szBuf;
  }
  
  return true;
}
