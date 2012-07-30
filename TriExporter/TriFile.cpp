#include "stdafx.h"
#include "TriFile.h"
#include "lib3ds/file.h"
#include "lib3ds/mesh.h"
#include "lib3ds/material.h"
#define FBXSDK_NEW_API
#include <fbxsdk.h>

template<class T>
int inline findAndReplace(T& source, const T& find, const T& replace)
{
	int num=0;
	typename T::size_type fLen = find.size();
	typename T::size_type rLen = replace.size();
	for (T::size_type pos=0; (pos=source.find(find, pos))!=T::npos; pos+=rLen)
	{
		num++;
		source.replace(pos, fLen, replace);
	}
	return num;
}

void TriFile::Clear()
{
	numTriangles = 0;
	if(m_vertices != NULL)
		delete [] m_vertices;
	m_vertices = NULL;
}

TriFile::~TriFile()
{
	Clear();
}

bool TriFile::LoadFile(StuffFileEntry &sfe)
{
	sfe.handle->seekg(sfe.fileOffset);
	return LoadFile((*sfe.handle));
}

bool TriFile::LoadFile(string filename)
{
	ifstream is;
	is.sync_with_stdio(false);
	is.open (filename.c_str(), ios::binary|ios::in);
	bool ret = LoadFile(is);
	is.close();
	return ret;
}

bool TriFile::LoadFile(ifstream &is)
{
	Clear();
	is.read(reinterpret_cast<char*>(&header), sizeof(header));
	if(header.versionLo != 4 && header.versionHi != 1)
		return false;
	surfaces.resize(header.numSurfaces);
	//sorry, actually we need this for loading old tri files :)
	//we should prolly add SCALE_AND_CENTER and vertextype as TriFile propertis
	//instead #define
	switch(header.sizeVertex)
	{
		case 32:
			m_vertices = new Vertex[header.numVertices];
			break;
		case 40:
			m_vertices = new CVertex<40>[header.numVertices];
			break;
		case 48:
			m_vertices = new CVertex<48>[header.numVertices];
			break;
		case 56:
			m_vertices = new CVertex<56>[header.numVertices];
			break;
		case 64:
			m_vertices = new CVertex<64>[header.numVertices];
			break;
		case 72:
			m_vertices = new CVertex<72>[header.numVertices];
			break;
		default:
			return false;
	}
	hasTangentsBinormals = false;
	int r = header.sizeVertex*header.numVertices;
	is.read(reinterpret_cast<char*>(m_vertices), r);
	r = sizeof(surfaces[0])*header.numSurfaces;
	is.read(reinterpret_cast<char*>(&surfaces[0]), r);
	triangles.resize(header.numSurfaces);
	textures.resize(header.numSurfaces);
	for(dword i = 0; i < header.numSurfaces; i++)
	{
		int len = 0;
		r = 0;
		if(surfaces[i].surfaceType == 5)
		{
			len = 2;
			r = len* (surfaces[i].numTriangles+ 2);
		}
		else
		{
			len = 3;
			r = len* surfaces[i].numTriangles*2;
		}
		vector<word> list;
		list.resize(surfaces[i].numTriangles*len);
		is.read(reinterpret_cast<char*>(&list[0]), r);
		triangles[i].resize(surfaces[i].numTriangles);
		int flush = 0;
		if(surfaces[i].surfaceType == 5)
		{
			dword count = 0;
			for(dword c = 0; c <  surfaces[i].numTriangles; c++)
			{
				triangles[i][c].resize(3);
				if(list[c] != list[c+1] && list[c] != list[c+2] && list[c+1] != list[c+2])
				{
				if((flush%2) ==0)
				{
					triangles[i][count][0] = list[c];
					triangles[i][count][1] = list[c+1];
					triangles[i][count][2] = list[c+2];
				}
				else
				{
					triangles[i][count][0] = list[c+2];
					triangles[i][count][1] = list[c+1];
					triangles[i][count][2] = list[c];
				}
				count++;
				}
				flush++;

			}
			surfaces[i].numTriangles = count;
		}
		else
		{
			for(dword c = 0; c <  surfaces[i].numTriangles; c++)
			{
				triangles[i][c].resize(3);
				triangles[i][c][0] = list[c*3];
				triangles[i][c][1] = list[c*3+1];
				triangles[i][c][2] = list[c*3+2];
			}
		}
		numTriangles += surfaces[i].numTriangles;
	}
	return true;
}

