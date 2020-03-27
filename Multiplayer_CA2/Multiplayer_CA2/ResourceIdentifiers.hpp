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
		PlayerTwoInputSetupBackground,//Texture for state background  - Jason Lynch
		HostTankLmg,
		HostTankHmg,
		HostTankGatling,
		HostTankTesla
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
		TankLMG, //Added by me to play sound of Light Machine Gun - Jason Lynch
		TankCannon1,//Added by me to play sound of Heavy Tank Cannon - Jason Lynch
		TankCannon2,//Added by me to play sound of Heavy Tank Cannon - Jason Lynch
		TankGatling,//Added by me to play sound of Gatling Gun - Jason Lynch
		TankHitBullet,//Added by me to play sound of tank being hit - Jason Lynch
		TeslaBullet,//Added by me to play sound of Tesla ball - Jason Lynch
		TankDestroyed,//Added by me to play sound of tank blowing up - Jason Lynch
		LaunchMissile,
		CollectPickup,
		Button
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
