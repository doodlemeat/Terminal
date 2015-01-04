#include "Audiomanager.h"

#include "SFML\Audio\Sound.hpp"
#include "SFML\Audio\Music.hpp"
#include "SFML\Audio\SoundBuffer.hpp"

#include <iostream>

std::string Audiomanager::sfx_path;
std::string Audiomanager::mfx_path;
std::map<std::string, SFX*> Audiomanager::sfxs;
std::map<std::string, MFX*> Audiomanager::mfxs;
std::map<std::string, Playlist*> Audiomanager::m_playlists;
bool Audiomanager::m_muted;

Audiomanager::Audiomanager()
{

}
Audiomanager::~Audiomanager()
{
}
void Audiomanager::cleanup()
{
	{
		auto it = sfxs.begin();
		while (it != sfxs.end())
		{
			delete it->second;
			++it;
		}
		sfxs.clear();
	}
	{
		auto it = m_playlists.begin();
		while (it != m_playlists.end())
		{
			delete it->second;
			++it;
		}
		m_playlists.clear();
	}
	{
		auto it = mfxs.begin();
		while (it != mfxs.end())
		{
			delete it->second;
			++it;
		}
		mfxs.clear();
	}
}
void Audiomanager::setSFXPath(std::string path)
{
	sfx_path = path;
}
void Audiomanager::setMFXPath(std::string path)
{
	mfx_path = path;
}
void Audiomanager::createPlaylist(std::string key)
{
	auto it = m_playlists.find(key);
	if (it != m_playlists.end())
	{
		std::cout << "There is already a playlist with name: " << key << std::endl;
		return;
	}
	Playlist* playlist = new Playlist();
	m_playlists.insert(std::make_pair(key, playlist));
}
SFX* Audiomanager::sfx(std::string key)
{
	auto it = sfxs.find(key);
	if (it != sfxs.end())
		return it->second;
	else
		return nullptr;
}
MFX* Audiomanager::mfx(std::string key)
{
	auto it = mfxs.find(key);
	if (it != mfxs.end())
		return it->second;
	else
		return nullptr;
}
Playlist* Audiomanager::getPlaylist(std::string key)
{
	auto it = m_playlists.find(key);
	if (it == m_playlists.end())
	{
		std::cout << "Couldn't find playlist " << key << std::endl;
		return nullptr;
	}
	return it->second;
}
bool Audiomanager::add(int audiotype, std::string key, std::string file)
{
	if (audiotype == AUDIO_MFX)
	{
		auto it = mfxs.find(key);
		if (it != mfxs.end())
		{
			printf("[AUDIOMANAGER] MFX with key '%s' already exists\n", key.c_str());
			return false;
		}
		else
		{
			MFX* mfx = new MFX(mfx_path + file);
			mfxs.insert(std::pair<std::string, MFX*>(key, mfx));
			return true;
		}
	}
	else if (audiotype == AUDIO_SFX)
	{
		auto it = sfxs.find(key);
		if (it != sfxs.end())
		{
			printf("[AUDIOMANAGER] SFX with key '%s' already exists\n", key.c_str());
			return false;
		}
		else
		{
			SFX* sfx = new SFX(sfx_path + file);
			sfxs.insert(std::pair<std::string, SFX*>(key, sfx));
			return true;
		}
	}
	return false;
}
void Audiomanager::stopAllMFX()
{
	for (auto it = mfxs.begin(); it != mfxs.end(); ++it)
	{
		it->second->music->stop();
	}
}
void Audiomanager::unmuteMusic(int volume)
{
	for (auto &music : mfxs)
	{
		music.second->setVolume(volume);
	}
}
void Audiomanager::unmuteSound(int volume)
{
	for (auto &sound : sfxs)
	{
		sound.second->setVolume(volume);
	}
}
void Audiomanager::unmutePlaylists(int volume)
{

}
void Audiomanager::muteMusic()
{
	for (auto &music : mfxs)
	{
		music.second->setVolume(0);
	}
}
void Audiomanager::muteSound()
{
	for (auto &sound : sfxs)
	{
		sound.second->setVolume(0);
	}
}
void Audiomanager::mutePlaylists()
{
	for (auto &playlist : m_playlists)
	{
		//playlist->mute();
	}
}
void Audiomanager::muteAll()
{
	m_muted = true;
	muteMusic();
	muteSound();
	mutePlaylists();
}
void Audiomanager::unmuteAll(int volume)
{
	m_muted = false;
	unmuteMusic(volume);
	unmuteSound(volume);
	unmutePlaylists(volume);
}
bool Audiomanager::isMuted()
{
	return m_muted;
}


