#ifndef BOOK_DATATABLES_HPP
#define BOOK_DATATABLES_HPP

#include "ResourceIdentifiers.hpp"
#include "Projectile.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <vector>
#include <functional>


class Tank;

struct Direction
{
	Direction(float angle, float distance)
	: angle(angle)
	, distance(distance)
	{
	}

	float angle;
	float distance;
};

struct TankData
{
	int								hitpoints;
	float							speed;
	Textures::ID					texture;
	sf::IntRect						textureRect;
	sf::Time						fireInterval;
	std::vector<Direction>			directions;
	Projectile::Type				bulletType;
};

struct ProjectileData
{
	int								damage;
	float							speed;
	Textures::ID					texture;
	sf::IntRect						textureRect;
};

struct ObstacleData //Struct for obstacles in world - Jason Lynch
{
	int hitpoints; //Hitpoints of obstacle - Jason Lynch
	int damage; //Damage done to player colliding with obstacle - Jason Lynch
	Textures::ID texture; //Texture of Obstacle - Jason Lynch
	//sf::IntRect textureRect;
};

struct PickupData
{
	std::function<void(Tank&)>		action;
	Textures::ID					texture;
	sf::IntRect						textureRect;
};

struct ParticleData
{
	sf::Color						color;
	sf::Time						lifetime;
};


std::vector<TankData>		initializeTankData();
std::vector<ProjectileData>	initializeProjectileData();
std::vector<PickupData>		initializePickupData();
std::vector<ParticleData>	initializeParticleData();
std::vector<ObstacleData>	initializeObstacleData();

#endif // BOOK_DATATABLES_HPP
