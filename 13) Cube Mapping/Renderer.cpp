#include "Renderer.h"
#include "..//nclgl/Light.h"
#include "..//nclgl/HeightMap.h"
#include "..//nclgl/Shader.h"
#include "../nclgl/Camera.h"
#include <algorithm>

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	//Shapes
	quad = Mesh::GenerateQuad();
	sceneQuad = Mesh::GenerateQuad();

	//Maps
	heightMap = new HeightMap(TEXTUREDIR"MAP003.png");

	
	sceneCube = Mesh::LoadFromMeshFile("OffsetCubeY.msh");
	Obj.emplace_back(Mesh::LoadFromMeshFile("Poplar_Tree.msh"));
	Obj.emplace_back(Mesh::LoadFromMeshFile("Fir_Tree.msh"));
	Obj.emplace_back(Mesh::LoadFromMeshFile("Role_T.msh"));


	
	
	Material.emplace_back(new MeshMaterial("Poplar_Tree.mat"));
	Material.emplace_back(new MeshMaterial("Fir_Tree.mat"));
	Material.emplace_back(new MeshMaterial("Role_T.mat"));


	//S_Objects
	S_Object.emplace_back(Mesh::LoadFromMeshFile("Oil_tank_v2.msh"));
	S_ObjectMaterial.emplace_back(new MeshMaterial("Oil_tank_v2.mat"));


	//SubMeshes & its Materials
	for (int j = 0; j < Obj.size(); j++)
	{
		for (int i = 0; i < Obj[j]->GetSubMeshCount(); ++i)
		{
			const MeshMaterialEntry* matEntry = Material[j]->GetMaterialForLayer(i);
			const string* filename = nullptr;
			matEntry->GetEntry("Diffuse", &filename);
			string path = TEXTUREDIR + *filename;
			GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
			MatTextures.emplace_back(texID);
		}
		MatTexturesColl.emplace_back(MatTextures);
		MatTextures.clear();
	}

	//S_Objects
	for (int j = 0; j < S_Object.size(); j++)
	{
		for (int i = 0; i < S_Object[j]->GetSubMeshCount(); ++i)
		{
			const MeshMaterialEntry* matEntry = S_ObjectMaterial[j]->GetMaterialForLayer(i);
			const string* filename = nullptr;
			matEntry->GetEntry("Diffuse", &filename);
			string path = TEXTUREDIR + *filename;
			GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
			S_ObjMatTextures.emplace_back(texID);
		}
		S_ObjMatTexturesColl.emplace_back(S_ObjMatTextures);
		S_ObjMatTextures.clear();
	}

	//Textures
	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterBump = SOIL_load_OGL_texture(TEXTUREDIR"waterbump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthTex = SOIL_load_OGL_texture(TEXTUREDIR "grass.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthBump = SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"right.jpg", TEXTUREDIR"left.jpg", TEXTUREDIR"top.jpg", TEXTUREDIR"bottom.jpg", TEXTUREDIR"front.jpg", TEXTUREDIR"back.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	sceneTex = SOIL_load_OGL_texture(TEXTUREDIR "stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);


	if (!earthTex || !earthBump || !cubeMap || !waterTex || !sceneTex) // || !S_ObjectTex)//|| !chestTex || !chestBump || !chestSpecularTex)
	{
		return;
	}

	//Texture Repeat
	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(earthBump, true);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(waterBump, true);

	//Shaders
	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	lightShader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");
	ObjShader = new Shader("objVertex.glsl", "objFragment.glsl");
	sceneShader = new Shader("sceneVertex.glsl", "sceneFragment.glsl");
	S_ObjectShader = new Shader("s_objectVertex.glsl","s_objectFragment.glsl");

	if (!reflectShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !lightShader->LoadSuccess() || !ObjShader->LoadSuccess() || !sceneShader->LoadSuccess() || !S_ObjectShader->LoadSuccess())// || !chestShader->LoadSuccess())
	{
		return;
	}

	//HeightMap Section
	Vector3 heightmapSize = heightMap->GetHeightmapSize();

	//Objects Random Rotation & placement
	randPosOnGround = heightMap->GetRandPositionOnGround();
	randRotation = rand() % 360;

	//SceneNode Section
	root = new SceneNode();

	for (int i = 0; i < 5; ++i)
	{
		SceneNode* s = new SceneNode();

		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));

		s->SetTransform(Matrix4::Translation(Vector3(0, 200.0f, -300.0f + 100.0f + 100 * i)));

		s->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));

		s->SetBoundingRadius(100.0f);

		s->SetMesh(sceneQuad);

		s->SetTexture(sceneTex);

		root->AddChild(s);
	}
	root->AddChild(new CubeRobot(sceneCube));

	//Camera & Light Positioning
	camera = new Camera(heightmapSize.x * 0.0f, heightmapSize.z * -0.5f, heightmapSize * Vector3(0.5f, -5.0f, 0.5f)); //heightmapSize.x * 0.0f heightmapSize.z * -0.5f
	//light = new Light();
	light = new Light(heightmapSize * Vector3(0.0f, -1.5f, 0.0f), Vector4(1, 1, 1, 1), heightmapSize.x);

	//Directional light Settings
	light->SetDirection(Vector3(heightmapSize * Vector3(0.1f, 2.5f, 0.3f)));
	light->SetAmbient(Vector3(0.5f, 0.5f, 0.5f));
	light->SetDiffuse(Vector3(1.0f, 1.0f, 1.0f));

	//Spotlight settings
	light->Set_S_CutOff(12.5f);
	light->Set_S_OuterCutOff(17.5f);
	light->Set_S_Ambient(Vector3(0.5f, 0.5f, 0.5f));
	light->Set_S_Diffuse(Vector3(1.0f, 1.0f, 1.0f));
	light->Set_S_Specular(Vector3(1.0f, 1.0f, 1.0f));
	light->Set_S_Constant(1.0f);
	light->Set_S_Linear(0.045f);
	light->Set_S_Quadratic(0.0075f);
	light->Set_S_Shininess(32.0f);

	
	projMatrix = Matrix4::Perspective(1.0f, 25000.0f, (float)width / (float)height, 45.0f);

	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	//ordinary integers & floats
	waterRotate = 0.0f;
	waterCycle = 0.0f;
	init = true;
}

