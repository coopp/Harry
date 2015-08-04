#include "pch.h"
#include "MainPage.xaml.h"
#include "SpriteEntity.h"

using namespace Harry;
using namespace SimpleGame;
using namespace SimpleGame::Utilities;

using namespace Platform;
using namespace Concurrency;
using namespace Windows::System::Threading;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Text;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Text;
using namespace Microsoft::Graphics::Canvas::Brushes;

MainPage::MainPage() :
    _state(GameState::Splash),
    _loadingTimer(0.0f),
    _mapModeManipulationInProgress(false),
    _mapModeEnabled(false),
    _currentLevel(1),
    _inputEnabled(true),
    _inputEnableTimer(0.0f),
    _distanceBar(52, Colors::Green, Colors::DarkOrange)
{
    InitializeComponent();

#if (WINAPI_FAMILY != WINAPI_FAMILY_PHONE_APP)
    auto pointerVisualizationSettings = PointerVisualizationSettings::GetForCurrentView();
    pointerVisualizationSettings->IsContactFeedbackEnabled = false;
    pointerVisualizationSettings->IsBarrelButtonFeedbackEnabled = false;
#endif

    CoreWindow^ window = Window::Current->CoreWindow;
    window->SizeChanged += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::WindowSizeChangedEventArgs ^>(this, &MainPage::OnSizeChanged);
    window->VisibilityChanged += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::VisibilityChangedEventArgs ^>(this, &MainPage::OnVisibilityChanged);
    window->KeyDown += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::KeyEventArgs ^>(this, &MainPage::OnKeyDown);

    _renderSize = Rect(0, 0, 400, 240);
    _canvasSize = _renderSize;
    _cameraPosition = float2(_renderSize.Width/2, _renderSize.Height/2);
    _gameButtons.resize(TotalButtons);
    _gameButtons[GameUIButton::NextLevel] = Rect(284, 124, 100, 100);
    _gameButtons[GameUIButton::ExitToTitle] = Rect(16, 124, 100, 100);

    GameContext& context = GameContext::GetInstance();
    context.SetRenderSize(_renderSize);
}

MainPage::~MainPage()
{
    _input->Dispatcher->StopProcessEvents();
}

void MainPage::OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    ResizeCanvasControl();
}

void MainPage::OnCreateResources(Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::CanvasCreateResourcesEventArgs^ args)
{
    _renderTarget = ref new CanvasRenderTarget(sender, _renderSize.Width, _renderSize.Height, 96.0f);
    GameContext::GetInstance().CreateResources(sender);

    // TODO: Load level data from file

    // Level 1
    // Tall and short trees, lots of bushes and all green brontosaurus.
    _levels.push_back(
    {
        10000, 150,
        12, 0,
        std::vector<Sprite>({ MountainBackground01, Tree02, Tree02, Bush03, Tree02 }),
        std::vector<Sprite>({ Bush03, Tree02, Tree01, Tree01, Tree01, Tree01, Bush03, Tree01, Tree01, Bush03, Tree02, Tree01 }),
        std::vector<Sprite>({ Bush01, Bush02, Bush02, Bush02, Bush03, Stone01, Bush03, Stone01, Tree01 })
    });

    // Level 2
    // Desert environment with short trees, few bushes and mostly brown brontosaurus with a few green brontosaurus.
    _levels.push_back(
    {
        10000, 150,
        3, 9,
        std::vector<Sprite>({ DesertBackground01, Bush06, Bush06}),
        std::vector<Sprite>({ Bush05, Bush03, Tree02, Bush04, Bush03, Bush06 }),
        std::vector<Sprite>({ Bush02, Bush04, Bush03, Stone01, Bush05, Stone01 })
    });

    // Level 3
    // Lava environment, nothing..
    _levels.push_back(
    {
        10000, 150,
        0, 0,
        std::vector<Sprite>({ LavaBackground01 }),
        std::vector<Sprite>({ }),
        std::vector<Sprite>({ })
    });
}

void MainPage::OnUpdate(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedUpdateEventArgs^ args)
{
    Update(args->Timing.ElapsedTime.Duration / 10000000.0f /*elapsed time in seconds*/);
}

