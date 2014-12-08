#ifndef __JTEST_HPP__
#define __JTEST_HPP__

#include <sstream>
#include <string>
#include <iostream>
#include <time.h>

static bool g_disable_log = false;

///////////////////////////////////////////////////////////////////////
#define LOG(m) do {\
	if (g_disable_log) break;\
	std::cout << CurrTime() << " " << __FUNCTION__ << "(" << __LINE__ << "): " << m << std::endl;\
}while(0)

#if defined(WIN32) && !defined(__GNUC__)
#define localtime_r(a,b) localtime_s(b,a)
#endif

inline
std::string CurrTime() {
    time_t curr = time(NULL);
    struct tm t;
	localtime_r(&curr, &t);
    char buf[12] = {'\0',};
    strftime(buf, sizeof(buf), "%H:%M:%S", &t); //%H:%M:%S.%L
    return std::string(buf);
}

template <typename L, typename R>
bool expect(L l, R r, const char *l_str, const char *r_str, size_t line, const char* func) {
	std::ostringstream oss;
	bool ret = false;

	oss << CurrTime() << " " << func << "(" << line << "): ";
	if (l == r) {
		oss << "\"" << l_str << " == " << r_str << "\" is pass";
		ret = true;
	} else {
		oss << "\"" << r_str << "\" expected value is " << l_str << ", but is "<< r;
	}

	std::cout << oss.str() << std::endl;

	return ret;
}


#define EXPECT_EQ(l, r) do {\
	if (!expect((l), (r), #l, #r, __LINE__, __FUNCTION__)) return 1;\
} while(0)

#define EXPECT_TRUE(r) do {\
	if (!expect(true, (r), "true", #r, __LINE__, __FUNCTION__)) return 1;\
} while(0)

#define EXPECT_FALSE(r) do {\
	if (!expect(false, (r), "false", #r, __LINE__, __FUNCTION__)) return 1;\
} while(0)

// ///////////////////////////////////////////////////////////////////////
// typedef unsigned long Time_t;

// Time_t get_time() {
// #ifdef WIN32
// 	return timeGetTime();
// #else
// 	struct timeval tv;
// 	gettimeofday(&tv, NULL);
// 	return (tv.tv_sec*1000 + tv.tv_usec/1000);
// #endif
// }

// std::string GetLastErrorStr() {
//   LPVOID lpMsgBuf; 
//   //MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
//   FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//      NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_ENGLISH_US), // Default language
//      (LPTSTR) &lpMsgBuf, 0, NULL ); // Process any inserts in lpMsgBuf. // ... 
//      // Display the string. 
//   std::string str((const char*)lpMsgBuf);
//   LocalFree( lpMsgBuf );
//   return str;
// }

// //http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process

// //http://msdn.microsoft.com/en-us/library/windows/desktop/aa366589(v=vs.85).aspx
// size_t get_free_global_mem() {
// 	MEMORYSTATUSEX mem;
// 	mem.dwLength = sizeof(MEMORYSTATUSEX);
// 	if (0==GlobalMemoryStatusEx(&mem)) {
// 		LOG("error: " << GetLastError());
// 		return 0;
// 	}

// 	return mem.ullAvailPhys;
// }

// //for Linux: sysinfo()
// //http://msdn.microsoft.com/en-us/library/windows/desktop/ms683219(v=vs.85).aspx
// size_t get_workingset_mem() {
// 	PROCESS_MEMORY_COUNTERS pmc;
// 	if (!GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
// 		LOG("error: " << GetLastError());
// 		return 0;
// 	}
// 	return pmc.WorkingSetSize;
// }

#endif //__JTEST_HPP__