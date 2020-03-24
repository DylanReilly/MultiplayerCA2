//Jason Lynch - D00137655
//Dylan Reilly D00194504
#include "Obstacle.hpp"
#include "ResourceHolder.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"
#include "Pickup.hpp"
#include "CommandQueue.hpp"
#include "SoundNode.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include "SFML/Graphics/RenderStates.hpp"

#include <cmath>

namespace //Gets data from tables - Jason Lynch
{
	const std::vector<ObstacleData> Table = initializeObstacleData();
}

//Created by me to handle obstacles, since they come in all forms and sizes a lot of variables needed to be passed in - Jason Lynch 
Obstacle::Obstacle(Obstacle::Type type, const TextureHolder& textures, const FontHolder& fonts, const Textures::ID deathAnimation, sf::Vector2i frameSize, int numberOfFrames, int seconds, sf::Vector2f scale)
	: Entity(Table[type].hitpoints)
	, mType(type)
	, mSprite(textures.get(Table[static_cast<int>(type)].texture)/*, Table[static_cast<int>(type)].textureRect*/)
	, mExplosion(textures.get(deathAnimation))
	, mShowExplosion(true)
	, mPlayedExplosionSound(false)
	, mIsMarkedForRemoval(false)
	, mHealthDisplay(nullptr)
	, mCurrentHitpoints(Table[static_cast<int>(type)].hitpoints)
{
	//Set up animation with custom paramaters 
	mExplosion.setFrameSize(frameSize);
	mExplosion.setNumFrames(numberOfFrames);
	mExplosion.setScale(scale);
	mExplosion.setDuration(sf::seconds(seconds));

	centerOrigin(mSprite);
	centerOrigin(mExplosion);

	//std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, "", sf::Color::Black)); //Health bar 
	//mHealthDisplay = healthDisplay.get();
	//mHealthDisplay->setScale(1.5f, 1.5f);
	//attachChild(std::move(healthDisplay));

	//updateTexts();
}

unsigned int Obstacle::getCategory() const //Returns collidable as category - Jason Lynch
{
	return static_cast<int>(Category::Collidable);
}

unsigned int Obstacle::getType() const { //Returns the type e.g barrel - Jason Lynch 
	return static_cast<int>(mType);
}

sf::FloatRect Obstacle::getBoundingRect() const //Returns bounding rectangle for collision - Jason Lynch 
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

bool Obstacle::isMarkedForRemoval() const
{
	return isDestroyed() && (mExplosion.isFinished() || !mShowExplosion);
}

unsigned int Obstacle::getDamage() const //Returns damage obstacle dooes on collision - Jason Lynch
{
	return Table[static_cast<int>(mType)].damage;
}

void Obstacle::playerLocalSound(CommandQueue& commands, SoundEffect::ID effect)
{
	sf::Vector2f worldPosition = getWorldPosition();

	Command command;
	command.category = static_cast<int>(Category::SoundEffect);
	command.action = derivedAction<SoundNode>(
		[effect, worldPosition](SoundNode& node, sf::Time)
		{
			node.playSound(effect, worldPosition);
		});
	commands.push(command);
}

void Obstacle::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (isDestroyed() && mShowExplosion)
		target.draw(mExplosion, states);
	else
		target.draw(mSprite, states);
}

void Obstacle::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// Entity has been destroyed: Possibly drop pickup, mark for removal
	if (isDestroyed())
	{

		mExplosion.update(dt);
		//mIsMarkedForRemoval = true;
		//Play explosion sound
		if (!mPlayedExplosionSound)
		{
			SoundEffect::ID soundEffect = (randomInt(2) == 0) ? SoundEffect::Explosion1 : SoundEffect::Explosion2;
			playerLocalSound(commands, soundEffect);

			mPlayedExplosionSound = true;
		}
		return;
	}

	Entity::updateCurrent(dt, commands);

	// Update texts
	//updateTexts();
}

void Obstacle::updateTexts() //Updates health display - Jason Lynch 
{
	mHealthDisplay->setString(toString(getHitpoints()) + " HP");
	mHealthDisplay->setPosition(0.f, 0.f);
	mHealthDisplay->setRotation(-getRotation());
}