Renderer::~Renderer(void)
{



	delete camera;

	delete heightMap;

	delete root;
	delete sceneQuad;
	delete sceneCube;
	delete quad;

	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete ObjShader;
	delete sceneShader;
	delete S_ObjectShader;

	delete light;

	for (auto& i : Material)
	{
		delete i;
	}
}



void Renderer::RenderScene()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawScene();
}

void Renderer::DrawScene()
{
	DrawSkybox();
	DrawHeightmap();
	DrawWater();
	DrawObjects();
	DrawSceneGraph();
	DrawS_Object();
}

// Draw SkyBox
void Renderer::DrawSkybox()
{
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();

	glDepthMask(GL_TRUE);
}

// Draw HeightMap
void Renderer::DrawHeightmap()
{
	BindShader(lightShader);
	SetShaderLight(*light);
	
	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	
	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);

	//modelMatrix.ToIdentity();
	modelMatrix = Matrix4::Translation(Vector3(hSize.x * -0.5f, hSize.y * -10.0f, hSize.z * -0.5f));
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	heightMap->Draw();
}

// Draw Water
void Renderer::DrawWater()
{
	BindShader(reflectShader);
	
	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	hSize = heightMap->GetHeightmapSize();
	
	modelMatrix = Matrix4::Translation(Vector3(hSize.x * 0.0f, hSize.y * -9.8f, hSize.z * 0.0f)) * Matrix4::Scale(Vector3(hSize.x * 0.5f,hSize.y*2.0f,hSize.z*0.5f)) * Matrix4::Rotation(90, Vector3(1, 0, 0));
	textureMatrix = Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) * Matrix4::Scale(Vector3(10, 10, 10)) * Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	UpdateShaderMatrices();
	SetShaderLight(*light); //No lighting in this shader!

	quad->Draw();
}

//Draw Non-Animated Objects
void Renderer::DrawObjects()
{
	BindShader(ObjShader);
	glUniform1i(glGetUniformLocation(ObjShader->GetProgram(), "material.diffuseTex"), 0);
	glUniform3fv(glGetUniformLocation(ObjShader->GetProgram(), "light.direction"), 1, (float*)&light->GetDirection());
	glUniform3fv(glGetUniformLocation(ObjShader->GetProgram(), "light.ambient"), 1, (float*)&light->GetAmbient());
	glUniform3fv(glGetUniformLocation(ObjShader->GetProgram(), "light.diffuse"), 1, (float*)&light->GetDiffuse());

	int b = 0;

	for  (int k = 0; k < Obj.size(); k++)
	{
		for (int v = 0; v < 4; v++)
		{
			modelMatrix = Matrix4::Translation(Vector3((hSize.x * -0.5f) + randPosOnGround[b].x, ((hSize.y * -10.0f) + 0.2f) + randPosOnGround[b].y, (hSize.z * -0.5f) + randPosOnGround[b].z)) * Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) * Matrix4::Rotation(randRotation, Vector3(0, 1, 0));
			UpdateShaderMatrices();

			for (int j = 0; j < Obj[k]->GetSubMeshCount(); ++j) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, MatTexturesColl[k][j]);
				Obj[k]->DrawSubMesh(j);
			}
			b +=1111;
		}
	}

	chestPos = b;
}

