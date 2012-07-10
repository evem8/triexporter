#pragma once
#include <d3dx9.h>
#include "d3dhelper.h"

class ArcBall 
{
	public:
		ArcBall();
		void Reset();
		void Size(int w, int h, float r=0.9);
		void BeginDrag(int x, int y);
		void EndDrag();
		void Mouse(int x, int y);
		inline void GetMat(D3DXMATRIXA16 *mat)
		{
			MatrixRotationQuaternion(mat, &(D3DXQUATERNION(-mN.x, -mN.y, -mN.z, mN.w)));
		};
		void Update();
	protected:
		inline D3DXVECTOR3 STV(float x, float y)
		{
			float z;
			int minWH = min(mW, mH);
			x = -((x - mC.x) / (minWH * mR * 0.5f));
			y = -((mC.y - y) / (minWH * mR * 0.5f));
			float mag = x*x + y*y;
			if (mag > 1.0) 
			{
				float scale = 1.0f/sqrtf(mag);
				x *= scale;
				y *= scale;
				z = 0.0;
			} 
			else 
				z = sqrtf((1.0f-mag));
			return D3DXVECTOR3(x,y,z);
		};
		D3DXVECTOR3 mC;
		D3DXVECTOR3 vD;
		D3DXVECTOR3 vC;
		D3DXQUATERNION mD;
		D3DXQUATERNION mN;
		int mW, mH;
		float mR;
		bool bDrag;
};