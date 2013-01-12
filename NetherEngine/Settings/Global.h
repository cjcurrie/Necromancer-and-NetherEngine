#ifndef NEInc_Global_h
  #define NEInc_Global_h

  /*
    The global macros here are used by the preprocessor to include or omit pieces of code
     at compile-time. They are not global constants to be read at runtime.
  */


  #define NE_DEBUG                  // For testing builds only (debug on)


  //#define NOASSERT                // For published builds only (conserves space, eliminates ASSERT-family checks)


  #define MAC_BUILD
  //#define WIN_BUILD               // Toggle build targets. Only one of these three may be enabled. DEATH WILL RESULT otherwise.
  //#define LIN_BUILD


  #define LOG_PREPEND_DATEANDTIME   // Adds a date and local-adjusted timestamp to all Log::Write() outputs

#endif