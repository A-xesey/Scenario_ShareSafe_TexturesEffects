#include "stdafx.h"
#include "Global.h"
#include "DebugCheats.h"

using namespace ArgScript;


namespace DebugCheats
{
    uint32_t ParseUniversal(FormatParser* pFormatParser, const char* szArgument)
    {
        return (szArgument[0] == '0' && szArgument[1] == 'x')
            ? pFormatParser->ParseUInt(szArgument)
            : id(szArgument);
    }

    ResourceKey ParseKey(FormatParser* pFormatParser, const char* szArgument)
    {
        char buffer[64];
        strncpy_s(buffer, sizeof(buffer), szArgument, _TRUNCATE);
        buffer[sizeof(buffer) - 1] = '\0';

        char* pInstance = strchr(buffer, '!');
        char* pType = strchr(buffer, '.');
        if (pInstance)
            *pInstance = '\0';
        if (pType)
            *pType = '\0';
        return {
            ParseUniversal(pFormatParser, pInstance
                ? pInstance + 1
                : buffer
            ),
            pType
                ? ParseUniversal(pFormatParser, pType + 1)
                : 0x0,
            pInstance
                ? ParseUniversal(pFormatParser, buffer)
                : 0x0
        };
    }
}
