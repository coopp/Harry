#pragma once

namespace SimpleGame
{
    enum TimeOfDay
    {
        Sunrise = 0,
        Day = 3,
        Sunset = 6,
        Night = 9,
    };

    struct TimeOfDayDarkness
    {
        TimeOfDay timeOfDay;
        float darkeness;
    };

    class Sky
    {
    public:
        Sky();
        void ChangeTo(TimeOfDay timeOfDay);
        void SetDayNightCycleDuration(float duration);
        void Update(float elapsedTime);
        void Draw();
        TimeOfDay GetTimeOfDay();
        float GetDarkenAmount();

    private:
        Microsoft::Graphics::Canvas::Brushes::CanvasLinearGradientBrush^ _skyBrush;
        std::vector<Windows::UI::Color> _gradients;
        TimeOfDay _timeOfDay;
        TimeOfDay _targetTimeOfDay;
        float _timeOfDayTimer;
        float _cycleTimer;
        float _fullDayNightCycleDuration;
        float _darkenAmount;
        float _timeOfDayLerpValue;
        std::vector<TimeOfDayDarkness> _timeOfDayDarknessValues;
        std::vector<Microsoft::Graphics::Canvas::Brushes::CanvasGradientStop> _stops;
        bool _stopsChanged;
    };
}