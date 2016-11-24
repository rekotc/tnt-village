#include <windows.h>
#include "resource.h"

#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib,"GdiPlus.lib")

Bitmap *gpBitmap;    // The bitmap for displaying an image
Bitmap *bmp;
ULONG_PTR gdiplusToken;
BOOL CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lpram);


Gdiplus::Bitmap* ResizeClone(Bitmap *bmp, INT width, INT height)
{
	UINT o_height = bmp->GetHeight();
	UINT o_width = bmp->GetWidth();
	INT n_width = width;
	INT n_height = height;
	double ratio = ((double)o_width) / ((double)o_height);
	if (o_width > o_height) {
		// Resize down by width
		n_height = static_cast<UINT>(((double)n_width) / ratio);
	}
	else {
		n_width = static_cast<UINT>(n_height * ratio);
	}
	Gdiplus::Bitmap* newBitmap = new Gdiplus::Bitmap(n_width, n_height, bmp->GetPixelFormat());
	Gdiplus::Graphics graphics(newBitmap);
	graphics.DrawImage(bmp, 0, 0, n_width, n_height);
	return newBitmap;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// Initialize GDI+
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	gpBitmap = NULL;
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DLGPROC(DlgProc), 0);
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lpram)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rc;
	switch (message)
	{
	case WM_INITDIALOG:
	{
		GetClientRect(GetDlgItem(hwnd, IDC_PICBOX), &rc);
		if (gpBitmap)
		{
			// We need to destroy the old bitmap
			delete gpBitmap;
		}
		gpBitmap = new Bitmap(L"cat.bmp");
		bmp = ResizeClone(gpBitmap, rc.right - rc.left, rc.bottom - rc.top);
		// We need to force the window to redraw itself
		InvalidateRect(GetDlgItem(hwnd, IDC_PICBOX), NULL, TRUE);
		UpdateWindow(GetDlgItem(hwnd, IDC_PICBOX));
	}
	return true;
	case WM_PAINT:
	{
		hdc = BeginPaint(GetDlgItem(hwnd, IDC_PICBOX), &ps);
		if (bmp)
		{
			Graphics graphics(hdc);
			graphics.DrawImage(bmp, 0, 0);
		}
		EndPaint(GetDlgItem(hwnd, IDC_PICBOX), &ps);
		DeleteDC(hdc);
	}
	return true;
	case WM_COMMAND:
		return true;
	case WM_CLOSE:
		if (gpBitmap) delete gpBitmap;
		if (bmp) delete bmp;
		GdiplusShutdown(gdiplusToken);
		EndDialog(hwnd, 0);
		return true;
	}
	return false;
}