#ifndef MENU_H
#define MENU_H

#include "common.h"
#include "sound.h"
#include "bg_level1.h"
#include "bg_level2.h"
#include "bg_level3.h"
#include "level1.h"
#include "level2.h"
#include "level3.h"

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
#endif // MENU_H
