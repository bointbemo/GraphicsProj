#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/Heightmap.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Camera.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include"../nclgl/SceneNode.h"
#include"../nclgl/Frustum.h"
#include <algorithm >


Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	//glEnable(GL_DEPTH_TEST);
	int POST_PASSES=0;
	Renderer::autocam=false;
	Renderer::autocam1 = false;
	glEnable(GL_CULL_FACE);

	quad = Mesh::GenerateQuad();
	FBOquad = Mesh::GenerateQuad();
	heightMap = new HeightMap(TEXTUREDIR"Terrain_heightmap(1).png");

	Renderer::ting = 1;
	waterTex = SOIL_load_OGL_texture(
		TEXTUREDIR"abstract_bright_light_textures_by_diza_74-d99nuee.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	earthTex = SOIL_load_OGL_texture(
		TEXTUREDIR"Barren Reds.jpg ", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	netherTex = SOIL_load_OGL_texture(
		TEXTUREDIR"lava-texture-stones-volcanic-11570027826lokoq0t3kx.jpg", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthBump = SOIL_load_OGL_texture(
		TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	night = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rightImage.png", TEXTUREDIR"leftImage.png",
		TEXTUREDIR"downImage.png", TEXTUREDIR"upImage.png",
		TEXTUREDIR"backImage.png", TEXTUREDIR"frontImage.png",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	night2 = SOIL_load_OGL_cubemap(
		TEXTUREDIR"leftImage.png", TEXTUREDIR"rightImage.png",
		TEXTUREDIR"upImage.png", TEXTUREDIR"downImage.png",
		TEXTUREDIR"frontImage.png", TEXTUREDIR"backImage.png",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	day = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	day2 = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rusted_east.jpg", TEXTUREDIR"rusted_west.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_north.jpg", TEXTUREDIR"rusted_south.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	// TREE MESH

	treeshader = new Shader("TexturedVertex.glsl", "texturedFragment.glsl");
	mesh = Mesh::LoadFromMeshFile("TigerGargoyle-GO.msh");
	material = new MeshMaterial("TigerGargoyle-GO.mat");
	if (!material) { return; };
	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry =
			material->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures.emplace_back(texID);

	}
	statueshader = new Shader("StatueVertex.glsl", "StatueFrag.glsl");
	// ANIMATION
	Animshader = new Shader("SkinningVertex.glsl", "texturedFragment.glsl");
	MeshAnimate = Mesh::LoadFromMeshFile("Role_T.msh");
	MaterialAnitmate = new MeshMaterial("Role_T.mat");
	Anim = new MeshAnimation("Role_T.anm");
	for (int i = 0; i < MeshAnimate->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry =
			MaterialAnitmate->GetMaterialForLayer(i);
		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTexturesAnim.emplace_back(texID);

	}
	currentFrame = 0;
	frameTime = 0.0f;
	if (!earthTex || !earthBump || !cubeMap || !waterTex) {
		return;

	}
	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(earthBump, true);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(netherTex, true);
	reflectShader = new Shader(
		"reflectVertex.glsl", "reflectFragment.glsl");
	skyboxShader = new Shader(
		"skyboxVertex.glsl", "skyboxFragment.glsl");
	lightShader = new Shader(
		"PerPixelVertex.glsl", "PerPixelFragment.glsl");
	sceneShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	processShader = new Shader("TexturedVertex.glsl",
		 "processfrag.glsl");
	simpletexshader = new Shader("TexturedVertex.glsl", "texturedFragment.glsl");
	
	if (!reflectShader->LoadSuccess() ||
		!skyboxShader->LoadSuccess() ||
		!lightShader->LoadSuccess() ||
		!treeshader->LoadSuccess() || !sceneShader->LoadSuccess()|| !statueshader->LoadSuccess()||!Animshader
		) {
		return;
	}
	Vector3 heightmapSize = heightMap->GetHeightmapSize();
	tree = Mesh::LoadFromMeshFile("Tree9_3.msh");
	treetex = new MeshMaterial("Role_T.mat");
	if (!material) { return; };
	for (int i = 0; i < tree->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry =
			treetex->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		treeTextures.emplace_back(texID);
	}

	root = new SceneNode();
	
	for (int j = 0; j < 8; j++) {
		SceneNode* s = new SceneNode();
		int Shift = j;
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
		s->SetTransform(Matrix4::Translation(
			Vector3(2300 - rand() % 200 * j, 70.0f, 6750 - rand() % 200 * j)));

		s->SetModelScale(Vector3(50.0f, 50.0f, 50.0f));
		s->SetBoundingRadius(1000.0f);
		s->SetMesh(tree);
		for (int i = 0; i < tree->GetSubMeshCount(); ++i) {
			s->SetTexture(treeTextures[i]);
		}
		root->AddChild(s);
	}
	root->Update(0);
	
	camera = new Camera(-45.0f, 0.0f,
		heightmapSize *Vector3(0.5f, 0.5f, 0.5f));
	light = new Light(heightmapSize * Vector3(0.5f, 1.5f, 0.5f),
		Vector4(1, 1, 1, 1), heightmapSize.x * 2);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);
	
	// POST PROCESS
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,
		0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	
	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
		glGenFramebuffers(1, &bufferFBO); 
		glGenFramebuffers(1, &processFBO);

		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, bufferDepthTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
			GL_TEXTURE_2D, bufferDepthTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, bufferColourTex[0], 0);
	
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
			GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0]) {
			return;
          }
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);
		
	     
		 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		 glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		
	
	waterRotate = 0.0f;
	waterCycle = 0.0f;
	init = true;
}
Renderer ::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete treeshader;
	delete light;

	delete mesh;
	delete material;
	delete statueshader;
	delete Anim;
	delete Animshader;
	delete MaterialAnitmate;
	delete MeshAnimate;
	delete simpletexshader;
	delete processShader;
	delete root;
	delete tree;
	delete sceneShader;
	for (int i = 0; i < tree->GetSubMeshCount(); ++i) {
		glDeleteTextures(1, &treeTextures[i]);
	}
	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);


}

