#pragma once
#include "SceneNode.h"

class TreeScene : public SceneNode {
public:
	TreeScene(Mesh* cube);
	~TreeScene(void) {};
	void Update(float dt) override;

protected:
	SceneNode* head;
	SceneNode* leftArm;
	SceneNode* rightArm;
};
