#include "pch.h"

using namespace SimpleGame;
using namespace SimpleGame::Utilities;
using namespace Platform;
using namespace Windows::System::Threading;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI;
using namespace Windows::UI::Text;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Effects;
using namespace Microsoft::Graphics::Canvas::Text;
using namespace Microsoft::Graphics::Canvas::Brushes;
using namespace Microsoft::Graphics::Canvas::UI::Xaml;

GameContext::GameContext() :
    _ds(nullptr),
    _enableDebugDrawing(false)
{
    _renderTransform = float3x2::identity();
    CreateSprites();
}

void GameContext::SetDrawingSession(CanvasDrawingSession^ ds)
{
    _ds = ds;
}

void GameContext::SetRenderTransform(float3x2 transform)
{
    _ds->Transform = transform;
}

void GameContext::CreateResources(CanvasAnimatedControl^ control)
{
    _resourceCreator = control;

    _textFont = ref new CanvasTextFormat();
    _textFont->FontFamily = "Segoe UI";
    _textFont->FontSize = 40;
    _textFont->FontWeight = FontWeights::Bold;
    _textFont->HorizontalAlignment = CanvasHorizontalAlignment::Center;
    _textFont->VerticalAlignment = CanvasVerticalAlignment::Center;

    _symbolFont = ref new CanvasTextFormat();
    _symbolFont->FontFamily = "Symbols.ttf#Symbols";
    _symbolFont->FontSize = 40;
    _symbolFont->FontWeight = FontWeights::Bold;
    _symbolFont->HorizontalAlignment = CanvasHorizontalAlignment::Center;
    _symbolFont->VerticalAlignment = CanvasVerticalAlignment::Center;

    _darkenSpriteEffect = ref new ColorMatrixEffect();
    _flipSpriteEffect = ref new Transform3DEffect();
    _sheets.clear();
    _sheets.resize(TotalSheets);

    _sheets[Sprites1Sheet] = WaitExecution(CanvasBitmap::LoadAsync(_resourceCreator, "sprites1.png"));
    _sheets[Sprites2Sheet] = WaitExecution(CanvasBitmap::LoadAsync(_resourceCreator, "sprites2.png"));
}

CanvasLinearGradientBrush^ GameContext::CreateLinearGradientBrush(std::vector<CanvasGradientStop>& stops)
{
    Platform::Array<CanvasGradientStop>^ stopsArray = ref new Platform::Array<CanvasGradientStop>(stops.data(), static_cast<unsigned int>(stops.size()));
    return ref new CanvasLinearGradientBrush(_resourceCreator, stopsArray);
}

Windows::Foundation::Rect GameContext::GetRenderSize()
{
    return _renderSize;
}

void GameContext::SetRenderSize(Rect renderSize)
{
    _renderSize = renderSize;
}

float GameContext::GetSpriteWidth(Sprite sprite)
{
    return _sprites[sprite].SourceRect.Width;
}

float GameContext::GetSpriteHeight(Sprite sprite)
{
    return _sprites[sprite].SourceRect.Height;
}

void GameContext::DrawCircle(float2 position, float radius, Color color, float strokeWidth)
{
    _ds->DrawCircle(position, radius, color, strokeWidth);
}

void GameContext::DrawRectangle(Windows::Foundation::Rect rect, Windows::UI::Color color, float strokeWidth)
{
    _ds->DrawRectangle(rect, color, strokeWidth);
}

void GameContext::DrawLine(float2 pos1, float2 pos2, Windows::UI::Color color, float strokeWidth)
{
    _ds->DrawLine(pos1, pos2, color, strokeWidth);
}

