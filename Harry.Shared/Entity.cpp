#include "pch.h"
#include "Entity.h"

using namespace SimpleGame;
using namespace Windows::UI;
using namespace Windows::Foundation::Numerics;

Entity::Entity(float2 position, float radius) :
    _position(position),
    _radius(radius),
    _dead(false),
    _collidable(false),
    _pointValue(0)
{

}

void Entity::Update(float elapsedTime)
{
}

void Entity::Draw(float darkenAmount)
{
    GameContext::GetInstance().DrawCircle(_position, _radius, Colors::Yellow);
}

void Entity::Kill()
{
    _dead = true;
}

void Entity::SetPosition(float2 position)
{
    _position = position;
}

float2 Entity::GetPosition()
{
    return _position;
}

float Entity::GetCollisionRadius()
{
    return _radius;
}

bool Entity::IsDead()
{
    return _dead;
}

bool Entity::IsCollidable()
{
    return _collidable;
}

int Entity::GetPoints()
{
    return _pointValue;
}
