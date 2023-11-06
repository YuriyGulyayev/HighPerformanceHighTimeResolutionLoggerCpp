#pragma once

#include "./TUtcToLocalWindowsDateTimeConverter.hpp"

namespace Quant::Time
{
#if(defined(_MSC_VER))
   class TMainUtcToLocalWindowsDateTimeConverterHelpers
//#if(defined(_MSC_VER))
      abstract
//#endif
      final
   {
      // Comment201905051 relates.
      public: static /*thread_local*/ inline TUtcToLocalWindowsDateTimeConverter MainUtcToLocalWindowsDateTimeConverter;
   };
#endif
}
