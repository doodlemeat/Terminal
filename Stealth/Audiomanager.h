#pragma once

#include <map>
#include <vector>
namespace sf
{
	class SoundBuffer;
	class Music;
	class Sound;
}

class SFX
{
public:
	SFX(std::string path);
	~SFX();

	void play(int p_loop = 1);
	void stop();
	void setPitch(float p_value);
	void addSound();
	void setVolume(int p_value);
	int soundCount();

	sf::SoundBuffer* buffer;
	std::vector<sf::Sound*> m_sounds;
	float m_pitch;
	int m_volume;
};

class MFX
{
public:
	MFX(std::string path);
	~MFX();

	void play();
	void stop();
	void pause();
	void setVolume(int value);

	sf::Music* music;
};

class Playlist
{
public:
	Playlist();
	~Playlist();

	void addTrack(std::string file);
	void play();
	void update();
	void mute();
	void unmute(int volume);
private:
	std::vector<sf::Music*> m_tracks;
	int currentTrack;
	bool paused;
};

enum AudioTypes
{
	AUDIO_SFX,
	AUDIO_MFX
};

class Audiomanager
{
	friend class Playlist;
public:
	Audiomanager();
	~Audiomanager();

	static void cleanup();

	static void setSFXPath(std::string path);
	static void setMFXPath(std::string path);

	static void createPlaylist(std::string key);
	static SFX* sfx(std::string key);
	static MFX* mfx(std::string key);
	static Playlist* getPlaylist(std::string key);
	static bool add(int audiotype, std::string key, std::string file);

	static void stopAllMFX();
	static void muteMusic();
	static void muteSound();
	static void mutePlaylists();
	static void muteAll();
	static void unmuteMusic(int volume);
	static void unmuteSound(int volume);
	static void unmutePlaylists(int volume);
	static void unmuteAll(int volume);

	static bool isMuted();
private:
	static bool m_muted;
	static std::string sfx_path;
	static std::string mfx_path;
	static std::map<std::string, Playlist*> m_playlists;
	static std::map<std::string, SFX*> sfxs;
	static std::map<std::string, MFX*> mfxs;
};