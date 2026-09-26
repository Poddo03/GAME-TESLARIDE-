#ifndef COMMON_H
#define COMMON_H

#define _CRT_SECURE_NO_WARNINGS
#include "iGraphics.h"
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600

// =========================================================================
// GAME STATE MANAGEMENT
// =========================================================================
#define STATE_TITLE     0   // Welcome / Splash Page
#define STATE_MENU      1   // Level Selection Menu
#define STATE_PLAYING   2   // In-Game Driving
#define STATE_GAMEOVER  3   // Game Over Screen
#define STATE_STORY     4   // Comic Cartoon Conversation Screen
#define STATE_VICTORY   5   // Destination Reached (Victory Screen)
#define STATE_GARAGE    6   // Garage Screen to Buy Yellow Rickshaw

int gameState = STATE_TITLE;
int selectedLevel = 1;      // 1: Station-Basabo, 2: Station-Moghbazar, 3: Station-Aust
int hoveredLevel = 1;

// Level Unlock Management
int level2Unlocked = 1;             // Unlocks after collecting 10 coins
int unlockNotificationTimer = 0;    // Timer to display Level 2 Unlocked banner
int level3Unlocked = 0;             // Unlocks using 10 coins
int level3UnlockCost = 10;          // Price: 10 Coins to unlock Level 3
bool musicOn = true;

// Mouse tracker
int mouseX = 0, mouseY = 0;

// Base Rickshaw (Level 1) Animation & Position (Blue Rickshaw - 8 frames)
char rickshaw[8][15] = {
	"char//R1.bmp", "char//R2.bmp", "char//R3.bmp", "char//R4.bmp",
	"char//R5.bmp", "char//R6.bmp", "char//R7.bmp", "char//R8.bmp"
};

// Upgraded Yellow Rickshaw (Level 2 - 9 frames)
char rickshawL2[9][15] = {
	"char\\NR1.bmp", "char\\NR2.bmp", "char\\NR3.bmp",
	"char\\NR4.bmp", "char\\NR5.bmp", "char\\NR6.bmp",
	"char\\NR7.bmp", "char\\NR8.bmp", "char\\NR9.bmp"
};

// Level 3 Rickshaw (9 frames)
char rickshawL3[9][15] = {
	"char\\LR1.bmp", "char\\LR2.bmp", "char\\LR3.bmp",
	"char\\LR4.bmp", "char\\LR5.bmp", "char\\LR6.bmp",
	"char\\LR7.bmp", "char\\LR8.bmp", "char\\LR9.bmp"
};

// ---------- Garage & Rickshaw Purchase System ----------
int ownedRickshaws[3] = { 1, 0, 0 }; // 0: Classic Blue (Default), 1: Deluxe Yellow, 2: Super Green
int equippedRickshaw = 0;           // 0: Blue, 1: Yellow, 2: Green
int yellowRickshawCost = 20;        // Price: 20 Coins
int greenRickshawCost = 30;         // Price: 30 Coins
int hasYellowRickshaw = 0;          // Kept for backward compatibility
int playerBulletsL3 = 2;            // Persistent collected bullets
int totalCoins = 0;                 // Persistent wallet coins across runs

// ---------- Energy Shield Power-Up System ----------
int hasEnergyShield = 0;            // 1 if purchased for next level run
int energyShieldCost = 15;          // Price: 15 Coins
float activeShieldTimer = 0.0f;     // Active shield countdown timer in seconds
#define SHIELD_DURATION 15.0f       // Shield lasts 15.0 seconds

char* getEquippedRickshawFrame(int animIndex) {
	if (equippedRickshaw == 1 && ownedRickshaws[1]) {
		return rickshawL2[animIndex % 9];
	}
	if (equippedRickshaw == 2 && ownedRickshaws[2]) {
		return rickshawL3[animIndex % 9];
	}
	return rickshaw[animIndex % 8];
}

