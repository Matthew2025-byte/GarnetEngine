#include "GarnetEngine/Scene.hpp"
using namespace Garnet;

void SpriteBuffer::push(const std::unordered_map<int, Sprite>& delta) {
	for (auto& [index, sprite] : delta) {
		if (index >= sprites.size()) {
			sprites.resize(index + 1);
		}
		sprites[index] = sprite;
	}
}
void SpriteBuffer::push() {
	push(this->spriteDelta);
	spriteDelta.clear();
}
void SpriteBuffer::setDelta(std::unordered_map<int, Sprite> delta) {
	this->spriteDelta.clear();
	this->spriteDelta = delta;
}
void SpriteBuffer::setSprites(std::vector<Sprite> sprites) { this->sprites = sprites; }
void SpriteBuffer::addSprite(Entity entity, Sprite sprite) {
	this->entity_index[entity] = sprites.size();
	this->sprites.push_back(sprite);
    this->spriteDelta[entity_index[entity]] = sprite;
}
Sprite& SpriteBuffer::operator[](Entity entity) {
	auto it = entity_index.find(entity);
	assert(it != entity_index.end() && "No sprite found with requested entity");
	Entity index = entity_index[entity];
	spriteDelta[index] = sprites[index];
	return spriteDelta[index];
}