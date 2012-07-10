#pragma once
#include <d3dx9.h>
#include <ddraw.h>

struct CUSTOMVERTEX
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 uv;
};

inline void MatrixMultiply(D3DXMATRIXA16* m1, D3DXMATRIXA16* m2, D3DXMATRIXA16* res)
{
	res->_11=m1->_11 * m2->_11 + m1->_12 * m2->_21 + m1->_13 * m2->_31 + m1->_14 * m2->_41;
	res->_12=m1->_11 * m2->_12 + m1->_12 * m2->_22 + m1->_13 * m2->_32 + m1->_14 * m2->_42;
	res->_13=m1->_11 * m2->_13 + m1->_12 * m2->_23 + m1->_13 * m2->_33 + m1->_14 * m2->_43;
	res->_14=m1->_11 * m2->_14 + m1->_12 * m2->_24 + m1->_13 * m2->_34 + m1->_14 * m2->_44;
	res->_21=m1->_21 * m2->_11 + m1->_22 * m2->_21 + m1->_23 * m2->_31 + m1->_24 * m2->_41;
	res->_22=m1->_21 * m2->_12 + m1->_22 * m2->_22 + m1->_23 * m2->_32 + m1->_24 * m2->_42;
	res->_23=m1->_21 * m2->_13 + m1->_22 * m2->_23 + m1->_23 * m2->_33 + m1->_24 * m2->_43;
	res->_24=m1->_21 * m2->_14 + m1->_22 * m2->_24 + m1->_23 * m2->_34 + m1->_24 * m2->_44;
	res->_31=m1->_31 * m2->_11 + m1->_32 * m2->_21 + m1->_33 * m2->_31 + m1->_34 * m2->_41;
	res->_32=m1->_31 * m2->_12 + m1->_32 * m2->_22 + m1->_33 * m2->_32 + m1->_34 * m2->_42;
	res->_33=m1->_31 * m2->_13 + m1->_32 * m2->_23 + m1->_33 * m2->_33 + m1->_34 * m2->_43;
	res->_34=m1->_31 * m2->_14 + m1->_32 * m2->_24 + m1->_33 * m2->_34 + m1->_34 * m2->_44;
	res->_41=m1->_41 * m2->_11 + m1->_42 * m2->_21 + m1->_43 * m2->_31 + m1->_44 * m2->_41;
	res->_42=m1->_41 * m2->_12 + m1->_42 * m2->_22 + m1->_43 * m2->_32 + m1->_44 * m2->_42;
	res->_43=m1->_41 * m2->_13 + m1->_42 * m2->_23 + m1->_43 * m2->_33 + m1->_44 * m2->_43;
	res->_44=m1->_41 * m2->_14 + m1->_42 * m2->_24 + m1->_43 * m2->_34 + m1->_44 * m2->_44;
}
inline void MatrixTranspose(D3DXMATRIXA16* res)
{
	swap(res->_12, res->_21);
	swap(res->_13, res->_31);
	swap(res->_14, res->_41);
	swap(res->_14, res->_41);
	swap(res->_23, res->_32);
	swap(res->_24, res->_42);
	swap(res->_34, res->_43);
}

inline void MatrixTranslation(D3DXMATRIXA16* res, float x, float y, float z)
{
	res->_41+=x;
	res->_42+=y;
	res->_43+=z;
}

inline void Vec3Normalize(D3DVECTOR *out, const D3DVECTOR *res)
{
	//float l = sqrtf(res->x*res->x + res->y*res->y + res->z*res->z);
		float x = res->x*res->x + res->y*res->y + res->z*res->z;
#ifdef X64
	float xhalf = 0.5f*x;
	int i = *(int*)&x;
	i = 0x5f3759df - (i >> 1);
	x = *(float*)&i;
	x = x*(1.5f - xhalf*x*x);

#else
	__asm
{
		   fld x
		   fsqrt
		   fstp x
		}
#endif
	out->x = res->x/x;
	out->y = res->y/x;
	out->z = res->z/x;
}

