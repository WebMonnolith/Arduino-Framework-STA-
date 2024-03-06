/*
  Coroutines.h - Library providing a simple coroutine system.
  Created by Renaud B�dard with code review help by Bryan McConkey and zerozshadow, July 18th, 2014.
  Released into the public domain.
  Version 1.1

  Edited by Jessy van Polanen, September 24th, 2023
  Version 1.2

  The variant of coroutines proposed in this library are inspired by Unity coroutines
  http://docs.unity3d.com/ScriptReference/Coroutine.html

  The basic idea is to be able to create blocks of code that execute sequentially,
  but can choose to stop temporarily and resume later. This is similar to threads,
  but in the case of coroutines, they never get pre-empted and will only give away
  focus if they explicitely yield.

  The library provides a coroutine manager that pre-allocates and updates coroutines,
  as well as provides wait, suspend and resume constructs to make their usage more
  convenient.

  A simple coroutine is declared like this :

    // flashes a LED attached to analog pin 5 for 100ms
    void flashOnce(COROUTINE_CONTEXT(coroutine))
    {
        BEGIN_COROUTINE;

        analogWrite(5, 255);

        coroutine.wait(100);
        COROUTINE_YIELD;

        analogWrite(5, 0);

        END_COROUTINE;
    }

  Here, the "wait" call adds 100ms to the timer that will prevent the coroutine from
  resuming on the next update. The COROUTINE_YIELD macro exits the function, and
  records the state of the coroutine so it can be resumed later. The BEGIN and END
  macros do the rest required for this to work.

  The COROUTINE_CONTEXT() macro defines the name of the context argument to the
  coroutine, which has the type Coroutine& (a reference type). You may not use a
  regular parameter definition, since the coroutine needs to know the name you choose
  for it, and using this macro was the most straightforward way.

  You may also use "coroutine locals", which are variables local to the coroutine
  and whose state will be preserved after a yield and recovered when resuming :

    void flashThrice(COROUTINE_CONTEXT(coroutine))
    {
        COROUTINE_LOCAL(int, i);

        BEGIN_COROUTINE;

        for (i = 0; i < 3; i++)
        {
            analogWrite(5, 255);

            coroutine.wait(100);
            COROUTINE_YIELD;

            analogWrite(5, 0);

            coroutine.wait(50);
            COROUTINE_YIELD;
        }

        END_COROUTINE;
    }

  Notice that the for(;;) loop does not declare "i" since it already has been.
  However, its value is undefined, like any other variable, until it's first set.
  Since it's declared as COROUTINE_LOCAL, after returning from the YIELD, its
  value will be restored to what it was prior to yielding.
  COROUTINE_LOCAL declarations must be done before BEGIN_COROUTINE.
  Coroutine locals live on the heap, so their size must be kept in check. Also,
  there is a maximum amount of them which defaults to 8, but can be tweaked in
  this header. (see Coroutine::MaxLocals)

  Coroutines may also loop instead of evaluate once, using the loop() function :

    void flashForever(COROUTINE_CONTEXT(coroutine))
    {
        BEGIN_COROUTINE;

        analogWrite(5, 255);

        coroutine.wait(100);
        COROUTINE_YIELD;

        analogWrite(5, 0);

        coroutine.wait(50);
        COROUTINE_YIELD;

        coroutine.loop();

        END_COROUTINE;
    }

  If the loop() function is not called in one of its iterations, the loop stops and
  the coroutine will end its execution normally.

  There are some preconditions that the sketch must meet to use coroutines :
  1. Declare a Coroutines<N> object, where N is the number of preallocated coroutines
     required; in other words, the number of coroutines you expect your program to
     "concurrently" run.
  2. In your loop() function, call the update() function on that Coroutines<N> object.

  Declared coroutines will not be started automatically. The sketch needs to start
  them with a function call :

    // where "coroutines" is a Coroutine<N> instance,
    // and "flashOnce" is the name of a declared coroutine
    coroutines.start(flashOnce);

  This fires the coroutine, which will begin in the next update.
  The return type of the function must be void, and it must be defined with the
  COROUTINE_CONTEXT() macro as only parameter.

  You can keep a reference to the coroutine object via the return value of "start",
  but since these objects are recycled, one must be careful to only use the reference
  while the coroutine it initially referred to is still alive.
  One way to do this would be to declare the coroutine reference as a pointer in the
  sketch's file-scope variables, and set it to NULL right before COROUTINE_END.

  If the sketch holds a reference or a pointer to a coroutine object, it can manipulate
  its execution from the outside using these functions :

  - suspend() will prevent any subsequent update to the coroutine
  - resume() reverts a suspended coroutine and allows it to execute in the next update
  - terminate() makes the coroutine prematurely exit in the next update

  The suspend() function may also be called from within a coroutine, which blocks
  its execution until resume() is called on it from the sketch.

  To let a coroutine clean up after an external termination, you can use the
  COROUTINE_FINALLY macro like this :

    void finallyExample(COROUTINE_CONTEXT(coroutine))
    {
        BEGIN_COROUTINE;

        coroutine.wait(1000);
        COROUTINE_YIELD;

        Serial.println("Waited 1000ms");

        COROUTINE_FINALLY
        {
            Serial.println("Exiting...");
        }

        END_COROUTINE;
    }

  In this example, the "Exiting..." string will be printed whether the coroutine
  is externally terminated or if it finished execution normally, after waiting 1000ms.
  The "Waited 1000ms" string however will only be printed if the coroutine stays
  alive for more than 1000ms.
  If used, the COROUTINE_FINALLY block must be placed before END_COROUTINE.

  There is currently no way to return something from a coroutine or to pass a parameter
  to a coroutine. However, they have access to the sketch's file-scope variables,
  which can be used for input and/or output.

  The library comes with debug-logging ability, which can be enabled by defining
  three macros :

  - trace(...)
  - assert(condition, ...)
  - P(string_literal)

  See their default definition below for how they need to be implemented.

  This coroutine implementation is based on Simon Tatham's
  http://www.chiark.greenend.org.uk/~sgtatham/coroutines.html

  VERSION LOG
  ===========
  1.0 (2014-07-18)
  - Initial release

  1.1 (2015-08-31)
  - Fixes lock when using more than 3 coroutines
  - Switched to single header to prevent include order issues
  - Free allocated locals as soon as the coroutine terminates
  - Fixed erroneous debugging define documentation
  - Fixed error when declaring more than one coroutine local (thanks stuntgoat!)

  1.2 (2023-05-13)
  - Fixed some bugs withing coroutine
  - Compatibility change with sta
*/

