#include "Projectile.hpp"
#include "EmitterNode.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cmath>
#include <cassert>


namespace
{
	const std::vector<ProjectileData> Table = initializeProjectileData();
}

Projectile::Projectile(Projectiles::ID type, const TextureHolder& textures)
: Entity(1)
, mType(type)
, mSprite(textures.get(Table[static_cast<int>(type)].texture), Table[static_cast<int>(type)].textureRect)
, mTargetDirection()
{
	centerOrigin(mSprite);
}

void Projectile::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	Entity::updateCurrent(dt, commands);
}

void Projectile::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mSprite, states);
}

unsigned int Projectile::getCategory() const
{
	if (mType == Projectiles::ID::RedGatlingBullet || mType == Projectiles::ID::RedLMGBullet || mType == Projectiles::ID::RedHMGBullet || mType == Projectiles::ID::RedTeslaBullet)
		return Category::EnemyProjectiles;
	else
		return Category::AlliedProjectiles;
}

sf::FloatRect Projectile::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

float Projectile::getMaxSpeed() const
{
	return Table[static_cast<int>(mType)].speed;
}

int Projectile::getDamage() const
{
	return Table[static_cast<int>(mType)].damage;
}
