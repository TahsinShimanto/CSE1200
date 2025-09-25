#include "iGraphics.h"
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <string>

//Screen
int Screen_Width = 1000;
int Screen_Heigth = 667;
int gameState = 0; // 0=menu,1=level-1,2=level select,4=game over,5=options,6=credits,7=level-2, 9=level-3, 10=img after L3
int level1 = 1;
int level2 = 7;
int level3 = 9;
int levelComplete = 8;
int levelTimer = 60;
int progressTime = 60;
bool timerOn = false;
int ic1, ic2;
int justFinishedLevel = 0;
// Menu Buttons 
int btnX = 613;
int startY = 495, levelY = 395, creditsY = 295, optionsY = 195, exitY = 95;
const int BUTTON_W = 310, BUTTON_H = 80;
int hoverStartImg, hoverLevelImg, hoverCreditsImg, hoverOptionImg, hoverExitImg;
int hoveredButton = -1, prevHoveredButton = -1;
// Options 
enum { OPT_IMG1 = 0, OPT_IMG2 = 1, OPT_HISCORE = 2, OPT_MUSIC = 3 };
int optionsTab = OPT_IMG1;  

// level select
const int btnW = 250, btnH = 170;
int btnX1 = 62, btnY1 = 300;
int btnX2 = 374, btnY2 = 300; 
int btnX3 = 686, btnY3 = 300;
//Level Complete
const int lcW = 150, lcH = 20;
int NoX = 257, NoY = 223;
int YesX = 563, YesY = 223;
// Path 
const int max_point = 19;
// Level 3 path
int path_x3[max_point] = { 0, 1, 110, 194, 583, 638, 677, 680, 663, 603, 398, 348, 320, 305, 308, 327, 373, 423, 1000 };
int path_y3[max_point] = { 200, 208, 185, 160, 153, 177, 222, 272, 317, 362, 367, 385, 422, 470, 508, 543, 570, 577, 577 };

// Level 2 path
int path_x2[max_point] = { 0, 317, 368, 382, 387, 427, 675, 732, 765, 820, 891, 924, 898, 847, 787, 749, 737, 737, 735 };
int path_y2[max_point] = { 205, 210, 247, 297, 482, 535, 551, 512, 403, 357, 325, 261, 199, 164, 143, 116, 74, 1, 0 };
// Level 1 path
int path_x[max_point] = { 0, 73, 141, 212, 273, 314, 356, 410, 466, 533, 604, 650, 704, 753, 805, 831, 844, 866, 998 };
int path_y[max_point] = { 328, 328, 328, 326, 326, 326, 326, 329, 329, 325, 317, 288, 279, 291, 314, 364, 409, 442, 479 };
// enemy passed
int ep = 100;

// Build Bar
int clickbarX = 450, clickbarY = 30;
int overlayX = 200, overlayY = 10, overlayW = Screen_Width - 400, overlayH = 120;
int closeX = overlayX + overlayW - 35, closeY = overlayY + overlayH - 35;
int showbar = 0;
int place = 0;
int clickbar;
int selectionbar;
int removeBtnX = clickbarX + 60;
int removeBtnY = clickbarY;
int removeBtnSize = 50;
const int imgAfterL3 = 10;
//Turrets 
struct Turret {
	int x, y;
	bool placed;
	int fireCooldown;
	int type;
	int slotIndex;
	int slotLevel;
	bool building;
	int buildTimer;
	int despawnSec;
};
const int TURRET_LIFETIME_SEC = 15;
const int MAX_TURRETS = 20;
const int TURRENT_DAMAGE[3] = { 1, 2, 3 };
Turret turrets[MAX_TURRETS];
int turretCount = 0;
int turretFrames[11], turret2Frames[11], turret3Frames[11];
int currentTurretFrame = 0;
int selectedTowerType = 0;
const int TOWER_W[3] = { 80, 80, 60 };
const int TOWER_H[3] = { 80, 80, 100 };

//  Turret Slots 
const int MAX_SLOTS_L3 = 6;
int slotX[MAX_SLOTS_L3] = { 425, 620, 790, 335, 515, 810 };
int slotY[MAX_SLOTS_L3] = { 520, 520, 520, 320, 320, 305 };
bool slotOccupied[MAX_SLOTS_L3] = { false };

const int MAX_SLOTS_L2 = 6;
int slotX2[MAX_SLOTS_L2] = { 185, 280, 280, 570, 875, 811 };
int slotY2[MAX_SLOTS_L2] = { 150, 526, 329, 493, 463, 293 };
bool slotOccupied2[MAX_SLOTS_L2] = { false };

const int MAX_SLOTS_L1 = 6;
int slotX3[MAX_SLOTS_L1] = { 290, 170, 415, 705, 830, 882 };
int slotY3[MAX_SLOTS_L1] = { 447, 267, 276, 409, 251, 578 };
bool slotOccupied3[MAX_SLOTS_L1] = { false };

int removeMode = 0;

//  Enemies 
struct Enemy {
	int position_x, position_y;
	int frames[7], frameIndex, speed, current_point;
	bool alive;
} enemy[5];

#define MAX_BOSSES 3

struct Boss {
	int frames[11];
	int frameIndex;
	int position_x, position_y;
	int speed;
	int current_point;
	bool alive;
	int hitCount;
	int hp;
	int maxHp;
	int curHP;
	int spawnLevel;
	bool proceed;
	int respawnCounter;
};
Boss bosses[MAX_BOSSES];
int bossW = 80, bossH = 60;

// Beams 
struct Beam { int x, y; bool active; int image; float vx, vy; int dmg; };
const int MAX_BEAMS = 50;
Beam beams[MAX_BEAMS];
int beamImage, beamSpeed = 10;

//  Score
int coinImg;
int creditImg;
int score = 0, displayScore = 0, coins = 0, coinPerKill = 5, enemiesPassed = 0;
float speedMultiplier = 1.0f; int lastSpeedTier = 0;

// Sound Options
bool menuBgmEnabled = true; // UI toggle (no real audio)
bool gameBgmEnabled = true; // UI toggle (no real audio)

// High Score storage
int highScore = 0;
void loadHighScore(){
	FILE* f = nullptr;
	if (fopen_s(&f, "highscore.txt", "r") == 0 && f){
		if (fscanf_s(f, "%d", &highScore) != 1) highScore = 0;
		fclose(f);
	}
	else {
		highScore = 0;
	}
}
void saveHighScore(){
	FILE* f = nullptr;
	if (fopen_s(&f, "highscore.txt", "w") == 0 && f){
		fprintf(f, "%d", highScore);
		fclose(f);
	}
}
inline void updateHighScoreWith(int val){
	if (val > highScore){
		highScore = val;
		saveHighScore();
	}
}
inline void updateHighScore(){ updateHighScoreWith(score); }


inline bool inButton(int mx, int my, int x, int y, int w, int h){ return mx >= x && mx <= x + w && my >= y && my <= y + h; }
static inline int mulRoundSpeed(int sp, float factor){ float v = sp*factor; if (v < 1) v = 1; return (int)(v + 0.5f); }

