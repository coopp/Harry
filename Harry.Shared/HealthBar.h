#pragma once
#include "AnimatedSprite.h"

namespace SimpleGame
{
    class HealthBar
    {
    public:
        HealthBar();
        void Update(float elapsedTime, int health);
        void Draw(Windows::Foundation::Numerics::float2 position);
    private:
        std::vector<std::shared_ptr<AnimatedSprite>> _hearts;
        int _healthyHearts;
    };
}