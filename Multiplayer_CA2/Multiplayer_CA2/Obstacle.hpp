//Jason Lynch - D00137655
//Dylan Reilly D00194504
#pragma once
#include "Entity.hpp"
#include "CommandQueue.hpp"
#include "TextNode.hpp"
#include "Projectile.hpp"
#include "Animation.hpp"

class Obstacle : public Entity //Built from tank.hpp framework. Handles all environmental objects in ObstacleID - Jason Lynch
{
public:
	Obstacle(Obstacles::ID type, const TextureHolder& textures, const FontHolder& fonts, const Textures::ID deathAnimation, sf::Vector2i frameSize, int numberOfFrmaes, int seconds, sf::Vector2f scale); //Constructor - Jason Lynch
	virtual unsigned int getCategory() const; //Returns collidable. Used for collision - Jason Lynch
	virtual unsigned int getType() const; //Returns Obstacle ID. USed for identifying type - Jason Lynch
	virtual sf::FloatRect getBoundingRect() const; //Returns bounding rect. Used for collision - Jason Lynch
	virtual bool isMarkedForRemoval() const; //Not used 
	virtual unsigned int getDamage() const; //Returns damage object deals - Jason Lynch 

	void playerLocalSound(CommandQueue& command, SoundEffect::ID effect); //Plays sound effect - Jason Lynch

private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	void updateTexts(); //Updates Hp of Object - Jason Lynch

private:
	Obstacles::ID mType;
	sf::Sprite mSprite;
	Animation mExplosion;
	TextNode* mHealthDisplay;

	bool mIsMarkedForRemoval;
	bool mShowExplosion;
	bool mPlayedExplosionSound;

	int mCurrentHitpoints;
};