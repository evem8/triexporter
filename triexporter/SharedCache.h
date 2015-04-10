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

		vector<CacheEntry> index;
		vector<ifstream*> handles;
};