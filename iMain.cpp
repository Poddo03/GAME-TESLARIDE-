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

void saveUserData() {
	FILE* fp = fopen("userdata.txt", "w");
	if (fp != NULL) {
		fprintf(fp, "coins %d\n", totalCoins);
		fprintf(fp, "bullets %d\n", playerBulletsL3);
		fprintf(fp, "has_shield %d\n", hasEnergyShield);
		fprintf(fp, "owned0 %d\n", ownedRickshaws[0]);
		fprintf(fp, "owned1 %d\n", ownedRickshaws[1]);
		fprintf(fp, "owned2 %d\n", ownedRickshaws[2]);
		fprintf(fp, "equipped %d\n", equippedRickshaw);
		fprintf(fp, "level2_unlocked %d\n", level2Unlocked);
		fprintf(fp, "level3_unlocked %d\n", level3Unlocked);
		fclose(fp);
	}
}

void loadUserData() {
	FILE* fp = fopen("userdata.txt", "r");
	if (fp != NULL) {
		char key[64];
		int val;
		while (fscanf(fp, "%63s %d", key, &val) == 2) {
			if (strcmp(key, "coins") == 0) totalCoins = val;
			else if (strcmp(key, "bullets") == 0) playerBulletsL3 = val;
			else if (strcmp(key, "has_shield") == 0) hasEnergyShield = (val != 0) ? 1 : 0;
			else if (strcmp(key, "owned0") == 0) ownedRickshaws[0] = 1;
			else if (strcmp(key, "owned1") == 0) ownedRickshaws[1] = (val != 0) ? 1 : 0;
			else if (strcmp(key, "owned2") == 0) ownedRickshaws[2] = (val != 0) ? 1 : 0;
			else if (strcmp(key, "equipped") == 0) equippedRickshaw = (val >= 0 && val <= 2) ? val : 0;
			else if (strcmp(key, "level2_unlocked") == 0) level2Unlocked = val;
			else if (strcmp(key, "level3_unlocked") == 0) level3Unlocked = val;
		}
		fclose(fp);
	}
	else {
		totalCoins = 0;
		playerBulletsL3 = 2;
		hasEnergyShield = 0;
		ownedRickshaws[0] = 1;
		ownedRickshaws[1] = 0;
		ownedRickshaws[2] = 0;
		equippedRickshaw = 0;
		level2Unlocked = 1;
		level3Unlocked = 0;
		saveUserData();
	}
	ownedRickshaws[0] = 1;
	if (equippedRickshaw < 0 || equippedRickshaw > 2 || !ownedRickshaws[equippedRickshaw]) {
		equippedRickshaw = 0;
	}
	hasYellowRickshaw = ownedRickshaws[1];
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

void drawCustomShieldIcon(float cx, float cy, float scale, bool isGlowing = false) {
	// Outer glowing aura if active/ready
	if (isGlowing) {
		static float glowAnim = 0.0f;
		glowAnim += 0.08f;
		float glowR = 26.0f * scale + (float)sin(glowAnim) * 2.5f * scale;
		iSetColor(0, 200, 255);
		iCircle(cx, cy, glowR);
		iCircle(cx, cy, glowR + 1.5f * scale);
	}

	// 1. Outer Bevel Rim (Heraldic pentagonal shield outline)
	double vxOuter[6] = {
		cx - 20.0 * scale, // Top-Left
		cx,                // Top-Center Dip
		cx + 20.0 * scale, // Top-Right
		cx + 22.0 * scale, // Mid-Right Curve
		cx,                // Bottom Point Tip
		cx - 22.0 * scale  // Mid-Left Curve
	};
	double vyOuter[6] = {
		cy + 18.0 * scale,
		cy + 14.0 * scale,
		cy + 18.0 * scale,
		cy + 1.0 * scale,
		cy - 22.0 * scale,
		cy + 1.0 * scale
	};

	// Outer metallic rim highlight
	iSetColor(140, 235, 255);
	iFilledPolygon(vxOuter, vyOuter, 6);

	// 2. Main Shield Body (Slightly inset)
	double vxBody[6] = {
		cx - 17.0 * scale,
		cx,
		cx + 17.0 * scale,
		cx + 19.0 * scale,
		cx,
		cx - 19.0 * scale
	};
	double vyBody[6] = {
		cy + 15.0 * scale,
		cy + 11.0 * scale,
		cy + 15.0 * scale,
		cy + 1.0 * scale,
		cy - 19.0 * scale,
		cy + 1.0 * scale
	};

	// Dark metallic cyan background fill
	iSetColor(10, 85, 145);
	iFilledPolygon(vxBody, vyBody, 6);

	// Inner Shield Gradient / Light Accent (Top Half)
	double vxLight[6] = {
		cx - 14.0 * scale,
		cx,
		cx + 14.0 * scale,
		cx + 15.0 * scale,
		cx,
		cx - 15.0 * scale
	};
	double vyLight[6] = {
		cy + 13.0 * scale,
		cy + 9.0 * scale,
		cy + 13.0 * scale,
		cy + 2.0 * scale,
		cy - 13.0 * scale,
		cy + 2.0 * scale
	};
	iSetColor(0, 150, 220);
	iFilledPolygon(vxLight, vyLight, 6);

	// Inner Rim Border line
	iSetColor(0, 235, 255);
	iPolygon(vxBody, vyBody, 6);

	// 3. Center 3D Pentagonal Comet Gold Star
	float starCx = cx;
	float starCy = cy + 1.0f * scale;
	float R = 11.0f * scale; // Outer star radius
	float r = 4.6f * scale;  // Inner star radius

	double ox[5], oy[5], ix[5], iy[5];
	const double PI = 3.14159265358979323846;

	for (int i = 0; i < 5; i++) {
		double angleO = (PI / 2.0) + (i * 2.0 * PI / 5.0);
		ox[i] = starCx + R * cos(angleO);
		oy[i] = starCy + R * sin(angleO);

		double angleI = angleO + (PI / 5.0);
		ix[i] = starCx + r * cos(angleI);
		iy[i] = starCy + r * sin(angleI);
	}

	// Draw 5 arms with 3D faceted shading
	for (int i = 0; i < 5; i++) {
		int prevI = (i + 4) % 5;

		// Left Facet (Light Gold)
		double fx1[3] = { starCx, ox[i], ix[prevI] };
		double fy1[3] = { starCy, oy[i], iy[prevI] };
		iSetColor(255, 230, 70);
		iFilledPolygon(fx1, fy1, 3);

		// Right Facet (Deep Amber Shadow)
		double fx2[3] = { starCx, ox[i], ix[i] };
		double fy2[3] = { starCy, oy[i], iy[i] };
		iSetColor(205, 135, 15);
		iFilledPolygon(fx2, fy2, 3);
	}

	// Crisp Star Edge Highlight & Center Specular Dot
	for (int i = 0; i < 5; i++) {
		iSetColor(255, 245, 160);
		iLine(starCx, starCy, ox[i], oy[i]);
	}
	iSetColor(255, 255, 220);
	iFilledCircle(starCx, starCy, 1.8f * scale);
}

void activateEnergyShieldInGame() {
	if (hasEnergyShield && activeShieldTimer <= 0.0f && gameState == STATE_PLAYING) {
		activeShieldTimer = SHIELD_DURATION;
		hasEnergyShield = 0; // Consumed for this run
		saveUserData();
		PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
	}
}

void drawInGameShieldHUD() {
	if (!hasEnergyShield || activeShieldTimer > 0.0f || gameState != STATE_PLAYING) return;

	int btnX = 15;
	int btnY = 485;
	int btnW = 185;
	int btnH = 50;

	// Animated hover pulse
	static float btnPulse = 0.0f;
	btnPulse += 0.08f;
	int borderGlow = (int)(200 + sin(btnPulse) * 55.0f);

	// Dark Glass Panel
	iSetColor(12, 22, 36);
	iFilledRectangle(btnX, btnY, btnW, btnH);

	// Cyan Glowing Border
	iSetColor(0, borderGlow, 255);
	iRectangle(btnX, btnY, btnW, btnH);
	iRectangle(btnX + 1, btnY + 1, btnW - 2, btnH - 2);

	// Draw 3D Star Shield Icon inside button
	drawCustomShieldIcon((float)btnX + 30, (float)btnY + 25, 0.85f, true);

	// Text Labels inside button
	iSetColor(255, 215, 0);
	iText(btnX + 58, btnY + 29, "SHIELD READY!", GLUT_BITMAP_HELVETICA_12);
	iSetColor(0, 220, 255);
	iText(btnX + 58, btnY + 12, "CLICK TO ACTIVATE", GLUT_BITMAP_HELVETICA_10);
}

void drawActiveShieldFX(float cx, float cy, float radius) {
	if (activeShieldTimer <= 0.0f) return;

	// Prominent Top HUD indicator for Energy Shield countdown
	int hudW = 240, hudH = 30;
	int hudX = SCREEN_WIDTH / 2 - hudW / 2;
	int hudY = SCREEN_HEIGHT - 80;

	iSetColor(12, 18, 30);
	iFilledRectangle(hudX, hudY, hudW, hudH);
	iSetColor(0, 230, 255);
	iRectangle(hudX, hudY, hudW, hudH);
	iRectangle(hudX + 1, hudY + 1, hudW - 2, hudH - 2);

	float progress = activeShieldTimer / SHIELD_DURATION;
	if (progress < 0.0f) progress = 0.0f;
	if (progress > 1.0f) progress = 1.0f;
	iSetColor(0, 150, 255);
	iFilledRectangle(hudX + 3, hudY + 3, (int)((hudW - 6) * progress), hudH - 6);

	char sMsg[60];
	sprintf_s(sMsg, sizeof(sMsg), "TITAN SHIELD: %.1fs", activeShieldTimer);
	iSetColor(255, 255, 255);
	iText(hudX + 35, hudY + 9, sMsg, GLUT_BITMAP_HELVETICA_12);
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

void updateAudioState() {
	if (musicOn && gameState == STATE_TITLE) {
		PlaySound("Audios\\logo_sound.wav", NULL, SND_LOOP | SND_ASYNC);
	}
	else if (musicOn && gameState == STATE_PLAYING) {
		if (selectedLevel == 3) {
			PlaySound("Audios\\L3_Music.wav", NULL, SND_LOOP | SND_ASYNC);
		}
		else {
			PlaySound("Audios\\bg.wav", NULL, SND_LOOP | SND_ASYNC);
		}
	}
	else if (gameState == STATE_GAMEOVER) {
		PlaySound("Audios\\Gameover.wav", NULL, SND_ASYNC);
	}
	else if (gameState == STATE_VICTORY) {
		PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
	}
	else {
		PlaySound(NULL, NULL, 0);
	}
}

void playCoinSound() {
	if (musicOn) {
		PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
	}
}

// =========================================================================
// LEVEL 1 FUNCTIONS
// =========================================================================
void resetCoin(int index) {
	coins[index].y = (float)horizonY;
	coins[index].x = 330 + (rand() % 240);
	coins[index].speed = 3.5f;
	coins[index].radius = 12.0f;
	coins[index].active = 0;
}

void initCoins() {
	for (int i = 0; i < MAX_COINS; i++) resetCoin(i);
	coinSpawnTimer = 0;
}

int checkCoinCollection(float rX, float rY, float rW, float rH, float cX, float cY, float cRadius) {
	if (cX + cRadius >= rX && cX - cRadius <= (rX + rW) &&
		cY + cRadius >= rY && cY - cRadius <= (rY + rH)) return 1;
	return 0;
}

void drawCoinL1(Coin c) {
	if (!c.active) return;
	int cx = (int)c.x, cy = (int)c.y, r = (int)c.radius;
	iSetColor(255, 200, 0);   iFilledCircle(cx, cy, r);
	iSetColor(210, 150, 0);   iCircle(cx, cy, r);
	iSetColor(255, 235, 120); iFilledCircle(cx - 2, cy + 2, (int)(r * 0.55));
	iSetColor(180, 110, 0);   iFilledRectangle(cx - 1, cy - (r / 2), 2, r);
}

void resetObstacle(int index) {
	obstacles[index].y = (float)horizonY;
	obstacles[index].x = 320 + (rand() % 260);
	obstacles[index].speed = 3.0f + (rand() % 100) / 50.0f;
	obstacles[index].type = rand() % 4;
	obstacles[index].active = 0;
}

void initObstacles() {
	for (int i = 0; i < MAX_OBSTACLES; i++) resetObstacle(i);
	obstacleSpawnTimer = 0;
}

int checkCollision(float rX, float rY, float rW, float rH, float oX, float oY, float oW, float oH) {
	if (rX <= (oX + oW) && (rX + rW) >= oX && rY <= (oY + oH) && (rY + rH) >= oY) return 1;
	return 0;
}

int checkAABB(float l1, float r1, float b1, float t1, float l2, float r2, float b2, float t2) {
	if (l1 > r2 || r1 < l2 || b1 > t2 || t1 < b2) return 0;
	return 1;
}

// =========================================================================
// BANGLADESHI SHOPFRONTS, LOCAL TONG/TEA STALLS, FUCHKA, & OVERHEAD WIRES
// =========================================================================

// Forward declarations of procedural Level 1 vehicles
void drawBRTCDoubleDeckerBus(double centerX, double baseY, double scale);
void drawWhiteMicrobusL1(double centerX, double baseY, double scale);
void drawYellowTaxiCabL1(double centerX, double baseY, double scale);
void drawDhakaPrivateCarL1(double centerX, double baseY, double scale);

void drawShopAwningL1(double x, double y, double w, double h, int r1, int g1, int b1, int r2, int g2, int b2) {
	int stripes = 6;
	double sw = w / stripes;
	for (int s = 0; s < stripes; s++) {
		if (s % 2 == 0) iSetColor(r1, g1, b1);
		else iSetColor(r2, g2, b2);
		double ax[] = { x + s * sw, x + (s + 1) * sw, x + (s + 1) * sw - sw * 0.15, x + s * sw - sw * 0.15 };
		double ay[] = { y + h, y + h, y, y };
		iFilledPolygon(ax, ay, 4);
		iFilledCircle(x + s * sw + sw * 0.5, y, sw * 0.45);
	}
	iSetColor(30, 30, 35);
	iLine(x, y, x + w, y);
}

// Draw a roadside tea stall ("Tong er Dokan") with kettle, banana bunch, biscuit jars, and benches
void drawTongTeaStallL1(double cx, double cy, double scale, int isLeft) {
	if (scale <= 0.12) return;
	double w = 58.0 * scale;
	double h = 48.0 * scale;
	double x = isLeft ? (cx - w) : cx;
	double y = cy;

	iSetColor(18, 20, 25);
	drawRect(x - 3.0 * scale, y - 2.0 * scale, w + 6.0 * scale, 5.0 * scale);

	iSetColor(115, 75, 45);
	drawRect(x + 2.0 * scale, y, 4.0 * scale, h);
	drawRect(x + w - 6.0 * scale, y, 4.0 * scale, h);

	iSetColor(140, 90, 55);
	drawRect(x + 4.0 * scale, y + 2.0 * scale, w - 8.0 * scale, 18.0 * scale);
	iSetColor(105, 65, 38);
	drawRect(x + 2.0 * scale, y + 18.0 * scale, w - 4.0 * scale, 3.5 * scale);

	iSetColor(220, 230, 240);
	drawRect(x + 6.0 * scale, y + 21.5 * scale, 9.0 * scale, 11.0 * scale);
	drawRect(x + 17.0 * scale, y + 21.5 * scale, 9.0 * scale, 11.0 * scale);
	iSetColor(225, 175, 65);
	iFilledCircle(x + 10.5 * scale, y + 26.0 * scale, 3.0 * scale);
	iFilledCircle(x + 21.5 * scale, y + 26.0 * scale, 3.0 * scale);
	iSetColor(210, 35, 35);
	drawRect(x + 6.0 * scale, y + 32.5 * scale, 9.0 * scale, 2.5 * scale);
	drawRect(x + 17.0 * scale, y + 32.5 * scale, 9.0 * scale, 2.5 * scale);

	iSetColor(210, 215, 225);
	drawRect(x + w - 20.0 * scale, y + 21.5 * scale, 13.0 * scale, 10.0 * scale);
	iSetColor(180, 185, 195);
	drawFilledEllipse(x + w - 13.5 * scale, y + 31.5 * scale, 6.0 * scale, 3.0 * scale, 10);
	iSetColor(40, 42, 48);
	iLine(x + w - 18.0 * scale, y + 32.0 * scale, x + w - 9.0 * scale, y + 32.0 * scale);
	iLine(x + w - 8.0 * scale, y + 25.0 * scale, x + w - 4.0 * scale, y + 29.0 * scale);
	iSetColor(240, 245, 250);
	iFilledCircle(x + w - 6.0 * scale, y + 35.0 * scale, 2.0 * scale);
	iFilledCircle(x + w - 4.0 * scale, y + 39.0 * scale, 2.8 * scale);

	iSetColor(250, 210, 35);
	for (int b = 0; b < 4; b++) {
		iLine(x + 28.0 * scale + b * 2.5 * scale, y + h - 5.0 * scale, x + 29.0 * scale + b * 2.5 * scale, y + h - 14.0 * scale);
	}

	double roofX[] = { x - 5.0 * scale, x + w + 5.0 * scale, x + w + 2.0 * scale, x - 2.0 * scale };
	double roofY[] = { y + h - 4.0 * scale, y + h - 4.0 * scale, y + h + 8.0 * scale, y + h + 8.0 * scale };
	iSetColor(135, 140, 148);
	iFilledPolygon(roofX, roofY, 4);
	iSetColor(165, 170, 180);
	for (double rx = -3.0 * scale; rx <= w + 3.0 * scale; rx += 5.0 * scale) {
		iLine(x + rx, y + h - 4.0 * scale, x + rx - 2.0 * scale, y + h + 8.0 * scale);
	}

	iSetColor(20, 24, 30);
	drawRect(x + 8.0 * scale, y + h + 2.0 * scale, w - 16.0 * scale, 8.0 * scale);
	iSetColor(255, 205, 30);
	iRectangle(x + 8.0 * scale, y + h + 2.0 * scale, w - 16.0 * scale, 8.0 * scale);

	iSetColor(105, 68, 40);
	drawRect(x + (isLeft ? -10.0 * scale : w + 2.0 * scale), y, 12.0 * scale, 10.0 * scale);
}

// Draw a roadside Fuchka / Chotpoti Pushcart Stall with colorful umbrella & stacked fuchkas
void drawFuchkaStallL1(double cx, double cy, double scale, int isLeft) {
	if (scale <= 0.12) return;
	double w = 50.0 * scale;
	double h = 55.0 * scale;
	double x = isLeft ? (cx - w) : cx;
	double y = cy;

	iSetColor(30, 32, 36);
	iFilledCircle(x + 8.0 * scale, y + 8.0 * scale, 8.0 * scale);
	iFilledCircle(x + w - 8.0 * scale, y + 8.0 * scale, 8.0 * scale);
	iSetColor(180, 185, 195);
	iFilledCircle(x + 8.0 * scale, y + 8.0 * scale, 3.0 * scale);
	iFilledCircle(x + w - 8.0 * scale, y + 8.0 * scale, 3.0 * scale);

	iSetColor(160, 45, 35);
	drawRect(x + 2.0 * scale, y + 8.0 * scale, w - 4.0 * scale, 14.0 * scale);

	iSetColor(200, 230, 245);
	drawRect(x + 4.0 * scale, y + 22.0 * scale, w - 8.0 * scale, 18.0 * scale);
	iSetColor(40, 50, 60);
	iRectangle(x + 4.0 * scale, y + 22.0 * scale, w - 8.0 * scale, 18.0 * scale);

	iSetColor(235, 175, 45);
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 5 - row; col++) {
			iFilledCircle(x + 10.0 * scale + col * 7.0 * scale + row * 3.5 * scale, y + 26.0 * scale + row * 5.0 * scale, 2.8 * scale);
		}
	}

	iSetColor(120, 45, 20);
	drawRect(x + 5.0 * scale, y + 17.0 * scale, 9.0 * scale, 6.0 * scale);
	iSetColor(220, 150, 40);
	drawRect(x + w - 16.0 * scale, y + 17.0 * scale, 10.0 * scale, 5.0 * scale);

	iSetColor(180, 185, 195);
	drawRect(cx - 1.5 * scale, y + 22.0 * scale, 3.0 * scale, 30.0 * scale);

	double umbW = 60.0 * scale;
	double umbY = y + h - 5.0 * scale;
	iSetColor(230, 35, 40);
	drawFilledEllipse(cx, umbY, umbW * 0.5, 13.0 * scale, 16);
	iSetColor(255, 215, 30);
	drawFilledEllipse(cx, umbY, umbW * 0.3, 11.0 * scale, 16);
	iSetColor(20, 145, 60);
	drawFilledEllipse(cx, umbY, umbW * 0.12, 9.0 * scale, 16);
}

// Draw Concrete Electric Utility Pole with cross-arms, insulators, and transformer
void drawElectricPoleL1(double x, double y, double scale, int isLeft) {
	if (scale <= 0.10) return;
	double pw = 6.5 * scale;
	double ph = 95.0 * scale;

	iSetColor(115, 120, 128);
	drawRect(x - pw / 2.0, y, pw, ph);
	iSetColor(95, 100, 108);
	drawRect(x - pw / 2.0, y, 2.0 * scale, ph);

	iSetColor(80, 85, 92);
	drawRect(x - pw, y, pw * 2.0, 7.0 * scale);

	double armY1 = y + ph - 12.0 * scale;
	double armY2 = y + ph - 24.0 * scale;
	double armW = 34.0 * scale;
	iSetColor(60, 65, 72);
	drawRect(x - armW / 2.0, armY1, armW, 3.2 * scale);
	drawRect(x - armW * 0.4, armY2, armW * 0.8, 3.0 * scale);

	iSetColor(245, 245, 250);
	drawRect(x - armW * 0.45, armY1 + 3.0 * scale, 3.2 * scale, 4.5 * scale);
	drawRect(x + armW * 0.45 - 3.2 * scale, armY1 + 3.0 * scale, 3.2 * scale, 4.5 * scale);
	drawRect(x - armW * 0.20, armY1 + 3.0 * scale, 3.2 * scale, 4.5 * scale);
	drawRect(x + armW * 0.20 - 3.2 * scale, armY1 + 3.0 * scale, 3.2 * scale, 4.5 * scale);

	if (isLeft) {
		iSetColor(48, 52, 58);
		drawRect(x - 14.0 * scale, y + ph - 42.0 * scale, 12.0 * scale, 18.0 * scale);
		iSetColor(85, 90, 98);
		drawRect(x - 13.0 * scale, y + ph - 41.0 * scale, 10.0 * scale, 2.0 * scale);
		drawRect(x - 13.0 * scale, y + ph - 35.0 * scale, 10.0 * scale, 2.0 * scale);
	}
	else {
		iSetColor(48, 52, 58);
		drawRect(x + 2.0 * scale, y + ph - 42.0 * scale, 12.0 * scale, 18.0 * scale);
		iSetColor(85, 90, 98);
		drawRect(x + 3.0 * scale, y + ph - 41.0 * scale, 10.0 * scale, 2.0 * scale);
		drawRect(x + 3.0 * scale, y + ph - 35.0 * scale, 10.0 * scale, 2.0 * scale);
	}

	iSetColor(245, 205, 30);
	drawRect(x - 3.0 * scale, y + 25.0 * scale, 6.0 * scale, 8.0 * scale);
	iSetColor(200, 20, 20);
	iFilledCircle(x, y + 29.0 * scale, 1.6 * scale);
}

// Draw Dhaka's famous tangled overhead power and cable internet wires hanging across the street
void drawOverheadWiresL1() {
	iSetColor(25, 28, 32);
	for (int i = 0; i < 4; i++) {
		float progress = fmod(i * 0.25f + lineOffset, 1.0f);
		if (progress < 0.0f) progress += 1.0f;
		float y = horizonY * (1.0f - progress);
		if (y < 10.0f || y > horizonY - 5.0f) continue;
		float scale = 0.25f + 0.75f * progress;

		float poleLeftX = 250.0f * (1.0f - progress) - 20.0f * scale;
		float poleRightX = (SCREEN_WIDTH - 250.0f * (1.0f - progress)) + 20.0f * scale;
		float poleTopLeftY = y + 83.0f * scale;
		float poleTopRightY = y + 83.0f * scale;

		float midX = (poleLeftX + poleRightX) * 0.5f;
		float midY = (poleTopLeftY + poleTopRightY) * 0.5f - 18.0f * scale;

		iLine(poleLeftX, poleTopLeftY, midX, midY);
		iLine(midX, midY, poleRightX, poleTopRightY);

		float midY2 = midY - 6.0f * scale;
		iLine(poleLeftX, poleTopLeftY - 10.0f * scale, midX, midY2);
		iLine(midX, midY2, poleRightX, poleTopRightY - 10.0f * scale);
	}
}

// Upgrade drawSideBuildings to render realistic Bangladeshi commercial & residential shopfronts
void drawSideBuildings() {
	const char* shopNamesLeft[] = { "BASABO STORE", "DHAKA PHARMACY", "MUDI DOKAN", "BKASH POINT", "KHAN HARDWARE" };
	const char* shopNamesRight[] = { "BENGAL SWEETS", "ISLAMIA STORE", "TELECOM & IT", "RAHMAN BAKERY", "AL-MADINA" };

	int buildingColors[5][3] = {
		{ 215, 190, 155 },
		{ 185, 100, 80 },
		{ 135, 180, 168 },
		{ 210, 160, 105 },
		{ 180, 185, 195 }
	};

	// 1. LEFT SIDE BUILDINGS & SHOPS
	for (int i = 0; i < 5; i++) {
		float progress = fmod(i * 0.20f + lineOffset, 1.0f);
		if (progress < 0.0f) progress += 1.0f;
		float scale = 0.30f + 0.70f * progress;
		int bw = (int)(95 * scale);
		int bh = (int)(220 * scale);
		int bx = (int)(250.0f * (1.0f - progress) - bw - 20.0f * scale);
		int by = (int)(horizonY * (1.0f - progress));

		if (bx + bw < -20) continue;

		iSetColor(buildingColors[i % 5][0], buildingColors[i % 5][1], buildingColors[i % 5][2]);
		iFilledRectangle(bx, by, bw, bh);

		iSetColor(buildingColors[i % 5][0] - 30, buildingColors[i % 5][1] - 30, buildingColors[i % 5][2] - 30);
		drawRect(bx + bw - 6.0 * scale, by, 6.0 * scale, bh);

		iSetColor(85, 90, 98);
		drawRect(bx, by + bh, bw, 6.0 * scale);
		iSetColor(24, 28, 36);
		drawRect(bx + 8.0 * scale, by + bh + 6.0 * scale, 18.0 * scale, 14.0 * scale);
		drawFilledEllipse(bx + 17.0 * scale, by + bh + 20.0 * scale, 9.0 * scale, 3.0 * scale, 8);

		for (int floor = 1; floor < 4; floor++) {
			int wy = by + (int)(bh * 0.25f * floor);
			for (int wcol = 0; wcol < 2; wcol++) {
				int wx = bx + (int)(10.0 * scale + wcol * 38.0 * scale);
				iSetColor(20, 24, 32);
				drawRect(wx, wy, 24.0 * scale, 18.0 * scale);
				iSetColor(75, 115, 150);
				drawRect(wx + 2.0 * scale, wy + 2.0 * scale, 20.0 * scale, 14.0 * scale);
				iSetColor(25, 30, 40);
				iLine(wx + 12.0 * scale, wy + 2.0 * scale, wx + 12.0 * scale, wy + 16.0 * scale);
				iLine(wx + 2.0 * scale, wy + 9.0 * scale, wx + 22.0 * scale, wy + 9.0 * scale);
			}
			if (floor == 1) {
				iSetColor(235, 235, 240);
				drawRect(bx + 72.0 * scale, wy - 3.0 * scale, 12.0 * scale, 9.0 * scale);
				iSetColor(40, 45, 52);
				iFilledCircle(bx + 78.0 * scale, wy + 1.5 * scale, 2.5 * scale);
			}
		}

		int shopH = (int)(bh * 0.28f);
		iSetColor(140, 145, 152);
		drawRect(bx + 6.0 * scale, by, bw - 12.0 * scale, shopH);
		iSetColor(100, 105, 112);
		for (double sy = 4.0 * scale; sy < shopH; sy += 5.0 * scale) {
			iLine(bx + 6.0 * scale, by + sy, bx + bw - 6.0 * scale, by + sy);
		}

		iSetColor(25, 28, 35);
		drawRect(bx + 4.0 * scale, by + shopH, bw - 8.0 * scale, 14.0 * scale);
		iSetColor(245, 205, 25);
		iRectangle(bx + 4.0 * scale, by + shopH, bw - 8.0 * scale, 14.0 * scale);
		iSetColor(255, 255, 255);
		if (scale >= 0.55f) {
			iText(bx + 8.0 * scale, by + shopH + 3.0 * scale, (char*)shopNamesLeft[i % 5], GLUT_BITMAP_HELVETICA_10);
		}

		drawShopAwningL1(bx + 2.0 * scale, by + shopH - 8.0 * scale, bw - 4.0 * scale, 8.0 * scale, 215, 35, 45, 245, 245, 245);
	}

	// 2. RIGHT SIDE BUILDINGS & SHOPS
	for (int i = 0; i < 5; i++) {
		float progress = fmod(i * 0.20f + lineOffset, 1.0f);
		if (progress < 0.0f) progress += 1.0f;
		float scale = 0.30f + 0.70f * progress;
		int bw = (int)(95 * scale);
		int bh = (int)(220 * scale);
		int bx = (int)((SCREEN_WIDTH - 250.0f * (1.0f - progress)) + 20.0f * scale);
		int by = (int)(horizonY * (1.0f - progress));

		if (bx > SCREEN_WIDTH + 20) continue;

		iSetColor(buildingColors[(i + 2) % 5][0], buildingColors[(i + 2) % 5][1], buildingColors[(i + 2) % 5][2]);
		iFilledRectangle(bx, by, bw, bh);

		iSetColor(buildingColors[(i + 2) % 5][0] - 30, buildingColors[(i + 2) % 5][1] - 30, buildingColors[(i + 2) % 5][2] - 30);
		drawRect(bx, by, 6.0 * scale, bh);

		iSetColor(85, 90, 98);
		drawRect(bx, by + bh, bw, 6.0 * scale);
		iSetColor(20, 65, 130);
		drawRect(bx + bw - 26.0 * scale, by + bh + 6.0 * scale, 18.0 * scale, 14.0 * scale);

		for (int floor = 1; floor < 4; floor++) {
			int wy = by + (int)(bh * 0.25f * floor);
			for (int wcol = 0; wcol < 2; wcol++) {
				int wx = bx + (int)(10.0 * scale + wcol * 38.0 * scale);
				iSetColor(20, 24, 32);
				drawRect(wx, wy, 24.0 * scale, 18.0 * scale);
				iSetColor(75, 115, 150);
				drawRect(wx + 2.0 * scale, wy + 2.0 * scale, 20.0 * scale, 14.0 * scale);
				iSetColor(25, 30, 40);
				iLine(wx + 12.0 * scale, wy + 2.0 * scale, wx + 12.0 * scale, wy + 16.0 * scale);
				iLine(wx + 2.0 * scale, wy + 9.0 * scale, wx + 22.0 * scale, wy + 9.0 * scale);
			}
		}

		int shopH = (int)(bh * 0.28f);
		iSetColor(140, 145, 152);
		drawRect(bx + 6.0 * scale, by, bw - 12.0 * scale, shopH);
		iSetColor(100, 105, 112);
		for (double sy = 4.0 * scale; sy < shopH; sy += 5.0 * scale) {
			iLine(bx + 6.0 * scale, by + sy, bx + bw - 6.0 * scale, by + sy);
		}

		iSetColor(20, 75, 45);
		drawRect(bx + 4.0 * scale, by + shopH, bw - 8.0 * scale, 14.0 * scale);
		iSetColor(245, 245, 245);
		iRectangle(bx + 4.0 * scale, by + shopH, bw - 8.0 * scale, 14.0 * scale);
		iSetColor(255, 255, 255);
		if (scale >= 0.55f) {
			iText(bx + 8.0 * scale, by + shopH + 3.0 * scale, (char*)shopNamesRight[i % 5], GLUT_BITMAP_HELVETICA_10);
		}

		drawShopAwningL1(bx + 2.0 * scale, by + shopH - 8.0 * scale, bw - 4.0 * scale, 8.0 * scale, 20, 135, 60, 245, 245, 245);
	}
}

// Upgrade drawSideTreesAndPoles to render electric poles, Tong tea stalls, fuchka carts, and roadside trees
void drawSideTreesAndPoles() {
	for (int i = 0; i < 5; i++) {
		float progress = fmod(i * 0.25f + lineOffset, 1.0f);
		if (progress < 0.0f) progress += 1.0f;
		float scale = 0.30f + 0.70f * progress;
		float sidewalkX = 250.0f * (1.0f - progress) - 15.0f * scale;
		float y = horizonY * (1.0f - progress);

		if (i % 3 == 0) {
			drawElectricPoleL1(sidewalkX - 5.0 * scale, y, scale, 1);
		}
		else if (i % 3 == 1) {
			drawTongTeaStallL1(sidewalkX - 8.0 * scale, y, scale, 1);
		}
		else {
			drawFuchkaStallL1(sidewalkX - 8.0 * scale, y, scale, 1);
		}
	}

	for (int i = 0; i < 5; i++) {
		float progress = fmod(i * 0.25f + lineOffset, 1.0f);
		if (progress < 0.0f) progress += 1.0f;
		float scale = 0.30f + 0.70f * progress;
		float sidewalkX = (SCREEN_WIDTH - 250.0f * (1.0f - progress)) + 15.0f * scale;
		float y = horizonY * (1.0f - progress);

		if (i % 3 == 0) {
			drawElectricPoleL1(sidewalkX + 5.0 * scale, y, scale, 0);
		}
		else if (i % 3 == 1) {
			drawTongTeaStallL1(sidewalkX + 8.0 * scale, y, scale, 0);
		}
		else {
			int trunkW = (int)(8 * scale);
			int treeH = (int)(45 * scale);
			iSetColor(105, 68, 38);
			drawRect(sidewalkX + 10.0 * scale, y, trunkW, treeH);
			iSetColor(32, 138, 48);
			iFilledCircle(sidewalkX + 10.0 * scale + trunkW / 2.0, y + treeH, 22.0 * scale);
			iSetColor(45, 168, 58);
			iFilledCircle(sidewalkX + 8.0 * scale, y + treeH + 6.0 * scale, 16.0 * scale);
		}
	}

	drawOverheadWiresL1();
}

// Upgrade drawLevel1Scene to render realistic Bangladesh road environment (Station - Basabo)
void drawLevel1Scene() {
	// 1. Atmospheric Dhaka daylight sky gradient
	for (int y = horizonY; y < SCREEN_HEIGHT; y += 4) {
		float ratio = (float)(y - horizonY) / (float)(SCREEN_HEIGHT - horizonY);
		int skyR = (int)(185 - ratio * 85);
		int skyG = (int)(215 - ratio * 45);
		int skyB = (int)(242 - ratio * 20);
		iSetColor(skyR, skyG, skyB);
		iFilledRectangle(0, y, SCREEN_WIDTH, 5);
	}

	// 2. Warm Sun & Halo
	iSetColor(255, 245, 200);
	iFilledCircle(740, 530, 40);
	iSetColor(255, 215, 60);
	iFilledCircle(740, 530, 28);
	iSetColor(255, 255, 255);
	iFilledCircle(740, 530, 16);

	// 3. Distant Dhaka City Skyline Silhouette (Basabo area apartment blocks & towers)
	iSetColor(140, 165, 185);
	int skylineData[12][3] = {
		{ 0, 45, 110 }, { 90, 65, 80 }, { 160, 50, 95 }, { 245, 75, 70 },
		{ 305, 40, 120 }, { 415, 60, 85 }, { 490, 80, 65 }, { 545, 55, 100 },
		{ 635, 70, 75 }, { 700, 45, 110 }, { 795, 65, 60 }, { 845, 50, 90 }
	};
	for (int b = 0; b < 12; b++) {
		iFilledRectangle(skylineData[b][0], horizonY, skylineData[b][1], skylineData[b][2]);
		iFilledRectangle(skylineData[b][0] + 8, horizonY + skylineData[b][2], 12, 8);
	}
	iSetColor(115, 140, 160);
	drawRect(285, horizonY, 5, 105);
	drawRect(670, horizonY, 5, 115);
	iSetColor(240, 40, 40);
	iFilledCircle(287, horizonY + 105, 2.5);
	iFilledCircle(672, horizonY + 115, 2.5);

	// 4. Sidewalk & Roadside Verge Ground
	iSetColor(110, 138, 92);
	iFilledRectangle(0, 0, SCREEN_WIDTH, horizonY);

	double leftWalkX[] = { 0, 250, 235, 0 };
	double leftWalkY[] = { 0, (double)horizonY, (double)horizonY, 0 };
	iSetColor(155, 150, 142);
	iFilledPolygon(leftWalkX, leftWalkY, 4);

	double rightWalkX[] = { (double)SCREEN_WIDTH, 650, 665, (double)SCREEN_WIDTH };
	double rightWalkY[] = { 0, (double)horizonY, (double)horizonY, 0 };
	iSetColor(155, 150, 142);
	iFilledPolygon(rightWalkX, rightWalkY, 4);

	// 5. Procedural Roadside Buildings, Stalls, Trees, and Overhead Wires
	drawSideBuildings();
	drawSideTreesAndPoles();

	// 6. Realistic Asphalt Road Surface
	double roadX[] = { 250, 650, (double)SCREEN_WIDTH, 0 };
	double roadY[] = { (double)horizonY, (double)horizonY, 0, 0 };
	iSetColor(48, 50, 54);
	iFilledPolygon(roadX, roadY, 4);

	// Asphalt tire tracks & wear grain
	iSetColor(42, 44, 48);
	double leftTrackX[] = { 320, 390, 280, 120 };
	double leftTrackY[] = { (double)horizonY, (double)horizonY, 0, 0 };
	iFilledPolygon(leftTrackX, leftTrackY, 4);

	double rightTrackX[] = { 510, 580, 780, 620 };
	double rightTrackY[] = { (double)horizonY, (double)horizonY, 0, 0 };
	iFilledPolygon(rightTrackX, rightTrackY, 4);

	// 7. Standard Bangladeshi Curbs: Alternating Black & Yellow / White Blocks
	int numCurbSegments = 16;
	for (int k = 0; k < numCurbSegments; k++) {
		float t1 = (float)k / numCurbSegments;
		float t2 = (float)(k + 1) / numCurbSegments;
		double y1 = horizonY * (1.0f - t1);
		double y2 = horizonY * (1.0f - t2);

		double x1_outer = 240.0 * (1.0f - t1);
		double x1_inner = 252.0 * (1.0f - t1);
		double x2_outer = 240.0 * (1.0f - t2);
		double x2_inner = 252.0 * (1.0f - t2);

		int curbCycle = ((int)((t1 + lineOffset * 2.0f) * 16)) % 2;
		if (curbCycle == 0) iSetColor(245, 215, 25);
		else iSetColor(30, 32, 36);

		double lCurbX[] = { x1_outer, x1_inner, x2_inner, x2_outer };
		double lCurbY[] = { y1, y1, y2, y2 };
		iFilledPolygon(lCurbX, lCurbY, 4);

		double rx1_inner = SCREEN_WIDTH - 252.0 * (1.0f - t1);
		double rx1_outer = SCREEN_WIDTH - 240.0 * (1.0f - t1);
		double rx2_inner = SCREEN_WIDTH - 252.0 * (1.0f - t2);
		double rx2_outer = SCREEN_WIDTH - 240.0 * (1.0f - t2);

		double rCurbX[] = { rx1_inner, rx1_outer, rx2_outer, rx2_inner };
		double rCurbY[] = { y1, y1, y2, y2 };
		iFilledPolygon(rCurbX, rCurbY, 4);
	}

	// 8. Road Divider / Center Lane Dashed Markings
	iSetColor(255, 215, 20);
	float dashOffset = (float)fmod(lineOffset, 0.20f);
	for (float t = dashOffset; t < 1.0f; t += 0.20f) {
		double currentY = horizonY - (t * horizonY);
		double nextY = horizonY - ((t + 0.10f) * horizonY);
		double currentWidth = 2.0 + (t * 14.0);
		double nextWidth = 2.0 + ((t + 0.10f) * 14.0);

		double lineX[] = {
			SCREEN_WIDTH / 2.0 - currentWidth, SCREEN_WIDTH / 2.0 + currentWidth,
			SCREEN_WIDTH / 2.0 + nextWidth, SCREEN_WIDTH / 2.0 - nextWidth
		};
		double segY[] = { currentY, currentY, nextY, nextY };
		if (nextY >= 0) iFilledPolygon(lineX, segY, 4);
	}
}

void drawTaxi(int vx, int vy) {
	int w = obstacleWidth, i;
	iSetColor(30, 30, 30);
	iFilledRectangle(vx + 6, vy, 12, 9);
	iFilledRectangle(vx + w - 18, vy, 12, 9);
	iSetColor(50, 50, 50);
	iFilledRectangle(vx + 4, vy + 6, w - 8, 8);
	iSetColor(250, 200, 0);
	iFilledRectangle(vx + 4, vy + 14, w - 8, 25);
	double roofX[] = { (double)vx + 4, (double)vx + 12, (double)vx + w - 12, (double)vx + w - 4 };
	double roofY[] = { (double)vy + 39, (double)vy + 62, (double)vy + 62, (double)vy + 39 };
	iFilledPolygon(roofX, roofY, 4);
	iSetColor(40, 50, 65);
	double glassX[] = { (double)vx + 8, (double)vx + 15, (double)vx + w - 15, (double)vx + w - 8 };
	double glassY[] = { (double)vy + 41, (double)vy + 59, (double)vy + 59, (double)vy + 41 };
	iFilledPolygon(glassX, glassY, 4);
	for (i = 0; i < 9; i++) {
		if (i % 2 == 0) iSetColor(20, 20, 20);
		else iSetColor(255, 255, 255);
		iFilledRectangle(vx + 7 + (i * 6), vy + 33, 6, 6);
	}
	iSetColor(245, 245, 245); iFilledRectangle(vx + 22, vy + 17, 20, 8);
	iSetColor(220, 30, 30);    iFilledRectangle(vx + 6, vy + 22, 14, 7);
	iFilledRectangle(vx + w - 20, vy + 22, 14, 7);
	iSetColor(230, 180, 0);   iFilledRectangle(vx - 3, vy + 40, 5, 8);
	iFilledRectangle(vx + w - 2, vy + 40, 5, 8);
	iSetColor(240, 240, 240); iFilledRectangle(vx + 23, vy + 62, 18, 7);
	iSetColor(20, 20, 20);     iRectangle(vx + 23, vy + 62, 18, 7);
}

void drawVan(int vx, int vy) {
	int w = obstacleWidth;
	iSetColor(30, 30, 30);
	iFilledRectangle(vx + 7, vy, 10, 8);
	iFilledRectangle(vx + w - 17, vy, 10, 8);
	iSetColor(100, 105, 110);
	iFilledRectangle(vx + 4, vy + 6, w - 8, 10);
	iSetColor(40, 40, 40);
	iFilledRectangle(vx + 12, vy + 9, 10, 3);
	iFilledRectangle(vx + w - 22, vy + 9, 10, 3);
	iSetColor(230, 230, 230);
	iFilledRectangle(vx + 10, vy + 60, w - 20, 9);
	iSetColor(180, 180, 180);
	iRectangle(vx + 10, vy + 60, w - 20, 9);
	iSetColor(60, 60, 60);
	iFilledRectangle(vx + 7, vy + 57, 3, 6);
	iFilledRectangle(vx + w - 10, vy + 57, 3, 6);
	iSetColor(245, 245, 250);
	iFilledRectangle(vx + 5, vy + 15, w - 10, 43);
	iSetColor(200, 205, 210);
	iRectangle(vx + 5, vy + 15, w - 10, 43);
	iSetColor(70, 80, 90);
	iFilledRectangle(vx + 9, vy + 37, 21, 16);
	iFilledRectangle(vx + w - 30, vy + 37, 21, 16);
	iSetColor(110, 125, 140);
	iFilledRectangle(vx + 11, vy + 39, 17, 12);
	iFilledRectangle(vx + w - 28, vy + 39, 17, 12);
	iSetColor(140, 145, 150);
	iFilledRectangle(vx + (w / 2), vy + 15, 2, 38);
	iSetColor(50, 50, 50);
	iFilledRectangle(vx + (w / 2) + 3, vy + 26, 3, 7);
	iSetColor(240, 140, 0);
	iFilledRectangle(vx + 1, vy + 22, 4, 14);
	iFilledRectangle(vx + w - 5, vy + 22, 4, 14);
	iSetColor(220, 40, 40);
	iFilledRectangle(vx + 1, vy + 16, 4, 6);
	iFilledRectangle(vx + w - 5, vy + 16, 4, 6);
	iSetColor(70, 70, 70);
	iFilledRectangle(vx - 3, vy + 37, 4, 7);
	iFilledRectangle(vx + w - 1, vy + 37, 4, 7);
}

// =========================================================================
// PROCEDURAL BANGLADESHI TRAFFIC VEHICLES (LEVEL 1)
// =========================================================================

// 1. BRTC Double Decker Bus (Ashok Leyland Titan - Signature Red/Green BRTC styling)
void drawBRTCDoubleDeckerBus(double centerX, double baseY, double scale) {
	if (scale <= 0.1) return;

	double baseW = 120.0;
	double baseH = 175.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = centerX - w / 2.0;
	double by = baseY;

	// Ground shadow
	iSetColor(15, 16, 20);
	double shX[] = { centerX - w * 0.55, centerX + w * 0.55, centerX + w * 0.46, centerX - w * 0.46 };
	double shY[] = { by - 3.0 * scale, by - 3.0 * scale, by + 8.0 * scale, by + 8.0 * scale };
	iFilledPolygon(shX, shY, 4);

	// Heavy Bus Dual Rear Wheels & Front Wheels
	iSetColor(22, 22, 25);
	drawRect(bx + 6.0 * scale, by, 18.0 * scale, 22.0 * scale);
	drawRect(bx + w - 24.0 * scale, by, 18.0 * scale, 22.0 * scale);
	iSetColor(65, 70, 78);
	drawRect(bx + 9.5 * scale, by + 4.0 * scale, 11.0 * scale, 14.0 * scale);
	drawRect(bx + w - 20.5 * scale, by + 4.0 * scale, 11.0 * scale, 14.0 * scale);
	iSetColor(20, 20, 24);
	drawRect(bx + 12.5 * scale, by + 8.0 * scale, 5.0 * scale, 6.0 * scale);
	drawRect(bx + w - 17.5 * scale, by + 8.0 * scale, 5.0 * scale, 6.0 * scale);

	// Lower underbody & heavy chassis bar
	iSetColor(32, 34, 40);
	drawRect(bx - 1.0 * scale, by + 7.0 * scale, w + 2.0 * scale, 12.0 * scale);
	iSetColor(55, 60, 68);
	drawRect(bx, by + 17.0 * scale, w, 2.5 * scale);

	// Main Double-Decker Red Body with aerodynamic chamfered roof
	double chamfer = 10.0 * scale;
	double bodyX[] = { bx, bx + w, bx + w, bx + w - chamfer, bx + chamfer, bx };
	double bodyY[] = { by + 10.0 * scale, by + 10.0 * scale, by + h - chamfer, by + h, by + h, by + h - chamfer };
	iSetColor(185, 22, 30); // BRTC Signature Vivid Red
	iFilledPolygon(bodyX, bodyY, 6);

	// Body side shading for 3D curved bus volume
	iSetColor(135, 14, 20);
	drawRect(bx, by + 10.0 * scale, 5.0 * scale, h - 20.0 * scale);
	drawRect(bx + w - 5.0 * scale, by + 10.0 * scale, 5.0 * scale, h - 20.0 * scale);

	// Middle Deck Divider Band (Separating Deck 1 & Deck 2 with authentic BRTC Green Stripe)
	double midY = by + h * 0.48;
	iSetColor(240, 242, 245);
	drawRect(bx, midY, w, 14.0 * scale);
	iSetColor(12, 120, 52); // Bangladesh Flag Green
	drawRect(bx, midY + 3.0 * scale, w, 8.0 * scale);

	// Lower Deck Windshield & Cabin Glass
	double lowWinY = by + 40.0 * scale;
	double lowWinH = midY - lowWinY - 5.0 * scale;
	iSetColor(18, 24, 34);
	drawRect(centerX - w * 0.40, lowWinY, w * 0.80, lowWinH);
	iSetColor(32, 52, 75);
	drawRect(centerX - w * 0.38, lowWinY + 2.0 * scale, w * 0.76, lowWinH - 4.0 * scale);

	// Lower Deck Passengers & Driver Silhouette
	iSetColor(18, 22, 30);
	iFilledCircle(centerX - w * 0.24, lowWinY + lowWinH * 0.45, 4.0 * scale);
	iFilledCircle(centerX + w * 0.24, lowWinY + lowWinH * 0.45, 4.0 * scale);

	// Windshield Wipers
	iSetColor(25, 25, 30);
	iLine(centerX - w * 0.26, lowWinY + 2.0 * scale, centerX - w * 0.14, lowWinY + 12.0 * scale);
	iLine(centerX + w * 0.14, lowWinY + 2.0 * scale, centerX + w * 0.26, lowWinY + 12.0 * scale);

	// Upper Deck Panoramic Windshield / Windows
	double upWinY = midY + 16.0 * scale;
	double upWinH = (by + h - 26.0 * scale) - upWinY;
	iSetColor(18, 24, 34);
	drawRect(centerX - w * 0.42, upWinY, w * 0.84, upWinH);
	// Dual Upper Panes
	iSetColor(28, 48, 70);
	drawRect(centerX - w * 0.40, upWinY + 2.0 * scale, w * 0.38, upWinH - 4.0 * scale);
	drawRect(centerX + w * 0.02, upWinY + 2.0 * scale, w * 0.38, upWinH - 4.0 * scale);

	// Upper Deck Safety Handrails (Vibrant Safety Yellow)
	iSetColor(245, 205, 30);
	drawRect(centerX - w * 0.38, upWinY + upWinH * 0.32, w * 0.34, 2.5 * scale);
	drawRect(centerX + w * 0.04, upWinY + upWinH * 0.32, w * 0.34, 2.5 * scale);

	// Upper Deck Passengers
	iSetColor(15, 20, 26);
	iFilledCircle(centerX - w * 0.30, upWinY + upWinH * 0.55, 4.2 * scale);
	iFilledCircle(centerX - w * 0.12, upWinY + upWinH * 0.55, 4.2 * scale);
	iFilledCircle(centerX + w * 0.12, upWinY + upWinH * 0.55, 4.2 * scale);
	iFilledCircle(centerX + w * 0.30, upWinY + upWinH * 0.55, 4.2 * scale);

	// Rooftop Destination Display Board (Electronic Route Display)
	double destY = by + h - 22.0 * scale;
	iSetColor(22, 25, 32);
	drawRect(centerX - w * 0.36, destY, w * 0.72, 14.0 * scale);
	iSetColor(245, 245, 245);
	drawRect(centerX - w * 0.34, destY + 2.0 * scale, w * 0.68, 10.0 * scale);
	// Route Display Colors: BRTC Red & Green Route Plate
	iSetColor(180, 20, 30);
	drawRect(centerX - w * 0.32, destY + 3.0 * scale, w * 0.22, 8.0 * scale);
	iSetColor(15, 100, 45);
	drawRect(centerX - w * 0.06, destY + 3.0 * scale, w * 0.38, 8.0 * scale);

	// Front Lower Grille & Radiator
	iSetColor(24, 26, 32);
	drawRect(centerX - w * 0.28, by + 26.0 * scale, w * 0.56, 11.0 * scale);
	iSetColor(80, 85, 95);
	drawRect(centerX - w * 0.26, by + 28.0 * scale, w * 0.52, 1.8 * scale);
	drawRect(centerX - w * 0.26, by + 31.0 * scale, w * 0.52, 1.8 * scale);
	drawRect(centerX - w * 0.26, by + 34.0 * scale, w * 0.52, 1.8 * scale);

	// Headlight Clusters & Amber Blinkers
	// Left Headlights
	iSetColor(255, 255, 240);
	drawRect(bx + 6.0 * scale, by + 27.0 * scale, 7.5 * scale, 9.0 * scale);
	iSetColor(250, 160, 20);
	drawRect(bx + 6.0 * scale, by + 37.0 * scale, 7.5 * scale, 6.0 * scale);
	// Right Headlights
	iSetColor(255, 255, 240);
	drawRect(bx + w - 13.5 * scale, by + 27.0 * scale, 7.5 * scale, 9.0 * scale);
	iSetColor(250, 160, 20);
	drawRect(bx + w - 13.5 * scale, by + 37.0 * scale, 7.5 * scale, 6.0 * scale);

	// Bangladeshi Green Metro Number Plate
	iSetColor(14, 115, 50);
	drawRect(centerX - 16.0 * scale, by + 11.0 * scale, 32.0 * scale, 9.0 * scale);
	iSetColor(255, 255, 255);
	drawRect(centerX - 14.5 * scale, by + 12.5 * scale, 29.0 * scale, 6.0 * scale);
	iSetColor(15, 80, 35);
	drawRect(centerX - 11.0 * scale, by + 14.0 * scale, 22.0 * scale, 2.5 * scale);

	// Roof Clearance / Hazard Marker Lights
	iSetColor(250, 160, 20);
	drawRect(bx + 6.0 * scale, by + h - 3.5 * scale, 5.0 * scale, 3.5 * scale);
	drawRect(bx + w - 11.0 * scale, by + h - 3.5 * scale, 5.0 * scale, 3.5 * scale);
	iSetColor(235, 30, 30);
	drawRect(centerX - 4.5 * scale, by + h - 3.5 * scale, 9.0 * scale, 3.5 * scale);

	// Tall Side Mirrors
	iSetColor(40, 44, 52);
	drawRect(bx - 7.0 * scale, by + 42.0 * scale, 7.0 * scale, 18.0 * scale);
	drawRect(bx + w, by + 42.0 * scale, 7.0 * scale, 18.0 * scale);
	iSetColor(170, 195, 220);
	drawRect(bx - 5.5 * scale, by + 43.5 * scale, 4.5 * scale, 15.0 * scale);
	drawRect(bx + w + 1.0 * scale, by + 43.5 * scale, 4.5 * scale, 15.0 * scale);
}

// 2. Bangladeshi Green CNG Auto-Rickshaw (Bajaj RE 4S with iconic green wire mesh & canopy)
void drawCNGAutoRickshawL1(double centerX, double baseY, double scale) {
	if (scale <= 0.08) return;

	double baseW = 84.0;
	double baseH = 92.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = centerX - w / 2.0;
	double by = baseY;

	// Ground shadow underneath
	iSetColor(18, 20, 24);
	double shX[] = { centerX - w * 0.52, centerX + w * 0.52, centerX + w * 0.44, centerX - w * 0.44 };
	double shY[] = { by - 2.5 * scale, by - 2.5 * scale, by + 6.0 * scale, by + 6.0 * scale };
	iFilledPolygon(shX, shY, 4);

	// Rear wheels visible slightly at bottom sides
	iSetColor(24, 25, 28);
	drawRect(bx + 2.0 * scale, by + 2.0 * scale, 10.0 * scale, 16.0 * scale);
	drawRect(bx + w - 12.0 * scale, by + 2.0 * scale, 10.0 * scale, 16.0 * scale);
	iSetColor(70, 75, 82);
	drawRect(bx + 4.0 * scale, by + 5.0 * scale, 6.0 * scale, 10.0 * scale);
	drawRect(bx + w - 10.0 * scale, by + 5.0 * scale, 6.0 * scale, 10.0 * scale);

	// Front single center wheel & suspension fork
	double fwW = 12.0 * scale;
	double fwH = 20.0 * scale;
	iSetColor(22, 24, 26);
	drawRect(centerX - fwW / 2.0, by, fwW, fwH);
	iSetColor(75, 80, 88);
	drawRect(centerX - 3.5 * scale, by + 4.0 * scale, 7.0 * scale, 12.0 * scale);
	iSetColor(180, 185, 195);
	drawRect(centerX - 1.5 * scale, by + 7.5 * scale, 3.0 * scale, 5.0 * scale);

	// Front suspension shock absorber springs (signature red & silver dual coil springs)
	iSetColor(190, 35, 35);
	drawRect(centerX - 9.0 * scale, by + 10.0 * scale, 3.5 * scale, 14.0 * scale);
	drawRect(centerX + 5.5 * scale, by + 10.0 * scale, 3.5 * scale, 14.0 * scale);
	iSetColor(240, 240, 245);
	for (int sp = 0; sp < 4; sp++) {
		iLine(centerX - 9.0 * scale, by + (11.0 + sp * 3.0) * scale, centerX - 5.5 * scale, by + (12.0 + sp * 3.0) * scale);
		iLine(centerX + 5.5 * scale, by + (11.0 + sp * 3.0) * scale, centerX + 9.0 * scale, by + (12.0 + sp * 3.0) * scale);
	}

	// Front Mudguard / Fender (CNG Green)
	iSetColor(16, 135, 58);
	double fndX[] = { centerX - 9.0 * scale, centerX + 9.0 * scale, centerX + 7.5 * scale, centerX - 7.5 * scale };
	double fndY[] = { by + 16.0 * scale, by + 16.0 * scale, by + 25.0 * scale, by + 25.0 * scale };
	iFilledPolygon(fndX, fndY, 4);

	// Chrome Crash Guard Tubular Bumper Rails (Wraps around bottom front)
	iSetColor(180, 190, 202);
	drawRect(bx - 2.5 * scale, by + 8.0 * scale, w + 5.0 * scale, 3.5 * scale);
	drawRect(bx - 2.5 * scale, by + 8.0 * scale, 3.5 * scale, 18.0 * scale);
	drawRect(bx + w - 1.0 * scale, by + 8.0 * scale, 3.5 * scale, 18.0 * scale);
	iSetColor(225, 235, 245);
	drawRect(centerX - w * 0.35, by + 9.0 * scale, w * 0.70, 1.5 * scale);

	// Main CNG Cabin Green Body
	double bodyChamfer = 8.0 * scale;
	double bodyX[] = {
		bx + 3.0 * scale,
		bx + w - 3.0 * scale,
		bx + w - bodyChamfer,
		bx + w - 8.0 * scale,
		bx + 8.0 * scale,
		bx + bodyChamfer
	};
	double bodyY[] = {
		by + 14.0 * scale,
		by + 14.0 * scale,
		by + 52.0 * scale,
		by + h - 8.0 * scale,
		by + h - 8.0 * scale,
		by + 52.0 * scale
	};
	iSetColor(18, 142, 60); // Authentic Dhaka CNG Emerald Green
	iFilledPolygon(bodyX, bodyY, 6);

	// Body 3D shading on sides
	iSetColor(12, 105, 42);
	drawRect(bx + 3.0 * scale, by + 14.0 * scale, 5.0 * scale, 38.0 * scale);
	drawRect(bx + w - 8.0 * scale, by + 14.0 * scale, 5.0 * scale, 38.0 * scale);

	// Iconic Dhaka Reflective Accent Stripes (Yellow & Red)
	iSetColor(245, 205, 25); // Bright Yellow
	drawRect(bx + 4.0 * scale, by + 32.0 * scale, w - 8.0 * scale, 4.0 * scale);
	iSetColor(215, 30, 30);  // Vivid Red
	drawRect(bx + 4.0 * scale, by + 36.0 * scale, w - 8.0 * scale, 2.5 * scale);

	// Traditional CNG Rickshaw Folk Flower Art on Lower Side Panels
	iSetColor(235, 40, 65);
	iFilledCircle(bx + 11.0 * scale, by + 23.0 * scale, 3.2 * scale);
	iFilledCircle(bx + w - 11.0 * scale, by + 23.0 * scale, 3.2 * scale);
	iSetColor(255, 220, 50);
	iFilledCircle(bx + 11.0 * scale, by + 23.0 * scale, 1.4 * scale);
	iFilledCircle(bx + w - 11.0 * scale, by + 23.0 * scale, 1.4 * scale);
	iSetColor(35, 185, 80);
	iLine(bx + 11.0 * scale, by + 23.0 * scale, bx + 11.0 * scale, by + 17.0 * scale);
	iLine(bx + w - 11.0 * scale, by + 23.0 * scale, bx + w - 11.0 * scale, by + 17.0 * scale);

	// Dhaka Metro Green License Plate
	iSetColor(12, 100, 42);
	drawRect(centerX - 13.0 * scale, by + 16.0 * scale, 26.0 * scale, 8.5 * scale);
	iSetColor(255, 255, 255);
	drawRect(centerX - 11.5 * scale, by + 17.5 * scale, 23.0 * scale, 5.5 * scale);
	iSetColor(15, 75, 30);
	drawRect(centerX - 9.0 * scale, by + 19.0 * scale, 18.0 * scale, 2.5 * scale);

	// Front Headlight Cluster with Protective Wire Grille
	double hlW = 16.0 * scale;
	double hlH = 13.0 * scale;
	iSetColor(28, 30, 35);
	drawRect(centerX - hlW / 2.0, by + 26.0 * scale, hlW, hlH);
	iSetColor(255, 255, 230); // Warm bright glass
	drawRect(centerX - hlW / 2.0 + 1.5 * scale, by + 27.5 * scale, hlW - 3.0 * scale, hlH - 3.0 * scale);
	iSetColor(255, 255, 255);
	iFilledCircle(centerX, by + 26.0 * scale + hlH / 2.0, 3.5 * scale);

	// Protective Wire Grill over headlight (authentic Dhaka feature)
	iSetColor(40, 44, 50);
	iLine(centerX - hlW / 2.0, by + 26.0 * scale, centerX + hlW / 2.0, by + 26.0 * scale + hlH);
	iLine(centerX - hlW / 2.0, by + 26.0 * scale + hlH, centerX + hlW / 2.0, by + 26.0 * scale);
	iLine(centerX, by + 26.0 * scale, centerX, by + 26.0 * scale + hlH);

	// Amber Turn Blinkers
	iSetColor(245, 150, 20);
	drawRect(centerX - hlW / 2.0 - 5.5 * scale, by + 28.0 * scale, 4.5 * scale, 7.5 * scale);
	drawRect(centerX + hlW / 2.0 + 1.0 * scale, by + 28.0 * scale, 4.5 * scale, 7.5 * scale);

	// Cabin Interior & Driver Silhouette
	double cabW = w * 0.76;
	double cabH = 40.0 * scale;
	double cabX = centerX - cabW / 2.0;
	double cabY = by + 42.0 * scale;

	iSetColor(20, 24, 30);
	drawRect(cabX, cabY, cabW, cabH);

	// Driver Silhouette seated inside
	iSetColor(16, 20, 26);
	iFilledCircle(centerX, cabY + cabH * 0.42, 5.5 * scale);
	drawRect(centerX - 8.0 * scale, cabY + 4.0 * scale, 16.0 * scale, 12.0 * scale);
	// Handlebar
	iSetColor(140, 145, 155);
	drawRect(centerX - 10.0 * scale, cabY + 8.0 * scale, 20.0 * scale, 2.0 * scale);

	// Front Slanted Windshield Glass
	double gw = w * 0.52;
	double gh = 28.0 * scale;
	double gx = centerX - gw / 2.0;
	double gy = by + 48.0 * scale;

	iSetColor(24, 32, 42); // Black rubber seal
	drawRect(gx - 1.5 * scale, gy - 1.5 * scale, gw + 3.0 * scale, gh + 3.0 * scale);
	iSetColor(48, 88, 125); // Tinted glass
	drawRect(gx, gy, gw, gh);
	iSetColor(115, 175, 220); // Glass specular highlight
	double glX[] = { gx + 3.0 * scale, gx + 12.0 * scale, gx + 8.0 * scale, gx + 2.0 * scale };
	double glY[] = { gy + 2.0 * scale, gy + gh - 2.0 * scale, gy + gh - 2.0 * scale, gy + 2.0 * scale };
	iFilledPolygon(glX, glY, 4);

	// Windshield Wiper
	iSetColor(20, 22, 26);
	iLine(gx + gw * 0.45, gy + 2.0 * scale, gx + gw * 0.72, gy + gh * 0.65);

	// Windshield Header Banner ("সিএনজি" Green/White banner plate)
	iSetColor(12, 105, 45);
	drawRect(gx, gy + gh - 6.0 * scale, gw, 6.0 * scale);
	iSetColor(255, 255, 255);
	drawRect(gx + 4.0 * scale, gy + gh - 4.8 * scale, gw - 8.0 * scale, 3.5 * scale);
	iSetColor(12, 105, 45);
	drawRect(gx + 6.0 * scale, gy + gh - 3.8 * scale, gw - 12.0 * scale, 1.6 * scale);

	// THE SIGNATURE DHAKA SECURITY WIRE MESH / DIAMOND GRILL (Both side doors)
	// Left Side Door Security Grill
	double meshLW = (gx - cabX);
	double meshH = cabH - 2.0 * scale;
	iSetColor(15, 85, 35);
	drawRect(cabX, cabY, meshLW, meshH);
	iSetColor(28, 160, 68); // Green painted steel wire mesh
	iRectangle(cabX, cabY, meshLW, meshH);
	for (double my = 3.0 * scale; my < meshH; my += 5.0 * scale) {
		iLine(cabX, cabY + my, cabX + meshLW, cabY + my);
	}
	for (double mx = 2.5 * scale; mx < meshLW; mx += 4.5 * scale) {
		iLine(cabX + mx, cabY, cabX + mx, cabY + meshH);
	}
	iSetColor(35, 185, 80);
	iLine(cabX, cabY + 4.0 * scale, cabX + meshLW, cabY + meshH - 4.0 * scale);
	iLine(cabX, cabY + meshH - 4.0 * scale, cabX + meshLW, cabY + 4.0 * scale);

	// Right Side Door Security Grill
	double meshRX = gx + gw;
	double meshRW = (cabX + cabW) - meshRX;
	iSetColor(15, 85, 35);
	drawRect(meshRX, cabY, meshRW, meshH);
	iSetColor(28, 160, 68);
	iRectangle(meshRX, cabY, meshRW, meshH);
	for (double my = 3.0 * scale; my < meshH; my += 5.0 * scale) {
		iLine(meshRX, cabY + my, meshRX + meshRW, cabY + my);
	}
	for (double mx = 2.5 * scale; mx < meshRW; mx += 4.5 * scale) {
		iLine(meshRX + mx, cabY, meshRX + mx, cabY + meshH);
	}
	iSetColor(35, 185, 80);
	iLine(meshRX, cabY + 4.0 * scale, meshRX + meshRW, cabY + meshH - 4.0 * scale);
	iLine(meshRX, cabY + meshH - 4.0 * scale, meshRX + meshRW, cabY + 4.0 * scale);

	// Aerodynamic Canopy Roof (Curved Green Canopy)
	double rfX[] = {
		bx + 6.0 * scale,
		bx + w - 6.0 * scale,
		bx + w - 10.0 * scale,
		bx + 10.0 * scale
	};
	double rfY[] = {
		by + h - 14.0 * scale,
		by + h - 14.0 * scale,
		by + h,
		by + h
	};
	iSetColor(14, 110, 48);
	iFilledPolygon(rfX, rfY, 4);

	// Roof top grass/trim accent (iconic Dhaka green eco-top feature)
	iSetColor(25, 155, 65);
	drawRect(bx + 12.0 * scale, by + h - 3.5 * scale, w - 24.0 * scale, 3.5 * scale);
	iSetColor(240, 205, 30);
	drawRect(bx + 8.0 * scale, by + h - 14.0 * scale, w - 16.0 * scale, 2.5 * scale);

	// Slender Side Mirrors
	iSetColor(30, 32, 38);
	iLine(gx, gy + gh * 0.7, bx - 5.0 * scale, gy + gh * 0.7 + 4.0 * scale);
	drawRect(bx - 7.0 * scale, gy + gh * 0.7 - 2.0 * scale, 4.5 * scale, 9.0 * scale);
	iSetColor(180, 205, 230);
	drawRect(bx - 6.0 * scale, gy + gh * 0.7 - 1.0 * scale, 2.8 * scale, 7.0 * scale);

	iSetColor(30, 32, 38);
	iLine(gx + gw, gy + gh * 0.7, bx + w + 5.0 * scale, gy + gh * 0.7 + 4.0 * scale);
	drawRect(bx + w + 2.5 * scale, gy + gh * 0.7 - 2.0 * scale, 4.5 * scale, 9.0 * scale);
	iSetColor(180, 205, 230);
	drawRect(bx + w + 3.2 * scale, gy + gh * 0.7 - 1.0 * scale, 2.8 * scale, 7.0 * scale);
}

// 3. Traditional Bangladeshi Traffic Rickshaw (Code-rendered Dhaka cycle rickshaw with folding hood & art)
void drawTraditionalRickshawL1(double centerX, double baseY, double scale) {
	if (scale <= 0.08) return;

	double baseW = 80.0;
	double baseH = 95.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = centerX - w / 2.0;
	double by = baseY;

	// Ground Shadow
	iSetColor(20, 22, 26);
	double shX[] = { centerX - w * 0.50, centerX + w * 0.50, centerX + w * 0.40, centerX - w * 0.40 };
	double shY[] = { by - 2.0 * scale, by - 2.0 * scale, by + 6.0 * scale, by + 6.0 * scale };
	iFilledPolygon(shX, shY, 4);

	// Large Dual Rear Wheels on Left and Right (Tall Spoked Wheels)
	iSetColor(26, 28, 32);
	drawRect(bx + 2.0 * scale, by + 4.0 * scale, 6.0 * scale, 32.0 * scale);
	iSetColor(190, 195, 205);
	drawRect(bx + 3.5 * scale, by + 8.0 * scale, 3.0 * scale, 24.0 * scale);
	// Decorative Painted Tin Mudguard over Left Wheel
	iSetColor(220, 35, 45); // Red
	drawRect(bx, by + 20.0 * scale, 8.0 * scale, 18.0 * scale);
	iSetColor(245, 205, 30); // Yellow stripe
	drawRect(bx + 1.0 * scale, by + 26.0 * scale, 6.0 * scale, 4.0 * scale);
	iSetColor(20, 145, 60); // Green reflector tip
	drawRect(bx + 1.5 * scale, by + 34.0 * scale, 5.0 * scale, 3.5 * scale);

	// Right Wheel
	iSetColor(26, 28, 32);
	drawRect(bx + w - 8.0 * scale, by + 4.0 * scale, 6.0 * scale, 32.0 * scale);
	iSetColor(190, 195, 205);
	drawRect(bx + w - 6.5 * scale, by + 8.0 * scale, 3.0 * scale, 24.0 * scale);
	// Decorative Painted Tin Mudguard over Right Wheel
	iSetColor(220, 35, 45); // Red
	drawRect(bx + w - 8.0 * scale, by + 20.0 * scale, 8.0 * scale, 18.0 * scale);
	iSetColor(245, 205, 30); // Yellow stripe
	drawRect(bx + w - 7.0 * scale, by + 26.0 * scale, 6.0 * scale, 4.0 * scale);
	iSetColor(20, 145, 60); // Green reflector tip
	drawRect(bx + w - 6.5 * scale, by + 34.0 * scale, 5.0 * scale, 3.5 * scale);

	// Front Center Steering Wheel (Bicycle Wheel)
	double fwW = 5.0 * scale;
	double fwH = 26.0 * scale;
	iSetColor(25, 26, 30);
	drawRect(centerX - fwW / 2.0, by, fwW, fwH);
	iSetColor(180, 185, 195);
	drawRect(centerX - 1.0 * scale, by + 4.0 * scale, 2.0 * scale, 18.0 * scale);

	// Chrome Front Bicycle Fork & Hub
	iSetColor(160, 165, 175);
	drawRect(centerX - 4.5 * scale, by + 12.0 * scale, 2.0 * scale, 16.0 * scale);
	drawRect(centerX + 2.5 * scale, by + 12.0 * scale, 2.0 * scale, 16.0 * scale);
	iFilledCircle(centerX, by + 13.0 * scale, 2.5 * scale);

	// Wooden Footrest Board & Lower Chassis
	double boardW = w * 0.68;
	double boardX = centerX - boardW / 2.0;
	iSetColor(115, 75, 45); // Teak wood board
	drawRect(boardX, by + 16.0 * scale, boardW, 5.0 * scale);
	iSetColor(165, 110, 65);
	drawRect(boardX + 2.0 * scale, by + 19.5 * scale, boardW - 4.0 * scale, 1.5 * scale);

	// Hand-Painted Folk Rickshaw Art Plate (Back/Under seat art board)
	double artW = w * 0.64;
	double artX = centerX - artW / 2.0;
	iSetColor(245, 205, 30); // Golden yellow tin plate
	drawRect(artX, by + 22.0 * scale, artW, 14.0 * scale);
	iSetColor(215, 35, 45);
	iRectangle(artX, by + 22.0 * scale, artW, 14.0 * scale);
	// Painted peacocks/flowers on tin plate
	iSetColor(20, 135, 55);
	drawFilledEllipse(centerX, by + 29.0 * scale, 10.0 * scale, 4.0 * scale, 10);
	iSetColor(235, 40, 45);
	iFilledCircle(centerX, by + 29.0 * scale, 3.2 * scale);
	iSetColor(255, 255, 255);
	iFilledCircle(centerX, by + 29.0 * scale, 1.2 * scale);

	// Traditional Rickshaw Puller (Mama)
	// Plaid Lungi (Lower body)
	iSetColor(30, 95, 165); // Dhaka blue lungi
	drawRect(centerX - 8.0 * scale, by + 26.0 * scale, 16.0 * scale, 18.0 * scale);
	iSetColor(240, 240, 240); // White lungi check pattern
	iLine(centerX - 8.0 * scale, by + 32.0 * scale, centerX + 8.0 * scale, by + 32.0 * scale);
	iLine(centerX - 8.0 * scale, by + 38.0 * scale, centerX + 8.0 * scale, by + 38.0 * scale);
	iLine(centerX - 2.5 * scale, by + 26.0 * scale, centerX - 2.5 * scale, by + 44.0 * scale);
	iLine(centerX + 3.0 * scale, by + 26.0 * scale, centerX + 3.0 * scale, by + 44.0 * scale);

	// Puller Torso (Vest / Shirt)
	iSetColor(245, 245, 248); // White cotton vest
	drawRect(centerX - 9.0 * scale, by + 44.0 * scale, 18.0 * scale, 18.0 * scale);

	// Signature Red & White Checked Gamcha draped over shoulder!
	iSetColor(220, 35, 45);
	double gX[] = { centerX - 8.0 * scale, centerX - 1.0 * scale, centerX + 4.0 * scale, centerX - 3.0 * scale };
	double gY[] = { by + 60.0 * scale, by + 60.0 * scale, by + 44.0 * scale, by + 44.0 * scale };
	iFilledPolygon(gX, gY, 4);

	// Arms holding handlebar
	iSetColor(220, 165, 125);
	drawRect(centerX - 13.0 * scale, by + 46.0 * scale, 5.0 * scale, 12.0 * scale);
	drawRect(centerX + 8.0 * scale, by + 46.0 * scale, 5.0 * scale, 12.0 * scale);

	// Chrome Bicycle Handlebars & Brass Bell
	iSetColor(180, 185, 195);
	drawRect(centerX - 16.0 * scale, by + 45.0 * scale, 32.0 * scale, 2.5 * scale);
	iSetColor(25, 25, 28);
	drawRect(centerX - 17.5 * scale, by + 44.0 * scale, 3.0 * scale, 4.0 * scale);
	drawRect(centerX + 14.5 * scale, by + 44.0 * scale, 3.0 * scale, 4.0 * scale);
	// Brass Bell
	iSetColor(255, 215, 0);
	iFilledCircle(centerX - 10.0 * scale, by + 48.0 * scale, 2.8 * scale);

	// Puller Head & Face
	iSetColor(220, 165, 125);
	iFilledCircle(centerX, by + 67.0 * scale, 6.0 * scale);
	// Puller Gamcha Cap / Hair
	iSetColor(215, 40, 40); // Red tied headband/cap
	drawRect(centerX - 6.2 * scale, by + 69.0 * scale, 12.4 * scale, 5.0 * scale);
	iFilledCircle(centerX, by + 72.0 * scale, 5.5 * scale);
	iSetColor(245, 230, 210);
	iLine(centerX - 6.0 * scale, by + 71.0 * scale, centerX + 6.0 * scale, by + 71.0 * scale);

	// Passenger Seat & Cushions Behind Puller
	double seatW = w * 0.74;
	double seatX = centerX - seatW / 2.0;
	iSetColor(180, 25, 35); // Plush crimson velvet cushion
	drawRect(seatX, by + 36.0 * scale, seatW, 14.0 * scale);
	iSetColor(255, 215, 30);
	drawRect(seatX, by + 49.0 * scale, seatW, 2.0 * scale);

	// Majestic Traditional Folding Rickshaw Hood (Canopy)
	double hoodW = w * 0.78;
	double hoodH = 38.0 * scale;
	double hX = centerX - hoodW / 2.0;
	double hY = by + 56.0 * scale;

	// Dark base hood backing
	iSetColor(20, 25, 30);
	double hBaseX[] = { hX + 2.0 * scale, hX + hoodW - 2.0 * scale, hX + hoodW - 6.0 * scale, hX + 6.0 * scale };
	double hBaseY[] = { hY, hY, hY + hoodH, hY + hoodH };
	iFilledPolygon(hBaseX, hBaseY, 4);

	// Vibrant Scalloped Dhaka Rexine Hood Strips (Vivid Pink, Golden Yellow, Green)
	iSetColor(235, 45, 125); // Vibrant Dhaka hot pink canopy
	double pinkX[] = { hX + 4.0 * scale, hX + hoodW - 4.0 * scale, hX + hoodW - 8.0 * scale, hX + 8.0 * scale };
	double pinkY[] = { hY + 14.0 * scale, hY + 14.0 * scale, hY + hoodH, hY + hoodH };
	iFilledPolygon(pinkX, pinkY, 4);

	// Yellow scalloped band
	iSetColor(250, 210, 30);
	drawRect(hX + 3.0 * scale, by + 64.0 * scale, hoodW - 6.0 * scale, 7.0 * scale);

	// Green lower canopy border
	iSetColor(20, 150, 60);
	drawRect(hX + 2.0 * scale, by + 57.0 * scale, hoodW - 4.0 * scale, 7.0 * scale);

	// Scalloped floral fringe / pom-poms hanging from canopy rim
	for (int sc = 0; sc < 7; sc++) {
		double scX = hX + 4.5 * scale + sc * (hoodW - 9.0 * scale) / 6.0;
		if (sc % 2 == 0) iSetColor(245, 230, 40);
		else iSetColor(235, 45, 125);
		iFilledCircle(scX, by + 57.0 * scale, 2.2 * scale);
	}

	// Chrome Hood Folding Struts / Scissor Ribs
	iSetColor(210, 215, 225);
	iLine(hX + 3.0 * scale, hY, hX + 8.0 * scale, hY + hoodH);
	iLine(hX + 3.0 * scale, hY + 18.0 * scale, hX + hoodW * 0.25, hY + hoodH);
	iLine(hX + hoodW - 3.0 * scale, hY, hX + hoodW - 8.0 * scale, hY + hoodH);
	iLine(hX + hoodW - 3.0 * scale, hY + 18.0 * scale, hX + hoodW * 0.75, hY + hoodH);
}

// 4. White Microbus / HiAce (Dhaka's Iconic White Microbus / Noah Van)
void drawWhiteMicrobusL1(double centerX, double baseY, double scale) {
	if (scale <= 0.1) return;

	double baseW = 96.0;
	double baseH = 110.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = centerX - w / 2.0;
	double by = baseY;

	// Ground shadow
	iSetColor(18, 20, 24);
	double shX[] = { centerX - w * 0.54, centerX + w * 0.54, centerX + w * 0.44, centerX - w * 0.44 };
	double shY[] = { by - 3.0 * scale, by - 3.0 * scale, by + 8.0 * scale, by + 8.0 * scale };
	iFilledPolygon(shX, shY, 4);

	// Tires
	iSetColor(24, 25, 28);
	drawRect(bx + 6.0 * scale, by, 15.0 * scale, 18.0 * scale);
	drawRect(bx + w - 21.0 * scale, by, 15.0 * scale, 18.0 * scale);
	iSetColor(70, 75, 82);
	drawRect(bx + 9.0 * scale, by + 3.0 * scale, 9.0 * scale, 10.0 * scale);
	drawRect(bx + w - 18.0 * scale, by + 3.0 * scale, 9.0 * scale, 10.0 * scale);

	// Lower chassis
	iSetColor(40, 44, 50);
	drawRect(bx + 3.0 * scale, by + 7.0 * scale, w - 6.0 * scale, 8.0 * scale);

	// Main White Aerodynamic Body
	double chamfer = 8.0 * scale;
	double bodyX[] = { bx, bx + w, bx + w, bx + w - chamfer, bx + chamfer, bx };
	double bodyY[] = { by + 10.0 * scale, by + 10.0 * scale, by + h - chamfer, by + h, by + h, by + h - chamfer };
	iSetColor(242, 245, 248);
	iFilledPolygon(bodyX, bodyY, 6);
	iSetColor(195, 200, 210);
	iRectangle(bx, by + 10.0 * scale, w, h - 10.0 * scale);

	// Roof contour
	iSetColor(185, 190, 200);
	drawRect(bx + 8.0 * scale, by + h - 4.5 * scale, w - 16.0 * scale, 3.0 * scale);

	// Large Tinted Windshield
	double gw = w * 0.82;
	double gh = 35.0 * scale;
	double gx = centerX - gw / 2.0;
	double gy = by + 52.0 * scale;
	iSetColor(22, 28, 40);
	drawRect(gx, gy, gw, gh);
	iSetColor(55, 90, 130);
	drawRect(gx + 2.5 * scale, gy + 2.5 * scale, gw - 5.0 * scale, gh - 5.0 * scale);

	// Rear-view mirror inside
	iSetColor(18, 20, 25);
	drawRect(centerX - 4.0 * scale, gy + gh - 6.0 * scale, 8.0 * scale, 4.5 * scale);

	// Passenger silhouettes inside
	iSetColor(25, 35, 50);
	iFilledCircle(centerX - gw * 0.28, gy + 11.0 * scale, 5.5 * scale);
	iFilledCircle(centerX + gw * 0.28, gy + 11.0 * scale, 5.5 * scale);

	// Front Hood & Chrome Grille
	iSetColor(232, 236, 244);
	drawRect(bx + 5.0 * scale, by + 32.0 * scale, w - 10.0 * scale, 20.0 * scale);
	iSetColor(215, 220, 228);
	drawRect(centerX - w * 0.28, by + 34.0 * scale, w * 0.56, 10.0 * scale);
	iSetColor(155, 160, 172);
	iRectangle(centerX - w * 0.28, by + 34.0 * scale, w * 0.56, 10.0 * scale);
	iSetColor(255, 255, 255);
	iFilledCircle(centerX, by + 39.0 * scale, 3.2 * scale);

	// Dual Crisp Headlights & Corner Amber Indicators
	iSetColor(255, 255, 240);
	drawRect(bx + 6.0 * scale, by + 33.0 * scale, 14.0 * scale, 11.0 * scale);
	drawRect(bx + w - 20.0 * scale, by + 33.0 * scale, 14.0 * scale, 11.0 * scale);
	iSetColor(245, 160, 20);
	drawRect(bx + 2.5 * scale, by + 33.0 * scale, 3.5 * scale, 11.0 * scale);
	drawRect(bx + w - 6.0 * scale, by + 33.0 * scale, 3.5 * scale, 11.0 * scale);

	// Front Bumper & Fog Lights
	iSetColor(220, 224, 232);
	drawRect(bx + 3.0 * scale, by + 14.0 * scale, w - 6.0 * scale, 18.0 * scale);
	iSetColor(40, 44, 52);
	drawRect(centerX - w * 0.25, by + 16.0 * scale, w * 0.50, 6.0 * scale);

	iSetColor(255, 255, 200);
	iFilledCircle(bx + 12.0 * scale, by + 20.0 * scale, 3.2 * scale);
	iFilledCircle(bx + w - 12.0 * scale, by + 20.0 * scale, 3.2 * scale);

	// License Plate (Bangladeshi Green Metro Plate)
	iSetColor(12, 115, 48);
	drawRect(centerX - 13.0 * scale, by + 15.5 * scale, 26.0 * scale, 9.0 * scale);
	iSetColor(255, 255, 255);
	drawRect(centerX - 11.5 * scale, by + 17.0 * scale, 23.0 * scale, 6.0 * scale);

	// Side Mirrors
	iSetColor(200, 205, 215);
	drawRect(bx - 6.5 * scale, by + 51.0 * scale, 6.5 * scale, 11.0 * scale);
	drawRect(bx + w, by + 51.0 * scale, 6.5 * scale, 11.0 * scale);
	iSetColor(160, 190, 220);
	drawRect(bx - 5.0 * scale, by + 52.5 * scale, 4.5 * scale, 8.0 * scale);
	drawRect(bx + w + 1.0 * scale, by + 52.5 * scale, 4.5 * scale, 8.0 * scale);
}

// 3. Dhaka Yellow & Black Taxi Cab (Classic Bangladeshi Yellow Cab)
void drawYellowTaxiCabL1(double centerX, double baseY, double scale) {
	if (scale <= 0.1) return;

	double baseW = 90.0;
	double baseH = 82.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = centerX - w / 2.0;
	double by = baseY;

	// Ground shadow
	iSetColor(18, 20, 24);
	double shX[] = { centerX - w * 0.54, centerX + w * 0.54, centerX + w * 0.44, centerX - w * 0.44 };
	double shY[] = { by - 3.0 * scale, by - 3.0 * scale, by + 8.0 * scale, by + 8.0 * scale };
	iFilledPolygon(shX, shY, 4);

	// Tires
	iSetColor(24, 25, 28);
	drawRect(bx + 5.0 * scale, by, 14.0 * scale, 17.0 * scale);
	drawRect(bx + w - 19.0 * scale, by, 14.0 * scale, 17.0 * scale);
	iSetColor(70, 75, 82);
	drawRect(bx + 8.0 * scale, by + 3.0 * scale, 8.0 * scale, 9.0 * scale);
	drawRect(bx + w - 16.0 * scale, by + 3.0 * scale, 8.0 * scale, 9.0 * scale);

	// Lower black skirt
	iSetColor(32, 34, 38);
	drawRect(bx + 2.0 * scale, by + 6.0 * scale, w - 4.0 * scale, 7.0 * scale);

	// Vibrant Taxi Yellow Lower Body
	iSetColor(250, 195, 20);
	drawRect(bx, by + 10.0 * scale, w, 27.0 * scale);

	// Black & White Checkerboard Taxi Stripe
	int checkCount = 8;
	double cw = w / (double)checkCount;
	for (int c = 0; c < checkCount; c++) {
		if (c % 2 == 0) iSetColor(22, 22, 25);
		else iSetColor(250, 250, 250);
		drawRect(bx + c * cw, by + 32.0 * scale, cw, 5.0 * scale);
	}

	// Cabin & Slanted Windshield
	double roofW = w * 0.72;
	double rx = centerX - roofW / 2.0;
	double roofY = by + h - 16.0 * scale;
	double cabinX[] = { bx + 5.0 * scale, bx + w - 5.0 * scale, rx + roofW, rx };
	double cabinY[] = { by + 37.0 * scale, by + 37.0 * scale, roofY, roofY };
	iSetColor(250, 195, 20);
	iFilledPolygon(cabinX, cabinY, 4);

	// Windshield Glass
	double gw = roofW * 0.88;
	double gx = centerX - gw / 2.0;
	double glassX[] = { bx + 9.0 * scale, bx + w - 9.0 * scale, gx + gw, gx };
	double glassY[] = { by + 39.0 * scale, by + 39.0 * scale, roofY - 2.5 * scale, roofY - 2.5 * scale };
	iSetColor(26, 38, 56);
	iFilledPolygon(glassX, glassY, 4);

	// Driver silhouette
	iSetColor(20, 26, 36);
	iFilledCircle(centerX - gw * 0.28, by + 47.0 * scale, 5.5 * scale);

	// Rooftop TAXI Sign
	double signW = 28.0 * scale;
	double signH = 9.0 * scale;
	double signX = centerX - signW / 2.0;
	double signY = roofY;
	iSetColor(35, 38, 44);
	drawRect(centerX - 5.0 * scale, signY, 10.0 * scale, 3.0 * scale);
	iSetColor(255, 255, 255);
	drawRect(signX, signY + 3.0 * scale, signW, signH);
	iSetColor(20, 20, 25);
	iRectangle(signX, signY + 3.0 * scale, signW, signH);
	iSetColor(220, 25, 25);
	drawRect(signX + 4.0 * scale, signY + 5.0 * scale, signW - 8.0 * scale, 4.5 * scale);

	// Headlights & Radiator Grille
	iSetColor(28, 30, 36);
	drawRect(centerX - w * 0.25, by + 19.0 * scale, w * 0.50, 10.0 * scale);
	iSetColor(85, 90, 100);
	for (double gy = by + 21.0 * scale; gy < by + 28.0 * scale; gy += 2.5 * scale) {
		iLine(centerX - w * 0.23, gy, centerX + w * 0.23, gy);
	}

	iSetColor(255, 255, 230);
	drawRect(bx + 5.0 * scale, by + 20.0 * scale, 12.0 * scale, 9.5 * scale);
	drawRect(bx + w - 17.0 * scale, by + 20.0 * scale, 12.0 * scale, 9.5 * scale);
	iSetColor(245, 150, 20);
	drawRect(bx + 2.0 * scale, by + 20.0 * scale, 3.0 * scale, 9.5 * scale);
	drawRect(bx + w - 5.0 * scale, by + 20.0 * scale, 3.0 * scale, 9.5 * scale);

	// Front Bumper & Number Plate
	iSetColor(42, 45, 52);
	drawRect(bx + 3.0 * scale, by + 11.0 * scale, w - 6.0 * scale, 9.0 * scale);
	iSetColor(250, 250, 250);
	drawRect(centerX - 11.0 * scale, by + 12.5 * scale, 22.0 * scale, 6.0 * scale);
	iSetColor(20, 20, 20);
	iRectangle(centerX - 11.0 * scale, by + 12.5 * scale, 22.0 * scale, 6.0 * scale);

	// Side Mirrors
	iSetColor(25, 25, 30);
	drawRect(bx - 5.0 * scale, by + 36.0 * scale, 5.0 * scale, 8.0 * scale);
	drawRect(bx + w, by + 36.0 * scale, 5.0 * scale, 8.0 * scale);
}

// 3. Dhaka Sleek Private Car / Sedan (Toyota Premio / Allion style with metallic finish & tinted glass)
void drawDhakaPrivateCarL1(double centerX, double baseY, double scale) {
	if (scale <= 0.1) return;

	double baseW = 88.0;
	double baseH = 76.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = centerX - w / 2.0;
	double by = baseY;

	// 1. Ground shadow
	iSetColor(18, 20, 24);
	double shX[] = { centerX - w * 0.54, centerX + w * 0.54, centerX + w * 0.44, centerX - w * 0.44 };
	double shY[] = { by - 3.0 * scale, by - 3.0 * scale, by + 7.0 * scale, by + 7.0 * scale };
	iFilledPolygon(shX, shY, 4);

	// 2. Low-profile tires & 5-spoke alloy wheels
	iSetColor(24, 25, 28);
	drawRect(bx + 4.0 * scale, by, 13.0 * scale, 16.0 * scale);
	drawRect(bx + w - 17.0 * scale, by, 13.0 * scale, 16.0 * scale);
	iSetColor(180, 185, 195); // Silver alloy rim
	drawRect(bx + 6.5 * scale, by + 2.5 * scale, 8.0 * scale, 11.0 * scale);
	drawRect(bx + w - 14.5 * scale, by + 2.5 * scale, 8.0 * scale, 11.0 * scale);
	iSetColor(60, 65, 75);
	iFilledCircle(bx + 10.5 * scale, by + 8.0 * scale, 2.0 * scale);
	iFilledCircle(bx + w - 10.5 * scale, by + 8.0 * scale, 2.0 * scale);

	// 3. Lower chassis & aerodynamic rocker skirt
	iSetColor(30, 32, 38);
	drawRect(bx + 2.0 * scale, by + 5.0 * scale, w - 4.0 * scale, 6.0 * scale);

	// 4. Main Metallic Sedan Body (Sleek Royal Navy / Charcoal Metallic)
	double bodyChamfer = 7.0 * scale;
	double bodyX[] = { bx, bx + w, bx + w - bodyChamfer, bx + bodyChamfer };
	double bodyY[] = { by + 8.0 * scale, by + 8.0 * scale, by + 34.0 * scale, by + 34.0 * scale };
	iSetColor(38, 70, 115); // Sophisticated metallic navy blue
	iFilledPolygon(bodyX, bodyY, 4);

	// 3D Metallic highlight reflection on hood
	iSetColor(65, 110, 165);
	double hoodHLX[] = { bx + 12.0 * scale, bx + w - 12.0 * scale, bx + w - 18.0 * scale, bx + 18.0 * scale };
	double hoodHLY[] = { by + 18.0 * scale, by + 18.0 * scale, by + 33.0 * scale, by + 33.0 * scale };
	iFilledPolygon(hoodHLX, hoodHLY, 4);

	// Aerodynamic hood crease lines
	iSetColor(25, 50, 85);
	iLine(bx + w * 0.28, by + 10.0 * scale, bx + w * 0.32, by + 33.0 * scale);
	iLine(bx + w * 0.72, by + 10.0 * scale, bx + w * 0.68, by + 33.0 * scale);

	// 5. Swept-Back Cabin Roof & Slanted Windshield
	double roofW = w * 0.68;
	double rx = centerX - roofW / 2.0;
	double roofY = by + h - 14.0 * scale;
	double cabinX[] = { bx + 4.0 * scale, bx + w - 4.0 * scale, rx + roofW, rx };
	double cabinY[] = { by + 33.0 * scale, by + 33.0 * scale, roofY, roofY };
	iSetColor(32, 60, 100);
	iFilledPolygon(cabinX, cabinY, 4);

	// Roof top panel
	iSetColor(42, 75, 120);
	drawRect(rx + 2.0 * scale, roofY - 2.0 * scale, roofW - 4.0 * scale, 3.0 * scale);

	// Tinted Glass Windshield
	double gw = roofW * 0.88;
	double gx = centerX - gw / 2.0;
	double glassX[] = { bx + 8.0 * scale, bx + w - 8.0 * scale, gx + gw, gx };
	double glassY[] = { by + 35.0 * scale, by + 35.0 * scale, roofY - 2.5 * scale, roofY - 2.5 * scale };
	iSetColor(20, 28, 42);
	iFilledPolygon(glassX, glassY, 4);

	// Glass specular glare streak
	iSetColor(80, 140, 195);
	double glareX[] = { bx + 12.0 * scale, bx + 22.0 * scale, gx + 16.0 * scale, gx + 6.0 * scale };
	double glareY[] = { by + 36.0 * scale, by + 36.0 * scale, roofY - 4.0 * scale, roofY - 4.0 * scale };
	iFilledPolygon(glareX, glareY, 4);

	// Driver & Front Passenger Silhouettes inside
	iSetColor(15, 20, 30);
	iFilledCircle(centerX - gw * 0.28, by + 44.0 * scale, 5.0 * scale);
	iFilledCircle(centerX + gw * 0.28, by + 44.0 * scale, 5.0 * scale);
	// Rear view mirror
	iSetColor(24, 25, 30);
	drawRect(centerX - 3.5 * scale, roofY - 7.0 * scale, 7.0 * scale, 3.5 * scale);

	// 6. Modern Chrome Radiator Grille & Logo
	double grW = w * 0.44;
	double grX = centerX - grW / 2.0;
	iSetColor(24, 26, 32);
	drawRect(grX, by + 16.0 * scale, grW, 10.0 * scale);
	iSetColor(210, 215, 225); // Chrome horizontal slats
	drawRect(grX + 2.0 * scale, by + 18.0 * scale, grW - 4.0 * scale, 1.8 * scale);
	drawRect(grX + 2.0 * scale, by + 21.0 * scale, grW - 4.0 * scale, 1.8 * scale);
	drawRect(grX + 2.0 * scale, by + 24.0 * scale, grW - 4.0 * scale, 1.8 * scale);
	// Car Emblem
	iSetColor(255, 255, 255);
	drawFilledEllipse(centerX, by + 21.0 * scale, 3.5 * scale, 2.2 * scale, 10);

	// 7. Sharp Angled Modern Projector Headlights
	// Left Headlight
	iSetColor(255, 255, 240);
	drawRect(bx + 4.0 * scale, by + 17.0 * scale, 12.0 * scale, 9.5 * scale);
	iSetColor(0, 220, 255); // Xenon projector dot
	iFilledCircle(bx + 11.0 * scale, by + 21.5 * scale, 2.8 * scale);
	iSetColor(255, 255, 255);
	iFilledCircle(bx + 11.0 * scale, by + 21.5 * scale, 1.2 * scale);
	iSetColor(245, 150, 20); // Amber corner signal
	drawRect(bx + 1.5 * scale, by + 17.0 * scale, 2.5 * scale, 9.5 * scale);

	// Right Headlight
	iSetColor(255, 255, 240);
	drawRect(bx + w - 16.0 * scale, by + 17.0 * scale, 12.0 * scale, 9.5 * scale);
	iSetColor(0, 220, 255);
	iFilledCircle(bx + w - 11.0 * scale, by + 21.5 * scale, 2.8 * scale);
	iSetColor(255, 255, 255);
	iFilledCircle(bx + w - 11.0 * scale, by + 21.5 * scale, 1.2 * scale);
	iSetColor(245, 150, 20);
	drawRect(bx + w - 4.0 * scale, by + 17.0 * scale, 2.5 * scale, 9.5 * scale);

	// 8. Front Bumper, Lower Grille & Fog Lamps
	iSetColor(32, 55, 90);
	drawRect(bx + 2.0 * scale, by + 8.0 * scale, w - 4.0 * scale, 9.0 * scale);
	iSetColor(20, 22, 28);
	drawRect(centerX - w * 0.22, by + 9.5 * scale, w * 0.44, 5.0 * scale);
	// Fog Lamps
	iSetColor(255, 255, 200);
	iFilledCircle(bx + 10.0 * scale, by + 12.0 * scale, 2.8 * scale);
	iFilledCircle(bx + w - 10.0 * scale, by + 12.0 * scale, 2.8 * scale);

	// 9. Dhaka Metro Green License Plate
	iSetColor(12, 115, 48);
	drawRect(centerX - 12.0 * scale, by + 9.5 * scale, 24.0 * scale, 7.5 * scale);
	iSetColor(255, 255, 255);
	drawRect(centerX - 10.5 * scale, by + 10.8 * scale, 21.0 * scale, 5.0 * scale);
	iSetColor(15, 80, 35);
	drawRect(centerX - 8.0 * scale, by + 12.0 * scale, 16.0 * scale, 2.2 * scale);

	// 10. Aerodynamic Side Mirrors
	iSetColor(30, 52, 88);
	drawRect(bx - 5.5 * scale, by + 34.0 * scale, 5.5 * scale, 8.5 * scale);
	drawRect(bx + w, by + 34.0 * scale, 5.5 * scale, 8.5 * scale);
	iSetColor(170, 205, 240);
	drawRect(bx - 4.2 * scale, by + 35.2 * scale, 3.8 * scale, 6.0 * scale);
	drawRect(bx + w + 0.8 * scale, by + 35.2 * scale, 3.8 * scale, 6.0 * scale);
}


void drawObstacleVehicle(Obstacle obs) {
	if (!obs.active) return;
	float t = (horizonY - obs.y) / (float)horizonY;
	if (t < 0.0f) t = 0.0f;
	if (t > 1.05f) t = 1.05f;
	float scale = 0.45f + 0.55f * t;

	if (obs.type == 0) {
		drawBRTCDoubleDeckerBus(obs.x, obs.y, scale);
	}
	else if (obs.type == 1) {
		drawWhiteMicrobusL1(obs.x, obs.y, scale);
	}
	else if (obs.type == 2) {
		drawYellowTaxiCabL1(obs.x, obs.y, scale);
	}
	else {
		drawDhakaPrivateCarL1(obs.x, obs.y, scale);
	}
}
void drawDestinationTimerHUD() {
	int hudX = SCREEN_WIDTH / 2 - 130;
	int hudY = SCREEN_HEIGHT - 45;
	int hudW = 260;
	int hudH = 32;
	iSetColor(18, 22, 30);
	iFilledRectangle(hudX, hudY, hudW, hudH);
	iSetColor(0, 220, 255);
	iRectangle(hudX, hudY, hudW, hudH);
	float progress = (LEVEL_TIME_LIMIT - levelTimeRemaining) / LEVEL_TIME_LIMIT;
	if (progress < 0.0f) progress = 0.0f;
	if (progress > 1.0f) progress = 1.0f;
	iSetColor(25, 80, 50);
	iFilledRectangle(hudX + 3, hudY + 3, (int)((hudW - 6) * progress), hudH - 6);
	char timerStr[50];
	sprintf_s(timerStr, sizeof(timerStr), "DESTINATION: %.1fs", levelTimeRemaining);
	iSetColor(255, 255, 255);
	iText(hudX + 38, hudY + 11, timerStr, GLUT_BITMAP_HELVETICA_12);
}

// =========================================================================
// COMIC CARTOON CONVERSATION & STORY SCREEN (STATE_STORY)
// =========================================================================
void drawComicSpeechBubble(int bx, int by, int bw, int bh, int tailX, int tailY, int isDriver) {
	iSetColor(15, 20, 30);
	drawRect(bx + 4, by - 4, bw, bh);
	iSetColor(255, 255, 255);
	drawRect(bx, by, bw, bh);
	double tX0 = (double)tailX - 16;
	double tX1 = (double)tailX + 16;
	if (tX0 < bx + 20) { tX0 = bx + 20; tX1 = bx + 52; }
	if (tX1 > bx + bw - 20) { tX1 = bx + bw - 20; tX0 = bx + bw - 52; }
	double triX[] = { tX0, tX1, (double)tailX };
	double triY[] = { (double)by, (double)by, (double)tailY };
	iFilledPolygon(triX, triY, 3);
	if (isDriver) iSetColor(0, 150, 70);
	else iSetColor(30, 110, 200);
	iRectangle(bx, by, bw, bh);
	iLine(triX[0], triY[0], triX[2], triY[2]);
	iLine(triX[1], triY[1], triX[2], triY[2]);
	if (isDriver) iSetColor(0, 160, 75);
	else iSetColor(30, 120, 210);
	drawRect(bx + 12, by + bh - 24, isDriver ? 190 : 160, 18);
}

void drawStoryHumanPassenger(int x, int y, int isActiveSpeaker) {
	iShowBMP2(x, y, "char\\p_Boy1.bmp", 0);
	if (isActiveSpeaker) {
		iSetColor(0, 220, 255);
		iFilledCircle(x + 28, y + 180, 5);
	}
}

void drawStoryRickshawPuller(int x, int y, int isActiveSpeaker) {
	iShowBMP2(x, y, "char\\driver.bmp", 0);
	if (isActiveSpeaker) {
		iSetColor(0, 220, 100);
		iFilledCircle(x + 45, y + 180, 5);
	}
}

void drawStoryScreen() {
	// Twilight Sky Gradient
	for (int y = 0; y < SCREEN_HEIGHT; y += 4) {
		float ratio = (float)y / (float)SCREEN_HEIGHT;
		int r = (int)(250 - ratio * 150);
		int g = (int)(140 - ratio * 95);
		int b = (int)(70 + ratio * 95);
		iSetColor(r, g, b);
		iFilledRectangle(0, y, SCREEN_WIDTH, 5);
	}
	// Setting Sun
	iSetColor(255, 195, 75);
	iFilledCircle(720, 420, 58);
	iSetColor(255, 235, 140);
	iFilledCircle(720, 420, 42);

	// Distant City Skyline Buildings (Realistic grand heights behind the street)
	iSetColor(42, 38, 55);
	drawRect(40, 175, 95, 230);
	drawRect(155, 175, 90, 270);
	drawRect(265, 175, 115, 210);
	drawRect(400, 175, 100, 250);
	drawRect(520, 175, 90, 190);
	drawRect(630, 175, 105, 260);
	drawRect(755, 175, 100, 220);

	// Building Windows
	iSetColor(255, 225, 120);
	drawRect(55, 330, 7, 10);
	drawRect(80, 290, 7, 10);
	drawRect(175, 380, 8, 11);
	drawRect(205, 340, 8, 11);
	drawRect(290, 310, 8, 10);
	drawRect(425, 360, 8, 11);
	drawRect(655, 370, 8, 11);
	drawRect(785, 330, 7, 10);

	// Foreground Road & Curb
	iSetColor(55, 60, 68);
	drawRect(0, 0, SCREEN_WIDTH, 175);
	iSetColor(130, 135, 145);
	drawRect(0, 170, SCREEN_WIDTH, 8);
	iSetColor(245, 195, 30);
	for (int x = 40; x < SCREEN_WIDTH; x += 130) {
		drawRect(x, 75, 65, 8);
	}
	// Street Lamp
	iSetColor(40, 42, 48);
	drawRect(350, 170, 8, 250);
	drawRect(335, 420, 38, 6);
	iSetColor(255, 240, 160);
	iFilledCircle(335, 417, 10);
	iFilledCircle(373, 417, 10);

	// Characters and Rickshaw standing on street (Balanced 1:1 human scale)
	int passX = 180, passY = 65;
	int isPassSpeaking = (storyStep == 0 || storyStep == 2);
	drawStoryHumanPassenger(passX, passY, isPassSpeaking);

	int driverX = 470, driverY = 65;
	int isDriverSpeaking = (storyStep == 1 || storyStep == 3);
	drawStoryRickshawPuller(driverX, driverY, isDriverSpeaking);

	int rBmpX = 620, rBmpY = 65;
	iShowBMP2(rBmpX, rBmpY, rickshaw[0], 0);
	iSetColor(215, 35, 45);
	drawRect(rBmpX + 10, rBmpY + 22, 28, 28);
	double hoodPolyX[] = { (double)rBmpX + 4, (double)rBmpX + 42, (double)rBmpX + 38, (double)rBmpX + 8 };
	double hoodPolyY[] = { (double)rBmpY + 28, (double)rBmpY + 28, (double)rBmpY + 54, (double)rBmpY + 54 };
	iFilledPolygon(hoodPolyX, hoodPolyY, 4);
	iSetColor(255, 215, 0);
	drawRect(rBmpX + 6, rBmpY + 26, 34, 4);
	iSetColor(28, 140, 65);
	drawRect(rBmpX + 12, rBmpY + 34, 22, 8);

	if (storyStep == 0) {
		int pBubbleX = 50, pBubbleY = 295, pBubbleW = 340, pBubbleH = 105;
		drawComicSpeechBubble(pBubbleX, pBubbleY, pBubbleW, pBubbleH, passX + 28, passY + 170, 0);
		iSetColor(255, 255, 255);
		iText(pBubbleX + 18, pBubbleY + 86, "Passenger (Student):", GLUT_BITMAP_HELVETICA_12);
		iSetColor(205, 35, 35);
		iText(pBubbleX + 18, pBubbleY + 56, "\"Mama, Station-Basabo jaben?\"", GLUT_BITMAP_HELVETICA_18);
		iSetColor(50, 55, 65);
		iText(pBubbleX + 18, pBubbleY + 28, "Mama, will you go to Station-Basabo?", GLUT_BITMAP_HELVETICA_12);
	}
	else if (storyStep == 1) {
		int dBubbleX = 400, dBubbleY = 295, dBubbleW = 350, dBubbleH = 105;
		drawComicSpeechBubble(dBubbleX, dBubbleY, dBubbleW, dBubbleH, driverX + 45, driverY + 170, 1);
		iSetColor(255, 255, 255);
		iText(dBubbleX + 18, dBubbleY + 86, "Rickshaw Mama:", GLUT_BITMAP_HELVETICA_12);
		iSetColor(0, 140, 60);
		iText(dBubbleX + 18, dBubbleY + 56, "\"Hae mama, jabo! 30 Taka lagbe.\"", GLUT_BITMAP_HELVETICA_18);
		iSetColor(50, 55, 65);
		iText(dBubbleX + 18, dBubbleY + 28, "Yes brother! It will take 30 Taka.", GLUT_BITMAP_HELVETICA_12);
	}
	else if (storyStep == 2) {
		int pBubbleX = 50, pBubbleY = 295, pBubbleW = 340, pBubbleH = 105;
		drawComicSpeechBubble(pBubbleX, pBubbleY, pBubbleW, pBubbleH, passX + 28, passY + 170, 0);
		iSetColor(255, 255, 255);
		iText(pBubbleX + 18, pBubbleY + 86, "Passenger (Bargaining):", GLUT_BITMAP_HELVETICA_12);
		iSetColor(210, 40, 40);
		iText(pBubbleX + 18, pBubbleY + 56, "\"Eto beshi? Mama 20 Taka rakhun!\"", GLUT_BITMAP_HELVETICA_18);
		iSetColor(50, 55, 65);
		iText(pBubbleX + 18, pBubbleY + 28, "That's too much! Take 20 Taka, let's go now!", GLUT_BITMAP_HELVETICA_12);
	}
	else if (storyStep == 3) {
		int dBubbleX = 400, dBubbleY = 295, dBubbleW = 350, dBubbleH = 105;
		drawComicSpeechBubble(dBubbleX, dBubbleY, dBubbleW, dBubbleH, driverX + 45, driverY + 170, 1);
		iSetColor(255, 255, 255);
		iText(dBubbleX + 18, dBubbleY + 86, "Rickshaw Mama (Agreed):", GLUT_BITMAP_HELVETICA_12);
		iSetColor(0, 140, 60);
		iText(dBubbleX + 18, dBubbleY + 56, "\"Hae mama! 20 Taka lagbe! Uthun!\"", GLUT_BITMAP_HELVETICA_18);
		iSetColor(50, 55, 65);
		iText(dBubbleX + 18, dBubbleY + 28, "Hop in! 20 Taka deal! I'll reach quickly!", GLUT_BITMAP_HELVETICA_12);
	}
	else {
		int infoY = 295;
		iSetColor(20, 25, 35);
		iFilledRectangle(SCREEN_WIDTH / 2 - 270, infoY, 540, 85);
		iSetColor(0, 220, 120);
		iRectangle(SCREEN_WIDTH / 2 - 270, infoY, 540, 85);
		iRectangle(SCREEN_WIDTH / 2 - 267, infoY + 3, 534, 79);
		iSetColor(255, 215, 0);
		iText(SCREEN_WIDTH / 2 - 195, infoY + 52, "*** FARE FIXED: 20 COINS ***", GLUT_BITMAP_TIMES_ROMAN_24);
		iSetColor(240, 245, 255);
		iText(SCREEN_WIDTH / 2 - 220, infoY + 22, "Survive the traffic and reach Station-Basabo to earn 20 Coins!", GLUT_BITMAP_HELVETICA_12);
	}

	if (storyStep < 4) {
		int barX = SCREEN_WIDTH / 2 - 220;
		int barY = 25;
		iSetColor(20, 25, 35);
		iFilledRectangle(barX, barY, 440, 36);
		iSetColor(0, 200, 255);
		iRectangle(barX, barY, 440, 36);
		char promptMsg[80];
		sprintf_s(promptMsg, sizeof(promptMsg), "Click or Press [SPACE] to continue (%d/4) >", storyStep + 1);
		iSetColor(255, 255, 255);
		iText(barX + 60, barY + 12, promptMsg, GLUT_BITMAP_HELVETICA_12);
		iSetColor(80, 85, 95);
		iFilledRectangle(SCREEN_WIDTH - 130, 25, 100, 36);
		iSetColor(255, 255, 255);
		iText(SCREEN_WIDTH - 115, 37, "SKIP [S] >", GLUT_BITMAP_HELVETICA_12);
	}
	else {
		int startBtnX = SCREEN_WIDTH / 2 - 190;
		int startBtnY = 20;
		int startBtnW = 380;
		int startBtnH = 46;
		int isHov = (mouseX >= startBtnX && mouseX <= startBtnX + startBtnW && mouseY >= startBtnY && mouseY <= startBtnY + startBtnH);
		if (isHov) iSetColor(0, 210, 100);
		else iSetColor(0, 170, 80);
		iFilledRectangle(startBtnX, startBtnY, startBtnW, startBtnH);
		iSetColor(255, 255, 255);
		iRectangle(startBtnX, startBtnY, startBtnW, startBtnH);
		iText(startBtnX + 34, startBtnY + 15, "PRESS [ENTER] OR CLICK TO START RIDE >", GLUT_BITMAP_HELVETICA_12);
	}
}

// =========================================================================
// DESTINATION REACHED / VICTORY MODAL (STATE_VICTORY)
// =========================================================================
void drawVictoryScreen() {
	if (selectedLevel == 3) drawLevel3Scene();
	else if (selectedLevel == 2) drawLevel2Scene();
	else drawLevel1Scene();

	iSetColor(10, 15, 25);
	for (int y = 0; y < SCREEN_HEIGHT; y += 4) {
		iFilledRectangle(0, y, SCREEN_WIDTH, 2);
	}
	int boxW = 540;
	int boxH = 280;
	int boxX = SCREEN_WIDTH / 2 - boxW / 2;
	int boxY = SCREEN_HEIGHT / 2 - boxH / 2;
	iSetColor(20, 25, 35);
	iFilledRectangle(boxX, boxY, boxW, boxH);
	iSetColor(0, 220, 120);
	iRectangle(boxX, boxY, boxW, boxH);
	iRectangle(boxX + 3, boxY + 3, boxW - 6, boxH - 6);
	iSetColor(255, 215, 0);
	iText(SCREEN_WIDTH / 2 - 180, boxY + 235, "*** DESTINATION REACHED! ***", GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(255, 255, 255);
	if (selectedLevel == 1) {
		iText(SCREEN_WIDTH / 2 - 155, boxY + 195, "Safely arrived at Station - Basabo!", GLUT_BITMAP_HELVETICA_18);
	}
	else if (selectedLevel == 2) {
		iText(SCREEN_WIDTH / 2 - 165, boxY + 195, "Safely arrived at Station - Moghbazar!", GLUT_BITMAP_HELVETICA_18);
	}
	else {
		iText(SCREEN_WIDTH / 2 - 145, boxY + 195, "Safely arrived at Station - Aust!", GLUT_BITMAP_HELVETICA_18);
	}
	char fareMsg[60];
	sprintf_s(fareMsg, sizeof(fareMsg), "Agreed Fare Earned: +%d Coins", AGREED_FARE);
	iSetColor(0, 255, 160);
	iText(SCREEN_WIDTH / 2 - 135, boxY + 155, fareMsg, GLUT_BITMAP_HELVETICA_18);
	char walletMsg[60];
	sprintf_s(walletMsg, sizeof(walletMsg), "Total Coins in Wallet: %d", totalCoins);
	iSetColor(255, 220, 50);
	iText(SCREEN_WIDTH / 2 - 80, boxY + 120, walletMsg, GLUT_BITMAP_HELVETICA_12);
	if (!level2Unlocked && totalCoins >= 10) {
		level2Unlocked = 1;
		iSetColor(255, 100, 100);
		iText(SCREEN_WIDTH / 2 - 125, boxY + 90, "LEVEL 2 (Station - Moghbazar) UNLOCKED!", GLUT_BITMAP_HELVETICA_12);
	}
	int rBtnX = SCREEN_WIDTH / 2 - 190, rBtnY = boxY + 30;
	iSetColor(0, 160, 80);
	iFilledRectangle(rBtnX, rBtnY, 170, 45);
	iSetColor(255, 255, 255);
	iRectangle(rBtnX, rBtnY, 170, 45);
	iText(rBtnX + 38, rBtnY + 16, "REPLAY (R)", GLUT_BITMAP_HELVETICA_12);
	int mBtnX = SCREEN_WIDTH / 2 + 20, mBtnY = boxY + 30;
	iSetColor(35, 95, 175);
	iFilledRectangle(mBtnX, mBtnY, 170, 45);
	iSetColor(255, 255, 255);
	iRectangle(mBtnX, mBtnY, 170, 45);
	iText(mBtnX + 45, mBtnY + 16, "MENU (M)", GLUT_BITMAP_HELVETICA_12);
}

// =========================================================================
// TITLE, MENU, LEVEL PREVIEWS & GARAGE SCREEN
// =========================================================================
void drawStripedAwning(int x, int y, int w, int h, int r1, int g1, int b1, int r2, int g2, int b2) {
	int stripes = 6, sw = w / stripes;
	for (int i = 0; i < stripes; i++) {
		if (i % 2 == 0) iSetColor(r1, g1, b1);
		else iSetColor(r2, g2, b2);
		iFilledRectangle(x + i * sw, y, sw, h);
		iFilledCircle(x + i * sw + sw / 2, y, sw / 2);
	}
	iSetColor(30, 25, 20);
	iFilledRectangle(x, y - 3, w, 3);
}

static unsigned int logoTextureID = 0;
static bool logoAttemptedLoad = false;
static int logoBlinkCounter = 0;

void drawTitleScreen() {
	if (!logoAttemptedLoad) {
		logoAttemptedLoad = true;
		logoTextureID = iLoadImage("Logo.bmp");
		if (!logoTextureID) {
			logoTextureID = iLoadImage("char\\Logo.bmp");
		}
	}

	iSetColor(12, 14, 20);
	iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	if (logoTextureID) {
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, logoTextureID);
	}
	else {
		FILE* fp = fopen("Logo.bmp", "rb");
		if (fp) {
			fclose(fp);
			iShowBMP(0, 0, "Logo.bmp");
		}
		else {
			fp = fopen("char\\Logo.bmp", "rb");
			if (fp) {
				fclose(fp);
				iShowBMP(0, 0, "char\\Logo.bmp");
			}
		}
	}

	logoBlinkCounter++;

	// High-visibility, polished Continue Button
	int boxW = 380;
	int boxH = 54;
	int boxX = (SCREEN_WIDTH - boxW) / 2;
	int boxY = 62;

	bool isHovered = (mouseX >= boxX && mouseX <= boxX + boxW && mouseY >= boxY && mouseY <= boxY + boxH);

	// Deep drop shadow behind button for high contrast against any background
	iSetColor(0, 0, 0);
	iFilledRectangle(boxX - 6, boxY - 6, boxW + 12, boxH + 12);

	// Outer pulsing neon border
	int glow = (int)(25.0 * sin(logoBlinkCounter * 0.08));
	if (isHovered) {
		iSetColor(255, 235, 60);
	}
	else {
		iSetColor(245, 195 + glow, 20);
	}
	iFilledRectangle(boxX - 3, boxY - 3, boxW + 6, boxH + 6);

	// Button background: rich, vibrant arcade plate
	if (isHovered) {
		iSetColor(28, 90, 160); // bright electric hover
		iFilledRectangle(boxX, boxY, boxW, boxH);
		iSetColor(45, 125, 210);
		iFilledRectangle(boxX + 2, boxY + boxH / 2, boxW - 4, boxH / 2 - 2);
	}
	else {
		// Sleek dark-slate plate with subtle top highlight
		iSetColor(18, 22, 34);
		iFilledRectangle(boxX, boxY, boxW, boxH);
		iSetColor(32, 42, 60);
		iFilledRectangle(boxX + 2, boxY + boxH / 2, boxW - 4, boxH / 2 - 2);
	}

	// Inner crisp gold border
	iSetColor(255, 215, 30);
	iRectangle(boxX + 1, boxY + 1, boxW - 2, boxH - 2);
	iRectangle(boxX + 2, boxY + 2, boxW - 4, boxH - 4);

	// Corner accent brackets for high-tech arcade polish
	iSetColor(255, 240, 100);
	iLine(boxX + 6, boxY + boxH - 6, boxX + 16, boxY + boxH - 6);
	iLine(boxX + 6, boxY + boxH - 6, boxX + 6, boxY + boxH - 16);
	iLine(boxX + boxW - 6, boxY + boxH - 6, boxX + boxW - 16, boxY + boxH - 6);
	iLine(boxX + boxW - 6, boxY + boxH - 6, boxX + boxW - 6, boxY + boxH - 16);

	// Bold text with drop shadow for 100% clarity
	const char* btnText = isHovered ? ">>>  TAP TO CONTINUE  <<<" : "CLICK TO CONTINUE  >>>";
	int textX = isHovered ? (boxX + 50) : (boxX + 45);
	int textY = boxY + 18;

	// Text shadow
	iSetColor(0, 0, 0);
	iText(textX + 2, textY - 2, (char*)btnText, GLUT_BITMAP_TIMES_ROMAN_24);
	iText(textX + 1, textY - 1, (char*)btnText, GLUT_BITMAP_TIMES_ROMAN_24);

	// Crisp golden/white text
	if (isHovered) {
		iSetColor(255, 255, 255);
	}
	else {
		iSetColor(255, 225, 40);
	}
	iText(textX, textY, (char*)btnText, GLUT_BITMAP_TIMES_ROMAN_24);

	// Secondary instruction badge underneath
	int subW = 340;
	int subH = 22;
	int subX = (SCREEN_WIDTH - subW) / 2;
	int subY = boxY - 30;

	iSetColor(8, 10, 15);
	iFilledRectangle(subX, subY, subW, subH);
	iSetColor(65, 80, 110);
	iRectangle(subX, subY, subW, subH);

	iSetColor(220, 230, 245);
	iText(subX + 16, subY + 6, "Press  [SPACE] ,  [ENTER] ,  or  Click  to  Continue", GLUT_BITMAP_HELVETICA_12);
}

void drawCityDrivePreview(int px, int py, int pw, int ph) {
	int miniHorizon = py + (int)(ph * 0.60f);
	iSetColor(135, 206, 235); iFilledRectangle(px, miniHorizon, pw, ph - (miniHorizon - py));
	iSetColor(255, 200, 0);   iFilledCircle(px + pw - 45, py + ph - 35, 18);
	iSetColor(102, 178, 60);  iFilledRectangle(px, py, pw, miniHorizon - py);
	double roadX[] = { (double)px + pw * 0.35, (double)px + pw * 0.65, (double)px + pw - 15, (double)px + 15 };
	double roadY[] = { (double)miniHorizon, (double)miniHorizon, (double)py, (double)py };
	iSetColor(70, 70, 70); iFilledPolygon(roadX, roadY, 4);
	iShowBMP2(px + pw / 2 - 22, py + 15, getEquippedRickshawPreviewBMP(), 0);
	iSetColor(0, 180, 50); iFilledRectangle(px + 12, py + ph - 30, 95, 20);
	iSetColor(255, 255, 255);
	iText(px + 18, py + ph - 25, "ACTIVE VIEW", GLUT_BITMAP_HELVETICA_10);
}

void drawRushHourPreview(int px, int py, int pw, int ph) {
	int miniHorizon = py + (int)(ph * 0.50f);
	iSetColor(70, 160, 235); iFilledRectangle(px, miniHorizon, pw, ph - (miniHorizon - py));
	iSetColor(200, 200, 200); iFilledRectangle(px, py, pw, miniHorizon - py);
	double roadX[] = { (double)px + 25, (double)px + pw * 0.44, (double)px + pw * 0.56, (double)px + pw - 25 };
	double roadY[] = { (double)py, (double)miniHorizon, (double)miniHorizon, (double)py };
	iSetColor(50, 55, 60); iFilledPolygon(roadX, roadY, 4);
	if (!level2Unlocked) {
		iSetColor(0, 0, 0);
		iFilledRectangle(px + pw / 2 - 125, py + ph / 2 - 45, 250, 80);
		iSetColor(255, 180, 0); iRectangle(px + pw / 2 - 125, py + ph / 2 - 45, 250, 80);
		iSetColor(255, 70, 70);
		iText(px + pw / 2 - 80, py + ph / 2 + 10, "LEVEL 2 : LOCKED", GLUT_BITMAP_HELVETICA_18);
		char lockMsg[60];
		sprintf_s(lockMsg, sizeof(lockMsg), "Collect 10 Coins to Unlock (%d/10)", totalCoins < 10 ? totalCoins : 10);
		iSetColor(255, 215, 0);
		iText(px + pw / 2 - 110, py + ph / 2 - 20, lockMsg, GLUT_BITMAP_HELVETICA_12);
	}
	else {
		iShowBMP2(px + pw / 2 - 22, py + 20, getEquippedRickshawPreviewBMP(), 0);
		iSetColor(0, 180, 50); iFilledRectangle(px + 12, py + ph - 30, 130, 20);
		iSetColor(255, 255, 255);
		iText(px + 18, py + ph - 25, "UNLOCKED : READY", GLUT_BITMAP_HELVETICA_10);
	}
}

void drawHighwayChasePreview(int px, int py, int pw, int ph) {
	if (!level3Unlocked) {
		for (int i = 0; i < ph; i += 6) {
			iSetColor(10 + (int)(i * 0.1f), 15 + (int)(i * 0.15f), 40 + (int)(i * 0.35f));
			iFilledRectangle(px, py + i, pw, 7);
		}
		iSetColor(0, 0, 0);
		iFilledRectangle(px + pw / 2 - 125, py + ph / 2 - 45, 250, 80);
		iSetColor(0, 220, 255); iRectangle(px + pw / 2 - 125, py + ph / 2 - 45, 250, 80);
		iSetColor(255, 70, 70);
		iText(px + pw / 2 - 80, py + ph / 2 + 10, "LEVEL 3 : LOCKED", GLUT_BITMAP_HELVETICA_18);
		char lockMsg[60];
		if (totalCoins >= level3UnlockCost) {
			sprintf_s(lockMsg, sizeof(lockMsg), "Unlock for 10 Coins (Have %d)", totalCoins);
			iSetColor(0, 255, 120);
		}
		else {
			sprintf_s(lockMsg, sizeof(lockMsg), "Collect 10 Coins to Unlock (%d/10)", totalCoins);
			iSetColor(255, 215, 0);
		}
		iText(px + pw / 2 - 110, py + ph / 2 - 20, lockMsg, GLUT_BITMAP_HELVETICA_12);
	}
	else {
		for (int i = 0; i < ph; i += 6) {
			iSetColor(14 + (int)(i * 0.05f), 20 + (int)(i * 0.05f), 38 + (int)(i * 0.15f));
			iFilledRectangle(px, py + i, pw, 7);
		}
		int miniHorizon = py + (int)(ph * 0.50f);
		iSetColor(55, 52, 58);
		iFilledRectangle(px, py, pw, miniHorizon - py);
		double roadX[] = { (double)px + 25, (double)px + pw * 0.44, (double)px + pw * 0.56, (double)px + pw - 25 };
		double roadY[] = { (double)py, (double)miniHorizon, (double)miniHorizon, (double)py };
		iSetColor(24, 28, 34); iFilledPolygon(roadX, roadY, 4);
		iShowBMP2(px + pw / 2 - 22, py + 20, getEquippedRickshawPreviewBMP(), 0);
		iSetColor(160, 200, 240);
		for (int r = 0; r < 20; r++) {
			int rx = px + (r * 23) % pw;
			int ry = py + (r * 17) % ph;
			iLine(rx, ry, rx - 3, ry - 10);
		}
		iSetColor(0, 180, 50); iFilledRectangle(px + 12, py + ph - 30, 130, 20);
		iSetColor(255, 255, 255);
		iText(px + 18, py + ph - 25, "UNLOCKED : READY", GLUT_BITMAP_HELVETICA_10);
	}
}

void drawGarageScreen() {
	iSetColor(20, 24, 34);
	iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Header Bar
	iSetColor(30, 38, 54);
	iFilledRectangle(0, 520, SCREEN_WIDTH, 80);
	iSetColor(255, 200, 0);
	iFilledRectangle(0, 517, SCREEN_WIDTH, 3);
	iSetColor(255, 255, 255);
	iText(35, 550, "DHAKA RICKSHAW GARAGE", GLUT_BITMAP_TIMES_ROMAN_24);

	// Top Right HUD - Coins, Bullets & Shield Status
	iSetColor(15, 20, 30);
	iFilledRectangle(SCREEN_WIDTH - 440, 532, 410, 54);
	iSetColor(255, 200, 0);
	iRectangle(SCREEN_WIDTH - 440, 532, 410, 54);

	// Coins
	iSetColor(255, 215, 0);
	iFilledCircle(SCREEN_WIDTH - 418, 559, 10);
	char coinMsg[50];
	sprintf_s(coinMsg, sizeof(coinMsg), "COINS: %d", totalCoins);
	iSetColor(255, 220, 0);
	iText(SCREEN_WIDTH - 402, 553, coinMsg, GLUT_BITMAP_HELVETICA_12);

	// Bullets
	iSetColor(255, 100, 60);
	iFilledCircle(SCREEN_WIDTH - 275, 559, 10);
	char bulletMsg[50];
	sprintf_s(bulletMsg, sizeof(bulletMsg), "BULLETS: %d", playerBulletsL3);
	iSetColor(255, 180, 150);
	iText(SCREEN_WIDTH - 259, 553, bulletMsg, GLUT_BITMAP_HELVETICA_12);

	// Shield Status Badge in Header
	drawCustomShieldIcon((float)(SCREEN_WIDTH - 138), 559.0f, 0.45f, (hasEnergyShield != 0));
	iSetColor(200, 240, 255);
	if (hasEnergyShield) {
		iText(SCREEN_WIDTH - 120, 553, "SHIELD: READY", GLUT_BITMAP_HELVETICA_12);
	}
	else {
		iText(SCREEN_WIDTH - 120, 553, "SHIELD: NONE", GLUT_BITMAP_HELVETICA_12);
	}

	// -------------------------------------------------------------
	// 3 RICKSHAW CARDS
	// -------------------------------------------------------------
	int cardY = 85, cardW = 260, cardH = 425;
	int cardXs[3] = { 35, 320, 605 };

	// --- CARD 0: CLASSIC BLUE (DEFAULT RICKSHAW) ---
	{
		int cX = cardXs[0];
		iSetColor(26, 33, 46);
		iFilledRectangle(cX, cardY, cardW, cardH);

		if (equippedRickshaw == 0) {
			iSetColor(0, 255, 140);
			iRectangle(cX, cardY, cardW, cardH);
			iRectangle(cX - 1, cardY - 1, cardW + 2, cardH + 2);
		}
		else {
			iSetColor(60, 80, 110);
			iRectangle(cX, cardY, cardW, cardH);
		}

		iSetColor(255, 255, 255);
		iText(cX + 24, cardY + cardH - 30, "CLASSIC BLUE", GLUT_BITMAP_HELVETICA_18);
		iSetColor(255, 215, 0);
		iText(cX + 24, cardY + cardH - 50, "[ DEFAULT RICKSHAW ]", GLUT_BITMAP_HELVETICA_12);

		// Rickshaw Image Container (230 x 225, Center is cX + 130, cardY + 207)
		int boxX = cX + 15, boxY = cardY + 95, boxW = 230, boxH = 225;
		iSetColor(18, 22, 32);
		iFilledRectangle(boxX, boxY, boxW, boxH);
		iSetColor(45, 55, 75);
		iRectangle(boxX, boxY, boxW, boxH);

		// Centered Classic Blue Rickshaw (100 x 162)
		int drawX = cX + 130 - 50;
		int drawY = cardY + 207 - 81;
		iShowBMP2(drawX, drawY, rickshaw[0], 0);

		// Status / Action Button
		int btnX = cX + 15, btnY = cardY + 20, btnW = 230, btnH = 56;
		if (equippedRickshaw == 0) {
			iSetColor(0, 180, 80);
			iFilledRectangle(btnX, btnY, btnW, btnH);
			iSetColor(255, 255, 255);
			iRectangle(btnX, btnY, btnW, btnH);
			iText(btnX + 38, btnY + 20, "* EQUIPPED *", GLUT_BITMAP_HELVETICA_18);
		}
		else {
			iSetColor(40, 60, 90);
			iFilledRectangle(btnX, btnY, btnW, btnH);
			iSetColor(100, 160, 240);
			iRectangle(btnX, btnY, btnW, btnH);
			iSetColor(200, 230, 255);
			iText(btnX + 45, btnY + 32, "OWNED (DEFAULT)", GLUT_BITMAP_HELVETICA_12);
			iSetColor(255, 255, 255);
			iText(btnX + 22, btnY + 12, "CLICK / PRESS [1] TO EQUIP", GLUT_BITMAP_HELVETICA_10);
		}
	}

	// --- CARD 1: SUPER GREEN RICKSHAW ---
	{
		int cX = cardXs[1];
		iSetColor(32, 32, 42);
		iFilledRectangle(cX, cardY, cardW, cardH);

		if (equippedRickshaw == 1) {
			iSetColor(0, 255, 140);
			iRectangle(cX, cardY, cardW, cardH);
			iRectangle(cX - 1, cardY - 1, cardW + 2, cardH + 2);
		}
		else if (ownedRickshaws[1]) {
			iSetColor(255, 200, 0);
			iRectangle(cX, cardY, cardW, cardH);
		}
		else {
			iSetColor(120, 100, 40);
			iRectangle(cX, cardY, cardW, cardH);
		}

		iSetColor(0, 255, 180);
		iText(cX + 20, cardY + cardH - 30, "SUPER GREEN", GLUT_BITMAP_HELVETICA_18);
		iSetColor(200, 200, 200);
		iText(cX + 20, cardY + cardH - 50, "Expressway Cruiser", GLUT_BITMAP_HELVETICA_12);

		// Rickshaw Image Container (230 x 225, Center is cX + 130, cardY + 207)
		int boxX = cX + 15, boxY = cardY + 95, boxW = 230, boxH = 225;
		iSetColor(18, 22, 32);
		iFilledRectangle(boxX, boxY, boxW, boxH);
		iSetColor(45, 55, 75);
		iRectangle(boxX, boxY, boxW, boxH);

		// Centered Deluxe Yellow Rickshaw (150 x 89)
		int drawX = cX + 130 - 75;
		int drawY = cardY + 207 - 45;
		iShowBMP2(drawX, drawY, rickshawL2[0], 0);

		// Status / Action Button
		int btnX = cX + 15, btnY = cardY + 20, btnW = 230, btnH = 56;
		if (equippedRickshaw == 1) {
			iSetColor(0, 180, 80);
			iFilledRectangle(btnX, btnY, btnW, btnH);
			iSetColor(255, 255, 255);
			iRectangle(btnX, btnY, btnW, btnH);
			iText(btnX + 38, btnY + 20, "* EQUIPPED *", GLUT_BITMAP_HELVETICA_18);
		}
		else if (ownedRickshaws[1]) {
			iSetColor(40, 60, 90);
			iFilledRectangle(btnX, btnY, btnW, btnH);
			iSetColor(100, 160, 240);
			iRectangle(btnX, btnY, btnW, btnH);
			iSetColor(200, 230, 255);
			iText(btnX + 65, btnY + 32, "OWNED", GLUT_BITMAP_HELVETICA_12);
			iSetColor(255, 255, 255);
			iText(btnX + 22, btnY + 12, "CLICK / PRESS [2] TO EQUIP", GLUT_BITMAP_HELVETICA_10);
		}
		else {
			if (totalCoins >= yellowRickshawCost) {
				iSetColor(0, 160, 80);
				iFilledRectangle(btnX, btnY, btnW, btnH);
				iSetColor(255, 255, 255);
				iRectangle(btnX, btnY, btnW, btnH);
				iText(btnX + 28, btnY + 32, "BUY FOR 20 COINS", GLUT_BITMAP_HELVETICA_12);
				iText(btnX + 42, btnY + 12, "CLICK / PRESS [2]", GLUT_BITMAP_HELVETICA_10);
			}
			else {
				iSetColor(90, 35, 35);
				iFilledRectangle(btnX, btnY, btnW, btnH);
				iSetColor(220, 80, 80);
				iRectangle(btnX, btnY, btnW, btnH);
				iSetColor(255, 200, 200);
				iText(btnX + 38, btnY + 32, "COST: 20 COINS", GLUT_BITMAP_HELVETICA_12);
				char needMsg[40];
				sprintf_s(needMsg, sizeof(needMsg), "Need %d more coins", yellowRickshawCost - totalCoins);
				iSetColor(255, 160, 160);
				iText(btnX + 45, btnY + 12, needMsg, GLUT_BITMAP_HELVETICA_10);
			}
		}
	}

	// --- CARD 2: SUPER RED RICKSHAW ---
	{
		int cX = cardXs[2];
		iSetColor(28, 38, 36);
		iFilledRectangle(cX, cardY, cardW, cardH);

		if (equippedRickshaw == 2) {
			iSetColor(0, 255, 140);
			iRectangle(cX, cardY, cardW, cardH);
			iRectangle(cX - 1, cardY - 1, cardW + 2, cardH + 2);
		}
		else if (ownedRickshaws[2]) {
			iSetColor(0, 220, 140);
			iRectangle(cX, cardY, cardW, cardH);
		}
		else {
			iSetColor(50, 110, 80);
			iRectangle(cX, cardY, cardW, cardH);
		}

		iSetColor(255, 80, 80);
		iText(cX + 24, cardY + cardH - 30, "SUPER RED", GLUT_BITMAP_HELVETICA_18);
		iSetColor(200, 200, 200);
		iText(cX + 24, cardY + cardH - 50, "Storm Runner Pro", GLUT_BITMAP_HELVETICA_12);

		// Rickshaw Image Container (230 x 225, Center is cX + 130, cardY + 207)
		int boxX = cX + 15, boxY = cardY + 95, boxW = 230, boxH = 225;
		iSetColor(18, 22, 32);
		iFilledRectangle(boxX, boxY, boxW, boxH);
		iSetColor(45, 55, 75);
		iRectangle(boxX, boxY, boxW, boxH);

		// Centered Super Green Rickshaw (80 x 128)
		int drawX = cX + 130 - 40;
		int drawY = cardY + 207 - 64;
		iShowBMP2(drawX, drawY, rickshawL3[0], 0);

		// Status / Action Button
		int btnX = cX + 15, btnY = cardY + 20, btnW = 230, btnH = 56;
		if (equippedRickshaw == 2) {
			iSetColor(0, 180, 80);
			iFilledRectangle(btnX, btnY, btnW, btnH);
			iSetColor(255, 255, 255);
			iRectangle(btnX, btnY, btnW, btnH);
			iText(btnX + 38, btnY + 20, "* EQUIPPED *", GLUT_BITMAP_HELVETICA_18);
		}
		else if (ownedRickshaws[2]) {
			iSetColor(40, 60, 90);
			iFilledRectangle(btnX, btnY, btnW, btnH);
			iSetColor(100, 160, 240);
			iRectangle(btnX, btnY, btnW, btnH);
			iSetColor(200, 230, 255);
			iText(btnX + 65, btnY + 32, "OWNED", GLUT_BITMAP_HELVETICA_12);
			iSetColor(255, 255, 255);
			iText(btnX + 22, btnY + 12, "CLICK / PRESS [3] TO EQUIP", GLUT_BITMAP_HELVETICA_10);
		}
		else {
			if (totalCoins >= greenRickshawCost) {
				iSetColor(0, 160, 80);
				iFilledRectangle(btnX, btnY, btnW, btnH);
				iSetColor(255, 255, 255);
				iRectangle(btnX, btnY, btnW, btnH);
				iText(btnX + 28, btnY + 32, "BUY FOR 30 COINS", GLUT_BITMAP_HELVETICA_12);
				iText(btnX + 42, btnY + 12, "CLICK / PRESS [3]", GLUT_BITMAP_HELVETICA_10);
			}
			else {
				iSetColor(90, 35, 35);
				iFilledRectangle(btnX, btnY, btnW, btnH);
				iSetColor(220, 80, 80);
				iRectangle(btnX, btnY, btnW, btnH);
				iSetColor(255, 200, 200);
				iText(btnX + 38, btnY + 32, "COST: 30 COINS", GLUT_BITMAP_HELVETICA_12);
				char needMsg[40];
				sprintf_s(needMsg, sizeof(needMsg), "Need %d more coins", greenRickshawCost - totalCoins);
				iSetColor(255, 160, 160);
				iText(btnX + 45, btnY + 12, needMsg, GLUT_BITMAP_HELVETICA_10);
			}
		}
	}

	// -------------------------------------------------------------
	// BOTTOM CONTROLS & ENERGY SHIELD POWER-UP SECTION
	// -------------------------------------------------------------
	// Left Button: Back to Menu
	int bkX = 35, bkY = 16, bkW = 230, bkH = 54;
	iSetColor(40, 50, 70);
	iFilledRectangle(bkX, bkY, bkW, bkH);
	iSetColor(255, 255, 255);
	iRectangle(bkX, bkY, bkW, bkH);
	iText(bkX + 32, bkY + 21, "<- BACK TO MENU [M]", GLUT_BITMAP_HELVETICA_12);

	// Right Banner: TITAN ENERGY SHIELD POWER-UP
	int shX = 285, shY = 16, shW = 580, shH = 54;
	iSetColor(20, 28, 44);
	iFilledRectangle(shX, shY, shW, shH);
	iSetColor(0, 200, 255);
	iRectangle(shX, shY, shW, shH);
	iRectangle(shX + 1, shY + 1, shW - 2, shH - 2);

	// Shield Icon
	drawCustomShieldIcon((float)(shX + 28), (float)(shY + 27), 0.85f, (hasEnergyShield != 0));

	// Shield Info
	iSetColor(255, 215, 0);
	iText(shX + 54, shY + 32, "TITAN ENERGY SHIELD [POWER-UP]", GLUT_BITMAP_HELVETICA_12);
	iSetColor(180, 220, 255);
	iText(shX + 54, shY + 14, "15s Invincible Shield in level | Immune to all crash damage", GLUT_BITMAP_HELVETICA_10);

	// Shield Action Button inside banner
	int shBtnX = shX + 360, shBtnY = shY + 7, shBtnW = 210, shBtnH = 40;
	if (hasEnergyShield) {
		iSetColor(0, 180, 80);
		iFilledRectangle(shBtnX, shBtnY, shBtnW, shBtnH);
		iSetColor(255, 255, 255);
		iRectangle(shBtnX, shBtnY, shBtnW, shBtnH);
		iText(shBtnX + 22, shBtnY + 23, "* SHIELD READY *", GLUT_BITMAP_HELVETICA_12);
		iSetColor(220, 255, 230);
		iText(shBtnX + 12, shBtnY + 8, "CLICK IN LEVEL TO ACTIVATE", GLUT_BITMAP_HELVETICA_10);
	}
	else if (totalCoins >= energyShieldCost) {
		iSetColor(0, 140, 220);
		iFilledRectangle(shBtnX, shBtnY, shBtnW, shBtnH);
		iSetColor(255, 255, 255);
		iRectangle(shBtnX, shBtnY, shBtnW, shBtnH);
		iSetColor(255, 255, 255);
		iText(shBtnX + 18, shBtnY + 23, "BUY SHIELD (15 COINS)", GLUT_BITMAP_HELVETICA_12);
		iSetColor(255, 225, 50);
		iText(shBtnX + 26, shBtnY + 8, "CLICK / PRESS [S] OR [4]", GLUT_BITMAP_HELVETICA_10);
	}
	else {
		iSetColor(90, 35, 35);
		iFilledRectangle(shBtnX, shBtnY, shBtnW, shBtnH);
		iSetColor(220, 80, 80);
		iRectangle(shBtnX, shBtnY, shBtnW, shBtnH);
		iSetColor(255, 200, 200);
		iText(shBtnX + 32, shBtnY + 23, "COST: 15 COINS", GLUT_BITMAP_HELVETICA_12);
		char needMsg[40];
		sprintf_s(needMsg, sizeof(needMsg), "Need %d more coins", energyShieldCost - totalCoins);
		iSetColor(255, 160, 160);
		iText(shBtnX + 38, shBtnY + 8, needMsg, GLUT_BITMAP_HELVETICA_10);
	}
}

void drawMenuScreen() {
	iSetColor(18, 22, 33);
	iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	iSetColor(28, 36, 52);
	iFilledRectangle(0, 520, SCREEN_WIDTH, 80);
	iSetColor(0, 200, 255);
	iFilledRectangle(0, 517, SCREEN_WIDTH, 3);
	iSetColor(255, 255, 255);
	iText(45, 550, "TESLA - RIDE", GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(160, 200, 230);
	iText(240, 553, "|   SELECT YOUR DESTINATION", GLUT_BITMAP_HELVETICA_18);

	// Total Coins HUD
	iSetColor(15, 20, 30);
	iFilledRectangle(SCREEN_WIDTH - 260, 535, 230, 50);
	iSetColor(255, 200, 0);
	iRectangle(SCREEN_WIDTH - 260, 535, 230, 50);
	iSetColor(255, 215, 0);
	iFilledCircle(SCREEN_WIDTH - 238, 560, 14);
	char totalCoinStr[50];
	sprintf_s(totalCoinStr, sizeof(totalCoinStr), "TOTAL COINS: %d", totalCoins);
	iSetColor(255, 220, 0);
	iText(SCREEN_WIDTH - 215, 553, totalCoinStr, GLUT_BITMAP_HELVETICA_12);

	// Garage Button
	int gBtnX = 45, gBtnY = 8, gBtnW = 280, gBtnH = 44;
	iSetColor(220, 140, 20);
	iFilledRectangle(gBtnX, gBtnY, gBtnW, gBtnH);
	iSetColor(255, 255, 255);
	iRectangle(gBtnX, gBtnY, gBtnW, gBtnH);
	char gBtnStr[60];
	if (equippedRickshaw == 1) sprintf_s(gBtnStr, sizeof(gBtnStr), "GARAGE [G]: YELLOW EQUIPPED");
	else if (equippedRickshaw == 2) sprintf_s(gBtnStr, sizeof(gBtnStr), "GARAGE [G]: GREEN EQUIPPED");
	else sprintf_s(gBtnStr, sizeof(gBtnStr), "GARAGE [G]: BLUE (DEFAULT)");
	iText(gBtnX + 20, gBtnY + 15, gBtnStr, GLUT_BITMAP_HELVETICA_12);

	// Card 1: Station - Basabo
	int c1X = 45, c1Y = 380, c1W = 280, c1H = 95;
	int isHov1 = (mouseX >= c1X && mouseX <= c1X + c1W && mouseY >= c1Y && mouseY <= c1Y + c1H);
	if (selectedLevel == 1) {
		iSetColor(0, 160, 100); iFilledRectangle(c1X, c1Y, c1W, c1H);
		iSetColor(0, 255, 160); iRectangle(c1X, c1Y, c1W, c1H);
	}
	else if (isHov1) {
		iSetColor(35, 65, 85); iFilledRectangle(c1X, c1Y, c1W, c1H);
		iSetColor(0, 200, 255); iRectangle(c1X, c1Y, c1W, c1H);
	}
	else {
		iSetColor(28, 38, 55); iFilledRectangle(c1X, c1Y, c1W, c1H);
		iSetColor(50, 70, 95);  iRectangle(c1X, c1Y, c1W, c1H);
	}
	iSetColor(255, 255, 255);
	iText(c1X + 18, c1Y + 60, "1. Station - Basabo", GLUT_BITMAP_HELVETICA_18);
	iSetColor(120, 255, 160);
	iText(c1X + 18, c1Y + 35, "STATUS: UNLOCKED", GLUT_BITMAP_HELVETICA_12);
	iSetColor(220, 230, 240);
	iText(c1X + 18, c1Y + 15, "Standard Traffic | 20s Trip", GLUT_BITMAP_HELVETICA_10);

	// Card 2: Station - Moghbazar
	int c2X = 45, c2Y = 265, c2W = 280, c2H = 95;
	int isHov2 = (mouseX >= c2X && mouseX <= c2X + c2W && mouseY >= c2Y && mouseY <= c2Y + c2H);
	if (selectedLevel == 2) {
		if (level2Unlocked) {
			iSetColor(20, 120, 160); iFilledRectangle(c2X, c2Y, c2W, c2H);
			iSetColor(0, 220, 255);  iRectangle(c2X, c2Y, c2W, c2H);
		}
		else {
			iSetColor(140, 60, 30);  iFilledRectangle(c2X, c2Y, c2W, c2H);
			iSetColor(255, 140, 40); iRectangle(c2X, c2Y, c2W, c2H);
		}
	}
	else if (isHov2) {
		iSetColor(45, 45, 60);   iFilledRectangle(c2X, c2Y, c2W, c2H);
		iSetColor(200, 120, 80); iRectangle(c2X, c2Y, c2W, c2H);
	}
	else {
		iSetColor(28, 38, 55); iFilledRectangle(c2X, c2Y, c2W, c2H);
		iSetColor(50, 70, 95);  iRectangle(c2X, c2Y, c2W, c2H);
	}
	iSetColor(210, 215, 225);
	iText(c2X + 18, c2Y + 60, "2. Station - Moghbazar", GLUT_BITMAP_HELVETICA_18);
	if (level2Unlocked) {
		iSetColor(120, 255, 160);
		iText(c2X + 18, c2Y + 35, "STATUS: UNLOCKED", GLUT_BITMAP_HELVETICA_12);
	}
	else {
		char lockProgress[50];
		sprintf_s(lockProgress, sizeof(lockProgress), "STATUS: LOCKED (%d/10 Coins)", totalCoins < 10 ? totalCoins : 10);
		iSetColor(255, 140, 50);
		iText(c2X + 18, c2Y + 35, lockProgress, GLUT_BITMAP_HELVETICA_12);
	}
	iSetColor(180, 180, 180);
	iText(c2X + 18, c2Y + 15, "Expressway Rush | 3 Lives", GLUT_BITMAP_HELVETICA_10);

	// Card 3: Station - Aust
	int c3X = 45, c3Y = 150, c3W = 280, c3H = 95;
	int isHov3 = (mouseX >= c3X && mouseX <= c3X + c3W && mouseY >= c3Y && mouseY <= c3Y + c3H);
	if (selectedLevel == 3) {
		if (level3Unlocked) {
			iSetColor(80, 25, 85);   iFilledRectangle(c3X, c3Y, c3W, c3H);
			iSetColor(220, 60, 240); iRectangle(c3X, c3Y, c3W, c3H);
		}
		else {
			iSetColor(90, 30, 45);   iFilledRectangle(c3X, c3Y, c3W, c3H);
			iSetColor(255, 120, 50); iRectangle(c3X, c3Y, c3W, c3H);
		}
	}
	else if (isHov3) {
		iSetColor(45, 35, 60);   iFilledRectangle(c3X, c3Y, c3W, c3H);
		iSetColor(180, 80, 200); iRectangle(c3X, c3Y, c3W, c3H);
	}
	else {
		iSetColor(28, 38, 55); iFilledRectangle(c3X, c3Y, c3W, c3H);
		iSetColor(50, 70, 95);  iRectangle(c3X, c3Y, c3W, c3H);
	}
	iSetColor(210, 215, 225);
	iText(c3X + 18, c3Y + 60, "3. Station - Aust", GLUT_BITMAP_HELVETICA_18);
	if (level3Unlocked) {
		iSetColor(120, 255, 160);
		iText(c3X + 18, c3Y + 35, "STATUS: UNLOCKED", GLUT_BITMAP_HELVETICA_12);
		iSetColor(180, 180, 180);
		iText(c3X + 18, c3Y + 15, "Monsoon Night Drive | 5 Lives", GLUT_BITMAP_HELVETICA_10);
	}
	else {
		char lockProgress[60];
		if (totalCoins >= level3UnlockCost) {
			sprintf_s(lockProgress, sizeof(lockProgress), "UNLOCK: 10 COINS (Ready!)");
			iSetColor(255, 220, 50);
			iText(c3X + 18, c3Y + 35, lockProgress, GLUT_BITMAP_HELVETICA_12);
			iSetColor(140, 230, 160);
			iText(c3X + 18, c3Y + 15, "Click Card or Button to Unlock", GLUT_BITMAP_HELVETICA_10);
		}
		else {
			sprintf_s(lockProgress, sizeof(lockProgress), "STATUS: LOCKED (%d/10 Coins)", totalCoins);
			iSetColor(255, 80, 100);
			iText(c3X + 18, c3Y + 35, lockProgress, GLUT_BITMAP_HELVETICA_12);
			iSetColor(180, 180, 180);
			iText(c3X + 18, c3Y + 15, "Earn 10 Coins to Unlock", GLUT_BITMAP_HELVETICA_10);
		}
	}
	// Back Button
	int bkX = 45, bkY = 60, bkW = 280, bkH = 55;
	int isHovBk = (mouseX >= bkX && mouseX <= bkX + bkW && mouseY >= bkY && mouseY <= bkY + bkH);
	if (isHovBk) { iSetColor(70, 80, 100); iFilledRectangle(bkX, bkY, bkW, bkH); }
	else { iSetColor(35, 45, 65); iFilledRectangle(bkX, bkY, bkW, bkH); }
	iSetColor(255, 255, 255);
	iText(bkX + 65, bkY + 20, "<- BACK TO STORY", GLUT_BITMAP_HELVETICA_12);

	// Preview Frame
	int fX = 365, fY = 60, fW = 490, fH = 415;
	iSetColor(24, 30, 44); iFilledRectangle(fX, fY, fW, fH);
	iSetColor(50, 70, 100); iRectangle(fX, fY, fW, fH);
	iSetColor(32, 42, 60); iFilledRectangle(fX + 15, fY + fH - 45, fW - 30, 32);
	iSetColor(0, 220, 255);
	if (selectedLevel == 1) iText(fX + 25, fY + fH - 33, "STAGE PREVIEW : STATION - BASABO", GLUT_BITMAP_HELVETICA_12);
	else if (selectedLevel == 2) iText(fX + 25, fY + fH - 33, "STAGE PREVIEW : STATION - MOGHBAZAR", GLUT_BITMAP_HELVETICA_12);
	else if (level3Unlocked) iText(fX + 25, fY + fH - 33, "STAGE PREVIEW : STATION - AUST", GLUT_BITMAP_HELVETICA_12);
	else iText(fX + 25, fY + fH - 33, "STAGE PREVIEW : STATION - AUST [LOCKED]", GLUT_BITMAP_HELVETICA_12);

	int pvX = fX + 15, pvY = fY + 95, pvW = fW - 30, pvH = 260;
	if (selectedLevel == 1) drawCityDrivePreview(pvX, pvY, pvW, pvH);
	else if (selectedLevel == 2) drawRushHourPreview(pvX, pvY, pvW, pvH);
	else drawHighwayChasePreview(pvX, pvY, pvW, pvH);
	iSetColor(80, 105, 140); iRectangle(pvX, pvY, pvW, pvH);

	int isPlayable = (selectedLevel == 1) || (selectedLevel == 2 && level2Unlocked) || (selectedLevel == 3 && level3Unlocked);
	int canUnlockL3 = (selectedLevel == 3 && !level3Unlocked && totalCoins >= level3UnlockCost);

	if (isPlayable) {
		int playBtnX = fX + fW - 195, playBtnY = fY + 20, playBtnW = 180, playBtnH = 55;
		int isHovPlay = (mouseX >= playBtnX && mouseX <= playBtnX + playBtnW && mouseY >= playBtnY && mouseY <= playBtnY + playBtnH);
		if (isHovPlay) iSetColor(0, 220, 120);
		else iSetColor(0, 170, 90);
		iFilledRectangle(playBtnX, playBtnY, playBtnW, playBtnH);
		iSetColor(255, 255, 255);
		iText(playBtnX + 28, playBtnY + 20, "START TRIP >", GLUT_BITMAP_HELVETICA_18);
		iSetColor(180, 200, 220);
		iText(fX + 20, fY + 56, "Survive to reach destination", GLUT_BITMAP_HELVETICA_12);
		iText(fX + 20, fY + 38, "& earn agreed fare reward coins!", GLUT_BITMAP_HELVETICA_10);
		iSetColor(130, 160, 190);
		iText(fX + 20, fY + 20, "Controls: UP / DOWN / A / D", GLUT_BITMAP_HELVETICA_10);
	}
	else if (canUnlockL3) {
		int playBtnX = fX + fW - 215, playBtnY = fY + 20, playBtnW = 200, playBtnH = 55;
		int isHovPlay = (mouseX >= playBtnX && mouseX <= playBtnX + playBtnW && mouseY >= playBtnY && mouseY <= playBtnY + playBtnH);
		if (isHovPlay) iSetColor(0, 220, 120);
		else iSetColor(215, 140, 20);
		iFilledRectangle(playBtnX, playBtnY, playBtnW, playBtnH);
		iSetColor(255, 255, 255);
		iRectangle(playBtnX, playBtnY, playBtnW, playBtnH);
		iText(playBtnX + 12, playBtnY + 20, "UNLOCK (10 COINS)", GLUT_BITMAP_HELVETICA_18);
		iSetColor(255, 220, 100);
		iText(fX + 20, fY + 56, "Spend 10 coins to unlock", GLUT_BITMAP_HELVETICA_12);
		iText(fX + 20, fY + 38, "Station - Aust permanently!", GLUT_BITMAP_HELVETICA_10);
		iSetColor(180, 200, 220);
		iText(fX + 20, fY + 20, "Click UNLOCK or press [ENTER]", GLUT_BITMAP_HELVETICA_10);
	}
	else {
		int playBtnX = fX + fW - 195, playBtnY = fY + 20, playBtnW = 180, playBtnH = 55;
		iSetColor(180, 80, 80);
		iFilledRectangle(playBtnX, playBtnY, playBtnW, playBtnH);
		iSetColor(255, 255, 255);
		iText(playBtnX + 22, playBtnY + 20, "STAGE LOCKED", GLUT_BITMAP_HELVETICA_18);
		iSetColor(255, 140, 140);
		iText(fX + 20, fY + 56, "Station - Aust is locked.", GLUT_BITMAP_HELVETICA_12);
		iSetColor(180, 200, 220);
		iText(fX + 20, fY + 38, "Earn 10 coins to unlock this stage.", GLUT_BITMAP_HELVETICA_10);
		iSetColor(130, 160, 190);
		iText(fX + 20, fY + 20, "Complete Level 1 or 2 to earn coins", GLUT_BITMAP_HELVETICA_10);
	}
}

void drawGameOverScreen() {
	iSetColor(0, 0, 0);
	iFilledRectangle(SCREEN_WIDTH / 2 - 210, SCREEN_HEIGHT / 2 - 130, 420, 260);
	iSetColor(220, 30, 30);
	iRectangle(SCREEN_WIDTH / 2 - 210, SCREEN_HEIGHT / 2 - 130, 420, 260);
	iSetColor(255, 50, 50);
	iText(SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 70, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(255, 255, 255);
	iText(SCREEN_WIDTH / 2 - 145, SCREEN_HEIGHT / 2 + 35, "FAILED TO REACH DESTINATION!", GLUT_BITMAP_HELVETICA_18);
	char goCoinStr[50];
	sprintf_s(goCoinStr, sizeof(goCoinStr), "Coins Collected: %d  |  Total: %d", currentRunCoins, totalCoins);
	iSetColor(255, 215, 0);
	iText(SCREEN_WIDTH / 2 - 145, SCREEN_HEIGHT / 2 + 5, goCoinStr, GLUT_BITMAP_HELVETICA_12);

	int rBtnX = SCREEN_WIDTH / 2 - 170, rBtnY = SCREEN_HEIGHT / 2 - 50;
	iSetColor(0, 150, 70);
	iFilledRectangle(rBtnX, rBtnY, 150, 45);
	iSetColor(255, 255, 255);
	iRectangle(rBtnX, rBtnY, 150, 45);
	iText(rBtnX + 22, rBtnY + 16, "RETRY (R)", GLUT_BITMAP_HELVETICA_12);

	int mBtnX = SCREEN_WIDTH / 2 + 20, mBtnY = SCREEN_HEIGHT / 2 - 50;
	iSetColor(40, 80, 150);
	iFilledRectangle(mBtnX, mBtnY, 150, 45);
	iSetColor(255, 255, 255);
	iRectangle(mBtnX, mBtnY, 150, 45);
	iText(mBtnX + 22, mBtnY + 16, "MENU (M)", GLUT_BITMAP_HELVETICA_12);
}

// =========================================================================
// LEVEL 2 PROCEDURAL RENDERING & LOGIC (EXACT FROM YOUR STANDALONE CODE)
// =========================================================================

// Unified perspective center X coordinate on the road
double getEntityCenterXL2(float normX, double y) {
	double t = (double)(horizonYL2 - y) / horizonYL2;
	if (t < 0.0) t = 0.0;
	double leftRoadX = 395.0 - 335.0 * t;
	double rightRoadX = 505.0 + 335.0 * t;
	return leftRoadX + (double)normX * (rightRoadX - leftRoadX);
}

// Perspective scale factor (small at horizon, full size at bottom)
double getPerspectiveScaleL2(double y) {
	double t = (double)(horizonYL2 - y) / horizonYL2;
	if (t < 0.0) t = 0.0;
	return 0.22 + 0.78 * t;
}

// Perspective X coordinate for roadside props on sidewalks outside asphalt
double getRoadsideXL2(int side, float lateralOffset, double y) {
	double t = (double)(horizonYL2 - y) / horizonYL2;
	if (t < 0.0) t = 0.0;
	double leftRoadX = 395.0 - 335.0 * t;
	double rightRoadX = 505.0 + 335.0 * t;
	double scale = getPerspectiveScaleL2(y);
	if (side == 0) {
		return leftRoadX - (16.0 + (double)lateralOffset * 34.0) * scale;
	}
	else {
		return rightRoadX + (16.0 + (double)lateralOffset * 34.0) * scale;
	}
}

// Perspective X coordinate for outer promenade
double getOuterPromenadeXL2(int side, float offsetRatio, double y) {
	double t = (double)(horizonYL2 - y) / horizonYL2;
	if (t < 0.0) t = 0.0;
	if (side == 0) {
		double railX = 375.0 * (1.0 - t);
		if (railX < 24.0) railX = 24.0;
		return 12.0 + (double)offsetRatio * (railX - 24.0);
	}
	else {
		double railX = 525.0 + 375.0 * t;
		if (railX > 876.0) railX = 876.0;
		return railX + 12.0 + (double)offsetRatio * (900.0 - railX - 24.0);
	}
}

// Physical base widths
float getEntityBaseWidthL2(int entityType) {
	if (entityType == 0) return 104.0f; // Bus
	if (entityType == 3) return 82.0f;  // Traffic Rickshaw
	if (entityType == 1) return 30.0f;  // Person
	return 24.0f;                       // Dog
}

// Physical base heights
float getEntityBaseHeightL2(int entityType) {
	if (entityType == 0) return 138.0f; // Bus
	if (entityType == 3) return 112.0f; // Traffic Rickshaw
	if (entityType == 1) return 105.0f; // Person
	return 34.0f;                       // Dog
}

// Safe spawn position check for vehicles
int isSpawnPositionSafeL2(float testNormX, int testType) {
	float testW = getEntityBaseWidthL2(testType);
	float testNormW = testW / 780.0f;

	for (int i = 0; i < MAX_BUSES; i++) {
		if (buses[i].active && buses[i].y > 210.0f) {
			float otherNormW = getEntityBaseWidthL2(0) / 780.0f;
			float safeSep = (testNormW + otherNormW) * 0.5f + 0.05f;
			if ((float)fabs(testNormX - buses[i].normX) < safeSep) return 0;
		}
	}

	for (int i = 0; i < MAX_TRAFFIC_RICKSHAWS; i++) {
		if (otherRickshaws[i].active && otherRickshaws[i].y > 210.0f) {
			float otherNormW = getEntityBaseWidthL2(3) / 780.0f;
			float safeSep = (testNormW + otherNormW) * 0.5f + 0.05f;
			if ((float)fabs(testNormX - otherRickshaws[i].normX) < safeSep) return 0;
		}
	}

	return 1;
}

// Safe spawn check for coins to never collide/overlap with obstacles
int isCoinSpawnSafeL2(float testNormX) {
	for (int i = 0; i < MAX_BUSES; i++) {
		if (buses[i].active && buses[i].y > 220.0f) {
			if ((float)fabs(testNormX - buses[i].normX) < 0.14f) return 0;
		}
	}
	for (int i = 0; i < MAX_TRAFFIC_RICKSHAWS; i++) {
		if (otherRickshaws[i].active && otherRickshaws[i].y > 220.0f) {
			if ((float)fabs(testNormX - otherRickshaws[i].normX) < 0.12f) return 0;
		}
	}
	return 1;
}

void spawnBusL2() {
	for (int i = 0; i < MAX_BUSES; i++) {
		if (!buses[i].active) {
			for (int attempt = 0; attempt < 20; attempt++) {
				float candidateNormX = 0.16f + ((rand() % 1000) / 1000.0f) * (0.84f - 0.16f);
				if (isSpawnPositionSafeL2(candidateNormX, 0)) {
					buses[i].active = 1;
					buses[i].y = (float)horizonYL2;
					buses[i].normX = candidateNormX;
					buses[i].speed = 1.35f + ((rand() % 6) / 10.0f);

					int nextColor;
					do {
						nextColor = rand() % 8;
					} while (nextColor == lastBusColor);
					lastBusColor = nextColor;
					buses[i].colorTheme = nextColor;
					return;
				}
			}
			busSpawnCounter = BUS_SPAWN_INTERVAL - 25;
			return;
		}
	}
}

void spawnTrafficRickshawL2() {
	for (int i = 0; i < MAX_TRAFFIC_RICKSHAWS; i++) {
		if (!otherRickshaws[i].active) {
			for (int attempt = 0; attempt < 20; attempt++) {
				float candidateNormX = 0.14f + ((rand() % 1000) / 1000.0f) * (0.86f - 0.14f);
				if (isSpawnPositionSafeL2(candidateNormX, 3)) {
					otherRickshaws[i].active = 1;
					otherRickshaws[i].y = (float)horizonYL2;
					otherRickshaws[i].normX = candidateNormX;
					otherRickshaws[i].speed = 1.05f + ((rand() % 4) / 10.0f);
					otherRickshaws[i].pedalAnim = (float)(rand() % 100);
					otherRickshaws[i].hoodColor = rand() % 4;
					return;
				}
			}
			rickshawSpawnCounter = RICKSHAW_SPAWN_INTERVAL - 25;
			return;
		}
	}
}

void spawnPersonL2() {
	for (int i = 0; i < MAX_PEOPLE; i++) {
		if (!people[i].active) {
			people[i].active = 1;
			people[i].y = (float)horizonYL2;
			people[i].normX = (rand() % 2 == 0) ? 0.07f : 0.93f; // Walks in road corners
			people[i].speed = 0.70f + ((rand() % 5) / 10.0f);
			people[i].walkAnim = (float)(rand() % 100);
			people[i].outfitTheme = rand() % 4;
			return;
		}
	}
}

void spawnDogL2() {
	for (int i = 0; i < MAX_DOGS; i++) {
		if (!dogs[i].active) {
			dogs[i].active = 1;
			dogs[i].y = (float)horizonYL2;
			dogs[i].normX = (rand() % 2 == 0) ? 0.06f : 0.94f; // Trots in road corners
			dogs[i].speed = 1.35f + ((rand() % 5) / 10.0f);
			dogs[i].trotAnim = (float)(rand() % 100);
			dogs[i].coatColor = rand() % 4;
			return;
		}
	}
}

// Spawns moderate realistic gold coins along road
void spawnNormalCoinL2() {
	for (int i = 0; i < MAX_COINS_L2; i++) {
		if (!coinsL2[i].active) {
			for (int attempt = 0; attempt < 15; attempt++) {
				float candidateNormX = 0.18f + ((rand() % 1000) / 1000.0f) * (0.82f - 0.18f);
				if (isCoinSpawnSafeL2(candidateNormX)) {
					coinsL2[i].active = 1;
					coinsL2[i].isSpecial = 0;
					coinsL2[i].y = (float)horizonYL2;
					coinsL2[i].normX = candidateNormX;
					coinsL2[i].spinAnim = (float)(rand() % 100);
					return;
				}
			}
			return;
		}
	}
}

// Spawns powerful special coin every 4 seconds (+5 value)
void spawnSpecialCoinL2() {
	for (int i = 0; i < MAX_COINS_L2; i++) {
		if (!coinsL2[i].active) {
			for (int attempt = 0; attempt < 20; attempt++) {
				float candidateNormX = 0.22f + ((rand() % 1000) / 1000.0f) * (0.78f - 0.22f);
				if (isCoinSpawnSafeL2(candidateNormX)) {
					coinsL2[i].active = 1;
					coinsL2[i].isSpecial = 1;
					coinsL2[i].y = (float)horizonYL2;
					coinsL2[i].normX = candidateNormX;
					coinsL2[i].spinAnim = 0.0f;
					return;
				}
			}
			return;
		}
	}
}

// Sets the types of props for the active theme while keeping their positions constant
void applyRoadsideTheme(int theme) {
	if (theme == 0) {
		// Theme 0: Traditional Stalls & Street Food Bazaar
		outerProps[0].type = 0; outerProps[0].variant = 0; // Tong Tea Stall
		outerProps[1].type = 5; outerProps[1].variant = 0; // Promenade Dustbin
		outerProps[2].type = 1; outerProps[2].variant = 0; // Fuchka Cart
		outerProps[3].type = 3; outerProps[3].variant = 0; // Park Bench
		outerProps[4].type = 2; outerProps[4].variant = 0; // Fruit Vendor Cart
		outerProps[5].type = 5; outerProps[5].variant = 0; // Promenade Dustbin
		outerProps[6].type = 6; outerProps[6].variant = 0; // News Kiosk
		outerProps[7].type = 0; outerProps[7].variant = 1; // Tong Tea Stall
	}
	else if (theme == 1) {
		// Theme 1: City Parks with Kids & Playgrounds
		outerProps[0].type = 4; outerProps[0].variant = 0; // Kids Playing Soccer with Ball
		outerProps[1].type = 3; outerProps[1].variant = 0; // Park Bench
		outerProps[2].type = 7; outerProps[2].variant = 0; // Playground Slide & Swing Set
		outerProps[3].type = 3; outerProps[3].variant = 1; // Park Bench
		outerProps[4].type = 4; outerProps[4].variant = 1; // Kids Playing in Park
		outerProps[5].type = 3; outerProps[5].variant = 0; // Park Bench
		outerProps[6].type = 7; outerProps[6].variant = 1; // Playground Slide & Swing Set
		outerProps[7].type = 4; outerProps[7].variant = 0; // Kids Playing
	}
	else {
		// Theme 2: Commercial Promenade & News Kiosks
		outerProps[0].type = 6; outerProps[0].variant = 0; // News Kiosk
		outerProps[1].type = 5; outerProps[1].variant = 0; // Recycling Dustbin
		outerProps[2].type = 2; outerProps[2].variant = 0; // Fruit Vendor Cart
		outerProps[3].type = 3; outerProps[3].variant = 0; // Park Bench
		outerProps[4].type = 6; outerProps[4].variant = 1; // News Kiosk
		outerProps[5].type = 5; outerProps[5].variant = 0; // Recycling Dustbin
		outerProps[6].type = 1; outerProps[6].variant = 1; // Fuchka Cart
		outerProps[7].type = 6; outerProps[7].variant = 0; // News Kiosk
	}
}

// Reset and initialize game objects for Level 2
void initGameObjectsL2() {
	playerLivesL2 = 3;
	invulnerableTimerL2 = 0;
	hitFlashTimerL2 = 0;
	isGameOverL2 = 0;
	currentSpeedL2 = baseSpeedL2;
	rickshawXL2 = 400;

	sfxTimerL2 = 0;
	PlaySound("Audios\\bg.wav", NULL, SND_ASYNC | SND_LOOP);

	currentRoadsideTheme = 0;
	themeCycleCounter = 0;

	coinCountL2 = 0;
	normalCoinSpawnCounter = 0;
	specialCoinSpawnCounter = 0;
	for (int i = 0; i < MAX_COINS_L2; i++) coinsL2[i].active = 0;

	// Start with one visible coin ahead
	coinsL2[0].active = 1;
	coinsL2[0].isSpecial = 0;
	coinsL2[0].y = 210.0f;
	coinsL2[0].normX = 0.48f;
	coinsL2[0].spinAnim = 0.0f;

	for (int i = 0; i < MAX_BUSES; i++) buses[i].active = 0;
	for (int i = 0; i < MAX_TRAFFIC_RICKSHAWS; i++) otherRickshaws[i].active = 0;
	for (int i = 0; i < MAX_PEOPLE; i++) people[i].active = 0;
	for (int i = 0; i < MAX_DOGS; i++) dogs[i].active = 0;

	buses[0].active = 1;
	buses[0].y = 220.0f;
	buses[0].normX = 0.72f;
	buses[0].speed = 1.4f;
	buses[0].colorTheme = rand() % 8;
	lastBusColor = buses[0].colorTheme;

	otherRickshaws[0].active = 1;
	otherRickshaws[0].y = 150.0f;
	otherRickshaws[0].normX = 0.28f;
	otherRickshaws[0].speed = 1.15f;
	otherRickshaws[0].hoodColor = rand() % 4;
	otherRickshaws[0].pedalAnim = 0.0f;

	int leftTypes[] = { 2, 0, 4, 2, 5, 0, 6 };
	int rightTypes[] = { 3, 1, 7, 3, 1, 4, 6 };

	for (int i = 0; i < 7; i++) {
		roadsideProps[i].active = 1;
		roadsideProps[i].side = 0;
		roadsideProps[i].y = 285.0f - (float)i * 42.0f;
		roadsideProps[i].type = leftTypes[i];
		roadsideProps[i].variant = rand() % 3;
		roadsideProps[i].lateralOffset = (roadsideProps[i].type <= 1) ? 1.0f : 0.4f;

		roadsideProps[7 + i].active = 1;
		roadsideProps[7 + i].side = 1;
		roadsideProps[7 + i].y = 270.0f - (float)i * 42.0f;
		roadsideProps[7 + i].type = rightTypes[i];
		roadsideProps[7 + i].variant = rand() % 3;
		roadsideProps[7 + i].lateralOffset = (roadsideProps[7 + i].type <= 1) ? 1.0f : 0.4f;
	}

	// Constant sidewalk positions for outer props (never move down or come out of screen)
	outerProps[0].active = 1; outerProps[0].side = 0; outerProps[0].y = 225.0f; outerProps[0].offsetRatio = 0.46f;
	outerProps[1].active = 1; outerProps[1].side = 0; outerProps[1].y = 210.0f; outerProps[1].offsetRatio = 0.82f;
	outerProps[2].active = 1; outerProps[2].side = 0; outerProps[2].y = 150.0f; outerProps[2].offsetRatio = 0.42f;
	outerProps[3].active = 1; outerProps[3].side = 0; outerProps[3].y = 280.0f; outerProps[3].offsetRatio = 0.50f;

	outerProps[4].active = 1; outerProps[4].side = 1; outerProps[4].y = 230.0f; outerProps[4].offsetRatio = 0.48f;
	outerProps[5].active = 1; outerProps[5].side = 1; outerProps[5].y = 215.0f; outerProps[5].offsetRatio = 0.82f;
	outerProps[6].active = 1; outerProps[6].side = 1; outerProps[6].y = 155.0f; outerProps[6].offsetRatio = 0.45f;
	outerProps[7].active = 1; outerProps[7].side = 1; outerProps[7].y = 280.0f; outerProps[7].offsetRatio = 0.50f;

	applyRoadsideTheme(0);
}

// Procedural realistic bus with 8 vibrant color themes
void drawRealisticBus(double centerX, double baseY, double scale, int colorTheme) {
	if (scale <= 0.05) return;

	double baseW = 104.0;
	double baseH = 138.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = centerX - w / 2.0;
	double by = baseY + 7.0 * scale;

	iSetColor(30, 32, 35);
	double shX[] = { centerX - w * 0.53, centerX + w * 0.53, centerX + w * 0.47, centerX - w * 0.47 };
	double shY[] = { baseY - 3.0 * scale, baseY - 3.0 * scale, baseY + 8.0 * scale, baseY + 8.0 * scale };
	iFilledPolygon(shX, shY, 4);

	iSetColor(20, 20, 22);
	drawRect(centerX - w * 0.45, baseY, w * 0.17, 18.0 * scale);
	iSetColor(90, 95, 100);
	drawRect(centerX - w * 0.42, baseY + 2.5 * scale, w * 0.11, 12.0 * scale);
	iSetColor(180, 185, 190);
	drawRect(centerX - w * 0.39, baseY + 5.5 * scale, w * 0.045, 6.0 * scale);

	iSetColor(20, 20, 22);
	drawRect(centerX + w * 0.28, baseY, w * 0.17, 18.0 * scale);
	iSetColor(90, 95, 100);
	drawRect(centerX + w * 0.31, baseY + 2.5 * scale, w * 0.11, 12.0 * scale);
	iSetColor(180, 185, 190);
	drawRect(centerX + w * 0.345, baseY + 5.5 * scale, w * 0.045, 6.0 * scale);

	iSetColor(40, 42, 45);
	drawRect(bx, by, w, 10.0 * scale);

	int r1 = 195, g1 = 30, b1 = 35;
	int r2 = 245, g2 = 245, b2 = 245;
	int r3 = 245, g3 = 195, b3 = 25;

	if (colorTheme == 1) {
		r1 = 25;  g1 = 135; b1 = 65;
		r2 = 240; g2 = 245; b2 = 235;
		r3 = 250; g3 = 210; b3 = 30;
	}
	else if (colorTheme == 2) {
		r1 = 25;  g1 = 85;  b1 = 185;
		r2 = 240; g2 = 245; b2 = 255;
		r3 = 60;  g3 = 195; b3 = 245;
	}
	else if (colorTheme == 3) {
		r1 = 235; g1 = 175; b1 = 25;
		r2 = 35;  g2 = 35;  b2 = 40;
		r3 = 255; g3 = 255; b3 = 255;
	}
	else if (colorTheme == 4) {
		r1 = 225; g1 = 95;  b1 = 25;
		r2 = 245; g2 = 245; b2 = 245;
		r3 = 50;  g3 = 55;  b3 = 60;
	}
	else if (colorTheme == 5) {
		r1 = 110; g1 = 35;  b1 = 145;
		r2 = 235; g2 = 230; b2 = 245;
		r3 = 255; g3 = 205; b3 = 35;
	}
	else if (colorTheme == 6) {
		r1 = 20;  g1 = 145; b1 = 145;
		r2 = 240; g2 = 248; b2 = 250;
		r3 = 245; g3 = 115; b3 = 35;
	}
	else if (colorTheme == 7) {
		r1 = 235; g1 = 238; b1 = 242;
		r2 = 30;  g2 = 70;  b2 = 140;
		r3 = 215; g3 = 40;  b3 = 45;
	}

	double chamfer = 10.0 * scale;
	double bodyX[] = { bx, bx + w, bx + w, bx + w - chamfer, bx + chamfer, bx };
	double bodyY[] = { by + 9.0 * scale, by + 9.0 * scale, by + h - chamfer, by + h, by + h, by + h - chamfer };
	iSetColor(r1, g1, b1);
	iFilledPolygon(bodyX, bodyY, 6);

	iSetColor((int)(r1 * 0.75), (int)(g1 * 0.75), (int)(b1 * 0.75));
	drawRect(bx, by + 9.0 * scale, 5.0 * scale, h - 18.0 * scale);
	drawRect(bx + w - 5.0 * scale, by + 9.0 * scale, 5.0 * scale, h - 18.0 * scale);

	iSetColor(r3, g3, b3);
	drawRect(bx, by + 35.0 * scale, w, 5.0 * scale);

	iSetColor(r2, g2, b2);
	double roofX[] = { bx + 4.0 * scale, bx + w - 4.0 * scale, bx + w - chamfer, bx + chamfer };
	double roofY[] = { by + h - 12.0 * scale, by + h - 12.0 * scale, by + h, by + h };
	iFilledPolygon(roofX, roofY, 4);

	iSetColor(15, 15, 18);
	drawRect(centerX - w * 0.37, by + h - 22.0 * scale, w * 0.74, 10.0 * scale);
	iSetColor(255, 175, 20);
	drawRect(centerX - w * 0.34, by + h - 20.0 * scale, w * 0.68, 6.0 * scale);
	iSetColor(220, 35, 35);
	drawRect(centerX - w * 0.34, by + h - 20.0 * scale, w * 0.15, 6.0 * scale);

	iSetColor(20, 22, 25);
	drawRect(centerX - w * 0.44, by + 44.0 * scale, w * 0.88, h * 0.42);
	iSetColor(35, 75, 110);
	drawRect(centerX - w * 0.41, by + 46.0 * scale, w * 0.82, h * 0.38);

	iSetColor(110, 165, 205);
	double glareX[] = { centerX - w * 0.38, centerX - w * 0.18, centerX - w * 0.26, centerX - w * 0.41 };
	double glareY[] = { by + 46.0 * scale, by + 46.0 * scale + h * 0.38, by + 46.0 * scale + h * 0.38, by + 46.0 * scale + 9.0 * scale };
	iFilledPolygon(glareX, glareY, 4);

	iSetColor(25, 32, 40);
	iFilledCircle(centerX + w * 0.21, by + 46.0 * scale + h * 0.21, 4.8 * scale);
	drawRect(centerX + w * 0.14, by + 46.0 * scale + 3.0 * scale, 12.0 * scale, 10.0 * scale);
	iFilledCircle(centerX - w * 0.23, by + 46.0 * scale + h * 0.19, 4.2 * scale);

	iSetColor(15, 15, 15);
	iLine(centerX - w * 0.31, by + 47.0 * scale, centerX - w * 0.11, by + 56.0 * scale);
	iLine(centerX + w * 0.08, by + 47.0 * scale, centerX + w * 0.28, by + 56.0 * scale);

	iSetColor(28, 30, 34);
	drawRect(centerX - w * 0.23, by + 18.0 * scale, w * 0.46, 16.0 * scale);
	iSetColor(155, 160, 170);
	drawRect(centerX - w * 0.20, by + 21.0 * scale, w * 0.40, 1.8 * scale);
	drawRect(centerX - w * 0.20, by + 25.0 * scale, w * 0.40, 1.8 * scale);
	drawRect(centerX - w * 0.20, by + 29.0 * scale, w * 0.40, 1.8 * scale);
	iSetColor(230, 235, 245);
	iFilledCircle(centerX, by + 26.0 * scale, 2.8 * scale);

	iSetColor(45, 48, 52);
	drawRect(centerX - w * 0.45, by + 17.0 * scale, w * 0.19, 16.0 * scale);
	iSetColor(255, 255, 230);
	drawRect(centerX - w * 0.39, by + 19.0 * scale, w * 0.11, 12.0 * scale);
	iSetColor(255, 145, 0);
	drawRect(centerX - w * 0.44, by + 19.0 * scale, w * 0.045, 12.0 * scale);

	iSetColor(45, 48, 52);
	drawRect(centerX + w * 0.26, by + 17.0 * scale, w * 0.19, 16.0 * scale);
	iSetColor(255, 255, 230);
	drawRect(centerX + w * 0.28, by + 19.0 * scale, w * 0.11, 12.0 * scale);
	iSetColor(255, 145, 0);
	drawRect(centerX + w * 0.395, by + 19.0 * scale, w * 0.045, 12.0 * scale);

	iSetColor(40, 42, 46);
	drawRect(bx + 2.5 * scale, by + 4.0 * scale, w - 5.0 * scale, 13.0 * scale);

	iSetColor(255, 235, 150);
	drawRect(centerX - w * 0.36, by + 6.5 * scale, 6.5 * scale, 5.0 * scale);
	drawRect(centerX + w * 0.36 - 6.5 * scale, by + 6.5 * scale, 6.5 * scale, 5.0 * scale);

	iSetColor(20, 115, 55);
	drawRect(centerX - 12.0 * scale, by + 5.5 * scale, 24.0 * scale, 9.0 * scale);
	iSetColor(255, 255, 255);
	drawRect(centerX - 10.0 * scale, by + 7.0 * scale, 20.0 * scale, 6.0 * scale);

	iSetColor(25, 25, 28);
	iLine(bx + 5.0 * scale, by + h - 30.0 * scale, bx - 10.0 * scale, by + h - 40.0 * scale);
	drawRect(bx - 12.0 * scale, by + h - 50.0 * scale, 6.5 * scale, 17.0 * scale);
	iSetColor(210, 225, 235);
	drawRect(bx - 10.5 * scale, by + h - 48.0 * scale, 4.0 * scale, 13.5 * scale);

	iSetColor(25, 25, 28);
	iLine(bx + w - 5.0 * scale, by + h - 30.0 * scale, bx + w + 10.0 * scale, by + h - 40.0 * scale);
	drawRect(bx + w + 5.5 * scale, by + h - 50.0 * scale, 6.5 * scale, 17.0 * scale);
	iSetColor(210, 225, 235);
	drawRect(bx + w + 6.5 * scale, by + h - 48.0 * scale, 4.0 * scale, 13.5 * scale);

	iSetColor(255, 160, 20);
	drawRect(centerX - 13.0 * scale, by + h - 3.5 * scale, 4.0 * scale, 2.5 * scale);
	drawRect(centerX - 2.0 * scale, by + h - 3.5 * scale, 4.0 * scale, 2.5 * scale);
	drawRect(centerX + 9.0 * scale, by + h - 3.5 * scale, 4.0 * scale, 2.5 * scale);
}

// Procedural realistic traffic rickshaw
void drawRealisticRickshawL2(double centerX, double baseY, double scale, float pedalAnim, int hoodColor) {
	if (scale <= 0.05) return;

	double baseW = 82.0;
	double baseH = 112.0;
	double w = baseW * scale;
	double h = baseH * scale;

	float pedal = (float)sin(pedalAnim);

	iSetColor(30, 32, 35);
	double shX[] = { centerX - w * 0.50, centerX + w * 0.50, centerX + w * 0.42, centerX - w * 0.42 };
	double shY[] = { baseY - 2.5 * scale, baseY - 2.5 * scale, baseY + 6.0 * scale, baseY + 6.0 * scale };
	iFilledPolygon(shX, shY, 4);

	int hoodR = 25, hoodG = 135, hoodB = 55;
	int accentR = 215, accentG = 35, accentB = 40;
	if (hoodColor == 1) {
		hoodR = 245; hoodG = 195; hoodB = 25;
		accentR = 25; accentG = 85; accentB = 185;
	}
	else if (hoodColor == 2) {
		hoodR = 205; hoodG = 30; hoodB = 40;
		accentR = 255; accentG = 215; accentB = 30;
	}
	else if (hoodColor == 3) {
		hoodR = 20; hoodG = 145; hoodB = 145;
		accentR = 245; accentG = 110; accentB = 30;
	}

	iSetColor(22, 22, 25);
	drawRect(centerX - w * 0.48, baseY + 4.0 * scale, 5.5 * scale, 30.0 * scale);
	iSetColor(hoodR, hoodG, hoodB);
	drawRect(centerX - w * 0.50, baseY + 18.0 * scale, 7.5 * scale, 17.0 * scale);
	iSetColor(230, 30, 30);
	drawRect(centerX - w * 0.49, baseY + 22.0 * scale, 5.5 * scale, 5.5 * scale);

	iSetColor(22, 22, 25);
	drawRect(centerX + w * 0.48 - 5.5 * scale, baseY + 4.0 * scale, 5.5 * scale, 30.0 * scale);
	iSetColor(hoodR, hoodG, hoodB);
	drawRect(centerX + w * 0.50 - 7.5 * scale, baseY + 18.0 * scale, 7.5 * scale, 17.0 * scale);
	iSetColor(230, 30, 30);
	drawRect(centerX + w * 0.50 - 6.5 * scale, baseY + 22.0 * scale, 5.5 * scale, 5.5 * scale);

	double hoodX[] = {
		centerX - w * 0.42,
		centerX + w * 0.42,
		centerX + w * 0.38,
		centerX + w * 0.22,
		centerX - w * 0.22,
		centerX - w * 0.38
	};
	double hoodY[] = {
		baseY + 54.0 * scale,
		baseY + 54.0 * scale,
		baseY + h - 6.0 * scale,
		baseY + h,
		baseY + h,
		baseY + h - 6.0 * scale
	};
	iSetColor(hoodR, hoodG, hoodB);
	iFilledPolygon(hoodX, hoodY, 6);

	iSetColor(accentR, accentG, accentB);
	drawRect(centerX - w * 0.38, baseY + 75.0 * scale, w * 0.76, 7.0 * scale);

	iSetColor(35, 38, 42);
	drawRect(centerX - w * 0.32, baseY + 55.0 * scale, w * 0.64, 26.0 * scale);
	iSetColor(185, 30, 45);
	drawRect(centerX - w * 0.28, baseY + 55.0 * scale, w * 0.56, 11.0 * scale);

	iSetColor(45, 48, 52);
	drawRect(centerX - w * 0.36, baseY + 32.0 * scale, w * 0.72, 22.0 * scale);
	iSetColor(245, 225, 55);
	drawRect(centerX - w * 0.28, baseY + 34.0 * scale, w * 0.56, 17.0 * scale);
	iSetColor(220, 35, 45);
	iFilledCircle(centerX, baseY + 42.5 * scale, 4.5 * scale);
	iSetColor(35, 145, 55);
	drawRect(centerX - 5.5 * scale, baseY + 41.0 * scale, 11.0 * scale, 3.0 * scale);

	iSetColor(230, 230, 235);
	double torsoX[] = { centerX - 8.0 * scale, centerX + 8.0 * scale, centerX + 10.0 * scale, centerX - 10.0 * scale };
	double torsoY[] = { baseY + 46.0 * scale, baseY + 46.0 * scale, baseY + 69.0 * scale, baseY + 69.0 * scale };
	iFilledPolygon(torsoX, torsoY, 4);

	iSetColor(235, 60, 60);
	drawRect(centerX - 7.5 * scale, baseY + 65.0 * scale, 15.0 * scale, 3.5 * scale);

	iSetColor(230, 230, 235);
	drawRect(centerX - 13.0 * scale, baseY + 56.0 * scale, 4.5 * scale, 10.0 * scale);
	drawRect(centerX + 8.5 * scale, baseY + 56.0 * scale, 4.5 * scale, 10.0 * scale);

	iSetColor(190, 195, 205);
	drawRect(centerX - 16.0 * scale, baseY + 54.0 * scale, 32.0 * scale, 2.5 * scale);
	iSetColor(25, 25, 25);
	drawRect(centerX - 17.5 * scale, baseY + 53.0 * scale, 3.0 * scale, 4.0 * scale);
	drawRect(centerX + 14.5 * scale, baseY + 53.0 * scale, 3.0 * scale, 4.0 * scale);

	iSetColor(255, 235, 140);
	iFilledCircle(centerX, baseY + 55.5 * scale, 2.8 * scale);

	iSetColor(225, 175, 140);
	drawRect(centerX - 3.0 * scale, baseY + 68.0 * scale, 6.0 * scale, 4.5 * scale);
	iFilledCircle(centerX, baseY + 77.0 * scale, 6.0 * scale);
	iSetColor(35, 30, 25);
	iFilledCircle(centerX, baseY + 80.0 * scale, 5.0 * scale);

	drawRect(centerX - 3.5 * scale, baseY + 76.0 * scale, 1.8 * scale, 1.6 * scale);
	drawRect(centerX + 1.7 * scale, baseY + 76.0 * scale, 1.8 * scale, 1.6 * scale);

	float pedalL = pedal * 4.5f * (float)scale;
	float pedalR = -pedal * 4.5f * (float)scale;

	iSetColor(45, 95, 155);
	drawRect(centerX - 7.5 * scale, baseY + 28.0 * scale + pedalL * 0.4, 4.5 * scale, 16.0 * scale);
	drawRect(centerX + 3.0 * scale, baseY + 28.0 * scale + pedalR * 0.4, 4.5 * scale, 16.0 * scale);

	iSetColor(25, 25, 25);
	drawRect(centerX - 8.5 * scale, baseY + 24.5 * scale + pedalL * 0.4, 6.0 * scale, 3.5 * scale);
	drawRect(centerX + 2.5 * scale, baseY + 24.5 * scale + pedalR * 0.4, 6.0 * scale, 3.5 * scale);

	iSetColor(22, 22, 25);
	drawRect(centerX - 2.2 * scale, baseY, 4.4 * scale, 24.0 * scale);
	iSetColor(180, 185, 195);
	drawRect(centerX - 2.8 * scale, baseY + 12.0 * scale, 5.6 * scale, 13.0 * scale);
}

// Procedural realistic walking pedestrian
void drawRealisticPerson(double centerX, double baseY, double scale, float walkPhase, int outfitTheme) {
	if (scale <= 0.05) return;

	double hipY = baseY + 45.0 * scale;

	float swing = (float)sin(walkPhase);
	float legSwingL = swing * 6.0f * (float)scale;
	float legSwingR = -swing * 6.0f * (float)scale;
	float armSwingL = -swing * 5.5f * (float)scale;
	float armSwingR = swing * 5.5f * (float)scale;

	iSetColor(30, 32, 35);
	double shX[] = { centerX - 10.0 * scale, centerX + 10.0 * scale, centerX + 8.0 * scale, centerX - 8.0 * scale };
	double shY[] = { baseY - 2.0 * scale, baseY - 2.0 * scale, baseY + 4.0 * scale, baseY + 4.0 * scale };
	iFilledPolygon(shX, shY, 4);

	int shirtR = 35, shirtG = 95, shirtB = 175;
	int pantsR = 180, pantsG = 160, pantsB = 120;
	int shoeR = 30, shoeG = 30, shoeB = 30;

	if (outfitTheme == 1) {
		shirtR = 145; shirtG = 30;  shirtB = 45;
		pantsR = 40;  pantsG = 50;  pantsB = 75;
		shoeR = 80;   shoeG = 50;   shoeB = 35;
	}
	else if (outfitTheme == 2) {
		shirtR = 240; shirtG = 242; shirtB = 245;
		pantsR = 55;  pantsG = 60;  pantsB = 68;
		shoeR = 20;   shoeG = 20;   shoeB = 20;
	}
	else if (outfitTheme == 3) {
		shirtR = 40;  shirtG = 125; shirtB = 65;
		pantsR = 50;  pantsG = 75;  pantsB = 120;
		shoeR = 230;  shoeG = 230;  shoeB = 230;
	}

	iSetColor(pantsR, pantsG, pantsB);
	double tLeftX[] = { centerX - 9.0 * scale, centerX - 2.0 * scale, centerX - 3.0 * scale + legSwingL * 0.4, centerX - 9.0 * scale + legSwingL * 0.4 };
	double tLeftY[] = { hipY, hipY, baseY + 5.0 * scale, baseY + 5.0 * scale };
	iFilledPolygon(tLeftX, tLeftY, 4);

	iSetColor(shoeR, shoeG, shoeB);
	drawRect(centerX - 9.5 * scale + legSwingL * 0.4, baseY, 7.5 * scale, 5.0 * scale);
	iSetColor(220, 220, 220);
	drawRect(centerX - 9.5 * scale + legSwingL * 0.4, baseY, 7.5 * scale, 1.2 * scale);

	iSetColor(pantsR, pantsG, pantsB);
	double tRightX[] = { centerX + 2.0 * scale, centerX + 9.0 * scale, centerX + 9.0 * scale + legSwingR * 0.4, centerX + 3.0 * scale + legSwingR * 0.4 };
	double tRightY[] = { hipY, hipY, baseY + 5.0 * scale, baseY + 5.0 * scale };
	iFilledPolygon(tRightX, tRightY, 4);

	iSetColor(shoeR, shoeG, shoeB);
	drawRect(centerX + 2.5 * scale + legSwingR * 0.4, baseY, 7.5 * scale, 5.0 * scale);
	iSetColor(220, 220, 220);
	drawRect(centerX + 2.5 * scale + legSwingR * 0.4, baseY, 7.5 * scale, 1.2 * scale);

	iSetColor(35, 30, 25);
	drawRect(centerX - 8.0 * scale, hipY - 2.0 * scale, 16.0 * scale, 3.0 * scale);
	iSetColor(210, 215, 220);
	drawRect(centerX - 1.5 * scale, hipY - 2.0 * scale, 3.0 * scale, 3.0 * scale);

	iSetColor(shirtR, shirtG, shirtB);
	double torsoX[] = { centerX - 8.0 * scale, centerX + 8.0 * scale, centerX + 11.5 * scale, centerX - 11.5 * scale };
	double torsoY[] = { hipY + 1.0 * scale, hipY + 1.0 * scale, hipY + 31.0 * scale, hipY + 31.0 * scale };
	iFilledPolygon(torsoX, torsoY, 4);

	iSetColor((int)(shirtR * 0.85), (int)(shirtG * 0.85), (int)(shirtB * 0.85));
	double collarX[] = { centerX - 4.0 * scale, centerX + 4.0 * scale, centerX };
	double collarY[] = { hipY + 31.0 * scale, hipY + 31.0 * scale, hipY + 22.0 * scale };
	iFilledPolygon(collarX, collarY, 3);

	iSetColor(shirtR, shirtG, shirtB);
	drawRect(centerX - 14.0 * scale, hipY + 19.0 * scale, 3.5 * scale, 12.0 * scale);
	iSetColor(225, 175, 140);
	drawRect(centerX - 14.0 * scale + armSwingL * 0.3, hipY + 7.0 * scale, 3.2 * scale, 12.0 * scale);
	drawRect(centerX - 14.5 * scale + armSwingL * 0.3, hipY + 2.0 * scale, 3.8 * scale, 5.0 * scale);

	iSetColor(shirtR, shirtG, shirtB);
	drawRect(centerX + 10.5 * scale, hipY + 19.0 * scale, 3.5 * scale, 12.0 * scale);
	iSetColor(225, 175, 140);
	drawRect(centerX + 10.8 * scale + armSwingR * 0.3, hipY + 7.0 * scale, 3.2 * scale, 12.0 * scale);
	drawRect(centerX + 10.8 * scale + armSwingR * 0.3, hipY + 2.0 * scale, 3.8 * scale, 5.0 * scale);

	iSetColor(225, 175, 140);
	drawRect(centerX - 3.0 * scale, hipY + 30.0 * scale, 6.0 * scale, 5.0 * scale);
	iFilledCircle(centerX, hipY + 39.0 * scale, 6.2 * scale);
	drawRect(centerX - 4.0 * scale, hipY + 34.0 * scale, 8.0 * scale, 5.0 * scale);

	iSetColor(30, 25, 25);
	double hairX[] = { centerX - 6.5 * scale, centerX + 6.5 * scale, centerX + 5.5 * scale, centerX - 5.5 * scale };
	double hairY[] = { hipY + 40.0 * scale, hipY + 40.0 * scale, hipY + 47.0 * scale, hipY + 47.0 * scale };
	iFilledPolygon(hairX, hairY, 4);
	iFilledCircle(centerX, hipY + 43.0 * scale, 5.2 * scale);

	iSetColor(35, 30, 30);
	drawRect(centerX - 3.8 * scale, hipY + 38.5 * scale, 2.6 * scale, 1.8 * scale);
	drawRect(centerX + 1.2 * scale, hipY + 38.5 * scale, 2.6 * scale, 1.8 * scale);
}

// Procedural realistic street dog
void drawRealisticDog(double centerX, double baseY, double scale, float trotAnim, int coatColor) {
	if (scale <= 0.05) return;

	double baseW = 24.0;
	double w = baseW * scale;
	float trot = (float)sin(trotAnim);

	iSetColor(32, 34, 38);
	double shX[] = { centerX - w * 0.65, centerX + w * 0.65, centerX + w * 0.50, centerX - w * 0.50 };
	double shY[] = { baseY - 2.0 * scale, baseY - 2.0 * scale, baseY + 3.5 * scale, baseY + 3.5 * scale };
	iFilledPolygon(shX, shY, 4);

	int coatR = 210, coatG = 145, coatB = 70;
	int darkR = 150, darkG = 95, darkB = 40;
	int chestR = 245, chestG = 220, chestB = 175;
	int muzzleR = 55, muzzleG = 40, muzzleB = 30;

	if (coatColor == 1) {
		coatR = 195; coatG = 130; coatB = 65;
		darkR = 30;  darkG = 28;  darkB = 26;
		chestR = 215; chestG = 165; chestB = 110;
		muzzleR = 25; muzzleG = 22; muzzleB = 20;
	}
	else if (coatColor == 2) {
		coatR = 140; coatG = 80;  coatB = 45;
		darkR = 95;  darkG = 48;  darkB = 25;
		chestR = 195; chestG = 145; chestB = 110;
		muzzleR = 65; muzzleG = 38; muzzleB = 25;
	}
	else if (coatColor == 3) {
		coatR = 242; coatG = 242; coatB = 245;
		darkR = 185; darkG = 110; darkB = 55;
		chestR = 255; chestG = 255; chestB = 255;
		muzzleR = 238; muzzleG = 235; muzzleB = 230;
	}

	iSetColor(coatR, coatG, coatB);
	double tailSway = trot * 5.0 * scale;
	double tailX[] = { centerX - 2.0 * scale, centerX + tailSway, centerX + tailSway + 3.5 * scale, centerX + 2.0 * scale };
	double tailY[] = { baseY + 15.0 * scale, baseY + 28.0 * scale, baseY + 29.5 * scale, baseY + 16.5 * scale };
	iFilledPolygon(tailX, tailY, 4);

	iSetColor((int)(coatR * 0.82), (int)(coatG * 0.82), (int)(coatB * 0.82));
	double lhX[] = { centerX - w * 0.46, centerX - w * 0.28, centerX - w * 0.32, centerX - w * 0.44 };
	double lhY[] = { baseY + 14.0 * scale, baseY + 14.0 * scale, baseY + 3.0 * scale, baseY + 3.0 * scale };
	iFilledPolygon(lhX, lhY, 4);

	double rhX[] = { centerX + w * 0.28, centerX + w * 0.46, centerX + w * 0.44, centerX + w * 0.32 };
	double rhY[] = { baseY + 14.0 * scale, baseY + 14.0 * scale, baseY + 3.0 * scale, baseY + 3.0 * scale };
	iFilledPolygon(rhX, rhY, 4);

	iSetColor(coatR, coatG, coatB);
	double chestX[] = { centerX - w * 0.32, centerX + w * 0.32, centerX + w * 0.24, centerX - w * 0.24 };
	double chestY[] = { baseY + 8.0 * scale, baseY + 8.0 * scale, baseY + 19.0 * scale, baseY + 19.0 * scale };
	iFilledPolygon(chestX, chestY, 4);

	float lPawLift = (trot > 0) ? trot * 3.0f * (float)scale : 0.0f;
	float rPawLift = (-trot > 0) ? -trot * 3.0f * (float)scale : 0.0f;

	iSetColor(coatR, coatG, coatB);
	double lLegX[] = { centerX - w * 0.26, centerX - w * 0.14, centerX - w * 0.16, centerX - w * 0.24 };
	double lLegY[] = { baseY + 14.0 * scale, baseY + 14.0 * scale, baseY + lPawLift + 2.5 * scale, baseY + lPawLift + 2.5 * scale };
	iFilledPolygon(lLegX, lLegY, 4);
	drawRect(centerX - w * 0.26, baseY + lPawLift, 4.0 * scale, 3.0 * scale);

	double rLegX[] = { centerX + w * 0.14, centerX + w * 0.26, centerX + w * 0.24, centerX + w * 0.16 };
	double rLegY[] = { baseY + 14.0 * scale, baseY + 14.0 * scale, baseY + rPawLift + 2.5 * scale, baseY + rPawLift + 2.5 * scale };
	iFilledPolygon(rLegX, rLegY, 4);
	drawRect(centerX + w * 0.16, baseY + rPawLift, 4.0 * scale, 3.0 * scale);

	iSetColor(220, 35, 35);
	drawRect(centerX - w * 0.22, baseY + 17.5 * scale, w * 0.44, 3.0 * scale);
	iSetColor(255, 215, 30);
	iFilledCircle(centerX, baseY + 17.0 * scale, 1.8 * scale);

	if (coatColor == 1) iSetColor(darkR, darkG, darkB);
	else iSetColor(coatR, coatG, coatB);
	iFilledCircle(centerX, baseY + 24.5 * scale, 7.5 * scale);

	double lEarX[] = { centerX - w * 0.38, centerX - w * 0.18, centerX - w * 0.32 };
	double lEarY[] = { baseY + 24.0 * scale, baseY + 27.0 * scale, baseY + 34.5 * scale };
	iFilledPolygon(lEarX, lEarY, 3);

	double rEarX[] = { centerX + w * 0.18, centerX + w * 0.38, centerX + w * 0.32 };
	double rEarY[] = { baseY + 27.0 * scale, baseY + 24.0 * scale, baseY + 34.5 * scale };
	iFilledPolygon(rEarX, rEarY, 3);

	iSetColor(20, 16, 14);
	drawRect(centerX - 5.5 * scale, baseY + 24.2 * scale, 2.8 * scale, 2.5 * scale);
	drawRect(centerX + 2.7 * scale, baseY + 24.2 * scale, 2.8 * scale, 2.5 * scale);

	iSetColor(muzzleR, muzzleG, muzzleB);
	double mBridgeX[] = { centerX - 3.2 * scale, centerX + 3.2 * scale, centerX + 4.2 * scale, centerX - 4.2 * scale };
	double mBridgeY[] = { baseY + 25.0 * scale, baseY + 25.0 * scale, baseY + 18.5 * scale, baseY + 18.5 * scale };
	iFilledPolygon(mBridgeX, mBridgeY, 4);

	iSetColor(18, 16, 15);
	iFilledCircle(centerX, baseY + 22.4 * scale, 1.7 * scale);

	iSetColor(245, 95, 115);
	double tongueX[] = { centerX - 1.8 * scale, centerX + 1.8 * scale, centerX + 1.4 * scale, centerX - 1.4 * scale };
	double tongueY[] = { baseY + 19.0 * scale, baseY + 19.0 * scale, baseY + 15.0 * scale, baseY + 15.0 * scale };
	iFilledPolygon(tongueX, tongueY, 4);
}

// Procedural realistic moderate 3D coins (cylindrical thickness & relief)
void drawCoinL2(double centerX, double baseY, double scale, int isSpecial, float spinAnim) {
	if (scale <= 0.05) return;

	if (!isSpecial) {
		// 1. REALISTIC MODERATE 3D GOLD COIN (Value: 1)
		double radius = 14.5 * scale;
		double thick = 3.2 * scale;
		float wf = 0.22f + 0.78f * (float)fabs(cos(spinAnim));
		double rx = radius * wf;
		double ry = radius;
		double cy = baseY + radius * 0.90 + 4.0 * scale;

		iSetColor(22, 24, 28);
		drawFilledEllipse(centerX, baseY - 1.0 * scale, radius * 0.90 * wf, radius * 0.30, 12);

		iSetColor(140, 90, 10);
		drawFilledEllipse(centerX, cy - thick, rx, ry, 16);
		drawRect(centerX - rx, cy - thick, rx * 2.0, thick);
		iSetColor(195, 140, 20);
		drawRect(centerX - rx * 0.65, cy - thick, rx * 1.3, thick);

		iSetColor(215, 160, 20);
		drawFilledEllipse(centerX, cy, rx, ry, 16);

		iSetColor(255, 235, 130);
		drawFilledEllipse(centerX, cy + 1.0 * scale, rx * 0.95, ry * 0.95, 16);

		iSetColor(165, 110, 12);
		drawFilledEllipse(centerX, cy, rx * 0.88, ry * 0.88, 16);

		iSetColor(245, 195, 30);
		drawFilledEllipse(centerX, cy, rx * 0.80, ry * 0.80, 16);

		iSetColor(255, 222, 68);
		drawFilledEllipse(centerX, cy, rx * 0.60, ry * 0.60, 16);

		double starW = 6.0 * scale * wf;
		double starH = 6.0 * scale;
		iSetColor(150, 95, 10);
		drawRect(centerX - starW * 0.5 + 0.8 * scale, cy - starH * 0.5 - 0.8 * scale, starW, starH);
		iSetColor(255, 255, 220);
		drawRect(centerX - starW * 0.5, cy - starH * 0.5, starW, starH);
		iSetColor(255, 255, 255);
		iFilledCircle(centerX, cy, 1.5 * scale);

		iSetColor(255, 255, 255);
		drawFilledEllipse(centerX - rx * 0.35, cy + ry * 0.35, 2.5 * scale * wf, 2.5 * scale, 10);
	}
	else {
		// 2. POWERFUL SPECIAL COIN (Value: 5)
		double radius = 21.0 * scale;
		double thick = 4.5 * scale;
		float wf = 0.25f + 0.75f * (float)fabs(cos(spinAnim));
		double rx = radius * wf;
		double ry = radius;
		double cy = baseY + radius * 0.95 + 5.0 * scale;

		float pulse = (float)fabs(sin(spinAnim * 1.8f));
		iSetColor(245, 50, 165);
		drawFilledEllipse(centerX, cy, (radius + 4.0 + pulse * 3.0) * wf * scale, (radius + 4.0 + pulse * 3.0) * scale, 16);

		iSetColor(20, 22, 26);
		drawFilledEllipse(centerX, baseY - 1.2 * scale, radius * 0.95 * wf, radius * 0.32, 12);

		iSetColor(90, 15, 80);
		drawFilledEllipse(centerX, cy - thick, rx, ry, 16);
		drawRect(centerX - rx, cy - thick, rx * 2.0, thick);
		iSetColor(150, 25, 130);
		drawRect(centerX - rx * 0.65, cy - thick, rx * 1.3, thick);

		iSetColor(125, 18, 110);
		drawFilledEllipse(centerX, cy, rx, ry, 16);

		iSetColor(255, 205, 25);
		drawFilledEllipse(centerX, cy, rx * 0.92, ry * 0.92, 16);

		iSetColor(180, 130, 15);
		drawFilledEllipse(centerX, cy, rx * 0.82, ry * 0.82, 16);

		iSetColor(215, 25, 75);
		drawFilledEllipse(centerX, cy, rx * 0.74, ry * 0.74, 16);

		iSetColor(30, 220, 255);
		drawFilledEllipse(centerX, cy, rx * 0.45, ry * 0.45, 16);

		double starSize = 7.5 * scale;
		double starW = starSize * wf;
		iSetColor(255, 255, 255);
		double sX[] = { centerX, centerX + starW * 0.5, centerX, centerX - starW * 0.5 };
		double sY[] = { cy + starSize * 0.5, cy, cy - starSize * 0.5, cy };
		iFilledPolygon(sX, sY, 4);

		iSetColor(255, 255, 255);
		iFilledCircle(centerX, cy, 2.0 * scale);
	}
}

// Volumetric cumulus cloud
void drawRealisticCloud(double cx, double cy, double scale) {
	if (scale <= 0.1) return;

	iSetColor(180, 210, 235);
	iFilledCircle(cx - 52.0 * scale, cy - 2.0 * scale, 24.0 * scale);
	iFilledCircle(cx - 22.0 * scale, cy - 4.0 * scale, 30.0 * scale);
	iFilledCircle(cx + 18.0 * scale, cy - 3.0 * scale, 28.0 * scale);
	iFilledCircle(cx + 52.0 * scale, cy - 2.0 * scale, 23.0 * scale);
	drawRect(cx - 52.0 * scale, cy - 14.0 * scale, 104.0 * scale, 16.0 * scale);

	iSetColor(255, 255, 255);
	iFilledCircle(cx - 50.0 * scale, cy + 3.0 * scale, 23.0 * scale);
	iFilledCircle(cx - 22.0 * scale, cy + 5.0 * scale, 29.0 * scale);
	iFilledCircle(cx + 18.0 * scale, cy + 6.0 * scale, 27.0 * scale);
	iFilledCircle(cx + 50.0 * scale, cy + 4.0 * scale, 22.0 * scale);
	drawRect(cx - 50.0 * scale, cy - 8.0 * scale, 100.0 * scale, 16.0 * scale);

	iFilledCircle(cx - 30.0 * scale, cy + 20.0 * scale, 23.0 * scale);
	iFilledCircle(cx + 2.0 * scale, cy + 26.0 * scale, 28.0 * scale);
	iFilledCircle(cx + 32.0 * scale, cy + 18.0 * scale, 22.0 * scale);
}

// Lush roadside tree
void drawRealisticTree(double baseX, double baseY, double scale, int variant) {
	if (scale <= 0.05) return;

	double trunkW = 14.0 * scale;
	double trunkH = 55.0 * scale;

	iSetColor(30, 32, 35);
	double shX[] = { baseX - 22.0 * scale, baseX + 22.0 * scale, baseX + 16.0 * scale, baseX - 16.0 * scale };
	double shY[] = { baseY - 2.0 * scale, baseY - 2.0 * scale, baseY + 4.0 * scale, baseY + 4.0 * scale };
	iFilledPolygon(shX, shY, 4);

	iSetColor(75, 48, 28);
	double trX[] = { baseX - trunkW * 0.9, baseX + trunkW * 0.9, baseX + trunkW * 0.45, baseX - trunkW * 0.45 };
	double trY[] = { baseY, baseY, baseY + trunkH, baseY + trunkH };
	iFilledPolygon(trX, trY, 4);

	int dR = 20, dG = 75, dB = 30;
	int mR = 35, mG = 135, mB = 48;
	int lR = 75, lG = 185, lB = 60;
	if (variant == 1) {
		dR = 35; dG = 80; dB = 25;
		mR = 65; mG = 145; mB = 35;
		lR = 125; lG = 195; lB = 55;
	}

	double canCenterY = baseY + trunkH + 28.0 * scale;

	iSetColor(dR, dG, dB);
	iFilledCircle(baseX - 24.0 * scale, canCenterY - 10.0 * scale, 25.0 * scale);
	iFilledCircle(baseX + 24.0 * scale, canCenterY - 10.0 * scale, 25.0 * scale);
	iFilledCircle(baseX, canCenterY - 5.0 * scale, 30.0 * scale);

	iSetColor(mR, mG, mB);
	iFilledCircle(baseX - 26.0 * scale, canCenterY + 5.0 * scale, 24.0 * scale);
	iFilledCircle(baseX + 26.0 * scale, canCenterY + 5.0 * scale, 24.0 * scale);
	iFilledCircle(baseX, canCenterY + 15.0 * scale, 28.0 * scale);

	iSetColor(lR, lG, lB);
	iFilledCircle(baseX - 14.0 * scale, canCenterY + 24.0 * scale, 16.0 * scale);
	iFilledCircle(baseX + 10.0 * scale, canCenterY + 26.0 * scale, 17.0 * scale);
}

// Coconut Palm Tree
void drawRealisticPalmTree(double baseX, double baseY, double scale, int variant) {
	if (scale <= 0.05) return;

	double trunkH = 95.0 * scale;
	iSetColor(30, 32, 35);
	iFilledCircle(baseX, baseY, 12.0 * scale);

	double curveLean = (variant % 2 == 0) ? -12.0 * scale : 12.0 * scale;
	double topX = baseX + curveLean;
	double topY = baseY + trunkH;

	int segments = 8;
	for (int s = 0; s < segments; s++) {
		double f1 = (double)s / segments;
		double f2 = (double)(s + 1) / segments;
		double sx1 = baseX + curveLean * (f1 * f1);
		double sy1 = baseY + trunkH * f1;
		double sx2 = baseX + curveLean * (f2 * f2);
		double sy2 = baseY + trunkH * f2;
		double w = (8.0 - 3.5 * f1) * scale;

		if (s % 2 == 0) iSetColor(120, 95, 65);
		else iSetColor(90, 70, 48);

		double segX[] = { sx1 - w * 0.5, sx1 + w * 0.5, sx2 + w * 0.4, sx2 - w * 0.4 };
		double segY[] = { sy1, sy1, sy2, sy2 };
		iFilledPolygon(segX, segY, 4);
	}

	iSetColor(115, 85, 35);
	iFilledCircle(topX - 4.0 * scale, topY - 3.0 * scale, 3.5 * scale);
	iFilledCircle(topX + 4.0 * scale, topY - 3.0 * scale, 3.5 * scale);

	double frondAngles[] = { -2.8, -2.2, -1.6, -1.0, -0.4, 0.4, 1.0, 1.6, 2.2, 2.8 };
	double frondLen = 42.0 * scale;

	for (int f = 0; f < 10; f++) {
		double ang = frondAngles[f];
		double endX = topX + cos(ang) * frondLen;
		double endY = topY + sin(ang) * frondLen * 0.65 - 8.0 * scale;
		double midX = (topX + endX) * 0.5;
		double midY = (topY + endY) * 0.5 + 8.0 * scale;

		iSetColor(28, 105, 38);
		double fPolyX[] = { topX, midX - 4.0 * scale, endX, midX + 4.0 * scale };
		double fPolyY[] = { topY, midY, endY, midY };
		iFilledPolygon(fPolyX, fPolyY, 4);
		iSetColor(75, 175, 55);
		iLine(topX, topY, endX, endY);
	}
}

// Concrete electrical utility pole
void drawElectricalPole(double baseX, double baseY, double scale, int side) {
	if (scale <= 0.05) return;

	double poleH = 135.0 * scale;
	double baseW = 7.5 * scale;
	double topW = 4.5 * scale;

	iSetColor(30, 32, 35);
	iFilledCircle(baseX, baseY, 6.0 * scale);

	iSetColor(125, 130, 135);
	double pX[] = { baseX - baseW * 0.5, baseX + baseW * 0.5, baseX + topW * 0.5, baseX - topW * 0.5 };
	double pY[] = { baseY, baseY, baseY + poleH, baseY + poleH };
	iFilledPolygon(pX, pY, 4);

	double armY1 = baseY + poleH - 12.0 * scale;
	double armW1 = 38.0 * scale;
	iSetColor(70, 72, 76);
	drawRect(baseX - armW1 * 0.5, armY1, armW1, 3.5 * scale);

	iSetColor(240, 242, 245);
	drawRect(baseX - armW1 * 0.45, armY1 + 3.5 * scale, 3.5 * scale, 5.5 * scale);
	drawRect(baseX - 1.7 * scale, armY1 + 3.5 * scale, 3.5 * scale, 5.5 * scale);
	drawRect(baseX + armW1 * 0.45 - 3.5 * scale, armY1 + 3.5 * scale, 3.5 * scale, 5.5 * scale);
}

// Modern Street Lamp
void drawStreetLamp(double baseX, double baseY, double scale, int side) {
	if (scale <= 0.05) return;

	double lampH = 115.0 * scale;
	double dir = (side == 0) ? 1.0 : -1.0;

	iSetColor(30, 32, 35);
	iFilledCircle(baseX, baseY, 5.0 * scale);

	iSetColor(110, 115, 122);
	drawRect(baseX - 1.8 * scale, baseY, 3.6 * scale, lampH - 24.0 * scale);

	double topY = baseY + lampH - 18.0 * scale;
	double p1X = baseX;
	double p2X = baseX + dir * 18.0 * scale;

	iLine(p1X, topY, p2X, topY + 10.0 * scale);

	iSetColor(255, 245, 180);
	iFilledCircle(p2X, topY + 6.0 * scale, 5.0 * scale);
	iSetColor(255, 255, 255);
	iFilledCircle(p2X, topY + 6.0 * scale, 2.5 * scale);
}

// Dhaka City Corporation Swing Barrel Dustbin
void drawDhakaWasteBin(double baseX, double baseY, double scale, int side) {
	if (scale <= 0.05) return;

	double binW = 34.0 * scale;
	double binH = 46.0 * scale;

	iSetColor(25, 27, 30);
	drawRect(baseX - binW * 0.65, baseY - 2.0 * scale, binW * 1.3, 4.5 * scale);

	iSetColor(35, 38, 42);
	drawRect(baseX - binW * 0.58, baseY, 4.0 * scale, binH + 10.0 * scale);
	drawRect(baseX + binW * 0.58 - 4.0 * scale, baseY, 4.0 * scale, binH + 10.0 * scale);
	drawRect(baseX - binW * 0.58, baseY + binH + 7.0 * scale, binW * 1.16, 3.5 * scale);
	drawRect(baseX - binW * 0.58, baseY + 3.0 * scale, binW * 1.16, 3.0 * scale);

	iSetColor(180, 185, 195);
	iFilledCircle(baseX - binW * 0.56, baseY + binH * 0.55, 3.0 * scale);
	iFilledCircle(baseX + binW * 0.56, baseY + binH * 0.55, 3.0 * scale);

	iSetColor(20, 145, 60);
	double barX = baseX - binW * 0.44;
	double barY = baseY + 8.0 * scale;
	double barW = binW * 0.88;
	drawRect(barX, barY, barW, binH);

	iSetColor(12, 70, 30);
	drawRect(barX, barY + binH - 5.0 * scale, barW, 5.0 * scale);
	iSetColor(15, 95, 40);
	drawRect(barX - 1.5 * scale, barY + binH - 2.0 * scale, barW + 3.0 * scale, 3.5 * scale);

	iSetColor(255, 255, 255);
	drawRect(barX + 4.0 * scale, barY + binH * 0.45, barW - 8.0 * scale, 4.0 * scale);
	drawRect(baseX - 4.0 * scale, barY + binH * 0.55, 8.0 * scale, 8.0 * scale);
	iSetColor(20, 145, 60);
	drawRect(baseX - 2.0 * scale, barY + binH * 0.55 + 2.0 * scale, 4.0 * scale, 4.0 * scale);
}

// Utility box
void drawElectricalBox(double baseX, double baseY, double scale, int side) {
	if (scale <= 0.05) return;

	double boxW = 20.0 * scale;
	double boxH = 28.0 * scale;

	iSetColor(30, 32, 35);
	drawRect(baseX - boxW * 0.55, baseY - 2.0 * scale, boxW * 1.1, 4.0 * scale);

	iSetColor(140, 145, 150);
	drawRect(baseX - boxW * 0.55, baseY, boxW * 1.1, 5.0 * scale);

	iSetColor(42, 85, 58);
	drawRect(baseX - boxW * 0.5, baseY + 5.0 * scale, boxW, boxH);

	iSetColor(245, 205, 20);
	double triX[] = { baseX + boxW * 0.2, baseX + boxW * 0.36, baseX + boxW * 0.04 };
	double triY[] = { baseY + 18.0 * scale, baseY + 11.0 * scale, baseY + 11.0 * scale };
	iFilledPolygon(triX, triY, 3);
}

// Highway sign
void drawRoadSign(double baseX, double baseY, double scale, int side, int variant) {
	if (scale <= 0.05) return;

	double signH = 45.0 * scale;
	iSetColor(140, 145, 150);
	drawRect(baseX - 7.0 * scale, baseY, 2.0 * scale, signH);
	drawRect(baseX + 5.0 * scale, baseY, 2.0 * scale, signH);

	double boardW = 32.0 * scale;
	double boardH = 18.0 * scale;
	double bX = baseX - boardW * 0.5;
	double bY = baseY + signH - 8.0 * scale;

	iSetColor(20, 110, 52);
	drawRect(bX, bY, boardW, boardH);
	iSetColor(255, 255, 255);
	drawRect(bX + 1.2 * scale, bY + 1.2 * scale, boardW - 2.4 * scale, 0.8 * scale);
	drawRect(bX + 4.0 * scale, bY + 8.0 * scale, boardW - 8.0 * scale, 3.0 * scale);
}

// Flower Planter
void drawFlowerPlanter(double baseX, double baseY, double scale, int side) {
	if (scale <= 0.05) return;

	double boxW = 22.0 * scale;
	double boxH = 10.0 * scale;

	iSetColor(175, 88, 55);
	drawRect(baseX - boxW * 0.5, baseY, boxW, boxH);

	iSetColor(30, 115, 45);
	iFilledCircle(baseX - 6.0 * scale, baseY + boxH + 6.0 * scale, 8.5 * scale);
	iFilledCircle(baseX + 6.0 * scale, baseY + boxH + 6.0 * scale, 8.5 * scale);

	iSetColor(240, 45, 45);
	iFilledCircle(baseX - 4.5 * scale, baseY + boxH + 7.5 * scale, 2.5 * scale);
	iSetColor(255, 215, 30);
	iFilledCircle(baseX + 3.5 * scale, baseY + boxH + 6.0 * scale, 2.2 * scale);
}

void drawRoadsidePropL2(int index) {
	if (!roadsideProps[index].active) return;

	double y = roadsideProps[index].y;
	double scale = getPerspectiveScaleL2(y);
	double cx = getRoadsideXL2(roadsideProps[index].side, roadsideProps[index].lateralOffset, y);

	int type = roadsideProps[index].type;
	int side = roadsideProps[index].side;
	int variant = roadsideProps[index].variant;

	if (type == 0) drawRealisticTree(cx, y, scale, variant);
	else if (type == 1) drawRealisticPalmTree(cx, y, scale, variant);
	else if (type == 2) drawElectricalPole(cx, y, scale, side);
	else if (type == 3) drawStreetLamp(cx, y, scale, side);
	else if (type == 4) drawDhakaWasteBin(cx, y, scale, side);
	else if (type == 5) drawElectricalBox(cx, y, scale, side);
	else if (type == 6) drawRoadSign(cx, y, scale, side, variant);
	else if (type == 7) drawFlowerPlanter(cx, y, scale, side);
}

void drawOverheadPowerLinesL2() {
	int poleIndices[MAX_ROADSIDE_PROPS];
	int poleCount = 0;
	for (int i = 0; i < MAX_ROADSIDE_PROPS; i++) {
		if (roadsideProps[i].active && roadsideProps[i].side == 0 && roadsideProps[i].type == 2) {
			poleIndices[poleCount++] = i;
		}
	}

	for (int i = 0; i < poleCount - 1; i++) {
		for (int j = i + 1; j < poleCount; j++) {
			if (roadsideProps[poleIndices[i]].y < roadsideProps[poleIndices[j]].y) {
				int tmp = poleIndices[i];
				poleIndices[i] = poleIndices[j];
				poleIndices[j] = tmp;
			}
		}
	}

	if (poleCount >= 1) {
		double hX = 388.0;
		double hY = (double)horizonYL2 + 22.0;

		double firstY = roadsideProps[poleIndices[0]].y;
		double firstScale = getPerspectiveScaleL2(firstY);
		double firstPoleX = getRoadsideXL2(0, roadsideProps[poleIndices[0]].lateralOffset, firstY);
		double firstWireY1 = firstY + (135.0 - 8.5) * firstScale;

		iSetColor(50, 52, 56);
		iLine(hX, hY, firstPoleX - 16.0 * firstScale, firstWireY1);
	}
}

// Realistic Windows
void drawFacadeWindows(double x, double y, double w, double h, int cols, int rows, double marginX, double marginY, int lightSeed) {
	double totalGapX = marginX * 2.0 + (cols - 1) * (marginX * 0.6);
	double totalGapY = marginY * 2.0 + (rows - 1) * (marginY * 0.7);
	double winW = (w - totalGapX) / cols;
	double winH = (h - totalGapY) / rows;
	if (winW <= 1.0 || winH <= 1.0) return;

	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			double wx = x + marginX + c * (winW + marginX * 0.6);
			double wy = y + marginY + r * (winH + marginY * 0.7);

			int hash = (c * 7 + r * 13 + lightSeed) % 11;
			if (hash == 0 || hash == 4) iSetColor(255, 235, 150);
			else if (hash == 1 || hash == 6 || hash == 9) iSetColor(145, 205, 245);
			else iSetColor(40, 58, 76);
			drawRect(wx, wy, winW, winH);
		}
	}
}

// Rooftop water tank
void drawRooftopWaterTank(double x, double y, double scale) {
	iSetColor(50, 55, 60);
	drawRect(x + 2.0 * scale, y, 2.0 * scale, 11.0 * scale);
	drawRect(x + 14.0 * scale, y, 2.0 * scale, 11.0 * scale);
	iLine(x + 2.0 * scale, y + 2.0 * scale, x + 16.0 * scale, y + 9.0 * scale);
	iLine(x + 16.0 * scale, y + 2.0 * scale, x + 2.0 * scale, y + 9.0 * scale);

	iSetColor(30, 50, 75);
	drawRect(x, y + 10.0 * scale, 18.0 * scale, 14.0 * scale);
	iSetColor(20, 36, 55);
	drawRect(x, y + 13.0 * scale, 18.0 * scale, 1.5 * scale);
	drawRect(x, y + 18.0 * scale, 18.0 * scale, 1.5 * scale);
	iSetColor(45, 70, 100);
	iFilledCircle(x + 9.0 * scale, y + 23.5 * scale, 7.0 * scale);
}

// Rooftop antenna
void drawRooftopAntenna(double x, double y, double height) {
	iSetColor(185, 190, 195);
	drawRect(x - 1.2, y, 2.4, height);

	iLine(x - 7.0, y + height * 0.45, x + 7.0, y + height * 0.45);
	iLine(x - 5.0, y + height * 0.72, x + 5.0, y + height * 0.72);
	iLine(x - 7.0, y + height * 0.45, x, y + height * 0.6);
	iLine(x + 7.0, y + height * 0.45, x, y + height * 0.6);

	iSetColor(255, 35, 35);
	iFilledCircle(x, y + height + 2.5, 2.5);
	iSetColor(255, 200, 200);
	iFilledCircle(x, y + height + 2.5, 1.0);
}

// Procedural realistic city buildings flanking the left and right sides of the avenue
void drawRealisticBuildingsL2() {
	iSetColor(110, 148, 185);
	drawRect(30, horizonYL2, 55, 165);
	drawRect(110, horizonYL2, 50, 185);
	drawRect(185, horizonYL2, 65, 150);
	drawRect(275, horizonYL2, 55, 140);

	drawRect(615, horizonYL2, 60, 180);
	drawRect(700, horizonYL2, 50, 195);
	drawRect(775, horizonYL2, 65, 160);
	drawRect(850, horizonYL2, 45, 145);

	drawFacadeWindows(35, horizonYL2, 45, 160, 3, 10, 3.0, 5.0, 1);
	drawFacadeWindows(115, horizonYL2, 40, 180, 2, 12, 3.0, 5.0, 2);
	drawFacadeWindows(190, horizonYL2, 55, 145, 3, 9, 3.0, 5.0, 3);
	drawFacadeWindows(620, horizonYL2, 50, 175, 3, 11, 3.0, 5.0, 4);
	drawFacadeWindows(705, horizonYL2, 40, 190, 2, 12, 3.0, 5.0, 5);
	drawFacadeWindows(780, horizonYL2, 55, 155, 3, 10, 3.0, 5.0, 6);

	// LEFT BUILDINGS
	{
		double bx = 0, by = horizonYL2, bw = 75, bh = 140;
		iSetColor(42, 60, 82);
		drawRect(bx, by, bw, bh);
		iSetColor(30, 44, 62);
		drawRect(bx + bw - 5, by, 5, bh);
		drawFacadeWindows(bx, by, bw - 5, bh - 10, 4, 9, 3.5, 4.0, 12);
		iSetColor(55, 75, 100);
		drawRect(bx + 15, by + bh, 35, 14);
		drawRooftopAntenna(bx + 32, by + bh + 14, 25);
	}
	{
		double bx = 72, by = horizonYL2, bw = 83, bh = 98;
		iSetColor(215, 195, 165);
		drawRect(bx, by, bw, bh);
		iSetColor(185, 165, 135);
		drawRect(bx + bw - 6, by, 6, bh);
		for (int floor = 0; floor < 4; floor++) {
			double fy = by + 8 + floor * 22;
			drawFacadeWindows(bx + 4, fy + 5, 45, 14, 3, 1, 2.0, 1.5, floor * 3);
			iSetColor(190, 170, 140);
			drawRect(bx + 52, fy, 24, 15);
			iSetColor(70, 75, 80);
			drawRect(bx + 52, fy + 8, 24, 7);
			iSetColor(240, 240, 245);
			drawRect(bx + 53, fy + 14, 22, 1.5);
		}
		iSetColor(175, 155, 125);
		drawRect(bx + 2, by + bh, bw - 4, 4);
		drawRooftopWaterTank(bx + 20, by + bh + 4, 1.0);
	}
	{
		double bx = 152, by = horizonYL2, bw = 80, bh = 162;
		iSetColor(35, 52, 75);
		drawRect(bx, by, bw, bh);
		iSetColor(25, 38, 55);
		drawRect(bx + bw - 5, by, 5, bh);
		drawFacadeWindows(bx, by, bw - 5, bh - 18, 5, 11, 2.5, 3.5, 27);
		iSetColor(50, 72, 98);
		drawRect(bx + 10, by + bh, bw - 20, 12);
		iSetColor(220, 45, 45);
		drawRect(bx + 14, by + bh + 2, bw - 28, 8);
		iSetColor(255, 255, 255);
		drawRect(bx + 18, by + bh + 4, bw - 36, 4);
		drawRooftopAntenna(bx + bw * 0.5, by + bh + 12, 22);
	}
	{
		double bx = 228, by = horizonYL2, bw = 77, bh = 112;
		iSetColor(165, 85, 68);
		drawRect(bx, by, bw, bh);
		iSetColor(135, 65, 50);
		drawRect(bx + bw - 5, by, 5, bh);
		drawFacadeWindows(bx, by + 18, bw - 5, bh - 24, 4, 5, 3.0, 3.5, 15);
		iSetColor(35, 120, 65);
		double awX[] = { bx, bx + bw - 5, bx + bw - 2, bx - 3 };
		double awY[] = { by + 16, by + 16, by + 8, by + 8 };
		iFilledPolygon(awX, awY, 4);
		drawRooftopWaterTank(bx + 35, by + bh, 0.9);
	}
	{
		double bx = 302, by = horizonYL2, bw = 70, bh = 88;
		iSetColor(230, 232, 238);
		drawRect(bx, by, bw, bh);
		iSetColor(185, 190, 198);
		drawRect(bx + bw - 6, by, 6, bh);
		for (int floor = 0; floor < 3; floor++) {
			double wy = by + 14 + floor * 24;
			iSetColor(35, 80, 125);
			drawRect(bx + 5, wy, bw - 16, 12);
			iSetColor(120, 190, 235);
			drawRect(bx + 8, wy + 2, bw - 22, 8);
		}
		iSetColor(25, 75, 145);
		drawRect(bx, by + bh - 6, bw - 6, 6);
	}

	// Treeline at open horizon corridor
	iSetColor(75, 135, 95);
	double treeX[] = { 372, 396, 422, 450, 478, 504, 528 };
	for (int i = 0; i < 6; i++) {
		iFilledCircle((treeX[i] + treeX[i + 1]) * 0.5, horizonYL2 + 2, 7.5);
	}

	// RIGHT BUILDINGS
	{
		double bx = 528, by = horizonYL2, bw = 70, bh = 90;
		iSetColor(230, 232, 238);
		drawRect(bx, by, bw, bh);
		iSetColor(190, 195, 202);
		drawRect(bx, by, 6, bh);
		for (int floor = 0; floor < 3; floor++) {
			double wy = by + 14 + floor * 24;
			iSetColor(35, 80, 125);
			drawRect(bx + 10, wy, bw - 16, 12);
			iSetColor(120, 190, 235);
			drawRect(bx + 13, wy + 2, bw - 22, 8);
		}
		iSetColor(215, 35, 45);
		drawRect(bx + 6, by + bh - 6, bw - 6, 6);
	}
	{
		double bx = 594, by = horizonYL2, bw = 78, bh = 158;
		iSetColor(32, 48, 72);
		drawRect(bx, by, bw, bh);
		iSetColor(22, 34, 52);
		drawRect(bx, by, 5, bh);
		drawFacadeWindows(bx + 5, by, bw - 5, bh - 18, 5, 10, 2.5, 3.5, 38);
		iSetColor(45, 65, 92);
		drawRect(bx + 12, by + bh, bw - 24, 14);
		drawRooftopAntenna(bx + bw * 0.5, by + bh + 14, 28);
	}
	{
		double bx = 668, by = horizonYL2, bw = 80, bh = 108;
		iSetColor(210, 188, 155);
		drawRect(bx, by, bw, bh);
		iSetColor(180, 160, 130);
		drawRect(bx, by, 5, bh);
		for (int floor = 0; floor < 4; floor++) {
			double fy = by + 8 + floor * 24;
			iSetColor(185, 165, 135);
			drawRect(bx + 8, fy, 22, 16);
			iSetColor(70, 75, 80);
			drawRect(bx + 8, fy + 9, 22, 7);
			iSetColor(240, 240, 245);
			drawRect(bx + 9, fy + 15, 20, 1.5);
			drawFacadeWindows(bx + 34, fy + 4, 42, 16, 3, 1, 2.0, 1.5, floor * 5);
		}
		drawRooftopWaterTank(bx + 40, by + bh, 1.0);
	}
	{
		double bx = 744, by = horizonYL2, bw = 82, bh = 145;
		iSetColor(40, 68, 92);
		drawRect(bx, by, bw, bh);
		iSetColor(28, 48, 68);
		drawRect(bx, by, 5, bh);
		drawFacadeWindows(bx + 5, by, bw - 5, bh - 14, 5, 9, 2.5, 4.0, 42);
		iSetColor(55, 90, 120);
		drawRect(bx + 14, by + bh, bw - 28, 10);
		drawRect(bx + 26, by + bh + 10, bw - 52, 8);
		drawRooftopAntenna(bx + bw * 0.5, by + bh + 18, 18);
	}
	{
		double bx = 822, by = horizonYL2, bw = 88, bh = 110;
		iSetColor(175, 182, 192);
		drawRect(bx, by, bw, bh);
		iSetColor(145, 150, 160);
		drawRect(bx, by, 5, bh);
		drawFacadeWindows(bx + 5, by + 12, bw - 8, bh - 20, 4, 5, 3.0, 3.5, 19);
		iSetColor(235, 175, 30);
		drawRect(bx + 15, by + bh, bw - 30, 8);
		iSetColor(255, 255, 255);
		drawRect(bx + 18, by + bh + 2, bw - 36, 4);
	}
}

// Authentic Bengali Tea Stall (Tong er dokan)
void drawTongTeaStall(double cx, double cy, double scale, int side, int variant) {
	if (scale <= 0.05) return;

	double baseW = 112.0;
	double baseH = 92.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = cx - w * 0.5;

	iSetColor(28, 30, 34);
	drawRect(bx - 4.0 * scale, cy - 3.0 * scale, w + 8.0 * scale, 6.0 * scale);

	iSetColor(75, 48, 28);
	drawRect(bx + 5.0 * scale, cy, 5.5 * scale, h - 14.0 * scale);
	drawRect(bx + w - 10.5 * scale, cy, 5.5 * scale, h - 14.0 * scale);

	iSetColor(115, 78, 48);
	drawRect(bx + 9.0 * scale, cy + 16.0 * scale, w - 18.0 * scale, h - 34.0 * scale);
	iSetColor(90, 58, 34);
	for (int p = 1; p <= 3; p++) {
		drawRect(bx + 9.0 * scale, cy + 16.0 * scale + p * 9.0 * scale, w - 18.0 * scale, 1.5 * scale);
	}

	iSetColor(135, 88, 55);
	drawRect(bx + 3.0 * scale, cy + 16.0 * scale, w - 6.0 * scale, 22.0 * scale);
	iSetColor(165, 110, 70);
	drawRect(bx - 2.0 * scale, cy + 36.0 * scale, w + 4.0 * scale, 5.0 * scale);

	// Biscuit Jars
	iSetColor(220, 45, 45);
	drawRect(bx + 8.0 * scale, cy + 50.0 * scale, 12.0 * scale, 3.5 * scale);
	iSetColor(190, 225, 245);
	drawRect(bx + 9.0 * scale, cy + 40.0 * scale, 10.0 * scale, 10.0 * scale);
	iSetColor(220, 160, 50);
	iFilledCircle(bx + 14.0 * scale, cy + 44.0 * scale, 3.0 * scale);

	iSetColor(245, 205, 30);
	drawRect(bx + 23.0 * scale, cy + 50.0 * scale, 12.0 * scale, 3.5 * scale);
	iSetColor(190, 225, 245);
	drawRect(bx + 24.0 * scale, cy + 40.0 * scale, 10.0 * scale, 10.0 * scale);
	iSetColor(220, 160, 50);
	iFilledCircle(bx + 29.0 * scale, cy + 44.0 * scale, 3.0 * scale);

	iSetColor(35, 145, 65);
	drawRect(bx + 38.0 * scale, cy + 50.0 * scale, 12.0 * scale, 3.5 * scale);
	iSetColor(190, 225, 245);
	drawRect(bx + 39.0 * scale, cy + 40.0 * scale, 10.0 * scale, 10.0 * scale);
	iSetColor(220, 160, 50);
	iFilledCircle(bx + 44.0 * scale, cy + 44.0 * scale, 3.0 * scale);

	// Stove & Kettle
	iSetColor(40, 42, 45);
	drawRect(bx + w - 34.0 * scale, cy + 39.0 * scale, 18.0 * scale, 4.0 * scale);
	iSetColor(245, 120, 20);
	drawRect(bx + w - 32.0 * scale, cy + 41.0 * scale, 14.0 * scale, 2.5 * scale);

	iSetColor(195, 200, 208);
	drawRect(bx + w - 32.0 * scale, cy + 43.5 * scale, 15.0 * scale, 13.0 * scale);
	iFilledCircle(bx + w - 24.5 * scale, cy + 56.5 * scale, 5.0 * scale);
	iSetColor(160, 165, 172);
	iLine(bx + w - 32.0 * scale, cy + 49.0 * scale, bx + w - 38.0 * scale, cy + 54.0 * scale);

	iSetColor(255, 255, 255);
	iFilledCircle(bx + w - 38.0 * scale, cy + 59.0 * scale, 2.5 * scale);
	iFilledCircle(bx + w - 36.0 * scale, cy + 65.0 * scale, 3.2 * scale);

	iSetColor(245, 215, 35);
	for (int b = 0; b < 4; b++) {
		iFilledCircle(bx + w - 14.0 * scale + b * 2.8 * scale, cy + h - 22.0 * scale - b * 1.8 * scale, 3.2 * scale);
	}

	int rColor = (variant == 0) ? 35 : 185;
	int gColor = (variant == 0) ? 95 : 45;
	int bColor = (variant == 0) ? 175 : 40;
	iSetColor(rColor, gColor, bColor);
	double roofX[] = { bx - 8.0 * scale, bx + w + 8.0 * scale, bx + w + 3.0 * scale, bx - 3.0 * scale };
	double roofY[] = { cy + h - 16.0 * scale, cy + h - 16.0 * scale, cy + h, cy + h };
	iFilledPolygon(roofX, roofY, 4);

	iSetColor((int)(rColor * 0.8), (int)(gColor * 0.8), (int)(bColor * 0.8));
	for (int f = 0; f < 6; f++) {
		double fx = bx - 6.0 * scale + f * (w / 5.2);
		iLine(fx, cy + h - 16.0 * scale, fx + 4.0 * scale, cy + h);
	}

	iSetColor(220, 35, 35);
	drawRect(bx + 12.0 * scale, cy + h - 14.0 * scale, w - 24.0 * scale, 9.5 * scale);
	iSetColor(255, 255, 255);
	drawRect(bx + 16.0 * scale, cy + h - 11.5 * scale, w - 32.0 * scale, 4.5 * scale);

	iSetColor(110, 72, 44);
	drawRect(bx + 8.0 * scale, cy, w - 16.0 * scale, 9.0 * scale);
	iSetColor(40, 95, 175);
	drawRect(bx + 18.0 * scale, cy + 9.0 * scale, 12.0 * scale, 16.0 * scale);
	iSetColor(225, 175, 140);
	iFilledCircle(bx + 24.0 * scale, cy + 30.0 * scale, 5.0 * scale);
	iSetColor(255, 255, 255);
	drawRect(bx + 28.0 * scale, cy + 20.0 * scale, 4.0 * scale, 4.5 * scale);
}

// Fuchka & Chotpoti Cart
void drawFuchkaCart(double cx, double cy, double scale, int side, int variant) {
	if (scale <= 0.05) return;

	double baseW = 96.0;
	double baseH = 90.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = cx - w * 0.5;

	iSetColor(28, 30, 34);
	drawRect(bx - 3.0 * scale, cy - 2.5 * scale, w + 6.0 * scale, 5.0 * scale);

	iSetColor(40, 42, 45);
	iFilledCircle(bx + 16.0 * scale, cy + 12.0 * scale, 12.0 * scale);
	iFilledCircle(bx + w - 16.0 * scale, cy + 12.0 * scale, 12.0 * scale);
	iSetColor(180, 185, 190);
	iFilledCircle(bx + 16.0 * scale, cy + 12.0 * scale, 7.5 * scale);
	iFilledCircle(bx + w - 16.0 * scale, cy + 12.0 * scale, 7.5 * scale);

	iSetColor(140, 88, 50);
	drawRect(bx + 4.0 * scale, cy + 14.0 * scale, w - 8.0 * scale, 24.0 * scale);
	iSetColor(170, 110, 65);
	drawRect(bx, cy + 36.0 * scale, w, 5.0 * scale);

	iSetColor(145, 205, 245);
	drawRect(bx + 6.0 * scale, cy + 41.0 * scale, w * 0.55, 22.0 * scale);
	iSetColor(240, 190, 45);
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 4; col++) {
			iFilledCircle(bx + 12.0 * scale + col * 9.0 * scale, cy + 46.0 * scale + row * 6.5 * scale, 3.8 * scale);
		}
	}

	iSetColor(195, 200, 205);
	drawRect(bx + w * 0.65, cy + 41.0 * scale, 18.0 * scale, 14.0 * scale);
	iSetColor(140, 65, 30);
	drawRect(bx + w * 0.67, cy + 43.0 * scale, 14.0 * scale, 10.0 * scale);

	iSetColor(50, 52, 55);
	drawRect(bx + 3.0 * scale, cy + 36.0 * scale, 3.0 * scale, h - 40.0 * scale);
	drawRect(bx + w - 6.0 * scale, cy + 36.0 * scale, 3.0 * scale, h - 40.0 * scale);

	for (int s = 0; s < 6; s++) {
		if (s % 2 == 0) iSetColor(220, 35, 35);
		else iSetColor(250, 250, 250);
		drawRect(bx + s * (w / 6.0), cy + h - 12.0 * scale, w / 6.0, 12.0 * scale);
	}
	iSetColor(190, 30, 30);
	double canopyTopX[] = { bx - 4.0 * scale, bx + w + 4.0 * scale, bx + w * 0.5 };
	double canopyTopY[] = { cy + h - 12.0 * scale, cy + h - 12.0 * scale, cy + h + 6.0 * scale };
	iFilledPolygon(canopyTopX, canopyTopY, 3);
}

// Fresh Fruit & Daab Vendor Cart
void drawFruitVendorCart(double cx, double cy, double scale, int side, int variant) {
	if (scale <= 0.05) return;

	double baseW = 92.0;
	double baseH = 92.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = cx - w * 0.5;

	iSetColor(28, 30, 34);
	drawRect(bx, cy - 2.5 * scale, w, 5.0 * scale);

	iSetColor(40, 42, 45);
	iFilledCircle(bx + 14.0 * scale, cy + 10.0 * scale, 10.0 * scale);
	iFilledCircle(bx + w - 14.0 * scale, cy + 10.0 * scale, 10.0 * scale);

	iSetColor(135, 85, 45);
	drawRect(bx + 4.0 * scale, cy + 12.0 * scale, w - 8.0 * scale, 22.0 * scale);
	iSetColor(160, 105, 60);
	drawRect(bx, cy + 32.0 * scale, w, 5.0 * scale);

	iSetColor(35, 145, 55);
	iFilledCircle(bx + 14.0 * scale, cy + 42.0 * scale, 6.5 * scale);
	iFilledCircle(bx + 26.0 * scale, cy + 43.0 * scale, 7.0 * scale);
	iFilledCircle(bx + 20.0 * scale, cy + 52.0 * scale, 6.2 * scale);

	iSetColor(225, 40, 45);
	double wmX[] = { bx + 42.0 * scale, bx + 64.0 * scale, bx + 53.0 * scale };
	double wmY[] = { cy + 38.0 * scale, cy + 38.0 * scale, cy + 52.0 * scale };
	iFilledPolygon(wmX, wmY, 3);
	iSetColor(35, 125, 45);
	drawRect(bx + 42.0 * scale, cy + 37.0 * scale, 22.0 * scale, 3.5 * scale);

	iSetColor(55, 58, 62);
	drawRect(cx - 2.0 * scale, cy + 32.0 * scale, 4.0 * scale, h - 34.0 * scale);

	double umbY = cy + h;
	iSetColor(245, 205, 30);
	iFilledCircle(cx, umbY, 32.0 * scale);
	iSetColor(220, 35, 35);
	double w1X[] = { cx - 22.0 * scale, cx - 7.0 * scale, cx };
	double w1Y[] = { umbY, umbY, umbY + 22.0 * scale };
	iFilledPolygon(w1X, w1Y, 3);
	double w2X[] = { cx + 7.0 * scale, cx + 22.0 * scale, cx };
	double w2Y[] = { umbY, umbY, umbY + 22.0 * scale };
	iFilledPolygon(w2X, w2Y, 3);
}

// Promenade Dual Recycling Dustbins (DCC Green & Blue)
void drawPromenadeDustbin(double cx, double cy, double scale, int side) {
	if (scale <= 0.05) return;

	double baseW = 48.0;
	double baseH = 48.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = cx - w * 0.5;

	iSetColor(140, 145, 152);
	drawRect(bx - 3.0 * scale, cy, w + 6.0 * scale, 5.0 * scale);

	iSetColor(22, 145, 62);
	drawRect(bx + 2.0 * scale, cy + 5.0 * scale, w * 0.44, 30.0 * scale);
	iSetColor(15, 80, 35);
	drawRect(bx, cy + 33.0 * scale, w * 0.48, 6.0 * scale);
	iSetColor(10, 45, 20);
	drawRect(bx + 5.0 * scale, cy + 26.0 * scale, w * 0.30, 4.0 * scale);

	iSetColor(30, 95, 205);
	drawRect(bx + w * 0.52, cy + 5.0 * scale, w * 0.44, 30.0 * scale);
	iSetColor(18, 55, 135);
	drawRect(bx + w * 0.50, cy + 33.0 * scale, w * 0.48, 6.0 * scale);
	iSetColor(10, 30, 80);
	drawRect(bx + w * 0.59, cy + 26.0 * scale, w * 0.30, 4.0 * scale);

	iSetColor(255, 255, 255);
	drawRect(bx + 7.0 * scale, cy + 14.0 * scale, 7.0 * scale, 7.0 * scale);
	drawRect(bx + w * 0.65, cy + 14.0 * scale, 7.0 * scale, 7.0 * scale);
}

// Park Bench
void drawParkBenchFlowerbed(double cx, double cy, double scale, int side) {
	if (scale <= 0.05) return;

	double w = 78.0 * scale;
	double h = 42.0 * scale;
	double bx = cx - w * 0.5;

	iSetColor(65, 45, 30);
	drawRect(bx - 4.0 * scale, cy, w + 8.0 * scale, 6.0 * scale);
	iSetColor(32, 125, 45);
	iFilledCircle(bx + 8.0 * scale, cy + 6.0 * scale, 9.0 * scale);
	iFilledCircle(bx + w - 8.0 * scale, cy + 6.0 * scale, 9.0 * scale);

	iSetColor(235, 40, 45);
	iFilledCircle(bx + 6.0 * scale, cy + 8.0 * scale, 3.5 * scale);
	iSetColor(255, 210, 30);
	iFilledCircle(bx + w - 6.0 * scale, cy + 8.0 * scale, 3.5 * scale);

	iSetColor(25, 65, 35);
	drawRect(bx + 6.0 * scale, cy + 2.0 * scale, 4.0 * scale, 20.0 * scale);
	drawRect(bx + w - 10.0 * scale, cy + 2.0 * scale, 4.0 * scale, 20.0 * scale);

	iSetColor(165, 100, 50);
	drawRect(bx + 2.0 * scale, cy + 16.0 * scale, w - 4.0 * scale, 4.5 * scale);
	drawRect(bx + 2.0 * scale, cy + 24.0 * scale, w - 4.0 * scale, 4.0 * scale);
}

// Kids playing in park
void drawKidsPark(double cx, double cy, double scale, int side, int variant) {
	if (scale <= 0.05) return;

	double baseW = 95.0;
	double w = baseW * scale;
	double bx = cx - w * 0.5;

	iSetColor(42, 148, 52);
	drawRect(bx - 5.0 * scale, cy, w + 10.0 * scale, 8.0 * scale);
	iSetColor(240, 55, 75);
	iFilledCircle(bx + 4.0 * scale, cy + 9.0 * scale, 2.5 * scale);
	iSetColor(255, 220, 30);
	iFilledCircle(bx + w - 6.0 * scale, cy + 9.0 * scale, 2.5 * scale);
	iSetColor(235, 130, 240);
	iFilledCircle(bx + w * 0.5, cy + 8.5 * scale, 2.2 * scale);

	// Kid 1: running/kicking
	double k1x = bx + 18.0 * scale;
	double k1y = cy + 4.0 * scale;
	iSetColor(35, 75, 150);
	drawRect(k1x - 3.0 * scale, k1y, 2.5 * scale, 9.0 * scale);
	drawRect(k1x + 1.0 * scale, k1y + 2.0 * scale, 2.5 * scale, 8.0 * scale);
	iSetColor(230, 45, 45);
	drawRect(k1x - 4.0 * scale, k1y + 8.5 * scale, 8.0 * scale, 12.0 * scale);
	iSetColor(245, 185, 145);
	drawRect(k1x + 3.5 * scale, k1y + 13.0 * scale, 7.0 * scale, 2.5 * scale);
	iFilledCircle(k1x, k1y + 23.5 * scale, 4.2 * scale);
	iSetColor(40, 25, 20);
	iFilledCircle(k1x, k1y + 25.5 * scale, 3.8 * scale);

	// Soccer ball
	double ballX = bx + w * 0.48;
	double ballY = cy + 18.0 * scale;
	iSetColor(30, 35, 30);
	iFilledCircle(ballX, cy + 2.0 * scale, 3.0 * scale);
	iSetColor(250, 250, 250);
	iFilledCircle(ballX, ballY, 4.5 * scale);
	iSetColor(25, 25, 25);
	iFilledCircle(ballX, ballY, 1.8 * scale);
	iFilledCircle(ballX - 2.5 * scale, ballY + 1.5 * scale, 1.2 * scale);
	iFilledCircle(ballX + 2.5 * scale, ballY - 1.5 * scale, 1.2 * scale);

	// Kid 2: catching
	double k2x = bx + w - 20.0 * scale;
	double k2y = cy + 6.0 * scale;
	iSetColor(40, 115, 60);
	drawRect(k2x - 3.5 * scale, k2y, 2.5 * scale, 9.0 * scale);
	drawRect(k2x + 1.5 * scale, k2y + 1.0 * scale, 2.5 * scale, 8.0 * scale);
	iSetColor(245, 200, 25);
	drawRect(k2x - 4.5 * scale, k2y + 8.5 * scale, 9.0 * scale, 12.0 * scale);
	iSetColor(245, 185, 145);
	drawRect(k2x - 7.5 * scale, k2y + 14.0 * scale, 6.0 * scale, 2.5 * scale);
	drawRect(k2x + 4.0 * scale, k2y + 16.0 * scale, 5.0 * scale, 2.5 * scale);
	iFilledCircle(k2x, k2y + 24.0 * scale, 4.2 * scale);
	iSetColor(65, 40, 25);
	iFilledCircle(k2x, k2y + 26.0 * scale, 3.8 * scale);

	// Balloon
	double balX = bx + w - 7.0 * scale;
	double balY = cy + 34.0 * scale;
	iSetColor(80, 80, 85);
	iLine(balX, cy + 5.0 * scale, balX, balY);
	iSetColor(235, 35, 120);
	iFilledCircle(balX, balY, 5.0 * scale);
	iSetColor(255, 170, 210);
	iFilledCircle(balX - 1.5 * scale, balY + 1.5 * scale, 1.5 * scale);
}

// Playground with slide & swings
void drawPlayground(double cx, double cy, double scale, int side) {
	if (scale <= 0.05) return;

	double baseW = 100.0;
	double w = baseW * scale;
	double bx = cx - w * 0.5;

	iSetColor(42, 148, 52);
	drawRect(bx - 4.0 * scale, cy, w + 8.0 * scale, 6.0 * scale);

	// Slide
	double sX = bx + 6.0 * scale;
	double sH = 38.0 * scale;
	iSetColor(145, 150, 155);
	drawRect(sX, cy, 2.5 * scale, sH);
	for (int r = 1; r <= 4; r++) {
		drawRect(sX - 1.0 * scale, cy + r * 7.0 * scale, 4.5 * scale, 1.5 * scale);
	}
	iSetColor(235, 45, 45);
	drawRect(sX - 2.0 * scale, cy + sH - 2.0 * scale, 10.0 * scale, 3.0 * scale);
	iSetColor(245, 185, 25);
	double chuteX[] = { sX + 8.0 * scale, sX + 32.0 * scale, sX + 34.0 * scale, sX + 8.0 * scale };
	double chuteY[] = { cy + sH, cy + 2.0 * scale, cy + 2.0 * scale, cy + sH - 4.0 * scale };
	iFilledPolygon(chuteX, chuteY, 4);
	iSetColor(215, 30, 30);
	iLine(sX + 8.0 * scale, cy + sH + 2.0 * scale, sX + 33.0 * scale, cy + 4.0 * scale);

	// Swing set
	double swX = bx + w - 38.0 * scale;
	double swW = 34.0 * scale;
	double swH = 36.0 * scale;
	iSetColor(35, 125, 220);
	iLine(swX, cy, swX + 6.0 * scale, cy + swH);
	iLine(swX + 12.0 * scale, cy, swX + 6.0 * scale, cy + swH);
	iLine(swX + swW - 12.0 * scale, cy, swX + swW - 6.0 * scale, cy + swH);
	iLine(swX + swW, cy, swX + swW - 6.0 * scale, cy + swH);
	drawRect(swX + 4.0 * scale, cy + swH - 2.0 * scale, swW - 8.0 * scale, 3.0 * scale);
	iSetColor(70, 75, 80);
	iLine(swX + 13.0 * scale, cy + swH, swX + 13.0 * scale, cy + 10.0 * scale);
	iLine(swX + 19.0 * scale, cy + swH, swX + 19.0 * scale, cy + 10.0 * scale);
	iSetColor(220, 45, 45);
	drawRect(swX + 11.5 * scale, cy + 8.5 * scale, 9.0 * scale, 2.5 * scale);
	iSetColor(70, 75, 80);
	iLine(swX + 23.0 * scale, cy + swH, swX + 25.0 * scale, cy + 13.0 * scale);
	iLine(swX + 29.0 * scale, cy + swH, swX + 31.0 * scale, cy + 13.0 * scale);
	iSetColor(245, 185, 25);
	drawRect(swX + 23.5 * scale, cy + 11.5 * scale, 9.0 * scale, 2.5 * scale);
}

// News & Magazine Kiosk
void drawNewsKiosk(double cx, double cy, double scale, int side) {
	if (scale <= 0.05) return;

	double baseW = 84.0;
	double baseH = 82.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = cx - w * 0.5;

	iSetColor(30, 32, 35);
	drawRect(bx - 3.0 * scale, cy - 2.5 * scale, w + 6.0 * scale, 5.0 * scale);

	iSetColor(40, 68, 105);
	drawRect(bx, cy, w, h - 14.0 * scale);
	iSetColor(15, 30, 50);
	drawRect(bx + 5.0 * scale, cy + 10.0 * scale, w - 10.0 * scale, h - 30.0 * scale);

	iSetColor(245, 245, 245);
	drawRect(bx + 8.0 * scale, cy + 14.0 * scale, 14.0 * scale, 18.0 * scale);
	iSetColor(225, 45, 45);
	drawRect(bx + 26.0 * scale, cy + 14.0 * scale, 14.0 * scale, 18.0 * scale);
	iSetColor(245, 205, 30);
	drawRect(bx + 44.0 * scale, cy + 14.0 * scale, 14.0 * scale, 18.0 * scale);

	iSetColor(220, 160, 35);
	double roofX[] = { bx - 5.0 * scale, bx + w + 5.0 * scale, bx + w, bx };
	double roofY[] = { cy + h - 16.0 * scale, cy + h - 16.0 * scale, cy + h, cy + h };
	iFilledPolygon(roofX, roofY, 4);
}

void drawOuterPropL2(int index) {
	if (!outerProps[index].active) return;
	double y = outerProps[index].y;
	double scale = getPerspectiveScaleL2(y);
	double cx = getOuterPromenadeXL2(outerProps[index].side, outerProps[index].offsetRatio, y);
	int type = outerProps[index].type;
	int side = outerProps[index].side;
	int variant = outerProps[index].variant;

	if (type == 0) drawTongTeaStall(cx, y, scale, side, variant);
	else if (type == 1) drawFuchkaCart(cx, y, scale, side, variant);
	else if (type == 2) drawFruitVendorCart(cx, y, scale, side, variant);
	else if (type == 3) drawParkBenchFlowerbed(cx, y, scale, side);
	else if (type == 4) drawKidsPark(cx, y, scale, side, variant);
	else if (type == 5) drawPromenadeDustbin(cx, y, scale, side);
	else if (type == 6) drawNewsKiosk(cx, y, scale, side);
	else if (type == 7) drawPlayground(cx, y, scale, side);
}

// Dynamic Outer Ground that alters appearance when theme cycles every 4 seconds
void drawDynamicOuterGroundL2(int theme) {
	if (theme == 1) {
		// Theme 1: City park green lawns
		iSetColor(52, 138, 62);
		drawRect(0, 0, SCREEN_WIDTH, horizonYL2);
		iSetColor(68, 162, 78);
		for (float p = 0.1f; p <= 0.9f; p += 0.18f) {
			double py = horizonYL2 * p;
			drawRect(0, py, SCREEN_WIDTH, 2.0);
		}
	}
	else if (theme == 0) {
		// Theme 0: Traditional warm sidewalk pavers
		iSetColor(188, 178, 168);
		drawRect(0, 0, SCREEN_WIDTH, horizonYL2);
		iSetColor(165, 155, 145);
		for (float p = 0.1f; p <= 0.9f; p += 0.2f) {
			double py = horizonYL2 * p;
			drawRect(0, py, SCREEN_WIDTH, 1.5);
		}
	}
	else {
		// Theme 2: Commercial Promenade clean modern tiles
		iSetColor(198, 202, 208);
		drawRect(0, 0, SCREEN_WIDTH, horizonYL2);
		iSetColor(170, 175, 182);
		for (float p = 0.1f; p <= 0.9f; p += 0.2f) {
			double py = horizonYL2 * p;
			drawRect(0, py, SCREEN_WIDTH, 1.8);
		}
	}
}

void drawProceduralBackgroundL2() {
	iSetColor(70, 160, 235);
	drawRect(0, horizonYL2, SCREEN_WIDTH, SCREEN_HEIGHT - horizonYL2);

	for (int i = -1; i <= 1; i++) {
		double shiftX = cloudOffsetL2 + (i * SCREEN_WIDTH);
		drawRealisticCloud(150 + shiftX, horizonYL2 + 195, 0.95);
		drawRealisticCloud(430 + shiftX, horizonYL2 + 225, 1.25);
		drawRealisticCloud(680 + shiftX, horizonYL2 + 175, 0.85);
		drawRealisticCloud(880 + shiftX, horizonYL2 + 215, 1.05);
	}

	drawRealisticBuildingsL2();
	drawDynamicOuterGroundL2(currentRoadsideTheme);

	// Outer Stalls / Parks (Constant at their positions)
	int sortedOuter[MAX_OUTER_PROPS];
	for (int i = 0; i < MAX_OUTER_PROPS; i++) sortedOuter[i] = i;
	for (int i = 0; i < MAX_OUTER_PROPS - 1; i++) {
		for (int j = i + 1; j < MAX_OUTER_PROPS; j++) {
			if (outerProps[sortedOuter[i]].y < outerProps[sortedOuter[j]].y) {
				int tmp = sortedOuter[i];
				sortedOuter[i] = sortedOuter[j];
				sortedOuter[j] = tmp;
			}
		}
	}
	for (int i = 0; i < MAX_OUTER_PROPS; i++) {
		drawOuterPropL2(sortedOuter[i]);
	}

	iSetColor(140, 140, 140);
	double leftCurbX[] = { 0, 380, 390, 0 };
	double rightCurbX[] = { 900, 520, 510, 900 };
	double roadY[] = { 0, (double)horizonYL2, (double)horizonYL2, 0 };
	iFilledPolygon(leftCurbX, roadY, 4);
	iFilledPolygon(rightCurbX, roadY, 4);

	iSetColor(50, 55, 60);
	double roadX[] = { 60, 395, 505, 840 };
	iFilledPolygon(roadX, roadY, 4);

	iSetColor(255, 255, 255);
	double leftWhiteX[] = { 75, 397, 400, 85 };
	double rightWhiteX[] = { 825, 503, 500, 815 };
	iFilledPolygon(leftWhiteX, roadY, 4);
	iFilledPolygon(rightWhiteX, roadY, 4);

	iSetColor(34, 120, 60);
	double lRailTopX[] = { 0, 375, 377, 0 };
	double lRailTopY[] = { 80, (double)horizonYL2 + 12, (double)horizonYL2 + 16, 88 };
	double lRailBotY[] = { 40, (double)horizonYL2 + 4, (double)horizonYL2 + 8, 48 };
	iFilledPolygon(lRailTopX, lRailTopY, 4);
	iFilledPolygon(lRailTopX, lRailBotY, 4);

	double rRailTopX[] = { 900, 525, 523, 900 };
	iFilledPolygon(rRailTopX, lRailTopY, 4);
	iFilledPolygon(rRailTopX, lRailBotY, 4);

	for (float p = 0.0f; p <= 1.0f; p += 0.2f) {
		double postY_bottom = horizonYL2 * p;
		double postY_top = postY_bottom + (20 + (1 - p) * 60);

		double lx = 375 * p;
		double leftPostX[] = { lx, lx + 4 + (1 - p) * 6, lx + 4 + (1 - p) * 6, lx };
		double postY[] = { postY_bottom, postY_bottom, postY_top, postY_top };
		iFilledPolygon(leftPostX, postY, 4);

		double rx = 900 - (375 * p);
		double rightPostX[] = { rx - (4 + (1 - p) * 6), rx, rx, rx - (4 + (1 - p) * 6) };
		iFilledPolygon(rightPostX, postY, 4);
	}

	iSetColor(245, 190, 20);
	for (float t = lineOffsetL2; t < 1.0f; t += 0.25f) {
		double topPercent = 1.0f - t;
		double bottomPercent = 1.0f - (t + 0.15f);
		if (bottomPercent < 0) bottomPercent = 0;

		double yTop = horizonYL2 * topPercent;
		double yBottom = horizonYL2 * bottomPercent;

		double halfWidthTop = 2.0 + (1.0 - topPercent) * 10.0;
		double halfWidthBottom = 2.0 + (1.0 - bottomPercent) * 10.0;

		double yellowX[] = { 450 - halfWidthBottom, 450 + halfWidthBottom, 450 + halfWidthTop, 450 - halfWidthTop };
		double segY[] = { yBottom, yBottom, yTop, yTop };
		iFilledPolygon(yellowX, segY, 4);
	}

	drawOverheadPowerLinesL2();
}

// Vector Red Heart (Love sign) & Lives HUD (Top-right corner)
void drawVectorHeart(int cx, int cy, int size, int isFilled) {
	double r = (double)size;
	if (isFilled) {
		iSetColor(20, 22, 25);
		iFilledCircle(cx - r * 0.48 + 1, cy + r * 0.22 - 1, r * 0.52);
		iFilledCircle(cx + r * 0.48 + 1, cy + r * 0.22 - 1, r * 0.52);
		double shX[] = { cx - r * 0.95 + 1, cx + r * 0.95 + 1, (double)(cx + 1) };
		double shY[] = { cy + r * 0.22 - 1, cy + r * 0.22 - 1, cy - r * 0.85 - 1 };
		iFilledPolygon(shX, shY, 3);

		iSetColor(235, 30, 45);
		iFilledCircle(cx - r * 0.48, cy + r * 0.22, r * 0.52);
		iFilledCircle(cx + r * 0.48, cy + r * 0.22, r * 0.52);
		double triX[] = { cx - r * 0.95, cx + r * 0.95, (double)cx };
		double triY[] = { cy + r * 0.22, cy + r * 0.22, cy - r * 0.85 };
		iFilledPolygon(triX, triY, 3);

		iSetColor(255, 175, 185);
		iFilledCircle(cx - r * 0.48, cy + r * 0.40, r * 0.18);
	}
	else {
		iSetColor(85, 90, 95);
		iFilledCircle(cx - r * 0.48, cy + r * 0.22, r * 0.52);
		iFilledCircle(cx + r * 0.48, cy + r * 0.22, r * 0.52);
		double triX[] = { cx - r * 0.95, cx + r * 0.95, (double)cx };
		double triY[] = { cy + r * 0.22, cy + r * 0.22, cy - r * 0.85 };
		iFilledPolygon(triX, triY, 3);

		iSetColor(32, 34, 38);
		iFilledCircle(cx - r * 0.48, cy + r * 0.22, r * 0.38);
		iFilledCircle(cx + r * 0.48, cy + r * 0.22, r * 0.38);
		double inX[] = { cx - r * 0.72, cx + r * 0.72, (double)cx };
		double inY[] = { cy + r * 0.20, cy + r * 0.20, cy - r * 0.65 };
		iFilledPolygon(inX, inY, 3);
	}
}

void drawLivesHUDL2() {
	int hudX = 710;
	int hudY = 538;
	int hudW = 175;
	int hudH = 48;

	iSetColor(18, 22, 28);
	drawRect(hudX, hudY, hudW, hudH);

	iSetColor(215, 180, 50);
	drawRect(hudX, hudY + hudH - 2, hudW, 2);
	drawRect(hudX, hudY, hudW, 2);
	drawRect(hudX, hudY, 2, hudH);
	drawRect(hudX + hudW - 2, hudY, 2, hudH);

	iSetColor(255, 255, 255);
	iText(hudX + 14, hudY + 18, "LIVES", GLUT_BITMAP_HELVETICA_12);

	for (int i = 0; i < 3; i++) {
		int heartX = hudX + 78 + i * 32;
		int heartY = hudY + 24;
		drawVectorHeart(heartX, heartY, 12, (i < playerLivesL2));
	}
}

void drawCoinsHUDL2() {
	int hudX = 710;
	int hudY = 482;
	int hudW = 175;
	int hudH = 48;

	iSetColor(18, 22, 28);
	drawRect(hudX, hudY, hudW, hudH);

	iSetColor(215, 180, 50);
	drawRect(hudX, hudY + hudH - 2, hudW, 2);
	drawRect(hudX, hudY, hudW, 2);
	drawRect(hudX, hudY, 2, hudH);
	drawRect(hudX + hudW - 2, hudY, 2, hudH);

	int iconX = hudX + 22;
	int iconY = hudY + 24;
	iSetColor(150, 95, 10);
	drawFilledEllipse(iconX, iconY - 2, 11, 11, 16);
	iSetColor(245, 195, 30);
	drawFilledEllipse(iconX, iconY, 11, 11, 16);
	iSetColor(255, 235, 120);
	drawFilledEllipse(iconX, iconY + 1, 9, 9, 16);
	iSetColor(255, 205, 30);
	drawFilledEllipse(iconX, iconY, 7, 7, 16);
	iSetColor(255, 255, 255);
	iFilledCircle(iconX - 3, iconY + 3, 2);

	iSetColor(255, 255, 255);
	iText(hudX + 42, hudY + 18, "COINS:", GLUT_BITMAP_HELVETICA_12);

	char coinStr[16];
	sprintf_s(coinStr, "%d", coinCountL2);
	iSetColor(255, 220, 50);
	iText(hudX + 104, hudY + 16, coinStr, GLUT_BITMAP_HELVETICA_18);
}

void drawLevel2Scene() {
	// Layer 1: Sky, Clouds, Buildings, Outer Sidewalk, Stalls, Road
	drawProceduralBackgroundL2();

	// Collect entities for 3D depth sorting
	SceneEntity entities[MAX_BUSES + MAX_PEOPLE + MAX_DOGS + MAX_TRAFFIC_RICKSHAWS + MAX_ROADSIDE_PROPS + MAX_COINS_L2];
	int totalEntities = 0;

	for (int i = 0; i < MAX_BUSES; i++) {
		if (buses[i].active) {
			entities[totalEntities].entityType = 0;
			entities[totalEntities].index = i;
			entities[totalEntities].y = buses[i].y;
			totalEntities++;
		}
	}
	for (int i = 0; i < MAX_PEOPLE; i++) {
		if (people[i].active) {
			entities[totalEntities].entityType = 1;
			entities[totalEntities].index = i;
			entities[totalEntities].y = people[i].y;
			totalEntities++;
		}
	}
	for (int i = 0; i < MAX_DOGS; i++) {
		if (dogs[i].active) {
			entities[totalEntities].entityType = 2;
			entities[totalEntities].index = i;
			entities[totalEntities].y = dogs[i].y;
			totalEntities++;
		}
	}
	for (int i = 0; i < MAX_TRAFFIC_RICKSHAWS; i++) {
		if (otherRickshaws[i].active) {
			entities[totalEntities].entityType = 3;
			entities[totalEntities].index = i;
			entities[totalEntities].y = otherRickshaws[i].y;
			totalEntities++;
		}
	}
	for (int i = 0; i < MAX_ROADSIDE_PROPS; i++) {
		if (roadsideProps[i].active) {
			entities[totalEntities].entityType = 4;
			entities[totalEntities].index = i;
			entities[totalEntities].y = roadsideProps[i].y;
			totalEntities++;
		}
	}
	for (int i = 0; i < MAX_COINS_L2; i++) {
		if (coinsL2[i].active) {
			entities[totalEntities].entityType = 5;
			entities[totalEntities].index = i;
			entities[totalEntities].y = coinsL2[i].y;
			totalEntities++;
		}
	}

	for (int i = 0; i < totalEntities - 1; i++) {
		for (int j = i + 1; j < totalEntities; j++) {
			if (entities[i].y < entities[j].y) {
				SceneEntity tmp = entities[i];
				entities[i] = entities[j];
				entities[j] = tmp;
			}
		}
	}

	// Layer 2: Entities behind player rickshaw (y >= rickshawYL2)
	for (int i = 0; i < totalEntities; i++) {
		if (entities[i].y >= rickshawYL2) {
			int idx = entities[i].index;
			if (entities[i].entityType == 0) {
				double cx = getEntityCenterXL2(buses[idx].normX, buses[idx].y);
				double scale = getPerspectiveScaleL2(buses[idx].y);
				drawRealisticBus(cx, buses[idx].y, scale, buses[idx].colorTheme);
			}
			else if (entities[i].entityType == 1) {
				double cx = getEntityCenterXL2(people[idx].normX, people[idx].y);
				double scale = getPerspectiveScaleL2(people[idx].y);
				drawRealisticPerson(cx, people[idx].y, scale, people[idx].walkAnim, people[idx].outfitTheme);
			}
			else if (entities[i].entityType == 2) {
				double cx = getEntityCenterXL2(dogs[idx].normX, dogs[idx].y);
				double scale = getPerspectiveScaleL2(dogs[idx].y);
				drawRealisticDog(cx, dogs[idx].y, scale, dogs[idx].trotAnim, dogs[idx].coatColor);
			}
			else if (entities[i].entityType == 3) {
				double cx = getEntityCenterXL2(otherRickshaws[idx].normX, otherRickshaws[idx].y);
				double scale = getPerspectiveScaleL2(otherRickshaws[idx].y);
				drawRealisticRickshawL2(cx, otherRickshaws[idx].y, scale, otherRickshaws[idx].pedalAnim, otherRickshaws[idx].hoodColor);
			}
			else if (entities[i].entityType == 4) {
				drawRoadsidePropL2(idx);
			}
			else if (entities[i].entityType == 5) {
				double cx = getEntityCenterXL2(coinsL2[idx].normX, coinsL2[idx].y);
				double scale = getPerspectiveScaleL2(coinsL2[idx].y);
				drawCoinL2(cx, coinsL2[idx].y, scale, coinsL2[idx].isSpecial, coinsL2[idx].spinAnim);
			}
		}
	}

	// Layer 3: Player Rickshaw (With invulnerability blink)
	if (invulnerableTimerL2 == 0 || (invulnerableTimerL2 / 4) % 2 == 0) {
		iShowBMP2(rickshawXL2, rickshawYL2, getEquippedRickshawFrame(rickshawIndexL2), 0);
		drawActiveShieldFX((float)rickshawXL2 + 38.0f, (float)rickshawYL2 + 36.0f, 54.0f);
	}

	// Layer 4: Entities in front of player rickshaw (y < rickshawYL2)
	for (int i = 0; i < totalEntities; i++) {
		if (entities[i].y < rickshawYL2) {
			int idx = entities[i].index;
			if (entities[i].entityType == 0) {
				double cx = getEntityCenterXL2(buses[idx].normX, buses[idx].y);
				double scale = getPerspectiveScaleL2(buses[idx].y);
				drawRealisticBus(cx, buses[idx].y, scale, buses[idx].colorTheme);
			}
			else if (entities[i].entityType == 1) {
				double cx = getEntityCenterXL2(people[idx].normX, people[idx].y);
				double scale = getPerspectiveScaleL2(people[idx].y);
				drawRealisticPerson(cx, people[idx].y, scale, people[idx].walkAnim, people[idx].outfitTheme);
			}
			else if (entities[i].entityType == 2) {
				double cx = getEntityCenterXL2(dogs[idx].normX, dogs[idx].y);
				double scale = getPerspectiveScaleL2(dogs[idx].y);
				drawRealisticDog(cx, dogs[idx].y, scale, dogs[idx].trotAnim, dogs[idx].coatColor);
			}
			else if (entities[i].entityType == 3) {
				double cx = getEntityCenterXL2(otherRickshaws[idx].normX, otherRickshaws[idx].y);
				double scale = getPerspectiveScaleL2(otherRickshaws[idx].y);
				drawRealisticRickshawL2(cx, otherRickshaws[idx].y, scale, otherRickshaws[idx].pedalAnim, otherRickshaws[idx].hoodColor);
			}
			else if (entities[i].entityType == 4) {
				drawRoadsidePropL2(idx);
			}
			else if (entities[i].entityType == 5) {
				double cx = getEntityCenterXL2(coinsL2[idx].normX, coinsL2[idx].y);
				double scale = getPerspectiveScaleL2(coinsL2[idx].y);
				drawCoinL2(cx, coinsL2[idx].y, scale, coinsL2[idx].isSpecial, coinsL2[idx].spinAnim);
			}
		}
	}

	// Layer 5: HUD Display (Hearts + Live Coins Counter + Shield Button)
	drawLivesHUDL2();
	drawCoinsHUDL2();
	drawInGameShieldHUD();

	// Layer 6: Red collision border flash
	if (hitFlashTimerL2 > 0) {
		iSetColor(220, 35, 35);
		drawRect(0, 0, SCREEN_WIDTH, 8);
		drawRect(0, SCREEN_HEIGHT - 8, SCREEN_WIDTH, 8);
		drawRect(0, 0, 8, SCREEN_HEIGHT);
		drawRect(SCREEN_WIDTH - 8, 0, 8, SCREEN_HEIGHT);
	}

	// Layer 7: Game Over Screen
	if (isGameOverL2 || gameState == STATE_GAMEOVER) {
		iSetColor(15, 18, 22);
		drawRect(SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT / 2 - 90, 440, 180);
		iSetColor(220, 45, 45);
		drawRect(SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT / 2 - 90, 440, 4);
		drawRect(SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT / 2 + 86, 440, 4);
		drawRect(SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT / 2 - 90, 4, 180);
		drawRect(SCREEN_WIDTH / 2 + 216, SCREEN_HEIGHT / 2 - 90, 4, 180);

		iSetColor(255, 55, 55);
		iText(SCREEN_WIDTH / 2 - 68, SCREEN_HEIGHT / 2 + 45, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);

		iSetColor(240, 240, 245);
		iText(SCREEN_WIDTH / 2 - 165, SCREEN_HEIGHT / 2 + 15, "All 3 lives lost! You collided with an obstacle.", GLUT_BITMAP_HELVETICA_12);

		char finalCoins[32];
		sprintf_s(finalCoins, "Coins Collected: %d", coinCountL2);
		iSetColor(255, 220, 50);
		iText(SCREEN_WIDTH / 2 - 90, SCREEN_HEIGHT / 2 - 15, finalCoins, GLUT_BITMAP_HELVETICA_18);

		iSetColor(255, 215, 30);
		iText(SCREEN_WIDTH / 2 - 157, SCREEN_HEIGHT / 2 - 55, "Press 'R' to Restart   |   Press 'M' for Menu", GLUT_BITMAP_HELVETICA_12);
	}
}

void onPlayerCollisionL2(int collisionType) {
	if (activeShieldTimer > 0.0f || invulnerableTimerL2 > 0 || isGameOverL2 || gameState == STATE_GAMEOVER) return;

	playerLivesL2--;
	invulnerableTimerL2 = 65; // ~1.3s invulnerability
	hitFlashTimerL2 = 10;

	if (playerLivesL2 <= 0) {
		playerLivesL2 = 0;
		isGameOverL2 = 1;
		gameState = STATE_GAMEOVER;
		sfxTimerL2 = 0;
		PlaySound("Audios\\Gameover.wav", NULL, SND_ASYNC);
	}
	else {
		if (collisionType == 1) {
			PlaySound("Audios\\dog_sound.wav", NULL, SND_ASYNC);
			sfxTimerL2 = 75; // Resume bg music after dog sound (~1.5s)
		}
		else if (collisionType == 0) {
			PlaySound("Audios\\horn.wav", NULL, SND_ASYNC);
			sfxTimerL2 = 45; // Resume bg music after horn sound (~0.9s)
		}
		// collisionType == 2 (Person): No horn sound played, bg music continues uninterrupted
	}
}

void updateGameL2() {
	if (isGameOverL2 || gameState != STATE_PLAYING) return;

	if (activeShieldTimer > 0.0f) {
		activeShieldTimer -= 0.02f;
		if (activeShieldTimer < 0.0f) activeShieldTimer = 0.0f;
	}

	if (invulnerableTimerL2 > 0) invulnerableTimerL2--;
	if (hitFlashTimerL2 > 0) hitFlashTimerL2--;

	if (sfxTimerL2 > 0) {
		sfxTimerL2--;
		if (sfxTimerL2 == 0 && !isGameOverL2 && gameState == STATE_PLAYING) {
			PlaySound("Audios\\bg.wav", NULL, SND_ASYNC | SND_LOOP);
		}
	}

	SHORT upState = GetAsyncKeyState(VK_UP) | GetAsyncKeyState('W');
	SHORT downState = GetAsyncKeyState(VK_DOWN) | GetAsyncKeyState('S');
	SHORT leftState = GetAsyncKeyState(VK_LEFT) | GetAsyncKeyState('A');
	SHORT rightState = GetAsyncKeyState(VK_RIGHT) | GetAsyncKeyState('D');

	// Speed Controls: UP accelerates moderately, DOWN slows to starting speed (baseSpeedL2)
	if (upState & 0x8000) {
		currentSpeedL2 += accelRateL2;
		if (currentSpeedL2 > maxSpeedL2) currentSpeedL2 = maxSpeedL2;
	}
	else if (downState & 0x8000) {
		currentSpeedL2 -= decelRateL2 * 1.5f;
		if (currentSpeedL2 < baseSpeedL2) currentSpeedL2 = baseSpeedL2;
	}
	else {
		if (currentSpeedL2 > baseSpeedL2) {
			currentSpeedL2 -= decelRateL2 * 0.5f;
			if (currentSpeedL2 < baseSpeedL2) currentSpeedL2 = baseSpeedL2;
		}
	}

	// 1. Move Road Lines Forward
	lineOffsetL2 += scrollSpeedL2 * (currentSpeedL2 / baseSpeedL2);
	if (lineOffsetL2 >= 0.2f) lineOffsetL2 -= 0.2f;

	// 2. Animate Rickshaw Driver Pedaling
	animTimerL2++;
	int pedalFreq = (currentSpeedL2 > 1.8f) ? 1 : 2;
	if (animTimerL2 % pedalFreq == 0) {
		int maxFramesL2 = (equippedRickshaw == 0) ? 8 : 9;
		rickshawIndexL2 = (rickshawIndexL2 + 1) % maxFramesL2;
	}

	// 3. Scroll Clouds
	cloudOffsetL2 -= (0.35f * currentSpeedL2);
	if (cloudOffsetL2 <= -SCREEN_WIDTH) cloudOffsetL2 += SCREEN_WIDTH;

	// 4. Steering
	if (leftState & 0x8000) {
		if (rickshawXL2 > 80) rickshawXL2 -= 8;
	}
	if (rightState & 0x8000) {
		if (rickshawXL2 < 720) rickshawXL2 += 8;
	}

	float playerApproachBoost = currentSpeedL2 * 0.95f;

	// Spawn timers
	busSpawnCounter++;
	if (busSpawnCounter >= BUS_SPAWN_INTERVAL) {
		spawnBusL2();
		busSpawnCounter = 0;
	}

	rickshawSpawnCounter++;
	if (rickshawSpawnCounter >= RICKSHAW_SPAWN_INTERVAL) {
		spawnTrafficRickshawL2();
		rickshawSpawnCounter = 0;
	}

	personSpawnCounter++;
	if (personSpawnCounter >= PERSON_SPAWN_INTERVAL) {
		spawnPersonL2();
		personSpawnCounter = 0;
	}

	dogSpawnCounter++;
	if (dogSpawnCounter >= DOG_SPAWN_INTERVAL) {
		spawnDogL2();
		dogSpawnCounter = 0;
	}

	// 5. Coin Spawning & Perspective Animation
	normalCoinSpawnCounter++;
	if (normalCoinSpawnCounter >= NORMAL_COIN_SPAWN_INTERVAL) {
		spawnNormalCoinL2();
		normalCoinSpawnCounter = 0;
	}

	specialCoinSpawnCounter++;
	if (specialCoinSpawnCounter >= SPECIAL_COIN_SPAWN_INTERVAL) {
		spawnSpecialCoinL2(); // Exactly once every 4 seconds
		specialCoinSpawnCounter = 0;
	}

	for (int i = 0; i < MAX_COINS_L2; i++) {
		if (coinsL2[i].active) {
			double t = (double)(horizonYL2 - coinsL2[i].y) / horizonYL2;
			if (t < 0.0) t = 0.0;
			double perspectiveFactor = 0.35 + 0.85 * t;
			coinsL2[i].y -= (float)(playerApproachBoost * perspectiveFactor);
			coinsL2[i].spinAnim += (coinsL2[i].isSpecial ? 0.09f : 0.12f);

			if (coinsL2[i].y < -40.0f) {
				coinsL2[i].active = 0;
			}
		}
	}

	// 6. Scenery Theme Cycling Every 4 Seconds (Positions Stay Constant)
	themeCycleCounter++;
	if (themeCycleCounter >= THEME_CYCLE_INTERVAL) {
		themeCycleCounter = 0;
		currentRoadsideTheme = (currentRoadsideTheme + 1) % 3;
		applyRoadsideTheme(currentRoadsideTheme);
	}

	// Move roadside trees, poles, and street lamps
	for (int i = 0; i < MAX_ROADSIDE_PROPS; i++) {
		if (roadsideProps[i].active) {
			double t = (double)(horizonYL2 - roadsideProps[i].y) / horizonYL2;
			if (t < 0.0) t = 0.0;
			double perspectiveFactor = 0.35 + 0.85 * t;
			roadsideProps[i].y -= (float)(playerApproachBoost * perspectiveFactor);

			if (roadsideProps[i].y < -70.0f) {
				roadsideProps[i].y = (float)horizonYL2 + 5.0f;
				roadsideProps[i].variant = rand() % 3;

				int r = rand() % 100;
				if (r < 28) roadsideProps[i].type = (roadsideProps[i].side == 0) ? 2 : 3;
				else if (r < 58) roadsideProps[i].type = (rand() % 2 == 0) ? 0 : 1;
				else if (r < 72) roadsideProps[i].type = 4;
				else if (r < 84) roadsideProps[i].type = 5;
				else if (r < 92) roadsideProps[i].type = 6;
				else roadsideProps[i].type = 7;

				roadsideProps[i].lateralOffset = (roadsideProps[i].type <= 1) ? 1.0f : 0.4f;
			}
		}
	}

	// Update pedestrians and dogs (walk in road corners)
	for (int i = 0; i < MAX_PEOPLE; i++) {
		if (people[i].active) {
			double t = (double)(horizonYL2 - people[i].y) / horizonYL2;
			if (t < 0.0) t = 0.0;
			double perspectiveFactor = 0.35 + 0.85 * t;
			people[i].y -= (float)((people[i].speed + playerApproachBoost) * perspectiveFactor);
			people[i].walkAnim += 0.20f;
			if (people[i].y < -150) people[i].active = 0;
		}
	}
	for (int i = 0; i < MAX_DOGS; i++) {
		if (dogs[i].active) {
			double t = (double)(horizonYL2 - dogs[i].y) / horizonYL2;
			if (t < 0.0) t = 0.0;
			double dogPerspective = 0.45 + 0.75 * t;
			dogs[i].y -= (float)((dogs[i].speed + playerApproachBoost) * dogPerspective);
			dogs[i].trotAnim += 0.28f;
			if (dogs[i].y < -100) dogs[i].active = 0;
		}
	}

	// Traffic AI and spacing
	ActiveObstacle activeList[MAX_BUSES + MAX_TRAFFIC_RICKSHAWS];
	int totalActive = 0;

	for (int i = 0; i < MAX_BUSES; i++) {
		if (buses[i].active) {
			activeList[totalActive].type = 0;
			activeList[totalActive].index = i;
			activeList[totalActive].y = buses[i].y;
			activeList[totalActive].normX = buses[i].normX;
			activeList[totalActive].baseSpeed = buses[i].speed;
			activeList[totalActive].currentSpeed = buses[i].speed;
			totalActive++;
		}
	}
	for (int i = 0; i < MAX_TRAFFIC_RICKSHAWS; i++) {
		if (otherRickshaws[i].active) {
			activeList[totalActive].type = 3;
			activeList[totalActive].index = i;
			activeList[totalActive].y = otherRickshaws[i].y;
			activeList[totalActive].normX = otherRickshaws[i].normX;
			activeList[totalActive].baseSpeed = otherRickshaws[i].speed;
			activeList[totalActive].currentSpeed = otherRickshaws[i].speed;
			totalActive++;
		}
	}

	for (int i = 0; i < totalActive - 1; i++) {
		for (int j = i + 1; j < totalActive; j++) {
			if (activeList[i].y > activeList[j].y) {
				ActiveObstacle tmp = activeList[i];
				activeList[i] = activeList[j];
				activeList[j] = tmp;
			}
		}
	}

	for (int i = 1; i < totalActive; i++) {
		for (int j = 0; j < i; j++) {
			float wI = getEntityBaseWidthL2(activeList[i].type);
			float wJ = getEntityBaseWidthL2(activeList[j].type);
			float safeSep = (wI + wJ) / 780.0f * 0.5f + 0.040f;

			if ((float)fabs(activeList[i].normX - activeList[j].normX) < safeSep) {
				float gap = activeList[i].y - activeList[j].y;
				double scaleI = getPerspectiveScaleL2(activeList[i].y);
				float hJ = getEntityBaseHeightL2(activeList[j].type);
				float safeGap = (hJ * 0.55f + 25.0f) * (float)scaleI;

				if (gap < safeGap && gap > 0.0f) {
					if (activeList[i].currentSpeed > activeList[j].currentSpeed) {
						activeList[i].currentSpeed = activeList[j].currentSpeed;
					}
					if (gap < safeGap * 0.70f) {
						activeList[i].currentSpeed = activeList[j].currentSpeed * 0.5f;
					}
					if (gap < safeGap * 0.40f) {
						activeList[i].currentSpeed = 0.0f;
					}
				}
			}
		}
	}

	for (int k = 0; k < totalActive; k++) {
		int type = activeList[k].type;
		int idx = activeList[k].index;
		float spd = activeList[k].currentSpeed;

		double t = (double)(horizonYL2 - activeList[k].y) / horizonYL2;
		if (t < 0.0) t = 0.0;
		double perspectiveFactor = 0.35 + 0.85 * t;

		if (type == 0) {
			buses[idx].y -= (float)((spd + playerApproachBoost) * perspectiveFactor);
			if (buses[idx].y < -220) buses[idx].active = 0;
		}
		else if (type == 3) {
			otherRickshaws[idx].y -= (float)((spd + playerApproachBoost) * perspectiveFactor);
			otherRickshaws[idx].pedalAnim += 0.22f;
			if (otherRickshaws[idx].y < -150) otherRickshaws[idx].active = 0;
		}
	}

	// =========================================================================
	// ACCURATE COLLISION & COIN COLLECTION SYSTEM
	// =========================================================================
	float pL, pR, pB, pT;
	getPlayerHitbox((float)rickshawXL2, (float)rickshawYL2, &pL, &pR, &pB, &pT);

	// Coin Touch Collection Check (+1 for normal, +5 for powerful special coin)
	for (int i = 0; i < MAX_COINS_L2; i++) {
		if (coinsL2[i].active) {
			double scale = getPerspectiveScaleL2(coinsL2[i].y);
			double cx = getEntityCenterXL2(coinsL2[i].normX, coinsL2[i].y);
			float cRadius = (coinsL2[i].isSpecial ? 21.0f : 14.5f) * (float)scale;
			float cL = (float)cx - cRadius;
			float cR = (float)cx + cRadius;
			float cB = coinsL2[i].y - 5.0f;
			float cT = coinsL2[i].y + cRadius * 2.2f;

			if (checkAABB(pL - 10.0f, pR + 10.0f, pB - 8.0f, pT + 8.0f, cL, cR, cB, cT)) {
				if (coinsL2[i].isSpecial) {
					coinCountL2 += 5; // Powerful special coin gives +5
					currentRunCoins += 5;
					totalCoins += 5;
				}
				else {
					coinCountL2 += 1; // Normal gold coin gives +1
					currentRunCoins += 1;
					totalCoins += 1;
				}
				coinsL2[i].active = 0;
				saveUserData();

				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
				sfxTimerL2 = 40; // Resume bg music after coin collect sound (~0.8s)
			}
		}
	}

	// 1. Check Bus Collisions -> Deflected by Shield or 1 Life Lost
	for (int i = 0; i < MAX_BUSES; i++) {
		if (buses[i].active) {
			double scale = getPerspectiveScaleL2(buses[i].y);
			double cx = getEntityCenterXL2(buses[i].normX, buses[i].y);
			float bW = (float)(96.0 * scale);
			float bH = (float)(130.0 * scale);
			float bL = (float)(cx - bW * 0.5);
			float bR = (float)(cx + bW * 0.5);
			float bB = buses[i].y;
			float bT = buses[i].y + bH;

			if (checkAABB(pL, pR, pB, pT, bL, bR, bB, bT)) {
				if (activeShieldTimer > 0.0f) {
					buses[i].active = 0;
					PlaySound("Audios\\bomb.wav", NULL, SND_ASYNC);
					sfxTimerL2 = 30;
				}
				else {
					onPlayerCollisionL2(0); // Hit bus -> horn sound
					buses[i].active = 0;
				}
				break;
			}
		}
	}

	// 2. Check Traffic Rickshaw Collisions -> Deflected by Shield or 1 Life Lost
	for (int i = 0; i < MAX_TRAFFIC_RICKSHAWS; i++) {
		if (otherRickshaws[i].active) {
			double scale = getPerspectiveScaleL2(otherRickshaws[i].y);
			double cx = getEntityCenterXL2(otherRickshaws[i].normX, otherRickshaws[i].y);
			float rW = (float)(74.0 * scale);
			float rH = (float)(100.0 * scale);
			float rL = (float)(cx - rW * 0.5);
			float rR = (float)(cx + rW * 0.5);
			float rB = otherRickshaws[i].y;
			float rT = otherRickshaws[i].y + rH;

			if (checkAABB(pL, pR, pB, pT, rL, rR, rB, rT)) {
				if (activeShieldTimer > 0.0f) {
					otherRickshaws[i].active = 0;
					PlaySound("Audios\\bomb.wav", NULL, SND_ASYNC);
					sfxTimerL2 = 30;
				}
				else {
					onPlayerCollisionL2(0); // Hit traffic rickshaw -> horn sound
					otherRickshaws[i].active = 0;
				}
				break;
			}
		}
	}

	// 3. Check Pedestrian (Human) Collisions -> Deflected by Shield or 1 Life Lost
	for (int i = 0; i < MAX_PEOPLE; i++) {
		if (people[i].active) {
			double scale = getPerspectiveScaleL2(people[i].y);
			double cx = getEntityCenterXL2(people[i].normX, people[i].y);
			float hW = (float)(32.0 * scale);
			float hH = (float)(100.0 * scale);
			float hL = (float)(cx - hW * 0.5);
			float hR = (float)(cx + hW * 0.5);
			float hB = people[i].y;
			float hT = people[i].y + hH;

			if (checkAABB(pL, pR, pB, pT, hL, hR, hB, hT)) {
				if (activeShieldTimer > 0.0f) {
					people[i].active = 0;
					PlaySound("Audios\\bomb.wav", NULL, SND_ASYNC);
					sfxTimerL2 = 30;
				}
				else {
					onPlayerCollisionL2(2); // Hit pedestrian -> no horn sound
					people[i].active = 0;
				}
				break;
			}
		}
	}

	// 4. Check Street Dog Collisions -> Deflected by Shield or 1 Life Lost
	for (int i = 0; i < MAX_DOGS; i++) {
		if (dogs[i].active) {
			double scale = getPerspectiveScaleL2(dogs[i].y);
			double cx = getEntityCenterXL2(dogs[i].normX, dogs[i].y);
			float dW = (float)(26.0 * scale);
			float dH = (float)(35.0 * scale);
			float dL = (float)(cx - dW * 0.5);
			float dR = (float)(cx + dW * 0.5);
			float dB = dogs[i].y;
			float dT = dogs[i].y + dH;

			if (checkAABB(pL, pR, pB, pT, dL, dR, dB, dT)) {
				if (activeShieldTimer > 0.0f) {
					dogs[i].active = 0;
					PlaySound("Audios\\bomb.wav", NULL, SND_ASYNC);
					sfxTimerL2 = 30;
				}
				else {
					onPlayerCollisionL2(1); // Hit dog -> dog sound
					dogs[i].active = 0;
				}
				break;
			}
		}
	}
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
double getEntityCenterXL3(float normX, double y) {
	double t = (double)(horizonYL3 - y) / horizonYL3;
	if (t < 0.0) t = 0.0;
	double leftRoadX = 395.0 - 335.0 * t;
	double rightRoadX = 505.0 + 335.0 * t;
	return leftRoadX + (double)normX * (rightRoadX - leftRoadX);
}

// Perspective scale factor (L3)
double getPerspectiveScaleL3(double y) {
	double t = (double)(horizonYL3 - y) / horizonYL3;
	if (t < 0.0) t = 0.0;
	return 0.22 + 0.78 * t;
}

// Perspective X coordinate for roadside props (L3)
double getRoadsideXL3(int side, float lateralOffset, double y) {
	double t = (double)(horizonYL3 - y) / horizonYL3;
	if (t < 0.0) t = 0.0;
	double leftRoadX = 395.0 - 335.0 * t;
	double rightRoadX = 505.0 + 335.0 * t;
	double scale = getPerspectiveScaleL3(y);
	if (side == 0) {
		return leftRoadX - (16.0 + (double)lateralOffset * 34.0) * scale;
	}
	else {
		return rightRoadX + (16.0 + (double)lateralOffset * 34.0) * scale;
	}
}

// Perspective X coordinate for outer promenade / river bank (L3)
double getOuterPromenadeXL3(int side, float offsetRatio, double y) {
	double t = (double)(horizonYL3 - y) / horizonYL3;
	if (t < 0.0) t = 0.0;
	if (side == 0) {
		double railX = 375.0 * (1.0 - t);
		if (railX < 24.0) railX = 24.0;
		return 12.0 + (double)offsetRatio * (railX - 24.0);
	}
	else {
		double railX = 525.0 + 375.0 * t;
		if (railX > 876.0) railX = 876.0;
		return railX + 12.0 + (double)offsetRatio * (900.0 - railX - 24.0);
	}
}

// Physical base widths (L3)
float getEntityBaseWidthL3(int entityType) {
	if (entityType == 0) return 104.0f; // Bus
	if (entityType == 3) return 82.0f;  // Traffic Rickshaw
	if (entityType == 1) return 32.0f;  // Person with umbrella
	if (entityType == 6) return 38.0f;  // Stone
	if (entityType == 7) return 30.0f;  // Boom
	return 24.0f;                       // Dog
}

// Physical base heights (L3)
float getEntityBaseHeightL3(int entityType) {
	if (entityType == 0) return 138.0f; // Bus
	if (entityType == 3) return 112.0f; // Traffic Rickshaw
	if (entityType == 1) return 105.0f; // Person
	if (entityType == 6) return 26.0f;  // Stone
	if (entityType == 7) return 30.0f;  // Boom
	return 34.0f;                       // Dog
}

// Safe spawn position check for vehicles, stones and booms (L3)
int isSpawnPositionSafeL3(float testNormX, int testType) {
	float testW = getEntityBaseWidthL3(testType);
	float testNormW = testW / 780.0f;

	for (int i = 0; i < MAX_BUSES_L3; i++) {
		if (busesL3[i].active && busesL3[i].y > 210.0f) {
			float otherNormW = getEntityBaseWidthL3(0) / 780.0f;
			float safeSep = (testNormW + otherNormW) * 0.5f + 0.05f;
			if ((float)fabs(testNormX - busesL3[i].normX) < safeSep) return 0;
		}
	}

	for (int i = 0; i < MAX_TRAFFIC_RICKSHAWS_L3; i++) {
		if (otherRickshawsL3[i].active && otherRickshawsL3[i].y > 210.0f) {
			float otherNormW = getEntityBaseWidthL3(3) / 780.0f;
			float safeSep = (testNormW + otherNormW) * 0.5f + 0.05f;
			if ((float)fabs(testNormX - otherRickshawsL3[i].normX) < safeSep) return 0;
		}
	}

	for (int i = 0; i < MAX_STONES_L3; i++) {
		if (roadStonesL3[i].active && roadStonesL3[i].y > 210.0f) {
			float otherNormW = getEntityBaseWidthL3(6) / 780.0f;
			float safeSep = (testNormW + otherNormW) * 0.5f + 0.06f;
			if ((float)fabs(testNormX - roadStonesL3[i].normX) < safeSep) return 0;
		}
	}

	for (int i = 0; i < MAX_BOOMS_L3; i++) {
		if (roadBoomsL3[i].active && roadBoomsL3[i].y > 210.0f) {
			float otherNormW = getEntityBaseWidthL3(7) / 780.0f;
			float safeSep = (testNormW + otherNormW) * 0.5f + 0.06f;
			if ((float)fabs(testNormX - roadBoomsL3[i].normX) < safeSep) return 0;
		}
	}

	return 1;
}

// Safe spawn check for coins (L3)
int isCoinSpawnSafeL3(float testNormX) {
	for (int i = 0; i < MAX_BUSES_L3; i++) {
		if (busesL3[i].active && busesL3[i].y > 220.0f) {
			if ((float)fabs(testNormX - busesL3[i].normX) < 0.14f) return 0;
		}
	}
	for (int i = 0; i < MAX_TRAFFIC_RICKSHAWS_L3; i++) {
		if (otherRickshawsL3[i].active && otherRickshawsL3[i].y > 220.0f) {
			if ((float)fabs(testNormX - otherRickshawsL3[i].normX) < 0.12f) return 0;
		}
	}
	for (int i = 0; i < MAX_STONES_L3; i++) {
		if (roadStonesL3[i].active && roadStonesL3[i].y > 220.0f) {
			if ((float)fabs(testNormX - roadStonesL3[i].normX) < 0.12f) return 0;
		}
	}
	for (int i = 0; i < MAX_BOOMS_L3; i++) {
		if (roadBoomsL3[i].active && roadBoomsL3[i].y > 220.0f) {
			if ((float)fabs(testNormX - roadBoomsL3[i].normX) < 0.12f) return 0;
		}
	}
	return 1;
}

// Safe spawn check for bullet pickups (L3) - ensures 2 bullets never overlap with anything
int isBulletSpawnSafeL3(float testNormX) {
	for (int i = 0; i < MAX_BUSES_L3; i++) {
		if (busesL3[i].active && busesL3[i].y > 210.0f) {
			if ((float)fabs(testNormX - busesL3[i].normX) < 0.15f) return 0;
		}
	}
	for (int i = 0; i < MAX_TRAFFIC_RICKSHAWS_L3; i++) {
		if (otherRickshawsL3[i].active && otherRickshawsL3[i].y > 210.0f) {
			if ((float)fabs(testNormX - otherRickshawsL3[i].normX) < 0.13f) return 0;
		}
	}
	for (int i = 0; i < MAX_STONES_L3; i++) {
		if (roadStonesL3[i].active && roadStonesL3[i].y > 210.0f) {
			if ((float)fabs(testNormX - roadStonesL3[i].normX) < 0.13f) return 0;
		}
	}
	for (int i = 0; i < MAX_BOOMS_L3; i++) {
		if (roadBoomsL3[i].active && roadBoomsL3[i].y > 210.0f) {
			if ((float)fabs(testNormX - roadBoomsL3[i].normX) < 0.13f) return 0;
		}
	}
	for (int i = 0; i < MAX_COINS_L3; i++) {
		if (coinsL3[i].active && coinsL3[i].y > 210.0f) {
			if ((float)fabs(testNormX - coinsL3[i].normX) < 0.12f) return 0;
		}
	}
	for (int i = 0; i < MAX_BULLET_PICKUPS_L3; i++) {
		if (bulletPickupsL3[i].active && bulletPickupsL3[i].y > 210.0f) {
			if ((float)fabs(testNormX - bulletPickupsL3[i].normX) < 0.14f) return 0;
		}
	}
	return 1;
}

// Spawns 2 non-overlapping bullet pickups every 3.5 seconds (L3)
void spawnBulletPickupsL3() {
	int spawned = 0;
	for (int i = 0; i < MAX_BULLET_PICKUPS_L3 && spawned < 2; i++) {
		if (!bulletPickupsL3[i].active) {
			for (int attempt = 0; attempt < 25; attempt++) {
				float candidateNormX = 0.18f + ((rand() % 1000) / 1000.0f) * (0.82f - 0.18f);
				if (isBulletSpawnSafeL3(candidateNormX)) {
					bulletPickupsL3[i].active = 1;
					bulletPickupsL3[i].y = (float)horizonYL3;
					bulletPickupsL3[i].normX = candidateNormX;
					bulletPickupsL3[i].animPhase = (float)(rand() % 100);
					spawned++;
					break;
				}
			}
		}
	}
}

void spawnBusL3() {
	for (int i = 0; i < MAX_BUSES_L3; i++) {
		if (!busesL3[i].active) {
			for (int attempt = 0; attempt < 20; attempt++) {
				float candidateNormX = 0.16f + ((rand() % 1000) / 1000.0f) * (0.84f - 0.16f);
				if (isSpawnPositionSafeL3(candidateNormX, 0)) {
					busesL3[i].active = 1;
					busesL3[i].y = (float)horizonYL3;
					busesL3[i].normX = candidateNormX;
					busesL3[i].speed = 1.35f + ((rand() % 6) / 10.0f);

					int nextColor;
					do {
						nextColor = rand() % 8;
					} while (nextColor == lastBusColorL3);
					lastBusColorL3 = nextColor;
					busesL3[i].colorTheme = nextColor;
					return;
				}
			}
			busSpawnCounterL3 = BUS_SPAWN_INTERVAL_L3 - 25;
			return;
		}
	}
}

void spawnTrafficRickshawL3() {
	for (int i = 0; i < MAX_TRAFFIC_RICKSHAWS_L3; i++) {
		if (!otherRickshawsL3[i].active) {
			for (int attempt = 0; attempt < 20; attempt++) {
				float candidateNormX = 0.14f + ((rand() % 1000) / 1000.0f) * (0.86f - 0.14f);
				if (isSpawnPositionSafeL3(candidateNormX, 3)) {
					otherRickshawsL3[i].active = 1;
					otherRickshawsL3[i].y = (float)horizonYL3;
					otherRickshawsL3[i].normX = candidateNormX;
					otherRickshawsL3[i].speed = 1.05f + ((rand() % 4) / 10.0f);
					otherRickshawsL3[i].pedalAnim = (float)(rand() % 100);
					otherRickshawsL3[i].hoodColor = rand() % 4;
					return;
				}
			}
			rickshawSpawnCounterL3 = RICKSHAW_SPAWN_INTERVAL_L3 - 25;
			return;
		}
	}
}

void spawnStoneL3() {
	for (int i = 0; i < MAX_STONES_L3; i++) {
		if (!roadStonesL3[i].active) {
			for (int attempt = 0; attempt < 20; attempt++) {
				float candidateNormX = 0.20f + ((rand() % 1000) / 1000.0f) * (0.80f - 0.20f);
				if (isSpawnPositionSafeL3(candidateNormX, 6)) {
					roadStonesL3[i].active = 1;
					roadStonesL3[i].y = (float)horizonYL3;
					roadStonesL3[i].normX = candidateNormX;
					roadStonesL3[i].size = 32.0f + (rand() % 12);
					roadStonesL3[i].variant = rand() % 3;
					return;
				}
			}
			return;
		}
	}
}

void spawnBoomL3() {
	for (int i = 0; i < MAX_BOOMS_L3; i++) {
		if (!roadBoomsL3[i].active) {
			for (int attempt = 0; attempt < 20; attempt++) {
				float candidateNormX = 0.18f + ((rand() % 1000) / 1000.0f) * (0.82f - 0.18f);
				if (isSpawnPositionSafeL3(candidateNormX, 7)) {
					roadBoomsL3[i].active = 1;
					roadBoomsL3[i].y = (float)horizonYL3;
					roadBoomsL3[i].normX = candidateNormX;
					roadBoomsL3[i].pulseAnim = (float)(rand() % 100);
					return;
				}
			}
			return;
		}
	}
}

void spawnPersonL3() {
	static int nextPersonGenderL3 = 0;
	for (int i = 0; i < MAX_PEOPLE_L3; i++) {
		if (!peopleL3[i].active) {
			peopleL3[i].active = 1;
			peopleL3[i].y = (float)horizonYL3;
			peopleL3[i].normX = (rand() % 2 == 0) ? 0.07f : 0.93f;
			peopleL3[i].speed = 0.70f + ((rand() % 5) / 10.0f);
			peopleL3[i].walkAnim = (float)(rand() % 100);
			peopleL3[i].outfitTheme = rand() % 4;
			peopleL3[i].umbrellaColor = rand() % 4;
			peopleL3[i].gender = nextPersonGenderL3;
			nextPersonGenderL3 = 1 - nextPersonGenderL3;
			return;
		}
	}
}

void spawnDogL3() {
	for (int i = 0; i < MAX_DOGS_L3; i++) {
		if (!dogsL3[i].active) {
			dogsL3[i].active = 1;
			dogsL3[i].y = (float)horizonYL3;
			dogsL3[i].normX = (rand() % 2 == 0) ? 0.06f : 0.94f;
			dogsL3[i].speed = 1.35f + ((rand() % 5) / 10.0f);
			dogsL3[i].trotAnim = (float)(rand() % 100);
			dogsL3[i].coatColor = rand() % 4;
			return;
		}
	}
}

void spawnNormalCoinL3() {
	for (int i = 0; i < MAX_COINS_L3; i++) {
		if (!coinsL3[i].active) {
			for (int attempt = 0; attempt < 15; attempt++) {
				float candidateNormX = 0.18f + ((rand() % 1000) / 1000.0f) * (0.82f - 0.18f);
				if (isCoinSpawnSafeL3(candidateNormX)) {
					coinsL3[i].active = 1;
					coinsL3[i].isSpecial = 0;
					coinsL3[i].y = (float)horizonYL3;
					coinsL3[i].normX = candidateNormX;
					coinsL3[i].spinAnim = (float)(rand() % 100);
					return;
				}
			}
			return;
		}
	}
}

void spawnSpecialCoinL3() {
	for (int i = 0; i < MAX_COINS_L3; i++) {
		if (!coinsL3[i].active) {
			for (int attempt = 0; attempt < 20; attempt++) {
				float candidateNormX = 0.22f + ((rand() % 1000) / 1000.0f) * (0.78f - 0.22f);
				if (isCoinSpawnSafeL3(candidateNormX)) {
					coinsL3[i].active = 1;
					coinsL3[i].isSpecial = 1;
					coinsL3[i].y = (float)horizonYL3;
					coinsL3[i].normX = candidateNormX;
					coinsL3[i].spinAnim = 0.0f;
					return;
				}
			}
			return;
		}
	}
}

void initRainSystemL3() {
	for (int i = 0; i < MAX_RAINDROPS_L3; i++) {
		rainDropsL3[i].x = (float)(rand() % (SCREEN_WIDTH + 150) - 50);
		rainDropsL3[i].y = (float)(rand() % SCREEN_HEIGHT);
		rainDropsL3[i].speed = 14.0f + (rand() % 9);
		rainDropsL3[i].length = 12.0f + (rand() % 8);
		rainDropsL3[i].slant = -2.2f - ((rand() % 10) / 10.0f);
		rainDropsL3[i].brightness = 170 + (rand() % 75);
	}
}

void initGameObjectsL3() {
	playerLivesL3 = 5;
	invulnerableTimerL3 = 0;
	hitFlashTimerL3 = 0;
	isGameOverL3 = 0;
	currentSpeedL3 = baseSpeedL3;
	rickshawXL3 = 400;

	sfxTimerL3 = 0;
	if (musicOn) {
		PlaySound("Audios\\L3_Music.wav", NULL, SND_ASYNC | SND_LOOP);
	}

	currentPathModeL3 = 0;
	pathCycleCounterL3 = 0;
	thunderTimerL3 = 160;
	thunderFlashTimerL3 = 0;
	lightningBoltActiveL3 = 0;

	coinCountL3 = 0;
	normalCoinSpawnCounterL3 = 0;
	specialCoinSpawnCounterL3 = 0;
	stoneSpawnCounterL3 = 60;
	boomSpawnCounterL3 = 120;
	for (int i = 0; i < MAX_COINS_L3; i++) coinsL3[i].active = 0;
	for (int i = 0; i < MAX_STONES_L3; i++) roadStonesL3[i].active = 0;
	for (int i = 0; i < MAX_BOOMS_L3; i++) roadBoomsL3[i].active = 0;
	for (int i = 0; i < MAX_BURSTS_L3; i++) burstsL3[i].active = 0;

	bulletSpawnCounterL3 = 80;
	if (playerBulletsL3 < 2) playerBulletsL3 = 2;
	doubleTapTimerL3 = 0;
	prevUpStateL3 = 0;
	for (int i = 0; i < MAX_BULLET_PICKUPS_L3; i++) bulletPickupsL3[i].active = 0;
	for (int i = 0; i < MAX_THROWN_BULLETS_L3; i++) thrownBulletsL3[i].active = 0;

	bulletPickupsL3[0].active = 1;
	bulletPickupsL3[0].y = 230.0f;
	bulletPickupsL3[0].normX = 0.35f;
	bulletPickupsL3[0].animPhase = 0.0f;

	coinsL3[0].active = 1;
	coinsL3[0].isSpecial = 0;
	coinsL3[0].y = 210.0f;
	coinsL3[0].normX = 0.48f;
	coinsL3[0].spinAnim = 0.0f;

	for (int i = 0; i < MAX_BUSES_L3; i++) busesL3[i].active = 0;
	for (int i = 0; i < MAX_TRAFFIC_RICKSHAWS_L3; i++) otherRickshawsL3[i].active = 0;
	for (int i = 0; i < MAX_PEOPLE_L3; i++) peopleL3[i].active = 0;
	for (int i = 0; i < MAX_DOGS_L3; i++) dogsL3[i].active = 0;

	peopleL3[0].active = 1;
	peopleL3[0].y = 210.0f;
	peopleL3[0].normX = 0.07f;
	peopleL3[0].speed = 0.70f;
	peopleL3[0].walkAnim = 0.0f;
	peopleL3[0].outfitTheme = 0;
	peopleL3[0].umbrellaColor = 0;
	peopleL3[0].gender = 0;

	peopleL3[1].active = 1;
	peopleL3[1].y = 135.0f;
	peopleL3[1].normX = 0.93f;
	peopleL3[1].speed = 0.75f;
	peopleL3[1].walkAnim = 25.0f;
	peopleL3[1].outfitTheme = 1;
	peopleL3[1].umbrellaColor = 1;
	peopleL3[1].gender = 1;

	dogsL3[0].active = 1;
	dogsL3[0].y = 180.0f;
	dogsL3[0].normX = 0.08f;
	dogsL3[0].speed = 1.35f;
	dogsL3[0].trotAnim = 0.0f;
	dogsL3[0].coatColor = 1;

	busesL3[0].active = 1;
	busesL3[0].y = 220.0f;
	busesL3[0].normX = 0.72f;
	busesL3[0].speed = 1.4f;
	busesL3[0].colorTheme = rand() % 8;
	lastBusColorL3 = busesL3[0].colorTheme;

	otherRickshawsL3[0].active = 1;
	otherRickshawsL3[0].y = 150.0f;
	otherRickshawsL3[0].normX = 0.28f;
	otherRickshawsL3[0].speed = 1.15f;
	otherRickshawsL3[0].hoodColor = rand() % 4;
	otherRickshawsL3[0].pedalAnim = 0.0f;

	int leftTypes[] = { 2, 0, 4, 2, 5, 0, 6 };
	int rightTypes[] = { 3, 1, 7, 3, 1, 4, 6 };

	for (int i = 0; i < 7; i++) {
		roadsidePropsL3[i].active = 1;
		roadsidePropsL3[i].side = 0;
		roadsidePropsL3[i].y = 285.0f - (float)i * 42.0f;
		roadsidePropsL3[i].type = leftTypes[i];
		roadsidePropsL3[i].variant = rand() % 3;
		roadsidePropsL3[i].lateralOffset = (roadsidePropsL3[i].type <= 1) ? 1.0f : 0.4f;

		roadsidePropsL3[7 + i].active = 1;
		roadsidePropsL3[7 + i].side = 1;
		roadsidePropsL3[7 + i].y = 270.0f - (float)i * 42.0f;
		roadsidePropsL3[7 + i].type = rightTypes[i];
		roadsidePropsL3[7 + i].variant = rand() % 3;
		roadsidePropsL3[7 + i].lateralOffset = (roadsidePropsL3[7 + i].type <= 1) ? 1.0f : 0.4f;
	}

	outerPropsL3[0].active = 1; outerPropsL3[0].side = 0; outerPropsL3[0].y = 215.0f; outerPropsL3[0].type = 0; outerPropsL3[0].variant = 0; outerPropsL3[0].offsetRatio = 0.50f; outerPropsL3[0].replaceCounter = 160; outerPropsL3[0].shopperGender = 1;
	outerPropsL3[1].active = 1; outerPropsL3[1].side = 0; outerPropsL3[1].y = 115.0f; outerPropsL3[1].type = 2; outerPropsL3[1].variant = 0; outerPropsL3[1].offsetRatio = 0.45f; outerPropsL3[1].replaceCounter = 80;  outerPropsL3[1].shopperGender = 0;
	outerPropsL3[2].active = 1; outerPropsL3[2].side = 1; outerPropsL3[2].y = 220.0f; outerPropsL3[2].type = 1; outerPropsL3[2].variant = 0; outerPropsL3[2].offsetRatio = 0.50f; outerPropsL3[2].replaceCounter = 120; outerPropsL3[2].shopperGender = 0;
	outerPropsL3[3].active = 1; outerPropsL3[3].side = 1; outerPropsL3[3].y = 120.0f; outerPropsL3[3].type = 3; outerPropsL3[3].variant = 1; outerPropsL3[3].offsetRatio = 0.45f; outerPropsL3[3].replaceCounter = 40;  outerPropsL3[3].shopperGender = 1;

	initRainSystemL3();
}

// Procedural Realistic Stone Road Obstacle (L3)
void drawRealisticStoneL3(double centerX, double baseY, double scale, int variant) {
	if (scale <= 0.05) return;

	double w = 36.0 * scale;
	double h = 24.0 * scale;

	iSetColor(15, 17, 22);
	drawFilledEllipse(centerX, baseY - 1.0 * scale, w * 0.58, h * 0.32, 12);

	iSetColor(48, 52, 60);
	double bX[] = { centerX - w * 0.5, centerX + w * 0.5, centerX + w * 0.35, centerX - w * 0.38 };
	double bY[] = { baseY, baseY, baseY + h * 0.45, baseY + h * 0.45 };
	iFilledPolygon(bX, bY, 4);

	iSetColor(75, 82, 92);
	double mX[] = { centerX - w * 0.45, centerX + w * 0.40, centerX + w * 0.20, centerX - w * 0.25 };
	double mY[] = { baseY + h * 0.30, baseY + h * 0.30, baseY + h, baseY + h };
	iFilledPolygon(mX, mY, 4);

	iSetColor(135, 145, 160);
	double hlX[] = { centerX - w * 0.25, centerX + w * 0.20, centerX + w * 0.08, centerX - w * 0.15 };
	double hlY[] = { baseY + h * 0.70, baseY + h * 0.70, baseY + h, baseY + h };
	iFilledPolygon(hlX, hlY, 4);

	iSetColor(32, 35, 40);
	iLine(centerX - w * 0.10, baseY + h * 0.85, centerX + w * 0.15, baseY + h * 0.35);
	iLine(centerX - w * 0.28, baseY + h * 0.40, centerX - w * 0.05, baseY + h * 0.20);
}

// Procedural Cartoon Bomb Obstacle (L3)
void drawBoomL3(double centerX, double baseY, double scale, float pulseAnim) {
	if (scale <= 0.05) return;

	double radius = 14.5 * scale;
	double cy = baseY + radius + 1.0 * scale;

	iSetColor(14, 16, 20);
	drawFilledEllipse(centerX, baseY - 1.0 * scale, radius * 0.95, radius * 0.28, 12);

	iSetColor(8, 8, 12);
	iFilledCircle(centerX, cy, radius + 1.2 * scale);

	iSetColor(30, 32, 38);
	iFilledCircle(centerX, cy, radius);

	iSetColor(18, 19, 24);
	drawFilledEllipse(centerX - radius * 0.18, cy - radius * 0.18, radius * 0.84, radius * 0.84, 16);

	iSetColor(255, 255, 255);
	drawFilledEllipse(centerX + radius * 0.36, cy + radius * 0.22, radius * 0.26, radius * 0.38, 14);
	iFilledCircle(centerX + radius * 0.36, cy - radius * 0.26, radius * 0.15);

	double neckBaseX = centerX + radius * 0.38;
	double neckBaseY = cy + radius * 0.85;

	iSetColor(8, 8, 12);
	double cPolyX[] = { neckBaseX - 4.5 * scale, neckBaseX + 5.5 * scale, neckBaseX + 8.0 * scale, neckBaseX - 2.0 * scale };
	double cPolyY[] = { neckBaseY - 1.0 * scale, neckBaseY + 3.0 * scale, neckBaseY + 8.5 * scale, neckBaseY + 4.5 * scale };
	iFilledPolygon(cPolyX, cPolyY, 4);

	iSetColor(42, 45, 54);
	drawFilledEllipse(neckBaseX + 3.0 * scale, neckBaseY + 6.5 * scale, 4.2 * scale, 2.5 * scale, 12);
	iSetColor(15, 16, 20);
	drawFilledEllipse(neckBaseX + 3.0 * scale, neckBaseY + 6.5 * scale, 2.4 * scale, 1.4 * scale, 10);

	double fX0 = neckBaseX + 3.0 * scale;
	double fY0 = neckBaseY + 6.5 * scale;
	double fX1 = neckBaseX + 8.0 * scale;
	double fY1 = neckBaseY + 12.0 * scale;
	double fX2 = neckBaseX + 13.5 * scale;
	double fY2 = neckBaseY + 15.5 * scale;
	double fX3 = neckBaseX + 18.0 * scale;
	double fY3 = neckBaseY + 17.0 * scale;

	iSetColor(205, 172, 125);
	double fuseSeg1X[] = { fX0 - 1.2 * scale, fX0 + 1.2 * scale, fX1 + 1.2 * scale, fX1 - 1.2 * scale };
	double fuseSeg1Y[] = { fY0, fY0, fY1, fY1 };
	iFilledPolygon(fuseSeg1X, fuseSeg1Y, 4);

	double fuseSeg2X[] = { fX1 - 1.2 * scale, fX1 + 1.2 * scale, fX2 + 1.2 * scale, fX2 - 1.2 * scale };
	double fuseSeg2Y[] = { fY1, fY1, fY2, fY2 };
	iFilledPolygon(fuseSeg2X, fuseSeg2Y, 4);

	double fuseSeg3X[] = { fX2 - 1.2 * scale, fX2 + 1.2 * scale, fX3 + 1.2 * scale, fX3 - 1.2 * scale };
	double fuseSeg3Y[] = { fY2, fY2, fY3, fY3 };
	iFilledPolygon(fuseSeg3X, fuseSeg3Y, 4);

	iSetColor(125, 95, 60);
	iLine(fX0, fY0 + 2.0 * scale, fX0 + 2.0 * scale, fY0 + 3.0 * scale);
	iLine(fX1, fY1 + 1.0 * scale, fX1 + 2.0 * scale, fY1 + 2.0 * scale);
	iLine(fX2, fY2, fX2 + 2.0 * scale, fY2 + 1.0 * scale);

	double spX = fX3 + 1.5 * scale;
	double spY = fY3 + 1.0 * scale;

	int points = 12;
	double outerStarX[24];
	double outerStarY[24];
	float flicker = (float)sin(pulseAnim * 12.0f);

	for (int p = 0; p < points * 2; p++) {
		double ang = (double)p * (3.14159265358979323846 / (double)points);
		double r;
		if (p % 2 == 0) {
			r = (10.0 + 3.0 * sin(pulseAnim * 15.0f + (double)p * 0.8)) * scale;
		}
		else {
			r = 3.6 * scale;
		}
		outerStarX[p] = spX + cos(ang) * r;
		outerStarY[p] = spY + sin(ang) * r;
	}
	iSetColor(230, 28, 15);
	iFilledPolygon(outerStarX, outerStarY, points * 2);

	double innerStarX[24];
	double innerStarY[24];
	for (int p = 0; p < points * 2; p++) {
		double ang = (double)p * (3.14159265358979323846 / (double)points) + 0.15;
		double r;
		if (p % 2 == 0) {
			r = (6.0 + 1.8 * sin(pulseAnim * 15.0f + (double)p * 0.8)) * scale;
		}
		else {
			r = 2.0 * scale;
		}
		innerStarX[p] = spX + cos(ang) * r;
		innerStarY[p] = spY + sin(ang) * r;
	}
	iSetColor(255, 220, 20);
	iFilledPolygon(innerStarX, innerStarY, points * 2);

	iSetColor(255, 255, 235);
	iFilledCircle(spX, spY, 1.8 * scale);

	iSetColor(255, 215, 30);
	iFilledCircle(spX + 6.0 * scale * flicker, spY + 8.0 * scale, 1.2 * scale);
	iFilledCircle(spX + 9.0 * scale, spY + 3.0 * scale * flicker, 1.0 * scale);
	iFilledCircle(spX - 5.0 * scale * flicker, spY + 7.0 * scale, 1.1 * scale);
}

// =========================================================================
// PROCEDURAL REALISTIC ROCKET-BULLET (MATCHING USER REFERENCE IMAGE)
// =========================================================================
void drawRocketBulletL3(double cx, double cy, double scale, float angleDeg, int isFired, float flameAnim) {
	if (scale <= 0.05) return;

	double rad = (double)angleDeg * (3.14159265358979323846 / 180.0);
	double cosA = cos(rad);
	double sinA = sin(rad);

#define ROT_X(u, v) (cx + ((u) * cosA - (v) * sinA))
#define ROT_Y(u, v) (cy + ((u) * sinA + (v) * cosA))

	// 1. Ground shadow & collectible aura (when resting on road as pickup)
	if (!isFired) {
		iSetColor(14, 16, 20);
		drawFilledEllipse(cx, cy - 10.0 * scale, 15.0 * scale, 4.0 * scale, 12);

		float pulse = 0.5f + 0.5f * (float)sin(flameAnim * 3.5f);
		iSetColor(245, 140, 25);
		drawFilledEllipse(cx, cy, (16.0 + pulse * 3.5) * scale, (16.0 + pulse * 3.5) * scale, 14);
	}

	// 2. Fiery Rocket Exhaust Thrust Plume
	float f1 = (float)sin(flameAnim * 14.0f);
	float f2 = (float)cos(flameAnim * 17.0f);
	double flLen = (isFired ? 35.0 : 20.0) * scale + (double)f1 * 3.5 * scale;
	double flW = (isFired ? 8.0 : 6.0) * scale;

	// Outer Orange Flame
	iSetColor(245, 95, 20);
	double outFX[] = {
		ROT_X(-13.0 * scale, -flW * 0.8),
		ROT_X(-13.0 * scale, flW * 0.8),
		ROT_X(-13.0 * scale - flLen * 0.45, flW * 1.1 + f2 * 1.5 * scale),
		ROT_X(-13.0 * scale - flLen * 0.75, flW * 0.6),
		ROT_X(-13.0 * scale - flLen, f1 * 2.0 * scale),
		ROT_X(-13.0 * scale - flLen * 0.70, -flW * 0.6),
		ROT_X(-13.0 * scale - flLen * 0.40, -flW * 1.0 - f1 * 1.5 * scale)
	};
	double outFY[] = {
		ROT_Y(-13.0 * scale, -flW * 0.8),
		ROT_Y(-13.0 * scale, flW * 0.8),
		ROT_Y(-13.0 * scale - flLen * 0.45, flW * 1.1 + f2 * 1.5 * scale),
		ROT_Y(-13.0 * scale - flLen * 0.75, flW * 0.6),
		ROT_Y(-13.0 * scale - flLen, f1 * 2.0 * scale),
		ROT_Y(-13.0 * scale - flLen * 0.70, -flW * 0.6),
		ROT_Y(-13.0 * scale - flLen * 0.40, -flW * 1.0 - f1 * 1.5 * scale)
	};
	iFilledPolygon(outFX, outFY, 7);

	// Middle Golden-Yellow Flame
	iSetColor(255, 215, 45);
	double midLen = flLen * 0.70;
	double midW = flW * 0.65;
	double midFX[] = {
		ROT_X(-13.0 * scale, -midW * 0.8),
		ROT_X(-13.0 * scale, midW * 0.8),
		ROT_X(-13.0 * scale - midLen * 0.5, midW + f1 * 1.2 * scale),
		ROT_X(-13.0 * scale - midLen, 0.0),
		ROT_X(-13.0 * scale - midLen * 0.5, -midW - f2 * 1.2 * scale)
	};
	double midFY[] = {
		ROT_Y(-13.0 * scale, -midW * 0.8),
		ROT_Y(-13.0 * scale, midW * 0.8),
		ROT_Y(-13.0 * scale - midLen * 0.5, midW + f1 * 1.2 * scale),
		ROT_Y(-13.0 * scale - midLen, 0.0),
		ROT_Y(-13.0 * scale - midLen * 0.5, -midW - f2 * 1.2 * scale)
	};
	iFilledPolygon(midFX, midFY, 5);

	// Intense White-Hot Inner Flame Core
	iSetColor(255, 255, 235);
	double corLen = flLen * 0.38;
	double corW = flW * 0.38;
	double corFX[] = {
		ROT_X(-13.0 * scale, -corW),
		ROT_X(-13.0 * scale, corW),
		ROT_X(-13.0 * scale - corLen, 0.0)
	};
	double corFY[] = {
		ROT_Y(-13.0 * scale, -corW),
		ROT_Y(-13.0 * scale, corW),
		ROT_Y(-13.0 * scale - corLen, 0.0)
	};
	iFilledPolygon(corFX, corFY, 3);

	// 3. Gunmetal Cylindrical Base Cap
	iSetColor(65, 62, 72);
	double baseCapX[] = {
		ROT_X(-13.5 * scale, -5.8 * scale),
		ROT_X(-7.0 * scale, -5.8 * scale),
		ROT_X(-7.0 * scale, 5.8 * scale),
		ROT_X(-13.5 * scale, 5.8 * scale)
	};
	double baseCapY[] = {
		ROT_Y(-13.5 * scale, -5.8 * scale),
		ROT_Y(-7.0 * scale, -5.8 * scale),
		ROT_Y(-7.0 * scale, 5.8 * scale),
		ROT_Y(-13.5 * scale, 5.8 * scale)
	};
	iFilledPolygon(baseCapX, baseCapY, 4);

	// Base cap 3D highlight
	iSetColor(115, 112, 126);
	double capHiX[] = {
		ROT_X(-13.0 * scale, -4.5 * scale),
		ROT_X(-7.5 * scale, -4.5 * scale),
		ROT_X(-7.5 * scale, -1.0 * scale),
		ROT_X(-13.0 * scale, -1.0 * scale)
	};
	double capHiY[] = {
		ROT_Y(-13.0 * scale, -4.5 * scale),
		ROT_Y(-7.5 * scale, -4.5 * scale),
		ROT_Y(-7.5 * scale, -1.0 * scale),
		ROT_Y(-13.0 * scale, -1.0 * scale)
	};
	iFilledPolygon(capHiX, capHiY, 4);

	// Base cap rim joint band
	iSetColor(40, 38, 46);
	double capRimX[] = {
		ROT_X(-7.2 * scale, -6.0 * scale),
		ROT_X(-6.2 * scale, -6.0 * scale),
		ROT_X(-6.2 * scale, 6.0 * scale),
		ROT_X(-7.2 * scale, 6.0 * scale)
	};
	double capRimY[] = {
		ROT_Y(-7.2 * scale, -6.0 * scale),
		ROT_Y(-6.2 * scale, -6.0 * scale),
		ROT_Y(-6.2 * scale, 6.0 * scale),
		ROT_Y(-7.2 * scale, 6.0 * scale)
	};
	iFilledPolygon(capRimX, capRimY, 4);

	// 4. Main Metallic Bronze / Amber Body Casing
	iSetColor(220, 118, 38);
	double bodyX[] = {
		ROT_X(-6.2 * scale, -5.4 * scale),
		ROT_X(6.5 * scale, -5.4 * scale),
		ROT_X(6.5 * scale, 5.4 * scale),
		ROT_X(-6.2 * scale, 5.4 * scale)
	};
	double bodyY[] = {
		ROT_Y(-6.2 * scale, -5.4 * scale),
		ROT_Y(6.5 * scale, -5.4 * scale),
		ROT_Y(6.5 * scale, 5.4 * scale),
		ROT_Y(-6.2 * scale, 5.4 * scale)
	};
	iFilledPolygon(bodyX, bodyY, 4);

	// 3D Cylindrical Light Reflection streak on casing
	iSetColor(255, 205, 125);
	double bodyHiX[] = {
		ROT_X(-6.0 * scale, -4.2 * scale),
		ROT_X(6.3 * scale, -4.2 * scale),
		ROT_X(6.3 * scale, -1.2 * scale),
		ROT_X(-6.0 * scale, -1.2 * scale)
	};
	double bodyHiY[] = {
		ROT_Y(-6.0 * scale, -4.2 * scale),
		ROT_Y(6.3 * scale, -4.2 * scale),
		ROT_Y(6.3 * scale, -1.2 * scale),
		ROT_Y(-6.0 * scale, -1.2 * scale)
	};
	iFilledPolygon(bodyHiX, bodyHiY, 4);

	// Deep ambient shadow on lower half of casing
	iSetColor(150, 62, 18);
	double bodyShX[] = {
		ROT_X(-6.0 * scale, 1.8 * scale),
		ROT_X(6.3 * scale, 1.8 * scale),
		ROT_X(6.3 * scale, 5.2 * scale),
		ROT_X(-6.0 * scale, 5.2 * scale)
	};
	double bodyShY[] = {
		ROT_Y(-6.0 * scale, 1.8 * scale),
		ROT_Y(6.3 * scale, 1.8 * scale),
		ROT_Y(6.3 * scale, 5.2 * scale),
		ROT_Y(-6.0 * scale, 5.2 * scale)
	};
	iFilledPolygon(bodyShX, bodyShY, 4);

	// Geometric Tech Notches / Stencil Markings
	iSetColor(135, 55, 18);
	double st1X[] = {
		ROT_X(-3.5 * scale, -2.5 * scale),
		ROT_X(0.0 * scale, -2.5 * scale),
		ROT_X(0.0 * scale, 3.5 * scale),
		ROT_X(-3.5 * scale, 3.5 * scale)
	};
	double st1Y[] = {
		ROT_Y(-3.5 * scale, -2.5 * scale),
		ROT_Y(0.0 * scale, -2.5 * scale),
		ROT_Y(0.0 * scale, 3.5 * scale),
		ROT_Y(-3.5 * scale, 3.5 * scale)
	};
	iFilledPolygon(st1X, st1Y, 4);

	double st2X[] = {
		ROT_X(1.5 * scale, -2.5 * scale),
		ROT_X(5.0 * scale, -2.5 * scale),
		ROT_X(5.0 * scale, 3.5 * scale),
		ROT_X(1.5 * scale, 3.5 * scale)
	};
	double st2Y[] = {
		ROT_Y(1.5 * scale, -2.5 * scale),
		ROT_Y(5.0 * scale, -2.5 * scale),
		ROT_Y(5.0 * scale, 3.5 * scale),
		ROT_Y(1.5 * scale, 3.5 * scale)
	};
	iFilledPolygon(st2X, st2Y, 4);

	// 5. Dark Graphite Collar Segment
	iSetColor(68, 68, 76);
	double colX[] = {
		ROT_X(6.5 * scale, -5.5 * scale),
		ROT_X(9.8 * scale, -5.5 * scale),
		ROT_X(9.8 * scale, 5.5 * scale),
		ROT_X(6.5 * scale, 5.5 * scale)
	};
	double colY[] = {
		ROT_Y(6.5 * scale, -5.5 * scale),
		ROT_Y(9.8 * scale, -5.5 * scale),
		ROT_Y(9.8 * scale, 5.5 * scale),
		ROT_Y(6.5 * scale, 5.5 * scale)
	};
	iFilledPolygon(colX, colY, 4);

	// 6. Crisp White / Chrome Ring Band
	iSetColor(245, 248, 255);
	double wRingX[] = {
		ROT_X(9.8 * scale, -5.7 * scale),
		ROT_X(11.8 * scale, -5.7 * scale),
		ROT_X(11.8 * scale, 5.7 * scale),
		ROT_X(9.8 * scale, 5.7 * scale)
	};
	double wRingY[] = {
		ROT_Y(9.8 * scale, -5.7 * scale),
		ROT_Y(11.8 * scale, -5.7 * scale),
		ROT_Y(11.8 * scale, 5.7 * scale),
		ROT_Y(9.8 * scale, 5.7 * scale)
	};
	iFilledPolygon(wRingX, wRingY, 4);

	// 7. Aerodynamic Pointed Bullet Warhead Tip
	iSetColor(240, 160, 55);
	double tipX[] = {
		ROT_X(11.8 * scale, -5.4 * scale),
		ROT_X(11.8 * scale, 5.4 * scale),
		ROT_X(17.5 * scale, 2.5 * scale),
		ROT_X(21.0 * scale, 0.0),
		ROT_X(17.5 * scale, -2.5 * scale)
	};
	double tipY[] = {
		ROT_Y(11.8 * scale, -5.4 * scale),
		ROT_Y(11.8 * scale, 5.4 * scale),
		ROT_Y(17.5 * scale, 2.5 * scale),
		ROT_Y(21.0 * scale, 0.0),
		ROT_Y(17.5 * scale, -2.5 * scale)
	};
	iFilledPolygon(tipX, tipY, 5);

	// Tip 3D Specular Highlight Streak
	iSetColor(255, 240, 185);
	double tipHiX[] = {
		ROT_X(12.0 * scale, -4.2 * scale),
		ROT_X(12.0 * scale, -1.0 * scale),
		ROT_X(19.0 * scale, -0.4 * scale),
		ROT_X(16.5 * scale, -2.5 * scale)
	};
	double tipHiY[] = {
		ROT_Y(12.0 * scale, -4.2 * scale),
		ROT_Y(12.0 * scale, -1.0 * scale),
		ROT_Y(19.0 * scale, -0.4 * scale),
		ROT_Y(16.5 * scale, -2.5 * scale)
	};
	iFilledPolygon(tipHiX, tipHiY, 4);

#undef ROT_X
#undef ROT_Y
}

// =========================================================================
// TOP-LEFT HUD: BULLET STORAGE DISPLAY (LEVEL 3 ONLY)
// =========================================================================
void drawBulletsHUDL3() {
	int hudX = 20;
	int hudY = 538;
	int hudW = 180;
	int hudH = 48;

	// Dark glass background panel
	iSetColor(14, 18, 26);
	drawRect(hudX, hudY, hudW, hudH);

	// Polished metallic gold border
	iSetColor(215, 180, 50);
	drawRect(hudX, hudY + hudH - 2, hudW, 2);
	drawRect(hudX, hudY, hudW, 2);
	drawRect(hudX, hudY, 2, hudH);
	drawRect(hudX + hudW - 2, hudY, 2, hudH);

	// Mini rocket-bullet icon
	drawRocketBulletL3(hudX + 22, hudY + 24, 0.72, 45.0f, 0, 0.0f);

	// Label & Bullet Count
	iSetColor(255, 255, 255);
	iText(hudX + 44, hudY + 26, "BULLETS:", GLUT_BITMAP_HELVETICA_12);

	char bulletStr[16];
	sprintf_s(bulletStr, sizeof(bulletStr), "%d", playerBulletsL3);
	iSetColor(255, 220, 50);
	iText(hudX + 115, hudY + 24, bulletStr, GLUT_BITMAP_HELVETICA_18);

	// Double-tap tip text
	iSetColor(120, 215, 255);
	iText(hudX + 44, hudY + 10, "[DOUBLE UP: SHOOT]", GLUT_BITMAP_HELVETICA_10);
}

void triggerBoomBurstL3(double x, double y) {
	for (int i = 0; i < MAX_BURSTS_L3; i++) {
		if (!burstsL3[i].active) {
			burstsL3[i].active = 1;
			burstsL3[i].x = (float)x;
			burstsL3[i].y = (float)y;
			burstsL3[i].timer = 18;
			burstsL3[i].maxTimer = 18;
			burstsL3[i].maxRadius = 65.0f;
			return;
		}
	}
	burstsL3[0].active = 1;
	burstsL3[0].x = (float)x;
	burstsL3[0].y = (float)y;
	burstsL3[0].timer = 18;
	burstsL3[0].maxTimer = 18;
	burstsL3[0].maxRadius = 65.0f;
}

void drawExplosionBurstsL3() {
	for (int i = 0; i < MAX_BURSTS_L3; i++) {
		if (burstsL3[i].active) {
			float progress = 1.0f - (float)burstsL3[i].timer / (float)burstsL3[i].maxTimer;
			double curRadius = 10.0 + (double)(burstsL3[i].maxRadius - 10.0) * progress;
			double cx = burstsL3[i].x;
			double cy = burstsL3[i].y;

			iSetColor(255, 55, 20);
			drawFilledEllipse(cx, cy, curRadius, curRadius * 0.75, 16);

			iSetColor(255, 160, 25);
			drawFilledEllipse(cx, cy, curRadius * 0.70, curRadius * 0.55, 16);

			if (burstsL3[i].timer > 7) {
				iSetColor(255, 250, 220);
				drawFilledEllipse(cx, cy, curRadius * 0.40, curRadius * 0.30, 16);
			}

			iSetColor(255, 235, 75);
			for (int s = 0; s < 12; s++) {
				double ang = (double)s * (3.14159265358979323846 / 6.0) + (double)s * 0.15;
				double spDist = curRadius * 1.18;
				double spX = cx + cos(ang) * spDist;
				double spY = cy + sin(ang) * (spDist * 0.75);
				iFilledCircle(spX, spY, 2.5);
			}

			burstsL3[i].timer--;
			if (burstsL3[i].timer <= 0) {
				burstsL3[i].active = 0;
			}
		}
	}
}

// Procedural Realistic Bus (L3)
void drawRealisticBusL3(double centerX, double baseY, double scale, int colorTheme) {
	if (scale <= 0.05) return;

	double baseW = 104.0;
	double baseH = 138.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = centerX - w / 2.0;
	double by = baseY + 7.0 * scale;

	iSetColor(12, 14, 18);
	double shX[] = { centerX - w * 0.55, centerX + w * 0.55, centerX + w * 0.48, centerX - w * 0.48 };
	double shY[] = { baseY - 3.0 * scale, baseY - 3.0 * scale, baseY + 8.0 * scale, baseY + 8.0 * scale };
	iFilledPolygon(shX, shY, 4);

	iSetColor(240, 235, 175);
	double beam1X[] = { centerX - w * 0.38, centerX - w * 0.32, centerX - w * 0.70, centerX - w * 0.90 };
	double beam1Y[] = { by + 18.0 * scale, by + 18.0 * scale, baseY - 55.0 * scale, baseY - 55.0 * scale };
	iFilledPolygon(beam1X, beam1Y, 4);

	double beam2X[] = { centerX + w * 0.32, centerX + w * 0.38, centerX + w * 0.90, centerX + w * 0.70 };
	double beam2Y[] = { by + 18.0 * scale, by + 18.0 * scale, baseY - 55.0 * scale, baseY - 55.0 * scale };
	iFilledPolygon(beam2X, beam2Y, 4);

	iSetColor(15, 15, 18);
	drawRect(centerX - w * 0.45, baseY, w * 0.17, 18.0 * scale);
	iSetColor(65, 70, 75);
	drawRect(centerX - w * 0.42, baseY + 2.5 * scale, w * 0.11, 12.0 * scale);
	iSetColor(140, 145, 155);
	drawRect(centerX - w * 0.39, baseY + 5.5 * scale, w * 0.045, 6.0 * scale);

	iSetColor(15, 15, 18);
	drawRect(centerX + w * 0.28, baseY, w * 0.17, 18.0 * scale);
	iSetColor(65, 70, 75);
	drawRect(centerX + w * 0.31, baseY + 2.5 * scale, w * 0.11, 12.0 * scale);
	iSetColor(140, 145, 155);
	drawRect(centerX + w * 0.345, baseY + 5.5 * scale, w * 0.045, 6.0 * scale);

	iSetColor(26, 28, 32);
	drawRect(bx, by, w, 10.0 * scale);

	int r1 = 18, g1 = 45, b1 = 115;
	int r2 = 215, g2 = 220, b2 = 230;
	int r3 = 245, g3 = 195, b3 = 25;

	if (colorTheme == 1) {
		r1 = 120; g1 = 20;  b1 = 35;
		r2 = 235; g2 = 215; b2 = 165;
		r3 = 255; g3 = 195; b3 = 45;
	}
	else if (colorTheme == 2) {
		r1 = 15;  g1 = 95;  b1 = 48;
		r2 = 245; g2 = 248; b2 = 250;
		r3 = 245; g3 = 210; b3 = 30;
	}
	else if (colorTheme == 3) {
		r1 = 22;  g1 = 24;  b1 = 28;
		r2 = 235; g2 = 185; b2 = 35;
		r3 = 245; g3 = 215; b3 = 60;
	}
	else if (colorTheme == 4) {
		r1 = 165; g1 = 25;  b1 = 35;
		r2 = 225; g2 = 230; b2 = 238;
		r3 = 35;  g3 = 40;  b3 = 48;
	}
	else if (colorTheme == 5) {
		r1 = 25;  g1 = 42;  b1 = 95;
		r2 = 40;  g2 = 185; b2 = 235;
		r3 = 255; g3 = 215; b3 = 45;
	}
	else if (colorTheme == 6) {
		r1 = 35;  g1 = 38;  b1 = 42;
		r2 = 245; g2 = 145; b2 = 20;
		r3 = 245; g3 = 245; b3 = 245;
	}
	else if (colorTheme == 7) {
		r1 = 210; g1 = 215; b1 = 225;
		r2 = 20;  g2 = 38;  b2 = 88;
		r3 = 205; g3 = 30;  b3 = 35;
	}

	double chamfer = 10.0 * scale;
	double bodyX[] = { bx, bx + w, bx + w, bx + w - chamfer, bx + chamfer, bx };
	double bodyY[] = { by + 9.0 * scale, by + 9.0 * scale, by + h - chamfer, by + h, by + h, by + h - chamfer };
	iSetColor(r1, g1, b1);
	iFilledPolygon(bodyX, bodyY, 6);

	iSetColor((int)(r1 * 0.70), (int)(g1 * 0.70), (int)(b1 * 0.70));
	drawRect(bx, by + 9.0 * scale, 5.0 * scale, h - 18.0 * scale);
	drawRect(bx + w - 5.0 * scale, by + 9.0 * scale, 5.0 * scale, h - 18.0 * scale);

	iSetColor(r2, g2, b2);
	drawRect(bx, by + 34.0 * scale, w, 6.0 * scale);
	iSetColor(r3, g3, b3);
	drawRect(bx, by + 40.0 * scale, w, 2.5 * scale);

	double roofX[] = { bx + 4.0 * scale, bx + w - 4.0 * scale, bx + w - chamfer, bx + chamfer };
	double roofY[] = { by + h - 12.0 * scale, by + h - 12.0 * scale, by + h, by + h };
	iSetColor((int)(r1 * 0.85), (int)(g1 * 0.85), (int)(b1 * 0.85));
	iFilledPolygon(roofX, roofY, 4);

	iSetColor(12, 14, 18);
	drawRect(centerX - w * 0.38, by + h - 22.0 * scale, w * 0.76, 10.0 * scale);
	iSetColor(255, 195, 25);
	drawRect(centerX - w * 0.35, by + h - 20.0 * scale, w * 0.70, 6.0 * scale);
	iSetColor(220, 30, 30);
	drawRect(centerX - w * 0.35, by + h - 20.0 * scale, w * 0.16, 6.0 * scale);

	iSetColor(15, 18, 22);
	drawRect(centerX - w * 0.44, by + 44.0 * scale, w * 0.88, h * 0.42);
	iSetColor(22, 38, 58);
	drawRect(centerX - w * 0.41, by + 46.0 * scale, w * 0.82, h * 0.38);

	iSetColor(225, 185, 95);
	drawRect(centerX - w * 0.38, by + 62.0 * scale, w * 0.76, h * 0.18);
	iSetColor(25, 28, 35);
	iFilledCircle(centerX - w * 0.28, by + 68.0 * scale, 3.5 * scale);
	iFilledCircle(centerX - w * 0.10, by + 68.0 * scale, 3.5 * scale);
	iFilledCircle(centerX + w * 0.10, by + 68.0 * scale, 3.5 * scale);
	iFilledCircle(centerX + w * 0.28, by + 68.0 * scale, 3.5 * scale);

	iSetColor(20, 22, 28);
	iFilledCircle(centerX + w * 0.21, by + 46.0 * scale + h * 0.16, 4.8 * scale);
	drawRect(centerX + w * 0.14, by + 46.0 * scale + 2.0 * scale, 12.0 * scale, 8.0 * scale);

	iSetColor(15, 15, 15);
	iLine(centerX - w * 0.31, by + 47.0 * scale, centerX - w * 0.14, by + 58.0 * scale);
	iLine(centerX + w * 0.08, by + 47.0 * scale, centerX + w * 0.25, by + 58.0 * scale);

	iSetColor(20, 22, 25);
	drawRect(centerX - w * 0.23, by + 18.0 * scale, w * 0.46, 16.0 * scale);
	iSetColor(165, 170, 180);
	drawRect(centerX - w * 0.20, by + 21.0 * scale, w * 0.40, 1.8 * scale);
	drawRect(centerX - w * 0.20, by + 25.0 * scale, w * 0.40, 1.8 * scale);
	drawRect(centerX - w * 0.20, by + 29.0 * scale, w * 0.40, 1.8 * scale);
	iSetColor(255, 255, 255);
	iFilledCircle(centerX, by + 26.0 * scale, 2.8 * scale);

	iSetColor(255, 255, 255);
	drawRect(centerX - w * 0.45, by + 17.0 * scale, w * 0.19, 16.0 * scale);
	iSetColor(255, 255, 210);
	drawRect(centerX - w * 0.39, by + 19.0 * scale, w * 0.11, 12.0 * scale);
	iSetColor(255, 140, 0);
	drawRect(centerX - w * 0.44, by + 19.0 * scale, w * 0.045, 12.0 * scale);

	iSetColor(255, 255, 255);
	drawRect(centerX + w * 0.26, by + 17.0 * scale, w * 0.19, 16.0 * scale);
	iSetColor(255, 255, 210);
	drawRect(centerX + w * 0.28, by + 19.0 * scale, w * 0.11, 12.0 * scale);
	iSetColor(255, 140, 0);
	drawRect(centerX + w * 0.395, by + 19.0 * scale, w * 0.045, 12.0 * scale);

	iSetColor(255, 235, 120);
	drawRect(centerX - w * 0.36, by + 6.5 * scale, 6.5 * scale, 5.0 * scale);
	drawRect(centerX + w * 0.36 - 6.5 * scale, by + 6.5 * scale, 6.5 * scale, 5.0 * scale);

	iSetColor(15, 85, 40);
	drawRect(centerX - 12.0 * scale, by + 5.5 * scale, 24.0 * scale, 9.0 * scale);
	iSetColor(255, 255, 255);
	drawRect(centerX - 10.0 * scale, by + 7.0 * scale, 20.0 * scale, 6.0 * scale);

	iSetColor(20, 22, 25);
	iLine(bx + 5.0 * scale, by + h - 30.0 * scale, bx - 10.0 * scale, by + h - 40.0 * scale);
	drawRect(bx - 12.0 * scale, by + h - 50.0 * scale, 6.5 * scale, 17.0 * scale);
	iSetColor(190, 215, 235);
	drawRect(bx - 10.5 * scale, by + h - 48.0 * scale, 4.0 * scale, 13.5 * scale);

	iSetColor(20, 22, 25);
	iLine(bx + w - 5.0 * scale, by + h - 30.0 * scale, bx + w + 10.0 * scale, by + h - 40.0 * scale);
	drawRect(bx + w + 5.5 * scale, by + h - 50.0 * scale, 6.5 * scale, 17.0 * scale);
	iSetColor(190, 215, 235);
	drawRect(bx + w + 6.5 * scale, by + h - 48.0 * scale, 4.0 * scale, 13.5 * scale);

	iSetColor(255, 175, 20);
	drawRect(centerX - 14.0 * scale, by + h - 3.5 * scale, 4.5 * scale, 3.0 * scale);
	drawRect(centerX - 2.2 * scale, by + h - 3.5 * scale, 4.5 * scale, 3.0 * scale);
	drawRect(centerX + 9.5 * scale, by + h - 3.5 * scale, 4.5 * scale, 3.0 * scale);
}

// Procedural Realistic Rickshaw (L3)
void drawRealisticRickshawL3(double centerX, double baseY, double scale, float pedalAnim, int hoodColor) {
	if (scale <= 0.05) return;

	double baseW = 82.0;
	double baseH = 114.0;
	double w = baseW * scale;
	double h = baseH * scale;

	float pedal = (float)sin(pedalAnim);

	iSetColor(14, 16, 20);
	double shX[] = { centerX - w * 0.52, centerX + w * 0.52, centerX + w * 0.44, centerX - w * 0.44 };
	double shY[] = { baseY - 2.5 * scale, baseY - 2.5 * scale, baseY + 6.5 * scale, baseY + 6.5 * scale };
	iFilledPolygon(shX, shY, 4);

	iSetColor(245, 220, 140);
	double ltX[] = { centerX - 7.0 * scale, centerX + 7.0 * scale, centerX + 30.0 * scale, centerX - 30.0 * scale };
	double ltY[] = { baseY + 48.0 * scale, baseY + 48.0 * scale, baseY - 42.0 * scale, baseY - 42.0 * scale };
	iFilledPolygon(ltX, ltY, 4);

	int hoodR = 215, hoodG = 28, hoodB = 42;
	int hoodDarkR = 145, hoodDarkG = 16, hoodDarkB = 26;
	int trimR = 255, trimG = 210, trimB = 35;
	int accentR = 20, accentG = 165, accentB = 190;
	if (hoodColor == 1) {
		hoodR = 16; hoodG = 142; hoodB = 76;
		hoodDarkR = 10; hoodDarkG = 92; hoodDarkB = 48;
		trimR = 255; trimG = 200; trimB = 30;
		accentR = 225; accentG = 35; accentB = 45;
	}
	else if (hoodColor == 2) {
		hoodR = 26; hoodG = 75; hoodB = 180;
		hoodDarkR = 16; hoodDarkG = 46; hoodDarkB = 118;
		trimR = 255; trimG = 215; trimB = 35;
		accentR = 240; accentG = 45; accentB = 125;
	}
	else if (hoodColor == 3) {
		hoodR = 235; hoodG = 115; hoodB = 22;
		hoodDarkR = 158; hoodDarkG = 72; hoodDarkB = 14;
		trimR = 255; trimG = 225; trimB = 45;
		accentR = 18; accentG = 135; accentB = 68;
	}

	iSetColor(18, 18, 22);
	drawRect(centerX - w * 0.49, baseY + 3.5 * scale, 5.5 * scale, 32.0 * scale);
	iSetColor(150, 155, 165);
	drawRect(centerX - w * 0.48, baseY + 6.0 * scale, 1.8 * scale, 26.0 * scale);
	iSetColor(hoodR, hoodG, hoodB);
	drawRect(centerX - w * 0.51, baseY + 18.0 * scale, 7.8 * scale, 18.0 * scale);
	iSetColor(trimR, trimG, trimB);
	drawRect(centerX - w * 0.51, baseY + 34.0 * scale, 7.8 * scale, 2.2 * scale);
	iSetColor(245, 30, 30);
	drawFilledEllipse(centerX - w * 0.47, baseY + 23.0 * scale, 3.2 * scale, 3.2 * scale, 10);
	iSetColor(255, 255, 255);
	iFilledCircle(centerX - w * 0.47 + 0.8 * scale, baseY + 24.0 * scale, 1.0 * scale);

	iSetColor(18, 18, 22);
	drawRect(centerX + w * 0.49 - 5.5 * scale, baseY + 3.5 * scale, 5.5 * scale, 32.0 * scale);
	iSetColor(150, 155, 165);
	drawRect(centerX + w * 0.49 - 3.7 * scale, baseY + 6.0 * scale, 1.8 * scale, 26.0 * scale);
	iSetColor(hoodR, hoodG, hoodB);
	drawRect(centerX + w * 0.51 - 7.8 * scale, baseY + 18.0 * scale, 7.8 * scale, 18.0 * scale);
	iSetColor(trimR, trimG, trimB);
	drawRect(centerX + w * 0.51 - 7.8 * scale, baseY + 34.0 * scale, 7.8 * scale, 2.2 * scale);
	iSetColor(245, 30, 30);
	drawFilledEllipse(centerX + w * 0.47, baseY + 23.0 * scale, 3.2 * scale, 3.2 * scale, 10);
	iSetColor(255, 255, 255);
	iFilledCircle(centerX + w * 0.47 - 0.8 * scale, baseY + 24.0 * scale, 1.0 * scale);

	double plateW = w * 0.68;
	double plateH = 22.0 * scale;
	double plateY = baseY + 32.0 * scale;

	iSetColor(25, 26, 30);
	drawRect(centerX - plateW * 0.5 - 1.5 * scale, plateY - 1.5 * scale, plateW + 3.0 * scale, plateH + 3.0 * scale);
	iSetColor(trimR, trimG, trimB);
	drawRect(centerX - plateW * 0.5, plateY, plateW, plateH);
	iSetColor(accentR, accentG, accentB);
	drawRect(centerX - plateW * 0.5 + 2.0 * scale, plateY + 2.0 * scale, plateW - 4.0 * scale, plateH - 4.0 * scale);

	iSetColor(22, 160, 65);
	drawFilledEllipse(centerX - 10.0 * scale, plateY + 11.0 * scale, 6.0 * scale, 3.2 * scale, 10);
	drawFilledEllipse(centerX + 10.0 * scale, plateY + 11.0 * scale, 6.0 * scale, 3.2 * scale, 10);

	iSetColor(245, 45, 75);
	drawFilledEllipse(centerX, plateY + 11.0 * scale, 7.5 * scale, 5.5 * scale, 12);
	iSetColor(255, 140, 175);
	drawFilledEllipse(centerX, plateY + 11.0 * scale, 5.0 * scale, 3.5 * scale, 10);

	iSetColor(255, 235, 45);
	iFilledCircle(centerX, plateY + 11.0 * scale, 2.2 * scale);

	iSetColor(trimR, trimG, trimB);
	iFilledCircle(centerX - plateW * 0.5 + 4.5 * scale, plateY + plateH - 4.5 * scale, 1.8 * scale);
	iFilledCircle(centerX + plateW * 0.5 - 4.5 * scale, plateY + plateH - 4.5 * scale, 1.8 * scale);

	iSetColor(245, 205, 35);
	for (int b = -2; b <= 2; b++) {
		double bellX = centerX + (double)b * (plateW * 0.20);
		double bellY = plateY - 3.5 * scale;
		drawRect(bellX - 0.7 * scale, bellY + 1.5 * scale, 1.4 * scale, 3.0 * scale);
		iFilledCircle(bellX, bellY, 1.8 * scale);
	}

	iSetColor(32, 34, 40);
	drawRect(centerX - w * 0.35, baseY + 54.0 * scale, w * 0.70, 24.0 * scale);
	iSetColor(48, 52, 62);
	drawRect(centerX - w * 0.32, baseY + 56.0 * scale, w * 0.64, 20.0 * scale);

	iSetColor(75, 80, 92);
	drawRect(centerX - w * 0.28, baseY + 65.0 * scale, w * 0.56, 1.5 * scale);
	drawRect(centerX - 10.0 * scale, baseY + 57.0 * scale, 1.5 * scale, 18.0 * scale);
	drawRect(centerX + 10.0 * scale, baseY + 57.0 * scale, 1.5 * scale, 18.0 * scale);

	iSetColor(235, 195, 45);
	drawRect(centerX - w * 0.36, baseY + 54.0 * scale, 2.5 * scale, 24.0 * scale);
	drawRect(centerX + w * 0.36 - 2.5 * scale, baseY + 54.0 * scale, 2.5 * scale, 24.0 * scale);

	double hoodX[] = {
		centerX - w * 0.44,
		centerX + w * 0.44,
		centerX + w * 0.40,
		centerX + w * 0.24,
		centerX - w * 0.24,
		centerX - w * 0.40
	};
	double hoodY[] = {
		baseY + 54.0 * scale,
		baseY + 54.0 * scale,
		baseY + h - 6.0 * scale,
		baseY + h,
		baseY + h,
		baseY + h - 6.0 * scale
	};

	iSetColor(hoodR, hoodG, hoodB);
	iFilledPolygon(hoodX, hoodY, 6);

	iSetColor(hoodDarkR, hoodDarkG, hoodDarkB);
	double rib1X[] = { centerX - w * 0.43, centerX - w * 0.22, centerX - w * 0.18, centerX - w * 0.38 };
	double rib1Y[] = { baseY + 55.0 * scale, baseY + 55.0 * scale, baseY + h - 5.0 * scale, baseY + h - 5.0 * scale };
	iFilledPolygon(rib1X, rib1Y, 4);

	double rib2X[] = { centerX + w * 0.22, centerX + w * 0.43, centerX + w * 0.38, centerX + w * 0.18 };
	double rib2Y[] = { baseY + 55.0 * scale, baseY + 55.0 * scale, baseY + h - 5.0 * scale, baseY + h - 5.0 * scale };
	iFilledPolygon(rib2X, rib2Y, 4);

	iSetColor(trimR, trimG, trimB);
	drawRect(centerX - w * 0.40, baseY + 76.0 * scale, w * 0.80, 5.0 * scale);
	iSetColor(accentR, accentG, accentB);
	drawRect(centerX - w * 0.38, baseY + 78.0 * scale, w * 0.76, 2.2 * scale);

	iSetColor(240, 245, 255);
	drawRect(centerX - w * 0.20, baseY + h - 4.5 * scale, w * 0.40, 2.0 * scale);

	iSetColor(220, 225, 235);
	drawRect(centerX - w * 0.41, baseY + 60.0 * scale, 2.0 * scale, 18.0 * scale);
	drawRect(centerX + w * 0.39, baseY + 60.0 * scale, 2.0 * scale, 18.0 * scale);
	iSetColor(255, 255, 255);
	iFilledCircle(centerX - w * 0.40, baseY + 68.0 * scale, 1.8 * scale);
	iFilledCircle(centerX + w * 0.40, baseY + 68.0 * scale, 1.8 * scale);

	iSetColor(230, 232, 238);
	double torsoX[] = { centerX - 8.5 * scale, centerX + 8.5 * scale, centerX + 11.0 * scale, centerX - 11.0 * scale };
	double torsoY[] = { baseY + 46.0 * scale, baseY + 46.0 * scale, baseY + 70.0 * scale, baseY + 70.0 * scale };
	iFilledPolygon(torsoX, torsoY, 4);

	iSetColor(225, 42, 45);
	drawRect(centerX - 8.0 * scale, baseY + 65.0 * scale, 16.0 * scale, 4.5 * scale);
	iSetColor(255, 240, 60);
	drawRect(centerX - 8.0 * scale, baseY + 66.5 * scale, 16.0 * scale, 1.5 * scale);

	iSetColor(230, 232, 238);
	drawRect(centerX - 13.0 * scale, baseY + 56.0 * scale, 4.5 * scale, 10.0 * scale);
	drawRect(centerX + 8.5 * scale, baseY + 56.0 * scale, 4.5 * scale, 10.0 * scale);

	iSetColor(215, 165, 130);
	drawRect(centerX - 13.0 * scale, baseY + 53.0 * scale, 4.0 * scale, 5.0 * scale);
	drawRect(centerX + 9.0 * scale, baseY + 53.0 * scale, 4.0 * scale, 5.0 * scale);

	iSetColor(190, 195, 205);
	drawRect(centerX - 16.5 * scale, baseY + 53.5 * scale, 33.0 * scale, 2.5 * scale);
	iSetColor(20, 20, 22);
	drawRect(centerX - 18.0 * scale, baseY + 52.5 * scale, 3.2 * scale, 4.5 * scale);
	drawRect(centerX + 14.8 * scale, baseY + 52.5 * scale, 3.2 * scale, 4.5 * scale);

	iSetColor(245, 205, 40);
	iFilledCircle(centerX + 11.5 * scale, baseY + 57.0 * scale, 2.5 * scale);

	iSetColor(255, 235, 120);
	iFilledCircle(centerX, baseY + 55.5 * scale, 3.8 * scale);
	iSetColor(255, 255, 255);
	iFilledCircle(centerX, baseY + 55.5 * scale, 1.8 * scale);

	iSetColor(215, 165, 130);
	drawRect(centerX - 3.0 * scale, baseY + 69.0 * scale, 6.0 * scale, 4.5 * scale);
	iFilledCircle(centerX, baseY + 78.0 * scale, 6.2 * scale);
	iSetColor(28, 24, 24);
	iFilledCircle(centerX, baseY + 81.0 * scale, 5.5 * scale);

	float pedalL = pedal * 4.5f * (float)scale;
	float pedalR = -pedal * 4.5f * (float)scale;

	iSetColor(42, 85, 145);
	drawRect(centerX - 7.5 * scale, baseY + 28.0 * scale + pedalL * 0.4, 4.5 * scale, 16.0 * scale);
	drawRect(centerX + 3.0 * scale, baseY + 28.0 * scale + pedalR * 0.4, 4.5 * scale, 16.0 * scale);

	iSetColor(22, 22, 25);
	drawRect(centerX - 8.5 * scale, baseY + 24.5 * scale + pedalL * 0.4, 6.0 * scale, 3.5 * scale);
	drawRect(centerX + 2.5 * scale, baseY + 24.5 * scale + pedalR * 0.4, 6.0 * scale, 3.5 * scale);

	iSetColor(18, 18, 22);
	drawRect(centerX - 2.2 * scale, baseY, 4.4 * scale, 24.0 * scale);
	iSetColor(170, 175, 185);
	drawRect(centerX - 2.8 * scale, baseY + 12.0 * scale, 5.6 * scale, 13.0 * scale);
}

// Procedural Realistic Person (L3)
void drawRealisticPersonL3(double centerX, double baseY, double scale, float walkPhase, int outfitTheme, int umbrellaColor, int gender) {
	if (scale <= 0.05) return;

	float swing = (float)sin(walkPhase);

	if (gender == 1) {
		double hipY = baseY + 42.0 * scale;
		float legSwing = swing * 4.5f * (float)scale;
		float armSwing = -swing * 4.5f * (float)scale;

		iSetColor(18, 20, 25);
		drawFilledEllipse(centerX, baseY - 1.0 * scale, 13.0 * scale, 3.8 * scale, 12);

		int sareeR = 198, sareeG = 32, sareeB = 58;
		int borderR = 245, borderG = 210, borderB = 60;
		if (outfitTheme == 1) {
			sareeR = 20;  sareeG = 135; sareeB = 145;
			borderR = 225; borderG = 235; borderB = 245;
		}
		else if (outfitTheme == 2) {
			sareeR = 225; sareeG = 125; sareeB = 25;
			borderR = 145; borderG = 25;  borderB = 35;
		}
		else if (outfitTheme == 3) {
			sareeR = 175; sareeG = 30;  sareeB = 105;
			borderR = 255; borderG = 215; borderB = 50;
		}

		iSetColor(sareeR, sareeG, sareeB);
		double skirtX[] = {
			centerX - 8.0 * scale,
			centerX + 8.0 * scale,
			centerX + 13.0 * scale + legSwing * 0.4,
			centerX - 13.0 * scale + legSwing * 0.4
		};
		double skirtY[] = {
			hipY + 2.0 * scale,
			hipY + 2.0 * scale,
			baseY + 4.0 * scale,
			baseY + 4.0 * scale
		};
		iFilledPolygon(skirtX, skirtY, 4);

		iSetColor(borderR, borderG, borderB);
		drawRect(centerX - 13.0 * scale + legSwing * 0.4, baseY + 4.0 * scale, 26.0 * scale, 3.5 * scale);

		iSetColor(40, 25, 20);
		drawRect(centerX - 6.0 * scale + legSwing * 0.3, baseY, 5.0 * scale, 4.0 * scale);
		drawRect(centerX + 1.5 * scale - legSwing * 0.3, baseY, 5.0 * scale, 4.0 * scale);

		iSetColor((int)(sareeR * 0.85), (int)(sareeG * 0.85), (int)(sareeB * 0.85));
		double torsoX[] = { centerX - 7.5 * scale, centerX + 7.5 * scale, centerX + 9.5 * scale, centerX - 9.5 * scale };
		double torsoY[] = { hipY + 1.0 * scale, hipY + 1.0 * scale, hipY + 28.0 * scale, hipY + 28.0 * scale };
		iFilledPolygon(torsoX, torsoY, 4);

		iSetColor(borderR, borderG, borderB);
		double palluX[] = { centerX - 9.0 * scale, centerX - 5.0 * scale, centerX + 8.5 * scale, centerX + 5.0 * scale };
		double palluY[] = { hipY + 2.0 * scale, hipY + 2.0 * scale, hipY + 28.0 * scale, hipY + 28.0 * scale };
		iFilledPolygon(palluX, palluY, 4);

		iSetColor(220, 170, 135);
		drawRect(centerX - 11.5 * scale, hipY + 10.0 * scale + armSwing * 0.2, 3.0 * scale, 12.0 * scale);
		drawRect(centerX + 8.5 * scale, hipY + 18.0 * scale, 3.0 * scale, 10.0 * scale);
		iSetColor(245, 215, 60);
		drawRect(centerX - 11.8 * scale, hipY + 12.0 * scale + armSwing * 0.2, 3.6 * scale, 2.5 * scale);
		drawRect(centerX + 8.2 * scale, hipY + 20.0 * scale, 3.6 * scale, 2.5 * scale);

		iSetColor(220, 170, 135);
		drawRect(centerX - 2.5 * scale, hipY + 27.0 * scale, 5.0 * scale, 5.0 * scale);
		iFilledCircle(centerX, hipY + 36.0 * scale, 5.5 * scale);

		iSetColor(22, 18, 18);
		iFilledCircle(centerX, hipY + 39.5 * scale, 4.8 * scale);
		iFilledCircle(centerX - 5.2 * scale, hipY + 37.0 * scale, 4.2 * scale);
		iSetColor(255, 250, 230);
		iFilledCircle(centerX - 5.2 * scale, hipY + 39.5 * scale, 2.0 * scale);
		iFilledCircle(centerX - 7.0 * scale, hipY + 37.0 * scale, 1.8 * scale);

		int umbR = 220, umbG = 45, umbB = 85;
		if (umbrellaColor == 1) { umbR = 35; umbG = 125; umbB = 215; }
		else if (umbrellaColor == 2) { umbR = 245; umbG = 180; umbB = 30; }
		else if (umbrellaColor == 3) { umbR = 28; umbG = 145; umbB = 75; }

		double shaftX = centerX + 10.0 * scale;
		double umbCenterY = hipY + 46.0 * scale;

		iSetColor(40, 42, 48);
		drawRect(shaftX - 1.2 * scale, hipY + 20.0 * scale, 2.4 * scale, 28.0 * scale);

		double umbW = 27.0 * scale;
		double umbH = 14.0 * scale;
		double ucx = centerX + 4.0 * scale;

		iSetColor(umbR, umbG, umbB);
		drawFilledEllipse(ucx, umbCenterY + 2.0 * scale, umbW, umbH, 16);
		iSetColor(18, 24, 35);
		drawRect(ucx - umbW, umbCenterY - umbH * 0.9, umbW * 2.0, umbH * 0.9);
	}
	else {
		double hipY = baseY + 45.0 * scale;
		float legSwingL = swing * 6.0f * (float)scale;
		float legSwingR = -swing * 6.0f * (float)scale;
		float armSwingL = -swing * 5.5f * (float)scale;

		iSetColor(18, 20, 25);
		double shX[] = { centerX - 10.0 * scale, centerX + 10.0 * scale, centerX + 8.0 * scale, centerX - 8.0 * scale };
		double shY[] = { baseY - 2.0 * scale, baseY - 2.0 * scale, baseY + 4.0 * scale, baseY + 4.0 * scale };
		iFilledPolygon(shX, shY, 4);

		int coatR = 215, coatG = 165, coatB = 25;
		int pantsR = 35, pantsG = 42, pantsB = 55;
		int shoeR = 25, shoeG = 25, shoeB = 28;

		if (outfitTheme == 1) {
			coatR = 25; coatG = 58;  coatB = 115;
			pantsR = 45;  pantsG = 48;  pantsB = 52;
		}
		else if (outfitTheme == 2) {
			coatR = 65; coatG = 75;  coatB = 88;
			pantsR = 30;  pantsG = 32;  pantsB = 38;
		}
		else if (outfitTheme == 3) {
			coatR = 28; coatG = 85;  coatB = 48;
			pantsR = 40;  pantsG = 50;  pantsB = 68;
		}

		iSetColor(pantsR, pantsG, pantsB);
		double tLeftX[] = { centerX - 9.0 * scale, centerX - 2.0 * scale, centerX - 3.0 * scale + legSwingL * 0.4, centerX - 9.0 * scale + legSwingL * 0.4 };
		double tLeftY[] = { hipY, hipY, baseY + 5.0 * scale, baseY + 5.0 * scale };
		iFilledPolygon(tLeftX, tLeftY, 4);

		iSetColor(shoeR, shoeG, shoeB);
		drawRect(centerX - 9.5 * scale + legSwingL * 0.4, baseY, 7.5 * scale, 5.0 * scale);

		iSetColor(pantsR, pantsG, pantsB);
		double tRightX[] = { centerX + 2.0 * scale, centerX + 9.0 * scale, centerX + 9.0 * scale + legSwingR * 0.4, centerX + 3.0 * scale + legSwingR * 0.4 };
		double tRightY[] = { hipY, hipY, baseY + 5.0 * scale, baseY + 5.0 * scale };
		iFilledPolygon(tRightX, tRightY, 4);

		iSetColor(shoeR, shoeG, shoeB);
		drawRect(centerX + 2.5 * scale + legSwingR * 0.4, baseY, 7.5 * scale, 5.0 * scale);

		iSetColor(coatR, coatG, coatB);
		double torsoX[] = { centerX - 8.5 * scale, centerX + 8.5 * scale, centerX + 11.5 * scale, centerX - 11.5 * scale };
		double torsoY[] = { hipY + 1.0 * scale, hipY + 1.0 * scale, hipY + 31.0 * scale, hipY + 31.0 * scale };
		iFilledPolygon(torsoX, torsoY, 4);

		iSetColor(coatR, coatG, coatB);
		drawRect(centerX - 13.5 * scale, hipY + 19.0 * scale, 3.5 * scale, 12.0 * scale);
		iSetColor(215, 165, 130);
		drawRect(centerX - 13.5 * scale + armSwingL * 0.3, hipY + 7.0 * scale, 3.2 * scale, 12.0 * scale);

		iSetColor(coatR, coatG, coatB);
		drawRect(centerX + 10.0 * scale, hipY + 18.0 * scale, 3.5 * scale, 12.0 * scale);
		iSetColor(215, 165, 130);
		drawRect(centerX + 10.5 * scale, hipY + 24.0 * scale, 3.5 * scale, 8.0 * scale);

		iSetColor(215, 165, 130);
		drawRect(centerX - 3.0 * scale, hipY + 30.0 * scale, 6.0 * scale, 5.0 * scale);
		iFilledCircle(centerX, hipY + 39.0 * scale, 6.2 * scale);
		iSetColor(25, 22, 22);
		iFilledCircle(centerX, hipY + 43.0 * scale, 5.2 * scale);

		int umbR = 195, umbG = 30, umbB = 40;
		if (umbrellaColor == 1) { umbR = 25; umbG = 65; umbB = 165; }
		else if (umbrellaColor == 2) { umbR = 235; umbG = 175; umbB = 25; }
		else if (umbrellaColor == 3) { umbR = 22; umbG = 115; umbB = 52; }

		double shaftX = centerX + 12.0 * scale;
		double umbCenterY = hipY + 49.0 * scale;

		iSetColor(40, 42, 48);
		drawRect(shaftX - 1.2 * scale, hipY + 22.0 * scale, 2.4 * scale, 30.0 * scale);

		double umbW = 28.0 * scale;
		double umbH = 14.0 * scale;
		double ucx = centerX + 6.0 * scale;

		iSetColor(umbR, umbG, umbB);
		drawFilledEllipse(ucx, umbCenterY + 2.0 * scale, umbW, umbH, 16);
		iSetColor(18, 24, 35);
		drawRect(ucx - umbW, umbCenterY - umbH * 0.9, umbW * 2.0, umbH * 0.9);
	}
}

// Procedural Realistic Street Dog (L3)
void drawRealisticDogL3(double centerX, double baseY, double scale, float trotAnim, int coatColor) {
	if (scale <= 0.05) return;

	double baseW = 24.0;
	double w = baseW * scale;
	float trot = (float)sin(trotAnim);

	iSetColor(16, 18, 22);
	double shX[] = { centerX - w * 0.65, centerX + w * 0.65, centerX + w * 0.50, centerX - w * 0.50 };
	double shY[] = { baseY - 2.0 * scale, baseY - 2.0 * scale, baseY + 3.5 * scale, baseY + 3.5 * scale };
	iFilledPolygon(shX, shY, 4);

	int coatR = 175, coatG = 115, coatB = 55;
	int darkR = 110, darkG = 70, darkB = 30;

	if (coatColor == 1) {
		coatR = 155; coatG = 95; coatB = 45;
		darkR = 24;  darkG = 22;  darkB = 20;
	}
	else if (coatColor == 2) {
		coatR = 115; coatG = 60;  coatB = 35;
		darkR = 75;  darkG = 35;  darkB = 18;
	}
	else if (coatColor == 3) {
		coatR = 145; coatG = 148; coatB = 155;
		darkR = 85;  darkG = 88;  darkB = 95;
	}

	iSetColor(coatR, coatG, coatB);
	double tailSway = trot * 5.0 * scale;
	double tailX[] = { centerX - 2.0 * scale, centerX + tailSway, centerX + tailSway + 3.5 * scale, centerX + 2.0 * scale };
	double tailY[] = { baseY + 15.0 * scale, baseY + 28.0 * scale, baseY + 29.5 * scale, baseY + 16.5 * scale };
	iFilledPolygon(tailX, tailY, 4);

	iSetColor((int)(coatR * 0.75), (int)(coatG * 0.75), (int)(coatB * 0.75));
	double lhX[] = { centerX - w * 0.46, centerX - w * 0.28, centerX - w * 0.32, centerX - w * 0.44 };
	double lhY[] = { baseY + 14.0 * scale, baseY + 14.0 * scale, baseY + 3.0 * scale, baseY + 3.0 * scale };
	iFilledPolygon(lhX, lhY, 4);

	double rhX[] = { centerX + w * 0.28, centerX + w * 0.46, centerX + w * 0.44, centerX + w * 0.32 };
	double rhY[] = { baseY + 14.0 * scale, baseY + 14.0 * scale, baseY + 3.0 * scale, baseY + 3.0 * scale };
	iFilledPolygon(rhX, rhY, 4);

	iSetColor(coatR, coatG, coatB);
	double chestX[] = { centerX - w * 0.32, centerX + w * 0.32, centerX + w * 0.24, centerX - w * 0.24 };
	double chestY[] = { baseY + 8.0 * scale, baseY + 8.0 * scale, baseY + 19.0 * scale, baseY + 19.0 * scale };
	iFilledPolygon(chestX, chestY, 4);

	float lPawLift = (trot > 0) ? trot * 3.0f * (float)scale : 0.0f;
	float rPawLift = (-trot > 0) ? -trot * 3.0f * (float)scale : 0.0f;

	iSetColor(coatR, coatG, coatB);
	double lLegX[] = { centerX - w * 0.26, centerX - w * 0.14, centerX - w * 0.16, centerX - w * 0.24 };
	double lLegY[] = { baseY + 14.0 * scale, baseY + 14.0 * scale, baseY + lPawLift + 2.5 * scale, baseY + lPawLift + 2.5 * scale };
	iFilledPolygon(lLegX, lLegY, 4);
	drawRect(centerX - w * 0.26, baseY + lPawLift, 4.0 * scale, 3.0 * scale);

	double rLegX[] = { centerX + w * 0.14, centerX + w * 0.26, centerX + w * 0.24, centerX + w * 0.16 };
	double rLegY[] = { baseY + 14.0 * scale, baseY + 14.0 * scale, baseY + rPawLift + 2.5 * scale, baseY + rPawLift + 2.5 * scale };
	iFilledPolygon(rLegX, rLegY, 4);
	drawRect(centerX + w * 0.16, baseY + rPawLift, 4.0 * scale, 3.0 * scale);

	iSetColor(225, 45, 45);
	drawRect(centerX - w * 0.22, baseY + 17.5 * scale, w * 0.44, 3.0 * scale);
	iSetColor(255, 235, 60);
	iFilledCircle(centerX, baseY + 17.0 * scale, 1.8 * scale);

	if (coatColor == 1) iSetColor(darkR, darkG, darkB);
	else iSetColor(coatR, coatG, coatB);
	iFilledCircle(centerX, baseY + 24.5 * scale, 7.5 * scale);

	double lEarX[] = { centerX - w * 0.38, centerX - w * 0.18, centerX - w * 0.32 };
	double lEarY[] = { baseY + 24.0 * scale, baseY + 27.0 * scale, baseY + 34.5 * scale };
	iFilledPolygon(lEarX, lEarY, 3);

	double rEarX[] = { centerX + w * 0.18, centerX + w * 0.38, centerX + w * 0.32 };
	double rEarY[] = { baseY + 27.0 * scale, baseY + 24.0 * scale, baseY + 34.5 * scale };
	iFilledPolygon(rEarX, rEarY, 3);

	iSetColor(15, 12, 10);
	drawRect(centerX - 5.5 * scale, baseY + 24.2 * scale, 2.8 * scale, 2.5 * scale);
	drawRect(centerX + 2.7 * scale, baseY + 24.2 * scale, 2.8 * scale, 2.5 * scale);

	iSetColor(28, 20, 16);
	iFilledCircle(centerX, baseY + 22.4 * scale, 1.7 * scale);
}

// Procedural Realistic Coins (L3)
void drawCoinL3(double centerX, double baseY, double scale, int isSpecial, float spinAnim) {
	if (scale <= 0.05) return;

	if (!isSpecial) {
		double radius = 14.5 * scale;
		double thick = 3.2 * scale;
		float wf = 0.22f + 0.78f * (float)fabs(cos(spinAnim));
		double rx = radius * wf;
		double ry = radius;
		double cy = baseY + radius * 0.90 + 4.0 * scale;

		iSetColor(22, 24, 28);
		drawFilledEllipse(centerX, baseY - 1.0 * scale, radius * 0.90 * wf, radius * 0.30, 12);

		iSetColor(140, 90, 10);
		drawFilledEllipse(centerX, cy - thick, rx, ry, 16);
		drawRect(centerX - rx, cy - thick, rx * 2.0, thick);
		iSetColor(195, 140, 20);
		drawRect(centerX - rx * 0.65, cy - thick, rx * 1.3, thick);

		iSetColor(215, 160, 20);
		drawFilledEllipse(centerX, cy, rx, ry, 16);

		iSetColor(255, 235, 130);
		drawFilledEllipse(centerX, cy + 1.0 * scale, rx * 0.95, ry * 0.95, 16);

		iSetColor(165, 110, 12);
		drawFilledEllipse(centerX, cy, rx * 0.88, ry * 0.88, 16);

		iSetColor(245, 195, 30);
		drawFilledEllipse(centerX, cy, rx * 0.80, ry * 0.80, 16);

		iSetColor(255, 222, 68);
		drawFilledEllipse(centerX, cy, rx * 0.60, ry * 0.60, 16);

		double starW = 6.0 * scale * wf;
		double starH = 6.0 * scale;
		iSetColor(150, 95, 10);
		drawRect(centerX - starW * 0.5 + 0.8 * scale, cy - starH * 0.5 - 0.8 * scale, starW, starH);
		iSetColor(255, 255, 220);
		drawRect(centerX - starW * 0.5, cy - starH * 0.5, starW, starH);
		iSetColor(255, 255, 255);
		iFilledCircle(centerX, cy, 1.5 * scale);

		iSetColor(255, 255, 255);
		drawFilledEllipse(centerX - rx * 0.35, cy + ry * 0.35, 2.5 * scale * wf, 2.5 * scale, 10);
	}
	else {
		double radius = 21.0 * scale;
		double thick = 4.5 * scale;
		float wf = 0.25f + 0.75f * (float)fabs(cos(spinAnim));
		double rx = radius * wf;
		double ry = radius;
		double cy = baseY + radius * 0.95 + 5.0 * scale;

		float pulse = (float)fabs(sin(spinAnim * 1.8f));
		iSetColor(245, 50, 165);
		drawFilledEllipse(centerX, cy, (radius + 4.0 + pulse * 3.0) * wf * scale, (radius + 4.0 + pulse * 3.0) * scale, 16);

		iSetColor(20, 22, 26);
		drawFilledEllipse(centerX, baseY - 1.2 * scale, radius * 0.95 * wf, radius * 0.32, 12);

		iSetColor(90, 15, 80);
		drawFilledEllipse(centerX, cy - thick, rx, ry, 16);
		drawRect(centerX - rx, cy - thick, rx * 2.0, thick);
		iSetColor(150, 25, 130);
		drawRect(centerX - rx * 0.65, cy - thick, rx * 1.3, thick);

		iSetColor(125, 18, 110);
		drawFilledEllipse(centerX, cy, rx, ry, 16);

		iSetColor(255, 205, 25);
		drawFilledEllipse(centerX, cy, rx * 0.92, ry * 0.92, 16);

		iSetColor(180, 130, 15);
		drawFilledEllipse(centerX, cy, rx * 0.82, ry * 0.82, 16);

		iSetColor(215, 25, 75);
		drawFilledEllipse(centerX, cy, rx * 0.74, ry * 0.74, 16);

		iSetColor(30, 220, 255);
		drawFilledEllipse(centerX, cy, rx * 0.45, ry * 0.45, 16);

		double starSize = 7.5 * scale;
		double starW = starSize * wf;
		iSetColor(255, 255, 255);
		double sX[] = { centerX, centerX + starW * 0.5, centerX, centerX - starW * 0.5 };
		double sY[] = { cy + starSize * 0.5, cy, cy - starSize * 0.5, cy };
		iFilledPolygon(sX, sY, 4);

		iSetColor(255, 255, 255);
		iFilledCircle(centerX, cy, 2.0 * scale);
	}
}

// Storm Clouds (L3)
void drawHeavyStormCloudL3(double cx, double cy, double scale, int isFlashing) {
	if (scale <= 0.1) return;

	int c1R = 18, c1G = 22, c1B = 32;
	int c2R = 28, c2G = 34, c2B = 48;
	int c3R = 42, c3G = 50, c3B = 68;

	if (isFlashing) {
		c1R = 95;  c1G = 120; c1B = 160;
		c2R = 145; c2G = 175; c2B = 220;
		c3R = 195; c3G = 220; c3B = 255;
	}

	iSetColor(c1R, c1G, c1B);
	iFilledCircle(cx - 55.0 * scale, cy - 2.0 * scale, 28.0 * scale);
	iFilledCircle(cx - 24.0 * scale, cy - 4.0 * scale, 34.0 * scale);
	iFilledCircle(cx + 20.0 * scale, cy - 3.0 * scale, 32.0 * scale);
	iFilledCircle(cx + 56.0 * scale, cy - 2.0 * scale, 26.0 * scale);
	drawRect(cx - 55.0 * scale, cy - 16.0 * scale, 111.0 * scale, 18.0 * scale);

	iSetColor(c2R, c2G, c2B);
	iFilledCircle(cx - 50.0 * scale, cy + 4.0 * scale, 25.0 * scale);
	iFilledCircle(cx - 22.0 * scale, cy + 6.0 * scale, 31.0 * scale);
	iFilledCircle(cx + 18.0 * scale, cy + 7.0 * scale, 29.0 * scale);
	iFilledCircle(cx + 50.0 * scale, cy + 5.0 * scale, 24.0 * scale);
	drawRect(cx - 50.0 * scale, cy - 8.0 * scale, 100.0 * scale, 16.0 * scale);

	iSetColor(c3R, c3G, c3B);
	iFilledCircle(cx - 30.0 * scale, cy + 22.0 * scale, 24.0 * scale);
	iFilledCircle(cx + 2.0 * scale, cy + 28.0 * scale, 30.0 * scale);
	iFilledCircle(cx + 34.0 * scale, cy + 20.0 * scale, 24.0 * scale);
}

// Lightning Bolt (L3)
void drawLightningBoltL3(float rootX, float rootY) {
	float x = rootX;
	float y = rootY;
	float stepY = 28.0f;

	iSetColor(160, 215, 255);

	while (y > horizonYL3 + 20.0f) {
		float nextX = x + ((rand() % 35) - 17.0f);
		float nextY = y - stepY;

		iLine(x - 1.5, y, nextX - 1.5, nextY);
		iLine(x + 1.5, y, nextX + 1.5, nextY);

		if ((rand() % 100) < 35 && y > horizonYL3 + 80.0f) {
			float forkX = nextX;
			float forkY = nextY;
			for (int f = 0; f < 3; f++) {
				float fNextX = forkX + ((rand() % 30) - 10.0f);
				float fNextY = forkY - 20.0f;
				iLine(forkX, forkY, fNextX, fNextY);
				forkX = fNextX;
				forkY = fNextY;
			}
		}

		x = nextX;
		y = nextY;
	}

	srand(12345);
	x = rootX;
	y = rootY;
	iSetColor(255, 255, 255);
	while (y > horizonYL3 + 20.0f) {
		float nextX = x + ((rand() % 35) - 17.0f);
		float nextY = y - stepY;
		iLine(x, y, nextX, nextY);
		x = nextX;
		y = nextY;
	}
}


// Procedural Realistic Props & Scenery (L3)
void drawRealisticTreeL3(double baseX, double baseY, double scale, int variant) {
	if (scale <= 0.05) return;

	double trunkW = 14.0 * scale;
	double trunkH = 55.0 * scale;

	iSetColor(15, 18, 22);
	double shX[] = { baseX - 22.0 * scale, baseX + 22.0 * scale, baseX + 16.0 * scale, baseX - 16.0 * scale };
	double shY[] = { baseY - 2.0 * scale, baseY - 2.0 * scale, baseY + 4.0 * scale, baseY + 4.0 * scale };
	iFilledPolygon(shX, shY, 4);

	iSetColor(42, 28, 18);
	double trX[] = { baseX - trunkW * 0.9, baseX + trunkW * 0.9, baseX + trunkW * 0.45, baseX - trunkW * 0.45 };
	double trY[] = { baseY, baseY, baseY + trunkH, baseY + trunkH };
	iFilledPolygon(trX, trY, 4);

	int dR = 12, dG = 38, dB = 20;
	int mR = 18, mG = 65, mB = 30;
	int lR = 32, lG = 95, lB = 42;

	double canCenterY = baseY + trunkH + 28.0 * scale;

	iSetColor(dR, dG, dB);
	iFilledCircle(baseX - 24.0 * scale, canCenterY - 10.0 * scale, 25.0 * scale);
	iFilledCircle(baseX + 24.0 * scale, canCenterY - 10.0 * scale, 25.0 * scale);
	iFilledCircle(baseX, canCenterY - 5.0 * scale, 30.0 * scale);

	iSetColor(mR, mG, mB);
	iFilledCircle(baseX - 26.0 * scale, canCenterY + 5.0 * scale, 24.0 * scale);
	iFilledCircle(baseX + 26.0 * scale, canCenterY + 5.0 * scale, 24.0 * scale);
	iFilledCircle(baseX, canCenterY + 15.0 * scale, 28.0 * scale);

	iSetColor(lR, lG, lB);
	iFilledCircle(baseX - 14.0 * scale, canCenterY + 24.0 * scale, 16.0 * scale);
	iFilledCircle(baseX + 10.0 * scale, canCenterY + 26.0 * scale, 17.0 * scale);
}

void drawRealisticPalmTreeL3(double baseX, double baseY, double scale, int variant) {
	if (scale <= 0.05) return;

	double trunkH = 95.0 * scale;
	iSetColor(15, 18, 22);
	iFilledCircle(baseX, baseY, 12.0 * scale);

	double curveLean = (variant % 2 == 0) ? -12.0 * scale : 12.0 * scale;
	double topX = baseX + curveLean;
	double topY = baseY + trunkH;

	int segments = 8;
	for (int s = 0; s < segments; s++) {
		double f1 = (double)s / segments;
		double f2 = (double)(s + 1) / segments;
		double sx1 = baseX + curveLean * (f1 * f1);
		double sy1 = baseY + trunkH * f1;
		double sx2 = baseX + curveLean * (f2 * f2);
		double sy2 = baseY + trunkH * f2;
		double w = (8.0 - 3.5 * f1) * scale;

		if (s % 2 == 0) iSetColor(65, 50, 35);
		else iSetColor(48, 38, 25);

		double segX[] = { sx1 - w * 0.5, sx1 + w * 0.5, sx2 + w * 0.4, sx2 - w * 0.4 };
		double segY[] = { sy1, sy1, sy2, sy2 };
		iFilledPolygon(segX, segY, 4);
	}

	double frondAngles[] = { -2.8, -2.2, -1.6, -1.0, -0.4, 0.4, 1.0, 1.6, 2.2, 2.8 };
	double frondLen = 42.0 * scale;

	for (int f = 0; f < 10; f++) {
		double ang = frondAngles[f];
		double endX = topX + cos(ang) * frondLen;
		double endY = topY + sin(ang) * frondLen * 0.65 - 8.0 * scale;
		double midX = (topX + endX) * 0.5;
		double midY = (topY + endY) * 0.5 + 8.0 * scale;

		iSetColor(15, 55, 24);
		double fPolyX[] = { topX, midX - 4.0 * scale, endX, midX + 4.0 * scale };
		double fPolyY[] = { topY, midY, endY, midY };
		iFilledPolygon(fPolyX, fPolyY, 4);
		iSetColor(35, 95, 35);
		iLine(topX, topY, endX, endY);
	}
}

void drawElectricalPoleL3(double baseX, double baseY, double scale, int side) {
	if (scale <= 0.05) return;

	double poleH = 135.0 * scale;
	double baseW = 7.5 * scale;
	double topW = 4.5 * scale;

	iSetColor(15, 18, 22);
	iFilledCircle(baseX, baseY, 6.0 * scale);

	iSetColor(65, 68, 75);
	double pX[] = { baseX - baseW * 0.5, baseX + baseW * 0.5, baseX + topW * 0.5, baseX - topW * 0.5 };
	double pY[] = { baseY, baseY, baseY + poleH, baseY + poleH };
	iFilledPolygon(pX, pY, 4);

	double armY1 = baseY + poleH - 12.0 * scale;
	double armW1 = 38.0 * scale;
	iSetColor(40, 42, 48);
	drawRect(baseX - armW1 * 0.5, armY1, armW1, 3.5 * scale);

	iSetColor(170, 175, 185);
	drawRect(baseX - armW1 * 0.45, armY1 + 3.5 * scale, 3.5 * scale, 5.5 * scale);
	drawRect(baseX - 1.7 * scale, armY1 + 3.5 * scale, 3.5 * scale, 5.5 * scale);
	drawRect(baseX + armW1 * 0.45 - 3.5 * scale, armY1 + 3.5 * scale, 3.5 * scale, 5.5 * scale);
}

void drawStreetLampL3(double baseX, double baseY, double scale, int side) {
	if (scale <= 0.05) return;

	double lampH = 115.0 * scale;
	double dir = (side == 0) ? 1.0 : -1.0;

	iSetColor(215, 185, 80);
	drawFilledEllipse(baseX + dir * 18.0 * scale, baseY, 22.0 * scale, 7.0 * scale, 16);

	iSetColor(18, 20, 25);
	iFilledCircle(baseX, baseY, 5.0 * scale);

	iSetColor(70, 75, 85);
	drawRect(baseX - 1.8 * scale, baseY, 3.6 * scale, lampH - 24.0 * scale);

	double topY = baseY + lampH - 18.0 * scale;
	double p1X = baseX;
	double p2X = baseX + dir * 18.0 * scale;

	iLine(p1X, topY, p2X, topY + 10.0 * scale);

	iSetColor(255, 235, 120);
	iFilledCircle(p2X, topY + 6.0 * scale, 8.0 * scale);
	iSetColor(255, 255, 255);
	iFilledCircle(p2X, topY + 6.0 * scale, 3.5 * scale);
}

void drawDhakaWasteBinL3(double baseX, double baseY, double scale, int side) {
	if (scale <= 0.05) return;

	double binW = 34.0 * scale;
	double binH = 46.0 * scale;

	iSetColor(16, 18, 22);
	drawRect(baseX - binW * 0.65, baseY - 2.0 * scale, binW * 1.3, 4.5 * scale);

	iSetColor(28, 30, 35);
	drawRect(baseX - binW * 0.58, baseY, 4.0 * scale, binH + 10.0 * scale);
	drawRect(baseX + binW * 0.58 - 4.0 * scale, baseY, 4.0 * scale, binH + 10.0 * scale);
	drawRect(baseX - binW * 0.58, baseY + binH + 7.0 * scale, binW * 1.16, 3.5 * scale);

	iSetColor(14, 85, 38);
	double barX = baseX - binW * 0.44;
	double barY = baseY + 8.0 * scale;
	double barW = binW * 0.88;
	drawRect(barX, barY, barW, binH);

	iSetColor(215, 220, 230);
	drawRect(barX + 4.0 * scale, barY + binH * 0.45, barW - 8.0 * scale, 4.0 * scale);
}

void drawElectricalBoxL3(double baseX, double baseY, double scale, int side) {
	if (scale <= 0.05) return;

	double boxW = 20.0 * scale;
	double boxH = 28.0 * scale;

	iSetColor(18, 20, 25);
	drawRect(baseX - boxW * 0.55, baseY - 2.0 * scale, boxW * 1.1, 4.0 * scale);

	iSetColor(65, 70, 78);
	drawRect(baseX - boxW * 0.55, baseY, boxW * 1.1, 5.0 * scale);

	iSetColor(24, 48, 35);
	drawRect(baseX - boxW * 0.5, baseY + 5.0 * scale, boxW, boxH);

	iSetColor(245, 195, 25);
	double triX[] = { baseX + boxW * 0.2, baseX + boxW * 0.36, baseX + boxW * 0.04 };
	double triY[] = { baseY + 18.0 * scale, baseY + 11.0 * scale, baseY + 11.0 * scale };
	iFilledPolygon(triX, triY, 3);
}

void drawRoadSignL3(double baseX, double baseY, double scale, int side, int variant) {
	if (scale <= 0.05) return;

	double signH = 45.0 * scale;
	iSetColor(75, 80, 88);
	drawRect(baseX - 7.0 * scale, baseY, 2.0 * scale, signH);
	drawRect(baseX + 5.0 * scale, baseY, 2.0 * scale, signH);

	double boardW = 32.0 * scale;
	double boardH = 18.0 * scale;
	double bX = baseX - boardW * 0.5;
	double bY = baseY + signH - 8.0 * scale;

	iSetColor(15, 75, 38);
	drawRect(bX, bY, boardW, boardH);
	iSetColor(220, 225, 235);
	drawRect(bX + 1.2 * scale, bY + 1.2 * scale, boardW - 2.4 * scale, 0.8 * scale);
	drawRect(bX + 4.0 * scale, bY + 8.0 * scale, boardW - 8.0 * scale, 3.0 * scale);
}

void drawFlowerPlanterL3(double baseX, double baseY, double scale, int side) {
	if (scale <= 0.05) return;

	double boxW = 22.0 * scale;
	double boxH = 10.0 * scale;

	iSetColor(95, 50, 32);
	drawRect(baseX - boxW * 0.5, baseY, boxW, boxH);

	iSetColor(20, 75, 32);
	iFilledCircle(baseX - 6.0 * scale, baseY + boxH + 6.0 * scale, 8.5 * scale);
	iFilledCircle(baseX + 6.0 * scale, baseY + boxH + 6.0 * scale, 8.5 * scale);

	iSetColor(215, 35, 35);
	iFilledCircle(baseX - 4.5 * scale, baseY + boxH + 7.5 * scale, 2.5 * scale);
	iSetColor(245, 195, 30);
	iFilledCircle(baseX + 3.5 * scale, baseY + boxH + 6.0 * scale, 2.2 * scale);
}

void drawRoadsidePropL3(int index) {
	if (!roadsidePropsL3[index].active) return;

	double y = roadsidePropsL3[index].y;
	double scale = getPerspectiveScaleL3(y);
	double cx = getRoadsideXL3(roadsidePropsL3[index].side, roadsidePropsL3[index].lateralOffset, y);

	int type = roadsidePropsL3[index].type;
	int side = roadsidePropsL3[index].side;
	int variant = roadsidePropsL3[index].variant;

	if (type == 0) drawRealisticTreeL3(cx, y, scale, variant);
	else if (type == 1) drawRealisticPalmTreeL3(cx, y, scale, variant);
	else if (type == 2) drawElectricalPoleL3(cx, y, scale, side);
	else if (type == 3) drawStreetLampL3(cx, y, scale, side);
	else if (type == 4) drawDhakaWasteBinL3(cx, y, scale, side);
	else if (type == 5) drawElectricalBoxL3(cx, y, scale, side);
	else if (type == 6) drawRoadSignL3(cx, y, scale, side, variant);
	else if (type == 7) drawFlowerPlanterL3(cx, y, scale, side);
}

void drawOverheadPowerLinesL3() {
	int poleIndices[MAX_ROADSIDE_PROPS_L3];
	int poleCount = 0;
	for (int i = 0; i < MAX_ROADSIDE_PROPS_L3; i++) {
		if (roadsidePropsL3[i].active && roadsidePropsL3[i].side == 0 && roadsidePropsL3[i].type == 2) {
			poleIndices[poleCount++] = i;
		}
	}

	for (int i = 0; i < poleCount - 1; i++) {
		for (int j = i + 1; j < poleCount; j++) {
			if (roadsidePropsL3[poleIndices[i]].y < roadsidePropsL3[poleIndices[j]].y) {
				int tmp = poleIndices[i];
				poleIndices[i] = poleIndices[j];
				poleIndices[j] = tmp;
			}
		}
	}

	if (poleCount >= 1) {
		double hX = 388.0;
		double hY = (double)horizonYL3 + 22.0;

		double firstY = roadsidePropsL3[poleIndices[0]].y;
		double firstScale = getPerspectiveScaleL3(firstY);
		double firstPoleX = getRoadsideXL3(0, roadsidePropsL3[poleIndices[0]].lateralOffset, firstY);
		double firstWireY1 = firstY + (135.0 - 8.5) * firstScale;

		iSetColor(28, 30, 36);
		iLine(hX, hY, firstPoleX - 16.0 * firstScale, firstWireY1);
	}
}

void drawFacadeWindowsL3(double x, double y, double w, double h, int cols, int rows, double marginX, double marginY, int lightSeed) {
	double totalGapX = marginX * 2.0 + (cols - 1) * (marginX * 0.6);
	double totalGapY = marginY * 2.0 + (rows - 1) * (marginY * 0.7);
	double winW = (w - totalGapX) / cols;
	double winH = (h - totalGapY) / rows;
	if (winW <= 1.0 || winH <= 1.0) return;

	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			double wx = x + marginX + c * (winW + marginX * 0.6);
			double wy = y + marginY + r * (winH + marginY * 0.7);

			int hash = (c * 7 + r * 13 + lightSeed) % 11;
			if (hash == 0 || hash == 4) iSetColor(255, 215, 110);
			else if (hash == 1 || hash == 6) iSetColor(180, 220, 255);
			else if (hash == 9) iSetColor(245, 180, 70);
			else iSetColor(24, 28, 38);
			drawRect(wx, wy, winW, winH);
		}
	}
}

void drawRooftopWaterTankL3(double x, double y, double scale) {
	iSetColor(55, 60, 70);
	drawRect(x + 2.0 * scale, y, 2.0 * scale, 11.0 * scale);
	drawRect(x + 14.0 * scale, y, 2.0 * scale, 11.0 * scale);
	iLine(x + 2.0 * scale, y + 2.0 * scale, x + 16.0 * scale, y + 9.0 * scale);
	iLine(x + 16.0 * scale, y + 2.0 * scale, x + 2.0 * scale, y + 9.0 * scale);

	iSetColor(38, 52, 72);
	drawRect(x, y + 10.0 * scale, 18.0 * scale, 14.0 * scale);
	iFilledCircle(x + 9.0 * scale, y + 23.5 * scale, 7.0 * scale);
}

void drawRooftopAntennaL3(double x, double y, double height) {
	iSetColor(140, 145, 155);
	drawRect(x - 1.2, y, 2.4, height);
	iLine(x - 7.0, y + height * 0.45, x + 7.0, y + height * 0.45);
	iLine(x - 5.0, y + height * 0.72, x + 5.0, y + height * 0.72);

	if ((animTimerL3 / 25) % 2 == 0) {
		iSetColor(255, 30, 30);
		iFilledCircle(x, y + height + 2.5, 3.5);
		iSetColor(255, 200, 200);
		iFilledCircle(x, y + height + 2.5, 1.5);
	}
	else {
		iSetColor(120, 20, 20);
		iFilledCircle(x, y + height + 2.5, 2.0);
	}
}

void drawRealisticBuildingsL3() {
	iSetColor(65, 75, 92);
	drawRect(30, horizonYL3, 55, 165);
	drawRect(110, horizonYL3, 50, 185);
	drawRect(185, horizonYL3, 65, 150);
	drawRect(275, horizonYL3, 55, 140);

	drawRect(615, horizonYL3, 60, 180);
	drawRect(700, horizonYL3, 50, 195);
	drawRect(775, horizonYL3, 65, 160);
	drawRect(850, horizonYL3, 45, 145);

	drawFacadeWindowsL3(35, horizonYL3, 45, 160, 3, 10, 3.0, 5.0, 1);
	drawFacadeWindowsL3(115, horizonYL3, 40, 180, 2, 12, 3.0, 5.0, 2);
	drawFacadeWindowsL3(190, horizonYL3, 55, 145, 3, 9, 3.0, 5.0, 3);
	drawFacadeWindowsL3(620, horizonYL3, 50, 175, 3, 11, 3.0, 5.0, 4);
	drawFacadeWindowsL3(705, horizonYL3, 40, 190, 2, 12, 3.0, 5.0, 5);
	drawFacadeWindowsL3(780, horizonYL3, 55, 155, 3, 10, 3.0, 5.0, 6);

	// LEFT BUILDINGS
	{
		double bx = 0, by = horizonYL3, bw = 75, bh = 140;
		iSetColor(155, 135, 110);
		drawRect(bx, by, bw, bh);
		iSetColor(125, 105, 85);
		drawRect(bx + bw - 5, by, 5, bh);
		drawFacadeWindowsL3(bx, by, bw - 5, bh - 10, 4, 9, 3.5, 4.0, 12);
		iSetColor(220, 225, 235);
		drawRect(bx, by + bh - 4, bw, 4);
		iSetColor(85, 95, 115);
		drawRect(bx + 15, by + bh, 35, 14);
		drawRooftopAntennaL3(bx + 32, by + bh + 14, 25);
	}
	{
		double bx = 72, by = horizonYL3, bw = 83, bh = 98;
		iSetColor(165, 85, 68);
		drawRect(bx, by, bw, bh);
		iSetColor(135, 65, 50);
		drawRect(bx + bw - 6, by, 6, bh);
		for (int floor = 0; floor < 4; floor++) {
			double fy = by + 8 + floor * 22;
			drawFacadeWindowsL3(bx + 4, fy + 5, 45, 14, 3, 1, 2.0, 1.5, floor * 3);
			iSetColor(140, 70, 55);
			drawRect(bx + 52, fy, 24, 15);
			iSetColor(225, 225, 230);
			drawRect(bx + 53, fy + 14, 22, 1.5);
		}
		iSetColor(245, 205, 50);
		drawRect(bx, by + bh - 4, bw, 4);
		drawRooftopWaterTankL3(bx + 20, by + bh + 4, 1.0);
	}
	{
		double bx = 152, by = horizonYL3, bw = 80, bh = 162;
		iSetColor(145, 155, 172);
		drawRect(bx, by, bw, bh);
		iSetColor(115, 125, 142);
		drawRect(bx + bw - 5, by, 5, bh);
		drawFacadeWindowsL3(bx, by, bw - 5, bh - 18, 5, 11, 2.5, 3.5, 27);
		iSetColor(225, 45, 45);
		drawRect(bx + 10, by + bh, bw - 20, 12);
		iSetColor(255, 255, 255);
		drawRect(bx + 14, by + bh + 3, bw - 28, 6);
		drawRooftopAntennaL3(bx + bw * 0.5, by + bh + 12, 22);
	}
	{
		double bx = 228, by = horizonYL3, bw = 77, bh = 112;
		iSetColor(178, 158, 128);
		drawRect(bx, by, bw, bh);
		iSetColor(145, 128, 102);
		drawRect(bx + bw - 5, by, 5, bh);
		drawFacadeWindowsL3(bx, by + 18, bw - 5, bh - 24, 4, 5, 3.0, 3.5, 15);
		iSetColor(35, 120, 65);
		double awX[] = { bx, bx + bw - 5, bx + bw - 2, bx - 3 };
		double awY[] = { by + 16, by + 16, by + 8, by + 8 };
		iFilledPolygon(awX, awY, 4);
		drawRooftopWaterTankL3(bx + 35, by + bh, 0.9);
	}
	{
		double bx = 302, by = horizonYL3, bw = 70, bh = 88;
		iSetColor(185, 175, 155);
		drawRect(bx, by, bw, bh);
		iSetColor(150, 140, 122);
		drawRect(bx + bw - 6, by, 6, bh);
		for (int floor = 0; floor < 3; floor++) {
			double wy = by + 14 + floor * 24;
			iSetColor(45, 75, 115);
			drawRect(bx + 5, wy, bw - 16, 12);
			iSetColor(165, 215, 255);
			drawRect(bx + 8, wy + 2, bw - 22, 8);
		}
		iSetColor(25, 85, 165);
		drawRect(bx, by + bh - 6, bw - 6, 6);
	}

	iSetColor(35, 75, 48);
	double treeX[] = { 372, 396, 422, 450, 478, 504, 528 };
	for (int i = 0; i < 6; i++) {
		iFilledCircle((treeX[i] + treeX[i + 1]) * 0.5, horizonYL3 + 2, 7.5);
	}

	// RIGHT BUILDINGS
	{
		double bx = 528, by = horizonYL3, bw = 70, bh = 90;
		iSetColor(185, 175, 155);
		drawRect(bx, by, bw, bh);
		iSetColor(150, 140, 122);
		drawRect(bx, by, 6, bh);
		for (int floor = 0; floor < 3; floor++) {
			double wy = by + 14 + floor * 24;
			iSetColor(45, 75, 115);
			drawRect(bx + 10, wy, bw - 16, 12);
			iSetColor(165, 215, 255);
			drawRect(bx + 13, wy + 2, bw - 22, 8);
		}
		iSetColor(215, 40, 45);
		drawRect(bx + 6, by + bh - 6, bw - 6, 6);
	}
	{
		double bx = 594, by = horizonYL3, bw = 78, bh = 158;
		iSetColor(140, 150, 168);
		drawRect(bx, by, bw, bh);
		iSetColor(110, 120, 138);
		drawRect(bx, by, 5, bh);
		drawFacadeWindowsL3(bx + 5, by, bw - 5, bh - 18, 5, 10, 2.5, 3.5, 38);
		iSetColor(245, 195, 30);
		drawRect(bx + 12, by + bh, bw - 24, 14);
		drawRooftopAntennaL3(bx + bw * 0.5, by + bh + 14, 28);
	}
	// AUST Campus Building (Ahsanullah University of Science and Technology)
	{
		double bx = 660, by = horizonYL3, bw = 88;
		double tw = 40.0;
		double tx = bx;
		double towerH = 180.0;
		double wWing = bw - tw;

		// 1. Campus Academic Wing (Attached to right of tower)
		double bh = 138.0;
		iSetColor(150, 140, 130);
		drawRect(tx + tw, by, wWing, bh);

		// 5 Floors with floor-to-floor tinted glass curtain windows & white concrete slabs
		for (int f = 0; f < 5; f++) {
			double fy = by + 6.0 + f * 25.0;
			// White concrete floor slab
			iSetColor(230, 232, 238);
			drawRect(tx + tw, fy + 18.0, wWing, 3.5);
			// Tinted architectural glass curtain wall
			iSetColor(52, 85, 112);
			drawRect(tx + tw + 2.0, fy, wWing - 4.0, 18.0);
			// Specular glass reflection highlight
			iSetColor(115, 170, 210);
			drawRect(tx + tw + 5.0, fy + 3.0, wWing * 0.40, 12.0);
			// Vertical window mullions
			iSetColor(35, 55, 75);
			drawRect(tx + tw + 15.0, fy, 1.5, 18.0);
			drawRect(tx + tw + 30.0, fy, 1.5, 18.0);
		}
		// Modern roof parapet
		iSetColor(220, 222, 228);
		drawRect(tx + tw, by + bh, wWing, 4.0);

		// 2. Iconic AUST Red Brick Tower (Column on the left)
		iSetColor(165, 62, 50); // Authentic terracotta red brick
		drawRect(tx, by, tw, towerH);
		// Shadow edge on the right side of the tower
		iSetColor(135, 48, 38);
		drawRect(tx + tw - 4.0, by, 4.0, towerH);
		// Horizontal architectural brick groove lines
		iSetColor(145, 52, 42);
		for (double gy = by + 14.0; gy < by + towerH - 42.0; gy += 14.0) {
			drawRect(tx, gy, tw - 4.0, 1.5);
		}
		// Tower top cap
		iSetColor(185, 72, 58);
		drawRect(tx - 1.0, by + towerH - 2.0, tw + 2.0, 3.0);

		// 3. AUST Emblem Crest at the top of the tower (like the image)
		double badgeW = 22.0;
		double badgeH = 22.0;
		double badgeX = tx + (tw - badgeW) / 2.0;
		double badgeY = by + towerH - 28.0;
		// White emblem shield background
		iSetColor(255, 255, 255);
		drawRect(badgeX, badgeY, badgeW, badgeH);
		// Ahsanullah Green border
		iSetColor(16, 122, 54);
		iRectangle(badgeX, badgeY, badgeW, badgeH);
		iRectangle(badgeX + 1.0, badgeY + 1.0, badgeW - 2.0, badgeH - 2.0);
		// Green university crest symbol (open book / emblem)
		drawRect(badgeX + 3.0, badgeY + 4.0, badgeW - 6.0, 2.0);
		drawRect(badgeX + badgeW * 0.5 - 1.0, badgeY + 4.0, 2.0, 10.0);
		iFilledCircle(badgeX + badgeW * 0.5, badgeY + 14.0, 3.2);
		iSetColor(255, 255, 255);
		iFilledCircle(badgeX + badgeW * 0.5, badgeY + 14.0, 1.3);

		// 4. Bold White "AUST" Letters under the emblem (like the image)
		double textY = badgeY - 14.0;
		double textX = tx + (tw - 30.0) / 2.0;
		// 3D drop shadow
		iSetColor(35, 12, 10);
		iText(textX + 1.0, textY - 1.0, (char*)"AUST", GLUT_BITMAP_HELVETICA_12);
		// Crisp bold white text
		iSetColor(255, 255, 255);
		iText(textX, textY, (char*)"AUST", GLUT_BITMAP_HELVETICA_12);
		iText(textX + 0.5, textY, (char*)"AUST", GLUT_BITMAP_HELVETICA_12);

		// 5. Lush green trees & campus entrance at base (like the image)
		iSetColor(25, 75, 40);
		iFilledCircle(tx + 6.0, by + 4.0, 8.0);
		iFilledCircle(tx + tw + 10.0, by + 5.0, 9.0);
		iFilledCircle(bx + bw - 6.0, by + 4.0, 7.0);
		// Iconic circular entrance gate feature from photo
		iSetColor(175, 168, 160);
		drawRect(tx + tw, by, 26.0, 16.0);
		iSetColor(200, 65, 75); // Circular red/pink motif
		iFilledCircle(tx + tw + 13.0, by + 8.0, 6.0);
		iSetColor(255, 255, 255);
		iFilledCircle(tx + tw + 13.0, by + 8.0, 2.0);
	}
	{
		double bx = 744, by = horizonYL3, bw = 82, bh = 145;
		iSetColor(160, 142, 118);
		drawRect(bx, by, bw, bh);
		iSetColor(130, 115, 95);
		drawRect(bx, by, 5, bh);
		drawFacadeWindowsL3(bx + 5, by, bw - 5, bh - 14, 5, 9, 2.5, 4.0, 42);
		iSetColor(35, 85, 155);
		drawRect(bx + 14, by + bh, bw - 28, 10);
		drawRooftopAntennaL3(bx + bw * 0.5, by + bh + 18, 18);
	}
	{
		double bx = 822, by = horizonYL3, bw = 88, bh = 110;
		iSetColor(175, 160, 138);
		drawRect(bx, by, bw, bh);
		iSetColor(140, 125, 105);
		drawRect(bx, by, 5, bh);
		drawFacadeWindowsL3(bx + 5, by + 12, bw - 8, bh - 20, 4, 5, 3.0, 3.5, 19);
		iSetColor(235, 165, 25);
		drawRect(bx + 15, by + bh, bw - 30, 8);
	}
}

void drawShopperAtStallL3(double cx, double cy, double scale, int umbrellaColor, int gender) {
	if (scale <= 0.05) return;
	double h = 55.0 * scale;

	if (gender == 1) {
		iSetColor(188, 35, 65);
		double skX[] = { cx - 5.0 * scale, cx + 5.0 * scale, cx + 8.5 * scale, cx - 8.5 * scale };
		double skY[] = { cy + h * 0.40, cy + h * 0.40, cy, cy };
		iFilledPolygon(skX, skY, 4);

		iSetColor(245, 205, 50);
		drawRect(cx - 8.5 * scale, cy, 17.0 * scale, 2.5 * scale);

		iSetColor(160, 28, 55);
		drawRect(cx - 4.2 * scale, cy + h * 0.40, 8.4 * scale, h * 0.28);

		iSetColor(220, 170, 135);
		iFilledCircle(cx, cy + h * 0.74, 3.8 * scale);

		iSetColor(25, 20, 20);
		iFilledCircle(cx - 3.2 * scale, cy + h * 0.76, 3.0 * scale);
		iSetColor(255, 250, 230);
		iFilledCircle(cx - 3.2 * scale, cy + h * 0.81, 1.4 * scale);
	}
	else {
		iSetColor(35, 45, 65);
		drawRect(cx - 4.5 * scale, cy, 9.0 * scale, h * 0.65);
		iSetColor(215, 165, 130);
		iFilledCircle(cx, cy + h * 0.75, 4.0 * scale);
		iSetColor(25, 22, 22);
		iFilledCircle(cx, cy + h * 0.79, 3.6 * scale);
	}

	int umbR = 220, umbG = 35, umbB = 40;
	if (umbrellaColor == 1) { umbR = 25; umbG = 85; umbB = 185; }
	else if (umbrellaColor == 2) { umbR = 245; umbG = 195; umbB = 25; }
	else if (umbrellaColor == 3) { umbR = 25; umbG = 145; umbB = 65; }

	iSetColor(40, 42, 48);
	drawRect(cx + 3.0 * scale, cy + h * 0.40, 1.8 * scale, 22.0 * scale);
	iSetColor(umbR, umbG, umbB);
	drawFilledEllipse(cx, cy + h + 6.0 * scale, 18.0 * scale, 9.0 * scale, 14);
}

void drawBismillahTeaStallL3(double cx, double cy, double scale, int side, int variant) {
	if (scale <= 0.05) return;

	double baseW = 126.0;
	double baseH = 102.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = cx - w * 0.5;

	iSetColor(225, 185, 75);
	drawFilledEllipse(cx, cy, w * 0.75, 11.0 * scale, 16);

	iSetColor(22, 24, 28);
	drawRect(bx - 4.0 * scale, cy - 3.0 * scale, w + 8.0 * scale, 6.0 * scale);
	iSetColor(85, 48, 25);
	drawRect(bx + 4.0 * scale, cy, w - 8.0 * scale, 28.0 * scale);
	iSetColor(115, 68, 38);
	drawRect(bx, cy + 28.0 * scale, w, 5.0 * scale);

	iSetColor(215, 165, 130);
	iFilledCircle(bx + w * 0.35, cy + 42.0 * scale, 6.5 * scale);
	iSetColor(215, 45, 45);
	drawRect(bx + w * 0.35 - 5.5 * scale, cy + 24.0 * scale, 11.0 * scale, 14.0 * scale);

	iSetColor(175, 180, 190);
	drawRect(bx + w * 0.52, cy + 33.0 * scale, 16.0 * scale, 14.0 * scale);
	iFilledCircle(bx + w * 0.60, cy + 47.0 * scale, 5.0 * scale);
	iSetColor(225, 230, 240);
	iFilledCircle(bx + w * 0.48, cy + 52.0 * scale, 3.0 * scale);
	iFilledCircle(bx + w * 0.46, cy + 60.0 * scale, 4.0 * scale);

	iSetColor(255, 235, 120);
	iFilledCircle(cx, cy + 68.0 * scale, 6.0 * scale);
	iSetColor(255, 255, 255);
	iFilledCircle(cx, cy + 68.0 * scale, 2.8 * scale);

	iSetColor(15, 115, 45);
	drawRect(bx - 6.0 * scale, cy + h - 18.0 * scale, w + 12.0 * scale, 22.0 * scale);
	iSetColor(245, 205, 30);
	drawRect(bx - 4.0 * scale, cy + h - 16.0 * scale, w + 8.0 * scale, 2.0 * scale);
	drawRect(bx - 4.0 * scale, cy + h + 2.0 * scale, w + 8.0 * scale, 2.0 * scale);

	iSetColor(255, 255, 255);
	drawRect(bx + 4.0 * scale, cy + h - 12.0 * scale, w - 8.0 * scale, 10.0 * scale);
	iSetColor(185, 25, 30);
	drawRect(bx + 8.0 * scale, cy + h - 10.0 * scale, w - 16.0 * scale, 6.0 * scale);

	drawShopperAtStallL3(bx + 14.0 * scale, cy, scale, 0, 0);
	drawShopperAtStallL3(bx + w - 16.0 * scale, cy, scale, 1, 1);
}

void drawChotpotiFuchkaStallL3(double cx, double cy, double scale, int side, int variant) {
	if (scale <= 0.05) return;

	double baseW = 120.0;
	double baseH = 98.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = cx - w * 0.5;

	iSetColor(225, 185, 75);
	drawFilledEllipse(cx, cy, w * 0.70, 10.0 * scale, 16);

	iSetColor(22, 24, 28);
	drawRect(bx - 3.0 * scale, cy - 2.5 * scale, w + 6.0 * scale, 5.0 * scale);

	iSetColor(110, 68, 38);
	drawRect(bx + 6.0 * scale, cy, w - 12.0 * scale, 28.0 * scale);
	iSetColor(140, 88, 52);
	drawRect(bx, cy + 28.0 * scale, w, 5.0 * scale);

	iSetColor(25, 35, 48);
	drawRect(bx + 8.0 * scale, cy + 33.0 * scale, w * 0.52, 24.0 * scale);
	iSetColor(245, 195, 35);
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 4; col++) {
			iFilledCircle(bx + 14.0 * scale + col * 10.5 * scale, cy + 38.0 * scale + row * 7.0 * scale, 4.0 * scale);
		}
	}

	iSetColor(215, 165, 130);
	iFilledCircle(bx + w * 0.72, cy + 45.0 * scale, 6.0 * scale);
	iSetColor(35, 85, 155);
	drawRect(bx + w * 0.72 - 5.0 * scale, cy + 28.0 * scale, 10.0 * scale, 14.0 * scale);

	iSetColor(195, 25, 35);
	drawRect(bx - 5.0 * scale, cy + h - 18.0 * scale, w + 10.0 * scale, 20.0 * scale);
	iSetColor(255, 255, 255);
	drawRect(bx + 6.0 * scale, cy + h - 12.0 * scale, w - 12.0 * scale, 9.0 * scale);
	iSetColor(195, 25, 35);
	drawRect(bx + 10.0 * scale, cy + h - 10.0 * scale, w - 20.0 * scale, 5.0 * scale);

	drawShopperAtStallL3(bx + 12.0 * scale, cy, scale, 2, 1);
	drawShopperAtStallL3(bx + w - 14.0 * scale, cy, scale, 3, 0);
}

void drawAlMadinaFruitStallL3(double cx, double cy, double scale, int side, int variant) {
	if (scale <= 0.05) return;

	double baseW = 118.0;
	double baseH = 98.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = cx - w * 0.5;

	iSetColor(22, 24, 28);
	drawRect(bx, cy - 2.5 * scale, w, 5.0 * scale);

	iSetColor(115, 72, 38);
	drawRect(bx + 4.0 * scale, cy, w - 8.0 * scale, 26.0 * scale);
	iSetColor(145, 92, 48);
	drawRect(bx, cy + 26.0 * scale, w, 5.0 * scale);

	iSetColor(28, 125, 45);
	iFilledCircle(bx + 16.0 * scale, cy + 38.0 * scale, 7.5 * scale);
	iFilledCircle(bx + 28.0 * scale, cy + 40.0 * scale, 8.0 * scale);
	iFilledCircle(bx + 22.0 * scale, cy + 50.0 * scale, 7.0 * scale);

	iSetColor(215, 35, 40);
	double wmX[] = { bx + 45.0 * scale, bx + 70.0 * scale, bx + 57.5 * scale };
	double wmY[] = { cy + 34.0 * scale, cy + 34.0 * scale, cy + 50.0 * scale };
	iFilledPolygon(wmX, wmY, 3);

	iSetColor(215, 165, 130);
	iFilledCircle(bx + w * 0.78, cy + 45.0 * scale, 6.0 * scale);

	iSetColor(20, 115, 48);
	drawRect(bx - 5.0 * scale, cy + h - 18.0 * scale, w + 10.0 * scale, 20.0 * scale);
	iSetColor(255, 255, 255);
	drawRect(bx + 6.0 * scale, cy + h - 12.0 * scale, w - 12.0 * scale, 9.0 * scale);
	iSetColor(20, 115, 48);
	drawRect(bx + 10.0 * scale, cy + h - 10.0 * scale, w - 20.0 * scale, 5.0 * scale);

	drawShopperAtStallL3(bx + 10.0 * scale, cy, scale, 1, 1);
}

void drawDhakaSweetsStallL3(double cx, double cy, double scale, int side, int variant) {
	if (scale <= 0.05) return;

	double baseW = 124.0;
	double baseH = 100.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = cx - w * 0.5;

	iSetColor(225, 185, 75);
	drawFilledEllipse(cx, cy, w * 0.72, 10.0 * scale, 16);

	iSetColor(42, 65, 95);
	drawRect(bx, cy, w, h - 20.0 * scale);
	iSetColor(18, 30, 48);
	drawRect(bx + 6.0 * scale, cy + 8.0 * scale, w - 12.0 * scale, 34.0 * scale);

	iSetColor(255, 215, 120);
	for (int b = 0; b < 5; b++) {
		iFilledCircle(bx + 16.0 * scale + b * 18.0 * scale, cy + 22.0 * scale, 5.0 * scale);
	}

	iSetColor(215, 165, 130);
	iFilledCircle(cx, cy + 34.0 * scale, 6.0 * scale);

	iSetColor(25, 55, 145);
	drawRect(bx - 5.0 * scale, cy + h - 18.0 * scale, w + 10.0 * scale, 20.0 * scale);
	iSetColor(255, 255, 255);
	drawRect(bx + 6.0 * scale, cy + h - 12.0 * scale, w - 12.0 * scale, 9.0 * scale);
	iSetColor(25, 55, 145);
	drawRect(bx + 10.0 * scale, cy + h - 10.0 * scale, w - 20.0 * scale, 5.0 * scale);

	drawShopperAtStallL3(bx + 12.0 * scale, cy, scale, 0, 0);
	drawShopperAtStallL3(bx + w - 14.0 * scale, cy, scale, 2, 1);
}

void drawSchoolBuildingL3(double cx, double cy, double scale, int side, int variant) {
	if (scale <= 0.05) return;

	double baseW = 122.0;
	double baseH = 102.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = cx - w * 0.5;

	iSetColor(155, 75, 60);
	drawRect(bx, cy, w, h);
	iSetColor(185, 190, 205);
	drawRect(bx, cy + h - 6.0 * scale, w, 6.0 * scale);

	iSetColor(25, 16, 14);
	drawRect(bx + w * 0.35, cy, w * 0.30, 34.0 * scale);
	iSetColor(255, 225, 110);
	iFilledCircle(bx + w * 0.34, cy + 38.0 * scale, 4.0 * scale);
	iFilledCircle(bx + w * 0.67, cy + 38.0 * scale, 4.0 * scale);

	for (int floor = 0; floor < 2; floor++) {
		double wy = cy + 42.0 * scale + floor * 26.0 * scale;
		for (int c = 0; c < 4; c++) {
			double wx = bx + 8.0 * scale + c * (w / 4.4);
			iSetColor(255, 220, 120);
			drawRect(wx, wy, 16.0 * scale, 18.0 * scale);
		}
	}

	iSetColor(15, 105, 45);
	drawRect(bx + 12.0 * scale, cy + h + 2.0 * scale, w - 24.0 * scale, 12.0 * scale);
	iSetColor(255, 255, 255);
	drawRect(bx + 14.0 * scale, cy + h + 3.5 * scale, w - 28.0 * scale, 9.0 * scale);

	drawShopperAtStallL3(bx + w * 0.82, cy, scale, 1, 1);
}

void drawCollegeBuildingL3(double cx, double cy, double scale, int side, int variant) {
	if (scale <= 0.05) return;

	double baseW = 124.0;
	double baseH = 105.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = cx - w * 0.5;

	iSetColor(155, 162, 175);
	drawRect(bx, cy, w, h);

	iSetColor(185, 192, 205);
	for (int p = 0; p < 5; p++) {
		double px = bx + 6.0 * scale + p * (w / 4.4);
		drawRect(px, cy, 6.0 * scale, h - 14.0 * scale);
	}

	for (int floor = 0; floor < 3; floor++) {
		double wy = cy + 18.0 * scale + floor * 24.0 * scale;
		for (int c = 0; c < 3; c++) {
			double wx = bx + 16.0 * scale + c * (w / 3.4);
			iSetColor(180, 220, 255);
			drawRect(wx, wy, 15.0 * scale, 14.0 * scale);
		}
	}

	iSetColor(135, 142, 155);
	double triX[] = { bx, bx + w, bx + w * 0.5 };
	double triY[] = { cy + h - 14.0 * scale, cy + h - 14.0 * scale, cy + h + 14.0 * scale };
	iFilledPolygon(triX, triY, 3);

	iSetColor(20, 45, 125);
	drawRect(bx + 16.0 * scale, cy + h - 12.0 * scale, w - 32.0 * scale, 10.0 * scale);
	iSetColor(255, 255, 255);
	drawRect(bx + 18.0 * scale, cy + h - 10.5 * scale, w - 36.0 * scale, 7.0 * scale);
	if (scale >= 0.22) {
		iSetColor(16, 122, 54);
		iText(bx + w * 0.5 - 14.0 * scale, cy + h - 9.0 * scale, (char*)"AUST", GLUT_BITMAP_HELVETICA_10);
	}

	drawShopperAtStallL3(bx + 14.0 * scale, cy, scale, 3, 0);
}

void drawRoadsidePondL3(double cx, double cy, double scale, int side, int variant) {
	if (scale <= 0.05) return;

	double pondRx = 58.0 * scale;
	double pondRy = 26.0 * scale;

	iSetColor(20, 52, 28);
	drawFilledEllipse(cx, cy + pondRy * 0.4, pondRx + 8.0 * scale, pondRy + 6.0 * scale, 18);
	iSetColor(12, 28, 48);
	drawFilledEllipse(cx, cy + pondRy * 0.4, pondRx, pondRy, 18);

	iSetColor(35, 75, 115);
	drawFilledEllipse(cx - 18.0 * scale, cy + pondRy * 0.3, 14.0 * scale, 5.0 * scale, 14);
	drawFilledEllipse(cx + 16.0 * scale, cy + pondRy * 0.5, 16.0 * scale, 6.0 * scale, 14);
	iSetColor(225, 195, 95);
	drawFilledEllipse(cx - 8.0 * scale, cy + pondRy * 0.4, 8.0 * scale, 3.2 * scale, 12);

	drawShopperAtStallL3(cx + pondRx * 0.70, cy, scale, 0, 1);
}

void drawNewsKioskL3(double cx, double cy, double scale, int side) {
	if (scale <= 0.05) return;

	double baseW = 84.0;
	double baseH = 82.0;
	double w = baseW * scale;
	double h = baseH * scale;
	double bx = cx - w * 0.5;

	iSetColor(225, 185, 75);
	drawFilledEllipse(cx, cy, w * 0.65, 8.0 * scale, 16);

	iSetColor(35, 62, 98);
	drawRect(bx, cy, w, h - 14.0 * scale);
	iSetColor(12, 22, 38);
	drawRect(bx + 5.0 * scale, cy + 10.0 * scale, w - 10.0 * scale, h - 30.0 * scale);

	iSetColor(240, 242, 245);
	drawRect(bx + 8.0 * scale, cy + 14.0 * scale, 14.0 * scale, 18.0 * scale);
	iSetColor(215, 35, 35);
	drawRect(bx + 26.0 * scale, cy + 14.0 * scale, 14.0 * scale, 18.0 * scale);
	iSetColor(245, 195, 25);
	drawRect(bx + 44.0 * scale, cy + 14.0 * scale, 14.0 * scale, 18.0 * scale);

	iSetColor(225, 165, 25);
	drawRect(bx - 3.0 * scale, cy + h - 14.0 * scale, w + 6.0 * scale, 12.0 * scale);
	iSetColor(255, 255, 255);
	drawRect(bx + 4.0 * scale, cy + h - 11.0 * scale, w - 8.0 * scale, 6.0 * scale);

	drawShopperAtStallL3(bx + w + 8.0 * scale, cy, scale, 1, 0);
}

void drawOuterPropL3(int index) {
	if (!outerPropsL3[index].active) return;
	double y = outerPropsL3[index].y;
	double localShiftY = sin((double)sideAnimTimerL3 * 0.04 + (double)index * 1.5) * 1.5;
	y += localShiftY;
	double scale = getPerspectiveScaleL3(y);
	double cx = getOuterPromenadeXL3(outerPropsL3[index].side, outerPropsL3[index].offsetRatio, y);
	int type = outerPropsL3[index].type;
	int side = outerPropsL3[index].side;
	int variant = outerPropsL3[index].variant;

	if (type == 0) drawBismillahTeaStallL3(cx, y, scale, side, variant);
	else if (type == 1) drawChotpotiFuchkaStallL3(cx, y, scale, side, variant);
	else if (type == 2) drawAlMadinaFruitStallL3(cx, y, scale, side, variant);
	else if (type == 3) drawDhakaSweetsStallL3(cx, y, scale, side, variant);
	else if (type == 4) drawSchoolBuildingL3(cx, y, scale, side, variant);
	else if (type == 5) drawCollegeBuildingL3(cx, y, scale, side, variant);
	else if (type == 6) drawRoadsidePondL3(cx, y, scale, side, variant);
	else if (type == 7) drawNewsKioskL3(cx, y, scale, side);
}

void drawRiverWaterAndBridgeRailsL3() {
	iSetColor(12, 34, 58);
	drawRect(0, 0, SCREEN_WIDTH, horizonYL3);

	iSetColor(20, 52, 85);
	for (float p = 0.05f; p <= 0.95f; p += 0.12f) {
		double py = horizonYL3 * p;
		double waveX = sin(riverWaveOffsetL3 + p * 6.0) * 18.0;
		drawRect(0, py, 380 + waveX, 4.0);
		drawRect(520 + waveX, py, SCREEN_WIDTH, 4.0);
	}

	iSetColor(35, 78, 120);
	for (float p = 0.10f; p <= 0.90f; p += 0.20f) {
		double py = horizonYL3 * p;
		drawFilledEllipse(180, py, 65.0, 5.0, 14);
		drawFilledEllipse(720, py, 65.0, 5.0, 14);
	}

	for (float p = 0.0f; p <= 1.0f; p += 0.18f) {
		double postY_bottom = horizonYL3 * p;
		double scale = 0.22 + 0.78 * (1.0 - p);
		double postH = 95.0 * scale;

		double lx = 375 * p;
		double rx = 900 - (375 * p);

		iSetColor(85, 98, 115);
		drawRect(lx - 5.0 * scale, postY_bottom, 6.0 * scale, postH);
		iSetColor(255, 215, 75);
		iFilledCircle(lx - 2.0 * scale, postY_bottom + postH, 5.0 * scale);

		iSetColor(85, 98, 115);
		drawRect(rx - 1.0 * scale, postY_bottom, 6.0 * scale, postH);
		iSetColor(255, 215, 75);
		iFilledCircle(rx + 2.0 * scale, postY_bottom + postH, 5.0 * scale);
	}

	iSetColor(65, 75, 90);
	for (float p = 0.0f; p < 0.9f; p += 0.18f) {
		double y1 = horizonYL3 * p;
		double y2 = horizonYL3 * (p + 0.18f);
		double lx1 = 375 * p;
		double lx2 = 375 * (p + 0.18f);
		double rx1 = 900 - (375 * p);
		double rx2 = 900 - (375 * (p + 0.18f));

		iLine(lx1, y1 + 50.0, lx2, y2 + 20.0);
		iLine(rx1, y1 + 50.0, rx2, y2 + 20.0);
	}
}

void drawDynamicOuterGroundL3(int pathMode) {
	if (pathMode == 1) {
		drawRiverWaterAndBridgeRailsL3();
	}
	else {
		iSetColor(55, 52, 58);
		drawRect(0, 0, SCREEN_WIDTH, horizonYL3);
		iSetColor(42, 40, 46);
		for (float p = 0.1f; p <= 0.9f; p += 0.2f) {
			double py = horizonYL3 * p;
			drawRect(0, py, SCREEN_WIDTH, 1.8);
		}
	}
}

void drawProceduralBackgroundL3() {
	int skyR = 14, skyG = 20, skyB = 38;
	int isThunderFlashing = 0;

	if (thunderFlashTimerL3 > 0) {
		isThunderFlashing = 1;
		if (thunderFlashTimerL3 >= 4) {
			skyR = 195; skyG = 220; skyB = 255;
		}
		else if (thunderFlashTimerL3 >= 2) {
			skyR = 95;  skyG = 135; skyB = 195;
		}
		else {
			skyR = 45;  skyG = 65;  skyB = 110;
		}
	}

	iSetColor(skyR, skyG, skyB);
	drawRect(0, horizonYL3, SCREEN_WIDTH, SCREEN_HEIGHT - horizonYL3);

	if (lightningBoltActiveL3) {
		drawLightningBoltL3(lightningForkXL3, (float)SCREEN_HEIGHT - 10.0f);
	}

	for (int i = -1; i <= 1; i++) {
		double shiftX = cloudOffsetL3 + (i * SCREEN_WIDTH);
		drawHeavyStormCloudL3(150 + shiftX, horizonYL3 + 195, 1.15, isThunderFlashing);
		drawHeavyStormCloudL3(430 + shiftX, horizonYL3 + 225, 1.45, isThunderFlashing);
		drawHeavyStormCloudL3(680 + shiftX, horizonYL3 + 175, 1.10, isThunderFlashing);
		drawHeavyStormCloudL3(880 + shiftX, horizonYL3 + 215, 1.25, isThunderFlashing);
	}

	drawRealisticBuildingsL3();
	drawDynamicOuterGroundL3(currentPathModeL3);

	if (currentPathModeL3 == 0) {
		int sortedOuter[MAX_OUTER_PROPS_L3];
		for (int i = 0; i < MAX_OUTER_PROPS_L3; i++) sortedOuter[i] = i;
		for (int i = 0; i < MAX_OUTER_PROPS_L3 - 1; i++) {
			for (int j = i + 1; j < MAX_OUTER_PROPS_L3; j++) {
				if (outerPropsL3[sortedOuter[i]].y < outerPropsL3[sortedOuter[j]].y) {
					int tmp = sortedOuter[i];
					sortedOuter[i] = sortedOuter[j];
					sortedOuter[j] = tmp;
				}
			}
		}
		for (int i = 0; i < MAX_OUTER_PROPS_L3; i++) {
			drawOuterPropL3(sortedOuter[i]);
		}
	}

	iSetColor(65, 68, 75);
	double leftCurbX[] = { 0, 380, 390, 0 };
	double rightCurbX[] = { 900, 520, 510, 900 };
	double roadY[] = { 0, (double)horizonYL3, (double)horizonYL3, 0 };
	iFilledPolygon(leftCurbX, roadY, 4);
	iFilledPolygon(rightCurbX, roadY, 4);

	iSetColor(24, 28, 34);
	double roadX[] = { 60, 395, 505, 840 };
	iFilledPolygon(roadX, roadY, 4);

	iSetColor(32, 36, 45);
	double wetSpecX[] = { 200, 420, 480, 700 };
	iFilledPolygon(wetSpecX, roadY, 4);

	iSetColor(215, 220, 230);
	double leftWhiteX[] = { 75, 397, 400, 85 };
	double rightWhiteX[] = { 825, 503, 500, 815 };
	iFilledPolygon(leftWhiteX, roadY, 4);
	iFilledPolygon(rightWhiteX, roadY, 4);

	iSetColor(22, 68, 38);
	double lRailTopX[] = { 0, 375, 377, 0 };
	double lRailTopY[] = { 80, (double)horizonYL3 + 12, (double)horizonYL3 + 16, 88 };
	double lRailBotY[] = { 40, (double)horizonYL3 + 4, (double)horizonYL3 + 8, 48 };
	iFilledPolygon(lRailTopX, lRailTopY, 4);
	iFilledPolygon(lRailTopX, lRailBotY, 4);

	double rRailTopX[] = { 900, 525, 523, 900 };
	iFilledPolygon(rRailTopX, lRailTopY, 4);
	iFilledPolygon(rRailTopX, lRailBotY, 4);

	iSetColor(235, 185, 25);
	for (float t = lineOffsetL3; t < 1.0f; t += 0.25f) {
		double topPercent = 1.0f - t;
		double bottomPercent = 1.0f - (t + 0.15f);
		if (bottomPercent < 0) bottomPercent = 0;

		double yTop = horizonYL3 * topPercent;
		double yBottom = horizonYL3 * bottomPercent;

		double halfWidthTop = 2.0 + (1.0 - topPercent) * 10.0;
		double halfWidthBottom = 2.0 + (1.0 - bottomPercent) * 10.0;

		double yellowX[] = { 450 - halfWidthBottom, 450 + halfWidthBottom, 450 + halfWidthTop, 450 - halfWidthTop };
		double segY[] = { yBottom, yBottom, yTop, yTop };
		iFilledPolygon(yellowX, segY, 4);

		iSetColor(255, 245, 160);
		drawRect(450 - 1.5, yBottom + 2.0, 3.0, 3.0);
		iSetColor(235, 185, 25);
	}
}

void drawContinuousRainL3() {
	for (int i = 0; i < MAX_RAINDROPS_L3; i++) {
		int b = rainDropsL3[i].brightness;
		iSetColor(b - 20, b, b + 25);
		iLine(rainDropsL3[i].x, rainDropsL3[i].y, rainDropsL3[i].x + rainDropsL3[i].slant * 2.5f, rainDropsL3[i].y - rainDropsL3[i].length);

		if (rainDropsL3[i].y < 150.0f) {
			iSetColor(140, 175, 215);
			drawFilledEllipse(rainDropsL3[i].x, rainDropsL3[i].y, 3.5, 1.2, 8);
		}
	}
}

typedef struct {
	int entityType;
	int index;
	float y;
} SceneEntityL3;

void drawVectorHeartL3(int cx, int cy, int size, int isFilled) {
	double r = (double)size;
	if (isFilled) {
		iSetColor(14, 16, 20);
		iFilledCircle(cx - r * 0.48 + 1, cy + r * 0.22 - 1, r * 0.52);
		iFilledCircle(cx + r * 0.48 + 1, cy + r * 0.22 - 1, r * 0.52);
		double shX[] = { cx - r * 0.95 + 1, cx + r * 0.95 + 1, (double)(cx + 1) };
		double shY[] = { cy + r * 0.22 - 1, cy + r * 0.22 - 1, cy - r * 0.85 - 1 };
		iFilledPolygon(shX, shY, 3);

		iSetColor(235, 30, 45);
		iFilledCircle(cx - r * 0.48, cy + r * 0.22, r * 0.52);
		iFilledCircle(cx + r * 0.48, cy + r * 0.22, r * 0.52);
		double triX[] = { cx - r * 0.95, cx + r * 0.95, (double)cx };
		double triY[] = { cy + r * 0.22, cy + r * 0.22, cy - r * 0.85 };
		iFilledPolygon(triX, triY, 3);

		iSetColor(255, 175, 185);
		iFilledCircle(cx - r * 0.48, cy + r * 0.40, r * 0.18);
	}
	else {
		iSetColor(65, 70, 78);
		iFilledCircle(cx - r * 0.48, cy + r * 0.22, r * 0.52);
		iFilledCircle(cx + r * 0.48, cy + r * 0.22, r * 0.52);
		double triX[] = { cx - r * 0.95, cx + r * 0.95, (double)cx };
		double triY[] = { cy + r * 0.22, cy + r * 0.22, cy - r * 0.85 };
		iFilledPolygon(triX, triY, 3);

		iSetColor(24, 26, 32);
		iFilledCircle(cx - r * 0.48, cy + r * 0.22, r * 0.38);
		iFilledCircle(cx + r * 0.48, cy + r * 0.22, r * 0.38);
		double inX[] = { cx - r * 0.72, cx + r * 0.72, (double)cx };
		double inY[] = { cy + r * 0.20, cy + r * 0.20, cy - r * 0.65 };
		iFilledPolygon(inX, inY, 3);
	}
}

void drawLivesHUDL3() {
	int hudX = 665;
	int hudY = 538;
	int hudW = 220;
	int hudH = 48;

	iSetColor(14, 18, 26);
	drawRect(hudX, hudY, hudW, hudH);

	iSetColor(215, 180, 50);
	drawRect(hudX, hudY + hudH - 2, hudW, 2);
	drawRect(hudX, hudY, hudW, 2);
	drawRect(hudX, hudY, 2, hudH);
	drawRect(hudX + hudW - 2, hudY, 2, hudH);

	iSetColor(255, 255, 255);
	iText(hudX + 12, hudY + 18, "LIVES", GLUT_BITMAP_HELVETICA_12);

	for (int i = 0; i < 5; i++) {
		int heartX = hudX + 68 + i * 30;
		int heartY = hudY + 24;
		drawVectorHeartL3(heartX, heartY, 11, (i < playerLivesL3));
	}
}

void drawCoinsHUDL3() {
	int hudX = 710;
	int hudY = 482;
	int hudW = 175;
	int hudH = 48;

	iSetColor(14, 18, 26);
	drawRect(hudX, hudY, hudW, hudH);

	iSetColor(215, 180, 50);
	drawRect(hudX, hudY + hudH - 2, hudW, 2);
	drawRect(hudX, hudY, hudW, 2);
	drawRect(hudX, hudY, 2, hudH);
	drawRect(hudX + hudW - 2, hudY, 2, hudH);

	int iconX = hudX + 22;
	int iconY = hudY + 24;
	iSetColor(150, 95, 10);
	drawFilledEllipse(iconX, iconY - 2, 11, 11, 16);
	iSetColor(245, 195, 30);
	drawFilledEllipse(iconX, iconY, 11, 11, 16);
	iSetColor(255, 235, 120);
	drawFilledEllipse(iconX, iconY + 1, 9, 9, 16);
	iSetColor(255, 205, 30);
	drawFilledEllipse(iconX, iconY, 7, 7, 16);
	iSetColor(255, 255, 255);
	iFilledCircle(iconX - 3, iconY + 3, 2);

	iSetColor(255, 255, 255);
	iText(hudX + 42, hudY + 18, "COINS:", GLUT_BITMAP_HELVETICA_12);

	char coinStr[16];
	sprintf_s(coinStr, "%d", coinCountL3);
	iSetColor(255, 220, 50);
	iText(hudX + 104, hudY + 16, coinStr, GLUT_BITMAP_HELVETICA_18);
}

void drawLevel3Scene() {
	// Layer 1: Procedural Background (Sky, Clouds, Buildings, Highway/Bridge, Road)
	drawProceduralBackgroundL3();

	// Collect entities for 3D depth sorting
	SceneEntityL3 entities[MAX_BUSES_L3 + MAX_PEOPLE_L3 + MAX_DOGS_L3 + MAX_TRAFFIC_RICKSHAWS_L3 + MAX_ROADSIDE_PROPS_L3 + MAX_COINS_L3 + MAX_STONES_L3 + MAX_BOOMS_L3 + MAX_BULLET_PICKUPS_L3];
	int totalEntities = 0;

	for (int i = 0; i < MAX_BUSES_L3; i++) {
		if (busesL3[i].active) {
			entities[totalEntities].entityType = 0;
			entities[totalEntities].index = i;
			entities[totalEntities].y = busesL3[i].y;
			totalEntities++;
		}
	}
	for (int i = 0; i < MAX_PEOPLE_L3; i++) {
		if (peopleL3[i].active) {
			entities[totalEntities].entityType = 1;
			entities[totalEntities].index = i;
			entities[totalEntities].y = peopleL3[i].y;
			totalEntities++;
		}
	}
	for (int i = 0; i < MAX_DOGS_L3; i++) {
		if (dogsL3[i].active) {
			entities[totalEntities].entityType = 2;
			entities[totalEntities].index = i;
			entities[totalEntities].y = dogsL3[i].y;
			totalEntities++;
		}
	}
	for (int i = 0; i < MAX_TRAFFIC_RICKSHAWS_L3; i++) {
		if (otherRickshawsL3[i].active) {
			entities[totalEntities].entityType = 3;
			entities[totalEntities].index = i;
			entities[totalEntities].y = otherRickshawsL3[i].y;
			totalEntities++;
		}
	}
	for (int i = 0; i < MAX_ROADSIDE_PROPS_L3; i++) {
		if (roadsidePropsL3[i].active) {
			entities[totalEntities].entityType = 4;
			entities[totalEntities].index = i;
			entities[totalEntities].y = roadsidePropsL3[i].y;
			totalEntities++;
		}
	}
	for (int i = 0; i < MAX_COINS_L3; i++) {
		if (coinsL3[i].active) {
			entities[totalEntities].entityType = 5;
			entities[totalEntities].index = i;
			entities[totalEntities].y = coinsL3[i].y;
			totalEntities++;
		}
	}
	for (int i = 0; i < MAX_STONES_L3; i++) {
		if (roadStonesL3[i].active) {
			entities[totalEntities].entityType = 6;
			entities[totalEntities].index = i;
			entities[totalEntities].y = roadStonesL3[i].y;
			totalEntities++;
		}
	}
	for (int i = 0; i < MAX_BOOMS_L3; i++) {
		if (roadBoomsL3[i].active) {
			entities[totalEntities].entityType = 7;
			entities[totalEntities].index = i;
			entities[totalEntities].y = roadBoomsL3[i].y;
			totalEntities++;
		}
	}
	for (int i = 0; i < MAX_BULLET_PICKUPS_L3; i++) {
		if (bulletPickupsL3[i].active) {
			entities[totalEntities].entityType = 8;
			entities[totalEntities].index = i;
			entities[totalEntities].y = bulletPickupsL3[i].y;
			totalEntities++;
		}
	}

	for (int i = 0; i < totalEntities - 1; i++) {
		for (int j = i + 1; j < totalEntities; j++) {
			if (entities[i].y < entities[j].y) {
				SceneEntityL3 tmp = entities[i];
				entities[i] = entities[j];
				entities[j] = tmp;
			}
		}
	}

	// Layer 2: Entities behind player rickshaw (y >= rickshawYL3)
	for (int i = 0; i < totalEntities; i++) {
		if (entities[i].y >= rickshawYL3) {
			int idx = entities[i].index;
			if (entities[i].entityType == 0) {
				double cx = getEntityCenterXL3(busesL3[idx].normX, busesL3[idx].y);
				double scale = getPerspectiveScaleL3(busesL3[idx].y);
				drawRealisticBusL3(cx, busesL3[idx].y, scale, busesL3[idx].colorTheme);
			}
			else if (entities[i].entityType == 1) {
				double cx = getEntityCenterXL3(peopleL3[idx].normX, peopleL3[idx].y);
				double scale = getPerspectiveScaleL3(peopleL3[idx].y);
				drawRealisticPersonL3(cx, peopleL3[idx].y, scale, peopleL3[idx].walkAnim, peopleL3[idx].outfitTheme, peopleL3[idx].umbrellaColor, peopleL3[idx].gender);
			}
			else if (entities[i].entityType == 2) {
				double cx = getEntityCenterXL3(dogsL3[idx].normX, dogsL3[idx].y);
				double scale = getPerspectiveScaleL3(dogsL3[idx].y);
				drawRealisticDogL3(cx, dogsL3[idx].y, scale, dogsL3[idx].trotAnim, dogsL3[idx].coatColor);
			}
			else if (entities[i].entityType == 3) {
				double cx = getEntityCenterXL3(otherRickshawsL3[idx].normX, otherRickshawsL3[idx].y);
				double scale = getPerspectiveScaleL3(otherRickshawsL3[idx].y);
				drawRealisticRickshawL3(cx, otherRickshawsL3[idx].y, scale, otherRickshawsL3[idx].pedalAnim, otherRickshawsL3[idx].hoodColor);
			}
			else if (entities[i].entityType == 4) {
				if (currentPathModeL3 == 0) {
					drawRoadsidePropL3(idx);
				}
			}
			else if (entities[i].entityType == 5) {
				double cx = getEntityCenterXL3(coinsL3[idx].normX, coinsL3[idx].y);
				double scale = getPerspectiveScaleL3(coinsL3[idx].y);
				drawCoinL3(cx, coinsL3[idx].y, scale, coinsL3[idx].isSpecial, coinsL3[idx].spinAnim);
			}
			else if (entities[i].entityType == 6) {
				double cx = getEntityCenterXL3(roadStonesL3[idx].normX, roadStonesL3[idx].y);
				double scale = getPerspectiveScaleL3(roadStonesL3[idx].y);
				drawRealisticStoneL3(cx, roadStonesL3[idx].y, scale, roadStonesL3[idx].variant);
			}
			else if (entities[i].entityType == 7) {
				double cx = getEntityCenterXL3(roadBoomsL3[idx].normX, roadBoomsL3[idx].y);
				double scale = getPerspectiveScaleL3(roadBoomsL3[idx].y);
				drawBoomL3(cx, roadBoomsL3[idx].y, scale, roadBoomsL3[idx].pulseAnim);
			}
			else if (entities[i].entityType == 8) {
				double cx = getEntityCenterXL3(bulletPickupsL3[idx].normX, bulletPickupsL3[idx].y);
				double scale = getPerspectiveScaleL3(bulletPickupsL3[idx].y);
				drawRocketBulletL3(cx, bulletPickupsL3[idx].y + 10.0 * scale, scale, 55.0f, 0, bulletPickupsL3[idx].animPhase);
			}
		}
	}

	// Layer 3: Player Rickshaw (With invulnerability blink)
	if (invulnerableTimerL3 == 0 || (invulnerableTimerL3 / 4) % 2 == 0) {
		iShowBMP2(rickshawXL3, rickshawYL3, getEquippedRickshawFrame(rickshawIndexL3), 0);
		drawActiveShieldFX((float)rickshawXL3 + 38.0f, (float)rickshawYL3 + 36.0f, 54.0f);
	}

	// Layer 4: Entities in front of player rickshaw (y < rickshawYL3)
	for (int i = 0; i < totalEntities; i++) {
		if (entities[i].y < rickshawYL3) {
			int idx = entities[i].index;
			if (entities[i].entityType == 0) {
				double cx = getEntityCenterXL3(busesL3[idx].normX, busesL3[idx].y);
				double scale = getPerspectiveScaleL3(busesL3[idx].y);
				drawRealisticBusL3(cx, busesL3[idx].y, scale, busesL3[idx].colorTheme);
			}
			else if (entities[i].entityType == 1) {
				double cx = getEntityCenterXL3(peopleL3[idx].normX, peopleL3[idx].y);
				double scale = getPerspectiveScaleL3(peopleL3[idx].y);
				drawRealisticPersonL3(cx, peopleL3[idx].y, scale, peopleL3[idx].walkAnim, peopleL3[idx].outfitTheme, peopleL3[idx].umbrellaColor, peopleL3[idx].gender);
			}
			else if (entities[i].entityType == 2) {
				double cx = getEntityCenterXL3(dogsL3[idx].normX, dogsL3[idx].y);
				double scale = getPerspectiveScaleL3(dogsL3[idx].y);
				drawRealisticDogL3(cx, dogsL3[idx].y, scale, dogsL3[idx].trotAnim, dogsL3[idx].coatColor);
			}
			else if (entities[i].entityType == 3) {
				double cx = getEntityCenterXL3(otherRickshawsL3[idx].normX, otherRickshawsL3[idx].y);
				double scale = getPerspectiveScaleL3(otherRickshawsL3[idx].y);
				drawRealisticRickshawL3(cx, otherRickshawsL3[idx].y, scale, otherRickshawsL3[idx].pedalAnim, otherRickshaws[idx].hoodColor);
			}
			else if (entities[i].entityType == 4) {
				if (currentPathModeL3 == 0) {
					drawRoadsidePropL3(idx);
				}
			}
			else if (entities[i].entityType == 5) {
				double cx = getEntityCenterXL3(coinsL3[idx].normX, coinsL3[idx].y);
				double scale = getPerspectiveScaleL3(coinsL3[idx].y);
				drawCoinL3(cx, coinsL3[idx].y, scale, coinsL3[idx].isSpecial, coinsL3[idx].spinAnim);
			}
			else if (entities[i].entityType == 6) {
				double cx = getEntityCenterXL3(roadStonesL3[idx].normX, roadStonesL3[idx].y);
				double scale = getPerspectiveScaleL3(roadStonesL3[idx].y);
				drawRealisticStoneL3(cx, roadStonesL3[idx].y, scale, roadStonesL3[idx].variant);
			}
			else if (entities[i].entityType == 7) {
				double cx = getEntityCenterXL3(roadBoomsL3[idx].normX, roadBoomsL3[idx].y);
				double scale = getPerspectiveScaleL3(roadBoomsL3[idx].y);
				drawBoomL3(cx, roadBoomsL3[idx].y, scale, roadBoomsL3[idx].pulseAnim);
			}
			else if (entities[i].entityType == 8) {
				double cx = getEntityCenterXL3(bulletPickupsL3[idx].normX, bulletPickupsL3[idx].y);
				double scale = getPerspectiveScaleL3(bulletPickupsL3[idx].y);
				drawRocketBulletL3(cx, bulletPickupsL3[idx].y + 10.0 * scale, scale, 55.0f, 0, bulletPickupsL3[idx].animPhase);
			}
		}
	}

	// Active Thrown Rocket Bullets Flying Forward
	for (int k = 0; k < MAX_THROWN_BULLETS_L3; k++) {
		if (thrownBulletsL3[k].active) {
			double scale = getPerspectiveScaleL3(thrownBulletsL3[k].y);
			drawRocketBulletL3(thrownBulletsL3[k].x, thrownBulletsL3[k].y, scale, 90.0f, 1, thrownBulletsL3[k].anim);
		}
	}

	drawExplosionBurstsL3();
	drawContinuousRainL3();

	drawBulletsHUDL3();
	drawLivesHUDL3();
	drawCoinsHUDL3();
	drawInGameShieldHUD();

	if (hitFlashTimerL3 > 0) {
		iSetColor(220, 35, 35);
		drawRect(0, 0, SCREEN_WIDTH, 8);
		drawRect(0, SCREEN_HEIGHT - 8, SCREEN_WIDTH, 8);
		drawRect(0, 0, 8, SCREEN_HEIGHT);
		drawRect(SCREEN_WIDTH - 8, 0, 8, SCREEN_HEIGHT);
	}

	if (isGameOverL3 || gameState == STATE_GAMEOVER) {
		iSetColor(12, 15, 20);
		drawRect(SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT / 2 - 90, 440, 180);
		iSetColor(255, 255, 255);
		drawRect(SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT / 2 - 90, 440, 4);
		drawRect(SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT / 2 + 86, 440, 4);
		drawRect(SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT / 2 - 90, 4, 180);
		drawRect(SCREEN_WIDTH / 2 + 216, SCREEN_HEIGHT / 2 - 90, 4, 180);

		iSetColor(255, 55, 55);
		iText(SCREEN_WIDTH / 2 - 68, SCREEN_HEIGHT / 2 + 45, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);

		iSetColor(240, 240, 245);
		iText(SCREEN_WIDTH / 2 - 165, SCREEN_HEIGHT / 2 + 15, "All 5 lives lost! You collided with an obstacle.", GLUT_BITMAP_HELVETICA_12);

		char finalCoins[64];
		sprintf_s(finalCoins, sizeof(finalCoins), "Coins Collected: %d  |  Total: %d", coinCountL3, totalCoins);
		iSetColor(255, 220, 50);
		iText(SCREEN_WIDTH / 2 - 155, SCREEN_HEIGHT / 2 - 15, finalCoins, GLUT_BITMAP_HELVETICA_18);

		iSetColor(255, 215, 30);
		iText(SCREEN_WIDTH / 2 - 157, SCREEN_HEIGHT / 2 - 55, "Press 'R' to Restart   |   Press 'M' for Menu", GLUT_BITMAP_HELVETICA_12);
	}
}

typedef struct {
	int type;
	int index;
	float y;
	float normX;
	float baseSpeed;
	float currentSpeed;
} ActiveObstacleL3;

void onPlayerCollisionL3(int collisionType) {
	if (activeShieldTimer > 0.0f || invulnerableTimerL3 > 0 || isGameOverL3 || gameState == STATE_GAMEOVER) return;

	playerLivesL3--;
	invulnerableTimerL3 = 65;
	hitFlashTimerL3 = 10;

	if (playerLivesL3 <= 0) {
		playerLivesL3 = 0;
		isGameOverL3 = 1;
		gameState = STATE_GAMEOVER;
		sfxTimerL3 = 0;
		PlaySound("Audios\\Gameover.wav", NULL, SND_ASYNC);
	}
	else {
		if (collisionType == 4) {
			PlaySound("Audios\\bomb.wav", NULL, SND_ASYNC);
			sfxTimerL3 = 65;
		}
		else if (collisionType == 1) {
			PlaySound("Audios\\dog_sound.wav", NULL, SND_ASYNC);
			sfxTimerL3 = 75;
		}
		else if (collisionType == 0 || collisionType == 3) {
			PlaySound("Audios\\horn.wav", NULL, SND_ASYNC);
			sfxTimerL3 = 45;
		}
	}
}

void updateGameL3() {
	if (isGameOverL3 || gameState != STATE_PLAYING) return;

	if (activeShieldTimer > 0.0f) {
		activeShieldTimer -= 0.02f;
		if (activeShieldTimer < 0.0f) activeShieldTimer = 0.0f;
	}

	if (invulnerableTimerL3 > 0) invulnerableTimerL3--;
	if (hitFlashTimerL3 > 0) hitFlashTimerL3--;

	if (sfxTimerL3 > 0) {
		sfxTimerL3--;
		if (sfxTimerL3 == 0 && !isGameOverL3 && gameState == STATE_PLAYING) {
			if (musicOn) {
				PlaySound("Audios\\L3_Music.wav", NULL, SND_ASYNC | SND_LOOP);
			}
		}
	}

	SHORT upState = GetAsyncKeyState(VK_UP) | GetAsyncKeyState('W');
	SHORT downState = GetAsyncKeyState(VK_DOWN) | GetAsyncKeyState('S');
	SHORT leftState = GetAsyncKeyState(VK_LEFT) | GetAsyncKeyState('A');
	SHORT rightState = GetAsyncKeyState(VK_RIGHT) | GetAsyncKeyState('D');
	SHORT fireState = GetAsyncKeyState('F') | GetAsyncKeyState(VK_SPACE);
	static int prevFireState = 0;

	int isUpPressed = (upState & 0x8000) ? 1 : 0;
	int isFirePressed = (fireState & 0x8000) ? 1 : 0;
	if (doubleTapTimerL3 > 0) doubleTapTimerL3--;

	int shouldShoot = 0;
	// Detect new UP key press event (0 -> 1 transition) for double-tap shooting
	if (isUpPressed && !prevUpStateL3) {
		if (doubleTapTimerL3 > 0) {
			shouldShoot = 1;
			doubleTapTimerL3 = 0;
		}
		else {
			doubleTapTimerL3 = 18; // ~360ms window for double tap
		}
	}
	// Direct single-tap shooting with 'F' or 'SPACE'
	if (isFirePressed && !prevFireState) {
		shouldShoot = 1;
	}
	prevUpStateL3 = isUpPressed;
	prevFireState = isFirePressed;

	if (shouldShoot && playerBulletsL3 > 0) {
		for (int k = 0; k < MAX_THROWN_BULLETS_L3; k++) {
			if (!thrownBulletsL3[k].active) {
				thrownBulletsL3[k].active = 1;
				thrownBulletsL3[k].x = getBulletSpawnXL3();
				thrownBulletsL3[k].y = getBulletSpawnYL3();
				thrownBulletsL3[k].speed = 12.0f;
				thrownBulletsL3[k].anim = 0.0f;
				playerBulletsL3--;
				saveUserData();
				PlaySound("Audios\\bomb.wav", NULL, SND_ASYNC);
				sfxTimerL3 = 35;
				break;
			}
		}
	}

	if (upState & 0x8000) {
		currentSpeedL3 += accelRateL3;
		if (currentSpeedL3 > maxSpeedL3) currentSpeedL3 = maxSpeedL3;
	}
	else if (downState & 0x8000) {
		currentSpeedL3 -= decelRateL3 * 1.5f;
		if (currentSpeedL3 < baseSpeedL3) currentSpeedL3 = baseSpeedL3;
	}
	else {
		if (currentSpeedL3 > baseSpeedL3) {
			currentSpeedL3 -= decelRateL3 * 0.5f;
			if (currentSpeedL3 < baseSpeedL3) currentSpeedL3 = baseSpeedL3;
		}
	}

	lineOffsetL3 += scrollSpeedL3 * (currentSpeedL3 / baseSpeedL3);
	if (lineOffsetL3 >= 0.2f) lineOffsetL3 -= 0.2f;

	animTimerL3++;
	int pedalFreq = (currentSpeedL3 > 1.8f) ? 1 : 2;
	if (animTimerL3 % pedalFreq == 0) {
		int maxFramesL3 = (equippedRickshaw == 0) ? 8 : 9;
		rickshawIndexL3 = (rickshawIndexL3 + 1) % maxFramesL3;
	}

	cloudOffsetL3 -= (0.35f * currentSpeedL3);
	if (cloudOffsetL3 <= -SCREEN_WIDTH) cloudOffsetL3 += SCREEN_WIDTH;

	riverWaveOffsetL3 += 0.08f;

	for (int i = 0; i < MAX_RAINDROPS_L3; i++) {
		rainDropsL3[i].y -= rainDropsL3[i].speed;
		rainDropsL3[i].x += rainDropsL3[i].slant;

		if (rainDropsL3[i].y < 0.0f) {
			rainDropsL3[i].y = (float)(SCREEN_HEIGHT + (rand() % 30));
			rainDropsL3[i].x = (float)(rand() % (SCREEN_WIDTH + 150) - 50);
		}
	}

	thunderTimerL3++;
	if (thunderTimerL3 >= THUNDER_INTERVAL_L3) {
		thunderTimerL3 = 0;
		thunderFlashTimerL3 = 6;
		lightningBoltActiveL3 = 1;
		lightningForkXL3 = (float)(200 + rand() % 500);
	}
	if (thunderFlashTimerL3 > 0) {
		thunderFlashTimerL3--;
		if (thunderFlashTimerL3 <= 2) {
			lightningBoltActiveL3 = 0;
		}
	}

	pathCycleCounterL3++;
	if (pathCycleCounterL3 >= PATH_CYCLE_INTERVAL_L3) {
		pathCycleCounterL3 = 0;
		currentPathModeL3 = 1 - currentPathModeL3;
	}

	if (leftState & 0x8000) {
		if (rickshawXL3 > 80) rickshawXL3 -= 8;
	}
	if (rightState & 0x8000) {
		if (rickshawXL3 < 720) rickshawXL3 += 8;
	}

	float playerApproachBoost = currentSpeedL3 * 0.95f;

	busSpawnCounterL3++;
	if (busSpawnCounterL3 >= BUS_SPAWN_INTERVAL_L3) {
		spawnBusL3();
		busSpawnCounterL3 = 0;
	}

	rickshawSpawnCounterL3++;
	if (rickshawSpawnCounterL3 >= RICKSHAW_SPAWN_INTERVAL_L3) {
		spawnTrafficRickshawL3();
		rickshawSpawnCounterL3 = 0;
	}

	stoneSpawnCounterL3++;
	if (stoneSpawnCounterL3 >= STONE_SPAWN_INTERVAL_L3) {
		spawnStoneL3();
		stoneSpawnCounterL3 = 0;
	}

	personSpawnCounterL3++;
	if (personSpawnCounterL3 >= PERSON_SPAWN_INTERVAL_L3) {
		spawnPersonL3();
		personSpawnCounterL3 = 0;
	}

	dogSpawnCounterL3++;
	if (dogSpawnCounterL3 >= DOG_SPAWN_INTERVAL_L3) {
		spawnDogL3();
		dogSpawnCounterL3 = 0;
	}

	normalCoinSpawnCounterL3++;
	if (normalCoinSpawnCounterL3 >= NORMAL_COIN_SPAWN_INTERVAL_L3) {
		spawnNormalCoinL3();
		normalCoinSpawnCounterL3 = 0;
	}

	specialCoinSpawnCounterL3++;
	if (specialCoinSpawnCounterL3 >= SPECIAL_COIN_SPAWN_INTERVAL_L3) {
		spawnSpecialCoinL3();
		specialCoinSpawnCounterL3 = 0;
	}

	for (int i = 0; i < MAX_COINS_L3; i++) {
		if (coinsL3[i].active) {
			double t = (double)(horizonYL3 - coinsL3[i].y) / horizonYL3;
			if (t < 0.0) t = 0.0;
			double perspectiveFactor = 0.35 + 0.85 * t;
			coinsL3[i].y -= (float)(playerApproachBoost * perspectiveFactor);
			coinsL3[i].spinAnim += (coinsL3[i].isSpecial ? 0.09f : 0.12f);

			if (coinsL3[i].y < -40.0f) {
				coinsL3[i].active = 0;
			}
		}
	}

	bulletSpawnCounterL3++;
	if (bulletSpawnCounterL3 >= BULLET_SPAWN_INTERVAL_L3) {
		spawnBulletPickupsL3();
		bulletSpawnCounterL3 = 0;
	}

	for (int i = 0; i < MAX_BULLET_PICKUPS_L3; i++) {
		if (bulletPickupsL3[i].active) {
			double t = (double)(horizonYL3 - bulletPickupsL3[i].y) / horizonYL3;
			if (t < 0.0) t = 0.0;
			double perspectiveFactor = 0.35 + 0.85 * t;
			bulletPickupsL3[i].y -= (float)(playerApproachBoost * perspectiveFactor);
			bulletPickupsL3[i].animPhase += 0.08f;

			if (bulletPickupsL3[i].y < -40.0f) {
				bulletPickupsL3[i].active = 0;
			}
		}
	}

	for (int i = 0; i < MAX_STONES_L3; i++) {
		if (roadStonesL3[i].active) {
			double t = (double)(horizonYL3 - roadStonesL3[i].y) / horizonYL3;
			if (t < 0.0) t = 0.0;
			double perspectiveFactor = 0.35 + 0.85 * t;
			roadStonesL3[i].y -= (float)(playerApproachBoost * perspectiveFactor);

			if (roadStonesL3[i].y < -50.0f) {
				roadStonesL3[i].active = 0;
			}
		}
	}

	boomSpawnCounterL3++;
	if (boomSpawnCounterL3 >= BOOM_SPAWN_INTERVAL_L3) {
		spawnBoomL3();
		boomSpawnCounterL3 = 0;
	}

	for (int i = 0; i < MAX_BOOMS_L3; i++) {
		if (roadBoomsL3[i].active) {
			double t = (double)(horizonYL3 - roadBoomsL3[i].y) / horizonYL3;
			if (t < 0.0) t = 0.0;
			double perspectiveFactor = 0.35 + 0.85 * t;
			roadBoomsL3[i].y -= (float)(playerApproachBoost * perspectiveFactor);
			roadBoomsL3[i].pulseAnim += 0.12f;

			if (roadBoomsL3[i].y < -45.0f) {
				roadBoomsL3[i].active = 0;
			}
		}
	}

	sideAnimTimerL3++;
	for (int i = 0; i < MAX_OUTER_PROPS_L3; i++) {
		if (outerPropsL3[i].active) {
			outerPropsL3[i].replaceCounter--;
			if (outerPropsL3[i].replaceCounter <= 0) {
				outerPropsL3[i].replaceCounter = 170 + rand() % 60;
				outerPropsL3[i].type = (outerPropsL3[i].type + 1 + rand() % 7) % 8;
				outerPropsL3[i].variant = rand() % 3;
				outerPropsL3[i].shopperGender = rand() % 2;
			}
		}
	}

	for (int i = 0; i < MAX_ROADSIDE_PROPS_L3; i++) {
		if (roadsidePropsL3[i].active) {
			double t = (double)(horizonYL3 - roadsidePropsL3[i].y) / horizonYL3;
			if (t < 0.0) t = 0.0;
			double perspectiveFactor = 0.35 + 0.85 * t;
			roadsidePropsL3[i].y -= (float)(playerApproachBoost * perspectiveFactor);

			if (roadsidePropsL3[i].y < -70.0f) {
				roadsidePropsL3[i].y = (float)horizonYL3 + 5.0f;
				roadsidePropsL3[i].variant = rand() % 3;

				int r = rand() % 100;
				if (r < 28) roadsidePropsL3[i].type = (roadsidePropsL3[i].side == 0) ? 2 : 3;
				else if (r < 58) roadsidePropsL3[i].type = (rand() % 2 == 0) ? 0 : 1;
				else if (r < 72) roadsidePropsL3[i].type = 4;
				else if (r < 84) roadsidePropsL3[i].type = 5;
				else if (r < 92) roadsidePropsL3[i].type = 6;
				else roadsidePropsL3[i].type = 7;

				roadsidePropsL3[i].lateralOffset = (roadsidePropsL3[i].type <= 1) ? 1.0f : 0.4f;
			}
		}
	}

	for (int i = 0; i < MAX_PEOPLE_L3; i++) {
		if (peopleL3[i].active) {
			double t = (double)(horizonYL3 - peopleL3[i].y) / horizonYL3;
			if (t < 0.0) t = 0.0;
			double perspectiveFactor = 0.35 + 0.85 * t;
			peopleL3[i].y -= (float)((peopleL3[i].speed + playerApproachBoost) * perspectiveFactor);
			peopleL3[i].walkAnim += 0.20f;
			if (peopleL3[i].y < -150) peopleL3[i].active = 0;
		}
	}
	for (int i = 0; i < MAX_DOGS_L3; i++) {
		if (dogsL3[i].active) {
			double t = (double)(horizonYL3 - dogsL3[i].y) / horizonYL3;
			if (t < 0.0) t = 0.0;
			double dogPerspective = 0.45 + 0.75 * t;
			dogsL3[i].y -= (float)((dogsL3[i].speed + playerApproachBoost) * dogPerspective);
			dogsL3[i].trotAnim += 0.28f;
			if (dogsL3[i].y < -100) dogsL3[i].active = 0;
		}
	}

	ActiveObstacleL3 activeList[MAX_BUSES_L3 + MAX_TRAFFIC_RICKSHAWS_L3];
	int totalActive = 0;

	for (int i = 0; i < MAX_BUSES_L3; i++) {
		if (busesL3[i].active) {
			activeList[totalActive].type = 0;
			activeList[totalActive].index = i;
			activeList[totalActive].y = busesL3[i].y;
			activeList[totalActive].normX = busesL3[i].normX;
			activeList[totalActive].baseSpeed = busesL3[i].speed;
			activeList[totalActive].currentSpeed = busesL3[i].speed;
			totalActive++;
		}
	}
	for (int i = 0; i < MAX_TRAFFIC_RICKSHAWS_L3; i++) {
		if (otherRickshawsL3[i].active) {
			activeList[totalActive].type = 3;
			activeList[totalActive].index = i;
			activeList[totalActive].y = otherRickshawsL3[i].y;
			activeList[totalActive].normX = otherRickshawsL3[i].normX;
			activeList[totalActive].baseSpeed = otherRickshawsL3[i].speed;
			activeList[totalActive].currentSpeed = otherRickshawsL3[i].speed;
			totalActive++;
		}
	}

	for (int i = 0; i < totalActive - 1; i++) {
		for (int j = i + 1; j < totalActive; j++) {
			if (activeList[i].y > activeList[j].y) {
				ActiveObstacleL3 tmp = activeList[i];
				activeList[i] = activeList[j];
				activeList[j] = tmp;
			}
		}
	}

	for (int i = 1; i < totalActive; i++) {
		for (int j = 0; j < i; j++) {
			float wI = getEntityBaseWidthL3(activeList[i].type);
			float wJ = getEntityBaseWidthL3(activeList[j].type);
			float safeSep = (wI + wJ) / 780.0f * 0.5f + 0.040f;

			if ((float)fabs(activeList[i].normX - activeList[j].normX) < safeSep) {
				float gap = activeList[i].y - activeList[j].y;
				double scaleI = getPerspectiveScaleL3(activeList[i].y);
				float hJ = getEntityBaseHeightL3(activeList[j].type);
				float safeGap = (hJ * 0.55f + 25.0f) * (float)scaleI;

				if (gap < safeGap && gap > 0.0f) {
					if (activeList[i].currentSpeed > activeList[j].currentSpeed) {
						activeList[i].currentSpeed = activeList[j].currentSpeed;
					}
					if (gap < safeGap * 0.70f) {
						activeList[i].currentSpeed = activeList[j].currentSpeed * 0.5f;
					}
					if (gap < safeGap * 0.40f) {
						activeList[i].currentSpeed = 0.0f;
					}
				}
			}
		}
	}

	for (int k = 0; k < totalActive; k++) {
		int type = activeList[k].type;
		int idx = activeList[k].index;
		float spd = activeList[k].currentSpeed;

		double t = (double)(horizonYL3 - activeList[k].y) / horizonYL3;
		if (t < 0.0) t = 0.0;
		double perspectiveFactor = 0.35 + 0.85 * t;

		if (type == 0) {
			busesL3[idx].y -= (float)((spd + playerApproachBoost) * perspectiveFactor);
			if (busesL3[idx].y < -220) busesL3[idx].active = 0;
		}
		else if (type == 3) {
			otherRickshawsL3[idx].y -= (float)((spd + playerApproachBoost) * perspectiveFactor);
			otherRickshawsL3[idx].pedalAnim += 0.22f;
			if (otherRickshawsL3[idx].y < -150) otherRickshawsL3[idx].active = 0;
		}
	}

	// Update active thrown bullets and handle obstacle collisions (annihilates obstacles)
	for (int k = 0; k < MAX_THROWN_BULLETS_L3; k++) {
		if (thrownBulletsL3[k].active) {
			thrownBulletsL3[k].y += thrownBulletsL3[k].speed;
			thrownBulletsL3[k].anim += 0.35f;

			double scale = getPerspectiveScaleL3(thrownBulletsL3[k].y);
			float bx = thrownBulletsL3[k].x;
			float by = thrownBulletsL3[k].y;
			float bRadius = (float)(18.0 * scale);
			if (bRadius < 8.0f) bRadius = 8.0f;
			float bL = bx - bRadius;
			float bR = bx + bRadius;
			float bB = by - bRadius;
			float bT = by + bRadius * 2.2f;

			int hitObstacle = 0;

			// 1. Check collision against Buses
			for (int i = 0; i < MAX_BUSES_L3; i++) {
				if (busesL3[i].active) {
					double bScale = getPerspectiveScaleL3(busesL3[i].y);
					double obX = getEntityCenterXL3(busesL3[i].normX, busesL3[i].y);
					float obW = (float)(96.0 * bScale);
					float obH = (float)(130.0 * bScale);
					if (checkAABB(bL, bR, bB, bT, (float)(obX - obW * 0.5), (float)(obX + obW * 0.5), busesL3[i].y, busesL3[i].y + obH)) {
						busesL3[i].active = 0; // Obstacle beaten and vanishes!
						triggerBoomBurstL3(obX, busesL3[i].y + obH * 0.5);
						hitObstacle = 1;
						break;
					}
				}
			}

			// 2. Check collision against Traffic Rickshaws
			if (!hitObstacle) {
				for (int i = 0; i < MAX_TRAFFIC_RICKSHAWS_L3; i++) {
					if (otherRickshawsL3[i].active) {
						double rScale = getPerspectiveScaleL3(otherRickshawsL3[i].y);
						double obX = getEntityCenterXL3(otherRickshawsL3[i].normX, otherRickshawsL3[i].y);
						float obW = (float)(74.0 * rScale);
						float obH = (float)(100.0 * rScale);
						if (checkAABB(bL, bR, bB, bT, (float)(obX - obW * 0.5), (float)(obX + obW * 0.5), otherRickshawsL3[i].y, otherRickshawsL3[i].y + obH)) {
							otherRickshawsL3[i].active = 0; // Obstacle beaten and vanishes!
							triggerBoomBurstL3(obX, otherRickshawsL3[i].y + obH * 0.5);
							hitObstacle = 1;
							break;
						}
					}
				}
			}

			// 3. Check collision against Road Stones
			if (!hitObstacle) {
				for (int i = 0; i < MAX_STONES_L3; i++) {
					if (roadStonesL3[i].active) {
						double sScale = getPerspectiveScaleL3(roadStonesL3[i].y);
						double obX = getEntityCenterXL3(roadStonesL3[i].normX, roadStonesL3[i].y);
						float obW = (float)(36.0 * sScale);
						float obH = (float)(24.0 * sScale);
						if (checkAABB(bL, bR, bB, bT, (float)(obX - obW * 0.5), (float)(obX + obW * 0.5), roadStonesL3[i].y, roadStonesL3[i].y + obH)) {
							roadStonesL3[i].active = 0; // Stone shattered!
							triggerBoomBurstL3(obX, roadStonesL3[i].y + obH * 0.5);
							hitObstacle = 1;
							break;
						}
					}
				}
			}

			// 4. Check collision against Booms
			if (!hitObstacle) {
				for (int i = 0; i < MAX_BOOMS_L3; i++) {
					if (roadBoomsL3[i].active) {
						double bmScale = getPerspectiveScaleL3(roadBoomsL3[i].y);
						double obX = getEntityCenterXL3(roadBoomsL3[i].normX, roadBoomsL3[i].y);
						float obW = (float)(28.0 * bmScale);
						float obH = (float)(28.0 * bmScale);
						if (checkAABB(bL, bR, bB, bT, (float)(obX - obW * 0.5), (float)(obX + obW * 0.5), roadBoomsL3[i].y, roadBoomsL3[i].y + obH)) {
							roadBoomsL3[i].active = 0; // Safely detonated!
							triggerBoomBurstL3(obX, roadBoomsL3[i].y + obH * 0.5);
							hitObstacle = 1;
							break;
						}
					}
				}
			}

			if (hitObstacle) {
				thrownBulletsL3[k].active = 0;
				PlaySound("Audios\\bomb.wav", NULL, SND_ASYNC);
				sfxTimerL3 = 45;
			}
			else if (thrownBulletsL3[k].y >= horizonYL3 + 10.0f) {
				thrownBulletsL3[k].active = 0;
			}
		}
	}

	float pL, pR, pB, pT;
	getPlayerHitbox((float)rickshawXL3, (float)rickshawYL3, &pL, &pR, &pB, &pT);

	for (int i = 0; i < MAX_COINS_L3; i++) {
		if (coinsL3[i].active) {
			double scale = getPerspectiveScaleL3(coinsL3[i].y);
			double cx = getEntityCenterXL3(coinsL3[i].normX, coinsL3[i].y);
			float cRadius = (coinsL3[i].isSpecial ? 21.0f : 14.5f) * (float)scale;
			float cL = (float)cx - cRadius;
			float cR = (float)cx + cRadius;
			float cB = coinsL3[i].y - 5.0f;
			float cT = coinsL3[i].y + cRadius * 2.2f;

			if (checkAABB(pL - 10.0f, pR + 10.0f, pB - 8.0f, pT + 8.0f, cL, cR, cB, cT)) {
				if (coinsL3[i].isSpecial) {
					coinCountL3 += 5;
					currentRunCoins += 5;
					totalCoins += 5;
				}
				else {
					coinCountL3 += 1;
					currentRunCoins += 1;
					totalCoins += 1;
				}
				coinsL3[i].active = 0;
				saveUserData();

				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
				sfxTimerL3 = 40;
			}
		}
	}

	for (int i = 0; i < MAX_BULLET_PICKUPS_L3; i++) {
		if (bulletPickupsL3[i].active) {
			double scale = getPerspectiveScaleL3(bulletPickupsL3[i].y);
			double cx = getEntityCenterXL3(bulletPickupsL3[i].normX, bulletPickupsL3[i].y);
			float bRadius = (float)(14.5 * scale);
			float bL = (float)cx - bRadius;
			float bR = (float)cx + bRadius;
			float bB = bulletPickupsL3[i].y - 5.0f;
			float bT = bulletPickupsL3[i].y + bRadius * 2.2f;

			if (checkAABB(pL - 10.0f, pR + 10.0f, pB - 8.0f, pT + 8.0f, bL, bR, bB, bT)) {
				playerBulletsL3++;
				bulletPickupsL3[i].active = 0;
				saveUserData();
				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
				sfxTimerL3 = 35;
			}
		}
	}

	for (int i = 0; i < MAX_BUSES_L3; i++) {
		if (busesL3[i].active) {
			double scale = getPerspectiveScaleL3(busesL3[i].y);
			double cx = getEntityCenterXL3(busesL3[i].normX, busesL3[i].y);
			float bW = (float)(96.0 * scale);
			float bH = (float)(130.0 * scale);
			float bL = (float)(cx - bW * 0.5);
			float bR = (float)(cx + bW * 0.5);
			float bB = busesL3[i].y;
			float bT = busesL3[i].y + bH;

			if (checkAABB(pL, pR, pB, pT, bL, bR, bB, bT)) {
				if (activeShieldTimer > 0.0f) {
					busesL3[i].active = 0;
					triggerBoomBurstL3(cx, busesL3[i].y + bH * 0.5);
					PlaySound("Audios\\bomb.wav", NULL, SND_ASYNC);
					sfxTimerL3 = 30;
				}
				else {
					onPlayerCollisionL3(0);
					busesL3[i].active = 0;
				}
				break;
			}
		}
	}

	for (int i = 0; i < MAX_TRAFFIC_RICKSHAWS_L3; i++) {
		if (otherRickshawsL3[i].active) {
			double scale = getPerspectiveScaleL3(otherRickshawsL3[i].y);
			double cx = getEntityCenterXL3(otherRickshawsL3[i].normX, otherRickshawsL3[i].y);
			float rW = (float)(74.0 * scale);
			float rH = (float)(100.0 * scale);
			float rL = (float)(cx - rW * 0.5);
			float rR = (float)(cx + rW * 0.5);
			float rB = otherRickshawsL3[i].y;
			float rT = otherRickshawsL3[i].y + rH;

			if (checkAABB(pL, pR, pB, pT, rL, rR, rB, rT)) {
				if (activeShieldTimer > 0.0f) {
					otherRickshawsL3[i].active = 0;
					triggerBoomBurstL3(cx, otherRickshawsL3[i].y + rH * 0.5);
					PlaySound("Audios\\bomb.wav", NULL, SND_ASYNC);
					sfxTimerL3 = 30;
				}
				else {
					onPlayerCollisionL3(0);
					otherRickshawsL3[i].active = 0;
				}
				break;
			}
		}
	}

	for (int i = 0; i < MAX_STONES_L3; i++) {
		if (roadStonesL3[i].active) {
			double scale = getPerspectiveScaleL3(roadStonesL3[i].y);
			double cx = getEntityCenterXL3(roadStonesL3[i].normX, roadStonesL3[i].y);
			float sW = (float)(36.0 * scale);
			float sH = (float)(24.0 * scale);
			float sL = (float)(cx - sW * 0.5);
			float sR = (float)(cx + sW * 0.5);
			float sB = roadStonesL3[i].y;
			float sT = roadStonesL3[i].y + sH;

			if (checkAABB(pL, pR, pB, pT, sL, sR, sB, sT)) {
				if (activeShieldTimer > 0.0f) {
					roadStonesL3[i].active = 0;
					triggerBoomBurstL3(cx, roadStonesL3[i].y + sH * 0.5);
					PlaySound("Audios\\bomb.wav", NULL, SND_ASYNC);
					sfxTimerL3 = 30;
				}
				else {
					onPlayerCollisionL3(3);
					roadStonesL3[i].active = 0;
				}
				break;
			}
		}
	}

	for (int i = 0; i < MAX_BOOMS_L3; i++) {
		if (roadBoomsL3[i].active) {
			double scale = getPerspectiveScaleL3(roadBoomsL3[i].y);
			double cx = getEntityCenterXL3(roadBoomsL3[i].normX, roadBoomsL3[i].y);
			float boomRadius = 14.5f * (float)scale;
			float bW = boomRadius * 1.8f;
			float bH = boomRadius * 1.8f;
			float bL = (float)(cx - bW * 0.5f);
			float bR = (float)(cx + bW * 0.5f);
			float bB = roadBoomsL3[i].y;
			float bT = roadBoomsL3[i].y + bH;

			if (checkAABB(pL, pR, pB, pT, bL, bR, bB, bT)) {
				triggerBoomBurstL3(cx, roadBoomsL3[i].y + boomRadius);
				if (activeShieldTimer > 0.0f) {
					roadBoomsL3[i].active = 0;
					PlaySound("Audios\\bomb.wav", NULL, SND_ASYNC);
					sfxTimerL3 = 30;
				}
				else {
					onPlayerCollisionL3(4);
					roadBoomsL3[i].active = 0;
				}
				break;
			}
		}
	}

	for (int i = 0; i < MAX_PEOPLE_L3; i++) {
		if (peopleL3[i].active) {
			double scale = getPerspectiveScaleL3(peopleL3[i].y);
			double cx = getEntityCenterXL3(peopleL3[i].normX, peopleL3[i].y);
			float hW = (float)(32.0 * scale);
			float hH = (float)(100.0 * scale);
			float hL = (float)(cx - hW * 0.5);
			float hR = (float)(cx + hW * 0.5);
			float hB = peopleL3[i].y;
			float hT = peopleL3[i].y + hH;

			if (checkAABB(pL, pR, pB, pT, hL, hR, hB, hT)) {
				if (activeShieldTimer > 0.0f) {
					peopleL3[i].active = 0;
					PlaySound("Audios\\bomb.wav", NULL, SND_ASYNC);
					sfxTimerL3 = 30;
				}
				else {
					onPlayerCollisionL3(2);
					peopleL3[i].active = 0;
				}
				break;
			}
		}
	}

	for (int i = 0; i < MAX_DOGS_L3; i++) {
		if (dogsL3[i].active) {
			double scale = getPerspectiveScaleL3(dogsL3[i].y);
			double cx = getEntityCenterXL3(dogsL3[i].normX, dogsL3[i].y);
			float dW = (float)(26.0 * scale);
			float dH = (float)(35.0 * scale);
			float dL = (float)(cx - dW * 0.5);
			float dR = (float)(cx + dW * 0.5);
			float dB = dogsL3[i].y;
			float dT = dogsL3[i].y + dH;

			if (checkAABB(pL, pR, pB, pT, dL, dR, dB, dT)) {
				if (activeShieldTimer > 0.0f) {
					dogsL3[i].active = 0;
					PlaySound("Audios\\bomb.wav", NULL, SND_ASYNC);
					sfxTimerL3 = 30;
				}
				else {
					onPlayerCollisionL3(1);
					dogsL3[i].active = 0;
				}
				break;
			}
		}
	}
}


// =========================================================================
// RESET GAME / STAGE
// =========================================================================
void resetGame() {
	levelTimeRemaining = LEVEL_TIME_LIMIT;
	currentRunCoins = 0;
	activeShieldTimer = 0.0f; // Shield stays ready in hasEnergyShield until manually activated by user in level
	if (selectedLevel == 3) {
		initGameObjectsL3();
		gameState = STATE_PLAYING;
		updateAudioState();
		return;
	}
	if (selectedLevel == 2) {
		initGameObjectsL2();
		gameState = STATE_PLAYING;
		updateAudioState();
		return;
	}
	rickshawCordinateX = 425;
	rickshawCordinateY = 15;
	lineOffset = 0.0f;
	horizonY = 380;
	currentSpeed = 1.0f;
	minSpeed = 0.5f;
	maxSpeed = 2.5f;
	isDriving = 1;
	initObstacles();
	initCoins();
	gameState = STATE_PLAYING;
	updateAudioState();
}

// =========================================================================
// MAIN RENDER (iDraw)
// =========================================================================
void iDraw() {
	iClear();
	if (gameState == STATE_GARAGE) {
		drawGarageScreen();
		return;
	}
	if (gameState == STATE_TITLE) {
		drawTitleScreen();
		return;
	}
	if (gameState == STATE_STORY) {
		drawStoryScreen();
		return;
	}
	if (gameState == STATE_MENU) {
		drawMenuScreen();
		return;
	}
	if (gameState == STATE_VICTORY) {
		drawVictoryScreen();
		return;
	}
	if (selectedLevel == 3) {
		drawLevel3Scene();
		return;
	}
	if (selectedLevel == 2) {
		drawLevel2Scene();
		return;
	}

	// Level 1 Gameplay
	drawLevel1Scene();
	for (int i = 0; i < MAX_COINS; i++) drawCoinL1(coins[i]);
	for (int i = 0; i < MAX_OBSTACLES; i++) drawObstacleVehicle(obstacles[i]);
	iShowBMP2(rickshawCordinateX, rickshawCordinateY, getEquippedRickshawFrame(rickshawIndex), 0);
	drawActiveShieldFX((float)rickshawCordinateX + 25.0f, (float)rickshawCordinateY + 30.0f, 50.0f);

	// Level 1 HUD
	iSetColor(0, 0, 0);
	iFilledRectangle(15, SCREEN_HEIGHT - 45, 220, 32);
	iSetColor(0, 200, 255);
	iRectangle(15, SCREEN_HEIGHT - 45, 220, 32);
	iSetColor(255, 255, 255);
	iText(25, SCREEN_HEIGHT - 33, "Station - Basabo | [M] Menu", GLUT_BITMAP_HELVETICA_12);

	drawDestinationTimerHUD();
	drawInGameShieldHUD();

	char coinHUD[60];
	sprintf_s(coinHUD, sizeof(coinHUD), "Coins: %d", currentRunCoins);
	iSetColor(0, 0, 0);
	iText(SCREEN_WIDTH - 150, SCREEN_HEIGHT - 35, coinHUD, GLUT_BITMAP_HELVETICA_18);

	if (gameState == STATE_GAMEOVER) {
		drawGameOverScreen();
	}
}

// =========================================================================
// MAIN LOGIC UPDATE
// =========================================================================
void updateGame() {
	if (gameState == STATE_GAMEOVER || gameState == STATE_VICTORY) {
		if (GetAsyncKeyState('R') & 0x8000) {
			resetGame();
			return;
		}
		else if (GetAsyncKeyState('M') & 0x8000) {
			gameState = STATE_MENU;
			updateAudioState();
			return;
		}
		return;
	}

	static bool key1PressedPrev = false;
	static bool key2PressedPrev = false;
	static bool key3PressedPrev = false;
	static bool keyBPressedPrev = false;
	static bool keySPressedPrev = false;
	static bool keyMPressedPrev = false;

	if (gameState == STATE_GARAGE) {
		bool k1 = ((GetAsyncKeyState('1') & 0x8000) != 0) || ((GetAsyncKeyState(VK_NUMPAD1) & 0x8000) != 0);
		bool k2 = ((GetAsyncKeyState('2') & 0x8000) != 0) || ((GetAsyncKeyState(VK_NUMPAD2) & 0x8000) != 0);
		bool k3 = ((GetAsyncKeyState('3') & 0x8000) != 0) || ((GetAsyncKeyState(VK_NUMPAD3) & 0x8000) != 0);
		bool kB = ((GetAsyncKeyState('B') & 0x8000) != 0);
		bool kS = ((GetAsyncKeyState('S') & 0x8000) != 0) || ((GetAsyncKeyState('4') & 0x8000) != 0) || ((GetAsyncKeyState(VK_NUMPAD4) & 0x8000) != 0);
		bool kM = ((GetAsyncKeyState('M') & 0x8000) != 0) || ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0);

		if (k1 && !key1PressedPrev) {
			equippedRickshaw = 0;
			saveUserData();
			PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
		}
		if (k2 && !key2PressedPrev) {
			if (ownedRickshaws[1]) {
				equippedRickshaw = 1;
				saveUserData();
				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
			}
			else if (totalCoins >= yellowRickshawCost) {
				totalCoins -= yellowRickshawCost;
				ownedRickshaws[1] = 1;
				hasYellowRickshaw = 1;
				equippedRickshaw = 1;
				saveUserData();
				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
			}
		}
		if (k3 && !key3PressedPrev) {
			if (ownedRickshaws[2]) {
				equippedRickshaw = 2;
				saveUserData();
				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
			}
			else if (totalCoins >= greenRickshawCost) {
				totalCoins -= greenRickshawCost;
				ownedRickshaws[2] = 1;
				equippedRickshaw = 2;
				saveUserData();
				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
			}
		}
		if (kB && !keyBPressedPrev) {
			if (!ownedRickshaws[1] && totalCoins >= yellowRickshawCost) {
				totalCoins -= yellowRickshawCost;
				ownedRickshaws[1] = 1;
				hasYellowRickshaw = 1;
				equippedRickshaw = 1;
				saveUserData();
				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
			}
			else if (!ownedRickshaws[2] && totalCoins >= greenRickshawCost) {
				totalCoins -= greenRickshawCost;
				ownedRickshaws[2] = 1;
				equippedRickshaw = 2;
				saveUserData();
				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
			}
		}
		if (kS && !keySPressedPrev) {
			if (!hasEnergyShield && totalCoins >= energyShieldCost) {
				totalCoins -= energyShieldCost;
				hasEnergyShield = 1;
				saveUserData();
				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
			}
		}
		if (kM && !keyMPressedPrev) {
			gameState = STATE_MENU;
			updateAudioState();
		}

		key1PressedPrev = k1;
		key2PressedPrev = k2;
		key3PressedPrev = k3;
		keyBPressedPrev = kB;
		keySPressedPrev = kS;
		keyMPressedPrev = kM;
		return;
	}
	else {
		key1PressedPrev = false;
		key2PressedPrev = false;
		key3PressedPrev = false;
		keyBPressedPrev = false;
		keySPressedPrev = false;
		keyMPressedPrev = false;
	}

	if (gameState != STATE_PLAYING) return;

	if (selectedLevel == 3) {
		updateGameL3();
		return;
	}
	if (selectedLevel == 2) {
		updateGameL2();
		return;
	}

	if (activeShieldTimer > 0.0f) {
		activeShieldTimer -= 0.02f;
		if (activeShieldTimer < 0.0f) activeShieldTimer = 0.0f;
	}

	// Level 1 Countdown Timer
	levelTimeRemaining -= 0.02f;
	if (levelTimeRemaining <= 0.0f) {
		levelTimeRemaining = 0.0f;
		gameState = STATE_VICTORY;
		totalCoins += AGREED_FARE;
		currentRunCoins += AGREED_FARE;
		saveUserData();
		updateAudioState();
		return;
	}

	if (unlockNotificationTimer > 0) unlockNotificationTimer--;

	if ((GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState('W') & 0x8000)) {
		if (currentSpeed < maxSpeed) currentSpeed += acceleration;
	}
	else if ((GetAsyncKeyState(VK_DOWN) & 0x8000) || (GetAsyncKeyState('S') & 0x8000)) {
		if (currentSpeed > minSpeed) currentSpeed -= acceleration;
	}

	lineOffset += baseScrollSpeed * currentSpeed;
	if (lineOffset >= 1.0f) lineOffset -= 1.0f;

	animTimer++;
	if (animTimer >= (int)(6.0f / currentSpeed)) {
		int maxFramesL1 = (equippedRickshaw == 0) ? 8 : 9;
		rickshawIndex = (rickshawIndex + 1) % maxFramesL1;
		animTimer = 0;
	}

	if ((GetAsyncKeyState(VK_LEFT) & 0x8000) || (GetAsyncKeyState('A') & 0x8000)) {
		if (rickshawCordinateX > 75) rickshawCordinateX -= 10;
	}
	if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) || (GetAsyncKeyState('D') & 0x8000)) {
		if (rickshawCordinateX < 780) rickshawCordinateX += 10;
	}

	// Accurate Level 1 Player Hitbox
	float pL1_L, pL1_B, pL1_W, pL1_H;
	getPlayerHitboxL1((float)rickshawCordinateX, (float)rickshawCordinateY, &pL1_L, &pL1_B, &pL1_W, &pL1_H);

	coinSpawnTimer++;
	if (coinSpawnTimer > 35) {
		for (int i = 0; i < MAX_COINS; i++) {
			if (!coins[i].active) {
				coins[i].active = 1;
				coins[i].y = (float)horizonY;
				coins[i].x = 330 + (rand() % 240);
				coins[i].radius = 8.0f;
				break;
			}
		}
		coinSpawnTimer = 0;
	}

	for (int i = 0; i < MAX_COINS; i++) {
		if (!coins[i].active) continue;
		coins[i].y -= (coins[i].speed * currentSpeed);
		float centerDist = coins[i].x - (SCREEN_WIDTH / 2.0f);
		coins[i].x += centerDist * 0.005f * currentSpeed;
		float progress = (horizonY - coins[i].y) / (float)horizonY;
		if (progress > 0) coins[i].radius = 8.0f + progress * 10.0f;
		if (coins[i].y < -30) resetCoin(i);
		if (checkCoinCollection(pL1_L, pL1_B, pL1_W, pL1_H, coins[i].x, coins[i].y, coins[i].radius)) {
			currentRunCoins++;
			totalCoins++;
			coins[i].active = 0;
			if (!level2Unlocked && totalCoins >= 10) {
				level2Unlocked = 1;
				unlockNotificationTimer = 110;
			}
			saveUserData();
			playCoinSound();
		}
	}

	obstacleSpawnTimer++;
	if (obstacleSpawnTimer > 40) {
		for (int i = 0; i < MAX_OBSTACLES; i++) {
			if (!obstacles[i].active) {
				obstacles[i].active = 1;
				obstacles[i].y = (float)horizonY;
				obstacles[i].x = 300 + (rand() % 300);
				obstacles[i].type = rand() % 4;
				break;
			}
		}
		obstacleSpawnTimer = 0;
	}

	for (int i = 0; i < MAX_OBSTACLES; i++) {
		if (!obstacles[i].active) continue;
		obstacles[i].y -= (obstacles[i].speed * (0.6f + currentSpeed * 0.5f));
		float centerDist = obstacles[i].x - (SCREEN_WIDTH / 2.0f);
		obstacles[i].x += centerDist * 0.005f * currentSpeed;
		if (obstacles[i].y < -130) resetObstacle(i);

		float t = (horizonY - obstacles[i].y) / (float)horizonY;
		if (t < 0.0f) t = 0.0f;
		if (t > 1.05f) t = 1.05f;
		float scale = 0.45f + 0.55f * t;

		float curW = 90.0f * scale;
		float curH = 80.0f * scale;
		if (obstacles[i].type == 0) { // Double Decker Bus
			curW = 120.0f * scale;
			curH = 155.0f * scale;
		}
		else if (obstacles[i].type == 1) { // White Microbus
			curW = 96.0f * scale;
			curH = 95.0f * scale;
		}
		else if (obstacles[i].type == 2) { // Yellow Taxi
			curW = 90.0f * scale;
			curH = 75.0f * scale;
		}
		else { // Sleek Private Car
			curW = 88.0f * scale;
			curH = 76.0f * scale;
		}
		float hitW = curW * 0.80f;
		float hitH = curH * 0.75f;
		float vehicleLeft = obstacles[i].x - (hitW / 2.0f);
		float vehicleBottom = obstacles[i].y + 4.0f * scale;

		if (checkCollision(pL1_L, pL1_B, pL1_W, pL1_H, vehicleLeft, vehicleBottom, hitW, hitH)) {
			if (activeShieldTimer > 0.0f) {
				obstacles[i].active = 0;
				PlaySound("Audios\\bomb.wav", NULL, SND_ASYNC);
			}
			else {
				gameState = STATE_GAMEOVER;
				updateAudioState();
			}
		}
	}
}
void iMouseMove(int mx, int my) { mouseX = mx; mouseY = my; }
void iPassiveMouseMove(int mx, int my) { mouseX = mx; mouseY = my; }

void iMouse(int button, int state, int mx, int my) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (gameState == STATE_PLAYING) {
			int btnX = 15, btnY = 485, btnW = 185, btnH = 50;
			if (mx >= btnX && mx <= btnX + btnW && my >= btnY && my <= btnY + btnH) {
				if (hasEnergyShield && activeShieldTimer <= 0.0f) {
					activateEnergyShieldInGame();
					return;
				}
			}
		}

		if (gameState == STATE_TITLE) {
			gameState = STATE_STORY;
			storyStep = 0;
			updateAudioState();
			return;
		}

		if (gameState == STATE_STORY) {
			if (storyStep < 4 && mx >= SCREEN_WIDTH - 130 && mx <= SCREEN_WIDTH - 30 && my >= 25 && my <= 61) {
				storyStep = 4;
				return;
			}
			if (storyStep < 4) {
				storyStep++;
			}
			else {
				selectedLevel = 1;
				resetGame();
			}
			return;
		}

		if (gameState == STATE_MENU) {
			if (mx >= 45 && mx <= 325 && my >= 380 && my <= 475) selectedLevel = 1;
			if (mx >= 45 && mx <= 325 && my >= 265 && my <= 360) selectedLevel = 2;
			if (mx >= 45 && mx <= 325 && my >= 150 && my <= 245) {
				selectedLevel = 3;
				if (!level3Unlocked && totalCoins >= level3UnlockCost) {
					totalCoins -= level3UnlockCost;
					level3Unlocked = 1;
					saveUserData();
					PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
					return;
				}
			}
			if (mx >= 45 && mx <= 325 && my >= 60 && my <= 115) {
				gameState = STATE_STORY;
				storyStep = 0;
				updateAudioState();
				return;
			}
			// Garage button click
			if (mx >= 45 && mx <= 325 && my >= 8 && my <= 52) {
				gameState = STATE_GARAGE;
				return;
			}
			// Start trip button
			if (selectedLevel == 1 || (selectedLevel == 2 && level2Unlocked) || (selectedLevel == 3 && level3Unlocked)) {
				int playBtnX = 365 + 490 - 195, playBtnY = 60 + 20, playBtnW = 180, playBtnH = 55;
				if (mx >= playBtnX && mx <= playBtnX + playBtnW && my >= playBtnY && my <= playBtnY + playBtnH) {
					resetGame();
				}
			}
			else if (selectedLevel == 3 && !level3Unlocked && totalCoins >= level3UnlockCost) {
				int playBtnX = 365 + 490 - 215, playBtnY = 60 + 20, playBtnW = 200, playBtnH = 55;
				if (mx >= playBtnX && mx <= playBtnX + playBtnW && my >= playBtnY && my <= playBtnY + playBtnH) {
					totalCoins -= level3UnlockCost;
					level3Unlocked = 1;
					saveUserData();
					PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
					return;
				}
			}
			return;
		}

		if (gameState == STATE_GARAGE) {
			// Back to Menu
			if (mx >= SCREEN_WIDTH / 2 - 120 && mx <= SCREEN_WIDTH / 2 + 120 && my >= 25 && my <= 70) {
				gameState = STATE_MENU;
				updateAudioState();
				return;
			}
			// Card 0: Classic Blue Rickshaw (Default)
			if (mx >= 35 && mx <= 295 && my >= 100 && my <= 505) {
				equippedRickshaw = 0;
				saveUserData();
				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
				return;
			}
			// Card 1: Deluxe Yellow Rickshaw
			if (mx >= 320 && mx <= 580 && my >= 100 && my <= 505) {
				if (!ownedRickshaws[1]) {
					if (totalCoins >= yellowRickshawCost) {
						totalCoins -= yellowRickshawCost;
						ownedRickshaws[1] = 1;
						hasYellowRickshaw = 1;
						equippedRickshaw = 1;
						saveUserData();
						PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
						return;
					}
				}
				else {
					equippedRickshaw = 1;
					saveUserData();
					PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
					return;
				}
			}
			// Card 2: Super Green Rickshaw
			if (mx >= 605 && mx <= 865 && my >= 100 && my <= 505) {
				if (!ownedRickshaws[2]) {
					if (totalCoins >= greenRickshawCost) {
						totalCoins -= greenRickshawCost;
						ownedRickshaws[2] = 1;
						equippedRickshaw = 2;
						saveUserData();
						PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
						return;
					}
				}
				else {
					equippedRickshaw = 2;
					saveUserData();
					PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
					return;
				}
			}
			// Titan Energy Shield Power-Up Banner click
			if (mx >= 285 && mx <= 865 && my >= 16 && my <= 70) {
				if (!hasEnergyShield && totalCoins >= energyShieldCost) {
					totalCoins -= energyShieldCost;
					hasEnergyShield = 1;
					saveUserData();
					PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
					return;
				}
			}
			return;
		}

		if (gameState == STATE_VICTORY) {
			int rBtnX = SCREEN_WIDTH / 2 - 190, rBtnY = SCREEN_HEIGHT / 2 - 140 + 30;
			int mBtnX = SCREEN_WIDTH / 2 + 20, mBtnY = SCREEN_HEIGHT / 2 - 140 + 30;
			if (mx >= rBtnX && mx <= rBtnX + 170 && my >= rBtnY && my <= rBtnY + 45) {
				resetGame();
				return;
			}
			if (mx >= mBtnX && mx <= mBtnX + 170 && my >= mBtnY && my <= mBtnY + 45) {
				gameState = STATE_MENU;
				updateAudioState();
				return;
			}
			return;
		}

		if (gameState == STATE_GAMEOVER) {
			int rBtnX = SCREEN_WIDTH / 2 - 170, rBtnY = SCREEN_HEIGHT / 2 - 50;
			int mBtnX = SCREEN_WIDTH / 2 + 20, mBtnY = SCREEN_HEIGHT / 2 - 50;
			if (mx >= rBtnX && mx <= rBtnX + 150 && my >= rBtnY && my <= rBtnY + 45) {
				resetGame();
				return;
			}
			if (mx >= mBtnX && mx <= mBtnX + 150 && my >= mBtnY && my <= mBtnY + 45) {
				gameState = STATE_MENU;
				updateAudioState();
				return;
			}
			return;
		}
	}
}
void iKeyboard(unsigned char key) {
	if (gameState == STATE_TITLE) {
		if (key == '\r' || key == ' ') {
			gameState = STATE_STORY;
			storyStep = 0;
			updateAudioState();
		}
	}
	else if (gameState == STATE_STORY) {
		if (key == '\r' || key == ' ') {
			if (storyStep < 4) storyStep++;
			else {
				selectedLevel = 1;
				resetGame();
			}
		}
		if (key == 's' || key == 'S') storyStep = 4;
		if (key == 'm' || key == 'M') {
			gameState = STATE_MENU;
			updateAudioState();
		}
		if (key == 27) {
			gameState = STATE_TITLE;
			updateAudioState();
		}
	}
	else if (gameState == STATE_MENU) {
		if (key == '1') selectedLevel = 1;
		if (key == '2') selectedLevel = 2;
		if (key == '3') selectedLevel = 3;
		if (key == 'g' || key == 'G') {
			gameState = STATE_GARAGE;
		}
		if ((key == 'u' || key == 'U') && selectedLevel == 3 && !level3Unlocked && totalCoins >= level3UnlockCost) {
			totalCoins -= level3UnlockCost;
			level3Unlocked = 1;
			saveUserData();
			PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
		}
		if (key == '\r') {
			if (selectedLevel == 1 || (selectedLevel == 2 && level2Unlocked) || (selectedLevel == 3 && level3Unlocked)) {
				resetGame();
			}
			else if (selectedLevel == 3 && !level3Unlocked && totalCoins >= level3UnlockCost) {
				totalCoins -= level3UnlockCost;
				level3Unlocked = 1;
				saveUserData();
				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
			}
		}
		if (key == 27) {
			gameState = STATE_STORY;
			storyStep = 4;
			updateAudioState();
		}
	}
	else if (gameState == STATE_GARAGE) {
		if (key == 'm' || key == 'M' || key == 27) {
			gameState = STATE_MENU;
			updateAudioState();
		}
		if (key == '1') {
			equippedRickshaw = 0;
			saveUserData();
			PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
		}
		if (key == '2') {
			if (ownedRickshaws[1]) {
				equippedRickshaw = 1;
				saveUserData();
				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
			}
			else if (totalCoins >= yellowRickshawCost) {
				totalCoins -= yellowRickshawCost;
				ownedRickshaws[1] = 1;
				hasYellowRickshaw = 1;
				equippedRickshaw = 1;
				saveUserData();
				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
			}
		}
		if (key == '3') {
			if (ownedRickshaws[2]) {
				equippedRickshaw = 2;
				saveUserData();
				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
			}
			else if (totalCoins >= greenRickshawCost) {
				totalCoins -= greenRickshawCost;
				ownedRickshaws[2] = 1;
				equippedRickshaw = 2;
				saveUserData();
				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
			}
		}
		if (key == 'b' || key == 'B') {
			if (!ownedRickshaws[1] && totalCoins >= yellowRickshawCost) {
				totalCoins -= yellowRickshawCost;
				ownedRickshaws[1] = 1;
				hasYellowRickshaw = 1;
				equippedRickshaw = 1;
				saveUserData();
				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
			}
			else if (!ownedRickshaws[2] && totalCoins >= greenRickshawCost) {
				totalCoins -= greenRickshawCost;
				ownedRickshaws[2] = 1;
				equippedRickshaw = 2;
				saveUserData();
				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
			}
		}
		if (key == 's' || key == 'S' || key == '4') {
			if (!hasEnergyShield && totalCoins >= energyShieldCost) {
				totalCoins -= energyShieldCost;
				hasEnergyShield = 1;
				saveUserData();
				PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
			}
		}
	}
	else if (gameState == STATE_PLAYING || gameState == STATE_GAMEOVER || gameState == STATE_VICTORY) {
		if (key == 'r' || key == 'R') {
			resetGame();
		}
		if (key == 'm' || key == 'M' || key == 27) {
			gameState = STATE_MENU;
			updateAudioState();
		}
		if (gameState == STATE_PLAYING && (key == 'e' || key == 'E' || key == 's' || key == 'S')) {
			if (hasEnergyShield && activeShieldTimer <= 0.0f) {
				activateEnergyShieldInGame();
			}
		}
		if (gameState == STATE_PLAYING && selectedLevel == 3 && (key == 'f' || key == 'F' || key == ' ')) {
			if (playerBulletsL3 > 0) {
				for (int b = 0; b < MAX_THROWN_BULLETS_L3; b++) {
					if (!thrownBulletsL3[b].active) {
						thrownBulletsL3[b].active = 1;
						thrownBulletsL3[b].x = getBulletSpawnXL3();
						thrownBulletsL3[b].y = getBulletSpawnYL3();
						thrownBulletsL3[b].speed = 10.0f;
						thrownBulletsL3[b].anim = 0.0f;
						playerBulletsL3--;
						saveUserData();
						PlaySound("Audios\\CoinCollect.wav", NULL, SND_ASYNC);
						sfxTimerL3 = 30;
						break;
					}
				}
			}
		}
	}
}
void iSpecialKeyboard(unsigned char key) {}

void fixedUpdate() {}

int main() {
	srand((unsigned int)time(NULL));
	loadUserData();
	initObstacles();
	initCoins();
	updateAudioState();
	iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Tesla-Ride");
	iSetTimer(20, updateGame);
	iStart();
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	return main();
}
