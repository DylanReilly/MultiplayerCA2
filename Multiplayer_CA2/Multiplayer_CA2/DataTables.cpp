#include "DataTables.hpp"
#include "Tank.hpp"
#include "Projectile.hpp"
#include "Pickup.hpp"
#include "Particle.hpp"
#include "Obstacle.hpp"


// For std::bind() placeholders _1, _2, ...
using namespace std::placeholders;

std::vector<TankData> initializeTankData()
{
	std::vector<TankData> data(Tank::TypeCount);
	//Dylan Reilly - Load in data for tanks
	data[Tank::GreenLmg].hitpoints = 100;
	data[Tank::GreenLmg].speed = 1.5f;
	data[Tank::GreenLmg].fireInterval = sf::seconds(1);
	data[Tank::GreenLmg].texture = Textures::Tanks;
	data[Tank::GreenLmg].textureRect = sf::IntRect(0, 0, 95, 128);
	data[Tank::GreenLmg].bulletType = Projectile::Type::LmgBullet;

	data[Tank::GreenHmg].hitpoints = 100;
	data[Tank::GreenHmg].speed = 1.5f;
	data[Tank::GreenHmg].fireInterval = sf::seconds(2);
	data[Tank::GreenHmg].texture = Textures::Tanks;
	data[Tank::GreenHmg].textureRect = sf::IntRect(285, 0, 95, 128);
	data[Tank::GreenHmg].bulletType = Projectile::Type::HmgBullet;

	data[Tank::GreenGatling].hitpoints = 100;
	data[Tank::GreenGatling].speed = 1.5f;
	data[Tank::GreenGatling].fireInterval = sf::seconds(0.8);
	data[Tank::GreenGatling].texture = Textures::Tanks;
	data[Tank::GreenGatling].textureRect = sf::IntRect(570, 0, 95, 128);
	data[Tank::GreenGatling].bulletType = Projectile::Type::LmgBullet;

	data[Tank::GreenTesla].hitpoints = 100;
	data[Tank::GreenTesla].speed = 1.5f;
	data[Tank::GreenTesla].fireInterval = sf::seconds(3);
	data[Tank::GreenTesla].texture = Textures::Tanks;
	data[Tank::GreenTesla].textureRect = sf::IntRect(855, 0, 95, 128);
	data[Tank::GreenTesla].bulletType = Projectile::Type::TeslaBullet;

	//-----------------------------------------------------------------------------------------
	// RED TANK
	//-----------------------------------------------------------------------------------------

	data[Tank::RedLmg].hitpoints = 100;
	data[Tank::RedLmg].speed = 1.5f;
	data[Tank::RedLmg].fireInterval = sf::seconds(1);
	data[Tank::RedLmg].texture = Textures::Tanks;
	data[Tank::RedLmg].textureRect = sf::IntRect(0, 128, 95, 112);
	data[Tank::RedLmg].bulletType = Projectile::Type::LmgBullet;
			   
	data[Tank::RedHmg].hitpoints = 100;
	data[Tank::RedHmg].speed = 1.5f;
	data[Tank::RedHmg].fireInterval = sf::seconds(2);
	data[Tank::RedHmg].texture = Textures::Tanks;
	data[Tank::RedHmg].textureRect = sf::IntRect(285, 128, 95, 112);
	data[Tank::RedHmg].bulletType = Projectile::Type::HmgBullet;
			   
	data[Tank::RedGatling].hitpoints = 100;
	data[Tank::RedGatling].speed = 1.5f;
	data[Tank::RedGatling].fireInterval = sf::seconds(0.8);
	data[Tank::RedGatling].texture = Textures::Tanks;
	data[Tank::RedGatling].textureRect = sf::IntRect(570, 128, 95, 112);
	data[Tank::RedGatling].bulletType = Projectile::Type::LmgBullet;
			   
	data[Tank::RedTesla].hitpoints = 100;
	data[Tank::RedTesla].speed = 1.5f;
	data[Tank::RedTesla].fireInterval = sf::seconds(3);
	data[Tank::RedTesla].texture = Textures::Tanks;
	data[Tank::RedTesla].textureRect = sf::IntRect(855, 128, 95, 112);
	data[Tank::RedTesla].bulletType = Projectile::Type::TeslaBullet;

	data[Tank::Eagle].hitpoints = 100;
	data[Tank::Eagle].speed = 1.5f;
	data[Tank::Eagle].fireInterval = sf::seconds(1);
	data[Tank::Eagle].texture = Textures::Tanks;
	data[Tank::Eagle].textureRect = sf::IntRect(0, 0, 95, 128);
	data[Tank::Eagle].bulletType = Projectile::Type::HmgBullet;

	return data;
}