inline void ComputeBoundingSphere(CUSTOMVERTEX* pVertices, DWORD NumVertices, D3DXVECTOR3 *pCenter, FLOAT *pRadius)
{
	
	D3DXVECTOR3 mn = pVertices[0].position;
	D3DXVECTOR3 mx = pVertices[0].position;
	for(DWORD i = 0; i < NumVertices; i++)
	{
		for(int c = 0; c < 3; c++)
		{
			mn[c] = min(mn[c], pVertices[i].position[c]);
			mx[c] = max(mx[c], pVertices[i].position[c]);
		}
	}
	(*pCenter) = (mx + mn)/2.0f;
	(*pRadius)=D3DXVec3Length(&(mx - *pCenter));
}

inline void MatrixRotationQuaternion(D3DXMATRIXA16* res, const D3DXQUATERNION *q)
{
	float xx = q->x * q->x;
	float xy = q->x * q->y;
	float xz = q->x * q->z;
	float xw = q->x * q->w;
	float yy = q->y * q->y;
	float yz = q->y * q->z;
	float yw = q->y * q->w;
	float zz = q->z * q->z;
	float zw = q->z * q->w;
	(*res)[0]  = 1 - 2 * ( yy + zz );
	(*res)[1]  =     2 * ( xy - zw );
	(*res)[2]  =     2 * ( xz + yw );
	(*res)[4]  =     2 * ( xy + zw );
	(*res)[5]  = 1 - 2 * ( xx + zz );
	(*res)[6]  =     2 * ( yz - xw );
	(*res)[8]  =     2 * ( xz - yw );
	(*res)[9]  =     2 * ( yz + xw );
	(*res)[10] = 1 - 2 * ( xx + yy );
	(*res)[3]  = (*res)[7] = (*res)[11] = (*res)[12] = (*res)[13] = (*res)[14] = 0;
	(*res)[15] = 1;
}

inline void QuaternionMultiply(D3DXQUATERNION *pOut, const D3DXQUATERNION *pQ1, const D3DXQUATERNION *pQ2)
{
	D3DXVECTOR3 va, vb, vc;
	pOut->w = pQ1->w * pQ2->w - D3DXVec3Dot( (D3DXVECTOR3 *)pQ1, (D3DXVECTOR3 *)pQ2);
	D3DXVec3Cross(&va, (D3DXVECTOR3 *)pQ1, (D3DXVECTOR3 *)pQ2);
	D3DXVec3Scale(&vb, (D3DXVECTOR3 *)pQ1, pQ2->w );
	D3DXVec3Scale(&vc, (D3DXVECTOR3 *)pQ2, pQ1->w );
	D3DXVec3Add(&va, &va, &vb);
	D3DXVec3Add((D3DXVECTOR3 *)pOut, &va, &vc );
}

inline void MatrixRotationX(D3DXMATRIXA16* res, float A)
{
	const float fRotX[16] = {
	1.0f,	0.0f,		0.0f,		0.0f,
	0.0f,	cosf(A),	sinf(A),	0.0f,
	0.0f,	-sinf(A),	cosf(A),	0.0f,
	0.0f,	0.0f,		0.0f,		1.0f};
	D3DXMATRIXA16 rotX(fRotX);
	D3DXMATRIXA16 tmp = *res;
	MatrixMultiply(&tmp, &rotX, res);
}

inline void MatrixRotationY(D3DXMATRIXA16* res, float A)
{
	const float fRotY[16] = {
	cosf(A),	0.0f,	-sinf(A),	0.0f,
	0.0f,		1.0f,	0.0f,		0.0f,
	sinf(A),	0.0f,	cosf(A),	0.0f,
	0.0f,		0.0f,	0.0f,		1.0f
	};
	D3DXMATRIXA16 rotY(fRotY);
	D3DXMATRIXA16 tmp = *res;
	MatrixMultiply(&tmp, &rotY, res);
}

