/*
 * Screen.cpp
 *
 *  Created on: 2014-10-10
 *      Author: XuChen
 */

#include "Screen.h"
#include "game.h"
#include "bmp.h"




int i = 0;
int jumpStage = 0;
int pose = 0;

//Initiate the screen buffer

void Sc_Init(){
	unsigned int pixel_buffer_addr1 = PIXEL_BUFFER_BASE;

	// Use the name of your pixel buffer DMA core
	pixel_buffer = alt_up_pixel_buffer_dma_open_dev(BUFFER_DMA_NAME);

	alt_up_pixel_buffer_dma_change_back_buffer_address(pixel_buffer,pixel_buffer_addr1);
	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
	while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer))
			;

	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);

}

void Sc_Clear(){
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);
}

int hw_flag=1;


void Draw_Background(){
	//unsigned int color = 0xc58e;
	unsigned int x, y;
	if(hw_flag==1){

		//printf("print background by hardware!");
		for(y=0; y < 240 ; y++) {
				for(x=0; x < 320 ; x++){
					IOWR_32DIRECT(drawer_base,0,x); // Set x1
					IOWR_32DIRECT(drawer_base,4,y); // Set y1
					IOWR_32DIRECT(drawer_base,8,x); // Set x2
					IOWR_32DIRECT(drawer_base,12,y); // Set y2
					IOWR_32DIRECT(drawer_base,16,background_ARRAY[y][x]);  // Set colour
					IOWR_32DIRECT(drawer_base,20,1);  // Start drawing
					while(IORD_32DIRECT(drawer_base,20)==0); // wait until done
				}
			}
	}

	else{

		for(y=0; y < SCREEN_HEIGHT ; y++) {
			for(x=0; x < SCREEN_WIDTH ; x++){

				alt_up_pixel_buffer_dma_draw(pixel_buffer, background_ARRAY[y][x], x,  y);

			}
		}
	}
	//printf("go here");

}

void Draw_PartOfBg(int x1, int y1, int x2, int y2){

		unsigned int x, y;
		for(y=y1; y <= y2 ; y++) {
			for(x=x1; x <= x2 ; x++){

				alt_up_pixel_buffer_dma_draw(pixel_buffer, background_ARRAY[y][x], x,  y);

			}
		}
}

void Draw_PointOfBg(int x, int y){
	alt_up_pixel_buffer_dma_draw(pixel_buffer, background_ARRAY[y][x], x,  y);
}


void Draw_Bullet(int x, int y){
	alt_up_pixel_buffer_dma_draw(pixel_buffer, 31<<11, x,  y);
}

void Draw_BigBullet(int x, int y){
	alt_up_pixel_buffer_dma_draw(pixel_buffer, 31<<11, x,  y);
	alt_up_pixel_buffer_dma_draw(pixel_buffer, 31<<11, x+1,  y);
	alt_up_pixel_buffer_dma_draw(pixel_buffer, 31<<11, x,  y-1);
	alt_up_pixel_buffer_dma_draw(pixel_buffer, 31<<11, x+1,  y-1);
}


/*
 * Draw the player in different posture
 * sizeX,sizeY -- The width and height of picture
 * posX,posY --  the position of picture
 * dir -- moving direction (right/LEFT)
 */
