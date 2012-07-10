#pragma once

struct StuffFileEntry
{
	dword fileSize;
	dword filenameLength;
	string filename;
	string stuffFilename;
	dword fileOffset;
	ifstream* handle;
	void Unstuff(CString path, CString start = "", int makedir=0);
};

class StuffFile
{
	public:
		dword filescount;
		bool loaded;
		vector<StuffFileEntry> files;
		vector<ifstream*> handles;
		StuffFile():loaded(false){}
		void LoadDir(string dir);
		void LoadFile(const string &filename);
		~StuffFile();
};

void close(ifstream *in);
void func(char &c);

bool operator < (const StuffFileEntry& left, const StuffFileEntry& right);