char* getEquippedRickshawPreviewBMP() {
	if (equippedRickshaw == 1 && ownedRickshaws[1]) return rickshawL2[0];
	if (equippedRickshaw == 2 && ownedRickshaws[2]) return rickshawL3[0];
	return rickshaw[0];
}

extern int rickshawXL3, rickshawYL3;

void getPlayerHitbox(float rX, float rY, float* outL, float* outR, float* outB, float* outT) {
	if (equippedRickshaw == 1) { // Yellow
		*outL = rX + 10.0f;
		*outR = rX + 70.0f;
		*outB = rY + 4.0f;
		*outT = rY + 72.0f;
	}
	else if (equippedRickshaw == 2) { // Green
		*outL = rX + 10.0f;
		*outR = rX + 62.0f;
		*outB = rY + 4.0f;
		*outT = rY + 72.0f;
	}
	else { // Blue Default
		*outL = rX + 12.0f;
		*outR = rX + 62.0f;
		*outB = rY + 4.0f;
		*outT = rY + 72.0f;
	}
}

void getPlayerHitboxL1(float rX, float rY, float* outL, float* outB, float* outW, float* outH) {
	if (equippedRickshaw == 1) { // Yellow
		*outL = rX + 10.0f;
		*outB = rY + 4.0f;
		*outW = 56.0f;
		*outH = 54.0f;
	}
	else if (equippedRickshaw == 2) { // Green
		*outL = rX + 10.0f;
		*outB = rY + 4.0f;
		*outW = 48.0f;
		*outH = 54.0f;
	}
	else { // Blue Default
		*outL = rX + 12.0f;
		*outB = rY + 4.0f;
		*outW = 46.0f;
		*outH = 54.0f;
	}
}

float getBulletSpawnXL3() {
	if (equippedRickshaw == 1) return (float)(rickshawXL3 + 40); // Center of Yellow rickshaw
	if (equippedRickshaw == 2) return (float)(rickshawXL3 + 36); // Center of Green rickshaw
	return (float)(rickshawXL3 + 37); // Center of Blue rickshaw
}

float getBulletSpawnYL3() {
	if (equippedRickshaw == 1) return (float)(rickshawYL3 + 68);
	if (equippedRickshaw == 2) return (float)(rickshawYL3 + 72);
	return (float)(rickshawYL3 + 70);
}


// ---------- Story Conversation Progression ----------
int storyStep = 0;                  // 0 to 4 steps of dialogue

// ---------- Timer & Destination Reward System ----------
#define LEVEL_TIME_LIMIT 20.0f      // 20 Seconds to complete Level 1
#define AGREED_FARE 20              // Fixed 20 Coins reward
float levelTimeRemaining = LEVEL_TIME_LIMIT;

// Level 1 Road & Horizon Settings
int horizonY = 380;
float lineOffset = 0.0f;
float baseScrollSpeed = 0.005f;

// Level 1 Speed Controls
float currentSpeed = 1.0f;
float minSpeed = 0.5f;
float maxSpeed = 2.5f;
float acceleration = 0.02f;
int isDriving = 1;

int rickshawCordinateX = 425;
int rickshawCordinateY = 10;
int rickshawIndex = 0;
int animTimer = 0;
int rickshawWidth = 45;
int rickshawHeight = 50;
int obstacleWidth = 65;
int obstacleHeight = 70;

// Level 1 Coins
#define MAX_COINS 4
typedef struct {
	float x, y, speed, radius;
	int active;
} Coin;

Coin coins[MAX_COINS];
int coinSpawnTimer = 0;
int currentRunCoins = 0;

// Level 1 Obstacles
#define MAX_OBSTACLES 3
typedef struct {
	float x, y, speed;
	int type, active;
} Obstacle;

Obstacle obstacles[MAX_OBSTACLES];
int obstacleSpawnTimer = 0;

