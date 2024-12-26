#pragma once
#include <Windows.h>
#include <D2D1.h>
#include <D2D1_1Helper.h>
#pragma comment(lib, "D2D1.lib")

namespace EZ {
	struct BitmapAsset {
		UINT32 Width;
		UINT32 Height;
		const BYTE* Buffer;
	};
	// These methods allow users to create rects with x, y, width, and height instead of left, top, right, and bottom.
	D2D1_RECT_F RectF(FLOAT x, FLOAT y, FLOAT width, FLOAT height);
	D2D1_RECT_L RectL(INT32 x, INT32 y, INT32 width, INT32 height);
	D2D1_RECT_U RectU(UINT32 x, UINT32 y, UINT32 width, UINT32 height);
	constexpr UINT32 DefaultRendererWidth = 256;
	constexpr UINT32 DefaultRendererHeight = 144;
	enum class RendererMode : BYTE {
		// Allows DirectX to choose the renderer mode automatically.
		DontCare = 0,
		// Forces DirectX to use a software renderer.
		// This means graphics will be rendered completely on the CPU.
		// This offers maximum compatibility with systems that lack a GPU but at the cost of preformance.
		Software = 1,
		// Forces DirectX to use a hardware renderer.
		// This means all graphics will be rendered on the GPU.
		// This offers better preformance but may cause the glitches or crashes if the GPU
		// lacks features required by DirectX.
		Hardware = 2,
	};
	struct RendererSettings {
		// Stores the width of the render buffer in pixels.
		// If BufferWidth == 0 then DefaultRendererWidth is used.
		UINT32 BufferWidth;
		// Stores the height of the render buffer in pixels.
		// If BufferHeight == 0 then DefaultRendererHeight is used.
		UINT32 BufferHeight;
		// If OptimizeForSingleThread == TRUE then the DirectX renderer is set to single threaded mode.
		// This improves preformance but removes multi-thread protections and should only be used for applications
		// where the renderer will be used by one and only one thread.
		// Else the DirectX renderer is set to its default multi-thread safe mode.
		BOOL OptimizeForSingleThread;
		// If RequireLatestDX == TRUE then the latest version of DirectX is required by this renderer.
		// Else any version of DirectX is fine. This may cause some advanced features to be unavailible
		// however it is generally recommended not to require the latest DirectX unless necesary so older
		// software and hardware can be supported.
		BOOL RequireLatestDX;
		// Determines weather this is a software or hardware renderer.
		// See RendererMode enum for detailed info on each option.
		EZ::RendererMode RenderMode = EZ::RendererMode::DontCare;
		// If UseVSync == TRUE then vertical sync is enabled for this renderer.
		// This causes the renderer to wait before presenting a frame for the physical display to be ready.
		// This artificially decreases FPS to match the monitor's refresh rate however it can help remove
		// visual artifacts such as tearing or flickering.
		// Else vertical sync is not used and frames are drawn as soon as they are rendered.
		// It is recommended to enable VSync for release builds for less artifacting and power consumption
		// but to disable VSync for debug builds so the true FPS potential of your app can be tested.
		BOOL UseVSync;
	};
	class Renderer {
	public:
		Renderer(HWND windowHandle, EZ::RendererSettings settings);
		void BeginDraw();
		void Clear(D2D1_COLOR_F color);
		void FillRect(D2D1_RECT_L rect, D2D1_COLOR_F color);
		void Resize(D2D1_SIZE_U newSize);
		void DrawBitmap(ID2D1Bitmap* bitmap, D2D1_POINT_2L position);
		void DrawBitmap(ID2D1Bitmap* bitmap, D2D1_RECT_L destination);
		void DrawBitmap(ID2D1Bitmap* bitmap, D2D1_RECT_L source, D2D1_RECT_L destination);
		ID2D1Bitmap* LoadBitmap(LPCWSTR filePath);
		ID2D1Bitmap* LoadBitmap(IStream* stream);
		ID2D1Bitmap* LoadBitmap(BitmapAsset asset);
		void EndDraw();
		D2D1_SIZE_U GetSize();
		D2D1_VECTOR_2F GetDpi();
		ID2D1HwndRenderTarget* GiveMePlz();
		~Renderer();

		HWND GetWindowHandle() const;
		EZ::RendererSettings GetSettings() const;

	private:
		HWND _windowHandle;
		ID2D1Factory* _factory;
		ID2D1HwndRenderTarget* _windowRenderTarget;
		EZ::RendererSettings _settings;
	};
}