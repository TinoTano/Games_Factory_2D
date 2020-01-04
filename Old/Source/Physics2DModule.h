#pragma once
#include "Module.h"
#include <Dynamics/b2WorldCallbacks.h>
#include <vec2.hpp>

class b2World;
class b2Body;
class b2Fixture;
class b2Shape;
class GameObject;
class Component;
class Data;

class ContactListener : public b2ContactListener
{
public:
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact*contact);
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	void PostSolve(b2Contact*contact, const b2ContactImpulse* impulse);
};

class Physics2DModule :
	public Module
{
public:
	Physics2DModule(const char* module_name, bool game_module = false);
	~Physics2DModule();

	bool Init(Data& settings);
	bool Update(float deltaTime);
	bool CleanUp();

	b2Body* CreateBody(GameObject* go);
	void DestroyBody(b2Body* body);
	void SetBodyTransform(b2Body * body, glm::vec2 pos, float angle, glm::vec2 scale);

	b2Fixture* CreateBoxCollider(b2Body* body, float width, float height);
	b2Fixture* CreateCircleCollider(b2Body* body, float radius);
	b2Fixture* CreateChainCollider(b2Body* body);
	b2Fixture* CreateEdgeCollider(b2Body* body, float x0, float y0, float x1, float y1);

	void SetGameObjectToDebug(GameObject& go);

public:
	float worldStep;

private:
	b2Fixture* CreateFixture(b2Body* body, b2Shape* shape);
	void DrawDebugData(GameObject& go);

private:
	b2World* physicsWorld;
	ContactListener* contactListener;
	int velocityIterations;
	int positionIterations;
	GameObject* selectedObject;
};

