#include "pch.h"
#include "./TUtcToLocalWindowsDateTimeConverter.hpp"
#include "./TTimeHelpers.hpp"

namespace Quant::Time
{
#if(defined(_MSC_VER))
   ::std::int64_t TUtcToLocalWindowsDateTimeConverter::Convert
      ( ::std::int64_t utcDateTimeInTicks
      )
   {
      //auto utcDateTimeSliceIdNumber
      //   ( static_cast<::std::int64_t>
      //         ( static_cast<::std::uint64_t>(utcDateTimeInTicks) /
      //            static_cast<::std::uint64_t>(15 * (TTimeHelpers::NumberNanoSecondsPerMinute / TTimeHelpers::NumberNanoSecondsPerWindowsDateTimeTick))
      //         )
      //   );
      ::std::int64_t localDateTimeInTicks;

      //if(utcDateTimeSliceIdNumber == UtcDateTimeSliceIdNumber_)
      if( static_cast<::std::uint64_t>(utcDateTimeInTicks - UtcDateTimeSliceBeginInTicks_) <
           static_cast<::std::uint64_t>(15 * (TTimeHelpers::NumberNanoSecondsPerMinute / TTimeHelpers::NumberNanoSecondsPerWindowsDateTimeTick))
        )
      {
         localDateTimeInTicks = utcDateTimeInTicks + LocalTimeZoneOffsetInTicks_;
      }
      else
      {
         //UtcDateTimeSliceIdNumber_ = utcDateTimeSliceIdNumber;
         UtcDateTimeSliceBeginInTicks_ =
            utcDateTimeInTicks -
            static_cast<::std::int64_t>
               ( static_cast<::std::uint64_t>(utcDateTimeInTicks) %
                  static_cast<::std::uint64_t>(15 * (TTimeHelpers::NumberNanoSecondsPerMinute / TTimeHelpers::NumberNanoSecondsPerWindowsDateTimeTick))
               );

         {
            // Comment201905083 applies.
            //::BOOL fileTimeToLocalFileTimeOutcomeCode
               ( ::FileTimeToLocalFileTime
                     ( reinterpret_cast<::FILETIME const *>( & utcDateTimeInTicks ),
                        reinterpret_cast<::LPFILETIME>( & localDateTimeInTicks )
                     )
               );

            //if(fileTimeToLocalFileTimeOutcomeCode != FALSE)
            //{
            //}
            //else
            //{
            //   throw ::std::runtime_error
            //      //("Error; {::FileTimeToLocalFileTime} failed");
            //      ("ERROR | {::FileTimeToLocalFileTime} failed");
            //}
         }

         LocalTimeZoneOffsetInTicks_ = localDateTimeInTicks - utcDateTimeInTicks;
      }
         
      return localDateTimeInTicks;
   }
#endif
}
