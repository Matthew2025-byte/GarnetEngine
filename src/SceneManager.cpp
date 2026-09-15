#include "GarnetEngine/SceneManager.hpp"
#include "GarnetEngine/Sprite.hpp"

#include <stdexcept>

using namespace Garnet;

void SceneManager::addScene(std::string name, std::unique_ptr<Scene> scene) {
	if (availableScenes.find(name) != availableScenes.end()) {
		SDL_Log("Scene already exists with name: %s", name);
		return;
	}
	availableScenes[name] = std::move(scene);
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
	activeScene = availableScenes[name].get();
	usedAssets assets = activeScene->getRequiredAssets();

	for (auto texture : assets.textures) {
		// update to get from texture vector
		for (auto properties : texture.second) {
			textures.Load(texture.first.c_str(), properties);
		}
	}
}

void SceneManager::switchScene(std::string name) {
	// Save scene
	// Load new scene
	throw std::runtime_error("switchScene Not yet implemented");
}

void SceneManager::start(const SpriteBuffer& spriteBuff) {
    SDL_Log("Initializing Scene");
	auto& callbacks = activeScene->getCallbacks();
	auto& initRegistry = activeScene->getInitRegistry();
	SpriteBuffer buff = spriteBuff;
	SDL_Log("Loading %i sprites", buff.getSprites().size());

	initRegistry.each<Components::Sprite>([&](Entity entity, Components::Sprite spriteInfo) {
		const std::string& textureName = spriteInfo.textureName;
		TextureID texture = getTextureManager().findTexture(textureName);
		buff[entity].texture = this->getTextureManager().getTexture(texture);
	});
	auto delta = buff.getDelta();
	buff.push();

	sceneData = std::make_unique<ThreadData>(activeScene->getInitRegistry(), buff, callbacks);
	sceneData->sprite_deltaBuff[0] = delta;
	sceneData->sprite_deltaBuff[1] = delta;

	updateThread = SDL_CreateThread(threadLogic, "Update Logic", sceneData.get());
    if (!updateThread) {
        SDL_Log("Unable to create thread: %s", SDL_GetError());
        return;
    }
    SDL_Log("Created Thread");
}

std::unordered_map<int, Sprite> Garnet::SceneManager::getSpriteDelta() {
	return sceneData->sprite_deltaBuff[SDL_GetAtomicInt(&sceneData->renderReg)];
}

void Garnet::SceneManager::logEvent(SDL_Event event) {
	this->events.push_back(event);
}

void Garnet::SceneManager::pushEvents() {
	if (!SDL_TryLockMutex(sceneData->eventMutex)) {
		return;
	}

	sceneData->events.resize(sceneData->events.size() + this->events.size());
	sceneData->events.append_range(this->events);
	this->events.clear();

	SDL_UnlockMutex(sceneData->eventMutex);
}

int SDLCALL SceneManager::threadLogic(void* args) {
	ThreadData& data = *static_cast<ThreadData*>(args);
	Registry lastBuffer = data.initRegistry;
	SpriteBuffer spriteBuffer = data.initSpriteBuff;
	GameState gameState = GameState(lastBuffer);

	constexpr Uint64 target_ns = 16'666'667;  // Target tick time in NS
	Uint64 last_ticks = SDL_GetTicksNS();	  // last frame for dt

	while (SDL_GetAtomicInt(&data.running)) {
		Uint64 frame_start = SDL_GetTicksNS();
		float dt = (frame_start - last_ticks) / 1'000'000'000.f;
		last_ticks = frame_start;

		while (data.events.size() > 0) {
			SDL_Event event = data.events.back();
			switch(event.type) {
				case SDL_EVENT_KEY_DOWN:
					gameState.input.keyboard.setKey(event.key.scancode, KeyEvent::DOWN);
					break;
				case SDL_EVENT_KEY_UP:
					gameState.input.keyboard.setKey(event.key.scancode, KeyEvent::UP);
					break;
				case SDL_EVENT_MOUSE_MOTION:
					gameState.input.mouse.position = { event.motion.x, event.motion.y };
					gameState.input.mouse.delta = { event.motion.xrel, event.motion.yrel };
					break;
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					gameState.input.mouse.buttonStates[event.button.button] = true;
					break;
				case SDL_EVENT_MOUSE_BUTTON_UP:
					gameState.input.mouse.buttonStates[event.button.button] = false;
					break;
			}

			data.events.pop_back();
		}

		for (auto& func : data.callbacks) {
			func(dt, gameState);
		}
		
		gameState.input.mouse.delta = {0, 0};

		lastBuffer.each<Components::Sprite, Components::Transform>([&](Entity entity, Components::Sprite sprite, Components::Transform& transform) {
			spriteBuffer[sprite.id].transform = transform;
		});

		// Locks the written-to mutex and associated registry and copies the internal buffer to
        // the render buffer
		int writeBuff = !SDL_GetAtomicInt(&data.renderReg);
		SDL_LockMutex(data.mutexes[writeBuff]);
		data.sprite_deltaBuff[writeBuff] = spriteBuffer.getDelta();
		SDL_UnlockMutex(data.mutexes[writeBuff]);
		spriteBuffer.push();

		SDL_SetAtomicInt(&data.renderReg, writeBuff);

		Uint64 frame_end = SDL_GetTicksNS();
		Uint64 elapsed = frame_end - frame_start;
		// delay elapsed to 1/60s
		if (elapsed < target_ns) {
			SDL_DelayNS(target_ns - elapsed);
		}
	}
	return 0;
}