void TriFile::ExportX(float size, string file, string dir)
{
	string xheader = "xof 0303txt 0032\ntemplate Vector {\n <3d82ab5e-62da-11cf-ab39-0020af71e433>\n FLOAT x;\n FLOAT y;\n FLOAT z;\n}\n\ntemplate MeshFace {\n <3d82ab5f-62da-11cf-ab39-0020af71e433>\n DWORD nFaceVertexIndices;\n array DWORD faceVertexIndices[nFaceVertexIndices];\n}\n\ntemplate Mesh {\n <3d82ab44-62da-11cf-ab39-0020af71e433>\n DWORD nVertices;\n array Vector vertices[nVertices];\n DWORD nFaces;\n array MeshFace faces[nFaces];\n [...]\n}\n\ntemplate MeshNormals {\n <f6f23f43-7686-11cf-8f52-0040333594a3>\n DWORD nNormals;\n array Vector normals[nNormals];\n DWORD nFaceNormals;\n array MeshFace faceNormals[nFaceNormals];\n}\n\ntemplate Coords2d {\n <f6f23f44-7686-11cf-8f52-0040333594a3>\n FLOAT u;\n FLOAT v;\n}\n\ntemplate MeshTextureCoords {\n <f6f23f40-7686-11cf-8f52-0040333594a3>\n DWORD nTextureCoords;\n array Coords2d textureCoords[nTextureCoords];\n}\n\ntemplate ColorRGBA {\n <35ff44e0-6c7c-11cf-8f52-0040333594a3>\n FLOAT red;\n FLOAT green;\n FLOAT blue;\n FLOAT alpha;\n}\n\ntemplate ColorRGB {\n <d3e16e81-7835-11cf-8f52-0040333594a3>\n FLOAT red;\n FLOAT green;\n FLOAT blue;\n}\n\ntemplate Material {\n <3d82ab4d-62da-11cf-ab39-0020af71e433>\n ColorRGBA faceColor;\n FLOAT power;\n ColorRGB specularColor;\n ColorRGB emissiveColor;\n [...]\n}\n\ntemplate MeshMaterialList {\n <f6f23f42-7686-11cf-8f52-0040333594a3>\n DWORD nMaterials;\n DWORD nFaceIndexes;\n array DWORD faceIndexes[nFaceIndexes];\n [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]\n}\n\ntemplate TextureFilename {\n <a42790e1-7810-11cf-8f52-0040333594a3>\n STRING filename;\n}\n\n\nMesh {\n";
	string xmaterial1 = "  Material {\n   1.000000;1.000000;1.000000;1.000000;;\n   256.000000;\n   1.000000;1.000000;1.000000;;\n   0.000000;0.000000;0.000000;;\n\n   TextureFilename {\n    \"";
	string xmaterial2 = "\";\n   }\n  }\n";
	string xfooter = " }\n}";
	string xnormals = " MeshNormals {";
	string xlendlast = ";;";
	string xlend = ";,";
	ofstream out;
	out.sync_with_stdio(false);
	out.open((dir + file + ".x").c_str());
	out << xheader << endl;
	out << header.numVertices << ";" << endl;
	for(dword i = 0; i < header.numVertices; i ++)
	{
		if((i+1) != header.numVertices)
			out << " " << fixed << setprecision(6) << verticesc(i)->vertexPosition[0]*size << ";" << setprecision(6) << verticesc(i)->vertexPosition[1]*size << ";" << setprecision(6) << verticesc(i)->vertexPosition[2]*size << xlend <<endl;
		else
			out << " " << setprecision(6) << verticesc(i)->vertexPosition[0]*size << ";" << setprecision(6) << verticesc(i)->vertexPosition[1]*size << ";" << setprecision(6) << verticesc(i)->vertexPosition[2]*size << xlendlast <<endl;
	}
	out << numTriangles << ";" << endl;
	for(dword i = 0; i < header.numSurfaces; i++)
	{
		for(dword c = 0; c < surfaces[i].numTriangles; c ++)
		{
			if(((i+1) == header.numSurfaces) && ((c+1) == surfaces[i].numTriangles))
				out << " 3;" << triangles[i][c][0] << "," << triangles[i][c][1] << "," << triangles[i][c][2] << xlendlast << endl;
			else
				out << " 3;" << triangles[i][c][0] << "," << triangles[i][c][1] << "," << triangles[i][c][2] << xlend << endl;
		}
	}
	out << xnormals << endl;
	out << "  " << header.numVertices << ";" << endl;
	for(dword i = 0; i < header.numVertices; i ++)
	{
		if((i+1) != header.numVertices)
			out << "  "  << fixed << setprecision(6) << verticesc(i)->vertexNormal[0] << ";" << setprecision(6) << verticesc(i)->vertexNormal[1] << ";" << setprecision(6) << verticesc(i)->vertexNormal[2] << xlend << endl;
		else
			out << "  " << setprecision(6) << verticesc(i)->vertexNormal[0] << ";" << setprecision(6) << verticesc(i)->vertexNormal[1] << ";" << setprecision(6) << verticesc(i)->vertexNormal[2] << xlendlast << endl;
	}
	out << numTriangles << ";" << endl;
	for(dword i = 0; i < header.numSurfaces; i++)
	{
		for(dword c = 0; c < surfaces[i].numTriangles; c ++)
		{
			if(((i+1) == header.numSurfaces) && ((c+1) == surfaces[i].numTriangles))
				out << " 3;" << triangles[i][c][0] << "," << triangles[i][c][1] << "," << triangles[i][c][2] << xlendlast << endl;
			else
				out << " 3;" << triangles[i][c][0] << "," << triangles[i][c][1] << "," << triangles[i][c][2] << xlend << endl;
		}
	}
	out << "}\n\n MeshTextureCoords {" << endl;
	out << "  " <<header.numVertices << ";" << endl;
	for(dword i = 0; i < header.numVertices; i ++)
	{
		if((i+1) != header.numVertices)
			out << fixed << setprecision(6) <<  verticesc(i)->vertexUV[0] << "; " << setprecision(6) <<  verticesc(i)->vertexUV[1] << xlend << endl;
		else
			out << setprecision(6) << verticesc(i)->vertexUV[0] << "; " << setprecision(6) <<  verticesc(i)->vertexUV[1] << xlendlast << endl;
	}
	out << "}\n\nMeshMaterialList {\n" << header.numSurfaces << ";\n" << numTriangles << ";" << endl;
	for(dword i = 0; i < header.numSurfaces; i++)
	{
		for(dword c = 0; c < surfaces[i].numTriangles; c++)
		{
			if((c+1) != surfaces[i].numTriangles)
				out << i << ","  << endl;
			else
				out << i << ";" << endl;
		}
	}
	for(dword c = 0; c < header.numSurfaces; c++)
	{
		out << xmaterial1 << textures[c] << xmaterial2;
	}
	out << xfooter;
	out.close();
}

