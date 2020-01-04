#include "Physics2DModule.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include <Dynamics/b2World.h>
#include <Dynamics/b2Body.h>
#include <Collision/Shapes/b2PolygonShape.h>
#include <Collision/Shapes/b2EdgeShape.h>
#include <Collision/Shapes/b2CircleShape.h>
#include <Collision/Shapes/b2ChainShape.h>
#include <Dynamics/b2Fixture.h>
#include <Collision/Shapes/b2Shape.h>
#include <Dynamics/Contacts/b2Contact.h>
#include "Application.h"
#include "ComponentPhysicsBody.h"
#include "Vertex.h"
#include "RendererModule.h"
#include "ComponentBoxCollider.h"
#include "ComponentCircleCollider.h"
#include "LuaScripting.h"
#include "LuaScript.h"
#include "ComponentScript.h"

#define PIXEL_TO_METERS(p) ((float) 0.02f * p)
#define METERS_TO_PIXELS(m) ((float) 50.0f * m)

Physics2DModule::Physics2DModule(const char* module_name, bool game_module) : Module(module_name, game_module)
{
	physicsWorld = nullptr;
	selectedObject = nullptr;
	velocityIterations = 8;
	positionIterations = 3;
}

Physics2DModule::~Physics2DModule()
{
	physicsWorld = nullptr;
	contactListener = nullptr;
	selectedObject = nullptr;
}

bool Physics2DModule::Init(Data& settings)
{
	physicsWorld = new b2World(b2Vec2(0.0f, -10.0f));
	contactListener = new ContactListener();
	physicsWorld->SetContactListener(contactListener);
	return true;
}

bool Physics2DModule::Update(float deltaTime)
{
	if (App->IsPlaying())
	{
		physicsWorld->Step(0.2f * deltaTime, velocityIterations, positionIterations);

		for (b2Body* body = physicsWorld->GetBodyList(); body; body = body->GetNext())
		{
			GameObject* go = (GameObject*)body->GetUserData();
			ComponentTransform* transform = (ComponentTransform*)go->GetComponentOfType(Component::TRANSFORM);
			if (transform != nullptr)
			{
				b2Vec2 pos = body->GetPosition();
				transform->SetPosition({ METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y) });
				float angle = body->GetAngle();
				transform->SetRotation(glm::degrees(angle));
			}
		}
	}

	if (selectedObject != nullptr)
	{
		App->rendererModule->CleanDebugVertex();
		DrawDebugData(*selectedObject);
	}
	
	return true;
}

bool Physics2DModule::CleanUp()
{
	delete contactListener;
	contactListener = nullptr;

	delete physicsWorld;
	physicsWorld = nullptr;

	return true;
}

b2Body * Physics2DModule::CreateBody(GameObject* go)
{
	ComponentTransform* transform = (ComponentTransform*)go->GetComponentOfType(Component::TRANSFORM);
	glm::vec2 pos = transform->GetGlobalPosition();
	b2Body* body = nullptr;
	b2BodyDef def;
	def.active = true;
	def.position.Set(PIXEL_TO_METERS(pos.x), PIXEL_TO_METERS(pos.y));
	def.userData = go;
	def.type = b2BodyType::b2_dynamicBody;

	body = physicsWorld->CreateBody(&def);

	return body;
}

void Physics2DModule::DestroyBody(b2Body * body)
{
	physicsWorld->DestroyBody(body);
}

void Physics2DModule::SetBodyTransform(b2Body * body, glm::vec2 pos, float angle, glm::vec2 scale)
{
	b2Vec2 b2Pos(PIXEL_TO_METERS(pos.x), PIXEL_TO_METERS(pos.y));
	body->SetTransform(b2Pos, glm::radians(angle));
}

b2Fixture * Physics2DModule::CreateFixture(b2Body * body, b2Shape* shape)
{
	b2Fixture* fixture = nullptr;
	b2FixtureDef fixtureDef;
	fixtureDef.shape = shape;
	fixtureDef.userData = body->GetUserData();
	fixture = body->CreateFixture(&fixtureDef);
	return fixture;
}

