#ifndef LEVEL1_H
#define LEVEL1_H

#include "common.h"
#include "bg_level1.h"
#include "sound.h"

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

	// Windshield Header Banner ("??????" Green/White banner plate)
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
#endif // LEVEL1_H