void drawPlayer(int sizeX, int sizeY, unsigned int player[], int posX, int posY, int dir) {
	int i = 0;
	int x, y, color;

	if (dir == WALK_RIGHT){
		for(y=posY-sizeY; y < posY ; y++) {
			for(x=posX; x < posX+sizeX ; x++){
				color = player[i];
				if (color != 0xffff) {
					if (hw_flag) {
						IOWR_32DIRECT(drawer_base,0,x); // Set x1
						IOWR_32DIRECT(drawer_base,4,y); // Set y1
						IOWR_32DIRECT(drawer_base,8,x); // Set x2
						IOWR_32DIRECT(drawer_base,12,y); // Set y2
						IOWR_32DIRECT(drawer_base,16,color);  // Set colour
						IOWR_32DIRECT(drawer_base,20,1);  // Start drawing
						while(IORD_32DIRECT(drawer_base,20)==0); // wait until done
				 	} else {
				 		alt_up_pixel_buffer_dma_draw(pixel_buffer, color , x,  y);
				 	}
				}
				i++;
			}
		}
	} else {
		for(y=posY-sizeY; y < posY ; y++) {
			for(x=posX+sizeX-1; x > posX-1 ; x--){
				color = player[i];
				if (color != 0xffff) {
					if (hw_flag) {
						IOWR_32DIRECT(drawer_base,0,x); // Set x1
						IOWR_32DIRECT(drawer_base,4,y); // Set y1
						IOWR_32DIRECT(drawer_base,8,x); // Set x2
						IOWR_32DIRECT(drawer_base,12,y); // Set y2
						IOWR_32DIRECT(drawer_base,16,color);  // Set colour
						IOWR_32DIRECT(drawer_base,20,1);  // Start drawing
						while(IORD_32DIRECT(drawer_base,20)==0); // wait until done
				 	} else {
				 		alt_up_pixel_buffer_dma_draw(pixel_buffer, color , x,  y);
				 	}
				}
				i++;
			}
		}
	}
}


//draw ufo
void drawUfo(){
	drawPlayer(UFO_SIZE_X, UFO_SIZE_Y, ufo, bo.x, bo.y, RIGHT);
}

void drawUfoHp()
{
	printString("UFO HP ",1,57);
	printRect(35,227,136,231);

	int i,j;
	for(j=228;j<231;j++){
		for(i=136-bo.hp; i<136;i++)
			alt_up_pixel_buffer_dma_draw(pixel_buffer, RED, i,  j);
	}

}

void drawGameOver(){
	 drawPlayer(GAMEOVER_SIZE_X, GAMEOVER_SIZE_Y, game_over, 100, 163, RIGHT);
}


//make the player walk LEFT or right from walkX,walkY according to walkDirection, returns destination x coordinate

void walk() {
		p.bulletDir = p.dir;
		//p.y = ground;
			if (p.dir == RIGHT){
				if (p.x < 340-42){
					p.x+=6; //move by 6 pixels
				}
			}else{
				if (p.x > 0+5){
					p.x-=6; //move by 6 pixels
				}
			}

		//pose changes
		if (pose == 1) {
			pose = 2;
		}else if (pose == 2) {
			pose = 3;
		}else if (pose == 3) {
			pose = 4;
		}else if (pose == 4) {
			pose = 1;
		}else {pose = 1;}

		if (pose == 1) {
					p.image = shoot2;
					p.sizeX = 25;
					p.sizeY = 24;
				}else if (pose == 2 || pose == 4) {
					p.image = shoot1;
					p.sizeX = 24;
					p.sizeY = 24;
				}else if (pose == 3) {
					p.image = shoot0;
					p.sizeX = 21;
					p.sizeY = 24;
				}
	}

void standStill() {

	p.image= stand;
	p.sizeX = 15;
	p.sizeY = 24;
	pose = 1;
	p.bulletDir = p.dir;
}

void standShoot() {
		p.image= shoot0;
		p.sizeX = 21;
		p.sizeY = 24;
		pose = 1;
		p.bulletDir = p.dir;
}

void layDown() {
	//p.y = ground;
	p.image= prone;
	p.sizeX = 32;
	p.sizeY = 11;
	p.bulletDir = p.dir;
}

void lookUp(){
		p.image= up_stand;
		p.sizeX = 13;
		p.sizeY = 34;
		p.bulletDir = UP;
	}

void do_die() {
		p.alive = 0;
		p.image= dead;
		p.sizeX = 32;
		p.sizeY = 7;
	}

//int jumpStage = 0;
/*
	int fulljump(int jumpX, int jumpY, int jumpDirection){
		int i = 0;
		for(i = 0; i<=16; i++)
			jumpY = makejump(jumpX, jumpY, jumpDirection, i);

		return jumpY;
	}
*/

