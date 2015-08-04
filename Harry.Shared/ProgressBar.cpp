#include "pch.h"
#include "ProgressBar.h"

using namespace SimpleGame;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI;

ProgressBar::ProgressBar(int length,Color foregroundColor, Color backgroundColor) :
    _barLength(length),
    _foregroundColor(foregroundColor),
    _backgroundColor(backgroundColor)
{

}

void ProgressBar::Update(float current, float total)
{
    _percentComplete = current / total;
}

void ProgressBar::Draw(float2 position)
{
    GameContext& context = GameContext::GetInstance();
    float2 startPos = position;
    float2 endPos = float2(position.x + _barLength, position.y);
    context.DrawLine(startPos, endPos, _backgroundColor, 6);
    endPos.x = startPos.x + (_barLength * _percentComplete);
    context.DrawLine(startPos, endPos, _foregroundColor, 6);
}