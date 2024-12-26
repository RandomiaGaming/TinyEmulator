#pragma once
#include <Windows.h>

namespace EZ {
	class Error {
	public:
		enum class DisposalMethod : BYTE {
			Delete = 0, // Error message buffer is freed with C++ style delete[].
			Free = 1, // Error message buffer is freed with C-Style free(void* _Block) function.
			LocalFree = 3, // Message buffer is freed with Win32 API LocalFree(HLOCAL hMem) from WinBase.h.
		};

		// Note calling any non-const constructor gives ownership of the error message string to the Error object.
		// The Error object will destroy the string when calling PrintAndDispose.
		Error(LPCWSTR message);
		Error(LPCSTR message);
		Error(LPWSTR message, DisposalMethod disposal = EZ::Error::DisposalMethod::Delete);
		Error(LPSTR message, DisposalMethod disposal = EZ::Error::DisposalMethod::Delete);
		void PrintAndFree();
		~Error();

		static void ThrowFromHR(HRESULT hr);
		static void ThrowFromCode(DWORD errorCode);
		static void ThrowFromLastError();

	private:
		BOOL _isWideMessage = TRUE;
		BOOL _isConstMessage = TRUE;
		EZ::Error::DisposalMethod _disposalMethod = EZ::Error::DisposalMethod::Delete;
		void* _message = NULL;
	};
}