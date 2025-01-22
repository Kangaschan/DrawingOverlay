#pragma once

#include <string> //save error
#include <Windows.h>
#include <vector>
#include <d3d9.h>
#include <d3dx9.h>
#include "StringQueue.h"
#include <gdiplus.h>	
#include "colors.h"	
#include <gdiplus.h>
#include <iostream>
#pragma comment(lib, "gdiplus.lib")

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <DxErr.h> //get error from error code
#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")



class Paint{
private:
	struct CUSTOMVERTEX {
		FLOAT x, y, z, rhw;
		D3DCOLOR color;
	};
	std::vector<POINT> brushStrokes;//мазки кистью
	std::vector<CUSTOMVERTEX> pixels;
	std::vector<CUSTOMVERTEX> pixelsOld;
	IDirect3DTexture9* texture;
	std::vector<std::pair<POINT, POINT>> lines;//линии между мазками 
	IDirect3D9Ex* d3dObject = NULL; //used to create device
	IDirect3DDevice9Ex* d3dDevice = NULL; //contains functions like begin and end scene 
	D3DPRESENT_PARAMETERS d3dparams; //parameters for creating device
	ID3DXFont* d3dFont = 0; // font used when displaying text
	HWND targetWnd;
	POINT lastPoint;
	D3DCOLOR brushColor;
	bool hasLastPoint = false;
	bool isEraser = false;
	bool saveFileChoose = false;
	bool isBack = false;
	BOOL isShowHelp = false;
	D3DCOLOR oldBrushColor;
	StringQueue queue;
	int width;
	int height;
	int brushRadius = 5;
	int d3D9Init(HWND hWnd);
	void drawText(char* String, int x, int y, int a, int r, int g, int b);
	void addCircle(POINT center);
	void drawPixels();
	void BresenhamLine(POINT p1, POINT p2);
	void drawFilledRect(int x, int y, int width, int height, D3DCOLOR color);
	void drawSaveFileDialog();
	void updateTextureWithNewPixels(IDirect3DTexture9* texture, const std::vector<CUSTOMVERTEX>& newPixels);
	void renderTexture(IDirect3DTexture9* texture);
	void showHelp();
	void drawBrushStrokes();
public:
	Paint();
	Paint(HWND hWnd, int width, int height);
	int render();
	void addBrushStroke(POINT point);
	void setBrushRadius(int radius);
	void resetLastPoint();
	void clearCanvas();
	void addMessage(const char* string);
	void stopMessage();
	void setBrushColor(int ind);
	void changeEraser();
	void savePaintToFile(const std::string& filename);
	void saveScreenshotToFile(HBITMAP hBitmap,const std::string& filename);
	void showSaveDialog(BOOL isShown);
	void endOfBrushStroke();
	void addMessageBrushInfo(int ind);
	void changeShowHelp();
	void rollBack();
};