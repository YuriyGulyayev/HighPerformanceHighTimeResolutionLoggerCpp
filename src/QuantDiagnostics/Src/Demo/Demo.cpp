#include "pch.h"
#include "../Quant/Diagnostics/TMainLoggerHelpers.hpp"

using namespace ::std::string_literals;

// We use this to call our thread unsafe functions from within a critical section.
// Your production app will likely have something like this.
// You might want to use {::std::lock_guard} for this, but that makes sense only if the code that throws exceptions
// guarantees no state corruption, which is not currently the case. So any exception would simply kill us.
::std::mutex MainMutEx;

// We call this on a dedicated thread. If your app has some kind of a timer that calls multiple functiona periodically
// you might want to call this fom there.
// Please review Comment201905121.
void ThreadSafeFlushMainLoggerIfNeeded()
{
   if(::Quant::Diagnostics::TMainLoggerHelpers::MainLogger.FlushIsNeeded())
   {
   }
   else
   {
      return;
   }

   ::MainMutEx.lock();

   //// Comment201905051 applies.
   //::std::int64_t timeSpan1(::Quant::Time::TTimeHelpers::GetCurrentLocalDateTime());

   ::Quant::Diagnostics::TMainLoggerHelpers::MainLogger.FlushIfNeeded();

   //// Comment201905051 applies.
   //timeSpan1 = ::Quant::Time::TTimeHelpers::GetCurrentLocalDateTime() - timeSpan1;

   ::MainMutEx.unlock();

//   // Comment201905117 applies.
//   ::std::printf
//#if(defined(_MSC_VER))
//      ( "%I64i\n",
//#else
//      ( "%li\n",
//#endif
//         timeSpan1
//      );
}

void MainLoggerFlushingThreadProc()
{
   for(;;)
   {
      // <Comment201905123>
      // Issue: This appears to make a system call to get the current system date-time, possibly even twice.
      // Issue: This will probably not work well if someone changes system date-time at a wrong moment.
      // </Comment201905123>
      ::std::this_thread::sleep_for(::std::chrono::milliseconds(500));

      ThreadSafeFlushMainLoggerIfNeeded();
   }
}

void LoggingExample5
   ( int dummy1,
      ...
   )
{
   ::std::va_list argPtr;
   va_start(argPtr, dummy1);

   // <Comment201905111>
   // Pay attention to the 1st parameter. It's how much space to reserve for the fragment that is to be formatted with the format string.
   // Issue: Is "%f" correct on both Linux and Windows? Would it be more correct to use "%lf" on at least one of the platforms?
   // </Comment201905111>
   ::Quant::Diagnostics::TMainLoggerHelpers::TryLog(50, "CRITICAL | Hello World 5 %i %f", argPtr);

   va_end(argPtr);
}

