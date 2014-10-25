 

#include "audio.h"
#include "sd_card_controller.h"
#include "game.h"


alt_up_audio_dev * audio_dev;

unsigned int* begin_data;
unsigned int* lose_data;
unsigned int* win_data;
unsigned int* background_data;


unsigned int* audio_buffer;	//pointer to sound data for writing to FIFO

unsigned int byteData[2];
unsigned int data_counter = 0;

void av_config_setup()
{
	alt_up_av_config_dev * av_config = alt_up_av_config_open_dev("/dev/audio_and_video_config");
	while (!alt_up_av_config_read_ready(av_config)) {}
}

void initialize_audio()
{
	//initial setup for AV
	av_config_setup();

	// open the Audio port
	audio_dev = alt_up_audio_open_dev("/dev/audio");
	if ( audio_dev == NULL)
		printf ("Error: could not open audio device \n");
	else
		printf ("Opened audio device \n");

	//connect to SD card
	connectToSDCard();
	//load sound data into our device
	load_sound_data();
	//initialize audio interrupt
	initialize_audio_irq();
}


//initialize irq for audio write
void initialize_audio_irq()
{
	printf("Initializing Audio IRQ...\n");

	alt_up_audio_disable_read_interrupt(audio_dev);
	alt_up_audio_disable_write_interrupt(audio_dev);

	printf("Audio IRQ initialized...\n");
}

static void begin_isr(void* context, alt_u32 id)
{
	// FIFO is 75% empty, need to fill it up
	int sample_counter;
	for (sample_counter = 0; sample_counter < SAMPLE_SIZE; sample_counter++)
	{
		// take 2 bytes at a time
		byteData[0] = begin_data[data_counter];
		byteData[1] = begin_data[data_counter+1];
		data_counter += 2;

		// combine the two bytes and store into sample buffer
		audio_buffer[sample_counter] = (byteData[1] << 8) | byteData[0];

		// if we finish reading our data buffer, then we loop back to start over
		if (data_counter >= BEGIN_SIZE)
		{
			data_counter = 0;
			alt_up_audio_disable_write_interrupt(audio_dev);
			free(begin_data);
			begin_data = NULL;
		}
	}
	// finally, we write this sample data to the FIFO
	alt_up_audio_write_fifo(audio_dev, audio_buffer, SAMPLE_SIZE, ALT_UP_AUDIO_LEFT);
	alt_up_audio_write_fifo(audio_dev, audio_buffer, SAMPLE_SIZE, ALT_UP_AUDIO_RIGHT);
}

static void lose_isr(void* context, alt_u32 id)
{
	// FIFO is 75% empty, need to fill it up
	int sample_counter;
	for (sample_counter = 0; sample_counter < SAMPLE_SIZE; sample_counter++)
	{
		// take 2 bytes at a time
		byteData[0] = lose_data[data_counter];
		byteData[1] = lose_data[data_counter+1];
		data_counter += 2;

		// combine the two bytes and store into sample buffer
		audio_buffer[sample_counter] = (byteData[1] << 8) | byteData[0];

		// if we finish reading our data buffer, then we loop back to start over
		if (data_counter >= LOSE_SIZE)
		{
			data_counter = 0;
			alt_up_audio_disable_write_interrupt(audio_dev);
		}
	}
	// finally, we write this sample data to the FIFO
	alt_up_audio_write_fifo(audio_dev, audio_buffer, SAMPLE_SIZE, ALT_UP_AUDIO_LEFT);
	alt_up_audio_write_fifo(audio_dev, audio_buffer, SAMPLE_SIZE, ALT_UP_AUDIO_RIGHT);
}

static void background_isr(void* context, alt_u32 id)
{
	// FIFO is 75% empty, need to fill it up
	int sample_counter;
	for (sample_counter = 0; sample_counter < SAMPLE_SIZE; sample_counter++)
	{
		// take 2 bytes at a time
		byteData[0] = background_data[data_counter];
		byteData[1] = background_data[data_counter+1];
		data_counter += 2;

		// combine the two bytes and store into sample buffer
		audio_buffer[sample_counter] = (byteData[1] << 8) | byteData[0];

		// if we finish reading our data buffer, then we loop back to start over
		if (data_counter >= BACKGROUND_SIZE)
		{
			data_counter = 0;
		}
	}
	// finally, we write this sample data to the FIFO
	alt_up_audio_write_fifo(audio_dev, audio_buffer, SAMPLE_SIZE, ALT_UP_AUDIO_LEFT);
	alt_up_audio_write_fifo(audio_dev, audio_buffer, SAMPLE_SIZE, ALT_UP_AUDIO_RIGHT);
}

