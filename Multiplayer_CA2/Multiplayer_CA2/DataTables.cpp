#include "DataTables.hpp"
#include "Tank.hpp"
#include "Projectile.hpp"
#include "Pickup.hpp"
#include "Particle.hpp"


// For std::bind() placeholders _1, _2, ...
using namespace std::placeholders;

std::vector<TankData> initializeTankData()
{
	//Added new data table entries for the different tanks, giving each tank different attributes - Dylan Reilly
	std::vector<TankData> data(static_cast<int>(Tanks::TypeCount));
	//Starter Tank - Light Machine Gun
	data[static_cast<int>(Tanks::GreenLMG1)].hitpoints = 50;
	data[static_cast<int>(Tanks::GreenLMG1)].speed = 200.f;
	data[static_cast<int>(Tanks::GreenLMG1)].fireInterval = sf::seconds(1);
	data[static_cast<int>(Tanks::GreenLMG1)].textureRect = sf::IntRect(0, 0, 95, 128);
	data[static_cast<int>(Tanks::GreenLMG1)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::GreenLMG1)].bulletType = Projectiles::ID::GreenLMGBullet;

	//Starter Tank - Double Light Machine Gun
	data[static_cast<int>(Tanks::GreenLMG2)].hitpoints = 50;
	data[static_cast<int>(Tanks::GreenLMG2)].speed = 200.f;
	data[static_cast<int>(Tanks::GreenLMG2)].fireInterval = sf::seconds(0.6f);
	data[static_cast<int>(Tanks::GreenLMG2)].textureRect = sf::IntRect(95, 0, 95, 128);
	data[static_cast<int>(Tanks::GreenLMG2)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::GreenLMG2)].bulletType = Projectiles::ID::GreenLMGBullet;

	//Starter Tank - Faster movement
	data[static_cast<int>(Tanks::GreenLMG3)].hitpoints = 50;
	data[static_cast<int>(Tanks::GreenLMG3)].speed = 250.f;
	data[static_cast<int>(Tanks::GreenLMG3)].fireInterval = sf::seconds(1);
	data[static_cast<int>(Tanks::GreenLMG3)].textureRect = sf::IntRect(190, 0, 95, 128);
	data[static_cast<int>(Tanks::GreenLMG3)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::GreenLMG3)].bulletType = Projectiles::ID::GreenLMGBullet;

	//Heavy Tank - Heavy Machine Gun
	data[static_cast<int>(Tanks::GreenHMG1)].hitpoints = 100;
	data[static_cast<int>(Tanks::GreenHMG1)].speed = 200.f;
	data[static_cast<int>(Tanks::GreenHMG1)].fireInterval = sf::seconds(1);
	data[static_cast<int>(Tanks::GreenHMG1)].textureRect = sf::IntRect(285, 0, 95, 128);
	data[static_cast<int>(Tanks::GreenHMG1)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::GreenHMG1)].bulletType = Projectiles::ID::GreenHMGBullet;

	//Heavy Tank - Double Heavy Machine Gun
	data[static_cast<int>(Tanks::GreenHMG2)].hitpoints = 100;
	data[static_cast<int>(Tanks::GreenHMG2)].speed = 200.f;
	data[static_cast<int>(Tanks::GreenHMG2)].fireInterval = sf::seconds(1);
	data[static_cast<int>(Tanks::GreenHMG2)].textureRect = sf::IntRect(380, 0, 95, 128);
	data[static_cast<int>(Tanks::GreenHMG2)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::GreenHMG2)].bulletType = Projectiles::ID::GreenHMGBullet;

	//Heavy Tank - Faster Movement
	data[static_cast<int>(Tanks::GreenHMG3)].hitpoints = 100;
	data[static_cast<int>(Tanks::GreenHMG3)].speed = 250.f;
	data[static_cast<int>(Tanks::GreenHMG3)].fireInterval = sf::seconds(1);
	data[static_cast<int>(Tanks::GreenHMG3)].textureRect = sf::IntRect(475, 0, 95, 128);
	data[static_cast<int>(Tanks::GreenHMG3)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::GreenHMG3)].bulletType = Projectiles::ID::GreenHMGBullet;

	//Gatling Tank - Gatling Gun
	data[static_cast<int>(Tanks::GreenGatling1)].hitpoints = 150;
	data[static_cast<int>(Tanks::GreenGatling1)].speed = 200.f;
	data[static_cast<int>(Tanks::GreenGatling1)].fireInterval = sf::seconds(0.6f);
	data[static_cast<int>(Tanks::GreenGatling1)].textureRect = sf::IntRect(570, 0, 95, 128);
	data[static_cast<int>(Tanks::GreenGatling1)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::GreenGatling1)].bulletType = Projectiles::ID::GreenGatlingBullet;

	//Gatling Tank - Double Gatling Gun
	data[static_cast<int>(Tanks::GreenGatling2)].hitpoints = 150;
	data[static_cast<int>(Tanks::GreenGatling2)].speed = 200.f;
	data[static_cast<int>(Tanks::GreenGatling2)].fireInterval = sf::seconds(0.4f);
	data[static_cast<int>(Tanks::GreenGatling2)].textureRect = sf::IntRect(665, 0, 95, 128);
	data[static_cast<int>(Tanks::GreenGatling2)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::GreenGatling2)].bulletType = Projectiles::ID::GreenGatlingBullet;

	//Gatling Tank - Faster Movement
	data[static_cast<int>(Tanks::GreenGatling3)].hitpoints = 150;
	data[static_cast<int>(Tanks::GreenGatling3)].speed = 300.f;
	data[static_cast<int>(Tanks::GreenGatling3)].fireInterval = sf::seconds(0.6f);
	data[static_cast<int>(Tanks::GreenGatling3)].textureRect = sf::IntRect(760, 0, 95, 128);
	data[static_cast<int>(Tanks::GreenGatling3)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::GreenGatling3)].bulletType = Projectiles::ID::GreenGatlingBullet;

	//Tesla Tank - Lightening Gun
	data[static_cast<int>(Tanks::GreenTesla1)].hitpoints = 200;
	data[static_cast<int>(Tanks::GreenTesla1)].speed = 200.f;
	data[static_cast<int>(Tanks::GreenTesla1)].fireInterval = sf::seconds(1);
	data[static_cast<int>(Tanks::GreenTesla1)].textureRect = sf::IntRect(855, 0, 95, 128);
	data[static_cast<int>(Tanks::GreenTesla1)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::GreenTesla1)].bulletType = Projectiles::ID::GreenTeslaBullet;

	//Tesla Tank - Double Lightening Gun
	data[static_cast<int>(Tanks::GreenTesla2)].hitpoints = 200;
	data[static_cast<int>(Tanks::GreenTesla2)].speed = 200.f;
	data[static_cast<int>(Tanks::GreenTesla2)].fireInterval = sf::seconds(1);
	data[static_cast<int>(Tanks::GreenTesla2)].textureRect = sf::IntRect(950, 0, 95, 128);
	data[static_cast<int>(Tanks::GreenTesla2)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::GreenTesla2)].bulletType = Projectiles::ID::GreenTeslaBullet;

	//Tesla Tank - Faster Movement
	data[static_cast<int>(Tanks::GreenTesla3)].hitpoints = 200;
	data[static_cast<int>(Tanks::GreenTesla3)].speed = 250.f;
	data[static_cast<int>(Tanks::GreenTesla3)].fireInterval = sf::seconds(1);
	data[static_cast<int>(Tanks::GreenTesla3)].textureRect = sf::IntRect(1045, 0, 95, 128);
	data[static_cast<int>(Tanks::GreenTesla3)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::GreenTesla3)].bulletType = Projectiles::ID::GreenTeslaBullet;

	//-----------------------------------------------------------------------------------------
	// RED TANK
	//-----------------------------------------------------------------------------------------

	data[static_cast<int>(Tanks::RedLMG1)].hitpoints = 50;
	data[static_cast<int>(Tanks::RedLMG1)].speed = 200.f;
	data[static_cast<int>(Tanks::RedLMG1)].fireInterval = sf::seconds(1);
	data[static_cast<int>(Tanks::RedLMG1)].textureRect = sf::IntRect(0, 128, 95, 112);
	data[static_cast<int>(Tanks::RedLMG1)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::RedLMG1)].bulletType = Projectiles::ID::RedLMGBullet;

	//Starter Tank - Double Light Redne Gun
	data[static_cast<int>(Tanks::RedLMG2)].hitpoints = 50;
	data[static_cast<int>(Tanks::RedLMG2)].speed = 200.f;
	data[static_cast<int>(Tanks::RedLMG2)].fireInterval = sf::seconds(0.6f);
	data[static_cast<int>(Tanks::RedLMG2)].textureRect = sf::IntRect(95, 128, 95, 112);
	data[static_cast<int>(Tanks::RedLMG2)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::RedLMG2)].bulletType = Projectiles::ID::RedLMGBullet;

	//Starter Tank - Faster movemeRed
	data[static_cast<int>(Tanks::RedLMG3)].hitpoints = 50;
	data[static_cast<int>(Tanks::RedLMG3)].speed = 250.f;
	data[static_cast<int>(Tanks::RedLMG3)].fireInterval = sf::seconds(1);
	data[static_cast<int>(Tanks::RedLMG3)].textureRect = sf::IntRect(190, 128, 95, 112);
	data[static_cast<int>(Tanks::RedLMG3)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::RedLMG3)].bulletType = Projectiles::ID::RedLMGBullet;

	//Heavy Tank - Heavy Machine GRed
	data[static_cast<int>(Tanks::RedHMG1)].hitpoints = 100;
	data[static_cast<int>(Tanks::RedHMG1)].speed = 200.f;
	data[static_cast<int>(Tanks::RedHMG1)].fireInterval = sf::seconds(1);
	data[static_cast<int>(Tanks::RedHMG1)].textureRect = sf::IntRect(285, 128, 95, 112);
	data[static_cast<int>(Tanks::RedHMG1)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::RedHMG1)].bulletType = Projectiles::ID::RedHMGBullet;

	//Heavy Tank - Double Heavy MaRed Gun
	data[static_cast<int>(Tanks::RedHMG2)].hitpoints = 100;
	data[static_cast<int>(Tanks::RedHMG2)].speed = 200.f;
	data[static_cast<int>(Tanks::RedHMG2)].fireInterval = sf::seconds(1);
	data[static_cast<int>(Tanks::RedHMG2)].textureRect = sf::IntRect(380, 128, 95, 112);
	data[static_cast<int>(Tanks::RedHMG2)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::RedHMG2)].bulletType = Projectiles::ID::RedHMGBullet;

	//Heavy Tank - Faster MovementRed
	data[static_cast<int>(Tanks::RedHMG3)].hitpoints = 100;
	data[static_cast<int>(Tanks::RedHMG3)].speed = 250.f;
	data[static_cast<int>(Tanks::RedHMG3)].fireInterval = sf::seconds(1);
	data[static_cast<int>(Tanks::RedHMG3)].textureRect = sf::IntRect(475, 128, 95, 112);
	data[static_cast<int>(Tanks::RedHMG3)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::RedHMG3)].bulletType = Projectiles::ID::RedHMGBullet;

	//Gatling Tank - Gatling Gun  Red
	data[static_cast<int>(Tanks::RedGatling1)].hitpoints = 150;
	data[static_cast<int>(Tanks::RedGatling1)].speed = 200.f;
	data[static_cast<int>(Tanks::RedGatling1)].fireInterval = sf::seconds(0.6f);
	data[static_cast<int>(Tanks::RedGatling1)].textureRect = sf::IntRect(570, 128, 95, 112);
	data[static_cast<int>(Tanks::RedGatling1)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::RedGatling1)].bulletType = Projectiles::ID::RedGatlingBullet;

	//Gatling Tank - Double GatlinRed
	data[static_cast<int>(Tanks::RedGatling2)].hitpoints = 150;
	data[static_cast<int>(Tanks::RedGatling2)].speed = 200.f;
	data[static_cast<int>(Tanks::RedGatling2)].fireInterval = sf::seconds(0.6f);
	data[static_cast<int>(Tanks::RedGatling2)].textureRect = sf::IntRect(665, 128, 95, 112);
	data[static_cast<int>(Tanks::RedGatling2)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::RedGatling2)].bulletType = Projectiles::ID::RedGatlingBullet;

	//Gatling Tank - Faster MovemeRed
	data[static_cast<int>(Tanks::RedGatling3)].hitpoints = 150;
	data[static_cast<int>(Tanks::RedGatling3)].speed = 300.f;
	data[static_cast<int>(Tanks::RedGatling3)].fireInterval = sf::seconds(0.6f);
	data[static_cast<int>(Tanks::RedGatling3)].textureRect = sf::IntRect(760, 128, 95, 112);
	data[static_cast<int>(Tanks::RedGatling3)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::RedGatling3)].bulletType = Projectiles::ID::RedGatlingBullet;

	//Tesla Tank - Lightening Gun Red
	data[static_cast<int>(Tanks::RedTesla1)].hitpoints = 200;
	data[static_cast<int>(Tanks::RedTesla1)].speed = 200.f;
	data[static_cast<int>(Tanks::RedTesla1)].fireInterval = sf::seconds(1);
	data[static_cast<int>(Tanks::RedTesla1)].textureRect = sf::IntRect(855, 128, 95, 112);
	data[static_cast<int>(Tanks::RedTesla1)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::RedTesla1)].bulletType = Projectiles::ID::RedTeslaBullet;

	//Tesla Tank - Double LighteniRedn
	data[static_cast<int>(Tanks::RedTesla2)].hitpoints = 200;
	data[static_cast<int>(Tanks::RedTesla2)].speed = 200.f;
	data[static_cast<int>(Tanks::RedTesla2)].fireInterval = sf::seconds(1);
	data[static_cast<int>(Tanks::RedTesla2)].textureRect = sf::IntRect(950, 128, 95, 112);
	data[static_cast<int>(Tanks::RedTesla2)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::RedTesla2)].bulletType = Projectiles::ID::RedTeslaBullet;

	//Tesla Tank - Faster MovementRed
	data[static_cast<int>(Tanks::RedTesla3)].hitpoints = 200;
	data[static_cast<int>(Tanks::RedTesla3)].speed = 250.f;
	data[static_cast<int>(Tanks::RedTesla3)].fireInterval = sf::seconds(1);
	data[static_cast<int>(Tanks::RedTesla3)].textureRect = sf::IntRect(1045, 128, 95, 112);
	data[static_cast<int>(Tanks::RedTesla3)].texture = Textures::ID::Tanks;
	data[static_cast<int>(Tanks::RedTesla3)].bulletType = Projectiles::ID::RedTeslaBullet;


	return data;
}