void Physics2DModule::DrawDebugData(GameObject& go)
{
	ComponentPhysicsBody* physBody = (ComponentPhysicsBody*)go.GetComponentOfType(Component::PHYSICS_BODY);
	if (physBody != nullptr)
	{
		b2Body* body = physBody->GetBody();
		if (body->IsActive())
		{
			const b2Transform& transform = body->GetTransform();

			for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
			{
				switch (fixture->GetType())
				{
				case b2Shape::e_circle:
				{
					b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();

					b2Vec2 center = b2Mul(transform, circle->m_p);
					float32 radius = circle->m_radius;
					//radius = METERS_TO_PIXELS(radius);

					const float32 k_segments = 32.0f;
					const float32 k_increment = 2.0f * b2_pi / k_segments;
					float32 sinInc = sinf(k_increment);
					float32 cosInc = cosf(k_increment);
					b2Vec2 r1(1.0f, 0.0f);
					b2Vec2 v1 = center + radius * r1;
					for (int32 i = 0; i < k_segments; ++i)
					{
						// Perform rotation to avoid additional trigonometry.
						b2Vec2 r2;
						r2.x = cosInc * r1.x - sinInc * r1.y;
						r2.y = sinInc * r1.x + cosInc * r1.y;
						b2Vec2 v2 = center + radius * r2;
						Vertex v_1;
						v_1.pos = { METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y) };
						v_1.color = { 0,1,0,1 };

						Vertex v_2;
						v_2.pos = { METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y) };
						v_2.color = { 0,1,0,1 };

						App->rendererModule->AddDebugVertex(v_1);
						App->rendererModule->AddDebugVertex(v_2);
						r1 = r2;
						v1 = v2;
					}
				}
				break;
				case b2Shape::e_edge:
				{
					b2EdgeShape* edge = (b2EdgeShape*)fixture->GetShape();
					b2Vec2 v1 = b2Mul(transform, edge->m_vertex1);
					b2Vec2 v2 = b2Mul(transform, edge->m_vertex2);
					Vertex v_1;
					v_1.pos = { METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y) };
					v_1.color = { 0,1,0,1 };

					Vertex v_2;
					v_2.pos = { METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y) };
					v_2.color = { 0,1,0,1 };

					App->rendererModule->AddDebugVertex(v_1);
					App->rendererModule->AddDebugVertex(v_2);
				}
				break;
				case b2Shape::e_chain:
				{
					b2ChainShape* chain = (b2ChainShape*)fixture->GetShape();
					int32 count = chain->m_count;
					const b2Vec2* vertices = chain->m_vertices;

					b2Vec2 v1 = b2Mul(transform, vertices[0]);

					for (int32 i = 1; i < count; ++i)
					{
						b2Vec2 v2 = b2Mul(transform, vertices[i]);

						Vertex v_1;
						v_1.pos = { METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y) };
						v_1.color = { 0,1,0,1 };

						Vertex v_2;
						v_2.pos = { METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y) };
						v_2.color = { 0,1,0,1 };

						App->rendererModule->AddDebugVertex(v_1);
						App->rendererModule->AddDebugVertex(v_2);

						v1 = v2;
					}
				}
				break;
				case b2Shape::e_polygon:
				{
					b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
					int32 vertexCount = poly->m_count;
					b2Assert(vertexCount <= b2_maxPolygonVertices);
					b2Vec2 vertices[b2_maxPolygonVertices];

					Vertex firstVert;

					for (int32 i = 0; i < vertexCount; ++i)
					{
						vertices[i] = b2Mul(transform, poly->m_vertices[i]);

						Vertex v;
						v.pos = { METERS_TO_PIXELS(vertices[i].x), METERS_TO_PIXELS(vertices[i].y) };
						v.color = { 0,1,0,1 };

						App->rendererModule->AddDebugVertex(v);

						if (i == 0)
						{
							firstVert = v;
						}
						else
						{
							App->rendererModule->AddDebugVertex(v);

							if (i == 3)
							{
								App->rendererModule->AddDebugVertex(firstVert);
							}
						}
					}
				}
				break;
				default:
					break;
				}
			}
		}
	}

	std::vector<GameObject*> childs = go.GetChilds();
	for (GameObject* go : childs)
	{
		DrawDebugData(*go);
	}
}

b2Fixture* Physics2DModule::CreateBoxCollider(b2Body * body, float width, float height)
{
	b2PolygonShape shape;
	shape.SetAsBox(PIXEL_TO_METERS(width), PIXEL_TO_METERS(height));
	b2Fixture* fixture = CreateFixture(body, &shape);
	return fixture;
}

