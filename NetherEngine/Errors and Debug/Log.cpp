#include "Log.h"
#include "Global.h"

#include <iostream>

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
  // Eser messages get logged to console
  
  //load the strings file
  if(!LoadStrings())return false;
  
  return true;
}

// Writes an message based upon the message table, logStrings
void NE::Log::Write(int target_enum, unsigned long msgID, ...)
{
  // va_list is mapped to char* in the header
  va_list args;
  // va_start is a macro
  
  std::cout << "writing to " << msgID << ", which is " << logStrings[msgID] << std::endl;
  
  va_start(args, msgID);
  char szBuf[1024];
  vsprintf(szBuf,logStrings[msgID].c_str(),args);
  Write(target_enum,szBuf);
}


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
    const char TBUFF_MAXSIZE = 32;
    // Prepend the time
    char timeBuffer[TBUFF_MAXSIZE];
  
  
    time_t rawtime = time(NULL);
  
    strftime (timeBuffer, TBUFF_MAXSIZE , "[%x %I:%M%p%z]\n", localtime(&rawtime) );
  #endif
  
  if(target_enum & LOGTO_APP)    // Note the bitwise operation
  {
    appLog << "\n";
    #ifdef LOG_PREPEND_DATEANDTIME
      appLog << timeBuffer;
    #endif
    appLog << buffer << "\n";
    
    /*
      Note: flush() will immediately synchronize the file with the buffer. When !NE_DEBUG,
        flush() does not occur, so the file is only synced when appLog is close()d or destroyed.
        This happens when the appLog, etc. goes out of scope & is destructed, at the end of the program.
        
      Because we want to know FOR SURE where the final sync takes place during !NE_DEBUG,
        main will call CloseAllLogs() at the end of the program.
     
      Log sync will also take place when the buffer object (buffer : char) is full.
    */
    
    #ifdef NE_DEBUG    // Note that NE_DEBUG, like NOASSERT, is defined in Global.h
      appLog.flush();   
                      //  @TODO: close logs at end of runtime? Even outside of debug mode?
    #endif
  }
  
  if(target_enum & LOGTO_CLIENT)
  {
    clientLog << "\n";
    #ifdef LOG_PREPEND_DATEANDTIME
      clientLog << timeBuffer;
    #endif
    clientLog << buffer << "\n";
    
    #ifdef NE_DEBUG
      clientLog.flush();
    #endif
  }
  
  if(target_enum & LOGTO_SERVER)
  {
    serverLog << "\n";
    #ifdef LOG_PREPEND_DATEANDTIME
      serverLog << timeBuffer;
    #endif
    
    serverLog << buffer << "\n";
    
    #ifdef NE_DEBUG
      serverLog.flush();
    #endif
  }
  
  if(target_enum & LOGTO_USER)
  {
    std::cout << std::endl;
    #ifdef LOG_PREPEND_DATEANDTIME
        std::cout << timeBuffer;
    #endif
    // @TODO: give the user some kind of output, such as a pop-up? (system-dependent)
    std::cout << buffer << std::endl << std::endl;
  }
  
  va_end(argList);    // Release the variable-argument list macro
}


// Load the strings from a previous log into this current log. Note that
//  any Write() operation will overwrite the entire contents of the log.
bool NE::Log::LoadStrings()
{
  char logStringPath[] = "logMessageStringTable.txt";
  // @TODO: get from settings
  
  std::ifstream inStream(logStringPath);
  
  if( !inStream.is_open() )
  {
    #ifdef NE_DEBUG
      std::cout << "Could not open file " << logStringPath << "; ";
      perror(NULL);
    #endif
    
    return false;
  }
  
  unsigned long int index=0;
  
  // For testing, enable exceptions
  inStream.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
  
  try
  {
    while( !inStream.eof() )
    {
      char szBuf[MAX_LINE_LENGTH];
      inStream.getline(szBuf, MAX_LINE_LENGTH);   // Has an implied line delimiter, '\n'
      logStrings[index++] = szBuf;
    }
  }
  catch (std::ifstream::failure e) {
    #ifdef NE_DEBUG
      std::cout << "Reading logStrings file threw error " << e.code() << ": " << e.what() << std::endl;
    #endif
    return false;
  }
  
  return true;
}
