#ifndef BOOK_DATATABLES_HPP
#define BOOK_DATATABLES_HPP

#include "ResourceIdentifiers.hpp"

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
	bool							hasRollAnimation;
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
	int								hitpoints; //Hitpoints of obstacle - Jason Lynch
	int								damage; //Damage done to player colliding with obstacle - Jason Lynch
	Textures::ID					texture; //Texture of Obstacle - Jason Lynch
};

struct PickupData
{
	std::function<void(Tank&)>	action;
	Textures::ID					texture;
	sf::IntRect						textureRect;
};

struct ParticleData
{
	sf::Color						color;
	sf::Time						lifetime;
};

std::vector<ObstacleData> initializeObstacleData() //Obstacle data. Holds possibly shifting values associated with barrels, walls, etc - Jason Lynch
{
	std::vector<ObstacleData> data(static_cast<int>(Obstacles::ID::TypeCount)); //Get number of different types - Jason Lynch

	data[static_cast<int>(Obstacles::ID::Wall)].damage = 1; //Damage done to player on collision - Jason Lynch
	data[static_cast<int>(Obstacles::ID::Wall)].hitpoints = 25; //Hitpoints of wall - Jason Lynch
	data[static_cast<int>(Obstacles::ID::Wall)].texture = Textures::ID::Wall; //Texture for wall - Jason Lynch

	data[static_cast<int>(Obstacles::ID::Barrel)].damage = 40; //Damage done to player on collision - Jason Lynch
	data[static_cast<int>(Obstacles::ID::Barrel)].hitpoints = 10; //Hitpoints of barrel - Jason Lynch
	data[static_cast<int>(Obstacles::ID::Barrel)].texture = Textures::ID::Barrel; //Texture for barrel - Jason Lynch

	data[static_cast<int>(Obstacles::ID::DestructableWall)].damage = 2; //Damage done to player on collision - Jason Lynch
	data[static_cast<int>(Obstacles::ID::DestructableWall)].hitpoints = 50; //Hitpoints of destructable wall - Jason Lynch
	data[static_cast<int>(Obstacles::ID::DestructableWall)].texture = Textures::ID::DestructableWall; //Texture for destructable wall - Jason Lynch

	data[static_cast<int>(Obstacles::ID::Nuke)].damage = 1; //Damage done to player on collision - Jason Lynch
	data[static_cast<int>(Obstacles::ID::Nuke)].hitpoints = 150; //Hitpoints of destructable wall - Jason Lynch
	data[static_cast<int>(Obstacles::ID::Nuke)].texture = Textures::ID::Nuke; //Texture for destructable wall - Jason Lynch

	return data;
}

std::vector<TankData>	initializeTankData();
std::vector<ProjectileData>	initializeProjectileData();
std::vector<PickupData>		initializePickupData();
std::vector<ParticleData>	initializeParticleData();

#endif // BOOK_DATATABLES_HPP
