#include "stdafx.h"
#include "3d.h"
#include "d3dhelper.h"


LRESULT C3d::OnDblClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	rotation = !rotation;
	m_abArcBall.Reset();
	return true;
}

LRESULT C3d::OnMouseWheel(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(((int)wParam) > 0)
	{
		distance+=0.05f;
	}
	else
	{
		distance-=0.05f;
	}
	if(distance < 0)
		distance =0;
	return TRUE;
}

LRESULT C3d::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	RECT rect;
	GetClientRect(&rect);
	ReCreateBuffers(rect.bottom-rect.top,rect.right-rect.left);
	return TRUE;
}

LRESULT C3d::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SAFE_RELEASE(mSwapChain);   
	SAFE_RELEASE(mDepthStencil);
	SAFE_RELEASE(g_pD3D);
	ClearTextures();
	Cleanup();
	return TRUE;
}

void C3d::ClearTextures()
{
	for(unsigned int i=0; i < g_pTexture.size(); i++)
		SAFE_RELEASE(g_pTexture[i]);
}

void C3d::ClearIndexes()
{
	for(unsigned int i=0; i < g_pIB.size(); i++)
		SAFE_RELEASE(g_pIB[i]);
}

void C3d::TextureChange(const StuffFile &sf,const vector<int> &textures)
{
	ClearTextures();
	for(unsigned int i=0; i < g_pTexture.size(); i++)
	{
		int index = textures[i];
		g_pTexture[i] = NULL;
		if(index > -1)
		{
			vector<byte> data;
			data.resize(sf.files[index].fileSize);
			sf.files[index].handle->seekg(sf.files[index].fileOffset);
			sf.files[index].handle->read(reinterpret_cast<char*>(&data[0]), sf.files[index].fileSize);
			LPDIRECT3DTEXTURE9 tmp;
			if(LoadTextureFromMemory(g_pd3dDevice, &data[0], &tmp))
				g_pTexture[i] = tmp;
		}	
	}
}

void C3d::SwapTexture(int x, int y)
{
	swap(g_pTexture[x], g_pTexture[y]);
}

