#ifndef BG_LEVEL2_H
#define BG_LEVEL2_H

#include "common.h"

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

#endif // BG_LEVEL2_H