int makejump() {
		//jumpStage changes
		p.bulletDir = p.dir;
		p.sizeX = 14;
		p.sizeY = 14;
		if (jumpStage == 1) {
			p.y -= 22;
			p.image= jump1;
			jumpStage++;
		}else if (jumpStage == 2) {
			p.y -= 16;
			p.image= jump2;
			jumpStage++;
		}else if (jumpStage == 3) {
			p.y -= 11;
			p.image= jump3;
			jumpStage++;
		}else if (jumpStage == 4) {
			p.y -= 7;
			p.image= jump0;
			jumpStage++;
		}else if (jumpStage == 5) {
			p.y -= 4;
			p.image= jump1;
			jumpStage++;
		}else if (jumpStage == 6) {
			p.y -= 2;
			p.image= jump2;
			jumpStage++;
		}else if (jumpStage == 7) {
			p.y -= 1;
			p.image= jump3;
			jumpStage++;
		}else if (jumpStage == 8) {
			p.y -= 0;
			p.image= jump0;
			jumpStage++;
		}else if (jumpStage == 9) {
			p.y += 1;
			p.image= jump1;
			jumpStage++;
		}else if (jumpStage == 10) {
			p.y += 2;
			p.image= jump2;
			jumpStage++;
		}else if (jumpStage == 11) {
			p.y += 4;
			p.image= jump3;
			jumpStage++;
		}else if (jumpStage == 12) {
			p.y += 7;
			p.image= jump0;
			jumpStage++;
		}else if (jumpStage == 13) {
			p.y += 11;
			p.image= jump1;
			jumpStage++;
		}else if (jumpStage == 14) {
			p.y += 16;
			p.image= jump2;
			jumpStage++;
		}else if (jumpStage == 15) {
			p.y += 22;
			p.image= jump3;
			jumpStage++;
		}else if (jumpStage == 16) {
			p.image= stand;
			p.sizeX = 15;
			p.sizeY = 24;
			jumpStage = 1;
		}
		return jumpStage;
	}

void eraseArea(int imageX, int imageY, int posX, int posY) {
			int x, y, color;

			for(y=posY-imageY; y < posY ; y++) {
				for(x=posX; x < posX+imageX ; x++){
					color = background_ARRAY[y][x];
					if (hw_flag) {
						IOWR_32DIRECT(drawer_base,0,x); // Set x1
						IOWR_32DIRECT(drawer_base,4,y); // Set y1
						IOWR_32DIRECT(drawer_base,8,x); // Set x2
						IOWR_32DIRECT(drawer_base,12,y); // Set y2
						IOWR_32DIRECT(drawer_base,16,color);  // Set colour
						IOWR_32DIRECT(drawer_base,20,1);  // Start drawing
						while(IORD_32DIRECT(drawer_base,20)==0); // wait until done
				 	} else {
				 		alt_up_pixel_buffer_dma_draw(pixel_buffer, color , x,  y);
				 	}
				}
			}
		}


