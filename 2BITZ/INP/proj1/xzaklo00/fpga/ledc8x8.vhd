-- Library
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

-- Entity
entity ledc8x8 is
port (
RESET, SMCLK : in std_logic;
ROW, LED : out std_logic_vector (7 downto 0)
);
end entity ledc8x8;

-- Architecture
architecture behavioral of ledc8x8 is
-- signaly
	signal CLK_256 :std_logic;
	signal switch :std_logic;
	signal ce :std_logic;
	signal ctr_cnt :std_logic_vector (21 downto 0);
	signal row_cnt :std_logic_vector (7 downto 0) := "11111110";
	signal jmeno :std_logic_vector (7 downto 0);

begin
-- citac
	ctrl_cnt: process (SMCLK, RESET) is
	begin
		if (RESET = '1') then
			ctr_cnt <= "0000000000000000000000";
		elsif (SMCLK'event) and (SMCLK = '1') then
			ctr_cnt <= ctr_cnt + '1';
		end if;
	end process ctrl_cnt;

switch <= ctr_cnt(21);

ce <='1' when ctr_cnt(7 downto 0) = "11111111" else '0';

-- rotacni registr
	rows_cnt: process (RESET,SMCLK,ce)
	begin
		if (RESET = '1') then
			row_cnt <= "11111110";
		elsif (SMCLK'event and SMCLK = '1' and ce = '1') then
			row_cnt <= row_cnt(0) & row_cnt(7 downto 1);
		end if;
		
	end process rows_cnt;

-- dekoder
	dec: process (row_cnt) is
	begin
		case (row_cnt) is
		when "01111111" =>
			jmeno <= "11110000";
		when "10111111" =>
			jmeno <= "00010000";
		when "11011111" =>
			jmeno <= "00100000";
		when "11101111" =>
			jmeno <= "01001001";
		when "11110111" =>
			jmeno <= "10000101";
		when "11111011" =>
			jmeno <= "11110011";
		when "11111101" =>
			jmeno <= "00000101";
		when "11111110" =>
			jmeno <= "00001001";
		when others =>
			jmeno <= "00000000";
		end case;
	end process dec;

-- multiplexor
	mux: for i in 0 to 7 generate
		ROW(i) <= row_cnt(i);
		LED(i) <= jmeno(i) when switch = '0' else '0';
	end generate;
end behavioral;