void MainPage::OnDraw(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedDrawEventArgs^ args)
{
    if (_renderTarget)
    {
        CanvasDrawingSession^ ds = _renderTarget->CreateDrawingSession();
        Draw(ds);
        delete ds;
    }

    args->DrawingSession->DrawImage(_renderTarget, _canvasSize, _renderSize, 1.0f, CanvasImageInterpolation::NearestNeighbor);
}

void MainPage::OnControlSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
    _canvasSize.Width = e->NewSize.Width;
    _canvasSize.Height = e->NewSize.Height;
}

void MainPage::OnSizeChanged(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::WindowSizeChangedEventArgs ^args)
{
    ResizeCanvasControl();
}

void MainPage::OnVisibilityChanged(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::VisibilityChangedEventArgs ^args)
{
    canvasControl->Paused = !args->Visible;
    if (canvasControl->Paused && _state == GameState::Playing)
    {
        ChangeGameState(GameState::Paused);
    }
}

void MainPage::OnKeyDown(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args)
{
#ifdef _DEBUG
    GameContext& context = GameContext::GetInstance();

    if (args->VirtualKey == Windows::System::VirtualKey::Space)
    {
        _mapModeEnabled = !_mapModeEnabled;
        if (_harry)
        {
            _harry->Activate(!_mapModeEnabled);
            _harry->SetPosition(float2(200, 100));
        }
    }

    if (args->VirtualKey == Windows::System::VirtualKey::Escape)
    {
       context.EnableDebugDrawing(!context.GetEnableDebugDrawing());
    }
#endif
}

void MainPage::OnTapped(float2 pt)
{
    if (!_inputEnabled)
    {
        return;
    }

    critical_section::scoped_lock lock(_dataCriticalSection);

    switch (_state)
    {
    case GameState::Title:
        ChangeGameState(GameState::Loading);
        break;
    case GameState::Playing:
        _harry->Tap();
        break;
    case GameState::Paused:
        ChangeGameState(GameState::Playing);
        break;
    case GameState::LevelComplete:
        if (IsUIButtonTapped(GameUIButton::NextLevel, pt))
        {
            LoadNextLevel();
        }
        else if (IsUIButtonTapped(GameUIButton::ExitToTitle, pt))
        {
            ChangeGameState(GameState::Title);
        }
        break;
    case GameState::GameOver:
        ChangeGameState(GameState::Title);
        break;
    default:
        break;
    }
}

void MainPage::OnPointerPressed(Object^ sender, PointerEventArgs^ e)
{
    _mapModeInput->ProcessDownEvent(e->CurrentPoint);
    if (!_mapModeEnabled || _state == GameState::Title)
    {
        Point pt = e->CurrentPoint->Position;
        OnTapped(float2(pt.X * _inputPointScale.Width, pt.Y * _inputPointScale.Height));
    }
}

void MainPage::OnPointerMoved(Object^ sender, PointerEventArgs^ e)
{
    _mapModeInput->ProcessMoveEvents(e->GetIntermediatePoints());
}

void MainPage::OnPointerReleased(Object^ sender, PointerEventArgs^ e)
{
    _mapModeInput->ProcessUpEvent(e->CurrentPoint);
}

void MainPage::OnGameLoopStarting(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Platform::Object^ args)
{
    _input = canvasControl->CreateCoreIndependentInputSource(
        Windows::UI::Core::CoreInputDeviceTypes::Mouse |
        Windows::UI::Core::CoreInputDeviceTypes::Touch
        );

    _inputPressedEventToken = _input->PointerPressed += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &MainPage::OnPointerPressed);
    _inputMovedEventToken = _input->PointerMoved += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &MainPage::OnPointerMoved);
    _inputReleasedEventToken = _input->PointerReleased += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &MainPage::OnPointerReleased);

    _mapModeInput = ref new GestureRecognizer();
    _mapModeInput->GestureSettings = GestureSettings::Tap |
                                     GestureSettings::DoubleTap |
                                     GestureSettings::ManipulationTranslateX |
                                     GestureSettings::ManipulationTranslateY |
                                     GestureSettings::ManipulationTranslateInertia;
    _mapModeInput->InertiaTranslationDeceleration = -0.05f;
    _mapModeInput->AutoProcessInertia = false;

    _mapModeInput->Tapped += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Input::GestureRecognizer ^, Windows::UI::Input::TappedEventArgs ^>(this, &MainPage::OnMapModeTapped);
    _mapModeInput->ManipulationStarted += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Input::GestureRecognizer ^, Windows::UI::Input::ManipulationStartedEventArgs ^>(this, &MainPage::OnMapModeManipulationStarted);
    _mapModeInput->ManipulationUpdated += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Input::GestureRecognizer ^, Windows::UI::Input::ManipulationUpdatedEventArgs ^>(this, &MainPage::OnMapModeManipulationUpdated);
    _mapModeInput->ManipulationCompleted += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Input::GestureRecognizer ^, Windows::UI::Input::ManipulationCompletedEventArgs ^>(this, &MainPage::OnMapModeManipulationCompleted);
}

