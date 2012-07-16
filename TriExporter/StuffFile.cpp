#include "stdafx.h"
#include "StuffFile.h"

void StuffFile::LoadDir(string dir)
{
	for_each(handles.begin(), handles.end(), close);
	handles.clear();
	HANDLE handle;
	filescount = 0;
	if(dir[dir.length()-1] != '\\')
		dir+="\\";
	WIN32_FIND_DATA find;
	handle =FindFirstFile((dir + "*.stuff").c_str(), &find);
	if (handle != INVALID_HANDLE_VALUE)
	{
		LoadFile(dir + find.cFileName);
		while(FindNextFile(handle, &find))
			LoadFile(dir + find.cFileName);
		FindClose(handle);
		loaded = true;
		std::sort(files.begin(), files.end());
	}
}

void func(char &c)
{
	if(c == '\\')
		c = '/';
}

void close(ifstream *in)
{
	if(in->is_open())
		in->close();
	delete in;
}

bool operator < (const StuffFileEntry& left, const StuffFileEntry& right)
{
	if  (left.filename.compare(right.filename)<0) 
		return true;
	return false;
}

StuffFile::~StuffFile()
{
	for_each(handles.begin(), handles.end(), close);
}

void StuffFile::LoadFile(const string &filename)
{
	ifstream* in = new ifstream();
	in->sync_with_stdio(false);
	in->open(filename.c_str(), ios::binary|ios::in);
	dword fileCount;
	in->read(reinterpret_cast<char *>(&fileCount), sizeof(dword));
	files.resize(filescount+fileCount);
	dword offset=0;
	for(dword i = 0; i < fileCount; i++)
	{
		char fn[256];
		in->read(reinterpret_cast<char *>(&files[filescount+i]), 2*sizeof(dword));
		in->read(reinterpret_cast<char *>(fn), files[filescount+i].filenameLength+1);
		files[filescount+i].filename = fn;
		transform(files[filescount+i].filename.begin(), files[filescount+i].filename.end(), files[filescount+i].filename.begin(), (int(*)(int))std::tolower);
		for_each(files[filescount+i].filename.begin(), files[filescount+i].filename.end(), func);
		files[filescount+i].fileOffset = offset;
		files[filescount+i].handle = in;
		offset += files[filescount+i].fileSize;
		files[filescount+i].stuffFilename = filename;
	}
	offset = (dword)in->tellg();
	for(dword i = 0; i < fileCount; i++)
		files[filescount+i].fileOffset += offset;
	filescount += fileCount;
	handles.push_back(in);
}

void StuffFileEntry::Unstuff(CString path, CString start, int makedir)
{
	CString file = filename.c_str();
	if(!makedir)
		file = "\\" + file.Right(file.GetLength()-file.ReverseFind('/')-1);
	else
	{
		file.Delete(0,start.GetLength());
		file.Replace("/", "\\");
		file = "\\" + file;
		int s =1;
		while((s = file.Find("\\",s+1)) >=0)
		{
			CString dir = path + file.Left(s);
			CreateDirectory(dir, NULL);
		}
	}
	vector<char> data;
	data.resize(fileSize);
	handle->seekg(fileOffset);
	handle->read(reinterpret_cast<char*>(&data[0]), fileSize);
	ofstream fout;
	fout.sync_with_stdio(false);
	fout.open(path + file, ios::binary);
	fout.write(reinterpret_cast<char*>(&data[0]), fileSize);
	fout.close();
}
