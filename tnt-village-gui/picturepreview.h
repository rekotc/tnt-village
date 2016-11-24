#ifndef PICTUREPREVIEW_H
#define PICTUREPREVIEW_H
//#pragma once
#include <Windows.h> //necessario per usare gdiplus
//forse non servono
#include <gdiplus.h>
#include <string.h>
#include <cwchar>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

class PicturePreview
{
public:
	PicturePreview();
	~PicturePreview();
	void initPicture(HWND, int);
	void paintPicture(HWND);
	void deletePicture();
private:

	Gdiplus::Bitmap* ResizeClone(Bitmap*, INT, INT);

	int image_code;
	const wchar_t file[8] = L"cat.bmp";

	Bitmap *gpBitmap;    // The bitmap for displaying an image
	Bitmap *bmp;
	ULONG_PTR gdiplusToken;

	PAINTSTRUCT ps;
	HDC hdc;
	RECT rc;
};
#endif