static void win_isr(void* context, alt_u32 id)
{
	// FIFO is 75% empty, need to fill it up
	int sample_counter;
	for (sample_counter = 0; sample_counter < SAMPLE_SIZE; sample_counter++)
	{
		// take 2 bytes at a time
		byteData[0] = win_data[data_counter];
		byteData[1] = win_data[data_counter+1];
		data_counter += 2;

		// combine the two bytes and store into sample buffer
		audio_buffer[sample_counter] = (byteData[1] << 8) | byteData[0];

		// if we finish reading our data buffer, then we loop back to start over
		if (data_counter >= WIN_SIZE)
		{
			data_counter = 0;
			alt_up_audio_disable_write_interrupt(audio_dev);
		}
	}
	// finally, we write this sample data to the FIFO
	alt_up_audio_write_fifo(audio_dev, audio_buffer, SAMPLE_SIZE, ALT_UP_AUDIO_LEFT);
	alt_up_audio_write_fifo(audio_dev, audio_buffer, SAMPLE_SIZE, ALT_UP_AUDIO_RIGHT);
}









// opens the sound file and loads it into memory
void load_sound_data()
{
	short int file_handle;
	unsigned int counter;

	printf("Reading sound file...\n");

	// create large buffer to store all sound data
	begin_data = (unsigned int*) malloc(BEGIN_SIZE * sizeof(unsigned int));
	lose_data = (unsigned int*) malloc(LOSE_SIZE * sizeof(unsigned int));
	win_data = (unsigned int*) malloc(WIN_SIZE * sizeof(unsigned int));
	background_data = (unsigned int*) malloc(BACKGROUND_SIZE * sizeof(unsigned int));

	printf("done\n");

	//begin
	file_handle = alt_up_sd_card_fopen("begin.wav", false);
	// skip header
	for (counter = 0; counter < HEADER_SIZE; counter++)
	{
		alt_up_sd_card_read(file_handle);
	}
	// read and store sound data into memory
	for (counter = 0; counter < BEGIN_SIZE; counter++)
	{
		begin_data[counter] = alt_up_sd_card_read(file_handle);
	}
	alt_up_sd_card_fclose(file_handle);
	printf("done\n");

	//lose
	file_handle = alt_up_sd_card_fopen("lose.wav", false);
	// skip header
	for (counter = 0; counter < HEADER_SIZE; counter++)
	{
		alt_up_sd_card_read(file_handle);
	}
	// read and store sound data into memory
	for (counter = 0; counter < LOSE_SIZE; counter++)
	{
		lose_data[counter] = alt_up_sd_card_read(file_handle);
	}
	alt_up_sd_card_fclose(file_handle);
	printf("done\n");

	//win
	file_handle = alt_up_sd_card_fopen("win.wav", false);
	// skip header
	for (counter = 0; counter < HEADER_SIZE; counter++)
	{
		alt_up_sd_card_read(file_handle);
	}
	// read and store sound data into memory
	for (counter = 0; counter < WIN_SIZE; counter++)
	{
		win_data[counter] = alt_up_sd_card_read(file_handle);
	}
	alt_up_sd_card_fclose(file_handle);
	printf("done\n");

	//background
	file_handle = alt_up_sd_card_fopen("back.wav", false);
	// skip header
	for (counter = 0; counter < HEADER_SIZE; counter++)
	{
		alt_up_sd_card_read(file_handle);
	}
	// read and store sound data into memory
	for (counter = 0; counter < BACKGROUND_SIZE; counter++)
	{
		background_data[counter] = alt_up_sd_card_read(file_handle);
	}
	alt_up_sd_card_fclose(file_handle);
	printf("done\n");




	// create buffer for storing samples from sound_data
	audio_buffer = (unsigned int*) malloc(SAMPLE_SIZE * sizeof(unsigned int));
}

