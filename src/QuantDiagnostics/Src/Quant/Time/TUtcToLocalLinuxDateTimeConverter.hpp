#pragma once

namespace Quant::Time
{
#if( ! defined(_MSC_VER) )
   // Comment201905072 applies.
   class TUtcToLocalLinuxDateTimeConverter //final
   {
      private: ::std::time_t UtcDateTimeSliceBeginInSeconds_;
      private: /*int*/ ::std::time_t LocalTimeZoneOffsetInSeconds_;

      public: /*explicit*/ TUtcToLocalLinuxDateTimeConverter():
         UtcDateTimeSliceBeginInSeconds_(::std::numeric_limits<::std::time_t>::min())
      {
      }

      TUtcToLocalLinuxDateTimeConverter( TUtcToLocalLinuxDateTimeConverter const & ) = delete;
      TUtcToLocalLinuxDateTimeConverter & operator = ( TUtcToLocalLinuxDateTimeConverter const & ) = delete;
      TUtcToLocalLinuxDateTimeConverter( TUtcToLocalLinuxDateTimeConverter && ) = delete;
      TUtcToLocalLinuxDateTimeConverter & operator = ( TUtcToLocalLinuxDateTimeConverter && ) = delete;
      ~TUtcToLocalLinuxDateTimeConverter() = default;

      public: ::std::time_t Convert
         ( ::std::time_t utcDateTimeInSeconds
         );
   };
#endif
}
