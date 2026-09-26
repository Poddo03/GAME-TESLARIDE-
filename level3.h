#ifndef LEVEL3_H
#define LEVEL3_H

#include "common.h"
#include "bg_level3.h"
#include "sound.h"


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
void drawCustomShieldIcon(float cx, float cy, float scale, bool isGlowing) {
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
#endif // LEVEL3_H
