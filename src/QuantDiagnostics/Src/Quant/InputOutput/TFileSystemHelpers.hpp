#pragma once

namespace Quant::InputOutput
{
   class TFileSystemHelpers
#if(defined(_MSC_VER))
      abstract
#endif
      final
   {
      // This method can throw an {::std::runtime_error} exception.
      public: static int Open
         ( char const * fileName,
            int oFlag,
#if(defined(_MSC_VER))
            int pMode = 0
#else
            ::mode_t pMode = 0U
#endif
         )
      {
#if(defined(_MSC_VER))
         int fileDescriptor(::_open(fileName, oFlag, pMode));
#else
         int fileDescriptor(::open(fileName, oFlag, pMode));
#endif

         if(fileDescriptor != ( - 1 ))
         {
         }
         else
         {
            throw ::std::runtime_error
#if(defined(_MSC_VER))
               //("Error; {::_open} failed");
               ("ERROR | {::_open} failed");
#else
               //("Error; {::open} failed");
               ("ERROR | {::open} failed");
#endif
         }

         return fileDescriptor;
      }

      // This method can throw an {::std::runtime_error} exception.
      public: static void Write
         ( int fileDescriptor,
            void const * buf,
#if(defined(_MSC_VER))
            unsigned int maxCharCount
#else
            ::std::size_t maxCharCount
#endif
         )
      {
#if(defined(_MSC_VER))
         int writeReturnValue(::_write(fileDescriptor, buf, maxCharCount));

         if(writeReturnValue != ( - 1 ))
         {
            assert(static_cast<unsigned int>(writeReturnValue) == maxCharCount);
         }
         else
         {
            throw ::std::runtime_error
               //("Error; {::_write} failed");
               ("ERROR | {::_write} failed");
         }
#else
         // <Comment201905084>
         // This is important where this comment is referenced.
         // </Comment201905084>
         assert(maxCharCount != ( - 1U ));

         // <Comment201905204>
         // This can potentially write fewer bytes than requested, which we will check for.
         // </Comment201905204>
         ::ssize_t writeReturnValue(::write(fileDescriptor, buf, maxCharCount));

         // Comment201905204 relates and/or applies.
         // Comment201905084 relates and/or applies.
         if(static_cast<::std::size_t>(writeReturnValue) == maxCharCount)

         {
         }
         else
         {
            throw ::std::runtime_error
               //("Error; {::write} failed");
               ("ERROR | {::write} failed");
         }
#endif
      }
   };
}
