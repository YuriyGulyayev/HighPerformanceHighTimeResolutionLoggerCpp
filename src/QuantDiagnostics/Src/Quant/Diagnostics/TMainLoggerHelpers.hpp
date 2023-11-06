#pragma once

#include "./TLoggerHelpers.hpp"

namespace Quant::Diagnostics
{
   class TMainLoggerHelpers
#if(defined(_MSC_VER))
      abstract
#endif
      final
   {
      public: static inline TLogger MainLogger;

      public: static void Log
         ( ::std::string const & data
         )
      {
         TLoggerHelpers::Log(MainLogger, data);
      }

      public: static void Log
         ( char const * data
         )
      {
         TLoggerHelpers::Log(MainLogger, data);
      }

      public: static void Log
         ( char const * data,
            ::std::intptr_t dataLength
         )
      {
         TLoggerHelpers::Log(MainLogger, data, dataLength);
      }

      public: static void TryLog
         ( ::std::intptr_t logFileBufferRemainingCapacityMinLimit,
            char const * formatString,
            ...
         )
      {
         ::std::va_list argPtr;
         va_start(argPtr, formatString);
         TLoggerHelpers::TryLog(MainLogger, logFileBufferRemainingCapacityMinLimit, formatString, argPtr);
         va_end(argPtr);
      }

      public: static void TryLog
         ( ::std::intptr_t logFileBufferRemainingCapacityMinLimit,
            char const * formatString,
            ::std::va_list argPtr
         )
      {
         TLoggerHelpers::TryLog(MainLogger, logFileBufferRemainingCapacityMinLimit, formatString, argPtr);
      }

      public: static void TryLogFragment
         ( ::std::intptr_t logFileBufferRemainingCapacityMinLimit,
            char const * formatString,
            ...
         )
      {
         ::std::va_list argPtr;
         va_start(argPtr, formatString);
         TLoggerHelpers::TryLogFragment(MainLogger, logFileBufferRemainingCapacityMinLimit, formatString, argPtr);
         va_end(argPtr);
      }

      public: static void TryLogFragment
         ( ::std::intptr_t logFileBufferRemainingCapacityMinLimit,
            char const * formatString,
            ::std::va_list argPtr
         )
      {
         TLoggerHelpers::TryLogFragment(MainLogger, logFileBufferRemainingCapacityMinLimit, formatString, argPtr);
      }

      public: static void BeginLog()
      {
         TLoggerHelpers::BeginLog(MainLogger);
      }

      public: static void BeginLog
         ( ::std::int64_t dateTimeStamp
         )
      {
         TLoggerHelpers::BeginLog(MainLogger, dateTimeStamp);
      }

      public: static void EndLog()
      {
         TLoggerHelpers::EndLog(MainLogger);
      }

      public: static void LogCurrentLocalDateTime()
      {
         TLoggerHelpers::LogCurrentLocalDateTime(MainLogger);
      }

      public: static void LogDateTime
         ( ::std::int64_t dateTime
         )
      {
         TLoggerHelpers::LogDateTime(MainLogger, dateTime);
      }

      // Comment201905118 applies.
      public: static void TryAdvanceLogFileBufferPositionBySNPrintFReturnValue
         ( ::std::intptr_t logFileBufferRemainingCapacityMinLimit,
            int sNPrintFReturnValue
         )
      {
         TLoggerHelpers::TryAdvanceLogFileBufferPositionBySNPrintFReturnValue
            (MainLogger, logFileBufferRemainingCapacityMinLimit, sNPrintFReturnValue);
      }
   };
}
