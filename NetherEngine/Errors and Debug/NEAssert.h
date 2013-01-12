 // Note: There is no implementation file associated with this header.

/*
  Notes:
    - The macro DEFINE_THIS_FILE must be called at the top of any .c, or .cpp that uses ASSERTions.
        Call the macro after the header guards and NEAssert inclusion statement.
        Said macro will enable onAssert to give some details regarding errors.
    - For templated classes in .h files (which, being templated, have instanced source code),
        do not DEFINE_THIS_FILE, but use
        UNCACHED_ASSERT (or UNCACHED_REQUIRE, etc.) instead. If more than one assertion is used in
        any given templated method, DEFINE_THIS_FILE inside the method and use normal ASSERTs.
        These complex instructions A) aren't really that complex and B) help to conserve ROM by
        cutting down on const static definitions.
 
    - REQUIRE, ENSURE, and INVARIANT are typedef'd ASSERTions used to design-by-contract.
    - Global.h::#define NOASSERT will deactivate ASSERT, REQUIRE, ENSURE, and INVARIANT at
        compile-time, so that any call to these macros evaluates to (void)0
    - ALLEGE, unlike all the others, will still have its arguments evaluated even when
        NOASSERT is defined. However, args == true will not be called (it's not defined).
        Having an ASSERT-family call whose arguments will be evaluated even when NOASSERT
        is valuable for any kind of check whose arguments include side effects (such
        as arg++ or ++arg).
*/

#ifndef NEInc_NEAssert_h
  #define NEInc_NEAssert_h

  #include "Global.h"   // NOASSERT is defined here

  #define MAX_PATH 512

  /* ASSERT and ALLEGE have been disabled */
  #ifdef NOASSERT

//    #define DEFINE_THIS_FILE

    // Makes these three macros useless.
    #define ASSERT(ignore_params) ((void)0)         // ASSERT's arguments and their side effects will only be evaluated when !NOASSERT
    #define ALLEGE(test_params)   ((void)(test_params))   // When NOASSERT, ALLEGE will still have its side-effects evaluated.

//    #define UNCACHED_ASSERT(ignore_params) ((void)0)
//    #define UNCACHED_ALLEGE(test_params)   ((void)(test_params))

  /* All assertions are enabled */
  #else

    #ifdef __cplusplus
      /* The extern case here makes this (c++) source compatible with c compilers. */
      extern "C" {
    #endif

          
    /* This is the global callback used when an assertion fails. */
    static void onAssert__(char const *file, unsigned line);

    #ifdef __cplusplus
     }
    #endif


//    #define DEFINE_THIS_FILE \
      static const char THIS_FILE__[] = __FILE__


    // This is the main assert macro used. Failures are sent to onAssert__()
    #define ASSERT(test_params) \
      ( (test_params)?(void)0 : onAssert__(__FILE__, __LINE__) )


    // Overloaded for templated classes that can't DEFINE_THIS_FILE outside of methods,
    //  due to being instanced and therefore not sharing scope.
    //  DEFINE_THIS_FILE is still useful if if more than one ASSERT is to be used
    //  per method. 
//    #define UNCACHED_ASSERT(test_params) \
      ( (test_params)?(void)0 : onAssert__(__FILE__, __LINE__) )    // Only difference is it doesn't use THIS_FILE__


    /*
      Note:
        All macros work the exact same was as ASSERT. REQUIRE, ENSURE, and INVARIANT are not disabled
        by NASSERT, so they can be used when the assertion creates necessary side-effects (like increment-analyze and its ilk).
    */

    // If and only if !NOASSERT, ALLEGE = ASSERT.
    #define ALLEGE(test_) ASSERT(test_)
//    #define UNCACHED_ALLEGE(test_) UNCACHED_ASSERT(test_)
  #endif

  // Here are three tools used to Design by Contract. They will always have their side-effects evaluated.
  #define REQUIRE(test_)    ASSERT(test_)    // Pre
  #define ENSURE(test_)     ASSERT(test_)    // Post
  #define INVARIANT(test_)  ASSERT(test_)    // always

//  #define UNCACHED_REQUIRE(test_)    UNCACHED_ASSERT(test_)
//  #define UNCACHED_ENSURE(test_)     UNCACHED_ASSERT(test_)
//  q#define UNCACHED_INVARIANT(test_)  UNCACHED_ASSERT(test_)

#ifndef Inc_iostream
#include <iostream>
#define Inc_iostream
#endif

/* This is the global callback used when an assertion fails. */
static void onAssert__(char const *file, unsigned line)
{
  //throw std::runtime_error("This assertion failed");
  std::cout << "ERROR: assertion in file " << file << " failed on line " << line << std::endl;
  
}

#endif