// Draws an image from a spritesheet using source rectangle information
void GameContext::DrawImage(SpriteSheet sheet, Rect dst, Rect src, bool drawflippedHorizontally, float darkenAmount)
{
    Matrix5x4 colorMatrix = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
        -darkenAmount, -darkenAmount, -darkenAmount, 0,
    };

    _darkenSpriteEffect->ColorMatrix = colorMatrix;
    _darkenSpriteEffect->Source = _sheets[sheet];
    _flipSpriteEffect->Source = _darkenSpriteEffect;
    _flipSpriteEffect->TransformMatrix = float4x4::identity();

    if (drawflippedHorizontally)
    {
        float4x4 matrix = make_float4x4_scale(float3(-1,1,1), float3(src.X + src.Width/2.0f, src.Y, 0));
        _flipSpriteEffect->TransformMatrix = matrix;
        _ds->DrawImage(_flipSpriteEffect, dst, src, 1.0f, CanvasImageInterpolation::NearestNeighbor);
    }
    else
    {
        _ds->DrawImage(_flipSpriteEffect, dst, src, 1.0f, CanvasImageInterpolation::NearestNeighbor);
    }

    if (_enableDebugDrawing)
    {
        _ds->DrawRectangle(dst, Windows::UI::Colors::Red);
    }
}

// Draws a single sprite
void GameContext::DrawSprite(Sprite sprite, float2 position, bool drawCentered, bool drawflippedHorizontally, float darkenAmount)
{
    float spriteWidth = _sprites[sprite].SourceRect.Width;
    float spriteHeight = _sprites[sprite].SourceRect.Height;
    if (drawCentered)
    {
        DrawImage(_sprites[sprite].Sheet, Rect(position.x - spriteWidth / 2.0f, position.y - spriteHeight / 2.0f, spriteWidth, spriteHeight), _sprites[sprite].SourceRect, drawflippedHorizontally, darkenAmount);
    }
    else
    {
        DrawImage(_sprites[sprite].Sheet, Rect(position.x, position.y, spriteWidth, spriteHeight), _sprites[sprite].SourceRect, drawflippedHorizontally, darkenAmount);
    }
}

void GameContext::DrawGradient(CanvasLinearGradientBrush^ brush, Rect dst)
{
    _ds->FillRectangle(dst, brush);
}

// Draws drop shadowed text with the specified size and color
void GameContext::DrawText(const wchar_t* str, float2 position, Color color, float fontSize, bool centered)
{
    _textFont->FontSize = fontSize;
    _textFont->HorizontalAlignment = centered ? CanvasHorizontalAlignment::Center : CanvasHorizontalAlignment::Left;
    _textFont->VerticalAlignment = centered ? CanvasVerticalAlignment::Center : CanvasVerticalAlignment::Top;

    _ds->DrawText(Platform::StringReference(str), float2(position.x + 1, position.y + 1), Colors::Black, _textFont);
    _ds->DrawText(Platform::StringReference(str), position, color, _textFont);
}

// Draws drop shadowed text with the specified size and color
void GameContext::DrawText(const std::wstring& str, float2 position, Color color, float fontSize, bool centered)
{
    DrawText(str.c_str(), position, color, fontSize, centered);
}

// Draws drop a shadowed symbol with the specified size and color
void GameContext::DrawSymbol(TextSymbol symbol, float2 position, Color color, float fontSize, bool centered)
{
    wchar_t str[2] = { static_cast<wchar_t>(symbol), 0 };
    Platform::StringReference symbolText(str);
    _symbolFont->FontSize = fontSize;
    _symbolFont->HorizontalAlignment = centered ? CanvasHorizontalAlignment::Center : CanvasHorizontalAlignment::Left;
    _symbolFont->VerticalAlignment = centered ? CanvasVerticalAlignment::Center : CanvasVerticalAlignment::Top;

    _ds->DrawText(symbolText, position, color, _symbolFont);
}

void GameContext::Clear(Color color)
{
    _ds->Clear(color);
}

void GameContext::EnableDebugDrawing(bool enable)
{
    _enableDebugDrawing = enable;
}

bool GameContext::GetEnableDebugDrawing()
{
    return _enableDebugDrawing;
}

