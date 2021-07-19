#ifndef SCENE_COLLISION_H
#define SCENE_COLLISION_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

class SceneCollision : public SceneBase
{
public:
	SceneCollision();
	~SceneCollision();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject* go);

	void BuildThickWall(Vector3 scale, Vector3 normal, Vector3 pos);

	GameObject* FetchGO();
	void ReturnGO(GameObject* go);
	bool CheckCollision(GameObject* go1, GameObject* go2);
	void CollisionResponse(GameObject* go1, GameObject* go2);
protected:

	//Physics
	std::vector<GameObject*> m_goList;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	GameObject* m_ghost;
	int m_objectCount;

	float coefFriction = 0.05;

	//Auditing
	float m1, m2;
	Vector3 u1, u2, v1, v2;
};

#endif