// =========================================================================
// LEVEL 2 SETUP (EXACT SYSTEM VARIABLES)
// =========================================================================
int horizonYL2 = 300;
float lineOffsetL2 = 0.0f;
float scrollSpeedL2 = 0.02f;
float cloudOffsetL2 = 0.0f;

float baseSpeedL2 = 1.30f;
float currentSpeedL2 = 1.30f;
float maxSpeedL2 = 2.85f; // Boosted top speed when holding UP
float accelRateL2 = 0.02f;
float decelRateL2 = 0.025f;

int playerLivesL2 = 3;
int invulnerableTimerL2 = 0;
int hitFlashTimerL2 = 0;
int isGameOverL2 = 0;
int sfxTimerL2 = 0;

#define MAX_COINS_L2 8
#define NORMAL_COIN_SPAWN_INTERVAL 65
#define SPECIAL_COIN_SPAWN_INTERVAL 200

typedef struct {
	float y, normX;
	int isSpecial;
	float spinAnim;
	int active;
} CoinL2;

CoinL2 coinsL2[MAX_COINS_L2];
int coinCountL2 = 0;
int normalCoinSpawnCounter = 0;
int specialCoinSpawnCounter = 0;

#define THEME_CYCLE_INTERVAL 200
int currentRoadsideTheme = 0;
int themeCycleCounter = 0;

int rickshawXL2 = 400;
int rickshawYL2 = 20;
int rickshawIndexL2 = 0;
int animTimerL2 = 0;

#define MAX_BUSES 4
#define BUS_SPAWN_INTERVAL 130
typedef struct {
	float y, normX, speed;
	int colorTheme, active;
} Bus;

Bus buses[MAX_BUSES];
int busSpawnCounter = 90;
int lastBusColor = -1;

#define MAX_TRAFFIC_RICKSHAWS 3
#define RICKSHAW_SPAWN_INTERVAL 170
typedef struct {
	float y, normX, speed;
	int hoodColor;
	float pedalAnim;
	int active;
} TrafficRickshaw;

TrafficRickshaw otherRickshaws[MAX_TRAFFIC_RICKSHAWS];
int rickshawSpawnCounter = 40;

#define MAX_PEOPLE 2
#define PERSON_SPAWN_INTERVAL 240
typedef struct {
	float y, normX, speed, walkAnim;
	int outfitTheme, active;
} Person;

Person people[MAX_PEOPLE];
int personSpawnCounter = 0;

#define MAX_DOGS 1
#define DOG_SPAWN_INTERVAL 250
typedef struct {
	float y, normX, speed, trotAnim;
	int coatColor, active;
} Dog;

Dog dogs[MAX_DOGS];
int dogSpawnCounter = 0;

#define MAX_ROADSIDE_PROPS 14
typedef struct {
	float y;
	int side, type, variant;
	float lateralOffset;
	int active;
} RoadsideProp;

RoadsideProp roadsideProps[MAX_ROADSIDE_PROPS];

#define MAX_OUTER_PROPS 8
typedef struct {
	float y;
	int side, type, variant;
	float offsetRatio, active;
} OuterProp;

OuterProp outerProps[MAX_OUTER_PROPS];

typedef struct {
	int entityType, index;
	float y;
} SceneEntity;

typedef struct {
	int type, index;
	float y, normX, baseSpeed, currentSpeed;
} ActiveObstacle;

// ---------- Forward Declarations ----------
void drawRect(double x, double y, double width, double height);
void drawFilledEllipse(double cx, double cy, double rx, double ry, int segments);
void resetObstacle(int index);
void initObstacles();
void resetCoin(int index);
void initCoins();
void resetGame();
void initGameObjectsL2();
void updateAudioState();
void playCoinSound();
int checkCollision(float rX, float rY, float rW, float rH, float oX, float oY, float oW, float oH);
int checkAABB(float l1, float r1, float b1, float t1, float l2, float r2, float b2, float t2);
void drawLevel1Scene();
void drawLevel2Scene();
void updateGameL2();
void drawDestinationTimerHUD();
void initGameObjectsL3();
void drawLevel3Scene();
void updateGameL3();

