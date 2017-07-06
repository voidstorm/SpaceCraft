#pragma once
#include <stdexcept>

#define LOGGING_ON


#ifdef _DEBUG
#include <iostream>
#ifndef V
#define V(x) std::cout << x << " in file: "<< __FILE__ " at line: " << __LINE__ << std::endl
#endif
#else
#define V(x) 
#endif

 


#ifdef _WIN32
#pragma warning( disable : 4251 )

//--------------------------------------DEBUG SETTINGS---------------------------------

#ifdef _DEBUG
//MEMCHECK MACRO
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#define VT_DO_LEAK_CHECK _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)


//VERBOSE
#include <iostream>
#include <string>
//#define V(x) std::cout << x << " in file: "<< __FILE__ " at line: " << __LINE__ << std::endl



#define VT_EXCEPT(x, msg) throw x(std::string("Exception in file: ") + std::string(__FILE__) + std::string(" in line: ") + std::to_string(__LINE__) + std::string(" -> ") + std::string(msg) + std::string("\n"))
#define VT_EXCEPT_RT(x, msg, e) throw x(std::string("Exception in file: ") + std::string(__FILE__) + std::string(" in line: ") + std::to_string(__LINE__) + std::string(" -> ") + std::string(msg) + std::string("\n") + e.what())


#define VK_CHECK_RESULT(x) \
{ \
   auto result = (x); \
      if (result != VkResult::VK_SUCCESS) { \
         SystemLogger::get().error("Failed to create instance with error: %s", VkErrorHelper::vkResultToStr(result).c_str()); \
      } \
} 

#ifdef LOGGING_ON
#define SYSTEM_LOG_INFO(x, ...) SystemLogger::get().info(x, ##__VA_ARGS__)
#define SYSTEM_LOG_WARN(x, ...) SystemLogger::get().warn(x, ##__VA_ARGS__)
#define SYSTEM_LOG_ERROR(x, ...) SystemLogger::get().error(x, ##__VA_ARGS__)
#else 
#define SYSTEM_LOG_INFO(x, ...)
#define SYSTEM_LOG_WARN(x, ...)
#define SYSTEM_LOG_ERROR(x, ...)
#endif

//D3D DEBUG MACROS
#define D3D_DEBUG

#else
//--------------------------------------RELEASE SETTINGS---------------------------------
#define VT_EXCEPT(x, msg) throw x(std::string("Exception in file: ") + std::string(__FILE__) + std::string(" in line: ") + std::to_string(__LINE__) + std::string(" -> ") + std::string(msg) + std::string("\n"))
#define VT_EXCEPT_RT(x, msg, e) throw x(std::string("Exception in file: ") + std::string(__FILE__) + std::string(" in line: ") + std::to_string(__LINE__) + std::string(" -> ") + std::string(msg) + std::string("\n") + e.what())


#define VK_CHECK_RESULT(x) \
{ \
   auto result = (x); \
      if (result != VkResult::VK_SUCCESS) { \
         SystemLogger::get().error("Failed to create instance with error: %s", VkErrorHelper::vkResultToStr(result).c_str()); \
      } \
} 

#ifdef LOGGING_ON
#define SYSTEM_LOG_INFO(x, ...) SystemLogger::get().info(x, ##__VA_ARGS__)
#define SYSTEM_LOG_WARN(x, ...) SystemLogger::get().warn(x, ##__VA_ARGS__)
#define SYSTEM_LOG_ERROR(x, ...) SystemLogger::get().error(x, ##__VA_ARGS__)
#else 
#define SYSTEM_LOG_INFO(x, ...)
#define SYSTEM_LOG_WARN(x, ...)
#define SYSTEM_LOG_ERROR(x, ...)
#endif

#define VT_MEM_CHECK {}
#define V(x) 
#define VT_DO_LEAK_CHECK {}
#endif
#endif

#ifdef VORTEXCORE_EXPORTS
#define VORTEX_API __declspec(dllexport)
#else
#define VORTEX_API __declspec(dllimport)
#endif


#ifndef VT_PACK32
#define VT_PACK32(h16, l16)	 (unsigned long)(((unsigned long)(h16) << 16) | (unsigned long)(l16)) 
#endif

#ifndef VT_LOWORD
#define VT_LOWORD(l)         ((unsigned short)((unsigned long)(l) & 0xffff))
#endif

#ifndef VT_HIWORD
#define VT_HIWORD(l)         ((unsigned short)((unsigned long)(l) >> 16))
#endif

#ifndef VT_PACK_XY32
#define VT_PACK_XY32 VT_PACK32
#endif

#ifndef VT_GET_X32
#define VT_GET_X32(lp)  ((int)(short)VT_HIWORD(lp))
#endif

#ifndef VT_GET_Y32
#define VT_GET_Y32(lp)  ((int)(short)VT_LOWORD(lp))
#endif

#define RAD2DEG( radian ) ((radian) * (57.29577951f))
#define DEG2RAD( degree ) ((degree) * (0.017453293f))
#define PI 3.1415926535897932384626433832795
#define PI2 6.283185307179586476925286766559



#define VT_TIMING