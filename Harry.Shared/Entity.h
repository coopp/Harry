#pragma once

namespace SimpleGame
{
    class Entity
    {
    public:
        Entity(Windows::Foundation::Numerics::float2 position, float radius);
        virtual void Update(float elapsedTime);
        virtual void Draw(float darkenAmount);
        virtual void Kill();
        bool IsDead();
        void SetPosition(Windows::Foundation::Numerics::float2 position);
        Windows::Foundation::Numerics::float2 GetPosition();
        float GetCollisionRadius();
        bool IsCollidable();
        int GetPoints();

    protected:
        Windows::Foundation::Numerics::float2 _position;
        float _radius;
        bool _dead;
        bool _collidable;
        int _pointValue;
    };
}