// =========================================================================
// BASIC DRAWING & AUDIO UTILITIES
// =========================================================================
void drawRect(double x, double y, double width, double height) {
	double px[] = { x, x + width, x + width, x };
	double py[] = { y, y, y + height, y + height };
	iFilledPolygon(px, py, 4);
}
void drawFilledEllipse(double cx, double cy, double rx, double ry, int segments) {
	if (rx <= 0.5 || ry <= 0.5) return;
	double px[24], py[24];
	if (segments > 24) segments = 24;
	for (int i = 0; i < segments; i++) {
		double angle = (2.0 * 3.14159265358979323846 * (double)i) / (double)segments;
		px[i] = cx + rx * cos(angle);
		py[i] = cy + ry * sin(angle);
	}
	iFilledPolygon(px, py, segments);
}

// LEVEL 3 SETUP: MONSOON NIGHT DRIVE (STATION - AUST)
// =========================================================================
int horizonYL3 = 300;
float lineOffsetL3 = 0.0f;
float scrollSpeedL3 = 0.02f;

// Dynamic Background Offset Variables
float cloudOffsetL3 = 0.0f;
float riverWaveOffsetL3 = 0.0f;

// Player Speed & Life System Setup
float baseSpeedL3 = 1.30f;       // Moderate cruising speed at startup
float currentSpeedL3 = 1.30f;    // Active vehicle speed
float maxSpeedL3 = 2.95f;        // Boosted top speed when holding UP
float accelRateL3 = 0.02f;       // Gradual acceleration rate
float decelRateL3 = 0.025f;      // Gradual deceleration rate

int playerLivesL3 = 5;           // 5 Lives Total
int invulnerableTimerL3 = 0;     // Post-hit recovery timer (~1.3s invulnerability)
int hitFlashTimerL3 = 0;         // Brief red border flash on collision
int isGameOverL3 = 0;            // Game Over state flag
int sfxTimerL3 = 0;              // Timer to resume background music after sound effects

// Coin Reward System Setup
#define MAX_COINS_L3 8
#define NORMAL_COIN_SPAWN_INTERVAL_L3 65   // Spawns normal coins periodically (~1.3s)
#define SPECIAL_COIN_SPAWN_INTERVAL_L3 200 // Exactly every 4 seconds (200 * 20ms = 4000ms)

typedef struct {
	float y;          // Vertical position along road
	float normX;      // Normalized road position (0.0 to 1.0)
	int isSpecial;    // 0: Normal Gold Coin (+1), 1: Large Powerful Coin (+5)
	float spinAnim;   // 3D spin rotation animation phase
	int active;       // 1 if active, 0 if inactive
} CoinL3;

CoinL3 coinsL3[MAX_COINS_L3];
int coinCountL3 = 0;
int normalCoinSpawnCounterL3 = 0;
int specialCoinSpawnCounterL3 = 0;

// Atmospheric Weather: Night Mode, Continuous Rain & Thunder
#define MAX_RAINDROPS_L3 130
#define THUNDER_INTERVAL_L3 200 // Exactly every 4 seconds (200 * 20ms = 4000ms)

typedef struct {
	float x;
	float y;
	float speed;
	float length;
	float slant;
	int brightness;
} RainDropL3;

RainDropL3 rainDropsL3[MAX_RAINDROPS_L3];
int thunderTimerL3 = 160;
int thunderFlashTimerL3 = 0;
int lightningBoltActiveL3 = 0;
float lightningForkXL3 = 450.0f;

// Road Path System: Highway <-> River Bridge
#define PATH_CYCLE_INTERVAL_L3 240 // 240 * 20ms = 4.8 seconds
int currentPathModeL3 = 0;        // 0: City Highway, 1: River Bridge Crossing
int pathCycleCounterL3 = 0;

