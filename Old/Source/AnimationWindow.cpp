#include "AnimationWindow.h"
#include <imgui.h>
#include "Texture.h"
#include "CustomImGui.h"
#include "Animation.h"
#include <tinyfiledialogs.h>
#include "Data.h"
#include "ResourceManagerModule.h"
#include "Application.h"
#include "FileSystemModule.h"
#include "CustomImGui.h"

AnimationWindow::AnimationWindow(const char* windowName, bool enabled) : EditorWindow(windowName, enabled)
{
	speed = 1.0f;
	loop = false;
	playing = false;
	pause = false;
	frameIndex = 0;
}

AnimationWindow::~AnimationWindow()
{
	animationTextures.clear();
}

void AnimationWindow::DrawWindow()
{
	ImGui::SetNextWindowSize({ 500,700 });
	ImGui::Begin("Animation", &enabled,
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse);

	static char* name;
	name = _strdup(animationName.c_str());
	ImGui::Text("Name");
	ImGui::SameLine(50);
	if (ImGui::InputText("##Name", name, 256, 0))
	{
		animationName = name;
	}

	ImGui::Text("Speed");
	ImGui::SameLine(50);
	ImGui::InputFloat("##Speed", &speed);
	ImGui::Text("Loop");
	ImGui::SameLine(50);
	ImGui::Checkbox("##Loop", &loop);
	ImGui::Spacing();
	ImGui::Text("Images");
	ImGui::Separator();

	for (int i = 0; i < animationTextures.size(); i++)
	{
		std::string name = "##" + std::to_string(i);
		ImGui::InputText(name.c_str(), animationTextures[i]->GetName().data(), animationTextures[i]->GetName().size(), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button(("Remove" + name).c_str()))
		{
			animationTextures.erase(animationTextures.begin() + i);
		}
	}

	Texture* texture = nullptr;
	std::string framName = "##frame" + std::to_string(animationTextures.size());
	if (ImGui::InputTexture(framName.c_str(), &texture))
	{
		animationTextures.emplace_back(texture);
	}

	if (ImGui::Button("Save"))
	{
		char const * lFilterPatterns[1] = { "*.animation" };
		const char* path = tinyfd_saveFileDialog("Save Animation...", animationName.c_str(), 1, lFilterPatterns, NULL);
		if (path != NULL) {
			std::string spath(path);
			Data data;
			data.AddFloat("Speed", speed);
			data.AddBool("Loop", loop);
			data.AddInt("ImagesCount", animationTextures.size());
			for (int i = 0; i < animationTextures.size(); i++) {
				data.AddString("ImagePath" + std::to_string(i), animationTextures[i]->GetAssetsPath());
			}
			if (App->fileSystemModule->GetExtension(path) != ".animation")
			{
				spath += ".animation";
			}
			data.SaveData(spath);
			App->resourceManagerModule->CreateResource(spath);
		}

		if (animationToEdit != nullptr)
		{
			animationToEdit->SetSpeed(speed);
			animationToEdit->SetLoop(loop);
			animationToEdit->SetName(animationName);
			animationToEdit->UpdateTextures(animationTextures);
		}
	}

	ImGui::Spacing();
	ImGui::Text("Preview");
	ImGui::Separator();

	if (!playing || pause)
	{
		if (ImGui::Button("Play"))
		{
			playing = true;
			pause = false;
		}
	}
	else
	{
		if (ImGui::Button("Pause"))
		{
			pause = true;
			playing = false;
		}
	}
	
	if (playing || pause)
	{
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			playing = false;
			pause = false;
			frameIndex = 0;
		}
	}

	if (playing)
	{
		frameIndex += speed * App->GetDeltaTime();
		if (frameIndex >= animationTextures.size())
		{
			frameIndex = (loop) ? 0.0f : animationTextures.size() - 1;
		}
	}

	ImGui::Image(*animationTextures[(int)frameIndex]);

	ImGui::End();
}

void AnimationWindow::SetAnimationToEdit(Animation ** animation)
{
	animationToEdit = *animation;
	animationName = (*animation)->GetName();
	speed = (*animation)->GetSpeed();
	loop = (*animation)->GetIsLoop();
	animationTextures = (*animation)->GetTextures();
	enabled = true;
}
