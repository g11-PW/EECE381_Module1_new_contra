#include "screen.h"
#include "game.h"


//pos: current position of bullet
//x,y: start point
//length: the length of trace
//angle: 0--east, 1--northeast, 2--north, 3--northwest, 4--west
//bullet_size: REG_BULLET, BIG_BULLET

struct position* moving_bullet(struct position* pos , int angle, int bullet_size)
{
	//Bullet Animation--------------------



	if(angle==0)
		pos->x = pos->x+3;
	else if(angle == 1){
		pos->x = pos->x+3;
		pos->y = pos->y-3;
	}
	else if(angle==2)
		pos->y = pos->y-3;

	if(bullet_size == REG_BULLET)
		Draw_Bullet(pos->x,pos->y);
	else
		Draw_BigBullet(pos->x,pos->y);

	usleep(50000);

	if(bullet_size == REG_BULLET)
			Draw_PointOfBg(pos->x,pos->y);
		else{
			Draw_PointOfBg(pos->x,pos->y);
			Draw_PointOfBg(pos->x+1,pos->y);
			Draw_PointOfBg(pos->x,pos->y-1);
			Draw_PointOfBg(pos->x+1,pos->y-1);
		}

	return pos;

}

