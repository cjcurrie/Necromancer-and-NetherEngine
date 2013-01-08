 // Note: There is no implementation file associated with this header.

/* Note that the macro DEFINE_THIS_FILE must be called at the top of any .c, .cpp, or .h that includes NEAssert.h.
    Said macro will enable onAssert to give some details.
*/

#ifndef NEInc_NEAssert_h
  #define NEInc_NEAssert_h

  /* ASSERT and ALLEGE have been disabled */
  #ifdef NOASSERT

    #define DEFINE_THIS_FILE

    // Makes these two macros useless.
    #define ASSERT(ignore_) ((void)0)
    #define ALLEGE(test_) ((void)(test_))

  /* All assertions are enabled */
  #else

    #ifdef __cplusplus
      /* The extern case here makes this (c++) source compatible with c compilers. */
      extern "C" {
    #endif

      /* This is the global callback used when an assertion fails. */
        static void onAssert__(char const *file, unsigned line);
      
        // @TODO: implement some output of the error
      

    #ifdef __cplusplus
     }
    #endif


    #define DEFINE_THIS_FILE  constexpr static char const THIS_FILE__[] = __FILE__


    // This is the main assert macro used. Failures are sent to onAssert__()
    #define ASSERT(test_) \
      ( (test_)?(void)0 : onAssert__(THIS_FILE__, __LINE__))


    /*
      Note: NE isn't yet using ALLEGE, REQUIRE, ENSURE, or INVARIANT, but it may in the future.
        All macros work the exact same was as ASSERT. REQUIRE, ENSURE, and INVARIANT are not disabled
        by NASSERT, so they can be used when the assertion creates necessary side-effects (like increment-analyze and its ilk).
    */

    // For now, ALLEGE is a simple alternative ASSERT
    #define ALLEGE(test_) ASSERT(test_)
  #endif

  #define REQUIRE(test_) ASSERT(test_)
  #define ENSURE(test_) ASSERT(test_)
  #define INVARIANT(test_) ASSERT(test_)


  
  #include <iostream>

  namespace NE
  {
    // Assertion error handler
    static void onAssert__(char const *file, unsigned line)
    {
      
      //throw std::runtime_error("This assertion failed");
      std::cout << "ERROR: assertion in file " << file << " failed on line " << line << std::endl;
      
      
    }
  }

#endif