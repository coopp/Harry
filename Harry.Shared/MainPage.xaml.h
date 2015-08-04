#pragma once

#include <WindowsNumerics.h>
#include "MainPage.g.h"
#include "Entity.h"
#include "Harry.h"
#include "Sky.h"
#include "HealthBar.h"
#include "ProgressBar.h"
#include "ScrollingBackground.h"

namespace Harry
{
    enum GameState
    {
        Splash,
        Title,
        Loading,
        Paused,
        Playing,
        LevelComplete,
        GameOver,
    };

    enum GameUIButton
    {
        NextLevel,
        ExitToTitle,
        TotalButtons
    };

    struct GameLevel
    {
        float LevelLength;
        int TotalCoins;
        int NumGreenBrontosaurus;
        int NumBrownBrontosaurus;
        // Sprites are specified in z-order and will be randomly placed when
        // the level is created
        std::vector<SimpleGame::Sprite> Layer1;
        std::vector<SimpleGame::Sprite> Layer2;
        std::vector<SimpleGame::Sprite> Layer3;
    };

    public ref class MainPage sealed
    {
    public:
        MainPage();
        virtual ~MainPage();

    private:
        void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnCreateResources(Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::CanvasCreateResourcesEventArgs^ args);
        void OnUpdate(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedUpdateEventArgs^ args);
        void OnDraw(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedDrawEventArgs^ args);
        void OnControlSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
        void OnSizeChanged(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::WindowSizeChangedEventArgs ^args);
        void OnVisibilityChanged(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::VisibilityChangedEventArgs ^args);
        void OnKeyDown(CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args);
        void OnTapped(Windows::Foundation::Numerics::float2 pt);
        void OnPointerPressed(Platform::Object^ sender, Windows::UI::Core::PointerEventArgs^ e);
        void OnPointerMoved(Platform::Object^ sender, Windows::UI::Core::PointerEventArgs^ e);
        void OnPointerReleased(Platform::Object^ sender, Windows::UI::Core::PointerEventArgs^ e);
        void OnGameLoopStarting(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Platform::Object^ args);
        void OnGameLoopStopped(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Platform::Object^ args);
        void OnMapModeTapped(Windows::UI::Input::GestureRecognizer ^sender, Windows::UI::Input::TappedEventArgs ^args);
        void OnMapModeManipulationStarted(Windows::UI::Input::GestureRecognizer ^sender, Windows::UI::Input::ManipulationStartedEventArgs ^args);
        void OnMapModeManipulationUpdated(Windows::UI::Input::GestureRecognizer ^sender, Windows::UI::Input::ManipulationUpdatedEventArgs ^args);
        void OnMapModeManipulationCompleted(Windows::UI::Input::GestureRecognizer ^sender, Windows::UI::Input::ManipulationCompletedEventArgs ^args);

    private:
        void Update(float elapsedTime);
        void UpdateTitleScreen(float elapsedTime);
        void UpdateLoadingScreen(float elapsedTime);
        void UpdateGameplayScreen(float elapsedTime);
        void UpdateLevelCompleteScreen(float elapsedTime);
        void UpdateGameOverScreen(float elapsedTime);
        void UpdatePausedScreen(float elapsedTime);

        void UpdateScore(float elapsedTime);

        void Draw(Microsoft::Graphics::Canvas::CanvasDrawingSession^ ds);
        void DrawTitleScreen();
        void DrawLoadingScreen();
        void DrawGameplayScreen();
        void DrawLevelCompleteScreen();
        void DrawGameOverScreen();
        void DrawPausedScreen();

        void DrawScore();

        void ResizeCanvasControl();
        void LoadTitleScreen();
        void LoadLevel(size_t level);
        void LoadNextLevel();
        void CreateScrollingLayers(size_t levelNumber);

        bool EntityCollisionCheck(SimpleGame::Entity* e1, SimpleGame::Entity* e2);

        void ChangeGameState(GameState state);
        bool IsUIButtonTapped(GameUIButton button, Windows::Foundation::Numerics::float2 point);

        float GetLevelEndPosition(size_t level);

        void DisableInput(float duration);
        void UpdateDisableInputTimer(float elapsedTime);

    private:
        Microsoft::Graphics::Canvas::CanvasRenderTarget^ _renderTarget;
        Windows::Foundation::Rect _renderSize;
        Windows::Foundation::Rect _canvasSize;
        Windows::Foundation::Numerics::float2 _cameraPosition;
        Windows::UI::Core::CoreIndependentInputSource^ _input;
        Windows::UI::Input::GestureRecognizer^ _mapModeInput;
        Windows::Foundation::Size _inputPointScale;
        Windows::Foundation::EventRegistrationToken _inputPressedEventToken;
        Windows::Foundation::EventRegistrationToken _inputMovedEventToken;
        Windows::Foundation::EventRegistrationToken _inputReleasedEventToken;
        bool _mapModeManipulationInProgress;
        std::random_device _random; 
        Concurrency::critical_section _dataCriticalSection;

        std::vector<std::shared_ptr<SimpleGame::Entity>> _entities;
        std::shared_ptr<Harrydactyl> _harry;

        SimpleGame::Sky _sky;
        SimpleGame::ScrollingBackground _layer1;
        SimpleGame::ScrollingBackground _layer2;
        SimpleGame::ScrollingBackground _layer3;
        SimpleGame::HealthBar _healthBar;
        SimpleGame::ProgressBar _distanceBar;
        std::wstring _scoreText;
        std::wstring _levelText;
        std::unique_ptr<SimpleGame::AnimatedSprite> _scoreCoinAnimation;
        std::vector<Windows::Foundation::Rect> _gameButtons;

        GameState _state;
        float _loadingTimer;
        std::vector<GameLevel> _levels;
        size_t _currentLevel;
        bool _mapModeEnabled;
        bool _inputEnabled;
        float _inputEnableTimer;
    };
}
