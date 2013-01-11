#ifndef NEInc_Log_h
  #define NEInc_Log_h

  #include <fstream>

  // ==================
  //    Declaration
  // ==================
  namespace NE
  {
    class Log
    {
      const static unsigned short MAX_LOG_STRINGS = 256,
                                  MAX_LINE_LENGTH = 512;
      
      protected:
      
        // Constructor
        Log();
      
        // The three logfiles
        std::ofstream appLog;
        std::ofstream clientLog;
        std::ofstream serverLog;
      
        std::string logStrings[MAX_LOG_STRINGS];    // logStrings is a flyweight object used to cache all the standard messages.
      
        bool LoadStrings();
        
      public:
        // Constants used for byte flags
        enum {LOGTO_APP=1, LOGTO_CLIENT=2, LOGTO_SERVER=4, LOGTO_USER=8 };
      
        static Log &Get();
        
        bool Init();
        
        void Write(int target, const char *msg, ...);
        void Write(int target, unsigned long msgID, ...);
    };
  }

#endif
