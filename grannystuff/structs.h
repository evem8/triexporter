struct t_FileInfo;

struct t_ArtToolInfo;

struct t_ExporterInfo;

struct t_Layout;

struct t_Images;

struct t_MIPLevels;

struct t_Pixels;

struct t_5526560;
typedef struct t_5526560 t_Map;
typedef struct t_5526560 t_Material;
typedef struct t_5526560 t_Materials;

struct t_Maps;

struct t_5516280;
typedef struct t_5516280 t_Skeleton;
typedef struct t_5516280 t_Skeletons;

struct t_Bones;

struct t_LightInfo;

struct t_CameraInfo;

struct t_5504952;
typedef struct t_5504952 t_PrimaryVertexData;
typedef struct t_5504952 t_VertexData;
typedef struct t_5504952 t_VertexDatas;

struct t_VertexAnnotationSets;

struct t_5443320;
typedef struct t_5443320 t_BonesForTriangle;
typedef struct t_5443320 t_Indices;
typedef struct t_5443320 t_SideToNeighborMap;
typedef struct t_5443320 t_TriAnnotationIndices;
typedef struct t_5443320 t_TriangleIndices;
typedef struct t_5443320 t_TriangleToBoneIndices;
typedef struct t_5443320 t_VertexAnnotationIndices;
typedef struct t_5443320 t_VertexToTriangleMap;
typedef struct t_5443320 t_VertexToVertexMap;

struct t_5503592;
typedef struct t_5503592 t_PrimaryTopology;
typedef struct t_5503592 t_TriTopologies;

struct t_Groups;

struct t_Indices16;

struct t_TriAnnotationSets;

struct t_5502264;
typedef struct t_5502264 t_Mesh;
typedef struct t_5502264 t_Meshes;

struct t_MorphTargets;

struct t_MaterialBindings;

struct t_BoneBindings;

struct t_Models;

struct t_MeshBindings;

struct t_TrackGroups;

struct t_5533528;
typedef struct t_5533528 t_OrientationCurve;
typedef struct t_5533528 t_PositionCurve;
typedef struct t_5533528 t_ScaleShearCurve;
typedef struct t_5533528 t_ValueCurve;

struct t_5443536;
typedef struct t_5443536 t_Controls;
typedef struct t_5443536 t_Knots;

struct t_TextTracks;

struct t_Entries;

struct t_PeriodicLoop;

struct t_Animations;

struct t_5514488;
typedef struct t_5514488 t_Texture;
typedef struct t_5514488 t_Textures;

struct t_ScalarTracks;

struct t_TransformTracks;

struct t_FileInfo {
	t_ArtToolInfo * ArtToolInfo;
	t_ExporterInfo * ExporterInfo;
	char * FromFileName;
	granny_int32 Textures_count;
	t_Textures ** Textures; /* VariantReferenceMember */
	granny_int32 Materials_count;
	t_Materials ** Materials; /* VariantReferenceMember */
	granny_int32 Skeletons_count;
	t_Skeletons ** Skeletons; /* VariantReferenceMember */
	granny_int32 VertexDatas_count;
	t_VertexDatas ** VertexDatas; /* VariantReferenceMember */
	granny_int32 TriTopologies_count;
	t_TriTopologies ** TriTopologies; /* VariantReferenceMember */
	granny_int32 Meshes_count;
	t_Meshes ** Meshes; /* VariantReferenceMember */
	granny_int32 Models_count;
	t_Models ** Models; /* VariantReferenceMember */
	granny_int32 TrackGroups_count;
	t_TrackGroups ** TrackGroups; /* VariantReferenceMember */
	granny_int32 Animations_count;
	t_Animations ** Animations; /* VariantReferenceMember */
};

struct t_ArtToolInfo {
	char * FromArtToolName;
	granny_int32 ArtToolMajorRevision;
	granny_int32 ArtToolMinorRevision;
	granny_real32 UnitsPerMeter;
	granny_real32 Origin[3];
	granny_real32 RightVector[3];
	granny_real32 UpVector[3];
	granny_real32 BackVector[3];
};

