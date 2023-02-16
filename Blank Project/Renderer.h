#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Frustum.h"
class Camera;
class Shader;
class HeightMap;
class Mesh;
class MeshMaterial;
class MeshAnimation;
class SceneNode;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);
	void RenderScene() override;
	void UpdateScene(float dt) override;
	int ting;
	float mouseyaw;
	float mousepitch;
	bool autocam;
	bool autocam1;
protected:
	void DrawHeightmap();
	void DrawPortal();
	void DrawSkybox();
	void DrawTree();
	void AnimDraw();
	void DrawStatue();
	// post process
	
	void PresentScene();
	void DrawPostProcess();
	void DrawScene();
	//Scenenode for trees
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode * n);

	Frustum frameFrustum;
	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
	SceneNode* root;
	Mesh* tree;
	MeshMaterial* treetex;
	vector<GLuint> treeTextures;
	
	
	SceneNode* root2;
	//
	Shader* statueshader;
	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* processShader;
	Shader* sceneShader;
	Shader* treeshader;
	Shader* Animshader;
	HeightMap* heightMap;
	Mesh* quad;
	
	 // camera locator
	Light* light; // lighting
	Camera* camera;
	//skybox
	GLuint day;
	GLuint night;
	GLuint day2;
	GLuint night2;
	GLuint cubeMap;
	//
	GLuint waterTex;
	GLuint earthTex;
	GLuint earthBump;
	GLuint netherTex;
	//post process
	Shader* simpletexshader;
	Mesh* FBOquad;
	GLuint heightTexture;
	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;
	//
	float waterRotate;
	float waterCycle;
	//Meshes
	Mesh* mesh;
	MeshMaterial* material;
	vector<GLuint> matTextures;

	MeshAnimation* Anim;
	Mesh* MeshAnimate;
	MeshMaterial* MaterialAnitmate;
	vector <GLuint> matTexturesAnim;
    int currentFrame;
	float frameTime;
};