void MainPage::OnGameLoopStopped(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Platform::Object^ args)
{
    _input->PointerPressed -= _inputPressedEventToken;
    _input->PointerMoved -= _inputMovedEventToken;
    _input->PointerReleased -= _inputReleasedEventToken;
}

void MainPage::OnMapModeTapped(Windows::UI::Input::GestureRecognizer ^sender, Windows::UI::Input::TappedEventArgs ^args)
{
    // Double tap in map mode will switch to the next level
    if (_mapModeEnabled && args->TapCount >= 2)
    {
        LoadNextLevel();
    }
}

void MainPage::OnMapModeManipulationStarted(Windows::UI::Input::GestureRecognizer ^sender, Windows::UI::Input::ManipulationStartedEventArgs ^args)
{
    _mapModeManipulationInProgress = true;
}

void MainPage::OnMapModeManipulationUpdated(Windows::UI::Input::GestureRecognizer ^sender, Windows::UI::Input::ManipulationUpdatedEventArgs ^args)
{
    _cameraPosition.x -= args->Delta.Translation.X * _inputPointScale.Width;
    if (_cameraPosition.x < 0.0f)
    {
        _cameraPosition.x = 0.0f;
    }
    if (_cameraPosition.x > GetLevelEndPosition(_currentLevel))
    {
        _cameraPosition.x = GetLevelEndPosition(_currentLevel);
    }
}

void MainPage::OnMapModeManipulationCompleted(Windows::UI::Input::GestureRecognizer ^sender, Windows::UI::Input::ManipulationCompletedEventArgs ^args)
{
    _mapModeManipulationInProgress = false;
}

void MainPage::Update(float elapsedTime)
{
    UpdateDisableInputTimer(elapsedTime);

    if (_state == GameState::Splash)
    {
        ChangeGameState(Title);
    }

    if (_mapModeInput && _mapModeManipulationInProgress)
    {
        _mapModeInput->ProcessInertia();
    }

    critical_section::scoped_lock lock(_dataCriticalSection);
    switch (_state)
    {
    case GameState::Loading:
        UpdateLoadingScreen(elapsedTime);
        break;
    case GameState::Title:
        UpdateTitleScreen(elapsedTime);
        break;
    case GameState::Playing:
        UpdateGameplayScreen(elapsedTime);
        break;
    case GameState::LevelComplete:
        UpdateLevelCompleteScreen(elapsedTime);
        break;
    case GameState::Paused:
        UpdatePausedScreen(elapsedTime);
        break;
    case GameState::GameOver:
        UpdateGameOverScreen(elapsedTime);
        break;
    default:
        break;
    }
}

void MainPage::UpdateTitleScreen(float elapsedTime)
{
    const float titleScrollSpeed = 200.0f;
    // The title screen is just moving the camera at a constant speed to make
    // an animated moving background.
    _sky.Update(elapsedTime);
    _cameraPosition.x -= titleScrollSpeed * elapsedTime;
}

void MainPage::UpdateLoadingScreen(float elapsedTime)
{
    // We are pretending here that loading is taking a while so we can show
    // a cool loading screen. This also allows the sky to transition to day
    // day for the start of the level.
    _loadingTimer += elapsedTime;
    _sky.ChangeTo(Day);
    _sky.Update(elapsedTime);

    // If the sky is transitioned to day and the loading timer has expired
    // then load the level.
    if (_mapModeEnabled || (_loadingTimer > 2.0f && _sky.GetTimeOfDay() == Day))
    {
        _loadingTimer = 0.0f;
        LoadLevel(_currentLevel);
    }
}