void drawBullet() {
		int x, y, color, i;
		for (i=0; i < MAXbulletNumber; i++) {
			if (bullet_array[i][0] == BUL_EXIST) {
				if (bullet_array[i][5] == TAR_GOOD) {
					color = 0x0000;
				} else {
					color = 0xd800;
				}
				for(y=bullet_array[i][2]-1; y <= bullet_array[i][2] ; y++) {
					for(x=bullet_array[i][1]; x <= bullet_array[i][1]+1 ; x++){
						if (hw_flag) {
							IOWR_32DIRECT(drawer_base,0,x); // Set x1
							IOWR_32DIRECT(drawer_base,4,y); // Set y1
							IOWR_32DIRECT(drawer_base,8,x); // Set x2
							IOWR_32DIRECT(drawer_base,12,y); // Set y2
							IOWR_32DIRECT(drawer_base,16,color);  // Set colour
							IOWR_32DIRECT(drawer_base,20,1);  // Start drawing
							while(IORD_32DIRECT(drawer_base,20)==0); // wait until done
						} else {
							alt_up_pixel_buffer_dma_draw(pixel_buffer, color , x,  y);
						}
					}
				}
			}
		}
	}

	void eraseBullet() {
		int x, y, color, i;
		for (i=0; i < MAXbulletNumber; i++) {
			if (bullet_array[i][0] == BUL_EXIST || bullet_array[i][0] == BUL_GONE) {
				for(y=bullet_array[i][2]-1; y <= bullet_array[i][2] ; y++) {
					for(x=bullet_array[i][1]; x <= bullet_array[i][1]+1 ; x++){
						color = background_ARRAY[y][x];
						if (hw_flag) {
							IOWR_32DIRECT(drawer_base,0,x); // Set x1
							IOWR_32DIRECT(drawer_base,4,y); // Set y1
							IOWR_32DIRECT(drawer_base,8,x); // Set x2
							IOWR_32DIRECT(drawer_base,12,y); // Set y2
							IOWR_32DIRECT(drawer_base,16,color);  // Set colour
							IOWR_32DIRECT(drawer_base,20,1);  // Start drawing
							while(IORD_32DIRECT(drawer_base,20)==0); // wait until done
						} else {
							alt_up_pixel_buffer_dma_draw(pixel_buffer, color , x,  y);
						}
					}
				}
				if (bullet_array[i][0] == BUL_GONE) {
					bullet_array[i][0] = BUL_NONE;
				}
			}
		}
	}


	void playerDoShoot(){
			int i;
			for (i=0; i < MAXbulletNumber; i++) {
				if (bullet_array[i][0] != BUL_EXIST) {
					bullet_array[i][0] = BUL_EXIST;
					if (p.sizeY == 11 && p.bulletDir == LEFT) {
						bullet_array[i][2] = p.y - 5;
						bullet_array[i][1] = p.x + 7;
					} else if (p.sizeY == 11 && p.bulletDir == RIGHT) {
						bullet_array[i][2] = p.y - 5;
						bullet_array[i][1] = p.x + 25;
					} else if (p.dir == LEFT && p.bulletDir == UP) {
						bullet_array[i][2] = p.y - 30;
						bullet_array[i][1] = p.x + 2;
					} else if (p.dir == RIGHT && p.bulletDir == UP) {
						bullet_array[i][2] = p.y - 30;
						bullet_array[i][1] = p.x + 9;
					} else if (p.dir == LEFT && p.bulletDir == LEFT) {
						bullet_array[i][2] = p.y - 15;
						bullet_array[i][1] = p.x + 7;
					} else {
						bullet_array[i][2] = p.y - 15;
						bullet_array[i][1] = p.x + 14;
					}
					bullet_array[i][4] = p.bulletDir;
					bullet_array[i][5] = TAR_BAD;
					break;
				}
			}
		}



	void bossDoShoot(){
			int i;
			int numOFbull = 0;
			for (i=0; i < MAXbulletNumber; i++) {
				if (bullet_array[i][0] != BUL_EXIST) {
					bullet_array[i][0] = BUL_EXIST;
					bullet_array[i][1] = bo.x + 26;
					bullet_array[i][2] = bo.y;
					if (numOFbull == 0) {
						bullet_array[i][4] = DOWNLEFT;
					} else if (numOFbull == 1) {
						bullet_array[i][4] = DOWN;
					} else {
						bullet_array[i][4] = DOWNRIGHT;
					}
					bullet_array[i][5] = TAR_GOOD;
					numOFbull++;
				}
				if (numOFbull == 3) {
					break;
				}
			}
		}


	void calcBullet(){
			int i;
			for (i=0; i < MAXbulletNumber; i++) {
				if (bullet_array[i][0] == BUL_EXIST) {
					if (bullet_array[i][4] == UP){
						//if(bullet_array[i][5] == good  &&  bullet_array[i][1] <= p.x  &&  bullet_array[i][1] >= p.x+p.sizeX  &&  bullet_array[i][2] <= p.y  &&  bullet_array[i][2] <= p.y+p.sizeY){
						//	bullet_array[i][0] = gone;
						//} else
						if(bullet_array[i][5] == TAR_BAD  &&  bullet_array[i][1] >= bo.x  &&  bullet_array[i][1] <= bo.x+UFO_SIZE_X  &&  bullet_array[i][2] <= bo.y  &&  bullet_array[i][2] >= bo.y-UFO_SIZE_Y) {
							bullet_array[i][0] = BUL_GONE;
							bo.hp--;
						} else if(bullet_array[i][2] <= 0) {
							bullet_array[i][0] = BUL_GONE;
						} else {
							bullet_array[i][2] -= 10;
						}
					} else if (bullet_array[i][4] == DOWNLEFT) {
						//if(bullet_array[i][5] == bad  &&  bullet_array[i][1] <= bo.x  &&  bullet_array[i][1] >= bo.x+bo.sizeX  &&  bullet_array[i][2] <= bo.y  &&  bullet_array[i][2] <= bo.y+bo.sizeY) {
						//	bullet_array[i][0] = gone;
						//} else
						if(bullet_array[i][5] == TAR_GOOD  &&  bullet_array[i][1] >= p.x  &&  bullet_array[i][1] <= p.x+p.sizeX  &&  bullet_array[i][2] <= p.y  &&  bullet_array[i][2] >= p.y-p.sizeY){
							bullet_array[i][0] = BUL_GONE;
							do_die();
						} else if(bullet_array[i][2] >= 239 || bullet_array[i][1] <= 0  ||  bullet_array[i][1] >= 319) {
							bullet_array[i][0] = BUL_GONE;
						} else {
							bullet_array[i][2] += 7;
							bullet_array[i][1] -= 7;
						}
					} else if (bullet_array[i][4] == DOWN) {
						//if(bullet_array[i][5] == bad  &&  bullet_array[i][1] <= bo.x  &&  bullet_array[i][1] >= bo.x+bo.sizeX  &&  bullet_array[i][2] <= bo.y  &&  bullet_array[i][2] <= bo.y+bo.sizeY) {
						//	bullet_array[i][0] = gone;
						//} else
						if(bullet_array[i][5] == TAR_GOOD  &&  bullet_array[i][1] >= p.x  &&  bullet_array[i][1] <= p.x+p.sizeX  &&  bullet_array[i][2] <= p.y  &&  bullet_array[i][2] >= p.y-p.sizeY){
							bullet_array[i][0] = BUL_GONE;
							do_die();
						} else if(bullet_array[i][2] >= 239 || bullet_array[i][1] <= 0  ||  bullet_array[i][1] >= 319) {
							bullet_array[i][0] = BUL_GONE;
						} else {
							bullet_array[i][2] += 10;
						}
					} else if (bullet_array[i][4] == DOWNRIGHT) {
						//if(bullet_array[i][5] == bad  &&  bullet_array[i][1] <= bo.x  &&  bullet_array[i][1] >= bo.x+bo.sizeX  &&  bullet_array[i][2] <= bo.y  &&  bullet_array[i][2] <= bo.y+bo.sizeY) {
						//	bullet_array[i][0] = gone;
						//} else
						if(bullet_array[i][5] == TAR_GOOD  &&  bullet_array[i][1] >= p.x  &&  bullet_array[i][1] <= p.x+p.sizeX  &&  bullet_array[i][2] <= p.y  &&  bullet_array[i][2] >= p.y-p.sizeY){
							bullet_array[i][0] = BUL_GONE;
							do_die();
						} else if(bullet_array[i][2] >= 239 || bullet_array[i][1] <= 0  ||  bullet_array[i][1] >= 319) {
							bullet_array[i][0] = BUL_GONE;
						} else {
							bullet_array[i][2] += 7;
							bullet_array[i][1] += 7;
						}
					} else if (bullet_array[i][4] == LEFT) {
						//if(bullet_array[i][5] == good  &&  bullet_array[i][1] <= p.x  &&  bullet_array[i][1] >= p.x+p.sizeX  &&  bullet_array[i][2] <= p.y  &&  bullet_array[i][2] <= p.y+p.sizeY){
						//	bullet_array[i][0] = gone;
						//} else
						if(bullet_array[i][5] == TAR_BAD  &&  bullet_array[i][1] >= bo.x  &&  bullet_array[i][1] <= bo.x+UFO_SIZE_X  &&  bullet_array[i][2] <= bo.y  &&  bullet_array[i][2] >= bo.y-UFO_SIZE_Y) {
							bullet_array[i][0] = BUL_GONE;
							bo.hp--;
						} else if(bullet_array[i][1] <= 0  ||  bullet_array[i][1] >= 319) {
							bullet_array[i][0] = BUL_GONE;
						} else {
							bullet_array[i][1] -= 10;
						}
					} else {
						//if(bullet_array[i][5] == good  &&  bullet_array[i][1] <= p.x  &&  bullet_array[i][1] >= p.x+p.sizeX  &&  bullet_array[i][2] <= p.y  &&  bullet_array[i][2] <= p.y+p.sizeY){
						//	bullet_array[i][0] = gone;
						//} else
						if(bullet_array[i][5] == TAR_BAD  &&  bullet_array[i][1] >= bo.x  &&  bullet_array[i][1] <= bo.x+UFO_SIZE_X  &&  bullet_array[i][2] <= bo.y  &&  bullet_array[i][2] >= bo.y-UFO_SIZE_Y) {
							bullet_array[i][0] = BUL_GONE;
							bo.hp--;
						} else if(bullet_array[i][1] <= 0  ||  bullet_array[i][1] >= 319) {
							bullet_array[i][0] = BUL_GONE;
						} else {
							bullet_array[i][1] += 10;
						}
					}
				}
			}
		}

	void drawBossHP(int hp) {
			int i = hp/2;
			ufo[i] = 0xFFFF;
	}

	void calcEnemy(){
			if (bo.shootCount == 0){
				if (bo.hp > 0) {
					bossDoShoot(bo);
				}
				bo.shootCount = SHOOT_SPEED;
			} else {
				bo.shootCount--;
			}

			if (bo.dirX == LEFT){
				if (bo.x > 3) {
					bo.x -= 3;
				} else {
					bo.x = 0;
					bo.dirX =  RIGHT;
				}
			} else {
				if (bo.x < 320-4-51) {
					bo.x += 2;
				} else {
					bo.x = 320-52;
					bo.dirX =  LEFT;
				}
			}

			if (bo.dirY == UP){
				if (bo.y > 33) {
					bo.y -= 3;
				} else {
					bo.y = 30;
					bo.dirY =  DOWN;
				}
			} else {
				if (bo.y < 58) {
					bo.y += 3;
				} else {
					bo.y = 60;
					bo.dirY =  UP;
				}
			}
		}

	void reborn(){
		p.alive = 1;
		p.x = 10;
		p.y = GROUND_Y;
		p.image = stand;
		p.sizeX = 15;
		p.sizeY = 24;
		p.dir = RIGHT;
		pose = 1;
		jumpStage = 1;
		p.lives--;
		printLives();
	}

