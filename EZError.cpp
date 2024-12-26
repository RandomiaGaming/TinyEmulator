#include "EZError.h"
#include <Windows.h>
#include <comdef.h>

EZ::Error::Error(LPCWSTR message) {
	_isWideMessage = TRUE;
	_isConstMessage = TRUE;
	_disposalMethod = Error::DisposalMethod::Delete;
	_message = const_cast<void*>(reinterpret_cast<const void*>(message));
}
EZ::Error::Error(LPCSTR message) {
	_isWideMessage = FALSE;
	_isConstMessage = TRUE;
	_disposalMethod = Error::DisposalMethod::Delete;
	_message = const_cast<void*>(reinterpret_cast<const void*>(message));
}
EZ::Error::Error(LPWSTR message, EZ::Error::DisposalMethod disposalMethod) {
	_isWideMessage = TRUE;
	_isConstMessage = FALSE;
	_disposalMethod = disposalMethod;
	_message = const_cast<void*>(reinterpret_cast<const void*>(message));
}
EZ::Error::Error(LPSTR message, EZ::Error::DisposalMethod disposalMethod) {
	_isWideMessage = FALSE;
	_isConstMessage = FALSE;
	_disposalMethod = disposalMethod;
	_message = const_cast<void*>(reinterpret_cast<const void*>(message));
}
void EZ::Error::PrintAndFree() {
	HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	// Get initial console attributes.
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(stdoutHandle, &consoleInfo);
	WORD savedAttributes = consoleInfo.wAttributes;

	// Set console attributes to red text.
	SetConsoleTextAttribute(stdoutHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);

	// Print error header.
	DWORD written;
	LPCWSTR errorHeader = L"ERROR: ";
	WriteConsole(stdoutHandle, errorHeader, lstrlen(errorHeader), &written, NULL);

	DWORD messageLength;
	if (_isWideMessage) {
		LPCWSTR wideMessage = reinterpret_cast<LPCWSTR>(_message);
		messageLength = lstrlenW(wideMessage);
	}
	else {
		LPCSTR narrowMessage = reinterpret_cast<LPCSTR>(_message);
		messageLength = lstrlenA(narrowMessage);
	}

	// Print error message.
	WriteConsole(stdoutHandle, _message, messageLength, &written, NULL);

	// Restore initial console attributes.
	SetConsoleTextAttribute(stdoutHandle, savedAttributes);

	// Dispose of string if needed.
	if (!_isConstMessage) {
		switch (_disposalMethod) {
		case EZ::Error::DisposalMethod::Free:
			free(const_cast<void*>(_message));
			break;
		case EZ::Error::DisposalMethod::Delete:
			delete[] const_cast<void*>(_message);
			break;
		case EZ::Error::DisposalMethod::LocalFree:
			::LocalFree(const_cast<void*>(_message));
			break;
		default: break;
		}
	}
	_message = NULL;
}
EZ::Error::~Error() {

}

void EZ::Error::ThrowFromHR(HRESULT hr) {
	if (SUCCEEDED(hr)) {
		return;
	}

	WCHAR* errorMessage = nullptr;
	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errorMessage, 0, NULL) > 0) {
		throw EZ::Error(errorMessage, EZ::Error::DisposalMethod::LocalFree);
	}
	else {
		_com_error comError(hr);
		LPCWSTR comErrorMessage = comError.ErrorMessage();

		errorMessage = new WCHAR[lstrlen(comErrorMessage)];
		lstrcpy(errorMessage, comErrorMessage);
		throw EZ::Error(errorMessage, EZ::Error::DisposalMethod::Delete);
	}
}
void EZ::Error::ThrowFromCode(DWORD errorCode) {
	if (errorCode == 0) {
		return;
	}

	LPWSTR errorMessage = nullptr;
	DWORD size = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errorMessage, 0, NULL);

	Error* error = new Error(errorMessage, EZ::Error::DisposalMethod::LocalFree);
}
void EZ::Error::ThrowFromLastError() {
	EZ::Error::ThrowFromCode(GetLastError());
}