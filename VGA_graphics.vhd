LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_unsigned.ALL;

ENTITY VGA_graphics IS
	PORT (
		CLOCK_50 : IN STD_LOGIC;
		KEY : IN STD_LOGIC_VECTOR (7 DOWNTO 0);
		SW : IN STD_LOGIC_VECTOR (7 DOWNTO 0);
		LEDG : OUT STD_LOGIC_VECTOR (7 DOWNTO 0);
		PS2_CLK : INOUT STD_LOGIC;
		PS2_DAT : INOUT STD_LOGIC;
		
		
		SD_CMD : inout std_logic;
		SD_DAT : inout std_logic;
		SD_DAT3 : inout std_logic;
		SD_CLK : out std_logic;
		
		VGA_R : out std_logic_vector(9	downto	0);	
		VGA_G : out std_logic_vector(9	downto	0);	
		VGA_B : out std_logic_vector(9	downto	0);	
		VGA_CLK : out std_logic;
		VGA_BLANK : out std_logic;	
		VGA_HS : out std_logic;	
		VGA_VS : out std_logic;	
		VGA_SYNC : out std_logic;
		
		SRAM_DQ : INOUT STD_LOGIC_VECTOR(15	downto	0);
		SRAM_ADDR : OUT STD_LOGIC_VECTOR(17	downto	0);
		SRAM_LB_N : OUT STD_LOGIC;
		SRAM_UB_N : OUT STD_LOGIC;
		SRAM_CE_N : OUT STD_LOGIC;
		SRAM_OE_N : OUT STD_LOGIC;
		SRAM_WE_N : OUT STD_LOGIC;
		
		DRAM_CLK, DRAM_CKE : OUT STD_LOGIC;
      DRAM_ADDR : OUT STD_LOGIC_VECTOR(11 DOWNTO 0);
      DRAM_BA_0, DRAM_BA_1 : BUFFER STD_LOGIC;
      DRAM_CS_N, DRAM_CAS_N, DRAM_RAS_N, DRAM_WE_N : OUT STD_LOGIC;
      DRAM_DQ : INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
      DRAM_UDQM, DRAM_LDQM : BUFFER STD_LOGIC;
	
		I2C_SDAT : inout std_logic;
		I2C_SCLK : out std_logic;
		
		AUD_XCK : out std_logic;
		CLOCK_27 : in std_logic;
		
		AUD_ADCDAT : in std_logic;
		AUD_ADCLRCK : in std_logic;
		AUD_BCLK : in std_logic;
		AUD_DACDAT : out std_logic;
		AUD_DACLRCK : in std_logic
	);
END VGA_graphics;


ARCHITECTURE VGA_display OF VGA_graphics IS

	COMPONENT nios_system
		PORT (
			SIGNAL clk_clk: IN STD_LOGIC;
			SIGNAL reset_reset_n : IN STD_LOGIC;
			SIGNAL switches_export : IN STD_LOGIC_VECTOR (7 DOWNTO 0);
			SIGNAL keys_export : IN STD_LOGIC_VECTOR (7 DOWNTO 0);
			SIGNAL leds_export : OUT STD_LOGIC_VECTOR (7 DOWNTO 0);
			
			SIGNAL keyboard_CLK : INOUT STD_LOGIC;        
			SIGNAL keyboard_DAT : INOUT STD_LOGIC;
			
			SIGNAL vga_controller_CLK : OUT STD_LOGIC;
			SIGNAL vga_controller_HS : OUT STD_LOGIC;
			SIGNAL vga_controller_VS : OUT STD_LOGIC;
			SIGNAL vga_controller_BLANK : OUT STD_LOGIC;
			SIGNAL vga_controller_SYNC : OUT STD_LOGIC;
			SIGNAL vga_controller_R : OUT STD_LOGIC_VECTOR(9	downto	0);
			SIGNAL vga_controller_G : OUT STD_LOGIC_VECTOR(9	downto	0);
			SIGNAL vga_controller_B : OUT STD_LOGIC_VECTOR(9	downto	0);
			SIGNAL sram_DQ : INOUT STD_LOGIC_VECTOR(15	downto	0);
			SIGNAL sram_ADDR : OUT STD_LOGIC_VECTOR(17	downto	0);
			SIGNAL sram_LB_N : OUT STD_LOGIC;
			SIGNAL sram_UB_N : OUT STD_LOGIC;
			SIGNAL SRAM_CE_N : OUT	STD_LOGIC;
			SIGNAL SRAM_OE_N : OUT	STD_LOGIC;
			SIGNAL SRAM_WE_N : OUT	STD_LOGIC;
			
			SIGNAL sd_card_interface_b_SD_cmd : inout std_logic;
			SIGNAL sd_card_interface_b_SD_dat : inout std_logic;
			SIGNAL sd_card_interface_b_SD_dat3 : inout std_logic;
			SIGNAL sd_card_interface_o_SD_clock : out std_logic;
			
			SIGNAL av_config_interface_SDAT : inout STD_LOGIC;
			SIGNAL av_config_interface_SCLK : out STD_LOGIC;
			
			SIGNAL audio_interface_ADCDAT : in STD_LOGIC;
			SIGNAL audio_interface_ADCLRCK : in STD_LOGIC;
			SIGNAL audio_interface_BCLK : in STD_LOGIC;
			SIGNAL audio_interface_DACDAT : out STD_LOGIC;
			SIGNAL audio_interface_DACLRCK : in STD_LOGIC;
			
			SIGNAL audio_out_clk_clk : out STD_LOGIC;
			SIGNAL audio_in_clk_clk : in STD_LOGIC;
			
			SIGNAL sdram_clk_clk : OUT STD_LOGIC;
			SIGNAL sdram_wire_addr : OUT STD_LOGIC_VECTOR(11 DOWNTO 0);
			SIGNAL sdram_wire_ba : BUFFER STD_LOGIC_VECTOR(1 DOWNTO 0);
			SIGNAL sdram_wire_cas_n : OUT STD_LOGIC;
			SIGNAL sdram_wire_cke : OUT STD_LOGIC;
			SIGNAL sdram_wire_cs_n : OUT STD_LOGIC;
			SIGNAL sdram_wire_dq : INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
			SIGNAL sdram_wire_dqm : BUFFER STD_LOGIC_VECTOR(1 DOWNTO 0);
			SIGNAL sdram_wire_ras_n : OUT STD_LOGIC;
			SIGNAL sdram_wire_we_n : OUT STD_LOGIC
		);
	END COMPONENT;
	
	SIGNAL DQM : STD_LOGIC_VECTOR(1 DOWNTO 0);
   SIGNAL BA : STD_LOGIC_VECTOR(1 DOWNTO 0);
	
