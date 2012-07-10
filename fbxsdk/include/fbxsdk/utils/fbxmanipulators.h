/****************************************************************************************
 
   Copyright (C) 2012 Autodesk, Inc.
   All rights reserved.
 
   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.
 
****************************************************************************************/

//! \file fbxmanipulators.h
#ifndef _FBXSDK_UTILS_MANIPULATORS_H_
#define _FBXSDK_UTILS_MANIPULATORS_H_

#include <fbxsdk/fbxsdk_def.h>

#include <fbxsdk/core/fbxobject.h>
#include <fbxsdk/core/math/fbxvector2.h>
#include <fbxsdk/core/math/fbxvector4.h>
#include <fbxsdk/scene/geometry/fbxcamera.h>

#include <fbxsdk/fbxsdk_nsbegin.h>

class FbxCameraManipulationState;

/** This class can be used to provide basic camera manipulation in any program using this library.
  * \nosubgrouping
  */
class FBXSDK_DLL FbxCameraManipulator : public FbxObject
{
	FBXSDK_OBJECT_DECLARE(FbxCameraManipulator, FbxObject);

public:
	enum EAction {eNone, eOrbit, eDolly, ePan};

    /** Set the camera used for the manipulation.
	  *	\param pCamera		Camera that will be used for the manipulation.
	  *	\param pCamAlign	How the camera should be aligned during camera manipulator initialization.
	  * \remark				The camera rotation or LookAt node position might be affected by calling
	  *						this function to make sure they align correctly together. */
	void Initialize(FbxCamera* pCamera);

    /** Begin manipulation of the camera.
	  *	\param	pAction	The action performed for this manipulation scope.
	  *	\param	pMouseX	Horizontal position of the mouse cursor.
	  *	\param	pMouseY	Vertical position of the mouse cursor.
	  *	\return If TRUE, manipulation successfully initialized. */
	bool Begin(const EAction& pAction, const int& pMouseX, const int& pMouseY);

    /** Notify manipulation of latest input.
	  *	\param pMouseX Horizontal position of the mouse cursor.
	  *	\param pMouseY Vertical position of the mouse cursor. */
	bool Notify(const int& pMouseX, const int& pMouseY);

    /** End current manipulation. */
	void End();

    /** Change camera rotation and LookAt node to frame all objects.
	  * \return True if the FrameAll action was successfully performed. */
	bool FrameAll();

    /** Change camera rotation and LookAt to frame all selected objects.
	  * \return True if the FrameSelected action was successfully performed. */
	bool FrameSelected();

	/** Retrieve the camera used by this manipulator.
	  * \return The camera currently in use.
	  */
	FbxCamera* GetCamera() const;

	/** Retrieve current manipulation action. */
	EAction GetCurrentAction() const;

/*****************************************************************************************************************************
** WARNING! Anything beyond these lines is for internal use, may not be documented and is subject to change without notice! **
*****************************************************************************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
	virtual FbxObject& Copy(const FbxObject& pObject);

protected:
	virtual void Construct(const FbxCameraManipulator* pFrom);
    virtual void Destruct(bool pRecursive);

private:
	FbxCameraManipulationState* mState;
#endif /* !DOXYGEN_SHOULD_SKIP_THIS *****************************************************************************************/
};

#include <fbxsdk/fbxsdk_nsend.h>

#endif /* _FBXSDK_UTILS_MANIPULATORS_H_ */
