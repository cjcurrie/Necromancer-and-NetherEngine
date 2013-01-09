#ifndef NEInc_Log_h
#define NEInc_Log_h

  #include <fstream>

  // ==================
  //    Declaration
  // ==================
  namespace NE
  {
    // Constants used for byte flags
    enum {LOG_APP=1, LOG_CLIENT=2, LOG_SERVER=4, LOG_USER=8 };

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

#endif
