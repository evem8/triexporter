#include "stdafx.h"
#include "grannyfile.h"
#include <grannystuff/AutoGranny.h>
#include <grannystuff/Granny.h>
#include <vector>

bool GrannyTriFile::LoadFile(CacheEntry &sce)
{
	if(!dllloaded)
		return false;
	Clear();
	std::vector<char> mem;
	mem.resize(sce.fileSize);
	ifstream is;
	is.sync_with_stdio(false);
	is.open(sce.cachename.c_str(), ios::binary|ios::in);
	is.read(reinterpret_cast<char*>(&mem[0]), sce.fileSize);
	is.close();
	GrannyFile* file = (*GrannyReadEntireFileFromMemory)(sce.fileSize, reinterpret_cast<char*>(&mem[0]));
	mem.clear();

	t_FileInfo* modelinfo = (*GrannyGetFileInfo)(file);
	t_Meshes* mesh = modelinfo->Meshes[0];

	header.numVertices = (*GrannyGetMeshVertexCount)(mesh);
	t_Groups* triangleGroups = (*GrannyGetMeshTriangleGroups)(mesh);

	//ok for now we load gr2 with Tangents and Binormals
	//so size is always 56 ...
	header.sizeVertex = 56;
	//but we have also flag and we set it only in "TriFile" loaded from gr2 not wrom old .tri file
	hasTangentsBinormals = true;

	numTriangles = (*GrannyGetMeshTriangleCount)(mesh);
	header.numTriangles = numTriangles;
	header.numSurfaces = (*GrannyGetMeshTriangleGroupCount)(mesh);
	triangles.resize(header.numSurfaces);
	surfaces.resize(header.numSurfaces);
	
	int nIndices = (*GrannyGetMeshIndexCount)(mesh);
	vector<dword> ind;
	ind.resize(nIndices);
	(*GrannyCopyMeshIndices)(mesh, 4, &ind[0]);
	for(unsigned int i = 0; i < header.numSurfaces; i++)
	{
		triangles[i].resize(triangleGroups[i].TriCount);
		surfaces[i].numTriangles = triangleGroups[i].TriCount;
			
		for (unsigned int j = 0; j < surfaces[i].numTriangles; ++j)
		{
			int w = (triangleGroups[i].TriFirst + j)*3 ;
			triangles[i][j].resize(3);
			triangles[i][j][0] = ind[w];
			triangles[i][j][1] = ind[w+1];
			triangles[i][j][2] = ind[w+2];
		}
	}
	m_vertices =  new CVertex<56>[header.numVertices];
	//ok for now we load gr2 with Tangents and Binormals
	(*GrannyCopyMeshVertices)(mesh, GrannyPNGBT33332VertexType, m_vertices);
	(*GrannyFreeFile)(file);

	// Calculate values of bounding box
	for (unsigned int i = 0; i < header.numVertices; i++)
	{
		for (unsigned int x = 0; x < 3 ; x++)
		{
			float currpos = verticesc(i)->vertexPosition[x];

			if(header.minBox[x] > currpos)
				header.minBox[x] = currpos;
			if(header.maxBox[x] < currpos)
				header.maxBox[x] = currpos;
		}
	}

	return true;
}

bool GrannyTriFile::LoadFile(string filename)
{
	return false;
}

bool GrannyTriFile::LoadFile(std::ifstream &is)
{
	return false;
}

bool GrannyTriFile::loadedstuff = false;
bool GrannyTriFile::dllloaded = false;

GrannyTriFile::GrannyTriFile():TriFile()
{
	if(!loadedstuff)
	{
		dllloaded = LoadStuff();
		loadedstuff = true;
	}
}