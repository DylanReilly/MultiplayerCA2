//D00137655 - Jason Lynch
//D00194504 - Dylan
#include "SoundPlayer.hpp"

#include <SFML/Audio/Listener.hpp>

#include <cmath>


namespace
{
	// Sound coordinate system, point of view of a player in front of the screen:
	// X = left; Y = up; Z = back (out of the screen)
	const float ListenerZ = 300.f;
	const float Attenuation = 8.f;
	const float MinDistance2D = 200.f;
	const float MinDistance3D = std::sqrt(MinDistance2D*MinDistance2D + ListenerZ*ListenerZ);
}

SoundPlayer::SoundPlayer()
: mSoundBuffers()
, mSounds()
{
	
	mSoundBuffers.load(SoundEffect::AlliedGunfire, "Media/Sound/AlliedGunfire.wav");
	mSoundBuffers.load(SoundEffect::EnemyGunfire, "Media/Sound/EnemyGunfire.wav");
	mSoundBuffers.load(SoundEffect::Explosion1, "Media/Sound/Explosion1.wav");
	mSoundBuffers.load(SoundEffect::Explosion2, "Media/Sound/Explosion2.wav");
	mSoundBuffers.load(SoundEffect::LaunchMissile, "Media/Sound/LaunchMissile.wav");
	mSoundBuffers.load(SoundEffect::CollectPickup, "Media/Sound/CollectPickup.wav"); //New pickup collection sound - Jason Lynch
	mSoundBuffers.load(SoundEffect::Button, "Media/Sound/Button.wav");
	mSoundBuffers.load(SoundEffect::TankLMG, "Media/Sound/TankLMG.wav"); //Tank Lmg sound - Jason Lynch 
	mSoundBuffers.load(SoundEffect::TankGatling, "Media/Sound/TankGatling.wav"); //Tank gatling gun sound - Jason Lynch
	mSoundBuffers.load(SoundEffect::TankCannon1, "Media/Sound/TankCannon1.wav"); //1st tank fire sound - Jason Lynch
	mSoundBuffers.load(SoundEffect::TankCannon2, "Media/Sound/TankCannon2.wav"); // 2nd tank Cannon fire sound - Jason Lynch 
	mSoundBuffers.load(SoundEffect::TankDestroyed, "Media/Sound/TankDestroyed.wav"); //Tank destroyed sound - Jason Lynch 
	mSoundBuffers.load(SoundEffect::TankHitBullet, "Media/Sound/ProjectileImpactTank.wav"); //Sound for bullet impacting tank - Jason Lynch 
	mSoundBuffers.load(SoundEffect::TeslaBullet, "Media/Sound/TeslaShot.wav"); //Sound for Tesla bullet - Jason Lynch 

	// Listener points towards the screen (default in SFML)
	sf::Listener::setDirection(0.f, 0.f, -1.f);
}

void SoundPlayer::play(SoundEffect::ID effect)
{
	play(effect, getListenerPosition());
}

void SoundPlayer::play(SoundEffect::ID effect, sf::Vector2f position)
{
	mSounds.push_back(sf::Sound());
	sf::Sound& sound = mSounds.back();

	sound.setBuffer(mSoundBuffers.get(effect));
	sound.setPosition(position.x, -position.y, 0.f);
	sound.setAttenuation(Attenuation);
	sound.setMinDistance(MinDistance3D);

	sound.play();
}

void SoundPlayer::removeStoppedSounds()
{
	mSounds.remove_if([] (const sf::Sound& s)
	{
		return s.getStatus() == sf::Sound::Stopped;
	});
}

void SoundPlayer::setListenerPosition(sf::Vector2f position)
{
	sf::Listener::setPosition(position.x, -position.y, ListenerZ);
}

sf::Vector2f SoundPlayer::getListenerPosition() const
{
	sf::Vector3f position = sf::Listener::getPosition();
	return sf::Vector2f(position.x, -position.y);
}
