#ifndef LEVEL2_H
#define LEVEL2_H

#include "common.h"
#include "bg_level2.h"
#include "sound.h"

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
#endif // LEVEL2_H
