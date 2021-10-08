#pragma once

class Scene;

enum
{
	MAX_LAYER = 32
};

class SceneManager
{
	DECLARE_SINGLE(SceneManager)

public:
	void Update();
	void Render();//TEMP
	void LoadScene(wstring sceneName);

	void SetLayerName(uint8 index, const wstring& name);
	const wstring& IndexToLayerName(uint8 index) { return _layerNames[index]; }
	uint8 LayerNameToIndex(const wstring& name);

	shared_ptr<class GameObject> Pick(int32 screenX, int32 screenY);

public:
	shared_ptr<Scene> GetActiveScene() { return _activeScene; }

	//test code
private:
	shared_ptr<Scene> LoadTestScene();

private:
	shared_ptr<Scene> _activeScene;
	
	array<wstring, MAX_LAYER> _layerNames;	// layer index로 layer 이름 관리
	map<wstring, uint8> _layerIndex;		// layer 이름으로 layer index 관리
};

