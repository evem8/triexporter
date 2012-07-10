#define FUNC(ret, name, args) \
	extern "C" typedef ret (__stdcall *raw_##name) args; \
	raw_##name* name = (raw_##name*)&_##name;

struct GrannyLogger
{
	void (__cdecl *a) (int a, int b, char* msg);
	void (__cdecl *b) (int a, int b, char* msg);
};

struct GrannyFile;

typedef char granny_int8;
typedef unsigned char granny_uint8;
typedef unsigned short granny_int16;
typedef unsigned short granny_uint16;
typedef int granny_int32;
typedef unsigned int granny_uint32;
typedef float granny_real32;
struct granny_transform { float a; float b[3][3]; };

struct t_StringMember
{
	granny_int32 Type;
	char * Name;
	void * StructOffset;
	int Count;
	int x[4];
};

#include "structs.h"

struct t_Vertex;
struct t_Vertex_PNT332
{
	granny_real32 Position[3];
	granny_real32 Normal[3];
	granny_real32 TextureCoordinates0[2];
};
struct t_Vertex_PNTT3332
{
	granny_real32 Position[3];
	granny_real32 Normal[3];
	granny_real32 Tangent[3];
	granny_real32 TextureCoordinates0[2];
};

struct t_Vertex_PNTBT3332
{
	granny_real32 Position[3];
	granny_real32 Normal[3];
	granny_real32 Tangent[3];
	granny_real32 Binormal[3];
	granny_real32 TextureCoordinates0[2];
};


struct t_Vertex_PNTBT33332
{
	granny_real32 Position[3];
	granny_real32 Normal[3];
	granny_real32 Tangent[3];
	granny_real32 Binormal[3];
	granny_real32 DiffuseColor0[3];
	granny_real32 TextureCoordinates0[2];
	granny_real32 TextureCoordinates1[2];
};


struct GrannyStack
{
	char x[20];
};


typedef struct { char c[380]; } t_Camera;
FUNC(void*, GrannyInitializeDefaultCamera, (t_Camera* camera));
FUNC(void*, GrannySetCameraAspectRatios, (t_Camera* camera, float ratio, float a, float b, float c, float d));

FUNC(void, GrannyCopyMeshIndices, (t_Meshes* mesh, int something, void* buffer));
FUNC(void, GrannyCopyMeshVertices, (t_Meshes* mesh, int vertextype, void* buffer));

FUNC(t_FileInfo*, GrannyGetFileInfo, (GrannyFile* file));
FUNC(void, GrannyFreeFile, (GrannyFile* file));
FUNC(int, GrannyGetMeshBytesPerIndex, (t_Meshes* mesh));
FUNC(int, GrannyGetMeshIndexCount, (t_Meshes* mesh));
FUNC(t_Indices16*, GrannyGetMeshIndices, (t_Meshes* mesh));
FUNC(int, GrannyGetMeshTriangleCount, (t_Meshes* mesh));
FUNC(int, GrannyGetMeshTriangleGroupCount, (t_Meshes* mesh));
FUNC(t_Groups*, GrannyGetMeshTriangleGroups, (t_Meshes* mesh));
FUNC(int, GrannyGetMeshVertexCount, (t_Meshes* mesh));
FUNC(int, GrannyGetMeshVertexType, (t_Meshes* mesh));
FUNC(t_Vertex*, GrannyGetMeshVertices, (t_Meshes* mesh));

typedef t_Models** t_Instance;
typedef void* t_MeshBinding;
typedef void* t_WorldPose;
typedef void* t_LocalPose;
FUNC(t_Instance, GrannyInstantiateModel, (t_Models* model));
FUNC(t_Skeleton*, GrannyGetSourceSkeleton, (t_Instance model));
FUNC(t_Models*, GrannyGetSourceModel, (t_Instance model));
FUNC(intptr_t, GrannyNewMeshBinding, (t_Meshes* mesh, t_Skeleton* skel0, t_Skeleton* skel1));
FUNC(int*, GrannyGetMeshBindingToBoneIndices, (t_MeshBinding meshbinding));

//FUNC(int, GrannyGetMeshVertexType, (t_Meshes* mesh));
FUNC(intptr_t, GrannyNewMeshDeformer, (int type, int newtype, int two_or_three));
FUNC(void, GrannyDeformVertices, (intptr_t edi, int arg_C, int one, int numvertices, t_Vertex* vertices, int arg_C2));
/*
edi: [[this+30] + 20*arg_0]
arg0 - var1e0
arg4 - meshbindingtoboneindices
arg8 - var1a8 / getworldcompositearray4x4
argc - 
arg10 - 1
arg14 - 0
(arg18 - 1)
*/

FUNC(bool, GrannySetModelClock, (t_Instance inst, float time));
FUNC(void, GrannyUpdateModelMatrix, (t_Instance inst, float time, float matrix0[4][4], float matrix1[4][4]));
FUNC(t_WorldPose, GrannyNewWorldPose, (int n));
FUNC(t_LocalPose, GrannyNewLocalPose, (int n));
FUNC(bool, GrannySampleModelAnimations, (t_Instance inst, int bone0, int numbones, t_LocalPose pose));
FUNC(int, GrannyBuildWorldPose, (t_Skeleton* skel, int bone0, int numbones, t_LocalPose pose, float matrix[4][4], t_WorldPose wpose));

FUNC(int, GrannyGetVertexComponentIndex, (const char* name, const t_StringMember* sm));
FUNC(int, GrannyNewStackUnit, (GrannyStack* stack, char** program));
FUNC(GrannyFile*, GrannyReadEntireFileFromMemory, (int size, char* mem));
FUNC(GrannyFile*, GrannyReadEntireFile, (char* filename));
FUNC(void, GrannySetLogCallback, (GrannyLogger* logger));
FUNC(void, GrannyStackInitialize, (GrannyStack* stack, int n, int m));