std::vector<ProjectileData> initializeProjectileData()
{
	std::vector<ProjectileData> data(Projectile::TypeCount);

	data[Projectile::AlliedBullet].damage = 10;
	data[Projectile::AlliedBullet].speed = 150.f;
	data[Projectile::AlliedBullet].texture = Textures::Entities;
	data[Projectile::AlliedBullet].textureRect = sf::IntRect(175, 64, 3, 14);

	data[Projectile::EnemyBullet].damage = 10;
	data[Projectile::EnemyBullet].speed = 300.f;
	data[Projectile::EnemyBullet].texture = Textures::Entities;
	data[Projectile::EnemyBullet].textureRect = sf::IntRect(178, 64, 3, 14);

	data[Projectile::Missile].damage = 200;
	data[Projectile::Missile].speed = 150.f;
	data[Projectile::Missile].texture = Textures::Entities;
	data[Projectile::Missile].textureRect = sf::IntRect(160, 64, 15, 32);

	//Dylan Reilly - Load in data or bullets
	data[Projectile::LmgBullet].damage = 10;
	data[Projectile::LmgBullet].speed = 150.f;
	data[Projectile::LmgBullet].texture = Textures::Entities;
	data[Projectile::LmgBullet].textureRect = sf::IntRect(175, 64, 3, 14);

	data[Projectile::HmgBullet].damage = 20;
	data[Projectile::HmgBullet].speed = 130.f;
	data[Projectile::HmgBullet].texture = Textures::Entities;
	data[Projectile::HmgBullet].textureRect = sf::IntRect(184, 66, 30, 38);

	data[Projectile::GatlingBullet].damage = 8;
	data[Projectile::GatlingBullet].speed = 150.f;
	data[Projectile::GatlingBullet].texture = Textures::Entities;
	data[Projectile::GatlingBullet].textureRect = sf::IntRect(175, 64, 3, 14);

	data[Projectile::TeslaBullet].damage = 30;
	data[Projectile::TeslaBullet].speed = 110.f;
	data[Projectile::TeslaBullet].texture = Textures::Entities;
	data[Projectile::TeslaBullet].textureRect = sf::IntRect(160, 54, 15, 32);


	return data;
}

std::vector<PickupData> initializePickupData()
{
	std::vector<PickupData> data(Pickup::TypeCount);
	
	data[Pickup::HeavyGun].texture = Textures::HeavyGunPickup; //Set HeavyGun Pickup to assosiated texture - Jason Lynch
	data[Pickup::HeavyGun].textureRect = sf::IntRect(0, 0, 256, 256);
	data[Pickup::HeavyGun].action = std::bind(&Tank::setTankTexture, std::placeholders::_1, 1); //Bind texture changing function to the action associated with pickup - Jason Lynch

	data[Pickup::GatlingGun].texture = Textures::GatlingGunPickup; //Set HeavyGun Pickup to assosiated texture - Jason Lynch
	data[Pickup::GatlingGun].action = std::bind(&Tank::setTankTexture, std::placeholders::_1, 2); //Bind texture changing function to the action associated with pickup - Jason Lynch

	data[Pickup::TeslaGun].texture = Textures::TeslaGunPickup; //Set HeavyGun Pickup to assosiated texture - Jason Lynch
	data[Pickup::TeslaGun].action = std::bind(&Tank::setTankTexture, std::placeholders::_1, 3); //Bind texture changing function to the action associated with pickup - Jason Lynch

	data[Pickup::HealthRefill].texture = Textures::Entities;
	data[Pickup::HealthRefill].textureRect = sf::IntRect(0, 64, 40, 40);
	data[Pickup::HealthRefill].action = [] (Tank& a) { a.repair(25); };
	
	data[Pickup::MissileRefill].texture = Textures::Entities;
	data[Pickup::MissileRefill].textureRect = sf::IntRect(40, 64, 40, 40);
	data[Pickup::MissileRefill].action = std::bind(&Tank::collectMissiles, _1, 3);
	
	data[Pickup::FireSpread].texture = Textures::Entities;
	data[Pickup::FireSpread].textureRect = sf::IntRect(80, 64, 40, 40);
	data[Pickup::FireSpread].action = std::bind(&Tank::increaseSpread, _1);
	
	data[Pickup::FireRate].texture = Textures::Entities;
	data[Pickup::FireRate].textureRect = sf::IntRect(120, 64, 40, 40);
	data[Pickup::FireRate].action = std::bind(&Tank::increaseFireRate, _1);

	return data;
}

std::vector<ParticleData> initializeParticleData()
{
	std::vector<ParticleData> data(Particle::ParticleCount);

	data[Particle::TankDust].color = sf::Color(211, 182, 140);
	data[Particle::TankDust].lifetime = sf::seconds(0.6f);

	data[Particle::TeslaSmoke].color = sf::Color(0, 234, 255);
	data[Particle::TeslaSmoke].lifetime = sf::seconds(0.6f);

	data[Particle::BulletSmoke].color = sf::Color(103, 103, 103);
	data[Particle::BulletSmoke].lifetime = sf::seconds(4.f);

	return data;
}

std::vector<ObstacleData> initializeObstacleData() //Obstacle data. Holds possibly shifting values associated with barrels, walls, etc - Jason Lynch
{
	std::vector<ObstacleData> data(Obstacle::Type::TypeCount); //Get number of different types - Jason Lynch

	data[Obstacle::Type::Wall].damage = 1; //Damage done to player on collision - Jason Lynch
	data[Obstacle::Type::Wall].hitpoints = 25; //Hitpoints of wall - Jason Lynch
	data[Obstacle::Type::Wall].texture = Textures::ID::Wall; //Texture for wall - Jason Lynch

	data[Obstacle::Type::Barrel].damage = 40; //Damage done to player on collision - Jason Lynch
	data[Obstacle::Type::Barrel].hitpoints = 10; //Hitpoints of barrel - Jason Lynch
	data[Obstacle::Type::Barrel].texture = Textures::ID::Barrel; //Texture for barrel - Jason Lynch

	data[Obstacle::Type::DestructableWall].damage = 2; //Damage done to player on collision - Jason Lynch
	data[Obstacle::Type::DestructableWall].hitpoints = 50; //Hitpoints of destructable wall - Jason Lynch
	data[Obstacle::Type::DestructableWall].texture = Textures::ID::DestructableWall; //Texture for destructable wall - Jason Lynch

	return data;
}
