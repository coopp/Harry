#pragma once
#include "AnimatedSprite.h"

namespace SimpleGame
{
    class ProgressBar
    {
    public:
        ProgressBar(int length, Windows::UI::Color forgroundColor, Windows::UI::Color backgroundColor);
        void Update(float current, float total);
        void Draw(Windows::Foundation::Numerics::float2 position);
    private:
        float _percentComplete;
        int _barLength;
        Windows::UI::Color _backgroundColor;
        Windows::UI::Color _foregroundColor;
    };
}