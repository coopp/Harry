#include "pch.h"
#include "ScrollingBackground.h"

using namespace SimpleGame;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;

ScrollingBackground::ScrollingBackground() :
    _scrollRate(1.0f),
    _layerWidth(1.0f),
    _layerHeight(1.0f)
{

}

void ScrollingBackground::Draw(float2 cameraPosition, float darkenAmount)
{
    if (_layers.size() == 0)
    {
        return;
    }

    GameContext& context = GameContext::GetInstance();

    float x = -cameraPosition.x * _scrollRate;
    float y = context.GetRenderSize().Height - _layerHeight;
    float leftSegment = floorf(x / _layerWidth);
    float rightSegment = leftSegment + 1.0f;
    x = ceilf((x / _layerWidth - leftSegment) * -_layerWidth);

    float2 pos1(x, y);
    float2 pos2(x + _layerWidth, y);

    context.DrawSprite(_layers[(int)leftSegment % _layers.size()], pos1, false, false, darkenAmount);
    context.DrawSprite(_layers[(int)rightSegment % _layers.size()], pos2, false, false, darkenAmount);
}

void ScrollingBackground::SetScrollRate(float scrollRate)
{
    _scrollRate = scrollRate;
}

void ScrollingBackground::AddLayer(Sprite layer)
{
    GameContext& context = GameContext::GetInstance();

    if (_layers.size() > 0 && _layerWidth != context.GetSpriteWidth(layer))
    {
        throw ref new Platform::FailureException("Failed to add scrolling layer. Layers must have equal widths");
    }

    _layers.push_back(layer);
    _layerWidth = context.GetSpriteWidth(layer);
    _layerHeight = context.GetSpriteHeight(layer);
}

void ScrollingBackground::Clear()
{
    _layers.clear();
    _layerWidth = 1.0f;
    _layerHeight = 1.0f;
}