void TriFile::ExportMy(float size, string file, string dir)
{
	//actually this is usles ... just tests for: simple format or WPF Geometry ... i'll get rid of it from menu
	ofstream out;
	out.sync_with_stdio(false);
	out.open((dir + file + ".my").c_str(), std::ios::ate);
	/*
	out.open((dir + file + ".my").c_str(), std::ios::binary | std::ios::ate);
	out.write(reinterpret_cast<char*>(&header.numVertices), sizeof(dword)) ;
	for(dword i = 0; i < header.numVertices; i ++)
	{
		float fo = vertices(i)->vertexPosition[0]*size;
		out.write(reinterpret_cast<char*>(&fo), sizeof(float));
		fo = vertices(i)->vertexPosition[1]*size;
		out.write(reinterpret_cast<char*>(&fo), sizeof(float));
		fo = vertices(i)->vertexPosition[2]*size;
		out.write(reinterpret_cast<char*>(&fo), sizeof(float));
		fo = vertices(i)->vertexNormal[0];
		out.write(reinterpret_cast<char*>(&fo), sizeof(float));
		fo = vertices(i)->vertexNormal[1];
		out.write(reinterpret_cast<char*>(&fo), sizeof(float));
		fo = vertices(i)->vertexNormal[2];
		out.write(reinterpret_cast<char*>(&fo), sizeof(float));
		fo =  vertices(i)->vertexUV[0];
		out.write(reinterpret_cast<char*>(&fo), sizeof(float));
		fo =  vertices(i)->vertexUV[1];
		out.write(reinterpret_cast<char*>(&fo), sizeof(float));
	}
	out.write(reinterpret_cast<char*>(&header.numSurfaces), sizeof(dword));
	for(dword i = 0; i < header.numSurfaces; i++)
	{
		out.write(reinterpret_cast<char*>(&surfaces[i].numTriangles), sizeof(dword));
		for(dword c = 0; c < surfaces[i].numTriangles; c ++)
		{
			for(int d = 0; d < 3; d++)
			{
				dword p = triangles[i][c][d];
				out.write(reinterpret_cast<char*>(&p), sizeof(dword));
			}
		}
	}*/
	out << "Position=\"";
	for(dword i = 0; i < header.numVertices; i ++)
	{
		out << " " << fixed << setprecision(6) << verticesc(i)->vertexPosition[0]*size << " " << setprecision(6) << verticesc(i)->vertexPosition[1]*size << " " << setprecision(6) << verticesc(i)->vertexPosition[2]*size;
	}
	out << "\"" << endl;
	out << "Normals=\"";
	for(dword i = 0; i < header.numVertices; i ++)
	{
		out << " " << fixed << setprecision(6) << verticesc(i)->vertexNormal[0]*size << ", " << setprecision(6) << verticesc(i)->vertexNormal[1]*size << ", " << setprecision(6) << verticesc(i)->vertexPosition[2]*size;
	}
	out << "\"" << endl;
	out << "TriangleIndices=\"";
	for(dword i = 0; i < header.numSurfaces; i++)
	{
		for(dword c = 0; c < surfaces[i].numTriangles; c ++)
		{
			for(int d = 0; d < 3; d++)
			{
				dword p = triangles[i][c][d];
				out << p << " ";
			}
		}
	}
	out << "\"" << endl;
	out.close();
}

template <typename T> void swap_endian(T& pX)
{
	char& raw = reinterpret_cast<char&>(pX);
	std::reverse(&raw, &raw + sizeof(T));
}

