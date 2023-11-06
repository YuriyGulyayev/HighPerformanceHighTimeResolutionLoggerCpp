#pragma once

#include "./TMainUtcToLocalWindowsDateTimeConverterHelpers.hpp"
#include "./TMainUtcToLocalLinuxDateTimeConverterHelpers.hpp"

namespace Quant::Time
{
   // <Comment201905134>
   // This comment is referenced in the documentation.
   // A date-time is an {int64_t} expressed in nanoseconds since 1970-01-01.
   // Negative values are not supported.
   // So the maximum supported date-time approximately equals this expression in C#:
   // (new System.DateTime(1970, 1, 1)).AddTicks(long.MaxValue / 100).ToString(System.Globalization.DateTimeFormatInfo.InvariantInfo) ==
   // "04/11/2262 23:47:16"
   // In calculations, we rarely if ever check for overflow/underflow.
   // If a date-time arrives from an unreliable source your app must validate that it's "reasonable"
   // so that any subsequent calculations could not result in an overflow/underflow.
   // We don't validate system date-time.
   // </Comment201905134>
   class TTimeHelpers
#if(defined(_MSC_VER))
      abstract
#endif
      final
   {
      public: static ::std::int64_t const NumberSecondsPerMinute = 60;
      public: static ::std::int64_t const NumberMinutesPerHour = 60;
      public: static ::std::int64_t const NumberHoursPerDay = 24;
      public: static ::std::int64_t const NumberNanoSecondsPerSecond = 1000000000;
      public: static ::std::int64_t const NumberNanoSecondsPerMinute = NumberSecondsPerMinute * NumberNanoSecondsPerSecond;
      public: static ::std::int64_t const NumberNanoSecondsPerHour = NumberMinutesPerHour * NumberNanoSecondsPerMinute;
      public: static ::std::int64_t const NumberNanoSecondsPerDay = NumberHoursPerDay * NumberNanoSecondsPerHour;

#if(defined(_MSC_VER))
      public: static ::std::int64_t const NumberNanoSecondsPerWindowsDateTimeTick = 100;
#endif

#if(defined(_MSC_VER))
      // ((new System.DateTime(1970, 1, 1)) - (new System.DateTime(1601, 1, 1))).Ticks
      public: static ::std::int64_t const UnixDateTimeEpochWindowsDateTimeInTicks = 116444736000000000;
#endif

      // <Comment201905051>
      // This function is thread unsafe.
      // In fact, most functions are not guaranteed to be thread safe.
      // It's possible to make this thread safe by making UTC-to-local date-time converters {thread_local},
      // but would constructors of those be called correctly on each thread?
      // </Comment201905051>
      public: static ::std::int64_t GetCurrentLocalDateTime()
      {
#if(defined(_MSC_VER))
         ::std::int64_t utcWindowsDateTimeInTicks;
         ::GetSystemTimePreciseAsFileTime(reinterpret_cast<::LPFILETIME>( & utcWindowsDateTimeInTicks ));
         ::std::int64_t localWindowsDateTimeInTicks
            (TMainUtcToLocalWindowsDateTimeConverterHelpers::MainUtcToLocalWindowsDateTimeConverter.Convert(utcWindowsDateTimeInTicks));
         return ConvertWindowsDateTimeInTicksToUnixDateTimeInNanoSeconds(localWindowsDateTimeInTicks);
#else
         ::timespec timeSpec1;
         ::clock_gettime(CLOCK_REALTIME, ( & timeSpec1 ));
         return
            static_cast<::std::int64_t>
               ( static_cast<::std::uint64_t>(TMainUtcToLocalLinuxDateTimeConverterHelpers::MainUtcToLocalLinuxDateTimeConverter.Convert(timeSpec1.tv_sec)) *
                  static_cast<::std::uint64_t>(NumberNanoSecondsPerSecond)
               ) +
            timeSpec1.tv_nsec;
#endif
      }

#if(defined(_MSC_VER))
      public: static ::std::int64_t ConvertWindowsDateTimeInTicksToUnixDateTimeInNanoSeconds
         ( ::std::int64_t windowsDateTimeInTicks
         )
      {
         return
            static_cast<::std::int64_t>
               ( static_cast<::std::uint64_t>(windowsDateTimeInTicks - UnixDateTimeEpochWindowsDateTimeInTicks) *
                  static_cast<::std::uint64_t>(NumberNanoSecondsPerWindowsDateTimeTick)
               );
      }
#endif

      public: static ::timespec ConvertUnixDateTimeInNanoSecondsToTimeSpec
         ( ::std::int64_t unixDateTimeInNanoSeconds
         )
      {
         ::timespec timeSpec1;
         timeSpec1.tv_sec =
            static_cast<::std::time_t>
               (static_cast<::std::uint64_t>(unixDateTimeInNanoSeconds) / static_cast<::std::uint64_t>(NumberNanoSecondsPerSecond));
         timeSpec1.tv_nsec =
            static_cast<long>
               (static_cast<::std::uint64_t>(unixDateTimeInNanoSeconds) % static_cast<::std::uint64_t>(NumberNanoSecondsPerSecond));
         return timeSpec1;
      }

      public: static void FormatSecondResolutionDateTime
         ( ::std::int64_t dateTime,
            char const * formatString,
            char * destinationBuffer,
            ::std::size_t destinationBufferCapacity
         );

      public: static void FormatHighResolutionDateTime
         ( ::std::int64_t dateTime,
            char const * formatString,
            char * destinationBuffer,
            ::std::size_t destinationBufferCapacity
         );
   };
}
