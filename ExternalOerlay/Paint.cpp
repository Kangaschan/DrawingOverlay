#pragma once
#include "Paint.h"


int Paint::d3D9Init(HWND hWnd){

	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &d3dObject))){
		exit(1);
	}

	ZeroMemory(&d3dparams, sizeof(d3dparams));

	d3dparams.BackBufferWidth = width;
	d3dparams.BackBufferHeight = height;
	d3dparams.Windowed = TRUE;
	d3dparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dparams.hDeviceWindow = hWnd;
	d3dparams.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	d3dparams.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dparams.EnableAutoDepthStencil = TRUE;
	d3dparams.AutoDepthStencilFormat = D3DFMT_D16;

	HRESULT res = d3dObject->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dparams, 0, &d3dDevice);

	if (FAILED(res)){
		std::wstring ws(DXGetErrorString(res));
		std::string str(ws.begin(), ws.end());
		std::wstring ws2(DXGetErrorDescription(res));
		std::string str2(ws2.begin(), ws2.end());
		std::string error = "Error: " + str + " error description: " + str2;
		exit(1);
	}
	D3DXCreateFont(d3dDevice, 50, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Comic Sans", &d3dFont);

	
	
	return 0;

}
	
Paint::Paint() {};

Paint::Paint(HWND hWnd, int width, int height){
	this->width = width;
	this->height = height;
	initQueue(&(this->queue));
	this->brushColor = D3DCOLOR_XRGB(255, 255, 255);

	d3D9Init(hWnd);
	HRESULT hr = d3dDevice->CreateTexture(
		width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL);

}

int Paint::render()
{
	if (d3dDevice == nullptr)	
		return 1;
	d3dDevice->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	d3dDevice->BeginScene();


	renderTexture(texture);
	drawBrushStrokes();  // ��������� ������ �����
	

	BOOL flag = true;
	char** messageArr = queueToArray(&queue);

	if (messageArr) {
		
		for (int i = 0; i < queue.size; i++) {

			flag = true;
			for (int j = 0; j < COLORS_AMOUNT; j++)
			{
				if (strcmp(messageArr[i], colorNamesPull[j]) == 0)
				{
					D3DCOLOR color = colorsPull[j];
					int r = (color >> 16) & 0xFF;  // ������� 
					int g = (color >> 8) & 0xFF;   // �������
					int b = color & 0xFF;		   // �����
					drawText((char*)messageArr[i], width / 10, height / 10 + i * 50, 255, r, g, b);
					flag = false;
				}
			}
			if(flag)
			drawText((char*)messageArr[i], width / 10, height / 10 + i * 50, 255, 255, 255, 255);
		}

		// ������������ ������ �������
		freeArray(messageArr, queue.size);
	}

	if (isShowHelp)
	{
		showHelp();
	}
	if (saveFileChoose) {
		drawSaveFileDialog();
	}

	//drawSaveFileDialog();
	d3dDevice->EndScene();
	d3dDevice->PresentEx(0, 0, 0, 0, 0);

	
	
	return 0;
}

void Paint::drawPixels() {
	// ������������� ������ ������
	d3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// ������ ��� �������
	if (!pixels.empty()) {
		d3dDevice->DrawPrimitiveUP(D3DPT_POINTLIST, pixels.size(), pixels.data(), sizeof(CUSTOMVERTEX));
	}
}

void Paint::drawText(char* String, int x, int y, int a, int r, int g, int b)
{
	RECT FontPos;
	FontPos.left = x;
	FontPos.top = y;
	d3dFont->DrawTextA(0, String, strlen(String), &FontPos, DT_NOCLIP, D3DCOLOR_ARGB(a, r, g, b));
}

// ��������� ������� � ��������
void Paint::updateTextureWithNewPixels(IDirect3DTexture9* texture, const std::vector<CUSTOMVERTEX>& newPixels) {
	// �������� ������ � ��������
	D3DLOCKED_RECT lockedRect;
	//HRESULT hr = texture->LockRect(0, &lockedRect, NULL, D3DLOCK_DISCARD);  // D3DLOCK_DISCARD ���������� ���������� ������
	HRESULT hr = texture->LockRect(0, &lockedRect, NULL, 0);  // D3DLOCK_DISCARD ���������� ���������� ������
	if (FAILED(hr)) {
		// ��������� ������
		return;
	}

	// �������� ��������� �� ������ �������� (������� � ��������)
	DWORD* pDest = (DWORD*)lockedRect.pBits;

	// ����������� ������ �� newPixels � ������ ��� ��������
	for (const auto& pixel : newPixels) {
		int x = static_cast<int>(pixel.x); // ������� �� X
		int y = static_cast<int>(pixel.y); // ������� �� Y

		// ���������, ����� ���������� �� �������� �� ������� ��������
		if (x >= 0 && x < width && y >= 0 && y < height) {
			// �������� ���� �������
			D3DCOLOR color = pixel.color;

			// ������ ������� � ��������: y * ������ + x
			pDest[y * (lockedRect.Pitch / 4) + x] = color;
		}
	}

	// ��������� ���������� ������ ��������
	texture->UnlockRect(0);
}

