#include "EZWindow.h"
#include "EZError.h"

void EZ::RegisterClass(EZ::ClassSettings settings) {
	WNDCLASS wc = { };
	if (settings.Name == NULL) {
		wc.lpszClassName = EZ::DefaultClassName;
	}
	else {
		wc.lpszClassName = settings.Name;
	}
	if (settings.WndProc == NULL) {
		wc.lpfnWndProc = DefWindowProc;
	}
	else {
		wc.lpfnWndProc = settings.WndProc;
	}
	// NULL tells windows to use the default icon so no need to explicitly set it.
	wc.hIcon = settings.Icon;
	if (settings.Cursor == NULL) {
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	}
	else {
		wc.hCursor = settings.Cursor;
	}
	if (!settings.CustomBackPaint) {
		wc.hbrBackground = CreateSolidBrush(RGB(settings.BackColorR, settings.BackColorG, settings.BackColorB));
		if (wc.hbrBackground == NULL) {
			EZ::Error::ThrowFromLastError();
		}
	}
	else {
		wc.hbrBackground = NULL;
	}
	wc.style = settings.Styles;
	if (!settings.DontRedrawOnSize) {
		wc.style |= CS_HREDRAW | CS_VREDRAW;
	}
	if (settings.UniversalDropShadow) {
		wc.style |= CS_DROPSHADOW;
	}
	if (!settings.IgnoreDoubleClicks) {
		wc.style |= CS_DBLCLKS;
	}
	if (settings.NoCloseOption) {
		wc.style |= CS_NOCLOSE;
	}
	if (settings.SaveClippedGraphics) {
		wc.style |= CS_SAVEBITS;
	}
	if (!settings.ThisThreadOnly) {
		wc.style |= CS_GLOBALCLASS;
	}

	wc.cbClsExtra = 0; // Allocate 0 extra bytes after the class declaration.
	wc.cbWndExtra = 0; // Allocate 0 extra bytes after windows of this class.
	wc.hInstance = GetModuleHandle(NULL); // WndProc is in the current hInstance.
	wc.lpszMenuName = NULL; // Windows of this class have no default menu.

	if (::RegisterClass(&wc) == 0) {
		EZ::Error::ThrowFromLastError();
	}
}



