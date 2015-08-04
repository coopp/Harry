#pragma once

namespace SimpleGame
{
    class AnimatedSprite
    {
    public:
        AnimatedSprite(Sprite firstFrame, int numFrames, float frameDuration, bool horizontallyFlipped);
        void Update(float elapsedTime);
        void Draw(Windows::Foundation::Numerics::float2 position, float darkenAmount);

    private:
        float _timer;
        float _duration;
        int _numFrames;
        int _currentFrame;
        Sprite _firstFrame;
        bool _horizontallyFlipped;
    };
}