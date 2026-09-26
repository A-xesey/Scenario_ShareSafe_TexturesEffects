#pragma once

namespace DebugCheats
{
    class VisualEffectTestCheat
        : public ArgScript::ICommand
    {
    public:
        static constexpr const char* NAME = "sssteSetVisualStyle";

        VisualEffectTestCheat();
        ~VisualEffectTestCheat();

        void ParseLine(const ArgScript::Line& line) override;

        const char* GetDescription(ArgScript::DescriptionMode mode) const override;
    };
}
