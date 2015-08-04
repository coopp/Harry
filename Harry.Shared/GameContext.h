#pragma once

namespace SimpleGame
{
    // Symbols to be rendered from Symbols.ttf
    enum TextSymbol
    {
        PlaySymbol = 0xE102,
        AcceptSymbol = 0xE10B,
        CancelSymbol = 0xE10A,
        MuteSymbol = 0xE198,
        UnmuteSymbol = 0xE15D
    };

    // All sprite sheets in the game
    enum SpriteSheet
    {
        Sprites1Sheet,
        Sprites2Sheet,
        BackgroundLayer1_1Sheet,
        BackgroundLayer1_2Sheet,
        BackgroundLayer1_3Sheet,
        BackgroundLayer2_1Sheet,
        BackgroundLayer2_2Sheet,
        BackgroundLayer2_3Sheet,
        BackgroundLayer3_1Sheet,
        BackgroundLayer3_2Sheet,
        BackgroundLayer3_3Sheet,
        TotalSheets,
    };

    // All sprites in the game
    enum Sprite
    {
        HarryFly00,
        HarryFly01,
        HarryFly02,
        HarryFly03,
        HarryFly04,
        HarryFly05,
        HarryFly06,
        HarryFly07,
        SpinningCoin00,
        SpinningCoin01,
        SpinningCoin02,
        SpinningCoin03,
        SpinningCoin04,
        SpinningCoin05,
        SpinningCoin06,
        SpinningCoin07,
        BeatingHeart00,
        BeatingHeart01,
        Cloud01,
        Cloud02,
        Cloud03,
        Bush01,
        Bush02,
        Bush03,
        Bush04,
        Bush05,
        Bush06,
        Stone01,
        Tree01,
        Tree02,
        BrontosaurusBrown00,
        BrontosaurusBrown01,
        BrontosaurusGreen00,
        BrontosaurusGreen01,
        BrontosaurusGreen02,
        MountainBackground01,
        DesertBackground01,
        LavaBackground01,
        BackgroundLayer1_1,
        BackgroundLayer1_2,
        BackgroundLayer1_3,
        BackgroundLayer2_1,
        BackgroundLayer2_2,
        BackgroundLayer2_3,
        BackgroundLayer3_1,
        BackgroundLayer3_2,
        BackgroundLayer3_3,
        TotalSprites,
    };

    struct SpriteInfo
    {
        SpriteSheet Sheet;
        Windows::Foundation::Rect SourceRect;
    };

    class GameContext
    {
    public:
        static GameContext& GetInstance()
        {
            static GameContext _instance;
            return _instance;
        }

        void SetDrawingSession(Microsoft::Graphics::Canvas::CanvasDrawingSession^ ds);
        void SetRenderTransform(Windows::Foundation::Numerics::float3x2 transform);
        void CreateResources(Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedControl^ control);
        void CreateLayer(Sprite layer, std::vector<Sprite> sprites, bool addClouds = false);
        Microsoft::Graphics::Canvas::Brushes::CanvasLinearGradientBrush^ CreateLinearGradientBrush(std::vector<Microsoft::Graphics::Canvas::Brushes::CanvasGradientStop>& stops);

        Windows::Foundation::Rect GetRenderSize();
        void SetRenderSize(Windows::Foundation::Rect renderSize);

        float GetSpriteWidth(Sprite sprite);
        float GetSpriteHeight(Sprite sprite);

        void DrawCircle(Windows::Foundation::Numerics::float2 position, float radius, Windows::UI::Color color, float strokeWidth = 1.0f);
        void DrawRectangle(Windows::Foundation::Rect rect, Windows::UI::Color color, float strokeWidth = 1.0f);
        void DrawLine(float2 pos1, float2 pos2, Windows::UI::Color color, float strokeWidth = 1.0f);
        void DrawImage(SpriteSheet sheet, Windows::Foundation::Rect dst, Windows::Foundation::Rect src, bool drawflippedHorizontally = false, float darkenAmount = 0.0f);
        void DrawSprite(Sprite sprite, Windows::Foundation::Numerics::float2 position, bool drawCentered = true, bool drawflippedHorizontally = false, float darkenAmount = 0.0f);
        void DrawGradient(Microsoft::Graphics::Canvas::Brushes::CanvasLinearGradientBrush^ brush, Windows::Foundation::Rect dst);
        void DrawText(const std::wstring& str, Windows::Foundation::Numerics::float2 position, Windows::UI::Color color, float fontSize, bool centered = false);
        void DrawText(const wchar_t* str, Windows::Foundation::Numerics::float2 position, Windows::UI::Color color, float fontSize, bool centered = false);
        void DrawSymbol(TextSymbol symbol, Windows::Foundation::Numerics::float2 position, Windows::UI::Color color, float fontSize, bool centered = false);
        void Clear(Windows::UI::Color color);
        void EnableDebugDrawing(bool enable);
        bool GetEnableDebugDrawing();

    private:
        void CreateSprites();
        void DrawSpriteRandom(Sprite sprite, Windows::Foundation::Rect bounds, Microsoft::Graphics::Canvas::CanvasDrawingSession^ ds, float scale = 1.0f);

    private:
        Microsoft::Graphics::Canvas::CanvasDrawingSession^ _ds;
        Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedControl^ _resourceCreator;
        Windows::Foundation::Numerics::float3x2 _renderTransform;
        Windows::Foundation::Rect _renderSize;

        std::vector<SpriteInfo> _sprites;
        std::vector<Microsoft::Graphics::Canvas::CanvasBitmap^> _sheets;
        Microsoft::Graphics::Canvas::Text::CanvasTextFormat^ _textFont;
        Microsoft::Graphics::Canvas::Text::CanvasTextFormat^ _symbolFont;
        Microsoft::Graphics::Canvas::Effects::ColorMatrixEffect^ _darkenSpriteEffect;
        Microsoft::Graphics::Canvas::Effects::Transform3DEffect^ _flipSpriteEffect;
        bool _enableDebugDrawing;

    private:
        GameContext();
        GameContext(GameContext const&) = delete;
        void operator=(GameContext const&) = delete;
    };
}