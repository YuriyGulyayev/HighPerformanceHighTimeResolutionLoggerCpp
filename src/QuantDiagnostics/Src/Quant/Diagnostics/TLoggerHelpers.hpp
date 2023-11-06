#pragma once

#include "./TLogger.hpp"
#include "../Time/TTimeHelpers.hpp"
#include "../Text/TTextHelpers.hpp"

namespace Quant::Diagnostics
{
   class TLoggerHelpers
#if(defined(_MSC_VER))
      abstract
#endif
      final
   {
      public: static void Log
         ( TLogger & logger,
            ::std::string const & data
         )
      {
         BeginLog(logger);
         logger.LogFragment(data);
         EndLog(logger);
      }

      public: static void Log
         ( TLogger & logger,
            char const * data
         )
      {
         BeginLog(logger);
         logger.LogFragment(data);
         EndLog(logger);
      }

      public: static void Log
         ( TLogger & logger,
            char const * data,
            ::std::intptr_t dataLength
         )
      {
         BeginLog(logger);
         logger.LogFragment(data, dataLength);
         EndLog(logger);
      }

      public: static void TryLog
         ( TLogger & logger,
            ::std::intptr_t logFileBufferRemainingCapacityMinLimit,
            char const * formatString,
            ...
         )
      {
         ::std::va_list argPtr;
         va_start(argPtr, formatString);
         TryLog(logger, logFileBufferRemainingCapacityMinLimit, formatString, argPtr);
         va_end(argPtr);
      }

      public: static void TryLog
         ( TLogger & logger,
            ::std::intptr_t logFileBufferRemainingCapacityMinLimit,
            char const * formatString,
            ::std::va_list argPtr
         )
      {
         BeginLog(logger);
         TryLogFragment(logger, logFileBufferRemainingCapacityMinLimit, formatString, argPtr);
         EndLog(logger);
      }

      public: static void TryLogFragment
         ( TLogger & logger,
            ::std::intptr_t logFileBufferRemainingCapacityMinLimit,
            char const * formatString,
            ...
         )
      {
         ::std::va_list argPtr;
         va_start(argPtr, formatString);
         TryLogFragment(logger, logFileBufferRemainingCapacityMinLimit, formatString, argPtr);
         va_end(argPtr);
      }

      public: static void TryLogFragment
         ( TLogger & logger,
            ::std::intptr_t logFileBufferRemainingCapacityMinLimit,
            char const * formatString,
            ::std::va_list argPtr
         )
      {
         logger.EnsureLogFileBufferRemainingCapacity(logFileBufferRemainingCapacityMinLimit);

         // This will assert {logFileBufferRemainingCapacityMinLimit}, near Comment201905133.
         TryAdvanceLogFileBufferPositionBySNPrintFReturnValue
            ( logger,
               logFileBufferRemainingCapacityMinLimit,

               // <Comment201905117>
               // This comment is referenced in the documentation.
               // This codebase isn't currently aware of locales.
               // It assumes that no locale has been set, which implies the default invariant C++ locale.
               // If your client code sets a particular locale the behavior will not necessarily be correct.
               // Comment201905118 relates.
               // </Comment201905117>
               ::std::vsnprintf(logger.LogFileBufferPointerAtPosition(), static_cast<::std::size_t>(logFileBufferRemainingCapacityMinLimit), formatString, argPtr)
            );
      }

      public: static void BeginLog
         ( TLogger & logger
         )
      {
         BeginLog(logger, Time::TTimeHelpers::GetCurrentLocalDateTime());
      }

      public: static void BeginLog
         ( TLogger & logger,
            ::std::int64_t dateTimeStamp
         )
      {
         logger.UpdateDateTime(dateTimeStamp);
         LogDateTime(logger, dateTimeStamp);
         logger.LogFragment
            //// Issue: I would prefer this.
            //("; ", 2);

            (" | ", 3);
      }

      public: static void EndLog
         ( TLogger & logger
         )
      {
         logger.LogFragment(Text::TTextHelpers::NewLine, static_cast<::std::intptr_t>(sizeof(Text::TTextHelpers::NewLine) - 1U));
      }

      public: static void LogCurrentLocalDateTime
         ( TLogger & logger
         )
      {
         LogDateTime(logger, Time::TTimeHelpers::GetCurrentLocalDateTime());
      }

      public: static void LogDateTime
         ( TLogger & logger,
            ::std::int64_t dateTime
         )
      {
         logger.EnsureLogFileBufferRemainingCapacity(27 + 1);
         Time::TTimeHelpers::FormatHighResolutionDateTime
            ( dateTime,
               "%F %T",

               //// Issue: I would prefer this. The size of this would be 25.
               //"%Y%m%d %T",

               logger.LogFileBufferPointerAtPosition(),
               27U + 1U
            );
         logger.AdvanceLogFileBufferPosition(27);
      }

      // <Comment201905118>
      // This supports function like {snprintf} and {vsnprintf}, possibly some others similar.
      // Note that Microsoft also offers functions like {_snprintf} and {_vsnprintf}, as well as their locale aware siblings,
      // but those have a different functionality and/or return a different value, therefore this logic would not necessarily work for those.
      // Comment201905117 relates.
      // </Comment201905118>
      public: static void TryAdvanceLogFileBufferPositionBySNPrintFReturnValue
         ( TLogger & logger,
            ::std::intptr_t logFileBufferRemainingCapacityMinLimit,
            int sNPrintFReturnValue
         )
      {
         // <Comment201905133>
         // Zero would be incorrect in this case because we need space at least for '\0'.
         // </Comment201905133>
         assert(logFileBufferRemainingCapacityMinLimit > 0);

         // We expect that this can fail.
         assert(sNPrintFReturnValue >= 0);

         // We expect that this can fail, which would mean that the output has been truncated.
         assert(sNPrintFReturnValue < logFileBufferRemainingCapacityMinLimit);

         if(sNPrintFReturnValue > 0)
         {
            // This assumes a successful assertion near Comment201905133.
            logger.AdvanceLogFileBufferPosition
               (::std::min(logFileBufferRemainingCapacityMinLimit - 1, static_cast<::std::intptr_t>(sNPrintFReturnValue)));
         }
         else
         {
         }
      }
   };
}
