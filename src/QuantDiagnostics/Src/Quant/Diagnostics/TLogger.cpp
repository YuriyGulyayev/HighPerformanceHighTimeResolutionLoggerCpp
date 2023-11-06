#include "pch.h"
#include "./TLogger.hpp"
#include "../Time/TTimeHelpers.hpp"

namespace Quant::Diagnostics
{
   void TLogger::Configure
      ( ::std::string && logFilePathName,
         ::std::intptr_t logFileBufferCapacity
      )
   {
      assert(logFileBufferCapacity > 0);

#if(defined(_MSC_VER))
      // <Comment201905062>
      // This is needed because we are going to convert this to {unsigned int} to pass this to {::_write}.
      // </Comment201905062>
      assert(static_cast<::std::size_t>(logFileBufferCapacity) <= ::std::numeric_limits<unsigned int>::max());
#endif

      DateStampPositionInLogFilePathName_ =
         static_cast<::std::intptr_t>
            ( logFilePathName.find
                  ( DateStampSubSrtingToSubstituteInLogFilePathName_,
                     0U,
                     sizeof(DateStampSubSrtingToSubstituteInLogFilePathName_) - 1U
                  )
            );

      if(DateStampPositionInLogFilePathName_ >= 0)
      {
         // Comment201905113 applies.
         static_assert(sizeof(DateStampSubSrtingToSubstituteInLogFilePathName_) - 1U > 8U);
         logFilePathName.erase
            ( static_cast<::std::string::size_type>(DateStampPositionInLogFilePathName_ + 8),
               (sizeof(DateStampSubSrtingToSubstituteInLogFilePathName_) - 1U) - 8U
            );
      }
      else
      {
      }

      logFilePathName.shrink_to_fit();
      LogFilePathName_.assign(::std::move(logFilePathName));
      LogFileBuffer_.resize(static_cast<::std::string::size_type>(logFileBufferCapacity));
   }

   void TLogger::UpdateDateTime
      ( ::std::int64_t dateTime
      )
   {
      assert(dateTime >= 0);

      if(dateTime >= NextLogFileDate_)
      {
         // This condition will work well if someone keeps caling {UpdateDateTime} without logging anything.
         // Checking {NextLogFileDateTime_ <= 0} instead would not work in such a case.
         if(LogFileBufferPosition_ <= NextLogFileBufferBeginPosition_)

         {
            // This condition and its branch prevent the possiblity that if we start a moment before midnight,
            // some next day records are logged, and only then we are flushed those next day records would be written
            // to the starting day log file.
            // But if nobody flushes us for too long, like a day, we would still have that problem, even a few days after start, which is OK.
            // System date-time leaping back also can potentially result in a similar behavior,
            // but it would really be a questionable idea to go back to a past day log file.
            if(NextLogFileDate_ <= 0)

            {
               NextLogFileDate_ =
                  dateTime -
                  static_cast<::std::int64_t>
                     (static_cast<::std::uint64_t>(dateTime) % static_cast<::std::uint64_t>(Time::TTimeHelpers::NumberNanoSecondsPerDay)) +
                  Time::TTimeHelpers::NumberNanoSecondsPerDay;
            }
            else
            {
               NextLogFileBufferBeginPosition_ = LogFileBufferPosition_;
               NextLogFileDateTime_ = dateTime;
            }
         }
         else
         {
         }
      }
      else
      {
      }
   }

   void TLogger::LogFragment
      ( char const * data,
         ::std::intptr_t dataLength
      )
   {
      // If {dataLength} is greater than buffer capacity it could make sense to write {data} directly to the file,
      // but buffer capacity is really supposed to be bigger than any data to be appended to it.
      // In any case, keeping it simple.

      assert(dataLength >= 0);

      // This can be zero.
      ::std::intptr_t dataFragmentLength(LogFileBufferRemainingCapacity());

      for(;;)
      {
         if(dataLength > dataFragmentLength)
         {
            dataLength -= dataFragmentLength;
         }
         else
         {
            dataFragmentLength = dataLength;
            dataLength = 0;
         }

         ::std::string::traits_type::copy(LogFileBufferPointerAtPosition(), data, static_cast<::std::size_t>(dataFragmentLength));
         AdvanceLogFileBufferPosition(dataFragmentLength);

         if(dataLength > 0)
         {
         }
         else
         {
            break;
         }

         Flush();
         data += dataFragmentLength;
         dataFragmentLength = static_cast<::std::intptr_t>(LogFileBuffer_.length());
      }
   }

