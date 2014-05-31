/* 
 ** ************************************************************
 ** 4. laboratorni uloha z IMP
 ** Mereni napeti na potenciometru pomoci AD prevodniku
 ** ------------------------------------------------------------ 
 ** Dle pokynu provedte inicializaci ADC ve funkci adc_init()
 ** a zajistete prepocet namerene hodnoty v obsluzne rutine
 ** preruseni od ADC a jeji korektni vypsani na displej LCD. 
 ** Podrobnosti jsou uvedeny ve funkcich init_adc() a ISR_adc().
 ** ************************************************************ 
 */

#include <hidef.h> 
#include "derivative.h" 
#include <stdio.h>
#include "string.h"

#define MAX_STR_LEN 16

/*  -----------------------------------
 *  prototypy pouzitych funkci
 *  -----------------------------------
 */
void delay(unsigned int n);
void spi_sendByte(byte b); 

void lcd_init(void); 
void lcd_sendCMD(byte cmd); 
void lcd_sendDAT(byte dat); 
void lcd_clear(void); 
void lcd_strDisplay(unsigned char s[MAX_STR_LEN], unsigned char line, unsigned char clr); 

void mcu_init(void);
void adc_init(void);

/*  -----------------------------------
 *  hlavni programova smycka
 *  -----------------------------------
 */
void main(void) 
{                              /* inicializace:                       */
  mcu_init();                  /*                MCU                  */
  lcd_init();                  /*                LCD                  */
  adc_init();                  /*                ADC                  */
  EnableInterrupts;            /*                povoleni preruseni   */
  
  for(;;);
}

void adc_init(void)
{
  // !!!! Sem doplnte kod pro inicializaci ADC modulu: !!!!
  // Nastavte 8-bit mod prevodu a jako zdroj hodin ADC zvolte
  // Asynchronous clock (vnitrni hodiny ADC). Dale povolte 
  // preruseni od ADC, nastavte kanal, na ktery je pripojen
  // potenciometr (viz schema pripravku) a rezim continuous 
  // conversion. Ostatni volby ponechte tak, jak jsou po resetu.
  ADCCFG_MODE = 0b00;      // 8bit mod
  ADCCFG_ADICLK = 0b11;    // asynchronous clock
  
  ADCSC1_AIEN = 1;         // interrupt enable
  ADCSC1_ADCH = 0b01000;   // channel 8 (data sheet 136)
  ADCSC1_ADCO = 1;         // continuous conversion
  
  
  
}

interrupt VectorNumber_Vadc void ISR_adc()
{
  // !!!! Sem doplnte zpracovani vysledku prevodu a zajistede jeho     !!!!
  // korektni vypsani na LCD displej (pouzijte preddefinovanou funkci
  // lcd_strDisplay(). Predpokladejte, ze potenciometr dava napeti od
  // 0 V (v prave krajni poloze) do 5 V (v leve krajni poloze). Provedte
  // prepocet vysledneho napeti s presnosti na 2 desetinna mista.
  // Pozn.: pouzijte pouze celociselnou aritmetiku.
  
  char number[6];
  int num = ADCRL * 5;
  sprintf(number, "%d.%02d",num/255,num%255*99/254);
  lcd_strDisplay(number,0,0);
  
}


/*  --------------------------------------------------------------------------
 *  pomocne funkce - prosim, nize uvedeny kod NEmodifikujte.
 *  --------------------------------------------------------------------------
 ****************************************************************************/
void delay(unsigned int n) 
{
	unsigned int i,j;
	for (i=0;i<n;i++)
    for (j=0;j<1;j++){ asm nop; }
} 

/* ----------------------------
 * funkce pro rizeni LCD 
 * ---------------------------- 
 */
void spi_sendByte(byte b) 
{
  while(!SPI2S_SPTEF);
  SPI2DL=b;
}

void lcd_sendCMD(byte cmd) 
{
  PTAD_PTAD2 = 0; /*  nahodit -SS */
  PTAD_PTAD3 = 0;
  spi_sendByte(cmd);
  while(!SPI2S_SPTEF);
  delay(50);
  PTAD_PTAD2 = 1;   /*  shodit -SS */
  delay(50);
}

void lcd_sendDAT(byte dat) 
{
  PTAD_PTAD2 = 0; /*  nahodit -SS */
  PTAD_PTAD3 = 1;
  spi_sendByte(dat);
  while(!SPI2S_SPTEF);
  delay(50);
  PTAD_PTAD2 = 1;   /*  shodit -SS */
  delay(50);
}

void lcd_init(void) 
{
  unsigned char j=0;
  unsigned int k=0;

  lcd_sendCMD(0x39); delay(50);
  lcd_sendCMD(0x14); delay(50);
  lcd_sendCMD(0x55); delay(50);
  lcd_sendCMD(0x6D); delay(50);
  lcd_sendCMD(0x78); delay(50);
  lcd_sendCMD(0x38); delay(200);
  lcd_sendCMD(0x0F); delay(50);
  lcd_sendCMD(0x01); delay(200);
  lcd_sendCMD(0x06); delay(50);
  lcd_sendCMD(0x02); delay(50);
  lcd_sendCMD(0x0C); delay(50);

//  lcd_sendCMD(0x02); delay(50);
//  sprintf(test_str, "IMP lab.3 (PWM)");
//  lcd_strDisplay(test, 0, 1);
//  sprintf(test_str, "RGB podsviceni");
//  lcd_strDisplay(str_tmp, 1, 0);

//  lcd_sendCMD(0x0F); delay(50);
}

void lcd_clear(void) 
{
  lcd_sendCMD(0x01); delay(200);
}

/* s - retezec znaku ke zobrazeni na LCD  */
/* line (0 nebo 1) - volba (horniho nebo spodniho) radku LCD  */
/* clr (0 nebo 1) - volba (ponechani nebo smazani obsahu LCD) pred zapisem retezce */
void lcd_strDisplay(unsigned char s[MAX_STR_LEN], unsigned char line, unsigned char clr) 
{
  unsigned int i;
  
  if(clr) lcd_clear();
  if(line==0) lcd_sendCMD(0x80); 
  else lcd_sendCMD(0xC0); 
  delay(200);
  
  for(i=0; s[i]!=0; i++) 
  {
    lcd_sendDAT(s[i]);
  }
}

/* ----------------------------
 * inicializace MCU
 * ---------------------------- 
 */
void mcu_init()
{
  SOPT1 = 0x13;                                      
  SOPT2 = 0x04;                                      
  SPMSC1 = 0x1C;                                      
  SPMSC2 = 0x00;                                      

  SPI2C1 = 0x00;                       
  SPI2C2 = 0x00;                                      
  SPI2BR = 0x04;                                      
  SPI2M = 0x00;                                      
  SPI2C1 = 0x50;                                      

  // nastavime si podsviceni LCD
  PTFDD |= (unsigned char) 0x0E;
  PTFD |= (unsigned char) 0x06;
  
  PTADD |= (unsigned char) 0x0C;                               
  PTAD |= (unsigned char) 0x0C;                               
}

/****************************************************************************/