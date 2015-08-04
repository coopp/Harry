#include "pch.h"
#include "MainPage.xaml.h"

using namespace Harry;
using namespace SimpleGame;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

App::App()
{
    InitializeComponent();
    Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
}

void App::OnLaunched(LaunchActivatedEventArgs^ e)
{
#if (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
    Windows::UI::ViewManagement::StatusBar::GetForCurrentView()->HideAsync();
#endif

#if _DEBUG
    if (IsDebuggerPresent())
    {
        DebugSettings->EnableFrameRateCounter = true;
    }
#endif

    auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);
    if (rootFrame == nullptr)
    {
        rootFrame = ref new Frame();
        rootFrame->CacheSize = 1;

        if (e->PreviousExecutionState == ApplicationExecutionState::Terminated)
        {
            // TODO: Restore the saved session state only when appropriate, scheduling the
            // final launch steps after the restore is complete.
        }

        Window::Current->Content = rootFrame;
    }

    if (rootFrame->Content == nullptr)
    {
#if WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP
        // Removes the turnstile navigation for startup.
        if (rootFrame->ContentTransitions != nullptr)
        {
            _transitions = ref new TransitionCollection();
            for (auto transition : rootFrame->ContentTransitions)
            {
                _transitions->Append(transition);
            }
        }

        rootFrame->ContentTransitions = nullptr;
        _firstNavigatedToken = rootFrame->Navigated += ref new NavigatedEventHandler(this, &App::RootFrame_FirstNavigated);
#endif

        if (!rootFrame->Navigate(MainPage::typeid, e->Arguments))
        {
            throw ref new FailureException("Failed to create initial page");
        }
    }

    // Ensure the current window is active
    Window::Current->Activate();
}

#if WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP

void App::RootFrame_FirstNavigated(Object^ sender, NavigationEventArgs^ e)
{
    auto rootFrame = safe_cast<Frame^>(sender);

    TransitionCollection^ newTransitions;
    if (_transitions == nullptr)
    {
        newTransitions = ref new TransitionCollection();
        newTransitions->Append(ref new NavigationThemeTransition());
    }
    else
    {
        newTransitions = _transitions;
    }

    rootFrame->ContentTransitions = newTransitions;

    rootFrame->Navigated -= _firstNavigatedToken;
}
#endif

void App::OnSuspending(Object^ sender, SuspendingEventArgs^ e)
{
    (void) sender;  // Unused parameter
    (void) e;       // Unused parameter

    // TODO: Save application state and stop any background activity
}