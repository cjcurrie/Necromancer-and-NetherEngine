#ifndef NEInc_ProfileSample_h
#define NEInc_ProfileSample_h

  #ifndef Inc_glfw_h
    #include <GL/glfw.h>    // also includes glu.h
    #define Inc_glfw_h
  #endif

  #ifndef NEInc_NEAssert_h
  #include "NEAssert.h"
  #endif

  #ifndef NEInc_ProfileLogHandler_h
  #include "ProfileLogHandler.h"
  #endif


// ==================
//    Declaration
// ==================

  #define MAX_PROFILER_SAMPLES 999

  namespace NE
  {
    class ProfileSample
    {
      //DEFINE_THIS_FILE;   // Used by NEAssert
      
      public:
        ProfileSample( std::string sampleName );
        ~ProfileSample();
        
        static void Output();
        
        static void ResetSample( std::string sampleName );
        static void ResetSample( unsigned int sampleIndex );
        static void ResetAll();
        
        static IProfilerOutputHandler *outputHandler;
        
      protected:
        //index into the array of samples
        int iSampleIndex;
        int iParentIndex;
        
        inline float GetTime(){ return ((float)glfwGetTime())/1000.0f; }
        
        struct profileSample
        {
          profileSample()
          {
            isValid = false;
            dataCount = 0;
            averagePc = minPc = maxPc = -1;
          }
          
          bool isValid;          // whether or not this sample is valid to be used
          bool isOpen;           // is this sample currently being profiled?
          unsigned int callCount; // number of times this sample has been executed
          
          std::string name;       // name of the sample
          
          float startTime;  // starting time on the clock, in seconds
          float totalTime;  // total time recorded across all executions of this sample
          float childTime;  // total time taken by children of this sample
          
          int parentCount;  // number of parents this sample has
                            // (useful for neat indenting)
          
          float averagePc;  //average percentage of game loop time taken up
          float minPc;      //minimum percentage of game loop time taken up
          float maxPc;      //maximum percentage of game loop time taken up
          
          unsigned long dataCount; //number of times values have been stored since
          //sample creation/reset
        };
      
        static profileSample samples[MAX_PROFILER_SAMPLES];
      
        static int lastOpenedSample;
        static int openSampleCount;
        static float rootBegin, rootEnd;
    };
  }



#endif
