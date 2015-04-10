#include "stdafx.h"
#include "SharedCache.h"

bool SharedCache::LoadDir(string eveDir)
{
	bool loadedDir = false;
	bool loadedIndex = false;
	string cacheDir;

	CRegKey rk;
	LONG lRet = rk.Open(HKEY_CURRENT_USER, "SOFTWARE\\CCP\\EVEONLINE");
	if (lRet == 0)
	{
		ULONG readLen = 256;
		TCHAR path[256];
		lRet = rk.QueryStringValue("CACHEFOLDER", path, &readLen);
		rk.Close();
		
		cacheDir = path;
		loadedDir = (lRet == 0);
	}

	if (loadedDir)
	{
		ifstream in(eveDir + "\\resfileindex.txt");
		if (in.good())
		{
			string line;
			CacheEntry file;
			char filename[255], cachename[255];
			int filesize = 0;
			while (!in.eof())
			{
				getline(in, line, '\n');
				sscanf(line.c_str(), "%255[^','],%255[^','],%*[^','],%d", filename, cachename, &filesize);
				
				file = CacheEntry();
				file.filename = filename;
				file.cachename = cacheDir + "ResFiles\\" + cachename;
				file.fileSize = filesize;
				index.push_back(file);
				line.clear();
			}
			loadedIndex = true;
		}
		in.close();
	}

	return loadedDir && loadedIndex;
}