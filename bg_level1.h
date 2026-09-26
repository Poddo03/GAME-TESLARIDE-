#ifndef BG_LEVEL1_H
#define BG_LEVEL1_H

#include "common.h"


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

#endif // BG_LEVEL1_H