void Renderer::UpdateScene(float dt) {


	if (Window::GetKeyboard()->KeyDown(KEYBOARD_3) == true) { autocam = true; }
	else { autocam = false; };
	if (autocam == true) {
		mouseyaw;
		mousepitch;
		ting += 2.0f;
		camera->SetYaw(Window::GetMouse()->GetRelativePosition().x+ting/1000);
		camera->SetPitch(Window::GetMouse()->GetRelativePosition().y+ting/1000);
		if (ting < 600) {
			Vector3 mover(-200.0f + Renderer::ting * 10, 500.0f + ting, ((heightMap->GetHeightmapSize().z / 2) + 2800.0f) - ting);

			camera->SetPosition(mover);
			
		}
		if (ting>599){
			Vector3 mover(-200.0f + 6000, 1100, ((heightMap->GetHeightmapSize().z / 2) + 2800.0f) +1200-2*ting);

			camera->SetPosition(mover);
		}
		if (ting > 3000) {
			Vector3 mover(-45.0f, 10.0f, heightMap->GetHeightmapSize().z / 2);
			camera->SetPosition(mover);
			ting = 0;
		}
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_4) == true) { autocam1 = true; }if (autocam1 == true) { Vector3 mover(-1000.0f, 4000.0f,6000+ heightMap->GetHeightmapSize().z / 2); camera->SetPosition(mover); camera->SetYaw(-50.0f); camera->SetPitch(-10.0f);
	autocam1 = false; }
	
	if(autocam == false){ camera->UpdateCamera(dt); }

	
	
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += dt * 2.0f; 
	waterCycle += dt * 6.0f; 
	
	frameTime -= dt;
	


	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % Anim->GetFrameCount();
		frameTime += 1.0f / Anim->GetFrameRate();
	}
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	root->Update(dt);
}
void Renderer::RenderScene() {
	Vector3 cameraPos = camera->GetPosition();

			glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
			BuildNodeLists(root);
			
			SortNodeLists();
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

			DrawSkybox();
			
			DrawPortal();
			DrawHeightmap();
			
			DrawStatue();
			//DrawAnim();
			DrawTree();
			
			
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
			DrawPostProcess();
			PresentScene();
			projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
				(float)width / (float)height, 45.0f);
			
}
void Renderer::DrawSkybox() {
	Vector3 cameraPos = camera->GetPosition();
	glDepthMask(GL_FALSE);
	
	BindShader(skyboxShader);
	glUniform1i(glGetUniformLocation(
		skyboxShader->GetProgram(), "cubeTex"), 2);
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, (cameraPos.z < heightMap->GetHeightmapSize().z / 2.04 ? night : day));
	quad->Draw();

	glDepthMask(GL_TRUE);
	
}
void Renderer::DrawHeightmap() {
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	BindShader(lightShader);

	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1f(glGetUniformLocation(
		lightShader->GetProgram(), "width"),heightMap->GetHeightmapSize().x);
	glUniform1f(glGetUniformLocation(
		lightShader->GetProgram(), "height"), heightMap->GetHeightmapSize().z);
	glUniform1i(glGetUniformLocation(
		lightShader->GetProgram(), "homeTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);
	
	glUniform1i(glGetUniformLocation(
		lightShader->GetProgram(), "netherTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, netherTex);
	

	glUniform1i(glGetUniformLocation(
		lightShader->GetProgram(), "bumpTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, earthBump);

	modelMatrix.ToIdentity(); //New!
	textureMatrix.ToIdentity(); //New!

	UpdateShaderMatrices();

	heightMap->Draw();
	

}
void Renderer::DrawPortal() {
	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);
	Vector3 cameraPos = camera->GetPosition();
	BindShader(reflectShader);
	
	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(
		reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(
		reflectShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);
	Vector3 hSize (heightMap->GetHeightmapSize().x/1.55f, heightMap->GetHeightmapSize().y/2, heightMap->GetHeightmapSize().z/2.04);
	Vector3 PortSize;
	PortSize.x = 0.3;
	PortSize.y = 1.0;
	PortSize.z = 0.01;
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, (cameraPos.z < heightMap->GetHeightmapSize().z /2.04? day2:night2));
	

	modelMatrix =
		
		Matrix4::Translation(hSize) *
		Matrix4::Scale(hSize * PortSize) *
		Matrix4::Rotation(180.0, Vector3(0, 0, 1));
	

	textureMatrix =
		Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
		Matrix4::Scale(Vector3(10, 10, 10)) *
		Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	UpdateShaderMatrices();
	// SetShaderLight (* light); //No lighting in this shader!
	quad->Draw();
	glEnable(GL_CULL_FACE);

}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() -
			camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);

		}
		else {
			nodeList.push_back(from);

		}

	}

	for (vector <SceneNode*>::const_iterator i =
		from->GetChildIteratorStart();
		i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}

}
void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(), //note the r!
		 transparentNodeList.rend(), //note the r!
		 SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(),
		 nodeList.end(),
		 SceneNode::CompareByCameraDistance);
	
}
void Renderer::DrawNodes() {
	for (const auto& i : nodeList) {
		 DrawNode(i);
		
	}
	 for (const auto& i : transparentNodeList) {
		 DrawNode(i);
		
	}
	
}
void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		Matrix4 model = n->GetWorldTransform() *
			Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(
			glGetUniformLocation(sceneShader->GetProgram(),
				"modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(sceneShader->GetProgram(),
			"nodeColour"), 1, (float*)&n->GetColour());

		n->Draw(*this);

	}


	for (int i = 0; i < tree->GetSubMeshCount(); ++i) {
		glUniform1i(glGetUniformLocation(sceneShader->GetProgram(),
			"useTexture"), treeTextures[i]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, treeTextures[i]);
		UpdateNodeShaderMatrices();
		tree->DrawSubMesh(i);
	}

}
void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}
void Renderer::DrawTree() {
	

	// glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(sceneShader);
	UpdateNodeShaderMatrices();

	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(),
		"diffuseTex"), 0);
	DrawNodes();

	ClearNodeLists();

}

