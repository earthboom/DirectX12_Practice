#pragma once

class Scene;


class SceneManager
{
	DECLARE_SINGLE(SceneManager)

public:
	void Update();
	void Render();//TEMP
	void LoadScene(wstring sceneName);

public:
	shared_ptr<Scene> GetActiveScene() { return _activeScene; }

	//test code
private:
	shared_ptr<Scene> LoadTestScene();

private:
	shared_ptr<Scene> _activeScene;
};