SFX::SFX(std::string path) : buffer(nullptr)
{
	buffer = new sf::SoundBuffer();
	if (!buffer->loadFromFile(path))
	{
		printf("[AUDIOMANAGER] Can't load SFX from '%s'\n", path.c_str());
	}
	else
	{
		printf("[AUDIOMANAGER] SFX created with path %s\n",  path.c_str());
	}
	m_sounds.clear();
	m_pitch = 1.0;
}
SFX::~SFX()
{
	auto it = m_sounds.begin();
	while (it != m_sounds.end())
	{
		delete *it;
		++it;
	}
	m_sounds.clear();
	delete buffer;
	buffer = nullptr;
}
void SFX::play(int p_loop)
{
	if (p_loop == -1)
	{
		if (m_sounds.empty())
		{
			addSound();
			m_sounds[0]->setLoop(true);
		}
	}
	else
	{
		for (unsigned int i = 0; i < m_sounds.size(); i++)
		{
			if (m_sounds[i]->getStatus() == sf::Sound::Stopped)
			{
				m_sounds[i]->play();
				return;
			}
		}

		sf::Sound* sound = new sf::Sound(*buffer);
		sound->play();
		sound->setPitch(m_pitch);
		m_sounds.push_back(sound);
	}
}
void SFX::stop()
{
	for (unsigned int i = 0; i < m_sounds.size(); i++)
	{
		if (m_sounds[i]->getStatus() == sf::Sound::Playing)
			m_sounds[i]->stop();
	}
}
void SFX::setPitch(float p_value)
{
	m_pitch = p_value;
	for (auto &sound: m_sounds)
	{
		sound->setPitch(m_pitch);
	}
}
void SFX::addSound()
{
	sf::Sound* sound = new sf::Sound(*buffer);
	sound->play();
	sound->setPitch(m_pitch);
	sound->setVolume(m_volume);
	m_sounds.push_back(sound);
}
void SFX::setVolume(int p_value)
{
	m_volume = p_value;
	for (auto &sound : m_sounds)
	{
		sound->setVolume(m_volume);
	}
}
int SFX::soundCount()
{
	return m_sounds.size();
}

MFX::MFX(std::string path)
{
	music = nullptr;
	music = new sf::Music();
	if (!music->openFromFile(path))
	{
		music = nullptr;
		printf("[AUDIOMANAGER] Can't load MFX from '%s'\n", path.c_str());
	}
}
MFX::~MFX()
{
	delete music;
	music = nullptr;
}
void MFX::play()
{
	music->play();
}
void MFX::stop()
{
	if (music->getStatus() == sf::Music::Playing)
		music->stop();
}
void MFX::pause()
{
	if (music->getStatus() == sf::Music::Playing)
		music->pause();
}
void MFX::setVolume(int value)
{
	music->setVolume(value);
}

Playlist::Playlist()
{
	m_tracks.clear();
	currentTrack = 0;
	paused = true;
}
Playlist::~Playlist()
{

}
void Playlist::play()
{
	paused = false;
	m_tracks[currentTrack]->play();
}
void Playlist::update()
{
	if (!paused)
	{
		if (m_tracks[currentTrack]->getStatus() == sf::Music::Stopped)
		{
			currentTrack++;
			if (currentTrack == m_tracks.size())
			{
				currentTrack = 0;
			}
			std::cout << currentTrack << std::endl;
			m_tracks[currentTrack]->play();
		}
	}
}
void Playlist::mute()
{
	for (auto &track : m_tracks)
	{
		track->setVolume(0);
	}
}
void Playlist::unmute(int volume)
{
	for (auto &track : m_tracks)
	{
		track->setVolume(volume);
	}
}
void Playlist::addTrack(std::string path)
{
	std::string mfx_path = Audiomanager::mfx_path + path;
	sf::Music* track = new sf::Music();
	if (track->openFromFile(mfx_path))
	{
		m_tracks.push_back(track);
	}
}