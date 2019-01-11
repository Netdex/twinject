#pragma once

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define LOG(fmt, ...) logm(__FILENAME__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define ASSERT(a) do{if(!(a)){LOG("Assertion Failed: " TOSTRING(a)); exit(1);}}while(0)

void logm(const char* file, const char* func, int line, const char* fmt, ...);