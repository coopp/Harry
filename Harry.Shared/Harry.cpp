#include "pch.h"
#include "Entity.h"
#include "Harry.h"

using namespace Harry;
using namespace SimpleGame;
using namespace Windows::Foundation::Numerics;

Harrydactyl ::Harrydactyl(float2 position) :
    Entity(position, 32.0f),
    _points(0),
    _healthPoints(3),
    _active(true),
    _flyingAnimation(HarryFly00, 8, 0.05f, false)
{
    _velocity = float2(100.0f, 0);
}

void Harrydactyl::Update(float elapsedTime)
{
    if (_active)
    {
        const float DefaultGravity = 420.0f;
        float force = DefaultGravity;

        _velocity.y += force * elapsedTime;
        _position += _velocity * elapsedTime;

        // Snap to a whole pixel value
        _position.x = ceilf(_position.x);
        _position.y = ceilf(_position.y);

        _invulnerableTimer -= elapsedTime;
        if (_invulnerableTimer < 0.0f)
        {
            _invulnerableTimer = 0.0f;
        }
    }

    _flyingAnimation.Update(elapsedTime);
}

void Harrydactyl::Draw(float darkenAmount)
{
    // Draw Harry at half of the darken amount to keep him bright enough to see
    _flyingAnimation.Draw(_position, darkenAmount / 2.0f);
}

void Harrydactyl::Tap()
{
    _velocity.y = -175.0f;
}

void Harrydactyl::FreeFall()
{
    _velocity.y = 0.0f;
}

int Harrydactyl::GetPoints()
{
    return _points;
}

void Harrydactyl::GivePoints(int points)
{
    _points += points;
}

bool Harrydactyl::IsHarryActive()
{
    return _active;
}

void Harrydactyl::AddHealth(int healthPoints)
{
    if (_healthPoints < 6)
    {
        _healthPoints += healthPoints;
    }
}

int Harrydactyl::GetHealth()
{
    return _healthPoints;
}

void Harrydactyl::Hurt()
{
    if (_invulnerableTimer > 0.0f)
    {
        return;
    }

    _healthPoints -= 1;
    if (_healthPoints <= 0)
    {
        _healthPoints = 0;
        Kill();
    }

    if (!IsDead())
    {
        _invulnerableTimer = 2.0f;
    }
}

void Harrydactyl::Activate(bool activate)
{
    if (!activate)
    {
        _velocity.y = 0;
    }
    _active = activate;
}