/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include "6D/Allocators"
#include "6D/wstrings"
// stdafx.cpp : source file that includes just the standard includes
// 5D.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

//#include <internal.h>

extern "C" {
	FILE * __cdecl _getstream(void);
};
FILE* fmemopen(void* contents, size_t contents_length, const char* mode) {
	FILE* str;
#ifdef _MSC_VER
	/*
	InitializeCriticalSection( &(((_FILEX *)str)->lock) );
	__crtInitCritSecAndSpinCount
	*/
	str = _getstream();
#else
	str = _tfdopen(0, _T("r"));
	str->_bufsiz = 0;
#endif
	str->_flag = _IOREAD|_IOSTRG|_IOMYBUF;
	str->_ptr = str->_base = (char *)contents;
	str->_cnt = contents_length; // INT_MAX;
	return(str);
	/*
	int	_file;
	int	_charbuf;
	char*	_tmpfname;*/
}
char* utf8FromWstring(const std::wstring& source) {
	int count = WideCharToMultiByte(CP_UTF8, 0, source.c_str(), -1, NULL, 0, NULL, NULL);
	char* result = (char*) GC_MALLOC_ATOMIC(count);
	if(WideCharToMultiByte(CP_UTF8, 0, source.c_str(), -1, result, count, NULL, NULL) != count)
		abort();
	return(result);
}
std::wstring wstringFromUtf8(const char* source) {
	int count = MultiByteToWideChar(CP_UTF8, 0, source, -1, NULL, 0);
	WCHAR* result = (WCHAR*) GC_MALLOC_ATOMIC(count*sizeof(WCHAR));
	if(MultiByteToWideChar(CP_UTF8, 0, source, -1, result, count) != count)
		abort();
	return(result);
}
/*
std::wstring GetWIN32Diagnostics(void) {
	LPVOID lpMsgBuf;
	//LPVOID lpDisplayBuf;
	std::wstring result;
	DWORD dw = GetLastError(); 
	if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL) > 0) {
		result = (LPWSTR) lpMsgBuf;
		LocalFree(lpMsgBuf);
	} else
		result = _T("???");
	return(result);
}
*/
/* TODO nicer way would be: 
std::wstring_convert::from_bytes (codecvt)
std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16conv;
std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf32conv;
std::u32string utf32 = utf32conv.from_bytes(utf8); (!!!!)
<locale>

*/