std::vector<ProjectileData> initializeProjectileData()
{
	std::vector<ProjectileData> data(static_cast<int>(Projectiles::ID::TypeCount));

	data[static_cast<int>(Projectiles::ID::AlliedBullet)].damage = 10;
	data[static_cast<int>(Projectiles::ID::AlliedBullet)].speed = 300.f;
	data[static_cast<int>(Projectiles::ID::AlliedBullet)].texture = Textures::Entities;
	data[static_cast<int>(Projectiles::ID::AlliedBullet)].textureRect = sf::IntRect(175, 64, 3, 14);

	data[static_cast<int>(Projectiles::ID::AlliedBullet)].damage = 10;
	data[static_cast<int>(Projectiles::ID::AlliedBullet)].speed = 300.f;
	data[static_cast<int>(Projectiles::ID::AlliedBullet)].texture = Textures::Entities;
	data[static_cast<int>(Projectiles::ID::AlliedBullet)].textureRect = sf::IntRect(178, 64, 3, 14);

	data[static_cast<int>(Projectiles::ID::AlliedBullet)].damage = 200;
	data[static_cast<int>(Projectiles::ID::AlliedBullet)].speed = 150.f;
	data[static_cast<int>(Projectiles::ID::AlliedBullet)].texture = Textures::Entities;
	data[static_cast<int>(Projectiles::ID::AlliedBullet)].textureRect = sf::IntRect(160, 64, 15, 32);

	return data;
}

