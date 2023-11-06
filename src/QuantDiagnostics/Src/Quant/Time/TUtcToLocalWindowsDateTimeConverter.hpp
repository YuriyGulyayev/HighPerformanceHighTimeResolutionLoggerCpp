#pragma once

namespace Quant::Time
{
#if(defined(_MSC_VER))
   // <Comment201905072>
   // This assumes that time zone offset is guaranteed to remain unchanged on any 15-minute slice.
   // </Comment201905072>
   class TUtcToLocalWindowsDateTimeConverter //final
   {
      //private: ::std::int64_t UtcDateTimeSliceIdNumber_;
      private: ::std::int64_t UtcDateTimeSliceBeginInTicks_;
      private: ::std::int64_t LocalTimeZoneOffsetInTicks_;

      public: /*explicit*/ TUtcToLocalWindowsDateTimeConverter():
         //UtcDateTimeSliceIdNumber_(::std::numeric_limits<::std::int64_t>::min())
         UtcDateTimeSliceBeginInTicks_(::std::numeric_limits<::std::int64_t>::min())
      {
      }

      TUtcToLocalWindowsDateTimeConverter( TUtcToLocalWindowsDateTimeConverter const & ) = delete;
      TUtcToLocalWindowsDateTimeConverter & operator = ( TUtcToLocalWindowsDateTimeConverter const & ) = delete;
      TUtcToLocalWindowsDateTimeConverter( TUtcToLocalWindowsDateTimeConverter && ) = delete;
      TUtcToLocalWindowsDateTimeConverter & operator = ( TUtcToLocalWindowsDateTimeConverter && ) = delete;
      ~TUtcToLocalWindowsDateTimeConverter() = default;

      // Wrong: This method can throw an {::std::runtime_error} exception.
      // Actually I commented that out.
      public: ::std::int64_t Convert
         ( ::std::int64_t utcDateTimeInTicks
         );
   };
#endif
}
