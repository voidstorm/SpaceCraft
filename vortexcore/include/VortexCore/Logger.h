#pragma once
#include "Api.h"
#include "ForwardDecls.h"
#include <memory>
#include <exception>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <mutex>
#include <chrono>
#include <array>
#include <iomanip>
#include <ctime>


namespace Vt {

enum class LogLevel : unsigned {
   LOG_INFO=0,
   LOG_WARNING,
   LOG_ERROR
};

class Logger {
public:
   static const unsigned MAX_LINE_LEN = 2048;

   Logger(const std::string & logfile) {
      try {
         mLog.open(logfile, std::ofstream::out | std::ofstream::trunc);
      } catch (const std::exception&) {
         V(std::string("Failed to open log file: ") + logfile);
      }      
   }
   
   ~Logger() {
      mLog.close();
   }

   template<class... Args>
   void log(LogLevel level, const std::string &msg, Args&&... args ) {
      std::lock_guard<decltype(mMutex)> lock(mMutex);
      if (mLog.is_open()) {
         snprintf(mBuffer.data(), MAX_LINE_LEN, msg.c_str(), args...);
         {
            std::stringstream ss;
            auto time = std::chrono::system_clock::now();
            auto time_c = std::chrono::system_clock::to_time_t(time);
            struct tm newtime;
            localtime_s(&newtime, &time_c);
            ss << std::put_time(&newtime, "%F %T")
               << (level == LogLevel::LOG_INFO ? " [INFO] " : 
                   level == LogLevel::LOG_WARNING ? " [WARNING] " : " [ERROR] ")
               << mBuffer.data()
               << std::endl;
            mLog << ss.str();
         }
         mLog.flush();
      }
   }

   template<class... Args>
   void info(const std::string &msg, Args&&... args) {
      log(LogLevel::LOG_INFO, msg, args...);
   }

   template<class... Args>
   void warn(const std::string &msg, Args&&... args) {
      log(LogLevel::LOG_WARNING, msg, args...);
   }

   template<class... Args>
   void error(const std::string &msg, Args&&... args) {
      log(LogLevel::LOG_ERROR, msg, args...);
   }

private:
   std::array<char, MAX_LINE_LEN> mBuffer;
   std::recursive_mutex mMutex;
   std::ofstream mLog;
};
}