// Stone Road Obstacles (Spawn every 4 seconds)
#define MAX_STONES_L3 2
#define STONE_SPAWN_INTERVAL_L3 200 // Exactly every 4 seconds (200 * 20ms = 4000ms)

typedef struct {
	float y;
	float normX;
	float size;
	int variant;
	int active;
} StoneL3;

StoneL3 roadStonesL3[MAX_STONES_L3];
int stoneSpawnCounterL3 = 60;

// Boom Road Obstacles
#define MAX_BOOMS_L3 2
#define BOOM_SPAWN_INTERVAL_L3 180 // Spawns periodically (~3.6 seconds)

typedef struct {
	float y;
	float normX;
	float pulseAnim;
	int active;
} BoomL3;

BoomL3 roadBoomsL3[MAX_BOOMS_L3];
int boomSpawnCounterL3 = 120;

#define MAX_BURSTS_L3 4
typedef struct {
	float x;
	float y;
	int timer;
	int maxTimer;
	float maxRadius;
	int active;
} ExplosionBurstL3;

ExplosionBurstL3 burstsL3[MAX_BURSTS_L3];

// ==========================================
// LEVEL 3 BULLET SYSTEM SETUP
// COLLECTIBLE AMMO PICKUPS & DOUBLE-TAP CANNON
// ==========================================
#define MAX_BULLET_PICKUPS_L3 6
#define BULLET_SPAWN_INTERVAL_L3 175 // Exactly every 3.5 seconds (175 * 20ms = 3500ms)

typedef struct {
	float y;
	float normX;
	float animPhase;
	int active;
} BulletPickupL3;

BulletPickupL3 bulletPickupsL3[MAX_BULLET_PICKUPS_L3];
int bulletSpawnCounterL3 = 80;

#define MAX_THROWN_BULLETS_L3 6
typedef struct {
	float x;
	float y;
	float speed;
	float anim;
	int active;
} ThrownBulletL3;

ThrownBulletL3 thrownBulletsL3[MAX_THROWN_BULLETS_L3];
int doubleTapTimerL3 = 0;
int prevUpStateL3 = 0;

// Rickshaw Player Setup (Level 3)
int rickshawXL3 = 400;
int rickshawYL3 = 20;
int rickshawIndexL3 = 0;
int animTimerL3 = 0;

// Bus Vehicle System Setup (Night Express)
#define MAX_BUSES_L3 4
#define BUS_SPAWN_INTERVAL_L3 130 // Spawns every ~2.6 seconds

typedef struct {
	float y;
	float normX;
	float speed;
	int colorTheme;
	int active;
} BusL3;

BusL3 busesL3[MAX_BUSES_L3];
int busSpawnCounterL3 = 90;
int lastBusColorL3 = -1;

// Traffic Rickshaw Vehicle System Setup
#define MAX_TRAFFIC_RICKSHAWS_L3 3
#define RICKSHAW_SPAWN_INTERVAL_L3 170 // Spawns every ~3.4 seconds

typedef struct {
	float y;
	float normX;
	float speed;
	int hoodColor;
	float pedalAnim;
	int active;
} TrafficRickshawL3;

TrafficRickshawL3 otherRickshawsL3[MAX_TRAFFIC_RICKSHAWS_L3];
int rickshawSpawnCounterL3 = 40;

// Walking Pedestrian System Setup (with Umbrellas)
#define MAX_PEOPLE_L3 3
#define PERSON_SPAWN_INTERVAL_L3 200

typedef struct {
	float y;
	float normX;
	float speed;
	float walkAnim;
	int outfitTheme;
	int umbrellaColor;
	int gender; // 0: Man, 1: Woman
	int active;
} PersonL3;

PersonL3 peopleL3[MAX_PEOPLE_L3];
int personSpawnCounterL3 = 0;

// Road Dog Obstacle System Setup
#define MAX_DOGS_L3 1
#define DOG_SPAWN_INTERVAL_L3 250