BEGIN
	   DRAM_BA_0 <= BA(0);
      DRAM_BA_1 <= BA(1);
      DRAM_UDQM <= DQM(1);
      DRAM_LDQM <= DQM(0);
		
	NiosII : nios_system
	PORT MAP(
		clk_clk => CLOCK_50,
		reset_reset_n => '1',
		switches_export => SW(7 DOWNTO 0),
		keys_export => KEY(7 DOWNTO 0),
		leds_export => LEDG(7 DOWNTO 0),
		
		keyboard_CLK => PS2_CLK,
		keyboard_DAT => PS2_DAT,
		
		sd_card_interface_b_SD_cmd => SD_CMD,
		sd_card_interface_b_SD_dat => SD_DAT,
		sd_card_interface_b_SD_dat3 => SD_DAT3,
		sd_card_interface_o_SD_clock => SD_CLK,
		 
		vga_controller_CLK	=>	VGA_CLK,
		vga_controller_HS	=>	VGA_HS,
		vga_controller_VS	=>	VGA_VS,
		vga_controller_BLANK	=>	VGA_BLANK,
		vga_controller_SYNC	=>	VGA_SYNC,
		vga_controller_R	=>	VGA_R,
		vga_controller_G	=>	VGA_G,
		vga_controller_B	=>	VGA_B,
		sram_DQ	=>	SRAM_DQ,
		sram_ADDR	=>	SRAM_ADDR,
		sram_LB_N	=>	SRAM_LB_N,
		sram_UB_N	=>	SRAM_UB_N,
		sram_CE_N	=>	SRAM_CE_N,
		sram_OE_N	=>	SRAM_OE_N,
		sram_WE_N	=>	SRAM_WE_N,
		
		sdram_clk_clk => DRAM_CLK,
		sdram_wire_addr => DRAM_ADDR,
      sdram_wire_ba => BA,
      sdram_wire_cas_n => DRAM_CAS_N,
      sdram_wire_cke => DRAM_CKE,
      sdram_wire_cs_n => DRAM_CS_N,
      sdram_wire_dq => DRAM_DQ,
      sdram_wire_dqm => DQM,
      sdram_wire_ras_n => DRAM_RAS_N,
      sdram_wire_we_n => DRAM_WE_N,
		 
		av_config_interface_SDAT => I2C_SDAT,
		av_config_interface_SCLK => I2C_SCLK,
			
		audio_interface_ADCDAT => AUD_ADCDAT,
		audio_interface_ADCLRCK => AUD_ADCLRCK,
		audio_interface_BCLK => AUD_BCLK,
		audio_interface_DACDAT => AUD_DACDAT,
		audio_interface_DACLRCK => AUD_DACLRCK,
			
		audio_out_clk_clk => AUD_XCK,
		audio_in_clk_clk => CLOCK_27
	);
END VGA_display;
