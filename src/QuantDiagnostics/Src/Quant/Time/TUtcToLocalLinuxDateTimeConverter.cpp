#include "pch.h"
#include "./TUtcToLocalLinuxDateTimeConverter.hpp"
#include "./TTimeHelpers.hpp"

namespace Quant::Time
{
#if( ! defined(_MSC_VER) )
   ::std::time_t TUtcToLocalLinuxDateTimeConverter::Convert
      ( ::std::time_t utcDateTimeInSeconds
      )
   {
      if( static_cast<::std::uint64_t>(utcDateTimeInSeconds - UtcDateTimeSliceBeginInSeconds_) <
           static_cast<::std::uint64_t>(15 * TTimeHelpers::NumberSecondsPerMinute)
        )
      {
      }
      else
      {
         UtcDateTimeSliceBeginInSeconds_ =
            utcDateTimeInSeconds -
            static_cast<::std::int64_t>
               ( static_cast<::std::uint64_t>(utcDateTimeInSeconds) %
                  static_cast<::std::uint64_t>(15 * TTimeHelpers::NumberSecondsPerMinute)
               );

         // Comment201905092 applies.
         ::std::tm tm1;

         // Comment201905083 applies.
         ::localtime_r(( & utcDateTimeInSeconds ), ( & tm1 ));

         LocalTimeZoneOffsetInSeconds_ = tm1.tm_gmtoff;
      }
         
      ::std::time_t localDateTimeInSeconds(utcDateTimeInSeconds + LocalTimeZoneOffsetInSeconds_);
      return localDateTimeInSeconds;
   }
#endif
}
