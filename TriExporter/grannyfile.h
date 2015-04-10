#pragma once

#include "SharedCache.h"
#include "trifile.h"


class GrannyTriFile : public TriFile
{
	public:
		GrannyTriFile();
		virtual bool LoadFile(CacheEntry &sce);
		virtual bool LoadFile(string filename);
		virtual bool LoadFile(ifstream &is);

	private:
		static bool loadedstuff;
		static bool dllloaded;
};