int main()
{
   #pragma region Initialization.

   // Comment201905205 applies.
   int const consoleFileDescriptor(2);

#if(defined(_MSC_VER))
   // Comment201905205 relates and/or applies.
   ::_setmode(consoleFileDescriptor, _O_BINARY);
#endif

   // It's a good practice to pass a full path here, but we can get by with a relative one.
   // I commented on that in this method comment.
   // A typical production app gets these params from a config file.
   // Note that some arguments are r-value references.
   // Comment201905122 applies.
   ::Quant::Diagnostics::TMainLoggerHelpers::MainLogger.Configure
#if(defined(_MSC_VER))
      ( "../<DateStamp>-WindowsDemo.txt"s,
#else
      ( "../<DateStamp>-LinuxDemo.txt"s,
#endif
         65536
      );

   ::Quant::Diagnostics::TMainLoggerHelpers::MainLogger.Prepare(consoleFileDescriptor);

   {
      ::std::thread mainLoggerFlushingThread( & MainLoggerFlushingThreadProc );
      mainLoggerFlushingThread.detach();
   }

   // Until this point, no other thread was running yet, so we didn't have to worry about thread synchronization.

   ::MainMutEx.lock();

   #pragma endregion
   #pragma region Logging examples.

   // We make a few logging iterations. The 1st one takes longer because we are still "cold".
   for(int counter(0); counter < 3; ++ counter)
   {
      #pragma region Logging example 1.

      {
         // <Comment201905122>
         // An inline {::std::string} constant is inefficient because a memory allocation occurs.
         // </Comment201905122>
         ::Quant::Diagnostics::TMainLoggerHelpers::Log("INFO | Hello World 1"s);

         //if(counter == 0) ::Quant::Diagnostics::TMainLoggerHelpers::MainLogger.Flush();
      }

      #pragma endregion
      #pragma region Logging example 2.

      {
         // <Comment201905021>
         // This is inefficient because this scans the string to find out its length.
         // </Comment201905021>
         ::Quant::Diagnostics::TMainLoggerHelpers::Log("NOTICE | Hello World 2");
      }

      #pragma endregion
      #pragma region Logging example 3.

      {
         char const charsToLog[] = "WARNING | Hello World 3";
         ::Quant::Diagnostics::TMainLoggerHelpers::Log(charsToLog, static_cast<::std::intptr_t>(sizeof(charsToLog) - 1U));

         // Comment201905021 applies.
         ::Quant::Diagnostics::TMainLoggerHelpers::Log
            (charsToLog, static_cast<::std::intptr_t>(::std::string::traits_type::length(charsToLog)));
      }

      #pragma endregion
      #pragma region Logging example 4.

      {
         // Comment201905111 applies.
         ::Quant::Diagnostics::TMainLoggerHelpers::TryLog(50, "ERROR | Hello World 4 %i %f", 123, 456.789);
      }

      #pragma endregion
      #pragma region Logging example 5.

      {
         ::LoggingExample5(0, 987, 654.321);
      }

      #pragma endregion
      #pragma region Logging example 6.

      // This is a low level logging example that is similar to how the above called helper functions are implemented.
      // This formats and logs a few fragments of the log record, one at a time.
      {
         ::Quant::Diagnostics::TMainLoggerHelpers::BeginLog();

         // Comment201905122 applies.
         ::Quant::Diagnostics::TMainLoggerHelpers::MainLogger.LogFragment("BUG | Hello World 6"s);
         ::Quant::Diagnostics::TMainLoggerHelpers::MainLogger.LogFragment("; More; "s);

         ::Quant::Diagnostics::TMainLoggerHelpers::MainLogger.EnsureLogFileBufferRemainingCapacity(8 + 1);

         {
            // Comment201905051 applies.
            auto number1(static_cast<unsigned int>(Quant::Time::TTimeHelpers::GetCurrentLocalDateTime()));

#if(defined(_MSC_VER))
            // Comment201905117 applies.
            auto dataLength
               ( static_cast<::std::intptr_t>
                     ( ::std::string::traits_type::length
                           (::_ultoa(number1, ::Quant::Diagnostics::TMainLoggerHelpers::MainLogger.LogFileBufferPointerAtPosition(), 16))
                     )
               );
#else
            // Comment201905117 applies.
            // This can't fail, right?
            int dataLength
               (::std::sprintf(::Quant::Diagnostics::TMainLoggerHelpers::MainLogger.LogFileBufferPointerAtPosition(), "%x", number1));
#endif

            ::Quant::Diagnostics::TMainLoggerHelpers::MainLogger.AdvanceLogFileBufferPosition(dataLength);
         }

         ::Quant::Diagnostics::TMainLoggerHelpers::TryLogFragment
            ( 40,
#if(defined(_MSC_VER))
               "; abc; %I64i; xyz",
#else
               "; abc; %li; xyz",
#endif

               // Comment201905051 applies.
               Quant::Time::TTimeHelpers::GetCurrentLocalDateTime()
            );
         ::Quant::Diagnostics::TMainLoggerHelpers::EndLog();
      }

      #pragma endregion
   }

   #pragma endregion
   #pragma region Finalization.

   ::MainMutEx.unlock();

   // Giving the other thread a chance to flush the logger.
   // Your production app should not actually sleep here.
   // Comment201905123 applies.
   ::std::this_thread::sleep_for(::std::chrono::seconds(1));

   ::MainMutEx.lock();

   // Let's flush it, just in case the other thread somehow hasn't done it.
   ::Quant::Diagnostics::TMainLoggerHelpers::MainLogger.FlushIfNeeded();

   ::MainMutEx.unlock();

   #pragma endregion
}
