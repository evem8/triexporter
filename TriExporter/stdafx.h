// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <Mmsystem.h>
#include <tchar.h>


#include <atlstr.h>
#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <map>
#include <algorithm>
#include <functional>
#include <cctype> 


using namespace std;
typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;
typedef unsigned long long  qword;


#if defined _M_IX86
	#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
	#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
	#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
	#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
