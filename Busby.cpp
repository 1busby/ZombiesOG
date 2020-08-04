//Zombies
//by Michael Busby

#include "Zombies.h"

BOOL GameInitialize(HINSTANCE hInstance)
{    
     // Create the game engine
  g_pGame = new GameEngine(hInstance, TEXT("Zombies"),
    TEXT("Zombies"), IDI_ZOMBIES, IDI_ZOMBIES_SM, 760, 570);
  if (g_pGame == NULL)
    return FALSE;
  
  // Set the frame rate
  g_pGame->SetFrameRate(30);

  // Store the instance handle
  g_hInstance = hInstance;
  
  bFire = FALSE;
  g_bGameOver = FALSE;
  g_iFireRate = 3;
  g_iNumLives = 3;
  g_iRound = 1;
  g_iZombiesKilled = 0;
  g_iDifficulty = 44;
  g_iRunningChance = 15;
  g_iFatChance = 10000;
  g_iZombieNoises = 1000;
  g_iSongTimer = 0;
  g_iShotsFired = 0;
  g_iReloadTimer = 100;

  return TRUE;
 }

void GameStart(HWND hWindow)
{
      // Seed the random number generator
  srand(GetTickCount());
  
  PlaySound((LPCSTR)IDW_OPENING,g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
  
  //g_pGame->PlayMIDISong(TEXT("Music2.mid"));
  
  BASS_Init(-1,44100,0,hWindow,0);
  g_chan3= BASS_StreamCreateFile(false,TEXT("AllMyLife.mp3"),0,0,0);
   g_CurrentChan = g_chan3;
   BASS_ChannelPlay(g_chan3,false);
   BASS_SetVolume(0.2);


  
  // Create the offscreen device context and bitmap
  g_hOffscreenDC = CreateCompatibleDC(GetDC(hWindow));
  g_hOffscreenBitmap = CreateCompatibleBitmap(GetDC(hWindow),
    g_pGame->GetWidth(), g_pGame->GetHeight());
  SelectObject(g_hOffscreenDC, g_hOffscreenBitmap);

  // Create and load the bitmaps
  HDC hDC = GetDC(hWindow);
  g_pBackground = new Bitmap(hDC, IDB_BACKGROUND, g_hInstance);
  g_pPlayerBit = new Bitmap(hDC, IDB_PLAYER, g_hInstance);
  g_pBullet = new Bitmap(hDC, IDB_BULLET, g_hInstance);
  g_pZombie = new Bitmap(hDC, IDB_ZOMBIE, g_hInstance);
  g_pZombie2 = new Bitmap(hDC, IDB_ZOMBIETWO, g_hInstance);
  g_pZombie3 = new Bitmap(hDC, IDB_ZOMBIETHREE, g_hInstance);
  
  
  //create player sprite
  RECT rcBounds = { 0, 0, 760, 570 };
  POINT ptPlayerPos;
  POINT ptPlayerVel;
  
  ptPlayerPos.x = 370;
  ptPlayerPos.y = 275;
  ptPlayerVel.x = ptPlayerVel.y = 0;
  
  g_pPlayerSpr = new Sprite(g_pPlayerBit, ptPlayerPos, ptPlayerVel, 0, rcBounds);
  g_pGame->AddSprite(g_pPlayerSpr);
 }
 
void GameEnd()
{
     DeleteObject(g_hOffscreenBitmap);
     DeleteDC(g_hOffscreenDC); 
     
    g_pGame->CloseMIDIPlayer();
    BASS_Free();

     
     // Cleanup the bitmaps
  delete   g_pBackground;
  delete   g_pPlayerBit;
     
     // Cleanup the sprites
  g_pGame->CleanupSprites();

  // Cleanup the game engine
  delete g_pGame;
}

void GameActivate(HWND hWindow)
{
     g_pGame->PlayMIDISong(TEXT(""), TRUE);
     BASS_ChannelPlay(g_CurrentChan,false);
 }
 
void GameDeactivate(HWND hWindow)
{
     g_pGame->PauseMIDISong();
     BASS_ChannelPause(g_CurrentChan);
 }
 
void GamePaint(HDC hDC)
{
     g_pBackground->Draw(hDC, 0, 0);
     
     //g_pPlayerSpr->Draw(hDC);
     
     ShowStats(hDC);
     
     // Draw the sprites
  g_pGame->DrawSprites(hDC);
 }
 
void GameCycle()
{
     if (!g_bGameOver)  
     {
        
        NextRound();
        
        ZombieNoises();
        
        if(g_iReloadTimer <= 25)
        {
                          g_iReloadTimer++;
        } else
        {
              CreateBullet();
              CheckReload();
        }
        
        
        
        
             if ((rand() % g_iDifficulty) == 0)
                AddZombie();
               
              MoveZombie();  
         
         
             
            
             g_pPlayerSpr->Update();
             //InvalidateRect(g_pGame->GetWindow(), NULL, FALSE);
             
             // Update the sprites
          g_pGame->UpdateSprites();
        
          // Obtain a device context for repainting the game
          HWND  hWindow = g_pGame->GetWindow();
          HDC   hDC = GetDC(hWindow);
        
          // Paint the game to the offscreen device context
          GamePaint(g_hOffscreenDC);
        
          // Blit the offscreen bitmap to the game screen
          BitBlt(hDC, 0, 0, g_pGame->GetWidth(), g_pGame->GetHeight(),
            g_hOffscreenDC, 0, 0, SRCCOPY);
        
          // Cleanup
          ReleaseDC(hWindow, hDC);
     }
}
 
void HandleKeys()
{
     int x = 0;
     int y = 0;
     
     
  if (GetAsyncKeyState(VK_SPACE) < 0)
  {
                                ChangeSong();
                                 }
                                 
  if (GetAsyncKeyState(VK_SHIFT) < 0)
  {
                                PlaySound((LPCSTR)IDW_RELOAD,g_hInstance, SND_ASYNC | SND_RESOURCE);
                       g_iReloadTimer = 0;
                       g_iShotsFired = 0;
                                 }
     // Change the speed of the player in response to arrow key presses
  if (GetAsyncKeyState(VK_LEFT) < 0 && GetAsyncKeyState(VK_UP) < 0 )
  {
    x -= 4;
    y -= 4;
}
  else if (GetAsyncKeyState(VK_RIGHT) < 0 && GetAsyncKeyState(VK_UP) < 0)
  {
    x += 4;
    y -= 4;
}
  else if (GetAsyncKeyState(VK_LEFT) < 0 && GetAsyncKeyState(VK_DOWN) < 0)
  {
    y += 4;
    x -= 4;
}
  else if (GetAsyncKeyState(VK_RIGHT) < 0 && GetAsyncKeyState(VK_DOWN) < 0)
  {
    y += 4;
    x += 4;
}
  else if (GetAsyncKeyState(VK_LEFT) < 0)
    x -= 5;
  else if (GetAsyncKeyState(VK_RIGHT) < 0)
    x += 5;
  else if (GetAsyncKeyState(VK_UP) < 0)
    y -= 5;
  else if (GetAsyncKeyState(VK_DOWN) < 0)
    y += 5;
    
    g_pPlayerSpr->OffsetPosition(x, y);
    
    if (GetAsyncKeyState(VK_RETURN) < 0)
       if (g_bGameOver)
          NewGame();
 }
 
void MouseButtonDown(int x, int y, BOOL bLeft)
{
     if (bLeft)
     {
               bFire = TRUE;
     }
 }
 
void MouseButtonUp(int x, int y, BOOL bLeft)
{
     if (bLeft)
     {
               g_iFireRate = 10;
               bFire = FALSE;
     }
 }
 
void MouseMove(int x, int y)
{
      g_iMouseX = x;
      g_iMouseY = y;
      //TurnDude();
     
 }
 
void HandleJoystick(JOYSTATE jsJoystickState)
{
 }
 
BOOL SpriteCollision(Sprite* pSpriteHitter, Sprite* pSpriteHittee)
{
  if (pSpriteHitter->GetBitmap() == g_pBullet &&
     pSpriteHittee->GetBitmap() == g_pBullet)
  {
     pSpriteHitter->Kill();
  }
  
  if ((pSpriteHitter->GetBitmap() == g_pZombie &&
     pSpriteHittee->GetBitmap() == g_pBullet) ||
     (pSpriteHitter->GetBitmap() == g_pZombie2 &&
     pSpriteHittee->GetBitmap() == g_pBullet) ||
     (pSpriteHitter->GetBitmap() == g_pZombie3 &&
     pSpriteHittee->GetBitmap() == g_pBullet))
  {
     PlaySound((LPCSTR)IDW_HIT,g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
     pSpriteHitter->Hit();
     pSpriteHittee->Hit();
     if (pSpriteHitter->IsDying())
        g_iZombiesKilled++;
  }
  
  if ((pSpriteHitter->GetBitmap() == g_pBullet &&
     pSpriteHittee->GetBitmap() == g_pZombie) ||
     (pSpriteHitter->GetBitmap() == g_pBullet &&
     pSpriteHittee->GetBitmap() == g_pZombie2) ||
     (pSpriteHitter->GetBitmap() == g_pBullet &&
     pSpriteHittee->GetBitmap() == g_pZombie3))
  {
     PlaySound((LPCSTR)IDW_HIT,g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
     pSpriteHitter->Hit();
     pSpriteHittee->Hit();
     if (pSpriteHittee->IsDying())
        g_iZombiesKilled++;
  }
  
  if ((pSpriteHitter->GetBitmap() == g_pPlayerBit &&
     pSpriteHittee->GetBitmap() == g_pZombie) ||
     (pSpriteHitter->GetBitmap() == g_pZombie &&
     pSpriteHittee->GetBitmap() == g_pPlayerBit) ||
     (pSpriteHitter->GetBitmap() == g_pPlayerBit &&
     pSpriteHittee->GetBitmap() == g_pZombie2) ||
     (pSpriteHitter->GetBitmap() == g_pZombie2 &&
     pSpriteHittee->GetBitmap() == g_pPlayerBit) ||
     (pSpriteHitter->GetBitmap() == g_pPlayerBit &&
     pSpriteHittee->GetBitmap() == g_pZombie3) ||
     (pSpriteHitter->GetBitmap() == g_pZombie3 &&
     pSpriteHittee->GetBitmap() == g_pPlayerBit))
  {
     Death();
  }
  
  return FALSE;
}

void CreateBullet()
{
    if (bFire)
    {
      g_iFireRate++;
      
      if (g_iFireRate >= 3)
      {
                      
           //PlaySound((LPCSTR)IDW_FIRE,g_hInstance, SND_ASYNC | SND_RESOURCE |SND_NOSTOP);
           
           
           g_iFireRate = 0;
           g_iShotsFired++;
           //create new bullet
           int iBulletX, iBulletY;
           double XDist, YDist, lAngle, Radian;
           int iZ;
           POINT ptBulletPos, ptBulletVel;
           RECT rcBounds = { 0, 0, 760, 570 };
           RECT rcPlayer;
           Sprite* pSprite;
           
           rcPlayer = g_pPlayerSpr->GetPosition();
           iBulletY = rcPlayer.bottom - 12;
           iBulletX = rcPlayer.right - 13;
           XDist = g_iMouseX - iBulletX;
           YDist = g_iMouseY - iBulletY;
           
           lAngle = atan(YDist / XDist);
           
           
           ptBulletPos.x = iBulletX;
           ptBulletPos.y = iBulletY;
             ptBulletVel.y = sin(lAngle) * 25;
           ptBulletVel.x = cos(lAngle) * 25;
           
           if(g_iMouseX < iBulletX)
           {
                        ptBulletVel.x = -ptBulletVel.x;
                        ptBulletVel.y = -ptBulletVel.y;
                        }
          
           iZ = g_pGame->GetNumSprites();
           pSprite = new Sprite(g_pBullet, ptBulletPos, ptBulletVel, iZ, rcBounds, BA_DIE);
           pSprite->SetHealth(1);
           g_pGame->AddSprite(pSprite);
      }
    }   
}

void AddZombie()
{
     int iSide = rand() % 4;
     int iPosX, iPosY;
     Sprite* pSprite;
     Bitmap* pImage;
     
     if (iSide == 0)
     {
        iPosY = rand() % 570;
        iPosX = -30;
     } else if (iSide == 1)
     {
        iPosY = -30;
        iPosX = rand() % 760;
     } else if (iSide == 2)
     {
        iPosY = rand() % 570;
        iPosX = 760;
     } else if (iSide == 3)
     {
        iPosY = 570;
        iPosX = rand() % 760;
     }
     
     RECT rcBounds = { -50, -50, 810, 620 };
     POINT ptPosition, ptVelocity;
     ptPosition.x = iPosX;
     ptPosition.y = iPosY;
     ptVelocity.x = ptVelocity.y = 0;
     int iZ = g_pGame->GetNumSprites();
     
     if (g_iRound >= 3)
     {
                  if (rand() % g_iRunningChance == 0)
                  {
                              pSprite = new Sprite(g_pZombie2, ptPosition, ptVelocity, iZ, rcBounds, BA_BOUNCE);
                              pSprite->SetHealth(4);
                  } else if (rand() % g_iFatChance == 0)
                  {
                         pSprite = new Sprite(g_pZombie3, ptPosition, ptVelocity, iZ, rcBounds, BA_BOUNCE);
                         pSprite->SetHealth(18);
                  } else
                  {
                         pSprite = new Sprite(g_pZombie, ptPosition, ptVelocity, iZ, rcBounds, BA_BOUNCE);
                         pSprite->SetHealth(4);
                  }
     } else 
     {
             pSprite = new Sprite(g_pZombie, ptPosition, ptVelocity, iZ, rcBounds, BA_BOUNCE);
             pSprite->SetHealth(4);
     }
     
    
     g_pGame->AddSprite(pSprite);
}

void MoveZombie()
{
     vector<Sprite*>  vSprites;
     vSprites = g_pGame->GetSprites();
     
     vector<Sprite*>::iterator siSprite;
      for (siSprite = vSprites.begin(); siSprite != vSprites.end(); siSprite++)
      {
        if ((*siSprite)->GetBitmap() == g_pZombie)
        {
           RECT ZombiePosition = (*siSprite)->GetPosition();
           RECT PlayerPosition = g_pPlayerSpr->GetPosition();
           int iZombieY = ZombiePosition.top ;
           int iZombieX = ZombiePosition.left ;
           int iPlayerY = PlayerPosition.top ;
           int iPlayerX = PlayerPosition.left ;
           double dZombVelY, dZombVelX, XDist, YDist, Angle;
           
           XDist = iPlayerX - iZombieX;
           YDist = iPlayerY - iZombieY;
           
           Angle = atan(YDist / XDist);
           
           dZombVelY = sin(Angle) * 3;
           dZombVelX = cos(Angle) * 3;
           
           if(iPlayerX < iZombieX)
           {
                        dZombVelX = -dZombVelX;
                        dZombVelY = -dZombVelY;
                        }
                        
           iZombieX += dZombVelX;
           iZombieY += dZombVelY;
           
           (*siSprite)->SetPosition(iZombieX, iZombieY);
        }else if ((*siSprite)->GetBitmap() == g_pZombie2)
        {
           RECT ZombiePosition = (*siSprite)->GetPosition();
           RECT PlayerPosition = g_pPlayerSpr->GetPosition();
           int iZombieY = ZombiePosition.top ;
           int iZombieX = ZombiePosition.left ;
           int iPlayerY = PlayerPosition.top ;
           int iPlayerX = PlayerPosition.left ;
           double dZombVelY, dZombVelX, XDist, YDist, Angle;
           
           XDist = iPlayerX - iZombieX;
           YDist = iPlayerY - iZombieY;
           
           Angle = atan(YDist / XDist);
           
           dZombVelY = sin(Angle) * 7;
           dZombVelX = cos(Angle) * 7;
           
           if(iPlayerX < iZombieX)
           {
                        dZombVelX = -dZombVelX;
                        dZombVelY = -dZombVelY;
                        }
                        
           iZombieX += dZombVelX;
           iZombieY += dZombVelY;
           
           (*siSprite)->SetPosition(iZombieX, iZombieY);
        }else if ((*siSprite)->GetBitmap() == g_pZombie3)
        {
           RECT ZombiePosition = (*siSprite)->GetPosition();
           RECT PlayerPosition = g_pPlayerSpr->GetPosition();
           int iZombieY = ZombiePosition.top ;
           int iZombieX = ZombiePosition.left ;
           int iPlayerY = PlayerPosition.top ;
           int iPlayerX = PlayerPosition.left ;
           double dZombVelY, dZombVelX, XDist, YDist, Angle;
           
           XDist = iPlayerX - iZombieX;
           YDist = iPlayerY - iZombieY;
           
           Angle = atan(YDist / XDist);
           
           dZombVelY = sin(Angle) * 2;
           dZombVelX = cos(Angle) * 2;
           
           if(iPlayerX < iZombieX)
           {
                        dZombVelX = -dZombVelX;
                        dZombVelY = -dZombVelY;
                        }
                        
           iZombieX += dZombVelX;
           iZombieY += dZombVelY;
           
           (*siSprite)->SetPosition(iZombieX, iZombieY);
        }
      }
}

void Death()
{
     PlaySound((LPCSTR)IDW_DEATH,g_hInstance, SND_ASYNC | SND_RESOURCE);
     
    g_iNumLives--;
    
    bFire = FALSE;
    g_iShotsFired = 0;
    
    if (g_iNumLives == 0)
       g_bGameOver = TRUE;
       
    //reset position and clear zombies
    
    g_pGame->CleanupSprites();
    //create player sprite
  RECT rcBounds = { 0, 0, 760, 570 };
  POINT ptPlayerPos;
  POINT ptPlayerVel;
  
  ptPlayerPos.x = 370;
  ptPlayerPos.y = 275;
  ptPlayerVel.x = ptPlayerVel.y = 0;
  
  g_pPlayerSpr = new Sprite(g_pPlayerBit, ptPlayerPos, ptPlayerVel, 0, rcBounds);
  
    
    g_pGame->AddSprite(g_pPlayerSpr);
}

//reset everything
void NewGame()
{
        // Cleanup the sprites
     g_pGame->CleanupSprites();
     
     PlaySound((LPCSTR)IDW_OPENING,g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
     
     g_iNumLives = 3;
     g_iDifficulty = 35;
     g_iRound = 1;
     g_iZombiesKilled = 0;
     g_iRunningChance = 20;
     g_iFatChance = 10000;
     g_iZombieNoises = 1000;
     
     //create player sprite
     RECT rcBounds = { 0, 0, 760, 570 };
     POINT ptPlayerPos;
     POINT ptPlayerVel;
  
     ptPlayerPos.x = 370;
     ptPlayerPos.y = 275;
     ptPlayerVel.x = ptPlayerVel.y = 0;
     g_pPlayerSpr = new Sprite(g_pPlayerBit, ptPlayerPos, ptPlayerVel, 0, rcBounds);
     g_pGame->AddSprite(g_pPlayerSpr);
     
     g_bGameOver = FALSE;
}
     
void NextRound()
{
     if (g_iZombiesKilled >= 10 && g_iRound == 1)
     {
        g_iRound++;
        g_iDifficulty -= 4;
     }
     
     if (g_iZombiesKilled >= 30 && g_iRound == 2)
     {
        g_iRound++;
        g_iDifficulty -= 5;
        g_iZombieNoises -=200;
     }
     
     if (g_iZombiesKilled >= 50 && g_iRound == 3)
     {
        g_iRound++;
        g_iDifficulty -= 4;
        g_iRunningChance -= 4;
        g_iZombieNoises -= 100;
     }
     
     if (g_iZombiesKilled >= 90 && g_iRound == 4)
     {
        g_iRound++;
        g_iDifficulty -= 3;
        g_iRunningChance -= 4;
        g_iFatChance -= 9975;
        
     }
     
     if (g_iZombiesKilled >= 130 && g_iRound == 5)
     {
        g_iRound++;
        g_iDifficulty -= 2;
        g_iRunningChance -= 2;
        g_iFatChance -=2;
     }
     
     if (g_iZombiesKilled >= 160 && g_iRound == 6)
     {
        g_iRound++;
        g_iDifficulty -= 3;
     }
}

void ShowStats(HDC hDC)
{
     // Draw the score
  TCHAR szText[64];
  RECT  rect = { 230, 0, 280, 50 };
  wsprintf(szText, "%d", g_iZombiesKilled);
  SetBkMode(hDC, TRANSPARENT);
  SetTextColor(hDC, RGB(255, 255, 255));
  DrawText(hDC, szText, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
  
  //RECT rect2 = { 350, 0, 400, 50 };
//  wsprintf(szText, "%d", g_iRound);
//  SetBkMode(hDC, TRANSPARENT);
//  SetTextColor(hDC, RGB(255, 255, 255));
//  DrawText(hDC, szText, -1, &rect2, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
  
  RECT rect3 = { 520, 0, 570, 50 };
  wsprintf(szText, "%d", g_iNumLives);
  SetBkMode(hDC, TRANSPARENT);
  SetTextColor(hDC, RGB(255, 255, 255));
  DrawText(hDC, szText, -1, &rect3, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

//void TurnDude()
//{
//     RECT PlayerPosition = g_pPlayerSpr->GetPosition();
//     
//     int TopX, TopY, BottomX, BottomY, XDist, YDist;
//     double Angle;
//     
//     TopX = PlayerPosition.left;
//     TopY = PlayerPosition.top;
//     BottomX = PlayerPosition.right;
//     BottomY = PlayerPosition.bottom;
//     
//     XDist = g_iMouseX - TopX;
//     YDist = g_iMouseY - TopY;
//     
//     Angle = atan(YDist / XDist);
//     
//     BottomX = BottomX - cos(Angle);
//     BottomY = BottomY - sin(Angle);
//     
//     RECT NewPosition = {TopX, TopY, BottomX, BottomY};
//     g_pPlayerSpr->SetPosition(NewPosition);
//}    

void ZombieNoises()
{
     int random = rand() % g_iZombieNoises;
     
     if (random == 1)
     {
               PlaySound((LPCSTR)IDW_DEMONSPAWN,g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
     } else if (random == 2)
     {
            PlaySound((LPCSTR)IDW_ZOMBIEATTACK,g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
     } else if (random == 3)
     {
            PlaySound((LPCSTR)IDW_ZOMBIEBRAINS,g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
     } else if (random == 4)
     {
               PlaySound((LPCSTR)IDW_ZOMBIEBACK,g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
     } else if (random == 5)
     {
            PlaySound((LPCSTR)IDW_ZOMBIEPAIN,g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
     } else if (random == 6)
     {
            PlaySound((LPCSTR)IDW_ZOMBIEMOAN,g_hInstance, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
     }
}

void CheckReload()
{
     if (g_iShotsFired >= 35)
     {
                       PlaySound((LPCSTR)IDW_RELOAD,g_hInstance, SND_ASYNC | SND_RESOURCE);
                       g_iReloadTimer = 0;
                       g_iShotsFired = 0;
                       }
}

void ChangeSong()
{
     int Song = rand() % 3;
     
     if(Song == 0)
     {
             if (g_CurrentChan != g_chan1)
             {
             BASS_StreamFree(g_CurrentChan);
             g_chan1= BASS_StreamCreateFile(false,TEXT("Pain.mp3"),0,0,0);
             
             BASS_ChannelPlay(g_chan1,false); 
             g_CurrentChan = g_chan1;
             }
     } else if (Song == 1)
     {
            if (g_CurrentChan != g_chan2)
            {
             BASS_StreamFree(g_CurrentChan);
             g_chan2 = BASS_StreamCreateFile(false,TEXT("Uprising.mp3"),0,0,0);
             BASS_ChannelPlay(g_chan2,false); 
             g_CurrentChan = g_chan2;
             }
     } else if (Song == 2)
     {
            if (g_CurrentChan != g_chan3)
            {
             BASS_StreamFree(g_CurrentChan);
             g_chan2 = BASS_StreamCreateFile(false,TEXT("AllMyLife.mp3"),0,0,0);
             BASS_ChannelPlay(g_chan3,false); 
             g_CurrentChan = g_chan3;
             }
     }
}