b2Fixture* Physics2DModule::CreateCircleCollider(b2Body * body, float radius)
{
	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2Fixture* fixture = CreateFixture(body, &shape);
	return fixture;
}

b2Fixture* Physics2DModule::CreateChainCollider(b2Body * body)
{
	b2ChainShape shape;
	//shape.
	b2Fixture* fixture = CreateFixture(body, &shape);
	return fixture;
}

b2Fixture* Physics2DModule::CreateEdgeCollider(b2Body * body, float x0, float y0, float x1, float y1)
{
	b2EdgeShape shape;
	shape.Set({ x0,y0 }, { x1,y1 });
	b2Fixture* fixture = CreateFixture(body, &shape);
	return fixture;
}

void Physics2DModule::SetGameObjectToDebug(GameObject & go)
{
	selectedObject = &go;
}

void ContactListener::BeginContact(b2Contact * contact)
{
	b2Fixture* fixture1 = contact->GetFixtureA();
	b2Fixture* fixture2 = contact->GetFixtureB();

	const char* functionName = "OnCollisionEnter";
	if (fixture1->IsSensor() || fixture2->IsSensor())
	{
		functionName = "OnSensorEnter";
	}

	if (fixture1 != nullptr)
	{
		GameObject* go = (GameObject*)fixture1->GetUserData();
		std::vector<Component*> scripts = go->GetComponentsOfType(Component::SCRIPT);
		for (Component* compScript : scripts)
		{
			LuaScript* script = ((ComponentScript*)compScript)->GetScript();
			if (script != nullptr)
			{
				if (LuaFunctions::FunctionExist(script->luaState, functionName))
				{
					LuaFunctions::CallUserDataFunction(script->luaState, functionName, 1, 0, go);
				}
			}
		}
	}

	if (fixture2 != nullptr)
	{
		GameObject* go = (GameObject*)fixture2->GetUserData();
		std::vector<Component*> scripts = go->GetComponentsOfType(Component::SCRIPT);
		for (Component* compScript : scripts)
		{
			LuaScript* script = ((ComponentScript*)compScript)->GetScript();
			if (script != nullptr)
			{
				if (LuaFunctions::FunctionExist(script->luaState, functionName))
				{
					LuaFunctions::CallUserDataFunction(script->luaState, functionName, 1, 0, go);
				}
			}
		}
	}
}

void ContactListener::EndContact(b2Contact * contact)
{
	b2Fixture* fixture1 = contact->GetFixtureA();
	b2Fixture* fixture2 = contact->GetFixtureB();

	const char* functionName = "OnCollisionExit";
	if (fixture1->IsSensor() || fixture2->IsSensor())
	{
		functionName = "OnSensorExit";
	}

	if (fixture1 != nullptr)
	{
		GameObject* go = (GameObject*)fixture1->GetUserData();
		std::vector<Component*> scripts = go->GetComponentsOfType(Component::SCRIPT);
		for (Component* compScript : scripts)
		{
			LuaScript* script = ((ComponentScript*)compScript)->GetScript();
			if (script != nullptr)
			{
				if (LuaFunctions::FunctionExist(script->luaState, functionName))
				{
					LuaFunctions::CallUserDataFunction(script->luaState, functionName, 1, 0, go);
				}
			}
		}
	}

	if (fixture2 != nullptr)
	{
		GameObject* go = (GameObject*)fixture2->GetUserData();
		std::vector<Component*> scripts = go->GetComponentsOfType(Component::SCRIPT);
		for (Component* compScript : scripts)
		{
			LuaScript* script = ((ComponentScript*)compScript)->GetScript();
			if (script != nullptr)
			{
				if (LuaFunctions::FunctionExist(script->luaState, functionName))
				{
					LuaFunctions::CallUserDataFunction(script->luaState, functionName, 1, 0, go);
				}
			}
		}
	}
}

void ContactListener::PreSolve(b2Contact * contact, const b2Manifold * oldManifold)
{
	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);
	if (worldManifold.normal.y < -0.5f)
	{
		contact->SetEnabled(false);
	}
}

void ContactListener::PostSolve(b2Contact * contact, const b2ContactImpulse * impulse)
{
}
