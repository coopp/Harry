#include "pch.h"
#include "Entity.h"
#include "SpriteEntity.h"

using namespace SimpleGame;
using namespace Windows::UI;
using namespace Windows::Foundation::Numerics;

SpriteEntity::SpriteEntity(float2 position, Sprite firstFrame, int numFrames, float duration, int pointValue, bool horizontallyFlipped) :
    Entity(position, GameContext::GetInstance().GetSpriteWidth(firstFrame) / 2.0f),
    _sprite(firstFrame, numFrames, duration, horizontallyFlipped)
{
    // If points are assigned, then this sprite is collidable and should
    // give points.
    if (pointValue > 0)
    {
        _collidable = true;
        _pointValue = pointValue;
    }
}

void SpriteEntity::Update(float elapsedTime)
{
    _sprite.Update(elapsedTime);
}

void SpriteEntity::Draw(float darkenAmount)
{
    _sprite.Draw(_position, darkenAmount);
    GameContext& context = GameContext::GetInstance();

    if (context.GetEnableDebugDrawing())
    {
        context.DrawCircle(_position, _radius, Windows::UI::Colors::Orange);
    }
}