void TriFile::ExportVbo(float size, string file, string dir)
{
	char file_id[11] = "VBO_ToLuSe";
	int file_version = 3;
	float mesh_scale = size;
	int mesh_elements = header.numVertices;
	int mesh_surfaces = header.numSurfaces;
	int mesh_faces_of_surface = 0;
	int mesh_faces = 0;
	byte index_type;

#if SCALE_AND_CENTER
	// Center model in the middle of the bounding box
	float offsetX = - ( header.minBox[0] + header.maxBox[0] ) / 2;
	float offsetY = - ( header.minBox[1] + header.maxBox[1] ) / 2;
	float offsetZ = - ( header.minBox[2] + header.maxBox[2] ) / 2;

	// Scale model by average size in x and y direction
	float deltaX = header.maxBox[0] - header.minBox[0];
	float deltaY = header.maxBox[1] - header.minBox[1];
	float autoscale = 7.5f / ((deltaX+deltaY)/2);
	mesh_scale *= autoscale;

	// Only shrink
	if (1.0f < mesh_scale)
		mesh_scale = size;
#else
	float offsetX = 0.0f;
	float offsetY = 0.0f;
	float offsetZ = 0.0f;
#endif

	ofstream out;
	out.sync_with_stdio(false);
	out.open((dir + file + ".vbo").c_str(), std::ios::binary | std::ios::ate);

	// Write file_id
	out.write(reinterpret_cast<char*>(&file_id), sizeof(file_id)-1);

	// Write file_version
	swap_endian(file_version);
	out.write(reinterpret_cast<char*>(&file_version), sizeof(file_version));

	// Write mesh_scale
	float mesh_scale_big = mesh_scale;
	swap_endian(mesh_scale_big);
	out.write(reinterpret_cast<char*>(&mesh_scale_big), sizeof(mesh_scale_big));
	
	// Write mesh_elements
	swap_endian(mesh_elements);
	out.write(reinterpret_cast<char*>(&mesh_elements), sizeof(mesh_elements));

	// Write mesh_surfaces
	swap_endian(mesh_surfaces);
	out.write(reinterpret_cast<char*>(&mesh_surfaces), sizeof(mesh_scale));

	// Write mesh_faces[mesh_surfaces]
	for(dword i = 0; i < header.numSurfaces; i++)
	{
		mesh_faces_of_surface = surfaces[i].numTriangles;
		swap_endian(mesh_faces_of_surface);
		out.write(reinterpret_cast<char*>(&mesh_faces_of_surface), sizeof(mesh_faces_of_surface));

		// Calculate total number of faces
		mesh_faces += surfaces[i].numTriangles;
	}

	// Set packing of indexes
	if( mesh_faces * 3 > 65535 )
	{
		// Pack indexes as unsigned int
		index_type = 0;
	}
	else
	{
		// Pack indexes as unsigned short
		index_type = 1;
	}

	// Write index_type
	out.write(reinterpret_cast<char*>(&index_type), sizeof(index_type));

	// For all vertices
	for(dword i = 0; i < header.numVertices; i ++)
	{
		// Write vertex_coordinates
		float fo = (verticesc(i)->vertexPosition[0]+offsetX)*mesh_scale;
		swap_endian(fo);
		out.write(reinterpret_cast<char*>(&fo), sizeof(float));
		fo = (verticesc(i)->vertexPosition[1]+offsetY)*mesh_scale;
		swap_endian(fo);
		out.write(reinterpret_cast<char*>(&fo), sizeof(float));
		fo = (verticesc(i)->vertexPosition[2]+offsetZ)*mesh_scale;
		swap_endian(fo);
		out.write(reinterpret_cast<char*>(&fo), sizeof(float));

		// Write vertex_normals
		fo = verticesc(i)->vertexNormal[0];
		swap_endian(fo);
		out.write(reinterpret_cast<char*>(&fo), sizeof(float));
		fo = verticesc(i)->vertexNormal[1];
		swap_endian(fo);
		out.write(reinterpret_cast<char*>(&fo), sizeof(float));
		fo = verticesc(i)->vertexNormal[2];
		swap_endian(fo);
		out.write(reinterpret_cast<char*>(&fo), sizeof(float));

		// Write vertex_uv
		fo =  verticesc(i)->vertexUV[0];
		swap_endian(fo);
		out.write(reinterpret_cast<char*>(&fo), sizeof(float));
		fo =  verticesc(i)->vertexUV[1];
		swap_endian(fo);
		out.write(reinterpret_cast<char*>(&fo), sizeof(float));
	}

	// For all surfaces
	for(dword i = 0; i < header.numSurfaces; i++)
	{
		for(dword c = 0; c < surfaces[i].numTriangles; c ++)
		{
			// Write faces
			for(int d = 0; d < 3; d++)
			{
				if (0 == index_type)
				{
					unsigned int p = (unsigned int) triangles[i][c][d];
					swap_endian(p);
					out.write(reinterpret_cast<char*>(&p), sizeof(unsigned int));
				}
				else
				{
					unsigned short p = (unsigned short) triangles[i][c][d];
					swap_endian(p);
					out.write(reinterpret_cast<char*>(&p), sizeof(unsigned short));
				}
			}
		}
	}

	out.close();
}

