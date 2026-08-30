/**
 * @file Engine.hpp
 * @author Matthew2025-byte
 * @brief Declaration of Garnet::Engine
 *
 */
#pragma once
#include <map>
#include <string>
#include <functional>

#include "Registry.hpp"
#include "Scene.hpp"

namespace Garnet {
/**
 * @brief Struct containing communication information betwen render and update threads
 * Automatically destroys contained members
 *
 */
struct ThreadData {
	Garnet::Registry registries[2];
	SDL_Mutex* mutexes[2];
	SDL_AtomicInt renderReg{0};
	SDL_AtomicInt running{1};
    std::vector<std::function<void(float, Registry&)>> callbacks;

	ThreadData(const Registry& referenceRegistry, const std::vector<std::function<void(float, Registry&)>>& callbacks)
		: callbacks(callbacks) {
        registries[0] = referenceRegistry;
        registries[1] = referenceRegistry;
        mutexes[0] = SDL_CreateMutex();
        mutexes[1] = SDL_CreateMutex();
    }
    ~ThreadData() {
        SDL_DestroyMutex(mutexes[0]);
        SDL_DestroyMutex(mutexes[1]);
    }
    
};
/**
 * @brief Main coordinator for rendering and logic
 *
 */
class SceneManager {
	public:
	// Scene Handling
	/**
	 * @brief Adds a scene object to game memory
	 *
	 * @param name Name of the scene
	 * @param scene Scene object
	 */
	void addScene(std::string name, Scene& scene);
	/**
	 * @brief Loads a scene into memory
	 *
	 * @param name Name of the scene
	 */
	void loadScene(std::string name);
	/**
	 * @brief Saves a scene to a file
	 *
	 * @param name Name of the scene
	 */
	void saveScene(std::string name);
	/**
	 * @brief Saves current scene state and loads the new scene
	 *
	 * @param name Scene to switch to
	 */
	void switchScene(std::string name);

	// Game loop
	void start();
	Registry& getRenderRegistry();

	void setActiveScene(Scene* scene) { activeScene = scene; }

	SceneManager(SDL_Renderer* r)
		: activeScene(nullptr), updateThread(nullptr), textures(r) {}
	~SceneManager() {
		if (updateThread) {
			SDL_SetAtomicInt(&sceneData->running, 0);
			SDL_WaitThread(updateThread, nullptr);
		}
	}

	private:
	std::string activeSceneName;
	Scene* activeScene;
	std::unordered_map<std::string, Scene*> availableScenes;

	TextureManager textures;

	// Variables for update thread
	std::unique_ptr<ThreadData> sceneData;
	SDL_Thread* updateThread;

	static int SDLCALL threadLogic(void* args);
};
}  // namespace Garnet