// KNOWN ISSUE: The loading cursor (IDC_APPSTARTING) which displays when starting debugging is a glitch in Visual Studio not
// an issue with EZWindow. That's why the loading cursor persists even when you switch to unrelated apps like file explorer.
// If you launch an app made with EZWindow from the .exe file directly this won't happen. Complain to Microsoft about it not me.

#pragma once
#include <Windows.h>

namespace EZ {
	constexpr LPCWSTR DefaultClassName = L"EZWindowClass";
	struct ClassSettings {
		// If Name == NULL then DefaultClassName is used.
		// Else Name specifies the name of the class to register.
		LPCWSTR Name;
		// If WndProc == NULL then DefWindowProc is used.
		// Else WndProc specifies the WndProc procedure to use for windows of this class.
		WNDPROC WndProc;
		// If Icon == NULL the default icon is used.
		// Else Icon specifies the icon used for windows of this class.
		HICON Icon;
		// If Cursor == NULL the default cursor is used (IDC_ARROW).
		// Else Cursor specifies the cursor used when the mouse is over windows of this class.
		HCURSOR Cursor;
		// If CustomBackPaint == FALSE then the window background drawn by the operating system using a solid color brush created from BackColor R, G, and B.
		// Else no operating system drawing is used and the user must handle WM_PAINT and WM_ERASEBKGND messages manually.
		BOOL CustomBackPaint;
		BYTE BackColorR;
		BYTE BackColorG;
		BYTE BackColorB;
		// Defines a list of styles which will be applied to this class.
		// Note that some of the flags below will change the styles including removing them.
		// Use flags if possible instead of setting Styles directly to avoid errors.
		DWORD Styles;
		// If DontRedrawOnSize == FALSE then the CS_HREDRAW and CS_VREDRAW styles are added to Styles.
		// Else nothing happens.
		BOOL DontRedrawOnSize;
		// If UniversalDropShadow == TRUE then the CS_DROPSHADOW style is added to Styles.
		// This will add a drop shadow to all windows of this class.
		// Else nothing happens.
		BOOL UniversalDropShadow;
		// If IgnoreDoubleClicks == FALSE then the CS_DBLCLKS style will be added to Styles.
		// This causes WM_LBUTTONDBLCLK messages to be sent to windows of this class when the user double clicks the window.
		// Else nothing happens and WM_LBUTTONDBLCLK messages will not be sent.
		BOOL IgnoreDoubleClicks;
		// If NoCloseOption == TRUE then the CS_NOCLOSE style will be added to Styles.
		// This causes the X to be greyed out. Additionally Alt+F4 no longer sends WM_CLOSE messages.
		// Else nothing happens.
		// NOTE: It is still possible to send WM_CLOSE messages though various system GUIs such as the Alt+Tab menu.
		//       These WM_CLOSE messages will still close the window unless otherwise ignored.
		BOOL NoCloseOption;
		// If SaveClippedGraphics == TRUE then the CS_SAVEBITS style is added to Styles.
		// This causes graphics from other windows to be saved and then restored when this window closes.
		// This is only useful for small PopUps which will close quickly.
		// Else nothing happens.
		BOOL SaveClippedGraphics;
		// If ThisThreadOnly == FALSE then the CS_GLOBALCLASS style is added to Styles.
		// and the class is availible to all threads in the current process.
		// Else nothing happens and the class will only be availible to the current thread.
		BOOL ThisThreadOnly;
	};
	void RegisterClass(EZ::ClassSettings settings);

