#pragma once

namespace DebugCheats
{
    uint32_t ParseUniversal(ArgScript::FormatParser* pFormatParser, const char* szArgument);

    ResourceKey ParseKey(ArgScript::FormatParser* pFormatParser, const char* szArgument);
}
