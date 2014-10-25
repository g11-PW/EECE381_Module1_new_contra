/*
 * Screen.h
 *
 *  Created on: 2014-10-10
 *      Author: XuChen
 */

#ifndef SCREEN_H_
#define SCREEN_H_


extern void Sc_Init();
extern void Sc_Clear();

extern void Draw_Background();
extern void Draw_PartOfBg(int x1, int y1, int x2, int y2);
extern void Draw_PointOfBg(int x, int y);

extern void Draw_Bullet(int x, int y);
extern void Draw_BigBullet(int x, int y);

void drawPlayer(int sizeX, int sizeY, unsigned int player[], int posX, int posY, int dir);
void drawUfo();
void printLives();

extern void walk();
extern void standStill();
extern void layDown();
extern int fulljump(int jumpX, int jumpY, int jumpDirection);
extern int makejump();
extern void standShoot();

void do_die();
void lookUp();
void bossDoShoot();
void playerDoShoot();
void calcEnemy();
void calcBullet();
void drawBossHP(int hp);
void reborn();
void addHPBar();

void initPlayer();

void eraseArea(int imageX, int imageY, int posX, int posY);

int image2index(unsigned int *image);
unsigned int *index2image (int index);

#endif /* SCREEN_H_ */