typedef struct {
	float y;
	float normX;
	float speed;
	float trotAnim;
	int coatColor;
	int active;
} DogL3;

DogL3 dogsL3[MAX_DOGS_L3];
int dogSpawnCounterL3 = 0;

// Roadside Environment & Props System Setup
#define MAX_ROADSIDE_PROPS_L3 14

typedef struct {
	float y;
	int side;
	int type;    // 0: Tree, 1: Palm, 2: Electrical Pole, 3: Street Lamp, 4: Waste Bin, 5: Electrical Box, 6: Sign, 7: Planter
	int variant;
	float lateralOffset;
	int active;
} RoadsidePropL3;

RoadsidePropL3 roadsidePropsL3[MAX_ROADSIDE_PROPS_L3];

// Roadside Big Stalls & Buildings
#define MAX_OUTER_PROPS_L3 4 // 2 Left Stations, 2 Right Stations

typedef struct {
	float y;           // Fixed vertical position in perspective
	int side;          // 0: Left side, 1: Right side
	int type;          // 0: Bismillah Tea, 1: Fuchka Corner, 2: Al-Madina Fruits, 3: Dhaka Sweets, 4: Model School, 5: Govt College, 6: Roadside Pond, 7: News Kiosk
	int variant;
	float offsetRatio; // Lateral positioning
	int replaceCounter;// Periodic replacement countdown (~3.5 to 4.5s)
	int shopperGender; // 0: Man, 1: Woman shopper at stall
	int active;
} OuterPropL3;

OuterPropL3 outerPropsL3[MAX_OUTER_PROPS_L3];
int sideAnimTimerL3 = 0; // Gentle in-place ambient animation timer

// Unified perspective center X coordinate on the road (L3)

// =========================================================================
// FORWARD DECLARATIONS FOR CROSS-HEADER CALLS
// =========================================================================
void saveUserData();
void loadUserData();
void updateAudioState();
void playCoinSound();
void resetGame();
void drawGameOverScreen();
void drawDestinationTimerHUD();
void drawCustomShieldIcon(float cx, float cy, float scale, bool isGlowing = false);
void activateEnergyShieldInGame();
void drawInGameShieldHUD();
void drawActiveShieldFX(float cx, float cy, float radius);
void drawRealisticBus(double centerX, double baseY, double scale, int colorTheme);
void drawRealisticRickshawL2(double centerX, double baseY, double scale, float pedalAnim, int hoodColor);
void drawRealisticPerson(double centerX, double baseY, double scale, float walkPhase, int outfitTheme);
void drawRealisticDog(double centerX, double baseY, double scale, float trotAnim, int coatColor);
void drawCoinL2(double centerX, double baseY, double scale, int isSpecial, float spinAnim);
void drawLivesHUDL2();
void drawCoinsHUDL2();
void drawRealisticStoneL3(double centerX, double baseY, double scale, int variant);
void drawBoomL3(double centerX, double baseY, double scale, float pulseAnim);
void triggerBoomBurstL3(double x, double y);
void drawExplosionBurstsL3();
void drawRealisticBusL3(double centerX, double baseY, double scale, int colorTheme);
void drawRealisticRickshawL3(double centerX, double baseY, double scale, float pedalAnim, int hoodColor);
void drawRealisticPersonL3(double centerX, double baseY, double scale, float walkPhase, int outfitTheme, int umbrellaColor, int gender);
void drawRealisticDogL3(double centerX, double baseY, double scale, float trotAnim, int coatColor);
void drawCoinL3(double centerX, double baseY, double scale, int isSpecial, float spinAnim);
void drawBulletPickupL3(double centerX, double baseY, double scale, float spinAnim);
void drawPlayerBulletsL3();
void drawRocketBulletL3(double cx, double cy, double scale, float angleDeg, int isFired, float flameAnim);
void drawBulletsHUDL3();
void drawLivesHUDL3();
void drawCoinsHUDL3();

#endif // COMMON_H