void updateEnemySpeedScaling(){
	int tier = score / 1000;
	if (tier > lastSpeedTier){
		int times = tier - lastSpeedTier;
		while (times--){
			speedMultiplier *= 1.2f;
			for (int i = 0; i < 5; i++) if (enemy[i].alive) enemy[i].speed = mulRoundSpeed(enemy[i].speed, 1.2f);
		}
		lastSpeedTier = tier;
	}
}

//  SOUND
#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

static const wchar_t* MENU_MUSIC_PATH = L"sound\\s9.wav"; // menu music
static const wchar_t* GAME_MUSIC_PATH = L"sound\\s3.wav"; // gameplay music

enum { MUSIC_NONE = 0, MUSIC_MENU = 1, MUSIC_GAME = 2 };
int currentMusic = MUSIC_NONE;

static inline bool isGameplayState(){
	return (gameState == level1 || gameState == level2 || gameState == level3);
}
void stopMusic(){
#ifdef _WIN32
	PlaySoundW(NULL, NULL, 0);
#endif
	currentMusic = MUSIC_NONE;
}
void playMenuMusic(){
#ifdef _WIN32
	if (!menuBgmEnabled){ stopMusic(); return; }
	PlaySoundW(MENU_MUSIC_PATH, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
#endif
	currentMusic = MUSIC_MENU;
}
void playGameMusic(){
#ifdef _WIN32
	if (!gameBgmEnabled){ stopMusic(); return; }
	PlaySoundW(GAME_MUSIC_PATH, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
#endif
	currentMusic = MUSIC_GAME;
}
void updateMusicForState(){
	if (isGameplayState()){
		if (gameBgmEnabled){
			if (currentMusic != MUSIC_GAME) playGameMusic();
		}
		else{
			if (currentMusic != MUSIC_NONE) stopMusic();
		}
	}
	else{
		if (menuBgmEnabled){
			if (currentMusic != MUSIC_MENU) playMenuMusic();
		}
		else{
			if (currentMusic != MUSIC_NONE) stopMusic();
		}
	}
}
// -----------------------------------------------------------

//  Resets
void resetBeams(){ for (int i = 0; i < MAX_BEAMS; i++){ beams[i].active = false; beams[i].vx = 0; beams[i].vy = 0; } }

void resetEnemy(int i){
	if (gameState == level1){
		enemy[i].position_x = path_x[0] - 30; enemy[i].position_y = path_y[0];
	}
	else if (gameState == level2){
		enemy[i].position_x = path_x2[0] - 30; enemy[i].position_y = path_y2[0];
	}
	else if (gameState == level3){
		enemy[i].position_x = path_x3[0] - 30; enemy[i].position_y = path_y3[0];
	}
	int base = (rand() % 3) + 3; enemy[i].speed = mulRoundSpeed(base, speedMultiplier);
	enemy[i].current_point = 1; enemy[i].alive = true; enemy[i].frameIndex = 0;
}
void resetEnemies(){ for (int i = 0; i < 5; i++) resetEnemy(i); }

void resetBosses() {
	for (int i = 0; i < MAX_BOSSES; i++) {
		bosses[i].alive = false;
		bosses[i].hitCount = 0;
		bosses[i].frameIndex = 0;
		bosses[i].current_point = 1;
		bosses[i].curHP = bosses[i].maxHp;
	}
}

void checkBossSpawn() {
	int respawnDelay = 300;

	if (gameState == level1) {
		if (!bosses[0].alive) {
			bosses[0].respawnCounter++;
			if (bosses[0].respawnCounter >= respawnDelay) {
				bosses[0].alive = true;
				bosses[0].curHP = bosses[0].maxHp;
				bosses[0].position_x = path_x[0] - 30;
				bosses[0].position_y = path_y[0];
				bosses[0].current_point = 1;
				bosses[0].respawnCounter = 0;
				bosses[0].hitCount = 0;
			}
		}
	}
	else if (gameState == level2) {
		int delay2[2] = { 0, 200 };
		for (int i = 0; i < 2; i++) {
			if (!bosses[i].alive) {
				bosses[i].respawnCounter++;
				if (bosses[i].respawnCounter >= respawnDelay + delay2[i]) {
					bosses[i].alive = true;
					bosses[i].position_x = path_x2[0] - 30;
					bosses[i].position_y = path_y2[0];
					bosses[i].current_point = 1;
					bosses[i].respawnCounter = 0;
					bosses[i].curHP = bosses[i].maxHp;
					bosses[i].hitCount = 0;
				}
			}
		}
	}
	else if (gameState == level3) {
		int delay3[3] = { 0, 200, 350 };
		for (int i = 0; i < 3; i++) {
			if (!bosses[i].alive) {
				bosses[i].respawnCounter++;
				if (bosses[i].respawnCounter >= respawnDelay + delay3[i]) {
					bosses[i].alive = true;
					bosses[i].position_x = path_x3[0] - 30;
					bosses[i].position_y = path_y3[0];
					bosses[i].current_point = 1;
					bosses[i].respawnCounter = 0;
					bosses[i].curHP = bosses[i].maxHp;
					bosses[i].hitCount = 0;
				}
			}
		}
	}
}

void resetTurrets() {
	for (int i = 0; i < MAX_TURRETS; i++) {
		turrets[i].placed = false;
		turrets[i].fireCooldown = 0;
		turrets[i].type = 0;
		turrets[i].x = 0;
		turrets[i].y = 0;
		turrets[i].slotIndex = -1;
		turrets[i].slotLevel = 0;
		turrets[i].despawnSec = 0;
	}
	for (int i = 0; i < MAX_SLOTS_L1; i++) slotOccupied[i] = false;
	for (int i = 0; i < MAX_SLOTS_L2; i++) slotOccupied2[i] = false;
	for (int i = 0; i < MAX_SLOTS_L3; i++) slotOccupied3[i] = false;
	turretCount = 0;
}

void freshStart(int level){
	gameState = (level == 1) ? level1 :
		(level == 2) ? level2 : level3;
	coins = 0; score = 0; displayScore = 0; enemiesPassed = 0;
	speedMultiplier = 1.0f; lastSpeedTier = 0;
	levelTimer = progressTime;              
	timerOn = true;
	resetTurrets(); resetBeams(); resetEnemies(); resetBosses();
	updateMusicForState();                
}

void levelChange(){
	if (gameState == level1 && levelTimer <= 0){
		int levelEndScore = score;
		updateHighScoreWith(levelEndScore);
		justFinishedLevel = 1;
		gameState = levelComplete;

		coins = 0; score = 0; displayScore = 0;
		resetTurrets(); resetBeams(); resetEnemies(); resetBosses();
		levelTimer = progressTime;
		timerOn = false;
		updateMusicForState();          
	}
	
	if (gameState == level2 && levelTimer <= 0){
		int levelEndScore = score;
		updateHighScoreWith(levelEndScore);
		justFinishedLevel = 2;
		gameState = levelComplete;

		coins = 0; score = 0; displayScore = 0;
		resetTurrets(); resetBeams(); resetEnemies(); resetBosses();
		levelTimer = progressTime;
		timerOn = false;
		updateMusicForState();
	}
	if (gameState == level3 && levelTimer <= 0){
		int levelEndScore = score;
		updateHighScoreWith(levelEndScore);

		gameState = imgAfterL3;
		timerOn = false;

		resetTurrets(); resetBeams(); resetEnemies(); resetBosses();
		updateMusicForState(); 
	}
}

// ===== Scoring =====
void increaseScore(){
	if (gameState == level1 || gameState == level2 || gameState == level3){
		score += 100;
		updateEnemySpeedScaling();
		levelChange();
	}
}
void updateDisplayScore() {
	if (displayScore < score) {
		displayScore += 2;
		if (displayScore > score) displayScore = score;
	}
}


inline void passAndRespawn(int idx){
	enemy[idx].alive = false;
	enemiesPassed++;
	resetEnemy(idx);
}

void advanceEnemiesOnPath(const int* px, const int* py){
	for (int i = 0; i < 5; i++){
		if (!enemy[i].alive) continue;

		if (enemy[i].current_point >= max_point){
			passAndRespawn(i);
			continue;
		}

		int tx = px[enemy[i].current_point];
		int ty = py[enemy[i].current_point];

		float dx = (float)tx - (float)enemy[i].position_x;
		float dy = (float)ty - (float)enemy[i].position_y;
		float dist = sqrtf(dx*dx + dy*dy);

		if (dist <= enemy[i].speed){
			enemy[i].position_x = tx;
			enemy[i].position_y = ty;
			enemy[i].current_point++;

			if (enemy[i].current_point >= max_point){
				passAndRespawn(i);
				continue;
			}
		}
		else {
			enemy[i].position_x += (int)(enemy[i].speed * dx / dist);
			enemy[i].position_y += (int)(enemy[i].speed * dy / dist);
		}
	}
}

void moveBossOnPath(Boss &b, const int* px, const int* py, int max_point) {
	if (!b.alive) return;

	if (b.current_point >= max_point) {
		b.alive = false;
		enemiesPassed++;
		return;
	}

	int tx = px[b.current_point];
	int ty = py[b.current_point];
	float dx = (float)tx - b.position_x;
	float dy = (float)ty - b.position_y;
	float dist = sqrtf(dx * dx + dy * dy);

	if (dist <= b.speed) {
		b.position_x = tx;
		b.position_y = ty;
		b.current_point++;
	}
	else {
		b.position_x += (int)(b.speed * dx / dist);
		b.position_y += (int)(b.speed * dy / dist);
	}
}

void moveBosses() {
	if (gameState == level1) {
		for (int i = 0; i < MAX_BOSSES; i++) moveBossOnPath(bosses[i], path_x, path_y, max_point);
	}
	else if (gameState == level2) {
		for (int i = 0; i < MAX_BOSSES; i++) moveBossOnPath(bosses[i], path_x2, path_y2, max_point);
	}
	else if (gameState == level3) {
		for (int i = 0; i < MAX_BOSSES; i++) moveBossOnPath(bosses[i], path_x3, path_y3, max_point);
	}
}

void moveEnemy(){
	if (gameState == level1){
		advanceEnemiesOnPath(path_x, path_y);
		moveBosses(); checkBossSpawn();
		if (enemiesPassed >= ep){
			int levelEndScore = score;
			updateHighScoreWith(levelEndScore);
			gameState = 4;
			updateMusicForState();       
		}
	}
	else if (gameState == level2){
		advanceEnemiesOnPath(path_x2, path_y2);
		moveBosses(); checkBossSpawn();
		if (enemiesPassed >= ep){
			int levelEndScore = score;
			updateHighScoreWith(levelEndScore);
			gameState = 4;
			updateMusicForState();
		}
	}
	else if (gameState == level3){
		advanceEnemiesOnPath(path_x3, path_y3);
		moveBosses(); checkBossSpawn();
		if (enemiesPassed >= ep){
			int levelEndScore = score;
			updateHighScoreWith(levelEndScore);
			gameState = 4;
			updateMusicForState();
		}
	}
}

// Firing
void fireBeamTurret1(int i, int ex, int ey) {
	int cx = turrets[i].x + TOWER_W[0] / 2;
	int cy = turrets[i].y + TOWER_H[0] / 2;

	for (int j = 0; j < MAX_BEAMS; j++) {
		if (!beams[j].active) {
			beams[j].active = true;
			beams[j].x = cx;
			beams[j].y = cy;
			beams[j].vx = 0;
			beams[j].vy = -abs(beamSpeed);
			beams[j].image = beamImage;
			beams[j].dmg = TURRENT_DAMAGE[0];
			break;
		}
	}
}

void fireBeamTurret2(int i, int ex, int ey) {
	int cx = turrets[i].x + TOWER_W[1] / 2;
	int cy = turrets[i].y;

	int dx = ex - cx;
	int dy = ey - cy;
	float len = sqrtf((float)dx*dx + (float)dy*dy);
	float vx = dx / len * beamSpeed;
	float vy = dy / len * beamSpeed;

	if (vy > 0) vy = -vy;

	for (int j = 0; j < MAX_BEAMS; j++) {
		if (!beams[j].active) {
			beams[j].active = true;
			beams[j].x = cx;
			beams[j].y = cy;
			beams[j].vx = vx;
			beams[j].vy = vy;
			beams[j].image = beamImage;
			beams[j].dmg = TURRENT_DAMAGE[1];
			break;
		}
	}
}

void fireBeamTurret3(int i, int ex, int ey) {
	int cx = turrets[i].x + TOWER_W[2] / 2;
	int cy = turrets[i].y + TOWER_H[2] / 2;

	float dx = (float)ex - cx;
	float dy = (float)ey - cy;
	float len = sqrtf(dx*dx + dy*dy);
	float vx = dx / len * beamSpeed;
	float vy = dy / len * beamSpeed;

	if (ey > cy) vy = fabs(vy);
	else vy = -fabs(vy);

	for (int j = 0; j < MAX_BEAMS; j++) {
		if (!beams[j].active) {
			beams[j].active = true;
			beams[j].x = cx;
			beams[j].y = cy;
			beams[j].vx = vx;
			beams[j].vy = vy;
			beams[j].image = beamImage;
			beams[j].dmg = TURRENT_DAMAGE[2];
			break;
		}
	}
}

void fireBeams() {
	const int FIRE_RANGE[3] = { 150, 200, 160 };
	const int FIRE_COOLDOWN[3] = { 10, 30, 40 };

	for (int i = 0; i < MAX_TURRETS; i++) {
		if (!turrets[i].placed) continue;

		if (turrets[i].fireCooldown > 0) {
			turrets[i].fireCooldown--;
			continue;
		}

		int type = turrets[i].type;
		int cx = turrets[i].x + TOWER_W[type] / 2;
		int cy = turrets[i].y + TOWER_H[type] / 2;

		bool fired = false;

		for (int e = 0; e < 5; e++) {
			if (!enemy[e].alive) continue;

			int dx = enemy[e].position_x - cx;
			int dy = enemy[e].position_y - cy;

			if (dx*dx + dy*dy <= FIRE_RANGE[type] * FIRE_RANGE[type]) {
				if (type == 0) fireBeamTurret1(i, enemy[e].position_x, enemy[e].position_y);
				else if (type == 1) fireBeamTurret2(i, enemy[e].position_x, enemy[e].position_y);
				else if (type == 2) fireBeamTurret3(i, enemy[e].position_x, enemy[e].position_y);
				fired = true;
				break;
			}
		}

		if (fired) turrets[i].fireCooldown = FIRE_COOLDOWN[type];
	}
}


void moveBeams(){
	if (gameState != level1 && gameState != level2 && gameState != level3) return;
	for (int i = 0; i < MAX_BEAMS; i++){
		if (beams[i].active){
			beams[i].x += (int)beams[i].vx; beams[i].y += (int)beams[i].vy;
			if (beams[i].x < -20 || beams[i].x > Screen_Width + 20 || beams[i].y < -20 || beams[i].y > Screen_Heigth + 20)
				beams[i].active = false;
		}
	}
}

void checkCollision() {
	if (gameState != level1 && gameState != level2 && gameState != level3) return;

	for (int b = 0; b < MAX_BEAMS; b++) {
		if (!beams[b].active) continue;

		// Enemy collisions
		for (int e = 0; e < 5; e++) {
			if (!enemy[e].alive) continue;

			if (beams[b].x >= enemy[e].position_x && beams[b].x <= enemy[e].position_x + 30 &&
				beams[b].y >= enemy[e].position_y && beams[b].y <= enemy[e].position_y + 30) {

				beams[b].active = false;
				enemy[e].alive = false;
				coins += coinPerKill;
				score += 100;
				updateEnemySpeedScaling();
				levelChange();
				resetEnemy(e);
				break;
			}
		}

		// Boss collisions 
		for (int i = 0; i < MAX_BOSSES; i++) {
			if (!bosses[i].alive) continue;

			if (beams[b].x >= bosses[i].position_x &&
				beams[b].x <= bosses[i].position_x + bossW &&
				beams[b].y >= bosses[i].position_y &&
				beams[b].y <= bosses[i].position_y + bossH) {

				beams[b].active = false;

				int damage = (beams[b].dmg>0) ? beams[b].dmg : 1;
				bosses[i].curHP -= damage;

				if (bosses[i].curHP <= 0) {
					bosses[i].alive = false;

					score += 200;
					coins += 20;
				}
				break;
			}
		}
	}
}

void animateTurret() {
	if (gameState != level1 && gameState != level2 && gameState != level3) return;

	currentTurretFrame = (currentTurretFrame + 1) % 11;

	const int FIRE_RANGE[3] = { 150, 200, 160 };
	const int FIRE_COOLDOWN[3] = { 5, 8, 3 };

	for (int t = 0; t < turretCount; t++) {
		if (!turrets[t].placed) continue;

		if (turrets[t].fireCooldown > 0) {
			turrets[t].fireCooldown--;
			continue;
		}

		int type = turrets[t].type;
		int cx = turrets[t].x + TOWER_W[type] / 2;
		int cy = turrets[t].y + TOWER_H[type] / 2;

		for (int e = 0; e < 5; e++) {
			if (!enemy[e].alive) continue;

			int dx = enemy[e].position_x - cx;
			int dy = enemy[e].position_y - cy;

			if (dx*dx + dy*dy <= FIRE_RANGE[type] * FIRE_RANGE[type]) {
				if (type == 0) fireBeamTurret1(t, enemy[e].position_x, enemy[e].position_y);
				else if (type == 1) fireBeamTurret2(t, enemy[e].position_x, enemy[e].position_y);
				else if (type == 2) fireBeamTurret3(t, enemy[e].position_x, enemy[e].position_y);

				turrets[t].fireCooldown = FIRE_COOLDOWN[type];
				break;
			}
		}
	}
}

void tickTurretLifetimes() {
	for (int i = 0; i < turretCount; i++) {
		if (!turrets[i].placed) continue;
		if (turrets[i].despawnSec > 0) {
			turrets[i].despawnSec--;
			if (turrets[i].despawnSec <= 0) {
				int s = turrets[i].slotIndex;
				if (s >= 0) {
					if (turrets[i].slotLevel == 1) slotOccupied[s] = false;
					else if (turrets[i].slotLevel == 2) slotOccupied2[s] = false;
					else if (turrets[i].slotLevel == 3) slotOccupied3[s] = false;
				}
				turrets[i].placed = false;
				turrets[i].slotIndex = -1;
				turrets[i].slotLevel = 0;
				turrets[i].x = turrets[i].y = 0;
				turrets[i].type = 0;
				turrets[i].fireCooldown = 0;
			}
		}
	}
}

void animateEnemies(){
	if (gameState != level1 && gameState != level2 && gameState != level3) return;
	for (int i = 0; i < 5; i++)
	if (enemy[i].alive) enemy[i].frameIndex = (enemy[i].frameIndex + 1) % 7;
}

void animateBosses() {
	for (int i = 0; i < MAX_BOSSES; i++) {
		if (bosses[i].alive) bosses[i].frameIndex = (bosses[i].frameIndex + 1) % 11;
	}
}


void removeTurretAt(int mx, int my) {
	for (int i = 0; i < turretCount; i++) {
		if (!turrets[i].placed) continue;
		int tw = TOWER_W[turrets[i].type];
		int th = TOWER_H[turrets[i].type];
		if (mx >= turrets[i].x && mx <= turrets[i].x + tw &&
			my >= turrets[i].y && my <= turrets[i].y + th) {
			int s = turrets[i].slotIndex;
			if (s >= 0) {
				if (turrets[i].slotLevel == 1) slotOccupied[s] = false;
				else if (turrets[i].slotLevel == 2) slotOccupied2[s] = false;
				else if (turrets[i].slotLevel == 3) slotOccupied3[s] = false;
			}
			turrets[i].placed = false;
			turrets[i].slotIndex = -1;
			turrets[i].slotLevel = 0;
			turrets[i].x = turrets[i].y = 0;
			turrets[i].type = 0;
			turrets[i].fireCooldown = 0;
			removeMode = 0;
			break;
		}
	}
}

void placeTurretAtSlot(int mx, int my){
	if (gameState == level3){
		int slot = -1;
		for (int i = 0; i < MAX_SLOTS_L1; i++){
			if (!slotOccupied[i]){
				if (mx >= slotX[i] - 25 && mx <= slotX[i] + 25 &&
					my >= slotY[i] - 25 && my <= slotY[i] + 25){
					slot = i; break;
				}
			}
		}
		if (slot != -1){
			int tIndex = -1;
			for (int i = 0; i < MAX_TURRETS; i++) if (!turrets[i].placed){ tIndex = i; break; }
			if (tIndex == -1 && turretCount < MAX_TURRETS) tIndex = turretCount;
			if (tIndex != -1) {
				turrets[tIndex].x = slotX[slot] - (TOWER_W[selectedTowerType] / 2);
				turrets[tIndex].y = slotY[slot] - (TOWER_H[selectedTowerType] / 2);
				turrets[tIndex].placed = true;
				turrets[tIndex].fireCooldown = 0;
				turrets[tIndex].type = selectedTowerType;
				turrets[tIndex].slotIndex = slot;
				turrets[tIndex].slotLevel = 1;
				turrets[tIndex].despawnSec = TURRET_LIFETIME_SEC;
				if (tIndex + 1 > turretCount) turretCount = tIndex + 1;
				slotOccupied[slot] = true;
			}
		}
	}
	else if (gameState == level2){
		int slot = -1;
		for (int i = 0; i < MAX_SLOTS_L2; i++){
			if (!slotOccupied2[i]){
				if (mx >= slotX2[i] - 25 && mx <= slotX2[i] + 25 &&
					my >= slotY2[i] - 25 && my <= slotY2[i] + 25){
					slot = i; break;
				}
			}
		}
		if (slot != -1){
			int tIndex = -1;
			for (int i = 0; i < MAX_TURRETS; i++) if (!turrets[i].placed){ tIndex = i; break; }
			if (tIndex == -1 && turretCount < MAX_TURRETS) tIndex = turretCount;
			if (tIndex != -1) {
				turrets[tIndex].x = slotX2[slot] - (TOWER_W[selectedTowerType] / 2);
				turrets[tIndex].y = slotY2[slot] - (TOWER_H[selectedTowerType] / 2);
				turrets[tIndex].placed = true;
				turrets[tIndex].fireCooldown = 0;
				turrets[tIndex].type = selectedTowerType;
				turrets[tIndex].slotIndex = slot;
				turrets[tIndex].slotLevel = 2;
				turrets[tIndex].despawnSec = TURRET_LIFETIME_SEC;
				if (tIndex + 1 > turretCount) turretCount = tIndex + 1;
				slotOccupied2[slot] = true;
			}
		}
	}
	else if (gameState == level1){
		int slot = -1;
		for (int i = 0; i < MAX_SLOTS_L1; i++){
			if (!slotOccupied3[i]){
				if (mx >= slotX3[i] - 25 && mx <= slotX3[i] + 25 &&
					my >= slotY3[i] - 25 && my <= slotY3[i] + 25){
					slot = i; break;
				}
			}
		}
		if (slot != -1){
			int tIndex = -1;
			for (int i = 0; i < MAX_TURRETS; i++) if (!turrets[i].placed){ tIndex = i; break; }
			if (tIndex == -1 && turretCount < MAX_TURRETS) tIndex = turretCount;
			if (tIndex != -1) {
				turrets[tIndex].x = slotX3[slot] - (TOWER_W[selectedTowerType] / 2);
				turrets[tIndex].y = slotY3[slot] - (TOWER_H[selectedTowerType] / 2);
				turrets[tIndex].placed = true;
				turrets[tIndex].fireCooldown = 0;
				turrets[tIndex].type = selectedTowerType;
				turrets[tIndex].slotIndex = slot;
				turrets[tIndex].slotLevel = 3;
				turrets[tIndex].despawnSec = TURRET_LIFETIME_SEC;
				if (tIndex + 1 > turretCount) turretCount = tIndex + 1;
				slotOccupied3[slot] = true;
			}
		}
	}
}


void drawScore() {
	iSetColor(0, 255, 0);
	char scoreText[64];
	sprintf_s(scoreText, "%d pts", score);
	iText(480, Screen_Heigth - 50, scoreText, GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(255, 215, 0);
	char coinText[64];
	sprintf_s(coinText, "%d", coins);
	iText(940, Screen_Heigth - 50, coinText, GLUT_BITMAP_TIMES_ROMAN_24);
	iShowImage(900, Screen_Heigth - 62, 40, 40, coinImg);

	iSetColor(184, 115, 51);
	iRectangle(20, 600, 150, 10);

	float progress = (float)(progressTime - levelTimer) / progressTime;
	if (progress < 0.0f) progress = 0.0f;
	if (progress > 1.0f) progress = 1.0f;

	iSetColor(0, 255, 0);
	iFilledRectangle(20, 600, (int)(150 * progress), 10);
}

void drawTurretLifetimeBar(const Turret &t) {
	if (!t.placed) return;
	float pct = (float)t.despawnSec / (float)TURRET_LIFETIME_SEC;
	if (pct < 0.f) pct = 0.f;
	if (pct > 1.f) pct = 1.f;

	const int barW = 40, barH = 6, gap = 6;
	int bx = t.x + (TOWER_W[t.type] - barW) / 2;
	int by = t.y + TOWER_H[t.type] + gap;

	iSetColor(40, 40, 40);
	iFilledRectangle(bx, by, barW, barH);
	iSetColor(180, 180, 180);
	iRectangle(bx, by, barW, barH);

	if (pct > 0.66f)      iSetColor(0, 200, 0);
	else if (pct > 0.33f) iSetColor(220, 180, 0);
	else                  iSetColor(220, 0, 0);

	int fillW = (int)(barW * pct + 0.5f);
	iFilledRectangle(bx, by, fillW, barH);
}

void drawBossHealthBar(const Boss &b) {
	if (!b.alive || b.maxHp <= 0) return;

	const int barW = bossW;
	const int barH = 8;
	const int gap = 8;

	int bx = b.position_x;
	int by = b.position_y + bossH + gap;
	if (by + barH > Screen_Heigth - 2) by = Screen_Heigth - 2 - barH;

	int cur = b.curHP; if (cur < 0) cur = 0; if (cur > b.maxHp) cur = b.maxHp;
	float pct = (float)cur / (float)b.maxHp;
	if (pct < 0.f) pct = 0.f;
	if (pct > 1.f) pct = 1.f;

	int fillW = (int)(barW * pct + 0.5f);
	if (fillW < 0) fillW = 0;
	if (fillW > barW) fillW = barW;

	iSetColor(40, 40, 40);
	iFilledRectangle(bx, by, barW, barH);
	iSetColor(180, 180, 180);
	iRectangle(bx, by, barW, barH);

	if (pct > 0.66f) iSetColor(0, 200, 0);
	else if (pct > 0.33f) iSetColor(220, 180, 0);
	else iSetColor(220, 0, 0);

	iFilledRectangle(bx, by, fillW, barH);
}

void drawLevel(int level){
	if (level == 3)      iShowBMP2(0, 0, "map//mapF.bmp", 0);
	if (level == 2)      iShowBMP2(0, 0, "map//mapF2.bmp", 0);
	if (level == 1)      iShowBMP2(0, 0, "map//mapF3.bmp", 0);

	if (level == 3){
		for (int i = 0; i < MAX_SLOTS_L1; i++) if (!slotOccupied[i]){ iSetColor(0, 255, 0); iRectangle(slotX[i] - 25, slotY[i] - 25, 50, 50); }
	}
	else if (level == 2){
		for (int i = 0; i < MAX_SLOTS_L2; i++) if (!slotOccupied2[i]){ iSetColor(0, 255, 0); iRectangle(slotX2[i] - 25, slotY2[i] - 25, 50, 50); }
	}
	else if (level == 1){
		for (int i = 0; i < MAX_SLOTS_L1; i++) if (!slotOccupied3[i]){ iSetColor(0, 255, 0); iRectangle(slotX3[i] - 25, slotY3[i] - 25, 50, 50); }
	}

	drawScore();

	if (level == 1) { iShowImage(170, 575, 70, 70, ic1); }
	if (level == 2) { iShowImage(170, 575, 70, 70, ic2); }
	if (level == 3) { iSetColor(0, 255, 0); iCircle(204, 610, 35); }

	char str[20];
	sprintf_s(str, sizeof(str), "%ds", levelTimer);
	iSetColor(0, 255, 0);
	iText(190, 605, str, GLUT_BITMAP_TIMES_ROMAN_24);

	for (int i = 0; i < 5; i++) if (enemy[i].alive)
		iShowImage(enemy[i].position_x, enemy[i].position_y, 30, 30, enemy[i].frames[enemy[i].frameIndex]);

	for (int i = 0; i < MAX_BOSSES; i++) {
		if (bosses[i].alive) {
			iShowImage(bosses[i].position_x, bosses[i].position_y, bossW, bossH, bosses[i].frames[bosses[i].frameIndex]);
			drawBossHealthBar(bosses[i]);
		}
	}

	for (int i = 0; i < MAX_BEAMS; i++) if (beams[i].active)
		iShowImage(beams[i].x, beams[i].y, 10, 10, beams[i].image);

	for (int i = 0; i < turretCount; i++) if (turrets[i].placed){
		if (turrets[i].type == 0)      iShowImage(turrets[i].x, turrets[i].y, TOWER_W[0], TOWER_H[0], turretFrames[currentTurretFrame]);
		else if (turrets[i].type == 1) iShowImage(turrets[i].x, turrets[i].y, TOWER_W[1], TOWER_H[1], turret2Frames[currentTurretFrame]);
		else if (turrets[i].type == 2) iShowImage(turrets[i].x, turrets[i].y, TOWER_W[2], TOWER_H[2], turret3Frames[currentTurretFrame]);
		drawTurretLifetimeBar(turrets[i]);
	}

	iSetColor(255, 0, 0);
	iRectangle(removeBtnX, removeBtnY, removeBtnSize, removeBtnSize);
	iText(removeBtnX + 10, removeBtnY + 15, "DEL", GLUT_BITMAP_HELVETICA_18);

	iShowImage(clickbarX, clickbarY, 50, 50, clickbar);
	if (showbar){
		iSetColor(0, 0, 0); iFilledRectangle(overlayX, overlayY, overlayW, overlayH);
		iSetColor(184, 115, 51); iRectangle(overlayX, overlayY, overlayW, overlayH);

		iSetColor(184, 115, 51); iFilledCircle(overlayX, overlayY, 5);
		iSetColor(184, 115, 51); iFilledCircle(overlayX, overlayY + overlayH, 5);
		iSetColor(184, 115, 51); iFilledCircle(overlayX + overlayW, overlayY + overlayH, 5);
		iSetColor(184, 115, 51); iFilledCircle(overlayX + overlayW, overlayY, 5);

		iShowImage(230, 20, 100, 100, selectionbar);
		iShowImage(253, 50, 60, 70, turretFrames[0]);
		iSetColor(0, 255, 0); iText(265, 30, "FREE", GLUT_BITMAP_HELVETICA_12);

		iShowImage(380, 20, 100, 100, selectionbar);
		iShowImage(403, 50, 60, 70, turret2Frames[0]);
		iSetColor(255, 216, 1); iText(438, 30, "10", GLUT_BITMAP_HELVETICA_12);
		iShowImage(420, 27, 15, 15, coinImg);

		iShowImage(530, 20, 100, 100, selectionbar);
		iShowImage(553, 50, 60, 70, turret3Frames[0]);
		iSetColor(255, 216, 1); iText(580, 30, "30", GLUT_BITMAP_HELVETICA_12);
		iShowImage(560, 27, 15, 15, coinImg);

		iSetColor(255, 255, 255);
		iShowBMP2(closeX, closeY, "bar//cross.bmp", 0);
	}
}

// Options Ui
const int optBoxX = 300, optBoxY = 260, optBoxW = 400, optBoxH = 180;
const int titleMenuX = optBoxX + 30, titleMenuY = optBoxY + 120;
const int titleGameX = optBoxX + 30, titleGameY = optBoxY + 80;
const int titleW = 340, titleH = 28;

void iMouseMove(int mx, int my) { (void)mx; (void)my; }
void iSpecialKeyboard(unsigned char key) { (void)key; }

//  Input 
void iPassiveMouseMove(int mx, int my){
	if (gameState == 0){
		int nh = -1;
		if (inButton(mx, my, btnX, startY, BUTTON_W, BUTTON_H)) nh = 0;
		else if (inButton(mx, my, btnX, levelY, BUTTON_W, BUTTON_H)) nh = 1;
		else if (inButton(mx, my, btnX, creditsY, BUTTON_W, BUTTON_H)) nh = 2;
		else if (inButton(mx, my, btnX, optionsY, BUTTON_W, BUTTON_H)) nh = 3;
		else if (inButton(mx, my, btnX, exitY, BUTTON_W, BUTTON_H)) nh = 4;

		hoveredButton = nh;
		prevHoveredButton = nh;
	}
	else {
		hoveredButton = prevHoveredButton = -1;
	}
}

void iMouse(int button, int state, int mx, int my) {
	if (button == GLUT_LEFT_BUTTON) {

		if (gameState == 0 && state == GLUT_UP) {
			if (inButton(mx, my, btnX, startY, BUTTON_W, BUTTON_H)) { freshStart(1); return; }
			if (inButton(mx, my, btnX, levelY, BUTTON_W, BUTTON_H))  { gameState = 2; updateMusicForState(); return; }
			if (inButton(mx, my, btnX, creditsY, BUTTON_W, BUTTON_H)){ gameState = 6; updateMusicForState(); return; }
			if (inButton(mx, my, btnX, optionsY, BUTTON_W, BUTTON_H)){
				gameState = 5; optionsTab = OPT_IMG1; updateMusicForState(); return;
			}
			if (inButton(mx, my, btnX, exitY, BUTTON_W, BUTTON_H)) {
				updateHighScoreWith(score);
				stopMusic();
				exit(0);
			}
			return;
		}


		if (gameState == 2 && state == GLUT_UP) {
			if (mx >= btnX1 && mx <= btnX1 + btnW && my >= btnY1 && my <= btnY1 + btnH) { freshStart(1); return; }
			if (mx >= btnX2 && mx <= btnX2 + btnW && my >= btnY2 && my <= btnY2 + btnH) { freshStart(2); return; }
			if (mx >= btnX3 && mx <= btnX3 + btnW && my >= btnY3 && my <= btnY3 + btnH) { freshStart(3); return; }
		}


		if (gameState == levelComplete && state == GLUT_UP){
			if (mx >= YesX && mx <= YesX + lcW && my >= YesY && my <= YesY + lcH){// Yes -> next level
				if (justFinishedLevel == 1)      freshStart(2);
				else if (justFinishedLevel == 2) freshStart(3);
				else                              freshStart(1);
				justFinishedLevel = 0;
				return;
			}
			if (mx >= NoX&& mx <= NoX + lcW && my >= NoY && my <= NoY + lcH){
				updateHighScoreWith(score);
				gameState = 0;
				coins = 0; score = 0; displayScore = 0; enemiesPassed = 0;
				speedMultiplier = 1.0f; lastSpeedTier = 0;
				resetTurrets(); resetBeams(); resetEnemies(); resetBosses();
				updateMusicForState();
			}
		}


		if (gameState == 5 && state == GLUT_UP) {

			if (optionsTab == OPT_MUSIC){
				if (mx >= titleMenuX && mx <= titleMenuX + titleW && my >= titleMenuY && my <= titleMenuY + titleH) {
					menuBgmEnabled = !menuBgmEnabled; updateMusicForState(); return;
				}
				if (mx >= titleGameX && mx <= titleGameX + titleW && my >= titleGameY && my <= titleGameY + titleH) {
					gameBgmEnabled = !gameBgmEnabled; updateMusicForState(); return;
				}
			}
			// BACK button removed: no click handler needed
		}


		if ((gameState == level1 || gameState == level2 || gameState == level3) && state == GLUT_DOWN) {
			if (mx >= removeBtnX && mx <= removeBtnX + removeBtnSize &&
				my >= removeBtnY && my <= removeBtnY + removeBtnSize) {
				removeMode = 1; place = 0; return;
			}
			if (mx >= clickbarX && mx <= clickbarX + 50 && my >= clickbarY && my <= clickbarY + 50) {
				showbar = 1; return;
			}

			if (showbar) {
				if (mx >= closeX && mx <= closeX + 20 && my >= closeY && my <= closeY + 20) { showbar = 0; return; }
				if (mx >= 230 && mx <= 330 && my >= 20 && my <= 120) { selectedTowerType = 0; place = 1; showbar = 0; return; }
				if (mx >= 380 && mx <= 480 && my >= 20 && my <= 120) {
					if (coins >= 10) { coins -= 10; selectedTowerType = 1; place = 1; showbar = 0; } return;
				}
				if (mx >= 530 && mx <= 630 && my >= 20 && my <= 120) {
					if (coins >= 30) { coins -= 30; selectedTowerType = 2; place = 1; showbar = 0; } return;
				}
			}
			if (place) { placeTurretAtSlot(mx, my); place = 0; }
			if (removeMode) { removeTurretAt(mx, my); }
		}
	}
}

void iKeyboard(unsigned char key){

	if (gameState == 5){
		if (key == 't'){ optionsTab = OPT_IMG2; return; }   // show others//2.bmp
		if (key == 's'){ optionsTab = OPT_HISCORE; return; } // show High Score
		if (key == 'y'){ optionsTab = OPT_MUSIC; return; }   // show Music settings
		if (key == '1'){ optionsTab = OPT_IMG1; return; }    // back to others//1.bmp
	}
	//if (key = 'p') iShowBMP(0, 0, "tuto2.bmp");

	if (key == 'h'){
		updateHighScoreWith(score);
		gameState = 0;
		coins = 0; score = 0; displayScore = 0; enemiesPassed = 0;
		speedMultiplier = 1.0f; lastSpeedTier = 0;
		resetTurrets(); resetBeams(); resetEnemies();
		updateMusicForState();
	}
	if (gameState == 4 && key == 'm'){
		updateHighScoreWith(score);
		gameState = 0;
		coins = 0; score = 0; displayScore = 0; enemiesPassed = 0;
		speedMultiplier = 1.0f; lastSpeedTier = 0;
		resetTurrets(); resetBeams(); resetEnemies();
		updateMusicForState();
	}
}

void iDraw(){
	iClear();
	if (gameState == 0){
		iShowBMP(0, 0, "homepage//h2.bmp");
		if (hoveredButton == 0) iShowImage(btnX, startY, BUTTON_W, BUTTON_H, hoverStartImg);
		else if (hoveredButton == 1) iShowImage(btnX, levelY - 7, BUTTON_W, BUTTON_H, hoverLevelImg);
		else if (hoveredButton == 2) iShowImage(btnX, creditsY - 15, BUTTON_W, BUTTON_H, hoverCreditsImg);
		else if (hoveredButton == 3) iShowImage(btnX, optionsY - 21, BUTTON_W, BUTTON_H, hoverOptionImg);
		else if (hoveredButton == 4) iShowImage(btnX, exitY - 27, BUTTON_W, BUTTON_H, hoverExitImg);
	}
	else if (gameState == level1){ drawLevel(1); }
	else if (gameState == level2){ drawLevel(2); }
	else if (gameState == level3){ drawLevel(3); }
	else if (gameState == 2){
		iShowBMP(0, 0, "Level//level.bmp");
		iShowBMP(62, 300, "Level//l1.bmp");
		iShowBMP(374, 300, "Level//l22.bmp");
		iShowBMP(686, 300, "Level//l33.bmp");
		iShowBMP(112, 250, "Level//ll1.bmp");
		iShowBMP(424, 250, "Level//ll2.bmp");
		iShowBMP(736, 250, "Level//ll3.bmp");
	}
	else if (gameState == levelComplete) {
		iShowBMP(0, 0, "map//lc.bmp");
	}
	else if (gameState == 5){
		// Tab title + hints
		iSetColor(255, 255, 255);
		iText(380, 620, "OPTIONS", GLUT_BITMAP_TIMES_ROMAN_24);
		iSetColor(200, 200, 200);
		iText(280, 590, "Press:  [1] Options   [t] Tutorial   [s] High Score   [y] Music Settings", GLUT_BITMAP_HELVETICA_18);

		if (optionsTab == OPT_IMG1){
			iShowBMP(0, 0, "others//1.bmp");
		}
		else if (optionsTab == OPT_IMG2){
			iShowBMP(0, 0, "others//2.bmp");
		}
		else if (optionsTab == OPT_HISCORE){
			iSetColor(10, 10, 10);  iFilledRectangle(optBoxX + 4, optBoxY - 4, optBoxW, optBoxH);
			iSetColor(20, 20, 20);  iFilledRectangle(optBoxX, optBoxY, optBoxW, optBoxH);
			iSetColor(184, 115, 51); iRectangle(optBoxX, optBoxY, optBoxW, optBoxH);

			iSetColor(255, 255, 255);
			char hs[80]; sprintf_s(hs, "High Score: %d", highScore);
			iText(optBoxX + 30, optBoxY + 90, hs, GLUT_BITMAP_TIMES_ROMAN_24);
		}
		else if (optionsTab == OPT_MUSIC){
			iSetColor(10, 10, 10);  iFilledRectangle(optBoxX + 4, optBoxY - 4, optBoxW, optBoxH);
			iSetColor(20, 20, 20);  iFilledRectangle(optBoxX, optBoxY, optBoxW, optBoxH);
			iSetColor(184, 115, 51); iRectangle(optBoxX, optBoxY, optBoxW, optBoxH);

			iSetColor(50, 35, 20); iFilledRectangle(optBoxX, optBoxY + optBoxH - 36, optBoxW, 36);
			iSetColor(255, 215, 160); iText(optBoxX + 14, optBoxY + optBoxH - 26, "Sound Settings (ON/OFF only)", GLUT_BITMAP_HELVETICA_18);

			iSetColor(35, 35, 35); iFilledRectangle(titleMenuX - 8, titleMenuY - 8, titleW + 16, titleH + 16);
			iSetColor(120, 120, 120); iRectangle(titleMenuX - 8, titleMenuY - 8, titleW + 16, titleH + 16);
			iSetColor(255, 255, 255);
			char t1[160]; sprintf_s(t1, "Menu Sound: %s  (click to ON/OFF)", menuBgmEnabled ? "ON" : "OFF");
			iText(titleMenuX, titleMenuY, t1, GLUT_BITMAP_HELVETICA_18);

			iSetColor(35, 35, 35); iFilledRectangle(titleGameX - 8, titleGameY - 8, titleW + 16, titleH + 16);
			iSetColor(120, 120, 120); iRectangle(titleGameX - 8, titleGameY - 8, titleW + 16, titleH + 16);
			iSetColor(255, 255, 255);
			char t2[160]; sprintf_s(t2, "Game Sound: %s  (click to ON/OFF)", gameBgmEnabled ? "ON" : "OFF");
			iText(titleGameX, titleGameY, t2, GLUT_BITMAP_HELVETICA_18);
		}
	}
	else if (gameState == 6){
		iShowBMP(0, 0, "credit//credit.bmp");
	}
	else if (gameState == 4){
		iShowBMP(0, 0, "map//go.bmp");
	}
	else if (gameState == imgAfterL3){
		iShowBMP(0, 0, "others//5.bmp");
	}
}


void loadEnemyImages(){
	for (int i = 0; i < 5; i++){
		for (int f = 0; f < 7; f++){
			char b[64]; sprintf_s(b, "enemy//zombie%d.png", f + 1);
			enemy[i].frames[f] = iLoadImage(b);
		}
		enemy[i].frameIndex = 0;
	}
}

void loadBossImages() {
	for (int i = 0; i < MAX_BOSSES; i++) {
		for (int f = 0; f < 11; f++) {
			char b[64]; sprintf_s(b, "Boss%d//mon%d.png", i + 1, f + 1);
			bosses[i].frames[f] = iLoadImage(b);
		}
		bosses[i].frameIndex = 0;
		bosses[i].alive = false;
		bosses[i].hitCount = 0;
		bosses[i].hp = 3 + i * 2;
		bosses[i].maxHp = 3 + i * 2;
		bosses[i].curHP = bosses[i].maxHp;
		bosses[i].speed = 2;
		bosses[i].current_point = 1;
		bosses[i].spawnLevel = i + 1;
		bosses[i].proceed = true;
		bosses[i].respawnCounter = 0;
	}
}

void loadTurretFrames(){
	for (int i = 0; i < 11; i++) {
		char b[64];
		sprintf_s(b, "turret//t%d.png", i + 1);
		turretFrames[i] = iLoadImage(b);

		sprintf_s(b, "turret2//t%d.png", i + 1);
		turret2Frames[i] = iLoadImage(b);

		sprintf_s(b, "turret3//t%d.png", i + 1);
		turret3Frames[i] = iLoadImage(b);
	}
}

void initBeams(){ resetBeams(); beamImage = iLoadImage("beam//b2.png"); }

void initTurrets(){
	turretCount = 0;
	for (int i = 0; i < MAX_TURRETS; i++){
		turrets[i].placed = false; turrets[i].fireCooldown = 0; turrets[i].type = 0; turrets[i].slotIndex = -1; turrets[i].slotLevel = 0;
	}
}

void initEnemies(){
	for (int i = 0; i < 5; i++){
		enemy[i].alive = false;
		enemy[i].position_x = path_x[0] - 30;
		enemy[i].position_y = path_y[0];
		enemy[i].speed = 2;
		enemy[i].current_point = 1;
		enemy[i].frameIndex = 0;
	}
	resetEnemies();
}

void gameInit(){
	loadEnemyImages(); loadTurretFrames(); initBeams(); initTurrets(); initEnemies(); loadBossImages();
	score = 0; displayScore = 0; coins = 0; enemiesPassed = 0; speedMultiplier = 1.0f; lastSpeedTier = 0;

	loadHighScore();

	coinImg = iLoadImage("others//cf.png");
	creditImg = iLoadImage("others//Credit.png");

	hoverStartImg = iLoadImage("hover//start2.png");
	hoverLevelImg = iLoadImage("hover//level.png");
	hoverCreditsImg = iLoadImage("hover//credit.png");
	hoverOptionImg = iLoadImage("hover//option.png");
	hoverExitImg = iLoadImage("hover//exit.png");

	clickbar = iLoadImage("bar//cbar2.png");
	selectionbar = iLoadImage("bar//s4.png");


	ic1 = iLoadImage("others//ic1.png");
	ic2 = iLoadImage("others//ic2.png");

	updateMusicForState();
}

void decreaseTimer() {
	if (timerOn && levelTimer > 0) {
		levelTimer--;
		if (levelTimer == 0){
			timerOn = false;
			levelChange();
		}
	}
}

//  Main 
int main(){
	srand((unsigned)time(NULL));
	iInitialize(Screen_Width, Screen_Heigth, "Tower Defense Game");
	gameInit();

	iSetTimer(30, moveEnemy);
	iSetTimer(30, moveBeams);
	iSetTimer(30, checkCollision);
	iSetTimer(100, animateTurret);
	iSetTimer(20, updateDisplayScore);
	iSetTimer(120, animateEnemies);
	iSetTimer(120, animateBosses);
	iSetTimer(5000, checkBossSpawn);
	iSetTimer(1000, decreaseTimer);


	iSetTimer(1000, tickTurretLifetimes);


	iStart();


	stopMusic();
	return 0;
}
