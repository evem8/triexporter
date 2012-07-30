#pragma once

#include "stufffile.h"

#pragma pack(push, 1)

struct Header
{
	byte versionHi;
	byte versionLo;
	dword unknown1[3];
	dword sizeVertex; 
	dword numVertices;
	dword numSurfaces;
	dword numTriangles;
	dword unknown2[4];
	float minBox[3];
	float maxBox[3];
	dword unknown3[2];
	dword sizeHeader;
	dword unknown4;
};

struct Vertex
{
	float vertexPosition[3];
	float vertexNormal[3];
	float vertexUV[2];
};

struct VertexTB
{
	float vertexPosition[3];
	float vertexNormal[3];
	float vertexTangent[3];
	float vertexBinormal[3];
	float vertexUV[2];
};

//static const word vmaxsize = 128;
template<int size> struct CVertex: Vertex
{
	float unused[(size-32)/4];
};

struct Surface
{
	dword surfaceType;
	dword unknown[3];
	dword startIndex;
	dword numTriangles;
};

//struct Triangle
//{
//	word vertices[3];
//};

#pragma pack()

class TriFile
{
	public:
		bool hasTangentsBinormals;
		Header header;
		Vertex *m_vertices;
		vector<Surface> surfaces;
		vector< vector <vector<dword>> > triangles;
		vector<string> textures;
		dword numTriangles;

		TriFile():numTriangles(0), m_vertices(NULL){ ZeroMemory(&header, sizeof(Header));}
		void Clear();
		virtual ~TriFile();
		virtual bool LoadFile(StuffFileEntry &sfe);
		virtual bool LoadFile(string filename);
		virtual bool LoadFile(ifstream &is);
		void ExportX(float size, string file, string dir);
		void ExportObj(float size, string file, string dir);
		void Export3ds(float size, string file, string dir);
		void ExportMy(float size, string file, string dir);
		void ExportVbo(float size, string file, string dir);
		void ExportA3D(float size, string file, string dir);
		void ExportFBX(float size, string file, string dir);

		inline Vertex* verticesc(int i) const
		{
			//return &m_vertices[i];
			return reinterpret_cast<Vertex *>(reinterpret_cast<char *>(m_vertices) + i * header.sizeVertex);
		}

		inline VertexTB* verticestb(int i) const
		{
			//return &m_vertices[i];
			return reinterpret_cast<VertexTB *>(reinterpret_cast<char *>(m_vertices) + i * header.sizeVertex);
		}
};
