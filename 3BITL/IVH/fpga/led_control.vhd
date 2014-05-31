library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity led_control is
   port (
     CLK : in std_logic;
     RESET : in std_logic;
     HALF_OUT : out std_logic_vector(1 downto 0);
     LAYER_OUT : out std_logic_vector(3 downto 0);
     LEDS : in std_logic_vector(63 downto 0);
     LEDS_ACT : out std_logic_vector(7 downto 0);
	 ENABLE : in std_logic
    );
end entity;

architecture beh of led_control is
	signal leds_out : std_logic_vector(7 downto 0) := (others => '0');	-- vystupni vektor obsahujici aktualne svitici led
	signal cnt : integer range 0 to 10000000 := 0;		-- citac pro prepinani vrstev a polovin
	signal half : std_logic_vector(1 downto 0) := "01";		--vektor polovin
	signal layer : std_logic_vector(3 downto 0) := "0001";	--vektor vrstev
	signal act_leds : std_logic_vector(63 downto 0) := (others => '0');	-- vektor do nejz budeme nacitat aktualni data z SPI
begin
   
   --ziskava data z SPI. pouze pri ENABLE(indikuje zapis a tudiz zmenu). je zbytecne pri kazdem CLK kdyz nedojde ke zmene dat
	process (CLK, RESET, ENABLE)
	begin
		if (RESET = '1') then
			act_leds <= (others => '0');
		elsif (CLK'event) and (CLK = '1') and (ENABLE = '1') then
			act_leds <= LEDS;
		end if;
	end process;
   
   --vybere spravne led podle aktualne aktivni vrstvy a poloviny
   process (act_leds, half, layer)
   begin
		if (half = "01") then		-- "prava" polovina
			if layer = "0001" then
				leds_out <= act_leds(55 downto 48);
			elsif layer = "0010" then
				leds_out <= act_leds(39 downto 32);
			elsif layer = "0100" then
				leds_out <= act_leds(23 downto 16);
			elsif layer = "1000" then
				leds_out <= act_leds(7 downto 0);
			end if;
		
		elsif (half = "10") then		-- "leva" polovina
			if layer = "0001" then
				leds_out <= act_leds(63 downto 56);
			elsif layer = "0010" then
				leds_out <= act_leds(47 downto 40);
			elsif layer = "0100" then
				leds_out <= act_leds(31 downto 24);
			elsif layer = "1000" then
				leds_out <= act_leds(15 downto 8);
			end if;
		end if;
   end process;

   LEDS_ACT <= leds_out;	--vyvedeme led
   
   -- citac pro prepinani vrstev/polovin
	process (CLK, RESET)
	begin
		if(RESET = '1') then
			cnt <= 0;
		elsif (CLK'event) and (CLK = '1') then
			cnt <= cnt + 1;
			
			if (cnt = 10000) then		-- zvoleno experimentalne
				half <= half(0) & half(1);	-- zmenime svitici poloviny
			elsif (cnt = 20000) then		-- experimentalni*2 ..... Huzzah!!
				layer <= layer(2 downto 0) & layer(3);	--zmenime svitici vrstvy
				cnt <= 0;	-- vynulujeme citac
			end if;
		end if;

	end process;
	-- vyvedeme poloviny a vrstvy
	LAYER_OUT <= layer;
	HALF_OUT <= half;
end architecture;
