#pragma once

#include "./TUtcToLocalLinuxDateTimeConverter.hpp"

namespace Quant::Time
{
#if( ! defined(_MSC_VER) )
   class TMainUtcToLocalLinuxDateTimeConverterHelpers
//#if(defined(_MSC_VER))
//      abstract
//#endif
      final
   {
      // Comment201905051 relates.
      public: static /*thread_local*/ inline TUtcToLocalLinuxDateTimeConverter MainUtcToLocalLinuxDateTimeConverter;
   };
#endif
}
