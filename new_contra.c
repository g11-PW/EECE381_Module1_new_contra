/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include "new_contra.h"
#include "screen.h"
#include "menu.h"
#include "game.h"
#include "animation.h"
#include "audio.h"
#include "sd_card_controller.h"
//#include "bmp.h"


//define each action mode
#define ACT_STANDSTILL 0
#define ACT_STANDSHOOT 1
#define ACT_WALKLEFT 2
#define ACT_WALKRIGHT 3
#define ACT_JUMP 4
#define ACT_LOOKUP 5
#define ACT_PROWN 6

//Action mode: player can be in one action mode at a time
int action_mode = ACT_STANDSTILL;

//determine if player shoots or not, 1 is yes, 0 is no
int shoot_flag = 0;

// determine if the player is jumoing or not, 1 is no, else is yes
int isJumping = 1;

//1--keyboard
alt_up_ps2_dev *ps2;
KB_CODE_TYPE *decode_mode;
alt_u8 buf;
char ascii;

//Menu
int menu_elem = 0;

//Control Flag
int exit_flag=0;

//Bullet
struct position pos_bullet = {0,0};
struct position *pPos = &pos_bullet;
int bullet_dir = -1;


void initKeyboard(){
	ps2 = alt_up_ps2_open_dev("/dev/keyboard");
	alt_up_ps2_init(ps2);
	printf("Detected device: %d\n",ps2->device_type);
}


void clean_up(){

	alt_up_char_buffer_clear(char_buffer); //Clear char buffer
	Sc_Clear();   //Clean the screen

}

void RunGame(){

	//init settings
	initPlayer();
	initMonster();

	//Get the start time
	alt_timestamp_start();
	start_time = (int)alt_timestamp();

	//enter the game mode
	system_settings.mode = NO_MENU;

	//draw lives
	alt_up_char_buffer_string(char_buffer, "LIVES: 3 ", 1, 1);
	//draw hp bar for ufo]
	addHPBar();
}

void SaveGame(){
	end_time = (int)alt_timestamp();
	system_settings.play_time = (int)((float)(end_time - start_time) / (float)alt_timestamp_freq());
	printf("play time is %d seconds\n", system_settings.play_time);
	saveToSDCard();
};

void LoadGame(){
	loadFromSDCard();
};

void ExitGame(){
	clean_up();
	exit_flag = 1;
}

int kb_flag=0;

static void keyboard_ISR(void *c, alt_u32 id) {

	if(decode_scancode(ps2,decode_mode,&buf,&ascii)!=0)
		return;


	//printf("system_settings.mode:%d  Decode mode: %d Buffer: 0x%X ASCII: %c\n",system_settings.mode, *decode_mode,buf,ascii);

	if(kb_flag==1)
	{
		kb_flag = 0;
		buf = 0x00;
		return;
	}
	else
		kb_flag = 1;

	if(system_settings.mode == START_MENU){

			// printf("Decode mode: %d Buffer: 0x%X ASCII: %c\n",*decode_mode,buf,ascii);
			switch(buf){
				case DOWN_ARROW:
					menu_elem = (menu_elem+1)%3;
					printMenu(system_settings.mode, menu_elem);
					//usleep(500000);
					break;

				case UP_ARROW:
					menu_elem = menu_elem - 1;
					if(menu_elem == -1)
						menu_elem = 2;
					printMenu(system_settings.mode, menu_elem);
					//usleep(500000);
					break;

				case ESC:
					ExitGame();
					break;

				case ENTER:
					if(menu_elem == 0){
						system_settings.num_player = (system_settings.num_player==1?2:1);
					}
					else if(menu_elem == 1){
						if(system_settings.level==EASY)
							system_settings.level=MEDIUM;
						else if(system_settings.level==MEDIUM)
							system_settings.level=HARD;
						else
							system_settings.level=EASY;
						}
					else if(menu_elem == 2){
						system_settings.audio = (system_settings.audio==1?0:1);
					}
					printMenu(system_settings.mode, menu_elem);
					break;

				case SPACE:
					initCharBuffer();
					RunGame();
					break;

				default:
					break;

			}

	}

	else if(system_settings.mode == GAME_MENU){
		switch(buf){
			case ESC:
				system_settings.mode = NO_MENU;
				menu_elem = 0;
				printMenu(system_settings.mode, menu_elem);
				break;
			case RIGHT_ARROW:
				menu_elem = (menu_elem+1)%3;
				printMenu(system_settings.mode, menu_elem);
				//usleep(500000);
				break;
			case LEFT_ARROW:
				menu_elem = menu_elem - 1;
				if(menu_elem == -1)
					menu_elem = 2;
				printMenu(system_settings.mode, menu_elem);
				//usleep(500000);
				break;

			case ENTER:
				if(menu_elem==0){
					SaveGame();
					system_settings.mode = NO_MENU;
					initCharBuffer();
				}
				else if(menu_elem==1){
					LoadGame();
					system_settings.mode = NO_MENU;
					initCharBuffer();
					printLives();
				}
				else
					exit_flag = 1;
				break;

			default:
				break;
		}
	}

	else if(system_settings.mode == GAME_OVER){
		switch(buf){
			case ESC:
				ExitGame();
				break;
			case SPACE:
				initCharBuffer();
				RunGame();
				//usleep(500000);
				break;
			default:
				break;
		}
	}

	else{ //in game
		//printf("system_settings.mode = %d",system_settings.mode);
		switch(buf){
			case ESC:
				system_settings.mode = GAME_MENU;
				menu_elem = 0;
				printMenu(system_settings.mode, menu_elem);
				break;
			case RIGHT_ARROW:
				action_mode = ACT_WALKRIGHT;
				break;
			case LEFT_ARROW:
				action_mode = ACT_WALKLEFT;
				break;
			case UP_ARROW:
				action_mode = ACT_LOOKUP;
				break;
			case DOWN_ARROW:
				layDown();
				break;
			case SPACE:
				shoot_flag = 1;
				break;
			case ALT_LEFT:
				action_mode = ACT_JUMP;
				break;

			case 0x00:
				//p.moving_dir = WALK_STOP;
				break;
		}
	}
	buf=0x00;

}



