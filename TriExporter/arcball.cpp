#include "stdafx.h"
#include "arcball.h"

ArcBall::ArcBall()
{
	Reset();
}

void ArcBall::Reset()
{
    D3DXQuaternionIdentity(&mD);
    D3DXQuaternionIdentity(&mN);
	bDrag = false;
}

void ArcBall::Size(int W, int H, float R)
{
	mW = W; 
    mH = H; 
    mR = R; 
	mC = D3DXVECTOR3(mW/2.0f,mH/2.0f,0.0f);
}

void ArcBall::BeginDrag(int x, int y)
{
	bDrag = true;
	vD = STV((float)x,(float)y);
}

void ArcBall::EndDrag()
{
	bDrag = false;
	mD = mN;
}

void ArcBall::Mouse(int x, int y)
{
	if (bDrag) 
	{
		vC = STV((float)x,(float)y);
		Update();
	}
}

void ArcBall::Update()
{
	if (bDrag)
	{
		D3DXVECTOR3 vP;
		D3DXVec3Cross(&vP, &vD, &vC);
		QuaternionMultiply(&mN, &(D3DXQUATERNION(vP.x, vP.y, vP.z, D3DXVec3Dot(&vD, &vC))), &mD);
	}
}
