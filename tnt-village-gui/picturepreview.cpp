#include "picturepreview.h"


PicturePreview::PicturePreview()
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	gpBitmap = NULL;
}


PicturePreview::~PicturePreview()
{
}


void PicturePreview::paintPicture(HWND hDlg)
{
	//image_code = IDC_IMAGE;
	hdc = BeginPaint(GetDlgItem(hDlg, image_code ), &ps);
	if (bmp)
	{
		Graphics graphics(hdc);
		Rect destRect(200, 50, 150, 75);
		//graphics.DrawImage(&bmp, destRect);
		graphics.DrawImage(bmp, 0, 0);
	}
	EndPaint(GetDlgItem(hDlg, image_code), &ps);
	DeleteDC(hdc);

}

void PicturePreview::deletePicture()
{
	if (gpBitmap) delete gpBitmap;
	if (bmp) delete bmp;
	GdiplusShutdown(gdiplusToken);
}

void PicturePreview::initPicture(HWND hDlg, int c)
{
	image_code = c;
	GetClientRect(GetDlgItem(hDlg, image_code), &rc);
	if (gpBitmap)
	{
		// We need to destroy the old bitmap
		delete gpBitmap;
	}
	
	//gpBitmap = new Bitmap(L"cat.bmp");
	gpBitmap = new Bitmap(file);
	bmp = ResizeClone(gpBitmap, rc.right - rc.left, rc.bottom - rc.top);

	InvalidateRect(GetDlgItem(hDlg, image_code), NULL, TRUE);
	UpdateWindow(GetDlgItem(hDlg, image_code));
}

Gdiplus::Bitmap* PicturePreview::ResizeClone(Bitmap *bmp, INT width, INT height)
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