#include "Tank.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"
#include "Pickup.hpp"
#include "CommandQueue.hpp"
#include "SoundNode.hpp"
#include "NetworkNode.hpp"
#include "ResourceHolder.hpp"
#include "EmitterNode.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cmath>


using namespace std::placeholders;

namespace
{
	const std::vector<TankData> Table = initializeTankData();
}

Tank::Tank(Type type, const TextureHolder& textures, const FontHolder& fonts)
: Entity(Table[type].hitpoints)
, mType(type)
, mSprite(textures.get(Table[type].texture), Table[type].textureRect)
, mTextures(textures) //Needed to change tank texture on powerup pickup - Jason Lynch
, mExplosion(textures.get(Textures::Explosion))
, mFireCommand()
, mMissileCommand()
, mFireCountdown(sf::Time::Zero)
, mIsFiring(false)
, mIsLaunchingMissile(false)
, mShowExplosion(true)
, mExplosionBegan(false)
, mSpawnedPickup(false)
, mPickupsEnabled(true)
, mFireRateLevel(1)
, mSpreadLevel(1)
, mMissileAmmo(2)
, mDropPickupCommand()
, mTravelledDistance(0.f)
, mDirectionIndex(0)
, mMissileDisplay(nullptr)
, mIdentifier(0)
{
	mExplosion.setFrameSize(sf::Vector2i(256, 256));
	mExplosion.setNumFrames(16);
	mExplosion.setDuration(sf::seconds(1));

	centerOrigin(mSprite);
	centerOrigin(mExplosion);

	mFireCommand.category = Category::SceneAirLayer;
	mFireCommand.action   = [this, &textures] (SceneNode& node, sf::Time)
	{
		createBullets(node, textures);
	};

	mMissileCommand.category = Category::SceneAirLayer;
	mMissileCommand.action   = [this, &textures] (SceneNode& node, sf::Time)
	{
		createProjectile(node, Projectile::Missile, 0.f, 0.5f, textures);
	};

	mDropPickupCommand.category = Category::SceneAirLayer;
	mDropPickupCommand.action   = [this, &textures] (SceneNode& node, sf::Time)
	{
		createPickup(node, textures);
	};

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	mHealthDisplay = healthDisplay.get();
	attachChild(std::move(healthDisplay));

	if (getCategory() == Category::PlayerTank)
	{
		std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
		missileDisplay->setPosition(0, 70);
		mMissileDisplay = missileDisplay.get();
		attachChild(std::move(missileDisplay));
	}

	// Dust trails for tank treads - Dylan
	//Adds trail to the front left & right of the sprite
	std::unique_ptr<EmitterNode> smokeLeft(new EmitterNode(Particle::Type::TankDust));
	smokeLeft->setPosition(-40.f, getBoundingRect().height / 2.f);
	attachChild(std::move(smokeLeft));

	std::unique_ptr<EmitterNode> smokeRight(new EmitterNode(Particle::Type::TankDust));
	smokeRight->setPosition(40.f, getBoundingRect().height / 2.f);
	attachChild(std::move(smokeRight));

	updateTexts();
}

int Tank::getMissileAmmo() const
{
	return mMissileAmmo;
}

void Tank::setMissileAmmo(int ammo)
{
	mMissileAmmo = ammo;
}

void Tank::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (isDestroyed() && mShowExplosion)
		target.draw(mExplosion, states);
	else
		target.draw(mSprite, states);
}

void Tank::disablePickups()
{
	mPickupsEnabled = false;
}

void Tank::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// Update texts and roll animation
	updateTexts();

	// Entity has been destroyed: Possibly drop pickup, mark for removal
	if (isDestroyed())
	{
		checkPickupDrop(commands);
		mExplosion.update(dt);

		// Play explosion sound only once
		if (!mExplosionBegan)
		{
			// Play sound effect
			SoundEffect::ID soundEffect = (randomInt(2) == 0) ? SoundEffect::Explosion1 : SoundEffect::Explosion2;
			playLocalSound(commands, soundEffect);
		}
		return;
	}

	// Check if bullets or missiles are fired
	checkProjectileLaunch(dt, commands);

	Entity::updateCurrent(dt, commands);
}

unsigned int Tank::getCategory() const
{
	if (isAllied())
		return Category::AlliedTank;
	else
		return Category::EnemyTank;
}

sf::FloatRect Tank::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

