#pragma once

#include "stufffile.h"
#include "trifile.h"



class GrannyTriFile : public TriFile
{
	public:
		GrannyTriFile();
		virtual bool LoadFile(StuffFileEntry &sfe);
		virtual bool LoadFile(string filename);
		virtual bool LoadFile(ifstream &is);
	private:
		static bool loadedstuff;
		static bool dllloaded;
	
};