void MainPage::UpdateGameplayScreen(float elapsedTime)
{
    GameContext& context = GameContext::GetInstance();
    _sky.Update(elapsedTime);
    float2 harryPos = _harry->GetPosition();

    for (auto entity : _entities)
    {
        // Update the entity
        entity->Update(elapsedTime);

        // Check for collisions with Harry
        if (entity->IsCollidable() && EntityCollisionCheck(_harry.get(), entity.get()))
        {
            int points = entity->GetPoints();

            // If Harry collides with an entity that gives points,
            // give the points to him and kill the entity to remove
            // it from play.
            if (points > 0)
            {
                entity->Kill();
                _harry->GivePoints(points);
            }
            // If Harry hits an entity that isn't a points giving
            // entity, then take one life.
            else
            {
                _harry->Hurt();
            }
            break;
        }
    }

    UpdateScore(elapsedTime);

    // If Harry hits the ground, then he
    // will take one life
    if (harryPos.y > _renderSize.Height)
    {
        _harry->Hurt();
        _harry->FreeFall();
        _harry->Tap();
    }

    // If Harry attempts to fly too high
    // then force him to stay within the
    // screen bounds
    if (harryPos.y < 0)
    {
        harryPos.y = 0;
        _harry->SetPosition(harryPos);
        _harry->FreeFall();
    }

    // If Harry reaches the end of the level
    // then the level is complete.
    if (harryPos.x > GetLevelEndPosition(_currentLevel))
    {
        ChangeGameState(LevelComplete);
    }

    // Clean up dead entites
    _entities.erase(std::remove_if(_entities.begin(), _entities.end(), [](std::shared_ptr<Entity> e) { return e->IsDead(); }), _entities.end());

    // If Harry is dead (all lives are used up), then
    // the game is over.
    if (_harry->IsDead())
    {
        ChangeGameState(GameOver);
    }
}

void MainPage::UpdateLevelCompleteScreen(float elapsedTime)
{
    if (!_inputEnabled && _inputEnableTimer <= 0.0f)
    {
        _inputEnableTimer += elapsedTime;
        if (_inputEnableTimer > 2.0f)
        {
            _inputEnabled = true;
        }
    }
}

void MainPage::UpdateGameOverScreen(float elapsedTime)
{

}

void MainPage::UpdatePausedScreen(float elapsedTime)
{

}

void MainPage::UpdateScore(float elapsedTime)
{
    _scoreCoinAnimation->Update(elapsedTime);
    _healthBar.Update(elapsedTime, _harry->GetHealth());
    _distanceBar.Update(_mapModeEnabled ? _cameraPosition.x : _harry->GetPosition().x, GetLevelEndPosition(_currentLevel));
    strprintf(_scoreText, L"x %d", _harry->GetPoints());
    strprintf(_levelText, L"Level %d", _currentLevel);
}

void MainPage::Draw(CanvasDrawingSession^ ds)
{
    GameContext& context = GameContext::GetInstance();
    context.SetDrawingSession(ds);
    context.SetRenderTransform(float3x2::identity());

    _sky.Draw();

    critical_section::scoped_lock lock(_dataCriticalSection);
    switch (_state)
    {
    case GameState::Loading:
        DrawLoadingScreen();
        break;
    case GameState::Title:
        DrawTitleScreen();
        break;
    case GameState::Playing:
        DrawGameplayScreen();
        break;
    case GameState::LevelComplete:
        DrawLevelCompleteScreen();
        break;
    case GameState::Paused:
        DrawPausedScreen();
        break;
    case GameState::GameOver:
        DrawGameOverScreen();
        break;
    default:
        break;
    }
}

void MainPage::DrawTitleScreen()
{
    GameContext& context = GameContext::GetInstance();

    // Draw scrolling background
    _layer1.Draw(_cameraPosition, _sky.GetDarkenAmount());
    _layer2.Draw(_cameraPosition, _sky.GetDarkenAmount());
    _layer3.Draw(_cameraPosition, _sky.GetDarkenAmount());

    // Draw title
    context.DrawText(L"Harrydactyl", float2(_renderSize.Width / 2, 50), Windows::UI::Colors::White, 60, true);

    // Draw play button
    float2 pos(_renderSize.Width / 2.0f, _renderSize.Height - 80);
    float2 shadowPos(pos.x + 1, pos.y + 1);
    context.DrawCircle(shadowPos, 24, Windows::UI::Colors::Black, 4);
    context.DrawSymbol(PlaySymbol, shadowPos, Colors::Black, 40, true);
    context.DrawCircle(pos, 24, Windows::UI::Colors::White, 4);
    context.DrawSymbol(PlaySymbol, pos, Colors::White, 40, true);
}