void Renderer::DrawStatue()
{
	
	glEnable(GL_BLEND);
	

		BindShader(statueshader);
		for (int j = 0; j < 10; j++) {
			float randy = rand() % 10 + 1;
			Vector3 vibe(44.0f + (rand() % 200000 + 1) / 200000.0f, 6.5f + (rand() % 200000 + 1) / 200000.0f, 52.0f + (rand() % 200000 + 1) / 200000.0f);
			glUniform1i(glGetUniformLocation(statueshader->GetProgram(), "diffuseTex"), 0);
			UpdateNodeShaderMatrices();
			modelMatrix = Matrix4::Scale(Vector3(100, 100, 100)) * Matrix4::Translation(vibe) * Matrix4::Rotation(180, (Vector3(0, 1, 0)));

			glUniformMatrix4fv(glGetUniformLocation(statueshader->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);
			glUniformMatrix4fv(glGetUniformLocation(statueshader->GetProgram(), "projMatrix"), 1, false, projMatrix.values);
			for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, matTextures[i]);
				mesh->DrawSubMesh(i);
			}
		}

	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
}

void Renderer::AnimDraw() {
	glDisable(GL_BLEND);
		Vector3 hSize(heightMap->GetHeightmapSize().x / 1.55f, heightMap->GetHeightmapSize().y / 2, heightMap->GetHeightmapSize().z / 2.04);
		modelMatrix = Matrix4::Translation(Vector3(1.0f, 100.0f, 10.0f)) * Matrix4::Scale(Vector3(100, 100, 100));
	BindShader(Animshader);
	glUniform1i(glGetUniformLocation(Animshader->GetProgram(),
		"diffuseTex"), 0);
	UpdateShaderMatrices();
	UpdateNodeShaderMatrices();
	modelMatrix = Matrix4::Translation(Vector3(1.0f, 100.0f, 10.0f)) * Matrix4::Scale(Vector3(100, 100, 100));
	glUniformMatrix4fv(glGetUniformLocation(Animshader->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);
	vector <Matrix4 > frameMatrices;
	
	const Matrix4* invBindPose = MeshAnimate->GetInverseBindPose();
	const Matrix4* frameData = Anim->GetJointData(currentFrame);

	for (unsigned int i = 0; i < MeshAnimate->GetJointCount(); ++i) {
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);

	}

	int h = glGetUniformLocation(Animshader->GetProgram(), "joints");
	glUniformMatrix4fv(h, frameMatrices.size(), false,
		(float*)frameMatrices.data());
	for (int i = 0; i < MeshAnimate->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, matTexturesAnim[i]);
		MeshAnimate->DrawSubMesh(i);

	}
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}
void Renderer::DrawPostProcess() {
		Vector3 cameraPos = camera->GetPosition();
		int POST_PASSES = 0;
		if (cameraPos.z < heightMap->GetHeightmapSize().z / 1.8f) {
			if (cameraPos.z > heightMap->GetHeightmapSize().z / 2.2f) { 
			
				POST_PASSES = 150 ; }
		}
		else { POST_PASSES = 0; }
	
		glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, bufferColourTex[1], 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		BindShader(processShader);
		modelMatrix.ToIdentity();
		viewMatrix.ToIdentity();
		projMatrix.ToIdentity();
		
		UpdateShaderMatrices();

		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(
			processShader->GetProgram(), "sceneTex"), 0);
		for (int i = 0; i < POST_PASSES; ++i) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D, bufferColourTex[1], 0);
			glUniform1i(glGetUniformLocation(processShader->GetProgram(),
				"isVertical"), 0);

			glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
			quad->Draw();
			//Now to swap the colour buffers , and do the second blur pass
			glUniform1i(glGetUniformLocation(processShader->GetProgram(),
				"isVertical"), 1);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D, bufferColourTex[0], 0);
			glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
			quad->Draw();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);

	}
void Renderer::PresentScene() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		BindShader(simpletexshader);
		modelMatrix.ToIdentity();
		viewMatrix.ToIdentity();
		projMatrix.ToIdentity();
		UpdateShaderMatrices();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
		glUniform1i(glGetUniformLocation(
			simpletexshader->GetProgram(), "diffuseTex"), 0);
		quad->Draw();

	}
	
