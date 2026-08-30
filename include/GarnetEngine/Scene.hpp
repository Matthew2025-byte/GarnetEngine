/**
 * @file Scene.hpp
 * @author Matthew2025-byte
 * @brief Contains Class Scene for coordinating level specific assets and logic
 *
 */

#pragma once
#include <string>
#include <vector>
#include "Registry.hpp"
#include "Renderer.hpp"

namespace Garnet {
/**
 * @brief Object returned by Scene.getRequiredAssets();
 * 
 */
struct usedAssets {
	std::vector<std::string> textures;
};
enum assetType {
	Texture,
};

/**
 * @brief Class representative of a single game level
 *
 * Scene is a class that coordinates all necessary assets needed to properly run a user defined
 * level.  It does not “own” any major parts of the engine, but contains lists of all assets and
 * components needed for its specific level.  It coordinates with asset managers to load necessary
 * information.It also keeps track of the game savestate (Non-hardcoded variables, unique to a save
 * file).
 */
class Scene {
	public:
	/**
	 * @brief Adds an asset to the asset list
	 * 
	 * @param name Name of asset to add
	 * @param type Type (eg. Texture)
	 */
	void addAsset(std::string name, assetType type) {
		switch (type) {
			case Texture:
				requiredAssets.textures.push_back(name);
		}
	};
	/**
	 * @brief Get the requiredAssets object
	 * 
	 * @return A list of all assets required to properly handle the scene
	 */
	usedAssets getRequiredAssets() { return requiredAssets; }

	/**
	 * @brief Runs all bound methods
	 *
	 * @param dt deltaTime for physics updates
	 */
	void update(float dt, Registry& registry);

	/**
	 * @brief Binds a method to Scene.update(dt)
	 * @details Argument types are automatically determined at compile time
	 *
	 * @param func Function to bind matching (float dt, Components&...)
	 */
	template <typename... Components>
	void bind(void (*func)(float, Entity, Components&...)) {
		callbacks.emplace_back([func](float dt, Registry& registry) {
			registry.each<Components...>(
				[func, dt](Entity e, Components&... c) { func(dt, e, c...); });
		});
	}

	std::vector<std::function<void(float, Registry&)>>& getCallbacks() { return callbacks; }
	Registry& getInitRegistry() { return initRegistry; }

	private:
	std::vector<std::function<void(float, Registry&)>> callbacks;
	usedAssets requiredAssets;
	Registry initRegistry;
};
}  // namespace Garnet