void MainPage::DrawLoadingScreen()
{
    std::wstring loadingText;
    strprintf(loadingText, L"Level %d...", _currentLevel);
    GameContext::GetInstance().DrawText(loadingText, float2(_renderSize.Width / 2.0f, _renderSize.Height / 2.0f), Colors::White, 40, true);
}

void MainPage::DrawGameplayScreen()
{
    GameContext& context = GameContext::GetInstance();
    float darkenAmount = _sky.GetDarkenAmount();

    // If harry is active, then use his position to 
    // as the camera's position
    if (_harry && _harry->IsHarryActive())
    {
        _cameraPosition = _harry->GetPosition();
    }

    // Snap to a whole pixel value
    _cameraPosition.x = ceilf(_cameraPosition.x);
    _cameraPosition.y = ceilf(_cameraPosition.y);

    // Draw the first two scrolling background layers behind the entities
    float2 adjustedCameraPosition(float2(-_cameraPosition.x + (_renderSize.Width / 2.0f), 0));
    _layer1.Draw(adjustedCameraPosition, darkenAmount);
    _layer2.Draw(adjustedCameraPosition, darkenAmount);

    // Draw the entities
    context.SetRenderTransform(make_float3x2_translation(adjustedCameraPosition));
    for (auto entity : _entities)
    {
        // Adjust the entity drawing by a darkening amount
        // read from the current sky settings. This gives
        // a better lighting effect for time of day
        // rendering.
        // Assume that entities with point values are important for gameplay so
        // do not darken them as much to keep easier to see.
        entity->Draw(entity->GetPoints() ? darkenAmount/2.0f : darkenAmount);
    }

    // Draw final scrolling layer on top of the entities
    context.SetRenderTransform(float3x2::identity());
    _layer3.Draw(adjustedCameraPosition, darkenAmount);

    // Draw the score
    DrawScore();
}

void MainPage::DrawLevelCompleteScreen()
{
    // Draw the gameplay screen to keep the score and other state on the screen
    // while we are showing the level complete message.
    DrawGameplayScreen();

    GameContext& context = GameContext::GetInstance();

    // Draw level complete message
    std::wstring finalScoreText;
    strprintf(finalScoreText, L"%d", _harry->GetPoints());
    context.DrawText(finalScoreText, float2(_renderSize.Width / 2.0f, _renderSize.Height/2.0f), Colors::Yellow, 100, true);
    float2 shadowPos;

    if (_inputEnabled)
    {
        // Draw play next level button
        float2 playAgainButtonPos = GetRectCenter(_gameButtons[GameUIButton::NextLevel]);
        shadowPos = float2(playAgainButtonPos.x + 1, playAgainButtonPos.y + 1);
        context.DrawCircle(shadowPos, 24, Windows::UI::Colors::Black, 4);
        context.DrawSymbol(PlaySymbol, shadowPos, Windows::UI::Colors::Black, 40, true);
        context.DrawCircle(playAgainButtonPos, 24, Windows::UI::Colors::White, 4);
        context.DrawSymbol(PlaySymbol, playAgainButtonPos, Windows::UI::Colors::White, 40, true);
        float2 pos(playAgainButtonPos.x, playAgainButtonPos.y + 36);
        context.DrawText(L"Continue?", pos, Windows::UI::Colors::White, 20, true);

        // Draw no thanks button
        float2 noThanksButtonPos = GetRectCenter(_gameButtons[GameUIButton::ExitToTitle]);
        shadowPos = float2(noThanksButtonPos.x + 1, noThanksButtonPos.y + 1);
        context.DrawCircle(shadowPos, 24, Windows::UI::Colors::Black, 4);
        context.DrawSymbol(CancelSymbol, shadowPos, Windows::UI::Colors::Black, 40, true);
        context.DrawCircle(noThanksButtonPos, 24, Windows::UI::Colors::White, 4);
        context.DrawSymbol(CancelSymbol, noThanksButtonPos, Windows::UI::Colors::White, 40, true);
        float2 pos2(noThanksButtonPos.x, noThanksButtonPos.y + 36);
        context.DrawText(L"Exit", pos2, Windows::UI::Colors::White, 20, true);
    }
}

