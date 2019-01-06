#pragma once
#include "EditorWindow.h"
#include <string>
#include <vector>

class Texture;
class Animation;

class AnimationWindow :
	public EditorWindow
{
public:
	AnimationWindow(const char* windowName, bool enabled = false);
	~AnimationWindow();

	void DrawWindow();

	void SetAnimationToEdit(Animation** animation);

private:
	Animation* animationToEdit;
	std::string animationName;
	std::vector<Texture*> animationTextures;
	float speed;
	bool loop;

	bool playing;
	bool pause;
	int frameIndex;
};

