#include "CrashReporter.h"

#include <windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "Dbghelp.lib")

inline bool CreateMiniDump(EXCEPTION_POINTERS *pException, TCHAR *sFileName)
{
	BOOL ret = FALSE;

	HANDLE hFile = CreateFile(sFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ExInfo;
		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pException;
		ExInfo.ClientPointers = NULL;

		ret = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);

		CloseHandle(hFile);
	}

	return ret;
}

LONG ExceptCallBack(EXCEPTION_POINTERS *pExcPointer)
{
	TCHAR sFileName[MAX_PATH] = {0};
	SYSTEMTIME st = {0};
	GetLocalTime(&st);

	wsprintf(sFileName, "Crash[%04d-%02d-%02d-%02d-%02d-%02d-%02d-%02d].dmp",
			 st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, rand() % 100);

	BOOL ret = CreateMiniDump(pExcPointer, sFileName);

	TCHAR sMsg[MAX_PATH] = {0};
	wsprintf(sMsg, "The application crashed, dump site is %s", sFileName);

	MessageBox(NULL, sMsg, "CrashReporter", MB_OK);

	return EXCEPTION_EXECUTE_HANDLER;
}

void CrashReporter::RegUnhandleException(std::function<void()> tUserCall)
{
	SetUnhandledExceptionFilter(ExceptCallBack);
}