void MainPage::DrawGameOverScreen()
{
    // Draw the gameplay screen to keep the score and other state on the screen
    // while we are showing the game over message.
    DrawGameplayScreen();
    GameContext::GetInstance().DrawText(L"Game Over", float2(_renderSize.Width / 2.0f, _renderSize.Height / 2.0f), Colors::White, 40, true);
}

void MainPage::DrawPausedScreen()
{
    // Draw the gameplay screen while we are showing the paused message.
    DrawGameplayScreen();
    GameContext::GetInstance().DrawText(L"Paused", float2(_renderSize.Width / 2.0f, _renderSize.Height / 2.0f), Colors::White, 40, true);
}

void MainPage::DrawScore()
{
    if (_harry)
    {
        GameContext& context = GameContext::GetInstance();

        // Draw coin count
        _scoreCoinAnimation->Draw(float2(10, 12), 0.0f);
        context.DrawText(_scoreText, float2(20,0), Colors::Yellow, 16, false);

        // Draw health bar
        _healthBar.Draw(float2(_renderSize.Width/2,10));

        // Draw level number
        context.DrawText(_levelText, float2(330, 0), Colors::Yellow, 16, false);

        // Draw distance bar
        _distanceBar.Draw(float2(330, 24));

        if (_mapModeEnabled)
        {
            context.DrawText(L"Map Mode Enabled", float2(_renderSize.Width / 2, 20), Colors::Blue, 16, true);
        }
    }
}

void MainPage::ResizeCanvasControl()
{
    CoreWindow^ coreWindow = Window::Current->CoreWindow;
    float scale = std::min<float>(coreWindow->Bounds.Width / _renderSize.Width, coreWindow->Bounds.Height / _renderSize.Height);
    canvasControl->Height = _renderSize.Height * scale;
    canvasControl->Width = _renderSize.Width * scale;
    _inputPointScale = Size(_renderSize.Width/(float)canvasControl->Width, _renderSize.Height/(float)canvasControl->Height);
}

void MainPage::LoadTitleScreen()
{
    _currentLevel = 1;
    CreateScrollingLayers(1);
    _cameraPosition.x = _renderSize.Width / 2.0f;
    _cameraPosition.y = _renderSize.Height / 2.0f;
}

void MainPage::LoadLevel(size_t level)
{
    _currentLevel = level;
    GameLevel* levelData = &_levels[level - 1];

    std::mt19937 eng(_random());

    // Create the scrolling layers
    CreateScrollingLayers(level);

    // Add game entities
    _entities.clear();
    _harry.reset(new Harrydactyl(float2(200, 100)));
    _harry->Activate(!_mapModeEnabled);
    _cameraPosition = _harry->GetPosition();
    _entities.push_back(_harry);
    
    // Add coins randomly throughout the level
    _scoreCoinAnimation.reset(new AnimatedSprite(SpinningCoin00, 8, 0.0625f, false));
    std::vector<float2> coins;
    std::uniform_int_distribution<> coinY(30, 200);
    std::uniform_int_distribution<> coinX(static_cast<int>(_renderSize.Width), static_cast<int>(GetLevelEndPosition(level)));
    for (int i = 0; i < levelData->TotalCoins; i++)
    {
        float2 coinPos(static_cast<float>(coinX(_random)), static_cast<float>(coinY(_random)));
        _entities.push_back(std::shared_ptr<Entity>(new SpriteEntity(coinPos, SpinningCoin00, 8, 0.0625f, 10)));
    }

    // Add dinosaurs randomly throughout the level
    std::uniform_int_distribution<> brontoX(static_cast<int>(_renderSize.Width), static_cast<int>(GetLevelEndPosition(level)));
    for (int i = 0; i < levelData->NumBrownBrontosaurus; i++)
    {
        float2 brontoPos(static_cast<float>(brontoX(_random)), 180);
        _entities.push_back(std::shared_ptr<Entity>(new SpriteEntity(brontoPos, BrontosaurusBrown00, 2, 0.3f, 0, (i % 2) == 0)));
    }

    for (int i = 0; i < levelData->NumGreenBrontosaurus; i++)
    {
        float2 brontoPos(static_cast<float>(brontoX(_random)), 180);
        _entities.push_back(std::shared_ptr<Entity>(new SpriteEntity(brontoPos, BrontosaurusGreen00, 3, 0.25f, 0, (i % 2) == 0)));
    }

    ChangeGameState(Playing);
}

