#include "pch.h"

namespace SimpleGame { namespace Utilities {

const size_t maxFormatString = 80;

void strprintf(std::wstring& sOut, const wchar_t* fmt, ...)
{
    if (sOut.size() < maxFormatString)
    {
        sOut.resize(maxFormatString);
    }
    va_list args;
    va_start(args, fmt);
    _vsnwprintf_s((wchar_t*)sOut.data(), sOut.size(), maxFormatString, fmt, args);
    va_end(args);
}

}}