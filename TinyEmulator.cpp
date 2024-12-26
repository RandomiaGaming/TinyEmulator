#include "EZProgram.h"
#include "EZError.h"
#include <thread>
#include <iostream>
#include <random>
#include <winnt.h>

BYTE emuMem[0xFFFF] = { };

constexpr UINT32 emuScreenWidth = 256;
constexpr UINT32 emuScreenHeight = 144;

ID2D1Bitmap* emuScreenBitmap = NULL;
BYTE emuScreenBuffer[emuScreenWidth * emuScreenHeight * 4] = { };

void Update(EZ::Program* program) {
	// Set Inputs
	emuMem[0] = 0;
	if (GetKeyState('W') & 0x8000) { emuMem[0] |= 1 << 0; }
	if (GetKeyState('S') & 0x8000) { emuMem[0] |= 1 << 1; }
	if (GetKeyState('A') & 0x8000) { emuMem[0] |= 1 << 2; }
	if (GetKeyState('D') & 0x8000) { emuMem[0] |= 1 << 3; }
	if (GetKeyState(VK_SPACE) & 0x8000) { emuMem[0] |= 1 << 4; }
	if (GetKeyState('J') & 0x8000) { emuMem[0] |= 1 << 5; }
	if (GetKeyState('K') & 0x8000) { emuMem[0] |= 1 << 6; }
	if (GetKeyState('L') & 0x8000) { emuMem[0] |= 1 << 7; }

	// Copy and convert from R8G8B8 in emuMem to B8G8R8A8 in emuScreenBuffer
	BYTE* emuMemPtr = emuMem;
	BYTE* emuScreenBufferPtr = emuScreenBuffer;
	for (UINT32 i = 0; i < emuScreenWidth * emuScreenHeight; i++) {
		emuScreenBufferPtr[0] = emuMemPtr[0]; // Copy B
		emuScreenBufferPtr[1] = emuMemPtr[0]; // Copy G
		emuScreenBufferPtr[2] = emuMemPtr[0]; // Copy R
		emuScreenBufferPtr[3] = 0xFF; // Set A to 0xFF
		// Move Ptrs into position for next pixel.
		emuMemPtr += 1;
		emuScreenBufferPtr += 4;
	}

	// Send emuScreenBuffer to the GPU and draw to the screen with emuScreenBitmap.
	D2D1_SIZE_U rendererSize = program->GetRenderer()->GetSize();
	D2D1_RECT_L rendererRect = EZ::RectL(0, 0, rendererSize.width, rendererSize.height);

	D2D1_RECT_U rect = D2D1::RectU(0, 0, emuScreenWidth, emuScreenHeight);
	emuScreenBitmap->CopyFromMemory(&rect, emuScreenBuffer, 256 * 4);

	program->GetRenderer()->DrawBitmap(emuScreenBitmap, rendererRect);
}

int main() {
	EZ::ClassSettings classSettings = { };
	classSettings.ThisThreadOnly = TRUE;

	EZ::WindowSettings windowSettings = { };
	windowSettings.Title = L"Tiny Emulator";
	windowSettings.LaunchHidden = TRUE;

	EZ::RendererSettings rendererSettings = { };
	rendererSettings.OptimizeForSingleThread = TRUE;

	EZ::ProgramSettings programSettings = { };
	programSettings.PreformanceLogInterval = 1000;
	programSettings.UpdateCallback = Update;

	EZ::Program* program = new EZ::Program(programSettings, classSettings, windowSettings, rendererSettings);

	EZ::Renderer* renderer = program->GetRenderer();

	// Define the bitmap properties
	D2D1_BITMAP_PROPERTIES bitmapProperties = {};
	D2D1_VECTOR_2F rendererDpi = renderer->GetDpi();
	bitmapProperties.dpiX = rendererDpi.x;
	bitmapProperties.dpiY = rendererDpi.y;
	bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	bitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;

	D2D1_SIZE_U bitmapSize = D2D1::SizeU(256, 144);

	EZ::Error::ThrowFromHR(renderer->GiveMePlz()->CreateBitmap(bitmapSize, nullptr, 0, &bitmapProperties, &emuScreenBitmap));

	program->Run();

	delete program;

	return 0;
}