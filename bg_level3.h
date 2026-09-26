#ifndef BG_LEVEL3_H
#define BG_LEVEL3_H

#include "common.h"

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

#endif // BG_LEVEL3_H
