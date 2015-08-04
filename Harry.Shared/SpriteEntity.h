#pragma once
#include "Entity.h"
#include "AnimatedSprite.h"

namespace SimpleGame
{
    class SpriteEntity : public Entity
    {
    public:
        SpriteEntity(Windows::Foundation::Numerics::float2 position, Sprite firstFrame, int numFrames = 1, float duration = 0.0f, int pointValue = 0, bool horizontallyFlipped = false);
        void Update(float elapsedTime) override;
        void Draw(float darkenAmount) override;
    private:
        AnimatedSprite _sprite;
        bool _horizontallyFlipped;
    };
}