// Initializes all sprite source rectangles and the sprite sheet they are located on
void GameContext::CreateSprites()
{
    _sprites.resize(TotalSprites);

    _sprites[HarryFly00] = { Sprites1Sheet, Rect(0, 0, 64, 64) };
    _sprites[HarryFly01] = { Sprites1Sheet, Rect(64, 0, 64, 64) };
    _sprites[HarryFly02] = { Sprites1Sheet, Rect(128, 0, 64, 64) };
    _sprites[HarryFly03] = { Sprites1Sheet, Rect(192, 0, 64, 64) };
    _sprites[HarryFly04] = { Sprites1Sheet, Rect(256, 0, 64, 64) };
    _sprites[HarryFly05] = { Sprites1Sheet, Rect(320, 0, 64, 64) };
    _sprites[HarryFly06] = { Sprites1Sheet, Rect(384, 0, 64, 64) };
    _sprites[HarryFly07] = { Sprites1Sheet, Rect(448, 0, 64, 64) };
    _sprites[SpinningCoin00] = { Sprites1Sheet, Rect(0, 126, 20, 20) };
    _sprites[SpinningCoin01] = { Sprites1Sheet, Rect(20, 126, 20, 20) };
    _sprites[SpinningCoin02] = { Sprites1Sheet, Rect(40, 126, 20, 20) };
    _sprites[SpinningCoin03] = { Sprites1Sheet, Rect(60, 126, 20, 20) };
    _sprites[SpinningCoin04] = { Sprites1Sheet, Rect(80, 126, 20, 20) };
    _sprites[SpinningCoin05] = { Sprites1Sheet, Rect(100, 126, 20, 20) };
    _sprites[SpinningCoin06] = { Sprites1Sheet, Rect(120, 126, 20, 20) };
    _sprites[SpinningCoin07] = { Sprites1Sheet, Rect(140, 126, 20, 20) };
    _sprites[BeatingHeart00] = { Sprites1Sheet, Rect(164, 128, 16, 16) };
    _sprites[BeatingHeart01] = { Sprites1Sheet, Rect(181, 128, 16, 16) };
    _sprites[Cloud01] = { Sprites1Sheet, Rect(0, 64, 64, 64) };
    _sprites[Cloud02] = { Sprites1Sheet, Rect(64, 64, 32, 32) };
    _sprites[Cloud03] = { Sprites1Sheet, Rect(128, 64, 64, 64) };
    _sprites[Bush01] = { Sprites1Sheet, Rect(0, 148, 70, 56) };
    _sprites[Bush02] = { Sprites1Sheet, Rect(74, 148, 70, 56) };
    _sprites[Bush03] = { Sprites1Sheet, Rect(148, 148, 44, 56) };
    _sprites[Bush04] = { Sprites2Sheet, Rect(0, 454, 70, 56) };
    _sprites[Bush05] = { Sprites2Sheet, Rect(74, 454, 70, 56) };
    _sprites[Bush06] = { Sprites2Sheet, Rect(148, 454, 44, 56) };
    _sprites[Stone01] = { Sprites1Sheet, Rect(194, 190, 30, 14) };
    _sprites[Tree01] = { Sprites1Sheet, Rect(231, 70, 91, 133) };
    _sprites[Tree02] = { Sprites1Sheet, Rect(331, 70, 77, 133) };
    _sprites[BrontosaurusBrown00] = { Sprites1Sheet, Rect(0, 205, 169, 151) };
    _sprites[BrontosaurusBrown01] = { Sprites1Sheet, Rect(170, 205, 169, 151) };
    _sprites[BrontosaurusGreen00] = { Sprites1Sheet, Rect(0, 358, 169, 151) };
    _sprites[BrontosaurusGreen01] = { Sprites1Sheet, Rect(170, 358, 169, 151) };
    _sprites[BrontosaurusGreen02] = { Sprites1Sheet, Rect(341, 358, 169, 151) };
    _sprites[MountainBackground01] = { Sprites2Sheet, Rect(0, 0, 400, 112) };
    _sprites[DesertBackground01] = { Sprites2Sheet, Rect(0, 112, 400, 112) };
    _sprites[LavaBackground01] = { Sprites2Sheet, Rect(0, 224, 400, 112) };
    _sprites[BackgroundLayer1_1] = { BackgroundLayer1_1Sheet, Rect(0,0,400,240) };
    _sprites[BackgroundLayer1_2] = { BackgroundLayer1_2Sheet, Rect(0,0,400,240) };
    _sprites[BackgroundLayer1_3] = { BackgroundLayer1_3Sheet, Rect(0,0,400,240) };
    _sprites[BackgroundLayer2_1] = { BackgroundLayer2_1Sheet, Rect(0,0,400,240) };
    _sprites[BackgroundLayer2_2] = { BackgroundLayer2_2Sheet, Rect(0,0,400,240) };
    _sprites[BackgroundLayer2_3] = { BackgroundLayer2_3Sheet, Rect(0,0,400,240) };
    _sprites[BackgroundLayer3_1] = { BackgroundLayer3_1Sheet, Rect(0,0,400,240) };
    _sprites[BackgroundLayer3_2] = { BackgroundLayer3_2Sheet, Rect(0,0,400,240) };
    _sprites[BackgroundLayer3_3] = { BackgroundLayer3_3Sheet, Rect(0,0,400,240) };
}