void TriFile::ExportObj(float size, string file, string dir)
{
	ofstream out;
	out.sync_with_stdio(false);
	ofstream outmtl;
	outmtl.sync_with_stdio(false);
	out.open((dir + file + ".obj").c_str());
	outmtl.open((dir + file + ".mtl").c_str());
	out << "mtllib " << file << ".mtl" << endl;
	out << "g shape" << endl;

#if 0 // SCALE_AND_CENTER
	// Center model in the middle of the bounding box
	float offsetX = - ( header.minBox[0] + header.maxBox[0] ) / 2;
	float offsetY = - ( header.minBox[1] + header.maxBox[1] ) / 2;
	float offsetZ = - ( header.minBox[2] + header.maxBox[2] ) / 2;

	// Scale model by average size in x and y direction
	float deltaX = header.maxBox[0] - header.minBox[0];
	float deltaY = header.maxBox[1] - header.minBox[1];
	size = 7.5f / ((deltaX+deltaY)/2);
#else
	float offsetX = 0.0f;
	float offsetY = 0.0f;
	float offsetZ = 0.0f;
#endif

	for(dword i = 0; i < header.numVertices; i ++)
		out << "v " << (verticesc(i)->vertexPosition[0]+offsetX)*size << " " << (verticesc(i)->vertexPosition[1]+offsetY)*size << " " << (verticesc(i)->vertexPosition[2]+offsetZ)*size << endl;
	for(dword i = 0; i < header.numVertices; i ++)
		out << "vt " << verticesc(i)->vertexUV[0] << " " << verticesc(i)->vertexUV[1] << endl;
	for(dword i = 0; i < header.numVertices; i ++)
		out << "vn " << verticesc(i)->vertexNormal[0] << " " << verticesc(i)->vertexNormal[1] << " " << verticesc(i)->vertexNormal[2] << endl;
#if 0 // Export tangents // there is no tg element in obj file... i think
	for(dword i = 0; i < header.numVertices; i ++)
		out << "tg " << vertices(i)->vertexTangent[0] << " " << vertices(i)->vertexTangent[1] << " " << vertices(i)->vertexTangent[2] << endl;
#endif
	for(dword i = 0; i < header.numSurfaces; i++)
	{
		out << "usemtl shape" << i << endl;
		outmtl <<"newmtl shape" << i << "\nKa 0.5 0.5 0.5\nKd 1 1 1\nKs 1 1 1\nillum 7\nNs 256\nmap_Kd " << textures[i] << "\n";
		for(dword c = 0; c < surfaces[i].numTriangles; c ++)
		{
			dword t1 = triangles[i][c][0] + 1;
			dword t2 = triangles[i][c][1] + 1;
			dword t3 = triangles[i][c][2] + 1;
			if(! (t1 == t2  || t2 == t3 || t1 == t3))
				out << "f " <<  t1 << "/" << t1 << "/" << t1 << " " << t2 << "/" << t2 << "/" << t2 << " " << t3 << "/" << t3 << "/" << t3 << endl;
		}
	}
	out.close();
	outmtl.close();
}

void TriFile::Export3ds(float size, string file, string dir)
{
	Lib3dsFile *ds3 = lib3ds_file_new();
	Lib3dsMesh *mesh = lib3ds_mesh_new("shape");
	lib3ds_mesh_new_point_list(mesh, header.numVertices);
	lib3ds_mesh_new_texel_list(mesh, header.numVertices);
	Lib3dsPoint* pointL = mesh->pointL;
	for(dword i = 0; i < header.numVertices; i ++)
	{
		mesh->pointL[i].pos[0] = verticesc(i)->vertexPosition[0]*size;
		mesh->pointL[i].pos[1] = verticesc(i)->vertexPosition[1]*size;
		mesh->pointL[i].pos[2] = verticesc(i)->vertexPosition[2]*size;
		mesh->texelL[i][0] = verticesc(i)->vertexUV[0];
		mesh->texelL[i][1] = verticesc(i)->vertexUV[1];
	}
	int count = 0;
	lib3ds_mesh_new_face_list(mesh, numTriangles);
	for(dword i = 0; i < header.numSurfaces; i++)
	{
		Lib3dsMaterial *canvas = lib3ds_material_new();
		stringstream tmp;
		string name;
		tmp << i;
		tmp >> name;
		name = "shape" + name;
		strncpy_s(canvas->name, name.c_str(), sizeof(canvas->name));
		canvas->ambient[0] = canvas->ambient[1] = canvas->ambient[2] = 1.f;
		canvas->diffuse[0] = canvas->diffuse[1] = canvas->diffuse[2] = 1.f;
		canvas->specular[0] = canvas->specular[1] = canvas->specular[2] = 0.5f;
		canvas->two_sided = 0;
		strncpy_s(canvas->texture1_map.name, textures[i].c_str(), sizeof(canvas->texture1_map.name));
		strncpy_s(canvas->texture2_map.name, textures[i].c_str(), sizeof(canvas->texture2_map.name));
		lib3ds_file_insert_material(ds3, canvas);
		for(dword c = 0; c < surfaces[i].numTriangles; c ++)
		{
				strncpy_s(mesh->faceL[count].material, name.c_str(), sizeof(mesh->faceL[count].material));
				mesh->faceL[count].points[0] = (word)triangles[i][c][0];
				mesh->faceL[count].points[1] = (word)triangles[i][c][1];
				mesh->faceL[count].points[2] = (word)triangles[i][c][2];
				count++;
		}
	}
	lib3ds_file_insert_mesh(ds3, mesh);
	ds3->meshes = mesh;
	lib3ds_file_save(ds3, (dir + file + ".3ds").c_str());
	lib3ds_file_free(ds3);
}

