#ifndef ZOMBIES_H
#define ZOMBIES_H

#include <windows.h>
#include "Resource.h"
#include "GameEngine.h"
#include "Bitmap.h"
#include "Sprite.h"
#include <math.h>
#include <conio.h> 
#include "c\\bass.h"

HINSTANCE   g_hInstance;
GameEngine* g_pGame;
HDC         g_hOffscreenDC;
HBITMAP     g_hOffscreenBitmap;
Bitmap*     g_pBackground;
Bitmap*     g_pPlayerBit;
Bitmap*     g_pBullet;
Bitmap*     g_pZombie;
Bitmap*     g_pZombie2;
Bitmap*     g_pZombie3;
Sprite*     g_pPlayerSpr;
int g_iPlayerX, g_iPlayerY;
BOOL        bFire, g_bGameOver;
int         g_iMouseX, g_iMouseY;
int         g_iFireRate, g_iDifficulty, g_iNumLives, g_iRound, g_iZombiesKilled;
int         g_iRunningChance, g_iFatChance, g_iZombieNoises;
int         g_iSongTimer, g_iShotsFired, g_iReloadTimer;
DWORD       g_CurrentChan, g_chan1, g_chan2, g_chan3;

void        CreateBullet();
void        AddZombie();
void        MoveZombie();
void        Death();
void        NewGame();
void        NextRound();
void        ShowStats(HDC hDC);
void        TurnDude();
void        ZombieNoises();
void        CheckReload();
void        ChangeSong();

#endif
