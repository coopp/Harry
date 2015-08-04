#include "pch.h"
#include "AnimatedSprite.h"

using namespace SimpleGame;
using namespace Windows::Foundation::Numerics;

AnimatedSprite::AnimatedSprite(Sprite firstFrame, int numFrames, float frameDuration, bool horizontallyFlipped) :
    _firstFrame(firstFrame),
    _numFrames(numFrames),
    _duration(frameDuration),
    _currentFrame(0),
    _timer(0),
    _horizontallyFlipped(horizontallyFlipped)
{

}

void AnimatedSprite::Update(float elapsedTime)
{
    _timer += elapsedTime;
    if (_timer > _duration)
    {
        _timer = 0.0f;
        _currentFrame++;

        if (_currentFrame > (_numFrames - 1))
        {
            _currentFrame = 0;
        }
    }
}

void AnimatedSprite::Draw(float2 position, float darkenAmount)
{
    GameContext& context = GameContext::GetInstance();
    context.DrawSprite((Sprite)(_firstFrame + _currentFrame), position, true, _horizontallyFlipped, darkenAmount);
}
