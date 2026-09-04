#include "GarnetEngine/SceneManager.hpp"

#include <stdexcept>

using namespace Garnet;

void SceneManager::addScene(std::string name, Scene& scene) {
	if (availableScenes.find(name) != availableScenes.end()) {
		SDL_Log("Scene already exists with name: %s", name);
		return;
	}
	availableScenes[name] = &scene;
    if (!activeScene) {
        setActiveScene(name);
    }
}

void SceneManager::loadScene(std::string name) {
	// Load scene to availableScenes from a file
	throw std::runtime_error("loadScene Not yet implemented");
}

void SceneManager::saveScene(std::string name) {
	// Save scene to a file
	throw std::runtime_error("saveScene Not yet implemented");
}

void Garnet::SceneManager::setActiveScene(std::string name) {
	activeScene = availableScenes[name];
	usedAssets assets = activeScene->getRequiredAssets();

	for (auto texture : assets.textures) {
		textures.Load(texture.first.c_str(), texture.second);
	}
}

void SceneManager::switchScene(std::string name) {
	// Save scene
	// Load new scene
	throw std::runtime_error("switchScene Not yet implemented");
}

void SceneManager::start() {
    SDL_Log("Initializing Scene");
	auto& callbacks = activeScene->getCallbacks();
    // Currently crashing on this line
	sceneData = std::make_unique<ThreadData>(activeScene->getInitRegistry(), callbacks);

	updateThread = SDL_CreateThread(threadLogic, "Update Logic", sceneData.get());
    if (!updateThread) {
        SDL_Log("Unable to create thread: %s", SDL_GetError());
        return;
    }
    SDL_Log("Created Thread");
}

Registry& SceneManager::getRenderRegistry() {
	return sceneData->registries[SDL_GetAtomicInt(&sceneData->renderReg)];
}

int SDLCALL SceneManager::threadLogic(void* args) {
	std::unique_ptr<ThreadData> data(static_cast<ThreadData*>(args));
	Registry lastBuffer = data->registries[!SDL_GetAtomicInt(&data->renderReg)];

	constexpr Uint64 target_ns = 16'666'667;  // Target tick time in NS
	Uint64 last_ticks = SDL_GetTicksNS();	  // last frame for dt

	while (SDL_GetAtomicInt(&data->running)) {
		Uint64 frame_start = SDL_GetTicksNS();
		float dt = (frame_start - last_ticks) / 1'000'000'000.f;
		last_ticks = frame_start;

		for (auto& func : data->callbacks) {
			func(dt, lastBuffer);
		}

		// Locks the written-to mutex and associated registry and copies the internal buffer to
        // the render buffer
		int writeBuff = !SDL_GetAtomicInt(&data->renderReg);
		SDL_LockMutex(data->mutexes[writeBuff]);
		data->registries[writeBuff] = lastBuffer;
		SDL_UnlockMutex(data->mutexes[writeBuff]);

		SDL_SetAtomicInt(&data->renderReg, writeBuff);

		Uint64 frame_end = SDL_GetTicksNS();
		Uint64 elapsed = frame_end - frame_start;
		// delay elapsed to 1/60s
		if (elapsed < target_ns) {
			SDL_DelayNS(elapsed - target_ns);
		}
	}
	return 0;
}