//Draw Spotlight_Object
void Renderer::DrawS_Object()
{
	BindShader(S_ObjectShader);
	light->Set_S_Position(Vector3(camera->GetPosition()));
	light->Set_S_Direction(Vector3(0.0f, 0.0f,-1.0f));
	chestPos += 10;

	glUniform1i(glGetUniformLocation(S_ObjectShader->GetProgram(), "material.diffuseTex"), 0);
	glUniform3fv(glGetUniformLocation(S_ObjectShader->GetProgram(), "light.position"), 1, (float*)&light->Get_S_Position());
	glUniform3fv(glGetUniformLocation(S_ObjectShader->GetProgram(), "light.direction"), 1, (float*)&light->Get_S_Direction());
	glUniform1f(glGetUniformLocation(S_ObjectShader->GetProgram(), "light.cutOff"), light->Get_S_CutOff());
	glUniform1f(glGetUniformLocation(S_ObjectShader->GetProgram(), "light.outerCutOff"), light->Get_S_OuterCutOff());
	glUniform3fv(glGetUniformLocation(S_ObjectShader->GetProgram(), "viewPos"), 1, (float*)&light->Get_S_Position());

	glUniform3fv(glGetUniformLocation(S_ObjectShader->GetProgram(), "light.ambient"), 1, (float*)&light->Get_S_Ambient());
	glUniform3fv(glGetUniformLocation(S_ObjectShader->GetProgram(), "light.diffuse"), 1, (float*)&light->Get_S_Diffuse());
	glUniform3fv(glGetUniformLocation(S_ObjectShader->GetProgram(), "light.specular"), 1, (float*)&light->Get_S_Specular());
	glUniform1f(glGetUniformLocation(S_ObjectShader->GetProgram(), "light.constant"), light->Get_S_Constant());
	glUniform1f(glGetUniformLocation(S_ObjectShader->GetProgram(), "light.linear"), light->Get_S_Linear());
	glUniform1f(glGetUniformLocation(S_ObjectShader->GetProgram(), "light.quadratic"), light->Get_S_Quadratic());
	glUniform1f(glGetUniformLocation(S_ObjectShader->GetProgram(), "material.shininess"), light->Get_S_Shininess());

	for (int k = 0; k < S_Object.size(); k++)
	{
		for (int v = 0; v < 2; v++)
		{
			modelMatrix = Matrix4::Translation(Vector3((hSize.x * -0.5f) + randPosOnGround[chestPos].x, ((hSize.y * -10.0f) + 20.0f) + randPosOnGround[chestPos].y, (hSize.z * -0.5f) + randPosOnGround[chestPos].z)) * Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) * Matrix4::Rotation(randRotation, Vector3(0, 1, 0));
			UpdateShaderMatrices();

			for (int j = 0; j < S_Object[k]->GetSubMeshCount(); ++j) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, S_ObjMatTexturesColl[k][j]);
				S_Object[k]->DrawSubMesh(j);
			}
			chestPos += 5;
		}
	}

	
}


void Renderer::DrawSceneGraph()
{
	BuildNodeLists(root);
	SortNodeLists();

	BindShader(sceneShader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);

	DrawNodes();

	ClearNodeLists();
}

//Funtions of Scene Graph
//--------------------------------------------------------------------------------------------------------
void Renderer::BuildNodeLists(SceneNode* from)
{
	if (frameFrustum.InsideFrustum(*from))
	{
		Vector3 dir = from->GetWorldTransform().GetPositionVector() -
			camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f)
		{
			transparentNodeList.push_back(from);
		}
		else
		{
			nodeList.push_back(from);
		}
	}

	for (vector < SceneNode* >::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i)
	{
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists()
{
	std::sort(transparentNodeList.rbegin(), // note the r!
		transparentNodeList.rend(), // note the r!
		SceneNode::CompareByCameraDistance);

	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes() {
	for (const auto& i : nodeList)
	{
		DrawNode(i);
	}

	for (const auto& i : transparentNodeList)
	{
		DrawNode(i);
	}
}

void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		

		glUniformMatrix4fv(glGetUniformLocation(sceneShader->GetProgram(), "modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(sceneShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

		sceneTex = n->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sceneTex);

		glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "useTexture"), sceneTex);

		n->Draw(*this);
	}
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}
//--------------------------------------------------------------------------------------------------------

void Renderer::UpdateScene(float dt)
{
	Vector3 heightmapSize = heightMap->GetHeightmapSize();

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_U))
	{
		camSwitch = !camSwitch;
	}
	if (camSwitch == true)
	{
		camera->UpdateCamera(dt);
		viewMatrix = camera->BuildViewMatrix();
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_O))
		{
			orienSwitch = true;
		}
		else if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P))
		{
			orienSwitch = false;
		}

		if (orienSwitch == true)
		{
			projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f, width / 2.0f, -width / 2.0f, height / 2.0f, -height / 2.0f);
		}
		else
		{
			projMatrix = Matrix4::Perspective(1.0f, 25000.0f, (float)width / (float)height, 45.0f);
		}
		frameFrustum.FromMatrix(projMatrix * viewMatrix);
	}
	else
	{
		viewMatrix = camera->BuildAutoMatrix(heightmapSize.x * 0.0f, heightmapSize.z * -0.5f);
		projMatrix = Matrix4::Perspective(1.0f, 25000.0f, (float)width / (float)height, 45.0f);
		frameFrustum.FromMatrix(projMatrix * viewMatrix);
	}
	waterRotate += dt * 2.0f; // 2 degrees a second
	waterCycle += dt * 0.25f; //10 units a second

	root->Update(dt);
}