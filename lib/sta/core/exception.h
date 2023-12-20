#ifndef _STA_EXCEPTION_
#define _STA_EXCEPTION_

#include "sta.h"

#pragma GCC visibility push(default)
#pragma GCC diagnostic ignored "-Wwrite-strings"

BEGIN_NP_BLOCK

_STAXXEXPORT static char* __std_exception_what_value = "exception";

/**
 *  @brief Base class for all library exceptions.
 *
 *  This is the base class for all exceptions thrown by the standard
 *  library, and by certain language expressions.  You are free to derive
 *  your own %exception classes, or use a different hierarchy, or to
 *  throw non-class data (e.g., fundamental types).
 */
class _STAXXEXPORT exception {
public:
    exception() _STAXX_NOTHROW { }
    virtual ~exception() _STAXX_NOTHROW {}
public:
    /** Returns a C-style character string describing the general cause
     *  of the current error.  */
    virtual const char* what() const _STAXX_NOTHROW { return __std_exception_what_value; }
};

/** If an %exception is thrown which is not listed in a function's
 *  %exception specification, one of these may be thrown.  */
class _STAXXEXPORT bad_exception 
    : public exception {
public:
    bad_exception() _STAXX_USE_NOEXCEPT { }

    // This declaration is not useless:
    // http://gcc.gnu.org/onlinedocs/gcc-3.0.2/gcc_6.html#SEC118
    virtual ~bad_exception() _STAXX_USE_NOEXCEPT {}
};

/// If you write a replacement %terminate handler, it must be of this type.
typedef void (*terminate_handler) ();
/// If you write a replacement %unexpected handler, it must be of this type.
typedef void (*unexpected_handler) ();

/// Takes a new handler function as an argument, returns the old function.
terminate_handler set_terminate(terminate_handler) _STAXX_USE_NOEXCEPT;
/** The runtime will call this function if %exception handling must be
 *  abandoned for any reason.  */
void terminate() __STAXXLIBCXX_NORETURN;

/// Takes a new handler function as an argument, returns the old function.
unexpected_handler set_unexpected(unexpected_handler) _STAXX_USE_NOEXCEPT;
/** The runtime will call this function if an %exception is thrown which
 *  violates the function's %exception specification.  */
void unexpected() __STAXXLIBCXX_NORETURN;

/** [18.6.4]/1:  "Returns true after completing evaluation of a
 *  throw-expression until either completing initialization of the
 *  exception-declaration in the matching handler or entering @c unexpected()
 *  due to the throw; or after entering @c terminate() for any reason
 *  other than an explicit call to @c terminate().  [Note: This includes
 *  stack unwinding [15.2].  end note]"
 *
 *  2:  "When @c uncaught_exception() is true, throwing an %exception can
 *  result in a call of @c terminate() (15.5.1)."
 */
bool uncaught_exception() _STAXX_USE_NOEXCEPT;

namespace __gnu_cxx {
    /** A replacement for the standard terminate_handler which prints more
        information about the terminating exception (if any) on stderr.  Call
        @code
          std::set_terminate (__gnu_cxx::__verbose_terminate_handler)
        @endcode
        to use.  For more info, see
        http://gcc.gnu.org/onlinedocs/libstdc++/19_diagnostics/howto.html#4
    */
    void __verbose_terminate_handler();
} // namespace __gnu_cxx

END_NP_BLOCK

#endif