inline void MatrixLookAtLH(D3DXMATRIXA16* res, const D3DXVECTOR3 *pEye, const D3DXVECTOR3 *pAt, const D3DXVECTOR3 *pUp)
{
	D3DXVECTOR3 zaxis, yaxis, xaxis;
	Vec3Normalize(&zaxis, &(*pAt - *pEye));
	D3DXVec3Cross(&xaxis, pUp, &zaxis);
	Vec3Normalize(&xaxis, &xaxis);
	D3DXVec3Cross(&yaxis, &zaxis, &xaxis);
	res->_11 = xaxis.x;
	res->_12 = yaxis.x;
	res->_13 = zaxis.x;
	res->_21 = xaxis.y;
	res->_22 = yaxis.y;
	res->_23 = zaxis.y;
	res->_31 = xaxis.z;
	res->_32 = yaxis.z;
	res->_33 = zaxis.z;
	res->_14 = res->_24 = res->_34 = 0;
	res->_41 = -D3DXVec3Dot(&xaxis, pEye);
	res->_42 = -D3DXVec3Dot(&yaxis, pEye);
	res->_43 = -D3DXVec3Dot(&zaxis, pEye);
	res->_44 = 1;
}

inline void MatrixPerspectiveFovLH(D3DXMATRIXA16* res, float fovy, float Aspect, float zn, float zf )
{
	float yScale = 1/tanf(fovy/2.0f);
	float xScale = Aspect * yScale;
	res->_11 = xScale;
	res->_22 = yScale;
	res->_33 = zf/(zf-zn);
	res->_43 = -zn*res->_33;
	res->_34 = 1.0f;
	res->_12 = res->_13 = res->_14 = res->_21 = res->_31 = res->_41 = res->_24 = res->_42 = res->_23 = res->_32 = res->_44 = 0;
}

inline bool LoadTextureFromMemory(LPDIRECT3DDEVICE9 m_pD3DDevice, byte* data, LPDIRECT3DTEXTURE9 *texture)
{
	D3DLOCKED_RECT pLockedRect;
	DDSURFACEDESC2 *ddDesc = (DDSURFACEDESC2 *)&data[4];
	void *dat = (void *)&data[128];
	DWORD size =  ddDesc->dwLinearSize;
	vector<dword> pixels;
	if(ddDesc->ddpfPixelFormat.dwFlags & DDPF_FOURCC)
		m_pD3DDevice->CreateTexture(ddDesc->dwWidth,ddDesc->dwHeight,ddDesc->dwMipMapCount,NULL, (D3DFORMAT)ddDesc->ddpfPixelFormat.dwFourCC, D3DPOOL_MANAGED, texture, NULL);
	else if(ddDesc->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
	{
		m_pD3DDevice->CreateTexture(ddDesc->dwWidth,ddDesc->dwHeight,ddDesc->dwMipMapCount,NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, texture, NULL);
		dword *map = (dword*)dat;
		byte *pixt = (byte*)&data[1152];
		pixels.resize(ddDesc->dwLinearSize);
		for(dword i = 0; i < ddDesc->dwLinearSize; i++)
			pixels[i] = map[pixt[i]];
		size = ddDesc->dwLinearSize*4;
		dat = (void*)&pixels[0];
	}
	else if(ddDesc->ddpfPixelFormat.dwFlags & DDPF_LUMINANCE)
	{
		m_pD3DDevice->CreateTexture(ddDesc->dwWidth,ddDesc->dwHeight,ddDesc->dwMipMapCount,NULL, D3DFMT_L8, D3DPOOL_MANAGED, texture, NULL);
	}
	else
	{
		//char ii[20];
		//itoa(ddDesc->ddpfPixelFormat.dwFlags, ii, 10);
		//MessageBox(0,ii,"",0);
		return false;
	}
	if(*texture)
	{
		(*texture)->LockRect(0,&pLockedRect, NULL,D3DLOCK_NOSYSLOCK);
		memcpy(pLockedRect.pBits,dat, size);
		(*texture)->UnlockRect(0);
		return true;
	}
	return false;
}