	constexpr LPCWSTR DefaultWindowTitle = L"Unnamed EZWindow";
	enum class WindowStylePreset : BYTE {
		// Creates a normal window. Like Notepad.
		// Adds the WS_OVERLAPPEDWINDOW style group to your window which contains the following styles:
		// WS_OVERLAPPED, WS_CAPTION, WS_SYSMENU, WS_THICKFRAME, WS_MINIMIZEBOX, and WS_MAXIMIZEBOX.
		Normal = 0,
		// Creates a popup window. Like a MessageBox.
		// Adds the WS_POPUPWINDOW style group to your window which contains the following styles:
		// WS_POPUP, WS_BORDER, WS_SYSMENU
		Popup = 1,
		// Creates a boarderless window. Like ScreenCoverForm.
		// Adds the following styles to create a window with no bells or whistles just a rectangle to draw pixels in:
		// WS_POPUP
		Boarderless = 2,
		// Window styles will be left as 0.
		// Adds no styles to your window.
		DontTouchMyStyles = 0xFF,
	};
	struct WindowSettings {
		// If Title == NULL then DefaultWindowTitle is used.
		// Else Title specifies the title of the window.
		LPCWSTR Title;
		// If ClassName == NULL then DefaultClassName is used.
		// Else ClassName specifies the name of the class to use for this window.
		LPCWSTR ClassName;
		// If InitialX == CW_USEDEFAULT then InitialX is set to main dispay width / 4.
		// Else InitialX specifies the starting x position of the window where 0 is the left side of the main display.
		int InitialX = CW_USEDEFAULT;
		// If InitialY == CW_USEDEFAULT then InitialY is set to main dispay height / 4.
		// Else InitialY specifies the starting y position of the window where 0 is the top of the main display.
		int InitialY = CW_USEDEFAULT;
		// If InitialWidth == CW_USEDEFAULT then InitialWidth is set to main dispay width / 2.
		// Else InitialWidth specifies the starting width of the window in pixels.
		int InitialWidth = CW_USEDEFAULT;
		// If InitialHeight == CW_USEDEFAULT then InitialHeight is set to main dispay height / 2.
		// Else InitialHeight specifies the starting height of the window in pixels.
		int InitialHeight = CW_USEDEFAULT;
		// Defines a list of styles which will be applied to this window.
		// Note that some of the flags below will change the styles including removing them.
		// Use flags if possible instead of setting Styles directly to avoid errors.
		DWORD Styles;
		// Defines a list of extended styles which will be applied to this window.
		// Note that some of the flags below will change the extended styles including removing them.
		// Use flags if possible instead of setting ExtendedStyles directly to avoid errors.
		DWORD ExtendedStyles;
		// Defines which window style preset to use for this window.
		// See WindowStylePreset enum for detailed info on each option.
		EZ::WindowStylePreset StylePreset = EZ::WindowStylePreset::Normal;
		// If LaunchHidden == FALSE the WS_VISIBLE style will be added to this window.
		// This is the same as calling Window.Show straight away but way less effort.
		// Else nothing happens.
		BOOL LaunchHidden;
		// If DragNDropFiles == TRUE then the WS_EX_ACCEPTFILES extended style is added to this window.
		// This allows the user to drag and drop files onto this window and send WM_DROPFILES messages to this window.
		// Else nothing happens.
		BOOL DragNDropFiles;
		// If IgnoreFocusSwitch == TRUE this the WS_EX_NOACTIVATE extended style is added to this window.
		// This makes the window unable to gain focus through normal means but instead the window can only gain focus programmatically.
		// Else nothing happens.
		BOOL IgnoreFocusSwitch;
		// If TopMost == TRUE then the WS_EX_TOPMOST extended style is added to this window.
		// This causes the window to be TopMost immediately after creation.
		// Else nothing happens.
		BOOL TopMost;
		// If HideInTaskbar == TRUE then the WS_EX_TOOLWINDOW extended style is added to this window.
		// This causes the window to not showup in the taskbar.
		// Else nothing happens.
		BOOL HideInTaskbar;
	};
	class Window {
	public:
		Window(EZ::WindowSettings settings);
		void Show(int showCommand = SW_NORMAL);
		// Processes one message from this window's message queue.
		// Returns early if the window is destroyed.
		// Optionally waits for a message if the queue is empty.
		// Returns FALSE if this function call did not process a message else TRUE.
		BOOL ProcessOne(BOOL wait = FALSE);
		// Processes all messages in this window's message queue.
		// Returns when window is destroyed.
		// Returns FALSE if this function call did not process a message else TRUE.
		BOOL ProcessAll();
		// Processes messages from this window's message queue as they appear.
		// Returns when the window is destroyed.
		// Returns FALSE if this function call did not process a message else TRUE.
		BOOL Run();
		~Window();

		HWND GetHandle() const;
		EZ::WindowSettings GetSettings() const;
		BOOL IsShowing() const;
		BOOL IsDestroyed() const;
	private:
		BOOL _processingMessage;
		HWND _handle;
		EZ::WindowSettings _settings;
		DWORD _threadID;
	};
}