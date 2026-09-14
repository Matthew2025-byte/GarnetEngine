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
#include "Sprite.hpp"

namespace Garnet {
/**
 * @brief Struct containing communication information betwen render and update threads
 * Automatically destroys contained members
 *
 */
struct ThreadData {
	Garnet::Registry initRegistry;
	SpriteBuffer initSpriteBuff;
	std::unordered_map<int, Sprite> sprite_deltaBuff[2];
	SDL_Mutex* mutexes[2];
	SDL_AtomicInt renderReg{0};
	SDL_AtomicInt running{1};
    std::vector<std::function<void(float, GameState&)>> callbacks;

	std::vector<SDL_Event> events;
	SDL_Mutex* eventMutex;

	ThreadData(const Registry& referenceRegistry, const SpriteBuffer& spriteBuffer, const std::vector<std::function<void(float, GameState&)>>& callbacks)
		: callbacks(callbacks) {
        initRegistry = referenceRegistry;
		initSpriteBuff = spriteBuffer;
        mutexes[0] = SDL_CreateMutex();
        mutexes[1] = SDL_CreateMutex();
		eventMutex = SDL_CreateMutex();
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
	void addScene(std::string name, std::unique_ptr<Scene> scene);
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
	 * @brief Set the Active Scene object
	 * 
	 * @param name Name of the scene to load
	 */
	void setActiveScene(std::string name);

	/**
	 * @brief Saves current scene state and loads the new scene
	 *
	 * @param name Scene to switch to Load
	 */
	void switchScene(std::string name);

	// Game loop
	void start(const SpriteBuffer& spriteBuff);
	std::unordered_map<int, Sprite> getSpriteDelta();
	TextureManager& getTextureManager() { return textures; }

	void logEvent(SDL_Event event);
	void pushEvents();

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
	std::unordered_map<std::string, std::unique_ptr<Scene>> availableScenes;

	TextureManager textures;

	// Variables for update thread
	std::unique_ptr<ThreadData> sceneData;
	SDL_Thread* updateThread;

	std::vector<SDL_Event> events;

	static int SDLCALL threadLogic(void* args);
};
}  // namespace Garnet