std::vector<PickupData> initializePickupData()
{
	std::vector<PickupData> data(Pickup::TypeCount);
	
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

	data[Particle::Propellant].color = sf::Color(255, 255, 50);
	data[Particle::Propellant].lifetime = sf::seconds(0.6f);

	data[Particle::Smoke].color = sf::Color(50, 50, 50);
	data[Particle::Smoke].lifetime = sf::seconds(4.f);

	return data;
}


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

std::vector<TankPickupData> initializeTankPickupData() //Tank pickups data. Holds possibly shifting values associated with power ups - Jason Lynch
{
	std::vector<TankPickupData> data(static_cast<int>(TankPickup::ID::TypeCount)); //Get number of different types - Jason Lynch

	data[static_cast<int>(TankPickup::ID::HeavyGun)].texture = Textures::ID::HeavyGunPickup; //Set HeavyGun Pickup to assosiated texture - Jason Lynch
	data[static_cast<int>(TankPickup::ID::HeavyGun)].action = std::bind(&Tank::setTankTexture, std::placeholders::_1, 1); //Bind texture changing function to the action associated with pickup - Jason Lynch

	data[static_cast<int>(TankPickup::ID::GatlingGun)].texture = Textures::ID::GatlingGunPickup; //Set HeavyGun Pickup to assosiated texture - Jason Lynch
	data[static_cast<int>(TankPickup::ID::GatlingGun)].action = std::bind(&Tank::setTankTexture, std::placeholders::_1, 2); //Bind texture changing function to the action associated with pickup - Jason Lynch

	data[static_cast<int>(TankPickup::ID::TeslaGun)].texture = Textures::ID::TeslaGunPickup; //Set HeavyGun Pickup to assosiated texture - Jason Lynch
	data[static_cast<int>(TankPickup::ID::TeslaGun)].action = std::bind(&Tank::setTankTexture, std::placeholders::_1, 3); //Bind texture changing function to the action associated with pickup - Jason Lynch

	data[static_cast<int>(TankPickup::ID::Repair)].texture = Textures::ID::Repair;
	data[static_cast<int>(TankPickup::ID::Repair)].action = [](Tank& a) {a.repair(25); };

	data[static_cast<int>(TankPickup::ID::FireRate)].texture = Textures::ID::FireRate;
	data[static_cast<int>(TankPickup::ID::FireRate)].action = std::bind(&Tank::increaseFireRate, std::placeholders::_1);

	return data;
}
