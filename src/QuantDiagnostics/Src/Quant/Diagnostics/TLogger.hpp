#pragma once

#include "../InputOutput/TFileSystemHelpers.hpp"

namespace Quant::Diagnostics
{
   // ToDo.3: Do we need to use/support any {::std::string_view}?
   class TLogger //final
   {
      private: static inline char const DateStampSubSrtingToSubstituteInLogFilePathName_[] = "<DateStamp>";

      // In here, a part of {DateStampSubSrtingToSubstituteInLogFilePathName_} has been removed.
      private: ::std::string LogFilePathName_;

      // <Comment201905125>
      // This may be negative.
      // If this is negative it's unnecessary to close and reopen the same file periodically, but keeping it simple.
      // </Comment201905125>
      private: ::std::intptr_t DateStampPositionInLogFilePathName_;

      // Different parts of the buffer can belong to different dates.
      private: ::std::string LogFileBuffer_;

      private: ::std::int64_t NextLogFileDate_;
      private: ::std::int64_t NextLogFileDateTime_;
      private: ::std::intptr_t LogFileBufferPosition_;

      // This used to be named {NextLogFileBeginPositionInLogFileBuffer_}.
      private: ::std::intptr_t NextLogFileBufferBeginPosition_;

      private: int LogFileDescriptor_;

      // <Comment201905205>
      // Writing to the console is implemented as writing to StdOut or StdErr.
      // StdOut file descriptor equals 1.
      // StdErr file descriptor equals 2.
      // If this equals -1 we will not write to this file.
      // On Windows, the application should set the file mode to binary by calling {_setmode}.
      // As a hack, you may use any other file descriptor as well.
      // </Comment201905205>
      private: int ConsoleFileDescriptor_;

      public: /*explicit*/ TLogger():
         NextLogFileDate_(0),
         NextLogFileDateTime_(0),
         LogFileBufferPosition_(0),
         NextLogFileBufferBeginPosition_(::std::numeric_limits<::std::intptr_t>::max()),
         LogFileDescriptor_( - 1 )
      {
      }

      TLogger( TLogger const & ) = delete;
      TLogger & operator = ( TLogger const & ) = delete;
      TLogger( TLogger && ) = delete;
      TLogger & operator = ( TLogger && ) = delete;
      ~TLogger() = default;

      // {logFilePathName} can be something like "C:/Your/Path/<DateStamp>-YourAppName.txt".
      // The path must be full in case it's possible that the app will change its current directory.
      public: void Configure
         ( ::std::string && logFilePathName,
            ::std::intptr_t logFileBufferCapacity
         );

      // Comment201905205 applies to {consoleFileDescriptor}.
      public: void Prepare
         ( int consoleFileDescriptor
         )
      {
         ConsoleFileDescriptor_ = consoleFileDescriptor;
      }

      public: ::std::intptr_t LogFileBufferRemainingCapacity() const
      {
         return static_cast<::std::intptr_t>(LogFileBuffer_.length()) - LogFileBufferPosition_;
      }

      // Comment201905119 applies.
      public: char * LogFileBufferPointerAtPosition() //const
      {
         return ( & LogFileBuffer_.front() ) + LogFileBufferPosition_;
      }

      // This is to be called every time before logging another record.
      // It's not required to log anything after calling this.
      // Based on this, we will name the log file.
      // The date-time is typically the current one, but it doesn't have to be.
      // A typical caller doesn't call this directly, but rather calls {TMainLoggerHelpers::BeginLog},
      // which, in turn, often isn't called directly either.
      public: void UpdateDateTime
         ( ::std::int64_t dateTime
         );

      // <Comment201905119>
      // A typical code pattern is to call {EnsureLogFileBufferRemainingCapacity}, append data to {LogFileBufferPointerAtPosition},
      // call {AdvanceLogFileBufferPosition} and pass a value that is less than or equal
      // the one passed to {EnsureLogFileBufferRemainingCapacity}.
      // </Comment201905119>
      // Assumption: {logFileBufferRemainingCapacityMinLimit} is not greater than the buffer capacity.
      public: void EnsureLogFileBufferRemainingCapacity
         ( ::std::intptr_t logFileBufferRemainingCapacityMinLimit
         )
      {
         // As noted above, this is assumed.
         assert(static_cast<::std::string::size_type>(logFileBufferRemainingCapacityMinLimit) <= LogFileBuffer_.length());

         if(logFileBufferRemainingCapacityMinLimit > LogFileBufferRemainingCapacity())
         {
            Flush();
         }
         else
         {
         }
      }

      // Comment201905119 applies.
      public: void AdvanceLogFileBufferPosition
         ( ::std::intptr_t logFileBufferPositionAdvance
         )
      {
         // Comment201905133 relates.
         assert(logFileBufferPositionAdvance >= 0);

         LogFileBufferPosition_ += logFileBufferPositionAdvance;
         assert(static_cast<::std::string::size_type>(LogFileBufferPosition_) <= LogFileBuffer_.length());
      }

      public: void LogFragment
         ( ::std::string const & data
         )
      {
         LogFragment(( & data.front() ), static_cast<::std::intptr_t>(data.length()));
      }

      public: void LogFragment
         ( char const * data
         )
      {
         LogFragment(data, static_cast<::std::intptr_t>(::std::string::traits_type::length(data)));
      }

      // {dataLength} may exceed the buffer capacity.
      public: void LogFragment
         ( char const * data,
            ::std::intptr_t dataLength
         );

      // <Comment201905121>
      // {Flush} assumes that {FlushIsNeeded} would return {true}.
      // The client code shall call {FlushIfNeeded} periodically, typically on a separate thread, as well as at the end of {main}.
      // It must be called from within a critical section, which applies to other functions as well.
      // Alternatively, you can call {FlushIsNeeded} outside a critical section, and only if it returns {true}
      // enter the critical section and call {FlushIfNeeded}. Note that in this case it's still important to double-check
      // the "is needed" condition because it's possible that main thread has flushed us a moment before,
      // therefore a typical app must not call {Flush} directly.
      // </Comment201905121>
      public: void FlushIfNeeded()
      {
         if(FlushIsNeeded())
         {
            Flush();
         }
         else
         {
         }
      }

      // Comment201905121 applies.
      public: bool FlushIsNeeded() const
      {
         // Non-volatile read.
         return LogFileBufferPosition_ > 0;
      }

      // Comment201905121 applies.
      public: void Flush();

      private: void CreateOrOpenLogFileIfNeeded()
      {
         if(LogFileDescriptor_ < 0)
         {
            CreateOrOpenLogFile();
         }
         else
         {
         }
      }

      private: void CreateOrOpenLogFile();

      private: void FlushLogFileBuffer
         ( ::std::intptr_t position,
            ::std::intptr_t length
         )
         const
      {
         void const * pointerWithinLogFileBuffer(( & LogFileBuffer_.front() ) + position);

         // Comment201905062 relates and/or applies.
         InputOutput::TFileSystemHelpers::Write
            ( LogFileDescriptor_,
               pointerWithinLogFileBuffer,
#if(defined(_MSC_VER))
               static_cast<unsigned int>(length)
#else
               static_cast<::std::size_t>(length)
#endif
            );

         if(ConsoleFileDescriptor_ != ( - 1 ))
         {
            // Comment201905062 relates and/or applies.
            InputOutput::TFileSystemHelpers::Write
               ( ConsoleFileDescriptor_,
                  pointerWithinLogFileBuffer,
#if(defined(_MSC_VER))
                  static_cast<unsigned int>(length)
#else
                  static_cast<::std::size_t>(length)
#endif
               );
         }
         else
         {
         }
      }
   };
}
