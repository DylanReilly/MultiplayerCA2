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

Projectile::Projectile(Type type, const TextureHolder& textures)
: Entity(1)
, mType(type)
, mSprite(textures.get(Table[type].texture), Table[type].textureRect)
, mTargetDirection()
, mFiringAnimation(textures.get(Textures::TeslaBullet))
{
	//Initializes animation for tesla bullet - Dylan Reilly
	mFiringAnimation.setFrameSize(sf::Vector2i(128, 128));
	mFiringAnimation.setScale(0.5f, 0.5f); //Reduce scale of tesla bullet - Jason Lynch
	mFiringAnimation.setNumFrames(12);
	mFiringAnimation.setDuration(sf::seconds(2));

	centerOrigin(mSprite);
	centerOrigin(mFiringAnimation);

	//Add particle system for projectiles - Dylan Reilly
	if (mType == Projectile::Type::GreenHmgBullet || mType == Projectile::Type::RedHmgBullet)
	{
		std::unique_ptr<EmitterNode> smoke(new EmitterNode(Particle::Type::BulletSmoke));
		smoke->setPosition(0.f, getBoundingRect().height / 2.f);
		attachChild(std::move(smoke));
	}

	if (mType == Projectile::Type::GreenTeslaBullet || mType == Projectile::Type::RedTeslaBullet)
	{
		std::unique_ptr<EmitterNode> smoke(new EmitterNode(Particle::Type::TeslaSmoke));
		smoke->setPosition(0.f, getBoundingRect().height / 2.f);
		attachChild(std::move(smoke));
	}
}

void Projectile::guideTowards(sf::Vector2f position)
{
	assert(isGuided());
	mTargetDirection = unitVector(position - getWorldPosition());
}

bool Projectile::isGuided() const
{
	return mType == Missile;
}

void Projectile::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	if (isGuided())
	{
		const float approachRate = 200.f;

		sf::Vector2f newVelocity = unitVector(approachRate * dt.asSeconds() * mTargetDirection + getVelocity());
		newVelocity *= getMaxSpeed();
		float angle = std::atan2(newVelocity.y, newVelocity.x);

		setRotation(toDegree(angle) + 90.f);
		setVelocity(newVelocity);
	}

	//Updates the firing animation if it shoots a tesla bullet - Dylan Reilly
	if (mType == Projectile::Type::GreenTeslaBullet || mType == Projectile::Type::RedTeslaBullet)
	{
		mFiringAnimation.update(dt);
	}

	Entity::updateCurrent(dt, commands);
}

void Projectile::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	//Draws tesla animation if it shoots a tesla bullet, elses draws plain sprite
	if (mType == Projectile::Type::GreenTeslaBullet || mType == Projectile::Type::RedTeslaBullet)
	{
		target.draw(mFiringAnimation, states);
	}
	else
	{
		target.draw(mSprite, states);
	}
}

unsigned int Projectile::getCategory() const
{
	if (mType == RedLmgBullet || mType == RedHmgBullet || mType == RedGatlingBullet || mType == RedTeslaBullet)
		return Category::EnemyProjectile;
	else
		return Category::AlliedProjectile;
}

sf::FloatRect Projectile::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

float Projectile::getMaxSpeed() const
{
	return Table[mType].speed;
}

int Projectile::getDamage() const
{
	return Table[mType].damage;
}
