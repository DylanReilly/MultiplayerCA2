#ifndef BOOK_RESOURCEIDENTIFIERS_HPP
#define BOOK_RESOURCEIDENTIFIERS_HPP


// Forward declaration of SFML classes
namespace sf
{
	class Texture;
	class Font;
	class Shader;
	class SoundBuffer;
}

namespace Textures
{
	enum ID
	{
		Entities,
		Jungle,
		TitleScreen,
		Buttons,
		Explosion,
		LmgBullet, //Texture for Light Machine gun bullet - Dylan Reilly 
		HmgBullet, //Texture for Heavy Machine gun bullet - Dylan Reilly 
		GatlingBullet, //Texture for Gatling gun bullet - Dylan Reilly 
		TeslaBullet, //Texture for Tesla gun bullet - Dylan Reilly 
		HeavyGunPickup, //Texture for heavy gun pickup - Jason Lynch
		GatlingGunPickup,//Texture for Gatling gun pickup - Jason Lynch
		TeslaGunPickup,//Texture for Tesla gun pickup - Jason Lynch
		Nuke,//Texture for nuke pickup - Jason Lynch
		NukeExplosion,//Texture for nuke explosion - Jason Lynch
		Particle,
		Tanks, //Texture for tanks - Dylan Reilly
		Barrel,//Texture for barrel obstacle - Jason Lynch
		Wall,//Texture for wall obstacle - Jason Lynch
		DestructableWall,//Texture for wall obstacle - Jason Lynch
		Repair,
		FireRate,
		FinishLine,
		HowToPlay,//Texture for state background - Jason Lynch
		PlayerOneInputSetupBackground,//Texture for state background  - Jason Lynch
		PlayerTwoInputSetupBackground//Texture for state background  - Jason Lynch
	};
}

namespace Tanks
{
	enum ID
	{
		GreenLMG1,
		GreenLMG2,
		GreenLMG3,
		GreenHMG1,
		GreenHMG2,
		GreenHMG3,
		GreenGatling1,
		GreenGatling2,
		GreenGatling3,
		GreenTesla1,
		GreenTesla2,
		GreenTesla3,
		RedLMG1,
		RedLMG2,
		RedLMG3,
		RedHMG1,
		RedHMG2,
		RedHMG3,
		RedGatling1,
		RedGatling2,
		RedGatling3,
		RedTesla1,
		RedTesla2,
		RedTesla3,
		TypeCount
	};
}

namespace Obstacles
{
	enum ID
	{
		Wall,
		Barrel,
		DestructableWall,
		Nuke,
		TypeCount
	};
}

namespace TankPickup 
{
	enum class ID
	{
		HeavyGun, //Heavy gun pickup to change tank texture - Jason Lynch
		GatlingGun,//Gatling gun pickup to change tank texture - Jason Lynch
		TeslaGun,//Tesla gun pickup to change tank texture - Jason Lynch
		Nuke,//Nuke pickup to KILL EM ALL!!!!! - Jason Lynch
		Repair, //Added for healing - Dylan Reilly 
		FireRate, //Added for increased fire speed - Dylan Reilly
		TypeCount
	};
}

namespace Projectiles
{
	enum class ID
	{
		None,
		AlliedBullet,
		EnemyBullet,
		Missile,
		GreenLMGBullet, //Added to distinguishing between each players Projectiles - Dylan Reilly
		GreenHMGBullet,//Added to distinguishing between each players Projectiles - Dylan Reilly
		GreenGatlingBullet,//Added to distinguishing between each players Projectiles - Dylan Reilly
		GreenTeslaBullet,//Added to distinguishing between each players Projectiles - Dylan Reilly
		RedLMGBullet,//Added to distinguishing between each players Projectiles - Dylan Reilly
		RedHMGBullet,//Added to distinguishing between each players Projectiles - Dylan Reilly
		RedGatlingBullet,//Added to distinguishing between each players Projectiles - Dylan Reilly
		RedTeslaBullet,//Added to distinguishing between each players Projectiles - Dylan Reilly
		TypeCount
	};
}

namespace Shaders
{
	enum ID
	{
		BrightnessPass,
		DownSamplePass,
		GaussianBlurPass,
		AddPass,
	};
}

namespace Fonts
{
	enum ID
	{
		Main,
	};
}

namespace SoundEffect
{
	enum ID
	{
		AlliedGunfire,
		EnemyGunfire,
		Explosion1,
		Explosion2,
		LaunchMissile,
		CollectPickup,
		Button,
	};
}

namespace Music
{
	enum ID
	{
		MenuTheme,
		MissionTheme,
	};
}


// Forward declaration and a few type definitions
template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures::ID>			TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID>					FontHolder;
typedef ResourceHolder<sf::Shader, Shaders::ID>				ShaderHolder;
typedef ResourceHolder<sf::SoundBuffer, SoundEffect::ID>	SoundBufferHolder;

#endif // BOOK_RESOURCEIDENTIFIERS_HPP