void MainPage::LoadNextLevel()
{
    _currentLevel++;
    // If the next level exceeds the total
    // levels defined for the game, wrap around
    // to level 1
    if (_currentLevel > _levels.size())
    {
        _currentLevel = 1;
    }
    ChangeGameState(GameState::Loading);
}

void MainPage::CreateScrollingLayers(size_t levelNumber)
{
    GameContext& context = GameContext::GetInstance();
    size_t levelIndex = levelNumber - 1;

    _layer1.Clear();
    _layer1.SetScrollRate(0.3f);

    _layer2.Clear();
    _layer2.SetScrollRate(0.5f);

    _layer3.Clear();
    _layer3.SetScrollRate(1.2f);

    // Create each background layer by specifying the z-ordered sprite data for the level.
    // Note: The first layer will get clouds.
    context.CreateLayer(BackgroundLayer1_1, _levels[levelIndex].Layer1, true /* layer 1 has clouds */);
    context.CreateLayer(BackgroundLayer1_2, _levels[levelIndex].Layer1, true /* layer 1 has clouds */);
    context.CreateLayer(BackgroundLayer1_3, _levels[levelIndex].Layer1, true /* layer 1 has clouds */);
    context.CreateLayer(BackgroundLayer2_1, _levels[levelIndex].Layer2);
    context.CreateLayer(BackgroundLayer2_2, _levels[levelIndex].Layer2);
    context.CreateLayer(BackgroundLayer2_3, _levels[levelIndex].Layer2);
    context.CreateLayer(BackgroundLayer3_1, _levels[levelIndex].Layer3);
    context.CreateLayer(BackgroundLayer3_2, _levels[levelIndex].Layer3);
    context.CreateLayer(BackgroundLayer3_3, _levels[levelIndex].Layer3);

    _layer1.AddLayer(BackgroundLayer1_1);
    _layer1.AddLayer(BackgroundLayer1_2);
    _layer1.AddLayer(BackgroundLayer1_3);

    _layer2.AddLayer(BackgroundLayer2_1);
    _layer2.AddLayer(BackgroundLayer2_2);
    _layer2.AddLayer(BackgroundLayer2_3);

    _layer3.AddLayer(BackgroundLayer3_1);
    _layer3.AddLayer(BackgroundLayer3_2);
    _layer3.AddLayer(BackgroundLayer3_3);
}

bool MainPage::EntityCollisionCheck(Entity* e1, Entity* e2)
{
    // Avoid checking against self
    if (e1 == e2)
    {
        return false;
    }

    float2 p1 = e1->GetPosition();
    float2 p2 = e2->GetPosition();

    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float radii = e1->GetCollisionRadius() + e2->GetCollisionRadius();
    if ((dx * dx) + (dy * dy) < radii * radii)
    {
        return true;
    }

    return false;
}

void MainPage::ChangeGameState(GameState state)
{
    if (state != _state)
    {
        _state = state;
        switch (_state)
        {
        case GameState::Title:
            LoadTitleScreen();
            break;
        case GameState::LevelComplete:
            // Level has been completed.
            // Disable input for a few seconds
            // so the player can view their score
            // without accidently continuing to the
            // next level or exiting to the title
            // screen.
            DisableInput(2.0f);
            break;
        case GameState::Loading:
        case GameState::Playing:
        case GameState::Paused:
        case GameState::GameOver:
        default:
            break;
        }
    }
}

bool MainPage::IsUIButtonTapped(GameUIButton button, float2 point)
{
    return _gameButtons[button].Contains(point);
}

float MainPage::GetLevelEndPosition(size_t level)
{
    return _levels[level - 1].LevelLength;
}

void MainPage::DisableInput(float duration)
{
    _inputEnableTimer = duration;
    _inputEnabled = false;
}

void MainPage::UpdateDisableInputTimer(float elapsedTime)
{
    // If input has been disabled, update the timer
    // so it will be enabled again.
    if (!_inputEnabled && _inputEnableTimer >= 0.0f)
    {
        _inputEnableTimer -= elapsedTime;
        if (_inputEnableTimer <= 0.0f)
        {
            _inputEnableTimer = 0.0f;
            _inputEnabled = true;
        }
    }
}
