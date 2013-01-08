#ifndef NEInc_Log_h
#define NEInc_Log_h

  #include <fstream>

  // ==================
  //    Declaration
  // ==================
  namespace NE
  {
    // Constants used for byte flags
    const int LOG_APP=1;
    const int LOG_CLIENT=2;
    const int LOG_SERVER=4;
    const int LOG_USER=8;

    #define MAX_LOG_STRINGS 256

    class Log
    {
      protected:
        // Constructor
        Log();
      
        // The three logfiles
        std::ofstream appLog;
        std::ofstream clientLog;
        std::ofstream serverLog;
        
        std::string logStrings[MAX_LOG_STRINGS];
      
        bool LoadStrings();
        
      public:
        static Log &Get();
        
        bool Init();
        
        void Write(int target, const char *msg, ...);
        void Write(int target, unsigned long msgID, ...);
    };
  }

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
      static Log log;   // Declared here but initialized in ... ?
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

    void NE::Log::Write(int target, const char *msg, ...)   // Last parameter is variable-argument; data is dealt with at the byte level
    {
      va_list args;
      va_start(args,msg);
      
      char szBuf[1024];
      vsprintf(szBuf,msg,args);
      
      if(target & LOG_APP)    // Note the bitwise operation
      {
        appLog<<szBuf<<"\n";
        
        #ifdef DEBUG    // Note that DEBUG, like NOASSERT, is set in main.*
            appLog.flush();
        #endif
      }
      
      if(target & LOG_CLIENT)
      {
        clientLog<<szBuf<<"\n";
        
        #ifdef DEBUG
            clientLog.flush();
        #endif
      }
      
      if(target&LOG_SERVER)
      {
        serverLog<<szBuf<<"\n";
        
        #ifdef DEBUG
            serverLog.flush();
        #endif
      }
      if(target&LOG_USER)
      {
        
        // @TODO: give the user some kind of output, such as a pop-up? (system-dependent)
        // MessageBox(NULL,szBuf,"Message",MB_OK);
        
      }
    }

    void NE::Log::Write(int target, unsigned long msgID, ...)
    {
      // va_list is mapped to char* in the header
      va_list args;
      // va_start is a macro
      
      va_start(args, msgID);
      char szBuf[1024];
      vsprintf(szBuf,logStrings[msgID].c_str(),args);
      Write(target,szBuf);
    }

    // All platforms load the strings in from strings.txt
    bool NE::Log::LoadStrings()
    {
      std::ifstream in("strings.txt");
      if(!in.is_open())return false;
      
      unsigned long int index=0;
      
      while(!in.eof())
      {
        char szBuf[1024];
        in.getline(szBuf,1024);
        logStrings[index++] = szBuf;
      }
      
      return true;
    }

#endif
