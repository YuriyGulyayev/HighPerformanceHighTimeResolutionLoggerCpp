#include "pch.h"
#include "./TTimeHelpers.hpp"

namespace Quant::Time
{
   void TTimeHelpers::FormatSecondResolutionDateTime
      ( ::std::int64_t dateTime,
         char const * formatString,
         char * destinationBuffer,
         ::std::size_t destinationBufferCapacity
      )
   {
      auto numberSeconds
         ( static_cast<::std::time_t>
               (static_cast<::std::uint64_t>(dateTime) / static_cast<::std::uint64_t>(NumberNanoSecondsPerSecond))
         );

      // <Comment201905092>
      // This comment is referenced in the documentation.
      // Note that this supports leap seconds, so second count can be 60, at least on Linux, right?
      // So an unsophisticated parser, such as a one that uses the .NET class library, will fail to parse this date-time once in a while.
      // </Comment201905092>
      ::std::tm tm1;

      // <Comment201905083>
      // Issue: Assuming that this will succeed.
      // </Comment201905083>
#if(defined(_MSC_VER))
      ::gmtime_s( ( & tm1 ), ( & numberSeconds ) );
#else
      ::gmtime_r( ( & numberSeconds ), ( & tm1 ) );
#endif

#if( ! defined(NDEBUG) )
      ::std::size_t destinationBufferOffset
#endif
         // Comment201905117 applies.
         (::std::strftime(destinationBuffer, destinationBufferCapacity, formatString, ( & tm1 )));
      assert(destinationBufferOffset > 0U);
   }

   void TTimeHelpers::FormatHighResolutionDateTime
      ( ::std::int64_t dateTime,
         char const * formatString,
         char * destinationBuffer,
         ::std::size_t destinationBufferCapacity
      )
   {
      ::timespec timeSpec1(ConvertUnixDateTimeInNanoSecondsToTimeSpec(dateTime));

      // Comment201905092 applies.
      ::std::tm tm1;

      // Comment201905083 applies.
#if(defined(_MSC_VER))
      ::gmtime_s( ( & tm1 ), ( & timeSpec1.tv_sec ) );
#else
      ::gmtime_r( ( & timeSpec1.tv_sec ), ( & tm1 ) );
#endif

      // Comment201905117 applies.
      ::std::size_t destinationBufferOffset(::std::strftime(destinationBuffer, destinationBufferCapacity /*- 9U*/, formatString, ( & tm1 )));

      assert(destinationBufferOffset > 0U);

      // This implies max 9 fractional digits, of which we will format 7.
      // Issue: I would prefer to format 9 fractional digits.
      static_assert(NumberNanoSecondsPerSecond == 1000000000);

      assert(destinationBufferOffset + 9U <= destinationBufferCapacity);

      // Issue: Truncating rather than rounding.
      auto uint1(static_cast<unsigned int>(timeSpec1.tv_nsec) / 100U);

      // Comment201905117 applies.
      ::std::sprintf(destinationBuffer + destinationBufferOffset, ".%0*u", 7, uint1);
   }
}
