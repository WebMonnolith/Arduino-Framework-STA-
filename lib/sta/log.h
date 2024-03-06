#ifndef _STA_LOG_
#define _STA_LOG_

#include "sta.h"

BEGIN_NP_BLOCK

/*
    Logs objects to the arduino console.
*/
template <typename T> 
void logln(T t) _STAXX_NOEXCEPT {
    Serial.println(t);
}

/*
    Logs objects to the arduino console.
*/
template<typename T, typename... Args> 
void log(T t, Args... args) _STAXX_NOEXCEPT {
    Serial.print(t);
    log(args...);
}

/*
    Logs objects to the arduino console.
*/
template <typename T> 
void logsln(T t) _STAXX_NOEXCEPT {
    Serial.println(t);
}

/*
    Logs objects to the arduino console.
*/
template<typename T, typename... Args> 
void logs(T t, Args... args) _STAXX_NOEXCEPT {
    Serial.print(t);
    Serial.print(" ");

    logs(args...);
}

END_NP_BLOCK

#endif