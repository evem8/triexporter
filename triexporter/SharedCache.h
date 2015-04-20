#pragma once

struct CacheEntry
{
	dword fileSize;
	string filename;
	string cachename;
};

class SharedCache
{
	public:
		bool LoadDir(string eveDir);
		void SaveFile(CacheEntry file, CString path, bool folder = false);

		vector<CacheEntry> index;
		vector<ifstream*> handles;
};