#ifndef GAME_H_
#define GAME_H_


#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include <altera_up_avalon_video_character_buffer_with_dma_regs.h>

#include <altera_up_avalon_ps2.h>
#include <altera_up_ps2_keyboard.h>
#include <Altera_UP_SD_Card_Avalon_Interface.h>


#include "altera_up_avalon_audio.h"
#include "altera_up_avalon_audio_regs.h"
#include "altera_up_avalon_audio_and_video_config.h"
#include "altera_up_avalon_audio_and_video_config_regs.h"


#include "sys/alt_irq.h"
#include "priv/alt_legacy_irq.h"
#include "altera_up_sd_card_avalon_interface.h"
#include "altera_avalon_pio_regs.h"

#include "sys/alt_alarm.h"
#include "sys/alt_timestamp.h"

#include "io.h"

#include <stdlib.h>
#include <stdio.h>
#include "system.h"
#include <math.h>
#include <unistd.h>
#include <time.h>



#define drawer_base (volatile int *) 0x4840
#define switches (volatile char *) 0x00004880
#define leds (char *) 0x00004870
#define keys (volatile char *) 0x00004860

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240


#define ESC 0x76
#define UP_ARROW 0x75
#define DOWN_ARROW 0x72
#define LEFT_ARROW 0x6B
#define RIGHT_ARROW 0x74
#define SPACE 0x29
#define ENTER 0x5A
#define ALT_LEFT 0x11

#define BULLET_SPEED 10
#define BIG_BULLET 1
#define REG_BULLET 0

#define NO_MENU 0
#define START_MENU 1
#define GAME_MENU 2
#define GAME_OVER 3

#define EASY 0
#define MEDIUM 1
#define HARD 2

#define AUDIO_OFF 0
#define AUDIO_ON 1

#define WALK_RIGHT 1
#define WALK_LEFT 0
#define WALK_STOP 2
#define LAY_DOWN 3
#define JUMP 4

#define SAVE_FILE_SIZE 26




//Bullet
#define MAXbulletNumber 40
#define BUL_EXIST 1
#define BUL_NONE 0
#define BUL_GONE 2
#define TAR_GOOD 1
#define TAR_BAD 0

//Bullet Direction
#define RIGHT 1
#define LEFT 0
#define UP 2
#define DOWN 3
#define DOWNLEFT 4
#define DOWNRIGHT 5

#define SHOOT_SPEED 11

//Color
#define RED 0xd800

//ground
#define GROUND_Y 215

//ufo
#define UFO_SIZE_X 51
#define UFO_SIZE_Y 27

//Game over size
#define GAMEOVER_SIZE_X 120
#define GAMEOVER_SIZE_Y 87

struct position{
	int x;
	int y;
};

/*
typedef struct player{
	int x;
	int y;
	char lives;
	char bulletType;
	char moving_dir;  //moving direction (WALK_LEFT, WALK_RIGHT, WALK_STOP)
	char stand_dir; //Standing direction
	char shooting;  //1--Yes  0--No
} player;
*/

typedef struct player{
	int x; //position
	int y; //position
	char sizeX; //size of image
	char sizeY; //size of image
	char lives; //lives left
	char alive; //alive or not
	char bulletDir; //bullet direction
	char dir;  //direction (LEFT, RIGHT)
	unsigned int *image;
}player;

/*
typedef struct monster{
	int x;
	int y;
	char hp;
	char bulletType;
} monster;
*/


typedef struct monster{
	int x;
	int y;
	char hp;  //health point
	char bulletDir;
	char shootCount; //shooting counter (11->0)
	char dirX;  //moving left and right
	char dirY; //moving up and down
}monster;

typedef struct settings{
	char num_player;  //1 or 2
	char audio;  //1--on  0--off
	char level; //0--easy  1--medium 2--hard
	int play_time; //in seconds
	char mode;
} settings;

alt_up_pixel_buffer_dma_dev *pixel_buffer;
alt_up_char_buffer_dev *char_buffer;
alt_up_char_buffer_dev *timer_buffer;

//int game_level;
//int num_player;
//int audio_flag;
struct player p;
struct monster bo;
struct settings system_settings;

int start_time;
int end_time;


#endif /* GAME_H_ */