struct t_ExporterInfo {
	char * ExporterName;
	granny_int32 ExporterMajorRevision;
	granny_int32 ExporterMinorRevision;
	granny_int32 ExporterCustomization;
	granny_int32 ExporterBuildNumber;
};

struct t_Layout {
	granny_int32 BytesPerPixel;
	granny_int32 ShiftForComponent[4];
	granny_int32 BitsForComponent[4];
};

struct t_Images {
	granny_int32 MIPLevels_count;
	t_MIPLevels * MIPLevels; /* ArrayOfReferencesMember */
};

struct t_MIPLevels {
	granny_int32 Stride;
	granny_int32 Pixels_count;
	t_Pixels * Pixels; /* ArrayOfReferencesMember */
};

struct t_Pixels {
	granny_uint8 UInt8;
};

struct t_5526560 {
	char * Name;
	granny_int32 Maps_count;
	t_Maps * Maps; /* ArrayOfReferencesMember */
	t_Texture * Texture;
	void * /* SwitchableTypeMember */ ExtendedData;
};

struct t_Maps {
	char * Usage;
	t_Map * Map;
};

struct t_5516280 {
	char * Name;
	granny_int32 Bones_count;
	t_Bones * Bones; /* ArrayOfReferencesMember */
};

struct t_Bones {
	char * Name;
	granny_int32 ParentIndex;
	granny_transform Transform;
	granny_real32 InverseWorldTransform[16];
	t_LightInfo * LightInfo;
	t_CameraInfo * CameraInfo;
	void * /* SwitchableTypeMember */ ExtendedData;
};

struct t_LightInfo {
	void * /* SwitchableTypeMember */ ExtendedData;
};

struct t_CameraInfo {
	void * /* SwitchableTypeMember */ ExtendedData;
};

struct t_5504952 {
	t_StringMember * Vertices_strings; /* StringMember */
	granny_int32 Vertices_count;
	void * Vertices;
	granny_int32 VertexAnnotationSets_count;
	t_VertexAnnotationSets * VertexAnnotationSets; /* ArrayOfReferencesMember */
};

struct t_VertexAnnotationSets {
	char * Name;
	t_StringMember * VertexAnnotations_strings; /* StringMember */
	granny_int32 VertexAnnotations_count;
	void * VertexAnnotations;
	granny_int32 VertexAnnotationIndices_count;
	t_VertexAnnotationIndices * VertexAnnotationIndices; /* ArrayOfReferencesMember */
};

struct t_5443320 {
	granny_int32 Int32;
};

struct t_5503592 {
	granny_int32 Groups_count;
	t_Groups * Groups; /* ArrayOfReferencesMember */
	granny_int32 Indices_count;
	t_Indices * Indices; /* ArrayOfReferencesMember */
	granny_int32 Indices16_count;
	t_Indices16 * Indices16; /* ArrayOfReferencesMember */
	granny_int32 VertexToVertexMap_count;
	t_VertexToVertexMap * VertexToVertexMap; /* ArrayOfReferencesMember */
	granny_int32 VertexToTriangleMap_count;
	t_VertexToTriangleMap * VertexToTriangleMap; /* ArrayOfReferencesMember */
	granny_int32 SideToNeighborMap_count;
	t_SideToNeighborMap * SideToNeighborMap; /* ArrayOfReferencesMember */
	granny_int32 BonesForTriangle_count;
	t_BonesForTriangle * BonesForTriangle; /* ArrayOfReferencesMember */
	granny_int32 TriangleToBoneIndices_count;
	t_TriangleToBoneIndices * TriangleToBoneIndices; /* ArrayOfReferencesMember */
	granny_int32 TriAnnotationSets_count;
	t_TriAnnotationSets * TriAnnotationSets; /* ArrayOfReferencesMember */
};

struct t_Groups {
	granny_int32 MaterialIndex;
	granny_int32 TriFirst;
	granny_int32 TriCount;
};

struct t_Indices16 {
	granny_int16 Int16;
};

