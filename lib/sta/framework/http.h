#ifndef _STA_HTTP_H_
#define _STA_HTTP_H_

#include "sta.h"
#if ARDUINO_ARCH == ARCH_ESP32
# include <WiFi.h>
# include <HTTPClient.h>
#endif

BEGIN_NP_BLOCK

#if ARDUINO_ARCH == ARCH_ESP32

struct t_http_request_response {
public:
    int32 code;
    String payload;
};

template <typename T>
class _STAXXEXPORT request {

};

template <typename T>
class _STAXXEXPORT post_request : public request<T> {};

template <typename T>
class _STAXXEXPORT get_request : public request<T> {};

template <typename T>
class _STAXXEXPORT put_request : public request<T> {};

template <typename T>
class _STAXXEXPORT delete_request : public request<T> {};

template <typename T>
class _STAXXEXPORT patch_request : public request<T> {};

#endif

END_NP_BLOCK

#endif