void playBegin()
{
	// register
	data_counter = 0;
	alt_irq_register(6, 0x0, begin_isr);
	// enable interrupt
	alt_irq_enable(6);
	alt_up_audio_enable_write_interrupt(audio_dev);
}

void playLose()
{
	// register
	data_counter = 0;
	alt_irq_register(6, 0x0, lose_isr);
	// enable interrupt
	alt_irq_enable(6);
	alt_up_audio_enable_write_interrupt(audio_dev);
}

void playWin()
{
	// register
	data_counter = 0;
	alt_irq_register(6, 0x0, win_isr);
	// enable interrupt
	alt_irq_enable(6);
	alt_up_audio_enable_write_interrupt(audio_dev);
}


void playBackground()
{
	// register
	data_counter = 0;
	alt_irq_register(6, 0x0, background_isr);
	// enable interrupt
	alt_irq_enable(6);
	alt_up_audio_enable_write_interrupt(audio_dev);
}


void disableSound()
{
	printf("disable\n");
	data_counter = 0;
	alt_up_audio_disable_write_interrupt(audio_dev);
	alt_up_audio_reset_audio_core(audio_dev);
}

void loadFromSDCard()
{
	short int file_handle = alt_up_sd_card_fopen("save.dat", false);
	if(file_handle<0){
		printf("Fail to load data!");
		return;
	}

	int i;
	char data[SAVE_FILE_SIZE];
	// skip header

	for (i = 0; i < SAVE_FILE_SIZE; i++)
	{
		data[i] = alt_up_sd_card_read(file_handle);
		//printf("%d\n",data[i]);
	}
	system_settings.audio = data[0];
	system_settings.level = data[1];
	system_settings.num_player = data[2];

	p.x = data[3]*10 + data[4];
	p.y = data[5]*10 + data[6];

	p.sizeX = data[7];
	p.sizeY = data[8];
	p.lives = data[9];
	p.alive = data[10];
	p.bulletDir = data[11];
	p.dir = data[12];

	p.image = index2image(data[13]);

	bo.x = data[14]*10 + data[15];
	bo.y = data[16]*10 + data[17];
	bo.hp = data[18];
	bo.dirX = data[19];
	bo.dirY = data[20];
	bo.bulletDir = data[21];
	bo.shootCount = data[22];

	system_settings.play_time = data[23]*100+data[24];
	system_settings.mode = data[25];

	alt_up_sd_card_fclose(file_handle);

}

void saveToSDCard(){
	short int file_handle = alt_up_sd_card_fopen("save.dat", false);

	char data[SAVE_FILE_SIZE];

	if(file_handle<0){
		printf("Fail to save data!");
		return;
	}

	data[0] = system_settings.audio;
	data[1] = system_settings.level;
	data[2] = system_settings.num_player;

	data[3] = p.x/10;
	data[4] = p.x-10*p.x/10;
	data[5] = p.y/10;
	data[6] = p.y-10*p.y/10;
	data[7] = p.sizeX;
	data[8] = p.sizeY;
	data[9] = p.lives;
	data[10] = p.alive;
	data[11] = p.bulletDir;
	data[12] = p.dir;

	//data[13] = p.image/10000;
	//data[14] = (p.image - data[13]*10000)/100;
	//data[15] = (p.image - data[13]*10000)%100;
	data[13] = image2index(p.image);

	data[14] = bo.x/10;
	data[15] = bo.x-10*bo.x/10;
	data[16] = bo.y/10;
	data[17] = bo.y-10*bo.y/10;
	data[18] = bo.hp;
	data[19] = bo.dirX;
	data[20] = bo.dirY;
	data[21] = bo.bulletDir;
	data[22] = bo.shootCount;

	//System settings 2
	int time = system_settings.play_time;
	if(time>9999)
		time = 9999;
	data[23] = time/100;
	data[24] = time%100;
	data[25] = system_settings.mode;


	int i;
	for(i=0; i<SAVE_FILE_SIZE; i++){
		alt_up_sd_card_write(file_handle, data[i]);
	}

	alt_up_sd_card_fclose(file_handle);


}




