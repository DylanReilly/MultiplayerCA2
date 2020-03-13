#include "Tank.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"
#include "Pickup.hpp"
#include "CommandQueue.hpp"
#include "SoundNode.hpp"
#include "NetworkNode.hpp"
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cmath>


using namespace std::placeholders;

namespace
{
	const std::vector<TankData> Table = initializeTankData();
}

Tank::Tank(Category::Type entity, Tanks::ID type, const TextureHolder& textures, const FontHolder& fonts)
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
	// Update textsn
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

			// Emit network game action for enemy explosions
			if (!isAllied())
			{
				sf::Vector2f position = getWorldPosition();

				Command command;
				command.category = Category::Network;
				command.action = derivedAction<NetworkNode>([position] (NetworkNode& node, sf::Time)
				{
					node.notifyGameAction(GameActions::EnemyExplode, position);
				});

				commands.push(command);
			}

			mExplosionBegan = true;
		}
		return;
	}

	// Check if bullets or missiles are fired
	checkProjectilesLaunch(dt, commands);

	// Update enemy movement pattern; apply velocity
	//updateMovementPattern(dt);
	Entity::updateCurrent(dt, commands);
}

unsigned int Tank::getCategory() const
{
	if (isAllied())
		return Category::PlayerTank;
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

//bool Tank::isAllied() const
//{
//	return mType == Eagle;
//}

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

void Tank::setTankTexture(unsigned int val) { //Allows change of tank texture (e.g pickups) - Jason Lynch
	if (val == 1) //Checks for type of tank to change to - Jason Lynch
	{
		int tank = getCategory();
		switch (tank) //2 id player 1, 8 is player 2 - Jason Lynch
		{
		case 2:
			//Assigns new texture to player one tank - Jason Lynch
			mSprite.setTexture(mTextures.get(Table[static_cast<int>(Tanks::GreenLMG1)].texture), false);
			mSprite.setTextureRect(Table[static_cast<int>(Tanks::GreenHMG1)].textureRect);
			mType = Tanks::GreenHMG1;
			break;
		case 8:
			//Assigns new texture to player two tank - Jason Lynch
			mSprite.setTexture(mTextures.get(Table[static_cast<int>(Tanks::RedHMG1)].texture), false);
			mSprite.setTextureRect(Table[static_cast<int>(Tanks::RedHMG1)].textureRect);
			mType = Tanks::RedHMG1;
			break;
		}
	}
	else if (val == 2) //Checks for type of tank to change to - Jason Lynch
	{
		int tank = getCategory();
		switch (tank)
		{
		case 2:
			//Assigns new texture to player one tank - Jason Lynch
			mSprite.setTexture(mTextures.get(Table[static_cast<int>(Tanks::GreenGatling2)].texture), false);
			mSprite.setTextureRect(Table[static_cast<int>(Tanks::GreenGatling2)].textureRect);
			mType = Tanks::GreenGatling2;
			break;
		case 8:
			//Assigns new texture to player two tank - Jason Lynch
			mSprite.setTexture(mTextures.get(Table[static_cast<int>(Tanks::RedGatling2)].texture), false);
			mSprite.setTextureRect(Table[static_cast<int>(Tanks::RedGatling2)].textureRect);
			mType = Tanks::RedGatling2;
			break;
		}
		increaseFireRate();
	}
	else if (val == 3) //Checks for type of tank to change to - Jason Lynch
	{
		int tank = getCategory();
		switch (tank)
		{
		case 2:
			//Assigns new texture to player one tank - Jason Lynch
			mSprite.setTexture(mTextures.get(Table[static_cast<int>(Tanks::GreenTesla2)].texture), false);
			mSprite.setTextureRect(Table[static_cast<int>(Tanks::GreenTesla2)].textureRect);
			mType = Tanks::GreenTesla2;
			break;
		case 8:
			//Assigns new texture to player two tank - Jason Lynch
			mSprite.setTexture(mTextures.get(Table[static_cast<int>(Tanks::RedTesla3)].texture), false);
			mSprite.setTextureRect(Table[static_cast<int>(Tanks::RedTesla3)].textureRect);
			mType = Tanks::RedTesla3;
			break;
		}
	}
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

//void Tank::updateMovementPattern(sf::Time dt)
//{
//	// Enemy airplane: Movement pattern
//	const std::vector<Direction>& directions = Table[mType].directions;
//	if (!directions.empty())
//	{
//		// Moved long enough in current direction: Change direction
//		if (mTravelledDistance > directions[mDirectionIndex].distance)
//		{
//			mDirectionIndex = (mDirectionIndex + 1) % directions.size();
//			mTravelledDistance = 0.f;
//		}
//
//		// Compute velocity from direction
//		float radians = toRadian(directions[mDirectionIndex].angle + 90.f);
//		float vx = getMaxSpeed() * std::cos(radians);
//		float vy = getMaxSpeed() * std::sin(radians);
//
//		setVelocity(vx, vy);
//
//		mTravelledDistance += getMaxSpeed() * dt.asSeconds();
//	}
//}

void Tank::checkPickupDrop(CommandQueue& commands)
{
	// Drop pickup, if enemy airplane, with probability 1/3, if pickup not yet dropped
	// and if not in network mode (where pickups are dropped via packets)
	if (!isAllied() && randomInt(3) == 0 && !mSpawnedPickup && mPickupsEnabled)
		commands.push(mDropPickupCommand);

	mSpawnedPickup = true;
}

void Tank::checkProjectilesLaunch(sf::Time dt, CommandQueue& commands)
{
	// Enemies try to fire all the time
	if (!isAllied())
		fire();

	// Check for automatic gunfire, allow only in intervals
	if (mIsFiring && mFireCountdown <= sf::Time::Zero)
	{
		// Interval expired: We can fire a new bullet
		commands.push(mFireCommand);
		playLocalSound(commands, isAllied() ? SoundEffect::AlliedGunfire : SoundEffect::EnemyGunfire);

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

void Tank::createBullets(SceneNode& node, const TextureHolder& textures) const
{
	//Projectiles::ID::Type type = isAllied() ? Projectiles::AlliedBullet : Projectiles::EnemyBullet;
	Projectiles::ID type = Tank::getProjectile();

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

void Tank::createProjectile(SceneNode& node, Projectiles::ID type, float xOffset, float yOffset, const TextureHolder& textures) const
{
	std::unique_ptr<Projectile> Projectiles(new Projectile(type, textures));

	sf::Vector2f offset(xOffset * mSprite.getGlobalBounds().width, yOffset * mSprite.getGlobalBounds().height);
	sf::Vector2f velocity(0, Projectiles->getMaxSpeed());

	float sign = isAllied() ? -1.f : +1.f;
	Projectiles->setPosition(getWorldPosition() + offset * sign);
	Projectiles->setVelocity(velocity * sign);
	node.attachChild(std::move(Projectiles));
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

//Returns whether tank is allied or enemy (green or red respectively) - Dylan Reilly
bool Tank::isAllied() const
{
	switch (mType)
	{
	case Tanks::ID::GreenLMG1:
	case Tanks::ID::GreenLMG2:
	case Tanks::ID::GreenLMG3:
	case Tanks::ID::GreenHMG1:
	case Tanks::ID::GreenHMG2:
	case Tanks::ID::GreenHMG3:
	case Tanks::ID::GreenGatling1:
	case Tanks::ID::GreenGatling2:
	case Tanks::ID::GreenGatling3:
	case Tanks::ID::GreenTesla1:
	case Tanks::ID::GreenTesla2:
	case Tanks::ID::GreenTesla3:
		return true;
	}
	return false;
}

//Returns correct projectile ID based on the tank being used - Dylan Reilly
Projectiles::ID Tank::getProjectile() const
{
	switch (mType) {
	case Tanks::ID::GreenLMG1:
	case Tanks::ID::GreenLMG2:
	case Tanks::ID::GreenLMG3:
		return Projectiles::ID::GreenLMGBullet;
	case Tanks::ID::RedLMG1:
	case Tanks::ID::RedLMG2:
	case Tanks::ID::RedLMG3:
		return Projectiles::ID::RedLMGBullet;
	case Tanks::ID::GreenHMG1:
	case Tanks::ID::GreenHMG2:
	case Tanks::ID::GreenHMG3:
		return Projectiles::ID::GreenHMGBullet;
	case Tanks::ID::RedHMG1:
	case Tanks::ID::RedHMG2:
	case Tanks::ID::RedHMG3:
		return Projectiles::ID::RedHMGBullet;
	case Tanks::ID::GreenGatling1:
	case Tanks::ID::GreenGatling2:
	case Tanks::ID::GreenGatling3:
		return Projectiles::ID::GreenGatlingBullet;
	case Tanks::ID::RedGatling1:
	case Tanks::ID::RedGatling2:
	case Tanks::ID::RedGatling3:
		return Projectiles::ID::RedGatlingBullet;
	case Tanks::ID::GreenTesla1:
	case Tanks::ID::GreenTesla2:
	case Tanks::ID::GreenTesla3:
		return Projectiles::ID::GreenTeslaBullet;
	case Tanks::ID::RedTesla1:
	case Tanks::ID::RedTesla2:
	case Tanks::ID::RedTesla3:
		return Projectiles::ID::RedTeslaBullet;
	default:
		return Projectiles::ID::None;
	}
}