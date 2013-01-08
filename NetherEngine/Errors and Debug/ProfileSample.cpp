#ifndef NEInc_ProfileSample_h
#include "ProfileSample.h"
#endif

// ====================
//    IMPLEMENTATION
// ====================

// Static vars
NE::IProfilerOutputHandler *NE::ProfileSample::outputHandler = new NE::ProfileLogHandler();
int NE::ProfileSample::lastOpenedSample = 0;
int NE::ProfileSample::openSampleCount = 0;
float NE::ProfileSample::rootBegin = 0;
float NE::ProfileSample::rootEnd = 0;
NE::ProfileSample::profileSample NE::ProfileSample::samples[MAX_PROFILER_SAMPLES];




NE::ProfileSample::ProfileSample(std::string sampleName)
{
  DEFINE_THIS_FILE;   // Required to use ASSERTions
  
  //The first thing we need to do is restore our previous pieces of sample
  //data from storage. That is, look in the samples[] array to see if there's
  //a valid sample with our name on it
  int i=0;
  //If we don't find it, we're going to need to create a new sample, and rather
  //than looping through the list a second time we store the first non-valid
  //index that we find this time round
  int storeIndex=-1;
  
  for(i=0; i<MAX_PROFILER_SAMPLES; ++i)
  {
    if(!samples[i].isValid)
    {
      if(storeIndex < 0)
        storeIndex = i;
    }
    else
    {
      if(samples[i].name == sampleName)
      {
        //this is the sample we want
        //check that it's not already open
        //assert only works in debug builds, but given that you don't use
        //the profiler in release builds, it doesn't really matter
        ASSERT(!samples[i].isOpen && "Failed to profile a sample which was already being profiled");
        
        //first, store its index
        iSampleIndex = i;
        
        //the parent sample is the last opened sample
        iParentIndex = lastOpenedSample;
        lastOpenedSample = i;
        
        //and the number of parents is the number of open
        //samples (excluding ourselves)
        samples[i].parentCount = openSampleCount;
        ++openSampleCount;
        samples[i].isOpen = true;
        
        //increment the number of times we've been called
        ++samples[i].callCount;
        
        ///finally (more or less) store the current time to start the sample
        samples[i].startTime=GetTime();
        
        //if this has no parent, it must be the 'main loop' sample, so copy
        //to the global timer as well
        if(iParentIndex<0)
          rootBegin = samples[i].startTime;
        
        //done
        return;
      }
    }
  }
  
  //we've not found it, so it must be a new sample
  //use the storeIndex value to store the new sample
  ASSERT(storeIndex >= 0 && "Profiler has run out of sample slots!");
  
  samples[storeIndex].isValid = true;
  samples[storeIndex].name = sampleName;
  
  iSampleIndex = storeIndex;
  iParentIndex = lastOpenedSample;
  
  lastOpenedSample = storeIndex;
  samples[i].parentCount = openSampleCount;
  openSampleCount++;
  
  samples[storeIndex].isOpen = true;
  samples[storeIndex].callCount = 1;
  
  //init the statistics for this sample
  samples[storeIndex].totalTime = 0.0f;
  samples[storeIndex].childTime = 0.0f;
  samples[storeIndex].startTime = GetTime();
  
  if(iParentIndex < 0)
    rootBegin = samples[storeIndex].startTime;
  
  
  // Lastly, find an output handler for the profiler data
  outputHandler = new ProfileLogHandler();
}

NE::ProfileSample::~ProfileSample()
{
  float fEndTime = GetTime();
  //phew... ok, we're done timing
  
  samples[iSampleIndex].isOpen = false;
  //calculate the time taken this profile, for ease of use later on
  
  float fTimeTaken = fEndTime - samples[iSampleIndex].startTime;
  
  if(iParentIndex >= 0)
  {
    samples[iParentIndex].childTime+=fTimeTaken;
  }
  else
  {
    //no parent, so this is the end of the main loop sample
    rootEnd = fEndTime;
  }
  
  samples[iSampleIndex].totalTime += fTimeTaken;
  lastOpenedSample = iParentIndex;
  
  --openSampleCount;
}


void NE::ProfileSample::Output()
{
  DEFINE_THIS_FILE;   // Required to use ASSERTions
  
  ASSERT(outputHandler && "Profiler has no output handler set");
  
  outputHandler->BeginOutput();
  
  for(int i=0; i<MAX_PROFILER_SAMPLES; ++i)
  {
    if(samples[i].isValid)
    {
      float sampleTime, percentage;
      //calculate the time spend on the sample itself (excluding children)
      sampleTime = samples[i].totalTime - samples[i].childTime;
      percentage = ( sampleTime / ( rootEnd - rootBegin ) ) * 100.0f;
      
      //add it to the sample's values
      float totalPc;
      totalPc = samples[i].averagePc*samples[i].dataCount;
      totalPc += percentage; samples[i].dataCount++;
      samples[i].averagePc = totalPc/samples[i].dataCount;
      
      if((samples[i].minPc == -1) || (percentage<samples[i].minPc))
        samples[i].minPc = percentage;
      
      if((samples[i].maxPc == -1)||(percentage>samples[i].maxPc))
        samples[i].maxPc = percentage;
      
      //output these values
      outputHandler->Sample(samples[i].minPc,
                            samples[i].averagePc,
                            samples[i].maxPc,
                            samples[i].callCount,
                            samples[i].name,
                            samples[i].parentCount);
      
      //reset the sample for next time
      samples[i].callCount=0;
      samples[i].totalTime=0;
      samples[i].childTime=0;
    }
  }
  
  outputHandler->EndOutput();
}

void NE::ProfileSample::ResetSample( std::string sampleName )
{
  DEFINE_THIS_FILE;   // Required to use ASSERTions
  
  int storeIndex = -1;
  
  for(int i=0; i<MAX_PROFILER_SAMPLES; ++i)
  {
    if(samples[i].isValid && samples[i].name == sampleName)
    {
      storeIndex = i;
    }
  }
  
  ASSERT(storeIndex != -1 && "No profiler sample found to reset.");
  
  samples[storeIndex].isValid = false;
}
void NE::ProfileSample::ResetSample( unsigned int sampleIndex )
{
  samples[sampleIndex].isValid = false;
}

void NE::ProfileSample::ResetAll()
{
  for(int i=0; i<MAX_PROFILER_SAMPLES; ++i)
  {
    samples[i].isValid = false;
  }
}
