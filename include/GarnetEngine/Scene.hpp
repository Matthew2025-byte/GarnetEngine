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
#include <functional>
#include <vector>
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
                break;
        }
    };
    /**
     * @brief Get the requiredAssets object
     * 
     * @return A list of all assets required to properly handle the scene
     */
    usedAssets getRequiredAssets() { return requiredAssets; }

    /**
         * @brief Binds a scene-wide system.
         *
         * Intended for systems that need access to the entire
         * scene or registry rather than individual entities.
         *
         * @param func Function matching (float dt, Scene&).
         */
        void bindSystem(void (*func)(float, Scene&))
        {
            callbacks.emplace_back(
                [this, func](float dt, Registry& registry)
                {
                    func(dt, *this);
                }
            );
        }

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
    void update(float dt, Registry& registry)
    {
        for (auto& cb : callbacks)
            cb(dt, registry);
    }
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

	/**
	 * @brief Binds a standalone method to the scene
	 * Argument types are automatically determined at compile time.
	 * Passes deltatime (dt), the currently selected entity, and a reference to the requested
	 * components bound to the entity.  An example use case would be updating a entities position
	 * based on velocity, where you don't need to access all entities at once.
	 * 
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

	/**
	 * @brief Binds a system to the scene
	 *
	 * Passes deltatime (dt), the registry, and a vector of entities that matches the given filter
	 * to the provided system. An example use case would be binding a collision detection system,
	 * where you need access to all entities at once.
	 *
	 * @tparam Components The filter used to select entities
	 * @param func System to bind matching the signature (float dt, Registry& r, const std::vector<Entity>& e)
	 */
	template <typename... Components>
	void bindSystem(void (*func)(float, Registry&, const std::vector<Entity>&)) {
		callbacks.emplace_back([func](float dt, Registry& registry) {
			auto entities = registry.getEntities<Components...>();
			func(dt, registry, entities);
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
