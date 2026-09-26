#pragma once

namespace DebugCheats
{
    class GroundTextureTestCheat
        : public ArgScript::ICommand
    {
    public:
        static constexpr const char* NAME = "sssteSetDetailTexture";

        GroundTextureTestCheat();
        ~GroundTextureTestCheat();

        void ParseLine(const ArgScript::Line& line) override;

        const char* GetDescription(ArgScript::DescriptionMode mode) const override;
    };
}
