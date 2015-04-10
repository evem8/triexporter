#pragma once

#include "arcball.h"
#include "SharedCache.h"
#include "TriFile.h"

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)
#define SAFE_RELEASE(p)            { if (p) { (p)->Release();    (p)=NULL; } }

class CDXMessageLoop : public CMessageLoop {
	
	BOOL OnIdle(int nIdleCount) {
		return !CMessageLoop::OnIdle(nIdleCount);
	}
	
};

class C3d : public CWindowImpl<C3d>
{
public:
	D3DPRESENT_PARAMETERS d3dpp;
	//IDirect3DSwapChain9* mSwapChain; 
	//IDirect3DSurface9* mDepthStencil; 
	LPDIRECT3D9						g_pD3D;
	LPDIRECT3DDEVICE9				g_pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9			g_pVB ;
	vector<LPDIRECT3DINDEXBUFFER9>	g_pIB;
	vector<LPDIRECT3DTEXTURE9>		g_pTexture;
	ArcBall m_abArcBall;
	float distance;
	float alight;
	UINT_PTR time;
	DWORD vcount;
	std::vector<DWORD> fcount;
	bool loaded;
	int m_lMousex;
	int m_lMousey;
	int drawSurface[10];
	bool rotation;
	D3DXVECTOR3 vCenter;
	D3DXMATRIXA16 matWorld;
	D3DLIGHT9 light;
	FLOAT vRadius;
	FLOAT Aspect;
	BEGIN_MSG_MAP(C3d)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnDblClick)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	void ReCreateBuffers(int w, int h);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDblClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void ClearTextures();
	void ClearIndexes();
	void TextureChange(const SharedCache &sc,const vector<int> &textures);
	void Open(const TriFile &tfile);
	C3d();
	void SwapTexture(int x, int y);
	void InitD3D();
	void Cleanup();
	void Reset();
	void SwapFillMode();
	void SetupMatrices();
	void SetupLights();
	BOOL SubclassWindow(HWND hWnd);
	void Render();
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};