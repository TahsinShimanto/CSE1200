// Audio.h  — header-only audio manager for Windows (BGM + SFX)
// Works with your existing calls in main code.
// Author: ChatGPT
#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <cstring>
#include <string>

// Link against WinMM
#pragma comment(lib, "winmm.lib")

namespace Audio {

	// =================== CONFIG: edit paths if needed ===================
	// Use .mp3 or .wav for BGM, .wav recommended for SFX
	static const char* MENU_BGM_PATH = "audio\\menu_bgm.mp3";
	static const char* GAME_BGM_PATH = "audio\\game_bgm.mp3";
	static const char* HOVER_SFX_PATH = "audio\\hover.wav";
	static const char* HIT_SFX_PATH = "audio\\hit.wav";

	// Which gameState(s) should be treated as "in-game" for BGM
	// Your code uses: 0=menu, 1=level1, 7=level2 (level2 = 7 via 'level2 = 7;')
	inline bool isGameState(int s) { return (s == 1 || s == 7); }
	inline bool isMenuState(int s) { return (s == 0); }

	// =================== Internal state ===================
	enum class Track { None, Menu, Game };
	static Track g_currentTrack = Track::None;
	static bool  g_menuEnabled = true;
	static bool  g_gameEnabled = true;

	// We use a single MCI device alias for looping BGM
	static const char* BGM_ALIAS = "bgm_audio_alias";

	// Helpers
	inline std::string extLower(const char* path) {
		std::string s(path ? path : "");
		auto p = s.find_last_of('.');
		std::string e = (p == std::string::npos) ? "" : s.substr(p);
		for (auto& c : e) c = (char)tolower((unsigned char)c);
		return e;
	}

	// Close any open BGM device
	inline void closeBgmDevice() {
		mciSendStringA(("close " + std::string(BGM_ALIAS)).c_str(), nullptr, 0, nullptr);
	}

	// Open a file into our BGM alias; choose device type by extension
	inline bool openBgmFile(const char* filePath) {
		std::string ext = extLower(filePath);
		const char* type =
			(ext == ".mp3") ? "mpegvideo" :
			(ext == ".wav") ? "waveaudio" :
			"mpegvideo"; // default

		std::string cmd =
			"open \"" + std::string(filePath) + "\" type " + type + " alias " + BGM_ALIAS;
		MCIERROR err = mciSendStringA(cmd.c_str(), nullptr, 0, nullptr);
		return (err == 0);
	}

	// Start/restart playing (loop)
	inline void playBgmLoop() {
		std::string cmd = "play " + std::string(BGM_ALIAS) + " repeat";
		mciSendStringA(cmd.c_str(), nullptr, 0, nullptr);
	}

	// Public: stop everything (BGM)
	inline void stopAllMusic() {
		// stop + close bgm
		mciSendStringA(("stop " + std::string(BGM_ALIAS)).c_str(), nullptr, 0, nullptr);
		closeBgmDevice();
		g_currentTrack = Track::None;
	}

	// Start menu music (if allowed)
	inline void playMenuMusic() {
		if (!g_menuEnabled) { stopAllMusic(); return; }
		if (g_currentTrack == Track::Menu) return; // already playing
		stopAllMusic();
		if (openBgmFile(MENU_BGM_PATH)) {
			playBgmLoop();
			g_currentTrack = Track::Menu;
		}
		else {
			g_currentTrack = Track::None; // fail silently if file missing
		}
	}

	// Start game music (if allowed)
	inline void playGameMusic() {
		if (!g_gameEnabled) { stopAllMusic(); return; }
		if (g_currentTrack == Track::Game) return;
		stopAllMusic();
		if (openBgmFile(GAME_BGM_PATH)) {
			playBgmLoop();
			g_currentTrack = Track::Game;
		}
		else {
			g_currentTrack = Track::None;
		}
	}

	// Apply the proper BGM given current gameState
	// Called multiple times safely; avoids restarting same track.
	inline void applyBgmNow(int gameState) {
		if (isMenuState(gameState)) {
			if (g_menuEnabled) playMenuMusic();
			else               stopAllMusic();
		}
		else if (isGameState(gameState)) {
			if (g_gameEnabled) playGameMusic();
			else               stopAllMusic();
		}
		else {
			// Non-menu, non-game screens → keep things quiet
			stopAllMusic();
		}
	}

	// =================== SFX (can overlap with BGM) ===================
	// Note: PlaySoundA can overlap with MCI BGM.
	inline void playHoverSfx(int /*gameState_ignored*/) {
		// Non-blocking async; if the file is missing, it just won't play
		PlaySoundA(HOVER_SFX_PATH, nullptr, SND_FILENAME | SND_ASYNC);
	}

	inline void playHitSfx() {
		PlaySoundA(HIT_SFX_PATH, nullptr, SND_FILENAME | SND_ASYNC);
	}

	// Clean up if needed (your main calls closeHover at exit; we stop BGM here)
	inline void closeHover() {
		stopAllMusic();
		// Stop any lingering SFX
		PlaySoundA(nullptr, nullptr, 0);
	}

	// =================== Toggles & getters ===================
	inline void toggleMenuEnabled() {
		g_menuEnabled = !g_menuEnabled;
	}
	inline void toggleGameEnabled() {
		g_gameEnabled = !g_gameEnabled;
	}

	inline bool isMenuEnabled() { return g_menuEnabled; }
	inline bool isGameEnabled() { return g_gameEnabled; }

} // namespace Audio