// Draws a sprite at a random location within the specified bounds.  A sprite will never be
// clipped or rendered outside of the bounds.
void GameContext::DrawSpriteRandom(Sprite sprite, Rect bounds, CanvasDrawingSession^ ds, float scale)
{
    std::random_device rd;
    std::mt19937 eng(rd());

    Rect dstRect;
    do
    {
        std::uniform_int_distribution<> spriteX(static_cast<int>(floorf(bounds.Left)), static_cast<int>(floorf(bounds.Left + bounds.Right)));
        std::uniform_int_distribution<> spriteY(static_cast<int>(floorf(bounds.Top)), static_cast<int>(floorf(bounds.Top + bounds.Bottom)));
        dstRect = Rect(static_cast<float>(spriteX(eng)), static_cast<float>(spriteY(eng)), _sprites[sprite].SourceRect.Width, _sprites[sprite].SourceRect.Height);
    } while ((!bounds.Contains(Point(dstRect.X, dstRect.Y)) || !bounds.Contains(Point(dstRect.X + dstRect.Width, dstRect.Y + dstRect.Height))));

    dstRect = Rect(dstRect.X, dstRect.Y, dstRect.Width * scale, dstRect.Height * scale);
    ds->DrawImage(_sheets[_sprites[sprite].Sheet], dstRect, _sprites[sprite].SourceRect, 1.0f, CanvasImageInterpolation::NearestNeighbor);
}

// Creates a spritesheet layer from a z-ordered sprite list.  Sprites are randomly
// placed on the layer based on the sprite's bounds. See CreateSprites( ) for layer
// sizes. This method is only used for background layer sprites.
void GameContext::CreateLayer(Sprite layer, std::vector<Sprite> sprites, bool addClouds)
{
    CanvasRenderTarget^ renderTarget = ref new CanvasRenderTarget(_resourceCreator, _sprites[layer].SourceRect.Width, _sprites[layer].SourceRect.Height, 96.0f);
    CanvasDrawingSession^ ds = renderTarget->CreateDrawingSession();
    ds->Clear(Windows::UI::Colors::Transparent);

    if (addClouds)
    {
        struct CloudSet
        {
            int count;
            float yPostion;
            float scale;
        };

        CloudSet cloudSets[3] = { {5, 16, 1.0f}, { 6, 60, 0.5f }, { 8, 100, 0.25f } };
        for (int cloudSet = 0; cloudSet < _countof(cloudSets); cloudSet++)
        {
            for (int cloud = 0; cloud < cloudSets[cloudSet].count; cloud++)
            {
                DrawSpriteRandom(Cloud01, Rect(0, cloudSets[cloudSet].yPostion, _sprites[layer].SourceRect.Width, _sprites[layer].SourceRect.Height / 2.0f), ds, cloudSets[cloudSet].scale);
            }
        }
    }

    for (auto sprite : sprites)
    {
        DrawSpriteRandom(sprite, Rect(0, _sprites[layer].SourceRect.Height - GetSpriteHeight(sprite), _sprites[layer].SourceRect.Width, GetSpriteHeight((sprite))), ds);
    }

    delete ds;

    _sheets[_sprites[layer].Sheet] = renderTarget;
}