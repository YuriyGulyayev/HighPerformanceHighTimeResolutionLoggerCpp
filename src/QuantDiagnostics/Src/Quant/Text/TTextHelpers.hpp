#pragma once

namespace Quant::Text
{
   class TTextHelpers
#if(defined(_MSC_VER))
      abstract
#endif
      final
   {
      // ToDo.3: Does this exist in any standard library?
      //public: static inline ::std::string const NewLine =
      public: static inline char const NewLine[] =
#if(defined(_MSC_VER))
         //"\r\n"s;
         "\r\n";
#else
         //"\n"s;
         "\n";
#endif
   };
}