void printLives(){

	if (p.lives == 2){
		alt_up_char_buffer_string(char_buffer, "LIVES: 2 ", 1, 1);
	}else if(p.lives == 1){
		alt_up_char_buffer_string(char_buffer, "LIVES: 1 ", 1, 1);
	}else if(p.lives == 0){
		alt_up_char_buffer_string(char_buffer, "LIVES: 0 ", 1, 1);
	}
}


void initPlayer(){
		//Player Settings
		 	p.lives = 3;
		 	p.x = 10;
		 	p.y = GROUND_Y;
		 	p.image = stand;
		 	p.sizeX = 15;
		 	p.sizeY = 24;
		 	p.alive = 1;
		 	p.dir = RIGHT;
}

void addHPBar(){
	int i;
	for (i=0; i<50; i++){
		ufo[i] = 0xd800;
	}
}

unsigned int *index2image (int index){
	unsigned int *image = NULL;
	switch(index){
			case 1:
				image = stand;
				break;
			case 2:
				image = shoot1;
				break;
			case 3:
				image = shoot2;
				break;
			case 4:
				image = shoot0;
				break;
			case 5:
				image = prone;
				break;
			case 6:
				image = up_stand;
				break;
			case 7:
				image = dead;
				break;
			case 8:
				image = jump0;
				break;
			case 9:
				image = jump1;
				break;
			case 10:
				image = jump2;
				break;
			case 11:
				image = jump3;
				break;
			default:
				break;
		}
	return image;
}

int image2index(unsigned int *image){
	int data;
	if(image == stand)
			data = 1;
		else if(image == shoot1)
			data = 2;
		else if(image == shoot2)
			data = 3;
		else if(image == shoot0)
			data = 4;
		else if(p.image == prone)
			data = 5;
		else if(image == up_stand)
			data = 6;
		else if(image == dead)
			data = 7;
		else if(image == jump0)
			data = 8;
		else if(image == jump1)
			data = 9;
		else if(image == jump2)
			data = 10;
		else if(image == jump3)
			data = 11;
		else
			data = 0;

	return data;

}