void initSettings(){
	system_settings.audio = 1;
	system_settings.num_player = 1;
	system_settings.level = 0;
	system_settings.play_time = 0.0;
	system_settings.mode = START_MENU;
}

void initMonster(){
	bo.x = 131;
	bo.y = 40;
	bo.dirX = LEFT;
	bo.dirY = UP;
	bo.shootCount = 25;
	bo.hp = 100;
}




int main()
{
	Sc_Init();  //Initiate the screen
	Sc_Clear();


	initCharBuffer();
	initKeyboard();

 	//init audio
	initializeSDCardController();
	initialize_audio();

	//Initialize settings
	initSettings();

	// Load the background picture
 	Draw_Background();


 	//enable keyboard IRQ
 	void* keyboard_control_register_ptr = (void*) (KEYBOARD_BASE + 4);
 	alt_irq_register(KEYBOARD_IRQ, keyboard_control_register_ptr, keyboard_ISR);
 	alt_up_ps2_enable_read_interrupt(ps2);

 	//print the start menu
 	printMenu(system_settings.mode, menu_elem);

 	playBackground();

 	//Initialize SD Card
 	//initializeSDCardController();
 	//connectToSDCard();

 	int spownCountDown = 10; //wait 10 loops for reborn
 	char cPlayTime[30];
	while (1){

		//quit or not
		if(exit_flag == 1)
		{
			ExitGame();
			return 0;
		}



		if(system_settings.mode==GAME_OVER){
			//printf("go here!");
			//initCharBuffer();
			//Draw_Background();
			//------------------------------

			// -----drawPlayer at coordinate:x100,y163----------
			// -----define image size in game.h-----------------
			//
			// -------------------------------------------------
			drawGameOver();
			// -----erase function is below---------------------
			//

			alt_up_char_buffer_string(char_buffer, "         ", 1, 1);
			alt_up_char_buffer_string(char_buffer, cPlayTime, 27, 32);
			//alt_up_char_buffer_string(char_buffer, "PRESS ESC TO QUIT", 28, 24);
			//alt_up_char_buffer_string(char_buffer, "PRESS SPACE TO RESTART", 27, 28);

			//usleep(500000);

		}
		else if(system_settings.mode==NO_MENU){ //in game

			if(p.alive == 0){ //if died
				if (spownCountDown == 0 && p.lives != 0){
					reborn();
					spownCountDown = 10;

				} else if (spownCountDown != 0){
					spownCountDown--;
				} else if (p.lives == 0) {
					alt_up_char_buffer_string(char_buffer, "GAME OVER", 1, 1);

					//calculate play time
					end_time = (int)alt_timestamp();
					system_settings.play_time = (int)((float)(end_time - start_time) / (float)alt_timestamp_freq());
					sprintf(cPlayTime,"Play Time : %d seconds",system_settings.play_time);
					system_settings.mode = GAME_OVER;

				}
			} else if (bo.hp <= 0) {
				alt_up_char_buffer_string(char_buffer, "YOU WIN!!", 1, 1);
			} else {

				if(action_mode == ACT_JUMP || isJumping != 1) {
					isJumping = makejump();
					if(shoot_flag == 1){
						shoot_flag = 0;
					}
				} else if(action_mode == ACT_WALKRIGHT) {
					p.dir = RIGHT;
					walk();
					if(shoot_flag == 1){
						playerDoShoot();
						shoot_flag = 0;
					}
				} else if(action_mode == ACT_PROWN) {
					layDown();
					if(shoot_flag == 1){
						playerDoShoot();
						shoot_flag = 0;
					}
				} else if(action_mode == ACT_WALKLEFT) {
					p.dir = LEFT;
					walk();
					if(shoot_flag == 1){
						playerDoShoot();
						shoot_flag = 0;
					}
				} else if(action_mode == ACT_LOOKUP) {
					lookUp();
					if(shoot_flag == 1){
						playerDoShoot();
						shoot_flag = 0;
					}
				} else if(action_mode == ACT_STANDSTILL) {
					if(shoot_flag == 1){
						standShoot();
						playerDoShoot();
						shoot_flag = 0;
					} else {
						standStill();
					}
				}

			}


			calcEnemy();
			calcBullet();
			//drawBossHP(bo.hp);

			drawUfoHp();

			if (bo.hp > 0) {
				drawUfo();
			}
			drawPlayer(p.sizeX, p.sizeY, p.image, p.x, p.y, p.dir);
			drawBullet();

			usleep(50000);

			if (bo.hp > 0) {
				eraseArea(UFO_SIZE_X, UFO_SIZE_Y, bo.x, bo.y);
			}
			eraseArea(p.sizeX, p.sizeY, p.x, p.y);
			eraseBullet();
		}

	}

	return 0;
}