bool Tank::isMarkedForRemoval() const
{
	return isDestroyed() && (mExplosion.isFinished() || !mShowExplosion);
}

void Tank::remove()
{
	Entity::remove();
	mShowExplosion = false;
}

//Dylan Reilly - Returns is allied for any green tank
bool Tank::isAllied() const
{
	return mType == GreenLmg || mType == GreenHmg || mType == GreenGatling || mType == GreenTesla;
}

float Tank::getMaxSpeed() const
{
	return Table[mType].speed;
}

void Tank::increaseFireRate()
{
	if (mFireRateLevel < 10)
		++mFireRateLevel;
}

void Tank::increaseSpread()
{
	if (mSpreadLevel < 3)
		++mSpreadLevel;
}

void Tank::collectMissiles(unsigned int count)
{
	mMissileAmmo += count;
}

void Tank::fire()
{
	// Only ships with fire interval != 0 are able to fire
	if (Table[mType].fireInterval != sf::Time::Zero)
		mIsFiring = true;
}

void Tank::launchMissile()
{
	if (mMissileAmmo > 0)
	{
		mIsLaunchingMissile = true;
		--mMissileAmmo;
	}
}

void Tank::playLocalSound(CommandQueue& commands, SoundEffect::ID effect)
{
	sf::Vector2f worldPosition = getWorldPosition();

	Command command;
	command.category = Category::SoundEffect;
	command.action = derivedAction<SoundNode>(
		[effect, worldPosition] (SoundNode& node, sf::Time)
		{
			node.playSound(effect, worldPosition); 
		});

	commands.push(command);
}

int	Tank::getIdentifier()
{
	return mIdentifier;
}

void Tank::setIdentifier(int identifier)
{
	mIdentifier = identifier;
}

void Tank::checkPickupDrop(CommandQueue& commands)
{
	// Drop pickup, if enemy airplane, with probability 1/3, if pickup not yet dropped
	// and if not in network mode (where pickups are dropped via packets)
	if (!isAllied() && randomInt(3) == 0 && !mSpawnedPickup && mPickupsEnabled)
		commands.push(mDropPickupCommand);

	mSpawnedPickup = true;
}

void Tank::checkProjectileLaunch(sf::Time dt, CommandQueue& commands)
{
	// Check for automatic gunfire, allow only in intervals
	if (mIsFiring && mFireCountdown <= sf::Time::Zero)
	{
		// Interval expired: We can fire a new bullet
		commands.push(mFireCommand);
		playLocalSound(commands,SoundEffect::TankLMG);

		mFireCountdown += Table[mType].fireInterval / (mFireRateLevel + 1.f);
		mIsFiring = false;
	}
	else if (mFireCountdown > sf::Time::Zero)
	{
		// Interval not expired: Decrease it further
		mFireCountdown -= dt;
		mIsFiring = false;
	}

	// Check for missile launch
	if (mIsLaunchingMissile)
	{
		commands.push(mMissileCommand);
		playLocalSound(commands, SoundEffect::LaunchMissile);

		mIsLaunchingMissile = false;
	}
}

//Returns correct projectile ID based on the tank being used - Dylan Reilly
Projectile::Type Tank::getProjectile() const
{
	switch (mType) {
	case Tank::GreenLmg:
	case Tank::RedLmg:
	case Tank::GreenGatling:
	case Tank::RedGatling:
		return Projectile::Type::LmgBullet;
	case Tank::GreenHmg:
	case Tank::RedHmg:
		return Projectile::Type::HmgBullet;
	case Tank::GreenTesla:
	case Tank::RedTesla:
		return Projectile::Type::TeslaBullet;
	default:
		return Projectile::Type::LmgBullet;
	}
}

void Tank::createBullets(SceneNode& node, const TextureHolder& textures) const
{
	Projectile::Type type = Tank::getProjectile();

	switch (mSpreadLevel)
	{
		case 1:
			createProjectile(node, type, 0.0f, 0.5f, textures);
			break;

		case 2:
			createProjectile(node, type, -0.33f, 0.33f, textures);
			createProjectile(node, type, +0.33f, 0.33f, textures);
			break;

		case 3:
			createProjectile(node, type, -0.5f, 0.33f, textures);
			createProjectile(node, type,  0.0f, 0.5f, textures);
			createProjectile(node, type, +0.5f, 0.33f, textures);
			break;
	}
}

