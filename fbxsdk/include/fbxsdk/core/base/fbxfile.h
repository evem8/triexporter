/****************************************************************************************
 
   Copyright (C) 2012 Autodesk, Inc.
   All rights reserved.
 
   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.
 
****************************************************************************************/

//! \file fbxfile.h
#ifndef _FBXSDK_CORE_BASE_FILE_H_
#define _FBXSDK_CORE_BASE_FILE_H_

#include <fbxsdk/fbxsdk_def.h>

#include <fbxsdk/core/base/fbxstring.h>

#include <fbxsdk/fbxsdk_nsbegin.h>

class FbxStream;

class FBXSDK_DLL FbxFile
{
public:
	enum EMode {eNone, eReadOnly, eReadWrite, eCreateWriteOnly, eCreateReadWrite, eCreateAppend};
	enum ESeekPos {eBegin, eCurrent, eEnd};

    FbxFile();
    virtual ~FbxFile();

    virtual bool        Open(const char* pFileName_UTF8, const EMode& pMode=eCreateReadWrite, const bool& pBinary=true);
    virtual bool        Open(FbxStream* pStream, void* pStreamData, const char* pMode);
    virtual bool        Close();

    virtual void		Seek(const FbxInt64& pOffset, const ESeekPos& pSeekPos=eBegin);
    virtual FbxInt64	Tell() const;

    virtual	size_t		Read(void* pDstBuf, const size_t& pSize);
	virtual char*		ReadString(char* pDstBuf, const size_t& pDstSize, bool pStopAtFirstWhiteSpace=false);
    virtual size_t		Write(const void* pSrcBuf, const size_t& pSize);
    virtual bool		WriteFormat(const char* pFormat, ...);

    virtual bool		Truncate(const FbxInt64& pSize);
    virtual bool		EndOfFile() const;
	virtual FbxInt64	GetSize();
	virtual void		GetMemoryFileInfo(void** pMemPtr, size_t& pSize);

    bool                IsOpen() const;
    bool                IsStream() const;
    const char*			GetFilePathName() const;
    EMode				GetFileMode() const;

    int                 GetLastError();
    void                ClearError();

protected:
	FILE*				mFilePtr;
    FbxStream*          mStreamPtr;
	bool                mIsOpen;
	bool                mIsStream;
	EMode				mMode;
	FbxString			mFileName;
};

class FBXSDK_DLL FbxFileUtils
{
public:
	/** Remove a file from disk.
	  * \param pFileName The file to be deleted.
	  * \return True if delete is successful.
	  */
    static bool Remove(const char* pFileName_UTF8);

	/** Rename a file on disk.
	  * \param pFileName_UTF8 The file to be renamed.
	  * \param pNewName_UTF8 The new file name upon rename.
	  * \return True if rename is successful.
	  */
    static bool Rename(const char* pFileName_UTF8, const char* pNewName_UTF8);

	/** Copy one file's content to another file (if the destination file not exist, it will be created).
	  * \param pDestination_UTF8 The destination file path
	  * \param pSource_UTF8 The source file path
	  * \return Return true if copy is successfully.
	  */
	static bool Copy(const char* pDestination_UTF8, const char* pSource_UTF8);

	//! Get given file's size.
	static FbxLong Size(const char* pFilePath_UTF8);

	/** Find if the specified file exist.
	  * \param pFilePath_UTF8 The file path to test against.
	  * \return Returns true if the file exist.
	  */
	static bool Exist(const char* pFilePath_UTF8);

	/** Find if the specified file is in read-only mode.
	  * \param pFilePath_UTF8 The file path to test against.
	  * \return Returns true if the file is in read-only mode.
	  */
	static bool IsReadOnly(const char* pFilePath_UTF8);

	// We return a KLong that in fact is a cast of a time_t.
	//! Get given file's last date.
	static FbxLong GetLastDate(const char* pPath_UTF8);

	//! Set the given file's last date as the given date.
	static bool SetLastDate(const char* pPath_UTF8, FbxLong pTime);

	/** Get some content of a file.
	  * \param pStr The content get from file.
	  * \param size The size of content.
	  * \param pStream The opened stream of file.
	  */
	static char* FGets(char* pStr, int pSize, FILE* pStream);
};

template<class T> inline const T FbxSwab(const T& x)
{
	switch( sizeof(x) )
	{
		case 2:
		{
			FbxUInt8 t[2];
			t[0] = ((FbxUInt8*)&x)[1];
			t[1] = ((FbxUInt8*)&x)[0];
			return *(T*)&t;
		}

		case 4:
		{
			FbxUInt8 t[4];
			t[0] = ((FbxUInt8*)&x)[3];
			t[1] = ((FbxUInt8*)&x)[2];
			t[2] = ((FbxUInt8*)&x)[1];
			t[3] = ((FbxUInt8*)&x)[0];
			return *(T*)&t;
		}

		case 8:
		{
			FbxUInt8 t[8];
			t[0] = ((FbxUInt8*)&x)[7];
			t[1] = ((FbxUInt8*)&x)[6];
			t[2] = ((FbxUInt8*)&x)[5];
			t[3] = ((FbxUInt8*)&x)[4];
			t[4] = ((FbxUInt8*)&x)[3];
			t[5] = ((FbxUInt8*)&x)[2];
			t[6] = ((FbxUInt8*)&x)[1];
			t[7] = ((FbxUInt8*)&x)[0];
			return *(T*)&t;
		}

		default:
			return x;
	}
}

#include <fbxsdk/fbxsdk_nsend.h>

#endif /* _FBXSDK_CORE_BASE_FILE_H_ */
