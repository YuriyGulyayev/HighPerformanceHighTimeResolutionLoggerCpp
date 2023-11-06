#pragma once

#include "../Quant/pch.h"

#if(defined(_MSC_VER))
   // Comment201802047 applies.
   #pragma warning(push)
   #pragma warning(disable: 4623)
   #pragma warning(disable: 4625)
   #pragma warning(disable: 4626)
   #pragma warning(disable: 5027)
   #pragma warning(disable: 5039)

   //#include <thread>
   #include <mutex>

   #pragma warning(pop)
#else
   #include <thread>
   #include <mutex>
#endif