void Paint::renderTexture(IDirect3DTexture9* texture) {
	// ������������� ��������
	d3dDevice->SetTexture(0, texture);

	// �������������, �� ������� ����� ����������� ��������
	struct Vertex {
		float x, y, z, rhw;
		DWORD color;
		float u, v;
	};

	// ������������� � ���������
	Vertex vertices[] = {
		{ 0.0f, 0.0f, 0.0f, 1.0f, 0xFFFFFFFF, 0.0f, 0.0f },
		{ (float)width, 0.0f, 0.0f, 1.0f, 0xFFFFFFFF, 1.0f, 0.0f },
		{ 0.0f, (float)height, 0.0f, 1.0f, 0xFFFFFFFF, 0.0f, 1.0f },
		{ (float)width, (float)height, 0.0f, 1.0f, 0xFFFFFFFF, 1.0f, 1.0f }
	};

	// ��������� FVF � ���������
	d3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(Vertex));

	d3dDevice->SetTexture(0, nullptr);
}

void Paint::drawFilledRect(int x, int y, int width, int height, D3DCOLOR color) {
	D3DRECT rect = { x, y, x + width, y + height };
	d3dDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 1.0f, 0);
}

void Paint::drawSaveFileDialog() {
	// ���������� � ������� ��������������
	int rectWidth = 1000;
	int rectHeight = 150;
	int rectX = (width - rectWidth) / 2;
	int rectY = (height - rectHeight) / 2;

	// ����� ��� ���������
	D3DCOLOR black = D3DCOLOR_XRGB(1, 1, 1);
	D3DCOLOR white = D3DCOLOR_XRGB(255, 255, 255);

	// ���������� ������ �������������
	drawFilledRect(rectX, rectY, rectWidth, rectHeight, black);

	// ����� "Choose save option" �� ������
	drawText((char*)"Choose save option", rectX + rectWidth / 2 - 200, rectY + 20, 255, 255, 255, 255);
		
	// ���������� ��� ������ ������
	int buttonY = rectY + rectHeight - 50;

	// ����� "save as canvas" (�����)
	drawText((char*)"save as canvas", rectX + 20, buttonY, 255, 255, 255, 255);

	// ����� "save as screenshot" (������)
	drawText((char*)"save as screenshot", rectX + rectWidth - 400, buttonY, 255, 255, 255, 255);
}

void Paint::BresenhamLine(POINT p1, POINT p2) {
	int dx = abs(p2.x - p1.x);  // ������� �� X
	int dy = abs(p2.y - p1.y);  // ������� �� Y
	int sx = (p1.x < p2.x) ? 1 : -1;  // ����������� �������� �� X
	int sy = (p1.y < p2.y) ? 1 : -1;  // ����������� �������� �� Y
	int err = dx - dy;  // ������, �������������� ������� ����� dx � dy

	int frequncy = brushRadius / 2;
	int pointInd = 0;
	while (true) {
		if ((pointInd % frequncy) == 0) {
			pixels.push_back({ (float)p1.x, (float)p1.y, 0.0f, 1.0f, brushColor });
			int r = brushRadius;

			for (int dx = -r; dx <= r; ++dx) {
				for (int dy = -r; dy <= r; ++dy) {
					if (dx * dx + dy * dy <= r * r) {
						POINT p = { p1.x + dx, p1.y + dy };

						pixels.push_back({ (float)p.x, (float)p.y, 0.0f, 1.0f, brushColor });

					}
				}
			}
			pointInd = 0;
		}
		pointInd++;
		// ���� ����� �� �������� �����, ���������
		if (p1.x == p2.x && p1.y == p2.y)
			break;

		int e2 = err * 2;  // ��������� ������ ��� ����������

		if (e2 > -dy) {
			err -= dy;
			p1.x += sx;  // ��� �� X
		}

		if (e2 < dx) {
			err += dx;
			p1.y += sy;  // ��� �� Y
		}
	}


}