void C3d::Open(const TriFile &tfile)
{
	Cleanup();
	rotation = true;
	if( FAILED( g_pd3dDevice->CreateVertexBuffer(tfile.header.numVertices*sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL)))
		return;
	CUSTOMVERTEX* pVertices;
	if( FAILED( g_pVB->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
		return;
	vcount = tfile.header.numVertices;
	for( DWORD i=0; i<tfile.header.numVertices; i++ )
	{
		pVertices[i].position = tfile.vertices(i)->vertexPosition;
		pVertices[i].normal   = tfile.vertices(i)->vertexNormal;
		pVertices[i].uv   = tfile.vertices(i)->vertexUV;
	}
	ComputeBoundingSphere(pVertices, vcount, &vCenter, &vRadius);
	g_pVB->Unlock();
	D3DVIEWPORT9 vp;
	g_pd3dDevice->GetViewport(&vp);
	vp.MinZ = -((int)vRadius/300)*55.0f;
	g_pd3dDevice->SetViewport(&vp);
	D3DXMatrixIdentity( &matWorld );
	MatrixTranslation( &matWorld, vCenter.x, -vCenter.y, -vCenter.z );
	DWORD *indices=NULL;
	
	ClearIndexes();
	ClearTextures();
	g_pTexture.resize(tfile.header.numSurfaces);
	for(unsigned int i = 0; i < tfile.header.numSurfaces; i++)
			g_pTexture[i] = NULL;
	g_pIB.resize(tfile.header.numSurfaces);
	fcount.resize(tfile.header.numSurfaces);
	for(dword i = 0; i < tfile.header.numSurfaces; i++)
	{
		g_pd3dDevice->CreateIndexBuffer(tfile.surfaces[i].numTriangles*3*4,D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, &g_pIB[i], NULL);
		g_pIB[i]->Lock( 0, 0, (void**)&indices, 0 );

		for(dword c = 0; c < tfile.surfaces[i].numTriangles; c++)
		{
			indices[3*c] = tfile.triangles[i][c][0];
			indices[3*c+1] = tfile.triangles[i][c][1];
			indices[3*c+2] = tfile.triangles[i][c][2];
		}
		g_pIB[i]->Unlock();

		fcount[i] = tfile.surfaces[i].numTriangles;
	}
	loaded = true;
}

C3d::C3d(): mSwapChain(0), mDepthStencil(0), g_pd3dDevice(0), g_pD3D(0), g_pVB(0), loaded(false), distance(1.0f)
{
	d3dpp.BackBufferWidth            = 0;    
	d3dpp.BackBufferHeight           = 0;   
	d3dpp.BackBufferFormat           = D3DFMT_UNKNOWN;   
	d3dpp.BackBufferCount            = 1;   
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;   
	d3dpp.MultiSampleQuality         = 0;   
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_FLIP;    
	d3dpp.hDeviceWindow              = m_hWnd;   
	d3dpp.Windowed                   = true;   
	d3dpp.EnableAutoDepthStencil     = true;    
	//d3dpp.EnableAutoDepthStencil     = false;    
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;   
	d3dpp.Flags                      = 0;   
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;   
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_ONE;
	g_pIB.resize(0);
	g_pTexture.resize(0);
}

void C3d::InitD3D()
{
	if(g_pD3D == NULL)
		if(NULL==(g_pD3D=Direct3DCreate9(D3D_SDK_VERSION)))
			return;
	if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,m_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
		return;
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	g_pd3dDevice->LightEnable( 0, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );
	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	mtrl.Power = 2.0f;
	g_pd3dDevice->SetMaterial( &mtrl );
	ZeroMemory( &light, sizeof(D3DLIGHT9) );
	light.Type       = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r  = 0.9f;
	light.Diffuse.g  = 0.9f;
	light.Diffuse.b  = 0.7f;
	light.Range       = 1000.0f;
	RECT rect;
	GetClientRect(&rect);
	ReCreateBuffers(rect.bottom-rect.top,rect.right-rect.left);
	Render();
}

void C3d::Reset()
{

}

void C3d::Cleanup()
{
	distance = 1.0f;
	SAFE_RELEASE(g_pVB);
	ClearIndexes();
}

void C3d::SetupMatrices()
{
	D3DXMATRIXA16 matWorld1;
	D3DXMATRIXA16 mat;
	D3DXVECTOR3 vEyePt(0.0f, vRadius*distance*1.025f,vRadius*distance*-3.0f );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 0.5f, 0.0f );
	if(rotation)
	{
		D3DXMatrixIdentity(&mat);
		MatrixRotationY( &mat, timeGetTime()/1500.0f );
	}
	else
	{
		m_abArcBall.GetMat(&mat);
		vEyePt = D3DXVECTOR3(0.0f, 0.0f,vRadius*distance*-4.0f);
		vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		vUpVec = D3DXVECTOR3( 0.0f, 1.0f, 1.0f );
	}
	
	MatrixMultiply(&matWorld , &mat, &matWorld1);
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld1 );
	D3DXMATRIXA16 matView;
	MatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	D3DXMATRIXA16 matProj;
	MatrixPerspectiveFovLH( &matProj, D3DX_PI/5, 1.0f, 1.0f, vRadius*20);// 10.0f );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}

BOOL C3d::SubclassWindow(HWND hWnd)
{
	BOOL bRet = CWindowImpl<C3d>::SubclassWindow(hWnd);
	if(bRet)
		InitD3D();
	return bRet;
}

void C3d::ReCreateBuffers(int w, int h)   
{   
	// Destroy the old ones.   
	SAFE_RELEASE(mSwapChain);   
	SAFE_RELEASE(mDepthStencil);
	// Create the swapchain associated with this view object.   
	d3dpp.hDeviceWindow    = m_hWnd;
	d3dpp.BackBufferWidth  = w - 10;   
	d3dpp.BackBufferHeight = h - 10;   
	g_pd3dDevice->CreateAdditionalSwapChain(&d3dpp, &mSwapChain);
	g_pd3dDevice->CreateDepthStencilSurface(w, h, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, true, &mDepthStencil, 0);   
	m_abArcBall.Size(w,h);
}   

void C3d::Render()
{
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100,100,255), 1.0f, 0);
	IDirect3DSurface9* backbuffer = 0;   
		mSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);   
		g_pd3dDevice->SetRenderTarget(0, backbuffer);   
		g_pd3dDevice->SetDepthStencilSurface(mDepthStencil);   
		SAFE_RELEASE(backbuffer);

	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		SetupMatrices();
		Vec3Normalize(&light.Direction, &(D3DXVECTOR3(cosf(alight), -1.0f, sinf(alight))));
		g_pd3dDevice->SetLight( 0, &light );
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		for(dword i = 0; i < g_pIB.size(); i++)
		{
			LPDIRECT3DTEXTURE9 tmp = g_pTexture[i];
			g_pd3dDevice->SetTexture( 0, tmp );
			g_pd3dDevice->SetIndices( g_pIB[i]);
			if(loaded)
				g_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,0,0,vcount,0,fcount[i]);
		}
		g_pd3dDevice->EndScene();
	}
	//g_pd3dDevice->Present(0, 0, 0, 0);
	mSwapChain->Present(0, 0, m_hWnd, 0, 0);
	//mSwapChain->Present(0, 0, 0, 0, 0);
}

LRESULT C3d::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetFocus();
	m_abArcBall.BeginDrag(m_lMousex, m_lMousey);
	return TRUE;
}

LRESULT C3d::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_abArcBall.EndDrag();
	return TRUE;
}

LRESULT C3d::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return FALSE;
}

LRESULT C3d::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	m_lMousex = GET_X_LPARAM(lParam);
	m_lMousey = GET_Y_LPARAM(lParam);
	m_abArcBall.Mouse(m_lMousex, m_lMousey);
	return TRUE;
}

void C3d::SwapFillMode()
{
	static bool wmode = true;
	if(wmode)
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	wmode = !wmode;
}