#pragma once

namespace SimpleGame
{
    class ScrollingBackground
    {
    public:
        ScrollingBackground();
        void SetScrollRate(float scrollRate);
        void AddLayer(Sprite layer);
        void Clear();
        void Draw(Windows::Foundation::Numerics::float2 cameraPosition, float darkenAmount);
    private:
        float _scrollRate;
        float _layerWidth;
        float _layerHeight;
        std::vector<Sprite> _layers;
    };
}