   void TLogger::Flush()
   {
      assert(FlushIsNeeded());

      if(NextLogFileBufferBeginPosition_ > 0)
      {
      }
      else
      {
         // <Comment201905101>
         // It's safe to unconditionally jump to this condition's branch.
         // </Comment201905101>
         assert(NextLogFileDateTime_ > 0);
         goto label11;
      }

      CreateOrOpenLogFileIfNeeded();
      FlushLogFileBuffer(0, ::std::min(LogFileBufferPosition_, NextLogFileBufferBeginPosition_));

      // Comment201905101 relates.
      if(NextLogFileDateTime_ > 0)

      {
      label11:
         NextLogFileDate_ =
            NextLogFileDateTime_ -
            static_cast<::std::int64_t>
               (static_cast<::std::uint64_t>(NextLogFileDateTime_) % static_cast<::std::uint64_t>(Time::TTimeHelpers::NumberNanoSecondsPerDay)) +
            Time::TTimeHelpers::NumberNanoSecondsPerDay;
         NextLogFileDateTime_ = 0;

         if(LogFileDescriptor_ != ( - 1 ))
         {
            // Comment201905125 relates and/or applies.
            // Assuming this will succeed. And even otherwise we will probably be OK.
#if(defined(_MSC_VER))
            ::_close(LogFileDescriptor_);
#else
            ::close(LogFileDescriptor_);
#endif

            LogFileDescriptor_ = ( - 1 );
         }
         else
         {
         }

         assert(LogFileBufferPosition_ >= NextLogFileBufferBeginPosition_);

         if(LogFileBufferPosition_ > NextLogFileBufferBeginPosition_)
         {
            CreateOrOpenLogFile();
            FlushLogFileBuffer(NextLogFileBufferBeginPosition_, LogFileBufferPosition_ - NextLogFileBufferBeginPosition_);
         }
         else
         {
         }

         NextLogFileBufferBeginPosition_ = ::std::numeric_limits<::std::intptr_t>::max();
      }
      else
      {
         assert(NextLogFileBufferBeginPosition_ == ::std::numeric_limits<::std::intptr_t>::max());
      }

      LogFileBufferPosition_ = 0;
   }

   void TLogger::CreateOrOpenLogFile()
   {
      assert(LogFileDescriptor_ == ( - 1 ));
      assert(NextLogFileDate_ >= Time::TTimeHelpers::NumberNanoSecondsPerDay);

      if(DateStampPositionInLogFilePathName_ >= 0)
      {
         // <Comment201905113>
         // Date-stamp in file name occupies 8 chars.
         // </Comment201905113>
         char chars1[8 + 1];
         Time::TTimeHelpers::FormatSecondResolutionDateTime
            (NextLogFileDate_ - Time::TTimeHelpers::NumberNanoSecondsPerDay, "%Y%m%d", chars1, sizeof(chars1));
         ::std::string::traits_type::copy
            (( & LogFilePathName_.front() ) + DateStampPositionInLogFilePathName_, chars1, sizeof(chars1) - 1U);
      }
      else
      {
      }

      LogFileDescriptor_ =
         InputOutput::TFileSystemHelpers::Open
            ( LogFilePathName_.c_str(),
#if(defined(_MSC_VER))
               _O_WRONLY | _O_CREAT | _O_APPEND | _O_SEQUENTIAL | _O_BINARY,

               // Without this, the file would be created with the read-only attribute.
               _S_IREAD | _S_IWRITE
#else
               O_WRONLY | O_CREAT | O_APPEND,

               // Issue: Is this correct?
               S_IRUSR | S_IWUSR //| S_IRGRP | S_IWGRP
#endif
            );
   }
}
