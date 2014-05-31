library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

architecture main of tlv_gp_ifc is
	-- signaly pro rizeni rozviceni led
	signal led_vect : std_logic_vector(63 downto 0) := (others => '0');		-- vector obsahujici stav vsech led (data z SPI)
	signal layer : std_logic_vector(3 downto 0) := "0001";
	signal half : std_logic_vector(1 downto 0) := "01";	
	signal led_act : std_logic_vector(7 downto 0) := (others => '0');
	signal spi_addr : std_logic_vector(5 downto 0):=(others=>'0');
	signal spi_in : std_logic_vector(63 downto 0) := (others => '0');
	signal spi_enable : std_logic;
  

   component SPI_adc
      generic (
         ADDR_WIDTH : integer;
         DATA_WIDTH : integer;
         ADDR_OUT_WIDTH : integer;
         BASE_ADDR  : integer
      );
      port (
         CLK      : in  std_logic;

         CS       : in  std_logic;
         DO       : in  std_logic;
         DO_VLD   : in  std_logic;
         DI       : out std_logic;
         DI_REQ   : in  std_logic;

         ADDR     : out std_logic_vector (ADDR_OUT_WIDTH-1 downto 0);
         DATA_OUT : out std_logic_vector (DATA_WIDTH-1 downto 0);
         DATA_IN  : in  std_logic_vector (DATA_WIDTH-1 downto 0);

         WRITE_EN : out std_logic;
         READ_EN  : out std_logic
      );
   end component;

begin

	led_ctrl: entity work.led_control port map(
		CLK => clk,
		RESET => reset,
		LEDS => led_vect,
		HALF_OUT => half,
		LAYER_OUT => layer,
		LEDS_ACT => led_act,
		ENABLE => spi_enable
	);
	--mapovani led
	X(7)  <= led_act(4);
	X(9)  <= led_act(5);
	X(11) <= led_act(6);
	X(13) <= led_act(7);
	
	X(15) <= led_act(3);
	X(17) <= led_act(2);
	X(19) <= led_act(1);
	X(21) <= led_act(0);
	-- mapovani polovin
	X(23) <= half(0);
	X(25) <= half(1);
	
	-- mapovani vrstev
	X(27) <= not layer(3);
	X(29) <= not layer(2);
	X(31) <= not layer(1);
	X(33) <= not layer(0);

   spidecd: SPI_adc
      generic map (
         ADDR_WIDTH => 8,
         DATA_WIDTH => 64,      -- sirka dat 64 bitu(64 led -> 1 bit pro kazdou)
         ADDR_OUT_WIDTH => 6,  
         BASE_ADDR  => 16#00#
      )
      port map (
         CLK      => CLK,
         CS       => SPI_CS,

         DO       => SPI_DO,
         DO_VLD   => SPI_DO_VLD,
         DI       => SPI_DI,
         DI_REQ   => SPI_DI_REQ,

         ADDR     => spi_addr,
         DATA_OUT => led_vect,
         DATA_IN  => spi_in,
         WRITE_EN => spi_enable,
         READ_EN  => open
      );

end main;
 