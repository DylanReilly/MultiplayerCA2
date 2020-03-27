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

extern std::string userName;

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
	mHealthDisplay->setColor(sf::Color::Green);
	attachChild(std::move(healthDisplay));

	/*std::unique_ptr<TextNode> userNameDisplay(new TextNode(fonts, ""));
	mUserNameDisplay = userNameDisplay.get();
	mUserNameDisplay->setString(userName);
	mUserNameDisplay->setColor(sf::Color::Yellow);
	attachChild(std::move(userNameDisplay));*/

	/*if (getCategory() == Category::PlayerTank)
	{
		std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
		missileDisplay->setPosition(0, 70);
		mMissileDisplay = missileDisplay.get();
		attachChild(std::move(missileDisplay));
	}*/

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
	if (mType == GreenLmg || mType == GreenLmg2 || mType == GreenLmg3 || mType == GreenHmg || mType == GreenGatling || mType == GreenTesla)
		return Category::AlliedTank;
	else if (mType == RedLmg || mType == RedLmg2 || mType == RedLmg3 || mType == RedHmg || mType == RedGatling || mType == RedTesla)
		return Category::EnemyTank;
	else
		return Category::HostTank;
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
	if (mType == GreenLmg || mType == GreenLmg2 || mType == GreenLmg3 || mType == GreenHmg || mType == GreenGatling || mType == GreenTesla)
	{
		return true;
	}
	else
	{
		return false;
	} 
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

void Tank::setSpread(int val) {
	if (val <= 3)
		mSpreadLevel = val;;
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
		checkProjectileType(commands);

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

void Tank::checkProjectileType(CommandQueue& commands)
{
	Projectile::Type id = getProjectile();

	if (id == Projectile::Type::GreenLmgBullet || id == Projectile::Type::RedLmgBullet || id == Projectile::Type::HostLmgBullet)  //If tank type is LMG - Jason lynch 
	{
		playLocalSound(commands, SoundEffect::TankLMG); //Play LMG sound - Jason lynch 
	}
	else if (id == Projectile::Type::GreenHmgBullet || id == Projectile::Type::RedHmgBullet || id == Projectile::Type::HostHmgBullet) //If tank type is HMG - Jason lynch 
	{
		SoundEffect::ID soundEffect = (randomInt(2) == 0) ? SoundEffect::TankCannon1 : SoundEffect::TankCannon2; //Pick one of two sounds - Jason lynch  
		playLocalSound(commands, soundEffect); //Play that sound - Jason lynch 
	}
	else if (id == Projectile::Type::GreenGatlingBullet || id == Projectile::Type::RedGatlingBullet || id == Projectile::Type::HostGatlingBullet) //If tank type is Gatling - Jason lynch  
	{
		playLocalSound(commands, SoundEffect::TankGatling);//Play gatling sound - Jason lynch 
	}
	else if (id == Projectile::Type::GreenTeslaBullet || id == Projectile::Type::RedTeslaBullet || id == Projectile::Type::HostTeslaBullet)//If tank type is Tesla - Jason lynch 
	{
		playLocalSound(commands, SoundEffect::TeslaBullet);
	}
}

//Returns correct projectile ID based on the tank being used - Dylan Reilly
Projectile::Type Tank::getProjectile() const
{
	if (mType == Type::GreenLmg || mType == Type::GreenLmg2 || mType == Type::GreenLmg3)
	{
		return Projectile::Type::GreenLmgBullet;
	}
	if (mType == Type::GreenHmg)
	{
		return Projectile::Type::GreenHmgBullet;
	}
	if (mType == Type::GreenGatling)
	{
		return Projectile::Type::GreenGatlingBullet;
	}
	if (mType == Type::GreenTesla)
	{
		return Projectile::Type::GreenTeslaBullet;
	}
	if (mType == Type::RedLmg || mType == Type::RedLmg2 || mType == Type::RedLmg3)
	{
		return Projectile::Type::RedLmgBullet;
	}
	if (mType == Type::RedHmg)
	{
		return Projectile::Type::RedHmgBullet;
	}
	if (mType == Type::RedGatling)
	{
		return Projectile::Type::RedGatlingBullet;
	}
	if (mType == Type::RedTesla)
	{
		return Projectile::Type::RedTeslaBullet;
	}
	if (mType == Type::HostLmg)
	{
		return Projectile::Type::HostLmgBullet;
	}
	if (mType == Type::HostHmg)
	{
		return Projectile::Type::HostHmgBullet;
	}
	if (mType == Type::HostGatlingGun)
	{
		return Projectile::Type::HostGatlingBullet;
	}
	if (mType == Type::HostTesla)
	{
		return Projectile::Type::HostTeslaBullet;
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
			createProjectile(node, type, -10.0f, 0.33f, textures);
			createProjectile(node, type, +10.0f, 0.33f, textures);
			break;

		case 3:
			createProjectile(node, type, -20.0f, 0.33f, textures);
			createProjectile(node, type,  0.0f, 0.5f, textures);
			createProjectile(node, type, +20.0f, 0.33f, textures);
			break;
	}
}

void Tank::createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const
{
	Projectile::Type bulletType = getProjectile();

	std::unique_ptr<Projectile> projectile(new Projectile(bulletType, textures));

	//Sets projectile spawn position to origin on the tank - Dylan
	sf::Vector2f offset(15.f * -sin(toRadian(Tank::getRotation()))+xOffset, 15.f * cos(toRadian(Tank::getRotation()))+yOffset);

	//Sets velocity respective to the type of bullet and direction based on the direction the tank is facing - Dylan
	sf::Vector2f velocity(projectile->getMaxSpeed() * 1.5f * -sin(toRadian(Tank::getRotation()))+xOffset, 
		projectile->getMaxSpeed() * 1.5f * cos(toRadian(Tank::getRotation()))+yOffset);

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
	if (isDestroyed()) {
		mHealthDisplay->setString("");
		//mUserNameDisplay->setString("");
	}
	else
		mHealthDisplay->setString(toString(getHitpoints()) + " HP");

	mHealthDisplay->setPosition(0.f, -getBoundingRect().height);
	mHealthDisplay->setRotation(-getRotation());

	/*mUserNameDisplay->setPosition(0.f, -getBoundingRect().height-50.0f);
	mUserNameDisplay->setRotation(-getRotation());*/

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
		case 4096:
			mSprite.setTexture(mTextures.get(Table[static_cast<int>(Tank::HostHmg)].texture), false);
			mSprite.setTextureRect(Table[static_cast<int>(Tank::HostHmg)].textureRect);
			mType = Tank::HostHmg;
			mSpreadLevel = 3;
			break;
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
		case 4096:
			mSprite.setTexture(mTextures.get(Table[static_cast<int>(Tank::HostGatlingGun)].texture), false);
			mSprite.setTextureRect(Table[static_cast<int>(Tank::HostGatlingGun)].textureRect);
			mType = Tank::HostGatlingGun;
			mSpreadLevel = 2;
			break;
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
		case 4096:
			mSprite.setTexture(mTextures.get(Table[static_cast<int>(Tank::HostTesla)].texture), false);
			mSprite.setTextureRect(Table[static_cast<int>(Tank::HostTesla)].textureRect);
			mType = Tank::HostTesla;
			mSpreadLevel = 3;
			break;
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