struct t_TriAnnotationSets {
	char * Name;
	t_StringMember * TriAnnotations_strings; /* StringMember */
	granny_int32 TriAnnotations_count;
	void * TriAnnotations;
	granny_int32 TriAnnotationIndices_count;
	t_TriAnnotationIndices * TriAnnotationIndices; /* ArrayOfReferencesMember */
};

struct t_5502264 {
	char * Name;
	t_PrimaryVertexData * PrimaryVertexData;
	granny_int32 MorphTargets_count;
	t_MorphTargets * MorphTargets; /* ArrayOfReferencesMember */
	t_PrimaryTopology * PrimaryTopology;
	granny_int32 MaterialBindings_count;
	t_MaterialBindings * MaterialBindings; /* ArrayOfReferencesMember */
	granny_int32 BoneBindings_count;
	t_BoneBindings * BoneBindings; /* ArrayOfReferencesMember */
	void * /* SwitchableTypeMember */ ExtendedData;
};

struct t_MorphTargets {
	char * ScalarName;
	t_VertexData * VertexData;
};

struct t_MaterialBindings {
	t_Material * Material;
};

struct t_BoneBindings {
	char * BoneName;
	granny_real32 OBBMin[3];
	granny_real32 OBBMax[3];
	granny_int32 TriangleIndices_count;
	t_TriangleIndices * TriangleIndices; /* ArrayOfReferencesMember */
};

struct t_Models {
	char * Name;
	t_Skeleton * Skeleton;
	granny_transform InitialPlacement;
	granny_int32 MeshBindings_count;
	t_MeshBindings * MeshBindings; /* ArrayOfReferencesMember */
};

struct t_MeshBindings {
	t_Mesh * Mesh;
};

struct t_TrackGroups {
	char * Name;
	granny_int32 ScalarTracks_count;
	t_ScalarTracks * ScalarTracks; /* ArrayOfReferencesMember */
	granny_int32 TransformTracks_count;
	t_TransformTracks * TransformTracks; /* ArrayOfReferencesMember */
	granny_int32 TextTracks_count;
	t_TextTracks * TextTracks; /* ArrayOfReferencesMember */
	granny_transform InitialPlacement;
	granny_int32 AccumulationFlags;
	granny_real32 LoopTranslation[3];
	t_PeriodicLoop * PeriodicLoop;
	void * /* SwitchableTypeMember */ ExtendedData;
};

struct t_5533528 {
	granny_int32 Degree;
	granny_int32 Knots_count;
	t_Knots * Knots; /* ArrayOfReferencesMember */
	granny_int32 Controls_count;
	t_Controls * Controls; /* ArrayOfReferencesMember */
};

struct t_5443536 {
	granny_real32 Real32;
};

struct t_TextTracks {
	char * Name;
	granny_int32 Entries_count;
	t_Entries * Entries; /* ArrayOfReferencesMember */
};

struct t_Entries {
	granny_real32 TimeStamp;
	char * Text;
};

struct t_PeriodicLoop {
	granny_real32 Radius;
	granny_real32 dAngle;
	granny_real32 dZ;
	granny_real32 BasisX[3];
	granny_real32 BasisY[3];
	granny_real32 Axis[3];
};

struct t_Animations {
	char * Name;
	granny_real32 Duration;
	granny_real32 TimeStep;
	granny_int32 TrackGroups_count;
	t_TrackGroups ** TrackGroups; /* VariantReferenceMember */
};

struct t_5514488 {
	char * FromFileName;
	granny_int32 TextureType;
	granny_int32 Width;
	granny_int32 Height;
	granny_int32 Encoding;
	granny_int32 SubFormat;
	t_Layout Layout;
	granny_int32 Images_count;
	t_Images * Images; /* ArrayOfReferencesMember */
	void * /* SwitchableTypeMember */ ExtendedData;
};

struct t_ScalarTracks {
	char * Name;
	t_ValueCurve ValueCurve;
};

struct t_TransformTracks {
	char * Name;
	t_PositionCurve PositionCurve;
	t_OrientationCurve OrientationCurve;
	t_ScaleShearCurve ScaleShearCurve;
};

