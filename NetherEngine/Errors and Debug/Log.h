#ifndef NEInc_Log_h
  #define NEInc_Log_h

  #include <fstream>

  // This class works like a singleton, although it's actually a global const due to the declaration of 

  // Must be initialized with Log::Init();

  // ==================
  //    Declaration
  // ==================
  namespace NE
  {
    class Log
    {
      public:
        // Constants used for byte flags
        // Note that flags can be combined with logical OR ('|').
        enum {  LOGTO_APP=1,  LOGTO_CLIENT=2,                 LOGTO_SERVER=4,                 LOGTO_CONSOLE=8 };
      // Where: ^ in-game     ^ all client-side engine ops    ^ all server-side engine ops    ^ developer c++ console
      
        static Log &Get();
        
        bool Init();    // This MUST be called before 
      
      
        typedef unsigned char LogID;
        const static LogID          MAX_LOG_STRINGS     = 255;   // Max char value
        
        const static unsigned short MAX_LOG_LINE_LENGTH = 1024;  // Not even close to max short value
      
        // For string-only messages. No formatting, no args. Defaults to console.
        inline void Write(const char *msg){   Write(LOGTO_CONSOLE, msg);  }
      
        // For pre-defined messages with no formatting symbols and no additional arguments. Defaults to the console output.
        inline void Write(LogID msgID) {  Write(LOGTO_CONSOLE, msgID); }
      
        // Pre-defined message from logStrings (which may or may not include formatting symbols. Option for additional arguments.
        void Write(int target, LogID msgID, ...);
      
        // Custom message with formatting and n number arguments.
        void Write(int target, const char *msg, ...);
      
      
      
        void CloseAllLogs();
      
      protected:
        
        // Constructor
        Log();
        
        // The three logfiles
        std::ofstream appLog;
        std::ofstream clientLog;
        std::ofstream serverLog;
        
        
        std::string logStrings_flyweight[MAX_LOG_STRINGS];    // logStrings is a flyweight object used to cache all the standard messages.
        
        bool LoadStrings();
    };
  }

#endif