void TriFile::ExportA3D(float size, string file, string dir)
{
	size *= 0.03f;
	const byte allocInfo[] =   {(byte)0x04, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x02, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x01, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, 
								(byte)0x08, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x70, (byte)0x6F, (byte)0x73, (byte)0x69, (byte)0x74, (byte)0x69, (byte)0x6F, (byte)0x6E, (byte)0x01, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x02, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, //position
								(byte)0x06, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x6E, (byte)0x6F, (byte)0x72, (byte)0x6D, (byte)0x61, (byte)0x6C, (byte)0x00, (byte)0x00, (byte)0x01, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x02, (byte)0x00, (byte)0x01, (byte)0x00, (byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, //normal
								(byte)0x08, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x74, (byte)0x65, (byte)0x78, (byte)0x74, (byte)0x75, (byte)0x72, (byte)0x65, (byte)0x30, (byte)0x01, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x02, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x02, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00};//texture0
	const byte allocInfo44[] = {(byte)0x04, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x02, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x01, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x04, (byte)0x00, (byte)0x00, (byte)0x00, 
								(byte)0x08, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x70, (byte)0x6F, (byte)0x73, (byte)0x69, (byte)0x74, (byte)0x69, (byte)0x6F, (byte)0x6E, (byte)0x01, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x02, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, //position
								(byte)0x06, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x6E, (byte)0x6F, (byte)0x72, (byte)0x6D, (byte)0x61, (byte)0x6C, (byte)0x00, (byte)0x00, (byte)0x01, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x02, (byte)0x00, (byte)0x01, (byte)0x00, (byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, //normal
								(byte)0x07, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x74, (byte)0x61, (byte)0x6E, (byte)0x67, (byte)0x65, (byte)0x6E, (byte)0x74, (byte)0x00, (byte)0x01, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x02, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, //tangent
								(byte)0x08, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x74, (byte)0x65, (byte)0x78, (byte)0x74, (byte)0x75, (byte)0x72, (byte)0x65, (byte)0x30, (byte)0x01, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x02, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x02, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00};//texture0
	const byte surfaceInfo[] = {(byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x01, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x04, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x08, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x6C, (byte)0x61, (byte)0x6D, (byte)0x62, (byte)0x65, (byte)0x72, (byte)0x74, (byte)0x31, (byte)0x02, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x09, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x01, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00};
	const byte surfaceInfo2[] ={(byte)0x03, (byte)0x00,							(byte)0x01, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x04, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x08, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x6C, (byte)0x61, (byte)0x6D, (byte)0x62, (byte)0x65, (byte)0x72, (byte)0x74, (byte)0x31, (byte)0x02, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x03, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x09, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x01, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00};
	ofstream out;
	out.sync_with_stdio(false);
	out.open((dir + file + ".a3d").c_str(), std::ios::binary | std::ios::ate);
	const char android3d_ff[] = "Android3D_ff";
	out.write(android3d_ff, sizeof(android3d_ff) - 1);
	qword tmp64 = 44; //header len
	char* tmp64aspointer = reinterpret_cast<char*>(&tmp64);
	out.write(tmp64aspointer, sizeof(tmp64));
	dword tmp = 0; //version maj
	char* tmpaspointer = reinterpret_cast<char*>(&tmp);
	out.write(tmpaspointer, sizeof(tmp));
	tmp = 1;//version min
	out.write(tmpaspointer, sizeof(tmp));
	tmp = 0;//version flags
	out.write(tmpaspointer, sizeof(tmp));
	tmp = 1;//numEntries
	out.write(tmpaspointer, sizeof(tmp));
	tmp = 11; //name len
	out.write(tmpaspointer, sizeof(tmp));
	const char name[] = "pTorus1Mesh";
	out.write(name, sizeof(name));
	tmp = 1; //type mesh
	out.write(tmpaspointer, sizeof(tmp));
	tmp = 0; //Offest
	out.write(tmpaspointer, sizeof(tmp));
	dword tmplen = 0;
	int lastmod = 0;
	for(dword i = 0; i < header.numSurfaces; i++)
	{
		tmplen+=(surfaces[i].numTriangles * 3 * 2 + sizeof(surfaceInfo) + 5 * 4 - lastmod * 2);
		lastmod = (surfaces[i].numTriangles * 3) % 2;
	}
	tmp = header.numVertices * (header.sizeVertex == 44 ? 44 : 32) + 9 * 4 + 12 + tmplen + (header.sizeVertex == 44 ? sizeof(allocInfo44) : sizeof(allocInfo)); 
	//Lenght ... count it
	out.write(tmpaspointer, sizeof(tmp));
	tmp64 = tmp;//Lenght again
	out.write(tmp64aspointer, sizeof(tmp64));
	tmp = 1; //type mesh
	out.write(tmpaspointer, sizeof(tmp));			//4
	tmp = 11; //name len
	out.write(tmpaspointer, sizeof(tmp));			//4
	out.write(name, sizeof(name));					//12
	tmp = 1;//vertexbuffers count
	out.write(tmpaspointer, sizeof(tmp));			//4
	if(header.sizeVertex == 44)
		out.write(reinterpret_cast<const char*>(&allocInfo44), sizeof(allocInfo44));
	else
		out.write(reinterpret_cast<const char*>(&allocInfo), sizeof(allocInfo));
	//type def
	tmp = header.numVertices;
	out.write(tmpaspointer, sizeof(tmp)); //X				//4
	tmp = 0;
	out.write(tmpaspointer, sizeof(tmp)); //Y				//4
	out.write(tmpaspointer, sizeof(tmp)); //Z				//4
	out.write(tmpaspointer, sizeof(tmp));					//4
	//lod = 0, faces = 0 + 2 bytes = 0
	tmp = header.numVertices * (header.sizeVertex == 44 ? 44 : 32);
	out.write(tmpaspointer, sizeof(tmp));					//4
	for(dword i = 0; i < header.numVertices; i ++)	//header.numVertices * 32
	{
		if(hasTangentsBinormals)
		{
			const float tmpflo[] = {verticestb(i)->vertexPosition[0]*size,
			verticestb(i)->vertexPosition[1]*size,
			verticestb(i)->vertexPosition[2]*size,
			verticestb(i)->vertexNormal[0],
			verticestb(i)->vertexNormal[1],
			verticestb(i)->vertexNormal[2],
			verticestb(i)->vertexTangent[0],
			verticestb(i)->vertexTangent[1],
			verticestb(i)->vertexTangent[2],
			verticestb(i)->vertexUV[0],
			verticestb(i)->vertexUV[1]};
			out.write(reinterpret_cast<const char*>(&tmpflo), sizeof(tmpflo));
		}
		else
		{
			const float tmpflo[] = {verticesc(i)->vertexPosition[0]*size,
			verticesc(i)->vertexPosition[1]*size,
			verticesc(i)->vertexPosition[2]*size,
			verticesc(i)->vertexNormal[0],
			verticesc(i)->vertexNormal[1],
			verticesc(i)->vertexNormal[2],
			verticesc(i)->vertexUV[0],
			verticesc(i)->vertexUV[1]};
			out.write(reinterpret_cast<const char*>(&tmpflo), sizeof(tmpflo));
		}
	}
	tmp = header.numSurfaces;
	out.write(tmpaspointer, sizeof(tmp));					//4
	lastmod = 0;
	for(dword i = 0; i < header.numSurfaces; i++)	//header.numSurfaces * ...
	{
		if(lastmod == 0)
			out.write(reinterpret_cast<const char*>(&surfaceInfo), sizeof(surfaceInfo)); //sizeof(surfaceInfo)
		else
			out.write(reinterpret_cast<const char*>(&surfaceInfo2), sizeof(surfaceInfo2)); //sizeof(surfaceInfo2)
		lastmod = (surfaces[i].numTriangles * 3) % 2;
		tmp = surfaces[i].numTriangles * 3;
		out.write(tmpaspointer, sizeof(tmp)); //X				//4
		tmp = 0;
		out.write(tmpaspointer, sizeof(tmp)); //Y				//4
		out.write(tmpaspointer, sizeof(tmp)); //Z				//4
		out.write(tmpaspointer, sizeof(tmp));					//4
		//lod = 0, faces = 0 + 2 bytes = 0
		tmp = surfaces[i].numTriangles * 3 * 2;
		out.write(tmpaspointer, sizeof(tmp));					//4
		for(dword c = 0; c < surfaces[i].numTriangles; c ++)//surfaces[i].numTriangles * 3 * 2
		{
			word tmpw[] = {(word)triangles[i][c][0],
			(word)triangles[i][c][1],
			(word)triangles[i][c][2]};
			out.write(reinterpret_cast<const char*>(&tmpw), sizeof(tmpw));
		}
	}
	out.close();
}

#ifdef IOS_REF
	#undef  IOS_REF
	#define IOS_REF (*(pManager->GetIOSettings()))
#endif


FbxTexture* CreateTexture(FbxScene* pScene, const string &name, const string &filename)
{
	FbxFileTexture* lTexture = FbxFileTexture::Create(pScene,name.c_str());
	lTexture->SetFileName(filename.c_str()); // Resource file is in current directory.
	lTexture->SetTextureUse(FbxTexture::eStandard);
	lTexture->SetMappingType(FbxTexture::eUV);
	lTexture->SetMaterialUse(FbxFileTexture::eModelMaterial);
	lTexture->SetSwapUV(false);
	lTexture->SetTranslation(0.0, 0.0);
	lTexture->SetScale(1.0, 1.0);
	lTexture->SetRotation(0.0, 0.0);    
	return lTexture;
}
void TriFile::ExportFBX(float size, string file, string dir)
{
	int lMajor, lMinor, lRevision;
	FbxManager* pManager = FbxManager::Create();
	if( !pManager )
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		return;
	}
	else FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	FbxScene* pScene = FbxScene::Create(pManager, "");
	if( !pScene )
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		return;
	}
	FbxMesh* lMesh = FbxMesh::Create(pScene,file.c_str());
	string node = file+"node";
	FbxNode* lNode = FbxNode::Create(pScene,node.c_str());
	lNode->SetNodeAttribute(lMesh);
	pScene->GetRootNode()->AddChild(lNode);
	lMesh->InitControlPoints(header.numVertices);
	FbxVector4* lControlPoints = lMesh->GetControlPoints();
	FbxGeometryElementNormal* lNormalElement = lMesh->CreateElementNormal();
	FbxGeometryElementTangent* lTangentElement = NULL;
	if(hasTangentsBinormals){
		lTangentElement = lMesh->CreateElementTangent();
		lTangentElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
		lTangentElement->SetReferenceMode(FbxGeometryElement::eDirect);
	}
	lNormalElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
	lNormalElement->SetReferenceMode(FbxGeometryElement::eDirect);
	FbxGeometryElementUV* lUVElement = lMesh->CreateElementUV("UV1");
	lUVElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
	lUVElement->SetReferenceMode(FbxGeometryElement::eDirect);
	for(dword i = 0; i < header.numVertices; i ++)	//header.numVertices * 32
	{
		if(hasTangentsBinormals){
			lControlPoints[i] = FbxVector4(verticestb(i)->vertexPosition[0]*size,
				verticestb(i)->vertexPosition[1]*size,
				verticestb(i)->vertexPosition[2]*size);
			lNormalElement->GetDirectArray().Add(FbxVector4(verticestb(i)->vertexNormal[0],
				verticestb(i)->vertexNormal[1],
				verticestb(i)->vertexNormal[2]));
			// Tangents not exported ?
			lUVElement->GetDirectArray().Add(FbxVector2(verticestb(i)->vertexUV[0],
				verticestb(i)->vertexUV[1]));
			lTangentElement->GetDirectArray().Add(FbxVector4(verticestb(i)->vertexTangent[0],
				verticestb(i)->vertexTangent[1],
				verticestb(i)->vertexTangent[2]));
		} else {
			lControlPoints[i] = FbxVector4(verticesc(i)->vertexPosition[0]*size,
				verticesc(i)->vertexPosition[1]*size,
				verticesc(i)->vertexPosition[2]*size);
			// Position as Normals ?! :) Selvin: my bad, copy-paste programming :)
			lNormalElement->GetDirectArray().Add( FbxVector4(verticesc(i)->vertexNormal[0],
				verticesc(i)->vertexNormal[1],
				verticesc(i)->vertexNormal[2]));
			lUVElement->GetDirectArray().Add( FbxVector2(verticesc(i)->vertexUV[0],
				verticesc(i)->vertexUV[1]));
		}
	}
		FbxGeometryElementMaterial* lMaterialElement = lMesh->CreateElementMaterial();
		lMaterialElement->SetMappingMode(FbxGeometryElement::eByPolygon);
		lMaterialElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);
		
	for(dword i = 0; i < header.numSurfaces; i++)   //header.numSurfaces * ...
	{
		for(dword c = 0; c < surfaces[i].numTriangles; c ++)//surfaces[i].numTriangles * 3 * 2
		{
			lMesh->BeginPolygon(i);
			lMesh->AddPolygon(triangles[i][c][0]);
			lMesh->AddPolygon(triangles[i][c][1]);
			lMesh->AddPolygon(triangles[i][c][2]);
			lMesh->EndPolygon();
		}
		FbxString lMaterialName = "material";
		lMaterialName = file.c_str() + lMaterialName ;
		lMaterialName += (int)i;
		FbxSurfacePhong* lMaterial = FbxSurfacePhong::Create(pScene,lMaterialName.Buffer());

		// Generate primary and secondary colors.
		double diff = 1.0 - 0.1 * i;
		lMaterial->Emissive.Set(FbxDouble3(0.0, 0.0, 0.0));
		lMaterial->Ambient.Set(FbxDouble3(diff, diff, diff));
		lMaterial->Diffuse.Set(FbxDouble3(diff,diff,diff));
		lMaterial->Specular.Set(FbxDouble3(0.0, 0.0, 0.0));
		lMaterial->TransparencyFactor.Set(0.0);
		lMaterial->Shininess.Set(0.5);
		lMaterial->ShadingModel.Set(FbxString("phong"));
		findAndReplace<string>(textures[i], ".dds", ".png");
		lMaterial->Diffuse.ConnectSrcObject(CreateTexture(pScene, textures[i], textures[i]));
		// add materials to the node
		lNode->AddMaterial(lMaterial);
	}



	
	FbxGeometryConverter lGeometryConverter(pManager);
	lGeometryConverter.ComputeEdgeSmoothingFromNormals(lMesh);
	lGeometryConverter.ComputePolygonSmoothingFromEdgeSmoothing(lMesh);

	FbxExporter* lExporter = FbxExporter::Create(pManager, "");
	int pFileFormat = pManager->GetIOPluginRegistry()->GetNativeWriterFormat();
			pFileFormat = pManager->GetIOPluginRegistry()->GetNativeWriterFormat();

	//Try to export in ASCII if possible
	int lFormatIndex, lFormatCount = pManager->GetIOPluginRegistry()->GetWriterFormatCount();

	for (lFormatIndex=0; lFormatIndex<lFormatCount; lFormatIndex++)
	{
		if (pManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
		{
			FbxString lDesc =pManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
			char *lASCII = "ascii";
			if (lDesc.Find(lASCII)>=0)
			{
				pFileFormat = lFormatIndex;
				break;
			}
		}
	}

	IOS_REF.SetBoolProp(EXP_FBX_MATERIAL,        true);
	IOS_REF.SetBoolProp(EXP_FBX_TEXTURE,         true);
	IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED,        true);
	IOS_REF.SetBoolProp(EXP_FBX_SHAPE,           true);
	IOS_REF.SetBoolProp(EXP_FBX_GOBO,            true);
	IOS_REF.SetBoolProp(EXP_FBX_ANIMATION,       true);
	IOS_REF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);
	string f = (dir + file + ".fbx");
	const char* pFilename = f.c_str();
	// Initialize the exporter by providing a filename.
	if(lExporter->Initialize(pFilename, pFileFormat, pManager->GetIOSettings()) == false)
	{
		FBXSDK_printf("Call to FbxExporter::Initialize() failed.\n");
		FBXSDK_printf("Error returned: %s\n\n", lExporter->GetLastErrorString());
		return;
	}

	FbxManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
	FBXSDK_printf("FBX file format version %d.%d.%d\n\n", lMajor, lMinor, lRevision);

	// Export the scene.
	int lStatus = lExporter->Export(pScene); 

	// Destroy the exporter.
	lExporter->Destroy();
	if( pManager ) pManager->Destroy();
}


