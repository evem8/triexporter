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

void SharedCache::SaveFile(CacheEntry file, CString path, bool folder)
{
	CString outpath;
	if (folder)
	{
		CString filename = file.filename.c_str();
		if (filename.Left(4).Compare("res:") == 0)
		{
			filename = filename.Mid(4);
		}
		filename.Replace("/", "\\");
		outpath = path + filename;

		int s = 1;
		while ((s = filename.Find("\\", s+1)) >= 0)
		{
			CString dir = path + filename.Left(s);
			CreateDirectory(dir, NULL);
		}
	}
	else
	{
		outpath = path;
	}

	vector<char> data;
	data.resize(file.fileSize);
	ifstream is;
	is.sync_with_stdio(false);
	is.open(file.cachename.c_str(), ios::binary|ios::in);
	is.read(reinterpret_cast<char*>(&data[0]), file.fileSize);
	is.close();

	ofstream fout;
	fout.sync_with_stdio(false);
	fout.open(outpath, ios::binary);
	fout.write(reinterpret_cast<char*>(&data[0]), file.fileSize);
	fout.close();
}