void Paint::addBrushStroke(POINT point) {
	// ��������� ����������� ����� � ������� �����
	int r = brushRadius;

	for (int dx = -r; dx <= r; ++dx) {
		for (int dy = -r; dy <= r; ++dy) {
			if (dx * dx + dy * dy <= r * r) {
				POINT p = { point.x + dx, point.y + dy };

					pixels.push_back({ (float)p.x, (float)p.y, 0.0f, 1.0f, brushColor });
				
			}
		}
	}

	// ���� ���� ���������� �����, ��������� ����� ����� ��������� � ������� ������
	if (hasLastPoint) {
		BresenhamLine(lastPoint, point);
	}

	// ��������� ������� ����� ��� ���������
	lastPoint = point;
	hasLastPoint = true;
}

void Paint::setBrushRadius(int radius) {
	brushRadius = radius;
}

void Paint::clearCanvas() {
	freeQueue(&queue);
	initQueue(&queue);
	pixels.clear();
	d3dDevice->CreateTexture(
		width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL);
}

void Paint::drawBrushStrokes() {
	drawPixels();
}

void Paint::resetLastPoint() {
	hasLastPoint = false;
}

void Paint::addMessage(const char* sting) {
	enqueue(&queue, sting);
}

void Paint::stopMessage() {
	dequeue(&queue);
}

void Paint::setBrushColor(int ind)
{
	brushColor = colorsPull[ind];
}

void Paint::changeEraser() {
	if (isEraser)
	{
		brushColor = oldBrushColor;
		isEraser = false;
	}
	else
	{
		oldBrushColor = brushColor;
		brushColor = eraser;
		isEraser = true;
	}
}

