#include "GarnetEngine/Scene.hpp"
#include "GarnetEngine/Components.hpp"

using namespace Garnet;
void Scene::addAsset(std::string name, assetType type, std::unordered_map<std::string, std::string> props) {
	switch (type) {
		case Texture: 
			requiredAssets.textures[name].push_back(std::move(props));
	}
};

usedAssets Scene::getRequiredAssets() { return requiredAssets; }
std::vector<std::function<void(float, GameState&)>>& Scene::getCallbacks() { return callbacks; }
Registry& Scene::getInitRegistry() { return initRegistry; }