#include "pch.h"
#include "HealthBar.h"

using namespace SimpleGame;
using namespace Windows::Foundation::Numerics;

HealthBar::HealthBar() :
    _healthyHearts(0)
{

}

void HealthBar::Update(float elapsedTime, int health)
{
    if (health != _healthyHearts)
    {
        _healthyHearts = health;
        _hearts.clear();
        for (int i = 0; i < _healthyHearts; i++)
        {
            _hearts.push_back(std::shared_ptr<AnimatedSprite>(new AnimatedSprite(BeatingHeart00, 2, 0.25f, false)));
        }
    }

    for (auto heart : _hearts)
    {
        heart->Update(elapsedTime);
    }
}

void HealthBar::Draw(float2 position)
{
    float spacing = GameContext::GetInstance().GetSpriteWidth(BeatingHeart00) + 2;
    float2 pos = position;
    pos.x -= (_hearts.size() / 2 * spacing);
    for (auto heart : _hearts)
    {
        heart->Draw(pos, 0.0f);
        pos.x += spacing;
    }
}