void Tank::createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const
{
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));

	//Sets projectile spawn position to origin on the tank - Dylan
	sf::Vector2f offset(75.f * -sin(toRadian(Tank::getRotation())), 75.f * cos(toRadian(Tank::getRotation())));

	//Sets velocity respective to the type of bullet and direction based on the direction the tank is facing - Dylan
	sf::Vector2f velocity(projectile->getMaxSpeed() * 1.5f * -sin(toRadian(Tank::getRotation())), 
		projectile->getMaxSpeed() * 1.5f * cos(toRadian(Tank::getRotation())));

	//float sign = isAllied() ? -1.f : +1.f;
	projectile->setPosition(getWorldPosition() + offset);
	projectile->setVelocity(velocity);
	projectile->setRotation(Tank::getRotation() + 180.f);
	node.attachChild(std::move(projectile));
}

void Tank::createPickup(SceneNode& node, const TextureHolder& textures) const
{
	auto type = static_cast<Pickup::Type>(randomInt(Pickup::TypeCount));

	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(getWorldPosition());
	pickup->setVelocity(0.f, 1.f);
	node.attachChild(std::move(pickup));
}

void Tank::updateTexts()
{
	// Display hitpoints
	if (isDestroyed())
		mHealthDisplay->setString("");
	else
		mHealthDisplay->setString(toString(getHitpoints()) + " HP");
	mHealthDisplay->setPosition(0.f, 50.f);
	mHealthDisplay->setRotation(-getRotation());

	// Display missiles, if available
	if (mMissileDisplay)
	{
		if (mMissileAmmo == 0 || isDestroyed())
			mMissileDisplay->setString("");
		else
			mMissileDisplay->setString("M: " + toString(mMissileAmmo));
	}
}

void Tank::setTankTexture(unsigned int val) { //Allows change of tank texture (e.g pickups) - Jason Lynch
	if (val == 1) //Checks for type of tank to change to - Jason Lynch
	{
		int tank = getCategory();
		switch (tank) //2 id player 1, 8 is player 2 - Jason Lynch
		{
		case 4:
			//Assigns new texture to player one tank - Jason Lynch
			mSprite.setTexture(mTextures.get(Table[static_cast<int>(Tank::GreenHmg)].texture), false);
			mSprite.setTextureRect(Table[static_cast<int>(Tank::GreenHmg)].textureRect);
			mType = Tank::GreenHmg;
			break;
		case 8:
			//Assigns new texture to player two tank - Jason Lynch
			mSprite.setTexture(mTextures.get(Table[static_cast<int>(Tank::RedHmg)].texture), false);
			mSprite.setTextureRect(Table[static_cast<int>(Tank::RedHmg)].textureRect);
			mType = Tank::RedHmg;
			break;
		}
	}
	else if (val == 2) //Checks for type of tank to change to - Jason Lynch
	{
		int tank = getCategory();
		switch (tank)
		{
		case 4:
			//Assigns new texture to player one tank - Jason Lynch
			mSprite.setTexture(mTextures.get(Table[static_cast<int>(Tank::GreenGatling)].texture), false);
			mSprite.setTextureRect(Table[static_cast<int>(Tank::GreenGatling)].textureRect);
			mType = Tank::GreenGatling;
			break;
		case 8:
			//Assigns new texture to player two tank - Jason Lynch
			mSprite.setTexture(mTextures.get(Table[static_cast<int>(Tank::RedGatling)].texture), false);
			mSprite.setTextureRect(Table[static_cast<int>(Tank::RedGatling)].textureRect);
			mType = Tank::RedGatling;
			break;
		}
		increaseFireRate();
	}
	else if (val == 3) //Checks for type of tank to change to - Jason Lynch
	{
		int tank = getCategory();
		switch (tank)
		{
		case 4:
			//Assigns new texture to player one tank - Jason Lynch
			mSprite.setTexture(mTextures.get(Table[static_cast<int>(Tank::GreenTesla)].texture), false);
			mSprite.setTextureRect(Table[static_cast<int>(Tank::GreenTesla)].textureRect);
			mType = Tank::GreenTesla;
			break;
		case 8:
			//Assigns new texture to player two tank - Jason Lynch
			mSprite.setTexture(mTextures.get(Table[static_cast<int>(Tank::RedTesla)].texture), false);
			mSprite.setTextureRect(Table[static_cast<int>(Tank::RedTesla)].textureRect);
			mType = Tank::RedTesla;
			break;
		}
	}
}