void Paint::savePaintToFile(const std::string& filename) {

	  // �������� ����������� �� ��������
	updateTextureWithNewPixels(texture, pixels);
	IDirect3DSurface9* surface = nullptr;
	HRESULT hr = texture->GetSurfaceLevel(0, &surface);

	if (FAILED(hr)) {
		MessageBoxA(nullptr, "Failed to get surface from texture!", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	// ��������� ����������� � ���� ������� PNG
	hr = D3DXSaveSurfaceToFileA(filename.c_str(), D3DXIFF_PNG, surface, nullptr, nullptr);

	if (FAILED(hr)) {
		MessageBoxA(nullptr, "Failed to save image to file!", "Error", MB_OK | MB_ICONERROR);
	}


	// ����������� �����������
	surface->Release();
}

bool MergeImages(const std::wstring& baseImagePath, const std::wstring& overlayImagePath, const std::wstring& outputPath) {
	
		using namespace Gdiplus;
		using namespace std;

		ULONG_PTR gdiplusToken;
		GdiplusStartupInput gdiplusStartupInput;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
		// ��������� ������� �����������
		Image baseImage(baseImagePath.c_str());
		if (baseImage.GetLastStatus() != Ok) {
			wcerr << L"Failed to load base image!" << endl;
			return false;
		}

		// ��������� �����������-�������
		Image overlayImage(overlayImagePath.c_str());
		if (overlayImage.GetLastStatus() != Ok) {
			wcerr << L"Failed to load overlay image!" << endl;
			return false;
		}

		// ������ ����������� ������ ��� �������� �����������
		Bitmap resultBitmap(baseImage.GetWidth(), baseImage.GetHeight(), PixelFormat32bppARGB);
		Graphics graphics(&resultBitmap);

		// ������ ������� �����������
		graphics.DrawImage(&baseImage, 0, 0, baseImage.GetWidth(), baseImage.GetHeight());

		// ������ �����������-������� ������ �������� �����������
		graphics.DrawImage(&overlayImage, 0, 0, overlayImage.GetWidth(), overlayImage.GetHeight());

		// ��������� �������� �����������
		CLSID pngClsid;
		CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &pngClsid); // CLSID ��� ������� PNG

		if (resultBitmap.Save(outputPath.c_str(), &pngClsid, nullptr) != Ok) {
			wcerr << L"Failed to save result image!" << endl;
			return false;
		}

		wcout << L"Image saved successfully to " << outputPath << endl;

		//GdiplusShutdown(gdiplusToken);
	
	return true;
}

void Paint::saveScreenshotToFile(HBITMAP hBitmap, const std::string& filename ) {
	if (!hBitmap) {
		MessageBoxA(nullptr, "Invalid HBITMAP!", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	// �������� ���������� � HBITMAP
	BITMAP bmp;
	GetObject(hBitmap, sizeof(BITMAP), &bmp);

	int width = bmp.bmWidth;
	int height = bmp.bmHeight;

	// �������������� Direct3D9
	IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d) {
		MessageBoxA(nullptr, "Failed to initialize Direct3D9!", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	// ��������� ��� �������� ���������� ����������
	D3DPRESENT_PARAMETERS d3dpp = {};
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = GetDesktopWindow();  // ���������� ���������� ����

	// ������ ��������� ����������
	IDirect3DDevice9* d3dDevice = nullptr;
	if (FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3dDevice))) {
		MessageBoxA(nullptr, "Failed to create Direct3D device!", "Error", MB_OK | MB_ICONERROR);
		d3d->Release();
		return;
	}

	// ������ ����������� ��� �������� ������ �����������
	IDirect3DSurface9* surface = nullptr;
	if (FAILED(d3dDevice->CreateOffscreenPlainSurface(width, height, D3DFMT_A8R8G8B8,
		D3DPOOL_SYSTEMMEM, &surface, nullptr))) {
		MessageBoxA(nullptr, "Failed to create surface!", "Error", MB_OK | MB_ICONERROR);
		d3dDevice->Release();
		d3d->Release();
		return;
	}

	// ��������� ����������� ������� �� HBITMAP
	HDC hdcSurface;
	if (FAILED(surface->GetDC(&hdcSurface))) {
		MessageBoxA(nullptr, "Failed to get surface DC!", "Error", MB_OK | MB_ICONERROR);
		surface->Release();
		d3dDevice->Release();
		d3d->Release();
		return;
	}

	HDC hdcBitmap = CreateCompatibleDC(nullptr);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcBitmap, hBitmap);

	BitBlt(hdcSurface, 0, 0, width, height, hdcBitmap, 0, 0, SRCCOPY);

	// ����������� DC
	SelectObject(hdcBitmap, oldBitmap);
	DeleteDC(hdcBitmap);
	surface->ReleaseDC(hdcSurface);

	// ��������� ����������� � ���� PNG

	//D3DXSaveSurfaceToFileA(filename.c_str(), D3DXIFF_PNG, surface, nullptr, nullptr);

	if (FAILED(D3DXSaveSurfaceToFileA("base.png", D3DXIFF_PNG, surface, nullptr, nullptr))) {
		MessageBoxA(nullptr, "Failed to save surface to PNG!", "Error", MB_OK | MB_ICONERROR);
	}
	else {
		//MessageBoxA(nullptr,  filename.c_str(), "Image saved to ", MB_OK);
	}

	// ����������� �������
	surface->Release();
	d3dDevice->Release();
	d3d->Release();


	int size_needed = MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), (int)filename.size(), nullptr, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), (int)filename.size(), &wstr[0], size_needed);

	savePaintToFile("overlay.png");
	MergeImages(L"base.png", L"overlay.png", wstr);
}

void Paint::showSaveDialog(BOOL isShown) {
	saveFileChoose = isShown;
}

void Paint::endOfBrushStroke() {
	updateTextureWithNewPixels(texture, pixels);
	pixelsOld = pixels;
	pixels.clear();
}

void Paint::rollBack() {
	if (!isBack)
	{
		isBack = true;
		pixelsOld.clear();
	}
}
void Paint::addMessageBrushInfo(int ind) {
	int size = snprintf(nullptr, 0, "Brush color - %s \n Brush radius - %d", colorNamesPull[ind], brushRadius) + 1;

	// �������� ������ ��� �������� ������
	char* result = new char[size];

	// ��������� ������ � �������������� sprintf
	snprintf(result, size, "Brush color - %s \n Brush radius - %d", colorNamesPull[ind], brushRadius);
	addMessage(result);
}

void Paint::changeShowHelp()
{
	if (isShowHelp)
		isShowHelp = false;
	else
		isShowHelp = true;
}

void Paint::showHelp() {
	drawText((char*)"ctrl+shift + ...:\n \
X - close \n \
D - clear canvas \n \
H - show / unshow help �\n \
I - info(configuration) \n \
right / left - change color \n \
up - ++ brush size \n \
down - -- brush size \n \
E - eraser \n \
� - save \n \
Q - show / unshow canvas \n \
W - paint mode", width / 10 * 7, height / 50, 255, 255, 255, 2555);
}

