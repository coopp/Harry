#include "pch.h"
#include "Sky.h"

using namespace SimpleGame;
using namespace SimpleGame::Utilities;
using namespace Platform;
using namespace Windows::UI;
using namespace Windows::Foundation::Numerics;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Brushes;

Sky::Sky() :
    _timeOfDayTimer(0.0f),
    _cycleTimer(0.0f),
    _fullDayNightCycleDuration(60.0f),
    _timeOfDay(Day),
    _targetTimeOfDay(Day),
    _darkenAmount(0.0f),
    _stopsChanged(false)
{
    // Sunrise
    _gradients.push_back(ColorHelper::FromArgb(255, 16, 81, 171));
    _gradients.push_back(ColorHelper::FromArgb(255, 165, 74, 176));
    _gradients.push_back(ColorHelper::FromArgb(255, 209, 124, 86));
    _timeOfDayDarknessValues.push_back({ TimeOfDay::Sunrise, 0.15f });

    // Day
    _gradients.push_back(ColorHelper::FromArgb(255, 116, 225, 255));
    _gradients.push_back(ColorHelper::FromArgb(255, 123, 219, 255));
    _gradients.push_back(ColorHelper::FromArgb(255, 98, 163, 253));
    _timeOfDayDarknessValues.push_back({ TimeOfDay::Day, 0.0f });

    // Sunset
    _gradients.push_back(ColorHelper::FromArgb(255, 16, 81, 171));
    _gradients.push_back(ColorHelper::FromArgb(255, 165, 74, 176));
    _gradients.push_back(ColorHelper::FromArgb(255, 209, 124, 86));
    _timeOfDayDarknessValues.push_back({ TimeOfDay::Sunset, 0.20f });

    // Night
    _gradients.push_back(ColorHelper::FromArgb(255, 0, 3, 13));
    _gradients.push_back(ColorHelper::FromArgb(255, 0, 10, 68));
    _gradients.push_back(ColorHelper::FromArgb(255, 0, 34, 106));
    _timeOfDayDarknessValues.push_back({ TimeOfDay::Day, 0.70f });

    _stops.resize(3);
}

void Sky::ChangeTo(TimeOfDay timeOfDay)
{
    if (timeOfDay != _timeOfDay && timeOfDay != _targetTimeOfDay)
    {
        _targetTimeOfDay = timeOfDay;
        _timeOfDayTimer = 0.0f;
    }
}

void Sky::Update(float elapsedTime)
{
    // If the time of day timer has reached its goal
    // then cycle to the next time of day.
    if (_timeOfDayTimer >= 3.0f)
    {
        _cycleTimer += elapsedTime;
        if (_cycleTimer > _fullDayNightCycleDuration / 4.0f)
        {
            _cycleTimer = 0.0f;
            switch (_timeOfDay)
            {
            case Day:
                ChangeTo(Sunset);
                break;
            case Night:
                ChangeTo(Sunrise);
                break;
            case Sunrise:
                ChangeTo(Day);
                break;
            case Sunset:
                ChangeTo(Night);
                break;
            }
            return;
        }
    }

    // If the time of day timer is in progress
    // create the new sky colors and continue.
    if (_timeOfDayTimer < 3.0f)
    {
        _timeOfDayTimer += elapsedTime;
        if (_timeOfDayTimer > 3.0f)
        {
            // Time of day has reached its target, so update them to match
            _timeOfDayTimer = 3.0f;
            _timeOfDay = _targetTimeOfDay;
        }

        _timeOfDayLerpValue = _timeOfDayTimer / 3.0f;

        // Calculate a darken amount which can be used for rendering sprites
        // with matching lighting effects.
        _darkenAmount = lerp(_timeOfDayDarknessValues[_timeOfDay / 3].darkeness, _timeOfDayDarknessValues[_targetTimeOfDay / 3].darkeness, _timeOfDayLerpValue);

        _stops[0].Position = 0.0f;
        _stops[1].Position = 0.5f;
        _stops[2].Position = 1.0f;
        for (size_t i = 0; i < _stops.size(); i++)
        {
            Windows::Foundation::Numerics::float4 c1(_gradients[_timeOfDay + i].A, _gradients[_timeOfDay + i].R, _gradients[_timeOfDay + i].G, _gradients[_timeOfDay + i].B);
            Windows::Foundation::Numerics::float4 c2(_gradients[_targetTimeOfDay + i].A, _gradients[_targetTimeOfDay + i].R, _gradients[_targetTimeOfDay + i].G, _gradients[_targetTimeOfDay + i].B);
            Windows::Foundation::Numerics::float4 finalColor = lerp(c1, c2, _timeOfDayLerpValue);
            _stops[i].Color = ColorHelper::FromArgb(static_cast<unsigned char>(finalColor.x), static_cast<unsigned char>(finalColor.y), static_cast<unsigned char>(finalColor.z), static_cast<unsigned char>(finalColor.w));
        }
        _stopsChanged = true;
    }
    else
    {
        // Time of day has reached its target, so update them to match
        _timeOfDay = _targetTimeOfDay;
        _stopsChanged = false;
    }
}

void Sky::Draw()
{
    GameContext& context = GameContext::GetInstance();
    if (_stopsChanged)
    {
        _skyBrush = context.CreateLinearGradientBrush(_stops);
        _skyBrush->EndPoint = float2(0, context.GetRenderSize().Height);
    }
    context.DrawGradient(_skyBrush, context.GetRenderSize());
}

TimeOfDay Sky::GetTimeOfDay()
{
    return _timeOfDay;
}

float Sky::GetDarkenAmount()
{
    return _darkenAmount;
}