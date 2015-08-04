#pragma once
#include "Entity.h"
#include "AnimatedSprite.h"

namespace Harry
{
    class Harrydactyl : public SimpleGame::Entity
    {
    public:
        Harrydactyl(Windows::Foundation::Numerics::float2 position);
        void Update(float elapsedTime) override;
        void Draw(float darkenAmount) override;
        void Tap();
        void FreeFall();
        void GivePoints(int points);
        int GetPoints();
        bool IsHarryActive();
        int GetHealth();
        void AddHealth(int healthPoints);
        void Hurt();
        void Activate(bool activate);
    private:
        Windows::Foundation::Numerics::float2 _velocity;
        SimpleGame::AnimatedSprite _flyingAnimation;
        int _points;
        int _healthPoints;
        bool _active;
        float _invulnerableTimer;
    };
}