EZ::Window::Window(EZ::WindowSettings settings) {
	_processingMessage = FALSE;
	_threadID = GetCurrentThreadId();
	_settings = settings;

	if (_settings.Title == NULL) {
		_settings.Title = DefaultWindowTitle;
	}
	if (_settings.ClassName == NULL) {
		_settings.ClassName = DefaultClassName;
	}
	if (_settings.InitialX == CW_USEDEFAULT) {
		_settings.InitialX = GetSystemMetrics(SM_CXSCREEN) / 4;
	}
	if (_settings.InitialY == CW_USEDEFAULT) {
		_settings.InitialY = GetSystemMetrics(SM_CYSCREEN) / 4;
	}
	if (_settings.InitialWidth == CW_USEDEFAULT) {
		_settings.InitialWidth = GetSystemMetrics(SM_CXSCREEN) / 2;
	}
	if (_settings.InitialHeight == CW_USEDEFAULT) {
		_settings.InitialHeight = GetSystemMetrics(SM_CYSCREEN) / 2;
	}
	switch (_settings.StylePreset) {
	case EZ::WindowStylePreset::Normal:
		_settings.Styles |= WS_OVERLAPPEDWINDOW;
		break;
	case EZ::WindowStylePreset::Popup:
		_settings.Styles |= WS_POPUPWINDOW;
		break;
	case EZ::WindowStylePreset::Boarderless:
		_settings.Styles |= WS_POPUP;
		break;
	case EZ::WindowStylePreset::DontTouchMyStyles:
	default:
		break;
	}
	if (!_settings.LaunchHidden) {
		_settings.Styles |= WS_VISIBLE;
	}
	if (_settings.DragNDropFiles) {
		_settings.ExtendedStyles |= WS_EX_ACCEPTFILES;
	}
	if (_settings.IgnoreFocusSwitch) {
		_settings.ExtendedStyles |= WS_EX_NOACTIVATE;
	}
	if (_settings.TopMost) {
		_settings.ExtendedStyles |= WS_EX_TOPMOST;
	}
	if (_settings.HideInTaskbar) {
		_settings.ExtendedStyles |= WS_EX_TOOLWINDOW;
	}

	_handle = CreateWindowEx(
		_settings.ExtendedStyles,
		_settings.ClassName,
		_settings.Title,
		_settings.Styles,
		_settings.InitialX,
		_settings.InitialY,
		_settings.InitialWidth,
		_settings.InitialHeight,
		NULL, // No parent window.
		NULL, // No target menu.
		GetModuleHandle(NULL), // Current process instance.
		NULL // No additional data.
	);
	if (_handle == INVALID_HANDLE_VALUE) {
		EZ::Error::ThrowFromLastError();
	}
}
void EZ::Window::Show(int showCommand) {
	if (GetCurrentThreadId() != _threadID) {
		throw Error(L"Cross thread GUI access is not allowed.");
	}
	if (IsDestroyed() || IsShowing()) {
		throw Error(L"Window must not be destroyed or already showing to show window.");
	}
	if (_processingMessage) {
		throw Error(L"Window cannot be shown from inside WndProc.");
	}
	ShowWindow(_handle, showCommand);
	if (GetLastError() != 0) {
		EZ::Error::ThrowFromLastError();
	}
}
BOOL EZ::Window::ProcessOne(BOOL wait) {
	if (GetCurrentThreadId() != _threadID) {
		throw Error(L"Cross thread GUI access is not allowed.");
	}
	if (!IsShowing()) {
		throw Error(L"Window must be showing to processing messages.");
	}
	if (_processingMessage) {
		throw Error(L"WndProc cannot be called from inside WndProc.");
	}
	_processingMessage = TRUE;
	BOOL output = FALSE;
	MSG msg = { };
	if (wait)
	{
		if (GetMessage(&msg, _handle, 0, 0)) {
			TranslateMessage(&msg);
			try {
				DispatchMessage(&msg);
			}
			catch (...) {
				_processingMessage = FALSE;
				throw;
			}
			output = TRUE;
		}
	}
	else
	{
		if (PeekMessage(&msg, _handle, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			try {
				DispatchMessage(&msg);
			}
			catch (...) {
				_processingMessage = FALSE;
				throw;
			}
			output = TRUE;
		}
	}
	_processingMessage = FALSE;
	return output;
}
BOOL EZ::Window::ProcessAll() {
	if (GetCurrentThreadId() != _threadID) {
		throw Error(L"Cross thread GUI access is not allowed.");
	}
	if (!IsShowing()) {
		throw Error(L"Window must be showing to processing messages.");
	}
	if (_processingMessage) {
		throw Error(L"WndProc cannot be called from inside WndProc.");
	}
	_processingMessage = TRUE;

	BOOL output = FALSE;
	MSG msg = { };
	while (!IsDestroyed() && PeekMessage(&msg, _handle, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		try {
			DispatchMessage(&msg);
		}
		catch (...) {
			_processingMessage = FALSE;
			throw;
		}
		output = TRUE;
	}

	_processingMessage = FALSE;
	return output;
}
BOOL EZ::Window::Run() {
	if (GetCurrentThreadId() != _threadID) {
		throw Error(L"Cross thread GUI access is not allowed.");
	}
	if (!IsShowing()) {
		throw Error(L"Window must be showing to processing messages.");
	}
	if (_processingMessage) {
		throw Error(L"WndProc cannot be called from inside WndProc.");
	}
	_processingMessage = TRUE;

	BOOL output = FALSE;
	MSG msg = { };
	while (!IsDestroyed() && GetMessage(&msg, _handle, 0, 0)) {
		TranslateMessage(&msg);
		try {
			DispatchMessage(&msg);
		}
		catch (...) {
			_processingMessage = FALSE;
			throw;
		}
		output = TRUE;
	}

	_processingMessage = FALSE;
	return output;
}
EZ::Window::~Window() {
	if (GetCurrentThreadId() != _threadID) {
		throw Error(L"Cross thread GUI access is not allowed.");
	}
	if (!IsDestroyed()) {
		DestroyWindow(_handle);
	}
}

HWND EZ::Window::GetHandle() const {
	return _handle;
}
EZ::WindowSettings EZ::Window::GetSettings() const {
	return _settings;
}
BOOL EZ::Window::IsShowing() const {
	return !IsDestroyed() && IsWindowVisible(_handle);
}
BOOL EZ::Window::IsDestroyed() const {
	return !IsWindow(_handle);
}