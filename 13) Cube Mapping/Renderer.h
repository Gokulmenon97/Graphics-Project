#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/CubeRobot.h"

class Camera;
class Shader;
class HeightMap;
class MeshMaterial;
class SceneNode;

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer(void);
	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	void DrawScene();
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void DrawObjects();
	void DrawS_Object();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);
	void DrawSceneGraph();

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* ObjShader;
	Shader* sceneShader;
	Shader* S_ObjectShader;

	HeightMap* heightMap;
	Mesh* quad;
	Mesh* sceneCube;
	Mesh* sceneQuad;

	vector<Mesh*> S_Object;
	vector<MeshMaterial*> S_ObjectMaterial;
	vector<GLuint> S_ObjMatTextures;
	vector<vector<GLuint>> S_ObjMatTexturesColl;

	vector<Mesh*> Obj;
	vector<MeshMaterial*> Material;
	vector<GLuint> MatTextures;
	vector<vector<GLuint>> MatTexturesColl;

	Light* light;
	Camera* camera;
	SceneNode* root;
	Frustum frameFrustum;

	GLuint cubeMap;
	GLuint waterTex;
	GLuint waterBump;
	GLuint earthTex;
	GLuint earthBump;
	GLuint sceneTex;
	GLuint S_ObjectTex;
	

	float waterRotate;
	float waterCycle;
	int randRotation;
	int chestPos;
	bool camSwitch = false;
	bool orienSwitch = false;

	Vector3 hSize;
	vector<Vector3> randPosOnGround;
	vector <SceneNode*> transparentNodeList;
	vector <SceneNode*> nodeList;
};