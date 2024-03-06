#ifndef _STANDARD_ARDUINO_
#define _STANDARD_ARDUINO_

#include <Arduino.h>

#define __STA_MAJOR__		1.0
#define __STA_MINOR__		0.1
#define __STA_PATCH__		0.1
#define __STA_REVISION__	0.0
#define __STA_VERSION__		1.0
#define __STA_MESSAGE__		"STA: V1.0"

// Architecture specific include
#define ARCH_AVR			0x0001
#define ARCH_SAM			0x0002
#define ARCH_SAMD			0x0004
#define ARCH_STM32F4		0x0008
#define ARCH_NRF52			0x000F
#define ARCH_MEGAAVR		0x001F
#define ARCH_MBED			0x003F

#if defined(ARDUINO_ARCH_AVR)
# define ARDUINO_ARCH ARCH_AVR
#elif defined(ARDUINO_ARCH_SAM)
# define ARDUINO_ARCH ARCH_SAM
#elif defined(ARDUINO_ARCH_SAMD)
# define ARDUINO_ARCH ARCH_SAMD
#elif defined(ARDUINO_ARCH_STM32F4)
# define ARDUINO_ARCH ARCH_STM32F4
#elif defined(ARDUINO_ARCH_NRF52)
# define ARDUINO_ARCH ARCH_NRF52
#elif defined(ARDUINO_ARCH_MEGAAVR)
# define ARDUINO_ARCH ARCH_MEGAAVR
#elif defined(ARDUINO_ARCH_MBED)
# define ARDUINO_ARCH ARCH_MBED
#elif defined(ARDUINO_ARCH_ESP32)
# define ARDUINO_ARCH ARCH_ESP32
#else
# error "This library only supports boards with an AVR, SAM, SAMD, NRF52, esp32 or STM32F4 processor."
#endif

#define BEGIN_NP_BLOCK namespace sta {
#define END_NP_BLOCK }

#define BEGIN_CONTROL_BLOCK BEGIN_NP_BLOCK namespace control {
#define END_CONTROL_BLOCK END_NP_BLOCK }

#ifdef GCC_HASCLASSVISIBILITY
#define _STAXXEXPORT __attribute__ ((visibility("default")))
#define _STAXXLOCAL __attribute__ ((visibility("hidden")))
#else
#define _STAXXEXPORT
#define _STAXXLOCAL
#endif

#ifdef __GCC__
#define __STAXXLIBCXX_NORETURN __attribute__ ((__noreturn__))
#else
#define __STAXXLIBCXX_NORETURN
#endif

#ifdef __GCC__
# ifndef _STAXX_NOTHROW
#  ifndef __cplusplus
#   define _STAXX_NOTHROW __attribute__((__nothrow__))
#  endif
# endif
#endif
#ifdef __cplusplus
# if __cplusplus >= 201103L
#  define _STAXX_NOEXCEPT noexcept
#  define _STAXX_USE_NOEXCEPT noexcept
#  define _STAXX_THROW(_EXCEPTION)
#  define _STAXX_ILIST
# else
#  define _STAXX_NOEXCEPT
#  define _STAXX_USE_NOEXCEPT throw()
#  define _STAXX_THROW(_EXCEPTION) throw(_EXCEPTION)
# endif
# ifndef _STAXX_NOTHROW
#  define _STAXX_NOTHROW _STAXX_USE_NOEXCEPT
# endif
#endif

BEGIN_NP_BLOCK

#include "./types/types.h"

void	pin_mode(byte pin, byte mode) _STAXX_NOEXCEPT { pinMode(pin, mode); }
void	pin_write(byte pin, bool level) _STAXX_NOEXCEPT { digitalWrite(pin, level); }
int32   pin_read(byte pin) _STAXX_NOEXCEPT { return digitalRead(pin); }

END_NP_BLOCK

// STA CORE
#include "./core/periodic.h"
#include "./core/functional.h"
#include "./core/exception.h"
#include "./core/cmath.h"
#include "./core/iterator.h"
#include "./core/memory.h"
#include "./core/containers.h"
#include "./core/coroutine.h"
#include "./core/semantics.h"

// STA COMPONENTS
#if ARDUINO_ARCH != ARCH_ESP32
#include "./components/display.h"
#endif

#include "./components/pins.h"
#include "./components/advanced_components.h"
#include "./components/basic_components.h"

// STA TYPES
#include "./types/fixed.h"
#include "./types/callback.h"

// CONTROL
#include "./control/pid.h"
#include "./control/pid_tuner.h"

// FRAMEWORK
#include "./framework/microcontroller.h"
#include "./framework/entry_point.h"
#include "./framework/event.h"
#include "./framework/http.h"

#include "log.h"
#include "utility.h"

#endif