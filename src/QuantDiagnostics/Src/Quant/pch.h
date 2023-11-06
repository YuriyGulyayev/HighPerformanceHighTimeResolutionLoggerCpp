#pragma once

// We check this to tell if we target Windows or Linux.
#if(defined(_MSC_VER))
   // <Comment201802047>
   // This comment is referenced in the documentation.
   // Remember to enable all warnings in project properties.
   // Google for a help page titled "Compiler Warnings That Are Off by Default".
   // </Comment201802047>
   #if(_MSC_VER > 1916)
      #error Revisit these warnings.
   #endif

   #pragma warning(disable: 4464)
   #pragma warning(disable: 4514)
   #pragma warning(disable: 4710)
   #pragma warning(disable: 4711)
   #pragma warning(disable: 4820)

   // Issue: This is questionable.
   #define _CRT_SECURE_NO_WARNINGS

   #define NOMINMAX

   #pragma warning(push)
   #pragma warning(disable: 4365)
   #pragma warning(disable: 4571)
   #pragma warning(disable: 4625)
   #pragma warning(disable: 4626)
   #pragma warning(disable: 4668)
   #pragma warning(disable: 4774)
   #pragma warning(disable: 5026)
   #pragma warning(disable: 5027)
   #pragma warning(disable: 5039)

   //#include <stdexcept>
   #include <string>
   #include <algorithm>
   //#include <cstdint>
   #include <cstdarg>
   #include <cassert>
   #include <ctime>
   #include <io.h>
   //#include <unistd.h>
   #include <fcntl.h>
   #include <Windows.h>

   #pragma warning(pop)
#else
   #include <stdexcept>
   //#include <string>
   //#include <algorithm>
   //#include <cstdint>
   #include <cstdarg>
   #include <cassert>
   #include <ctime>
   //#include <io.h>
   #include <unistd.h>
   #include <fcntl.h>
   //#include <Windows.h>
#endif
