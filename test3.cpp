// 基于CommandLineToArgvW，支持ascii类型参数
// https://blog.csdn.net/qq_22000459/article/details/83539644
LPSTR* CommandLineToArgvA(LPSTR lpCmdLine, INT *pNumArgs)
{
    int retval;
    retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, NULL, 0);
    if (!SUCCEEDED(retval))
        return NULL;
 
    LPWSTR lpWideCharStr = (LPWSTR)malloc(retval * sizeof(WCHAR));
    if (lpWideCharStr == NULL)
        return NULL;
 
    retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, lpWideCharStr, retval);
    if (!SUCCEEDED(retval))
    {
        free(lpWideCharStr);
        return NULL;
    }
 
    int numArgs;
    LPWSTR* args;
    args = CommandLineToArgvW(lpWideCharStr, &numArgs);
    free(lpWideCharStr);
    if (args == NULL)
        return NULL;
 
    int storage = numArgs * sizeof(LPSTR);
    for (int i = 0; i < numArgs; ++ i)
    {
        BOOL lpUsedDefaultChar = FALSE;
        retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, NULL, 0, NULL, &lpUsedDefaultChar);
        if (!SUCCEEDED(retval))
        {
            LocalFree(args);
            return NULL;
        }
 
        storage += retval;
    }
 
    LPSTR* result = (LPSTR*)LocalAlloc(LMEM_FIXED, storage);
    if (result == NULL)
    {
        LocalFree(args);
        return NULL;
    }
 
    int bufLen = storage - numArgs * sizeof(LPSTR);
    LPSTR buffer = ((LPSTR)result) + numArgs * sizeof(LPSTR);
    for (int i = 0; i < numArgs; ++ i)
    {
        assert(bufLen > 0);
        BOOL lpUsedDefaultChar = FALSE;
        retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, buffer, bufLen, NULL, &lpUsedDefaultChar);
        if (!SUCCEEDED(retval))
        {
            LocalFree(result);
            LocalFree(args);
            return NULL;
        }
 
        result[i] = buffer;
        buffer += retval;
        bufLen -= retval;
    }
 
    LocalFree(args);
 
    *pNumArgs = numArgs;
    return result;
}

// 基于 CommandLineToArgvW 的参数获取
// https://blog.csdn.net/hellokandy/article/details/76445151
#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <map>

int ParseCmdLine(const wchar_t* lpCmdLine, std::map<std::wstring, std::wstring>& pMapCmdLine)
{
	int nArgs = 0;
	LPWSTR * szArglist = CommandLineToArgvW(lpCmdLine, &nArgs);		
 
	for (int i = 0; i < nArgs; i++)
	{
		if (wcsncmp(L"-", szArglist[i], 1) != 0)
		{
			continue;
		}
		if (i+1 < nArgs) //结束
		{
			if (wcsncmp(L"-", szArglist[i+1], 1) != 0)
			{
				pMapCmdLine.insert(std::make_pair(szArglist[i], szArglist[i+1]));
				i++;
				continue;
			}
		}
		pMapCmdLine.insert(std::make_pair(szArglist[i], L"1"));
	}
	LocalFree(szArglist);
	return 0;
}
 
int _tmain(int argc, _TCHAR* argv[])
{
	std::map<std::wstring, std::wstring> mapCmd;
	ParseCmdLine(GetCommandLineW(), mapCmd);
 
	int nCount = mapCmd.size();
	std::map<std::wstring, std::wstring>::iterator itr = mapCmd.begin();
	for (; itr!= mapCmd.end(); )
	{
		printf("key: %ls value: %ls\r\n", itr->first.c_str(), itr->second.c_str());
		++itr;
	}
	
	getchar();
 
	return 0;
}