#if ARDUINO_ARCH != ARCH_ESP32

#ifndef COROUTINES_H
#define COROUTINES_H

// The Arduino header is primarily required for use of the millis() function
#include "Arduino.h"

#define COROUTINES_VERSION 1.2

#include "sta.h"

BEGIN_NP_BLOCK

// Debugging macros, null operations unless defined prior to including this .h
// trace should be : printf(__VA_ARGS__) 
// or : printf_P(__VA_ARGS__) // if P is defined
#ifndef trace
#define trace(...)
#endif
// assert should be : while(!cond) { trace(__VA_ARGS__); }
#ifndef assert
#define assert(cond, ...)
#endif
// P should be : PSTR(string_literal "\n") // if you used printf_P in trace
#ifndef P
#define P(string_literal)
#endif

// See documentation at top of file for usage of the COROUTINE macros

#define COROUTINE_CONTEXT(coroutine)                            \
coroutine_interface& coroutine)                                           \
{                                                               \
    byte COROUTINE_localIndex = 0;                              \
    (void) COROUTINE_localIndex;                                \
    coroutine_impl& COROUTINE_ctx = (coroutine_impl&) coroutine;  \
    (void) coroutine;                                           \
    if (true

#define COROUTINE_LOCAL(type, name)                                                        \
    if (COROUTINE_ctx.jumpLocation == 0 && !COROUTINE_ctx.looping)                         \
    {                                                                                      \
        assert(COROUTINE_ctx.numSavedLocals >= CoroutineImpl::MaxLocals,                   \
               P("Ran out of coroutine locals! Increase Coroutine::MaxLocals"));           \
        trace(P("Allocating local '" #name "' (#%hhu)"), COROUTINE_ctx.numSavedLocals);    \
        COROUTINE_localIndex = COROUTINE_ctx.numSavedLocals;                               \
        COROUTINE_ctx.savedLocals[COROUTINE_ctx.numSavedLocals++] = malloc(sizeof(type));  \
    }                                                                                      \
    else                                                                                   \
        COROUTINE_localIndex = COROUTINE_ctx.numRecoveredLocals++;                         \
    type& name = *((type*) COROUTINE_ctx.savedLocals[COROUTINE_localIndex]);

#define BEGIN_COROUTINE                                             \
    trace(P("Entering coroutine #%hhu ('%s') at %lu ms"),           \
          COROUTINE_ctx.id, __func__, COROUTINE_ctx.sinceStarted);  \
    COROUTINE_ctx.looping = false;                                  \
    switch (COROUTINE_ctx.jumpLocation)                             \
    {                                                               \
    case 0:										

#define COROUTINE_YIELD                         \
        COROUTINE_ctx.jumpLocation = __LINE__;  \
        COROUTINE_ctx.numRecoveredLocals = 0;   \
        trace(P("...yielding..."));             \
        return;                                 \
    case __LINE__:	

#define COROUTINE_FINALLY           \
    case -1:                        \
        if (COROUTINE_ctx.looping)  \
            break;				

#define END_COROUTINE                                   \
    default:                                            \
        {}                                              \
    }                                                   \
    COROUTINE_ctx.terminated = !COROUTINE_ctx.looping;  \
    return;                                             \
}

// Coroutine context object
// Provides functions for manipulating the execution of coroutines from within
// a coroutine or from the sketch.
class _STAXXEXPORT coroutine_interface
{
public:
    // Sets the time in milliseconds to wait before the coroutine can come back from a yield
    virtual void wait(unsigned long millis) = 0;
    // Stops the coroutine on its next update
    virtual void terminate() = 0;
    // Suspends the coroutine indefinitely starting from the next update, pausing its execution
    virtual void suspend() = 0;
    // Resumes a suspended coroutine, allowing it to update and continue executing
    virtual void resume() = 0;
    // Makes the coroutine loop back to the beginning instead of terminating when reaching END_COROUTINE
    virtual void loop() = 0;

    // returns true if the coroutine is terminated (false if it is active)
    virtual bool isTerminated() const = 0;
    // returns true if the coroutine is suspended
    virtual bool isSuspended() const = 0;
};

// Delegate type of coroutine functions
typedef void (*coroutineBody)(coroutine_interface&);

// Internal class for coroutines, which implements the public abstract one
class _STAXXEXPORT coroutine_impl : public coroutine_interface
{
public:
    // Maximum number of coroutine locals, increase/decrease if needed
    const static byte MaxLocals = 8;

    coroutineBody function;
    unsigned long barrierTime, sinceStarted, startedAt, suspendedAt;
    byte id;
    bool terminated, suspended, looping;
    long jumpLocation;
    // Coroutine locals are heap-allocated on demand and freed on termination
    void* savedLocals[MaxLocals];
    byte numSavedLocals, numRecoveredLocals;

public:
    // Resets the coroutine's state, used when recycling coroutine objects
    void reset() {
        this->barrierTime = 0;
        this->sinceStarted = 0;
        this->jumpLocation = 0;
        this->terminated = suspended = false;
        this->function = NULL;
        this->numSavedLocals = 0;
        this->numRecoveredLocals = 0;
        this->terminated = false;
        this->suspended = false;
        this->looping = false;
    }

    // frees coroutine locals when it's terminated
    void freeLocals() {
        if (this->numSavedLocals == 0)
            return;

        trace(P("Freeing %hhu local(s)"), this->numSavedLocals);
        for (int i = 0; i < this->numSavedLocals; i++)
            free(this->savedLocals[i]);
        this->numSavedLocals = 0;
    }

    bool update(unsigned long millis) {
        if (this->suspended)
            return false;

        if (this->barrierTime <= millis)
        {
            this->sinceStarted = this->startedAt > millis ? 0 : millis - this->startedAt;
            this->function(*this);
            return terminated;
        }

        return false;
    }

public:
    void wait(unsigned long time) override {
        this->barrierTime = millis() + time;
    }

    void terminate() override {
        this->terminated = true;
        this->suspended = false;
        this->looping = false;
        this->jumpLocation = -1;
        this->barrierTime = 0;
    }

    void suspend() override {
        if (!this->suspended && !this->terminated)
        {
            this->suspended = true;
            this->suspendedAt = millis();
        }
    }

    void resume() override {
        if (this->suspended && !this->terminated)
        {
            this->suspended = false;
            this->startedAt += millis() - this->suspendedAt;
        }
    }    
    
    void loop() override {
        this->jumpLocation = 0;
        this->numRecoveredLocals = 0;
        this->looping = true;
        trace(P("...looping..."));
    }

    inline bool isTerminated() const override {
        return this->terminated;
    }

    inline bool isSuspended() const override {
        return this->suspended;
    }
};

// Coroutines manager class
// The N template argument determines how many coroutines are allocated, which is to say
// how many coroutines can be active at once. Since an 32-bit mask is used to track active
// coroutines, the maximum value for N is 32, not 255.
template <byte N>
class _STAXXEXPORT coroutines {
private:
    // The coroutine context objects
    coroutine_impl coroutines[N];
    // bitmask tracking the number of active coroutines
    unsigned long activeMask;
    // The count of active coroutines
    byte activeCount;

public:
    coroutines() 
        : activeMask(0),
        activeCount(0)
    {
        // ids are assigned sequentially and never change
        for (byte i = 0; i < N; i++)
            this->coroutines[i].id = i;
    }

    // Starts a coroutine
    // The function parameter is the name of the coroutine's function.
    // The coroutine's context object is returned by reference so it can be manipulated from the sketch.
    coroutine_impl& start(coroutineBody function) {
        for (byte i = 0; i < min(N, sizeof(unsigned long) * 8); i++)
            // take the first inactive slot
            if (!bitRead(activeMask, i))
            {
                // mark as active
                bitSet(activeMask, i);
                activeCount++;

                trace(P("Adding coroutine #%hhu"), i);
                coroutine_impl& coroutine = coroutines[i];
                // reset state of the context object on start
                coroutine.reset();
                coroutine.function = function;
                // remember the time it starts at
                coroutine.startedAt = millis();

                return coroutine;
            }

#if ARDUINO_ARCH != ARCH_ESP32
        assert(false, P("Out of allocated coroutines!"));
#endif
        abort(); // to avoid compile warning
    }

    // Updates the active coroutines.
    // Use this overload if you already have called millis() in your loop function and kept the value.
    void update(unsigned long millis) {
        int b = 0;
        int removed = 0;
        for (int i = 0; i < activeCount; i++)
        {
            while (!bitRead(activeMask, b))
            {
                b++;
                if (b == N) b = 0;
            }

#if ARDUINO_ARCH != ARCH_ESP32
            assert(b >= N, P("Couldn't find active coroutine!"));
#endif

            coroutine_impl& coroutine = coroutines[b];
            bool result = coroutine.update(millis);
            if (result)
            {
                // remove coroutine
                coroutine.freeLocals();
                trace(P("Removing coroutine #%hhu"), b);
                bitClear(activeMask, b);
                coroutine.terminated = true;
                removed++;
            }

            b++;
        }

        activeCount -= removed;
    }

    // Updates the active coroutines.
    // This overload will call millis() by itself.
    void update() {
        update(millis());
    }
};

END_NP_BLOCK

#endif
#endif