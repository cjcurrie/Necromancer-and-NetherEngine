#ifndef NEInc_NEAssert_h
#include "NEAssert.h"
#endif

namespace NE
{
  // Assertion error handler
  static void onAssert__(char const *file, unsigned line)
  {
    
    //throw std::runtime_error("This assertion failed");
    std::cout << "ERROR: assertion in file " << file << " failed on line " << line << std::endl;
  }
}