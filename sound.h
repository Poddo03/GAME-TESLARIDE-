#ifndef SOUND_H
#define SOUND_H

#include "common.h"

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
#endif // SOUND_H
