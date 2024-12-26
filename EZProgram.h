#pragma once
#include "EZRenderer.h"
#include "EZWindow.h"
#include "EZProfiler.h"
#include "EZError.h"
#include <thread>

namespace EZ {
	class Program; // Forward declaration of Program because C++ is stupid.
	typedef LRESULT(*WindowCallback)(EZ::Program* program, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	typedef void (*UpdateCallback)(EZ::Program* program);
	constexpr UINT64 DefaultPreformanceLogInterval = 60;
	struct ProgramSettings {
		// This is a user defined pointer which can point to anything.
		// This is useful for transferring data from your main code into callbacks.
		// For example you could place important data into a Context class you created and initialized in your main function.
		// Then by setting user data equal to a pointer to Context you would easily be able to have access to that information
		// inside the WndProc and Update callbacks simply by calling GetUserDataAs().
		void* UserData;
		// If DontResizeBuffer == TRUE then the renderer buffer is left at its original size.
		// Otherwise WM_SIZE events are handled by resizing the renderer buffer before continueing.
		BOOL DontResizeBuffer;
		// If IgnoreWMClose == TRUE then the WM_CLOSE and WM_DISTROY messages are ignored.
		// This results in a window which can only be closed by ending the parent process in task manager.
		BOOL IgnoreWMClose;
		// If DontLogPreformace == TRUE then the profiler will never print to the console.
		BOOL DontLogPreformace;
		// The system will print the current FPS and TPS to the console every few frames.
		// PreformanceLogInterval stores the number of frames between each print.
		// If PreformanceLogInterval < 0 the default of 60 is used.
		UINT64 PreformanceLogInterval = DefaultPreformanceLogInterval;
		// MaximumFramerate is the maximum FPS allowed before the game loop busy waits to artificially lower FPS.
		// Note that a MaximumFramerate of 0 means uncapped FPS.
		// If MaximumFramerate == 0 then the framerate is only limited by hardware speed.
		UINT32 MaximumFramerate = 0;
		// This callback is ran whenever there is a message for the window to handle.
		// It is equivalent to WndProc in normal Win32 programming.
		// This callback will not be called for messages which are ignored.
		WindowCallback WndProcCallback;
		// This callback is called once per frame to render the graphics and preform updates.
		// It is called between BeginDraw and EndDraw.
		UpdateCallback UpdateCallback;
	};
	class Program {
	public:
		Program(EZ::ProgramSettings programSettings, EZ::ClassSettings classSettings, EZ::WindowSettings windowSettings, EZ::RendererSettings rendererSettings);
		void Run();
		~Program();

		EZ::Renderer* GetRenderer() const;
		EZ::Window* GetWindow() const;
		EZ::ProgramSettings GetProgramSettings() const;
		EZ::ClassSettings GetClassSettings() const;
		EZ::WindowSettings GetWindowSettings() const;
		EZ::RendererSettings GetRendererSettings() const;

		template <typename T> T* GetUserDataAs() const;

	private:
		static LRESULT CALLBACK CustomWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		enum class State : BYTE {
			Created = 0,
			Running = 1,
			Closed = 2,
			Destroyed = 3
		};
		EZ::Program::State _state;
		D2D1_SIZE_U _newSize;
		BOOL _resizeRequested;

		EZ::Profiler* _profiler;
		EZ::Renderer* _renderer;
		EZ::Window* _window;

		EZ::ProgramSettings _programSettings;
		EZ::ClassSettings _classSettings;
		EZ::WindowSettings _windowSettings;
		EZ::RendererSettings _rendererSettings;
	};
}
// This is defined here not in EZProgram.cpp because the source code for
// functions using templates must be #included wherever they are called.
template <typename T> T* EZ::Program::GetUserDataAs() const {
	return reinterpret_cast<T*>(_programSettings.UserData);
}