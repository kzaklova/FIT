/* 
 ** ***********************************************************
 ** 
 ** 3. laboratorni uloha z IMP
 ** Pulsni sirkova modulace (PWM) s vyuzitim casovace TPM1
 ** (ovladani RGB podsviceni LCD zmenou stridy PWM signalu)
 ** 
 ** ----------------------------------------------------------- 
 ** Nastavte parametry casovace TPM1 tak, aby spravnym 
 ** generovanim PWM signalu na kazdem z kanalu 3(R), 4(G), 5(B) 
 ** casovace bylo mozne zajistit zvolene R, G, B podsviceni 
 ** znakového LCD na laboratornim kitu
 ** (detaily viz funkce timer_init())
 ** ----------------------------------------------------------- 
 ** 
 ** Vyvody TPM1   Kanal barevne slozky
 ** ------------  --------------------
 ** PTE2/TPM1CH0  -
 ** PTE3/TPM1CH1  -
 ** PTF0/TPM1CH2  -
 ** PTF1/TPM1CH3  Cervena (Red)
 ** PTF2/TPM1CH4  Zelena (Green)
 ** PTF3/TPM1CH5  Modra (Blue)
 ** 
 ** *********************************************************
 */

#include <hidef.h> 
#include "derivative.h" 
#include <stdio.h>
#include "string.h"

/*  -----------------------------------
 *  definice k tlacitkum a LED
 *  -----------------------------------
 */
#define BTN_PORT1D PTGD
#define BTN_PORT2D PTDD
#define LED_PORTD PTCD
#define LED_PORTDD PTCDD

#define BTN_LED_UP 4
#define BTN_LED_DOWN 8
#define BTN_LED_LEFT 16
#define BTN_LED_RIGHT 32
#define BTN_LED_OK 64

#define BTN_LED_ALL (BTN_LED_UP | BTN_LED_DOWN | BTN_LED_LEFT | BTN_LED_RIGHT | BTN_LED_OK )

#define BTN_UP 1
#define BTN_DOWN 2 
#define BTN_LEFT 4
#define BTN_RIGHT 8 
#define BTN_OK 16

/*  -----------------------------------
 *  definice k RGB modelu barev
 *  -----------------------------------
 */
#define COL_LEV_MAX 250   /* max. hodnota barevne slozky (8-bit)  */
#define COL_INC_STEP 25   /* hodnota kroku zmeny barevne slozky   */
enum EnumColorChan {chanR=3, chanG=4, chanB=5} color_chan = chanB;
unsigned char valR=0, valG=0, valB=0;

#define MAX_STR_LEN 16
char str_R[MAX_STR_LEN] = "Cervena (Red)";
char str_G[MAX_STR_LEN] = "Zelena (Green)";
char str_B[MAX_STR_LEN] = "Modra (Blue)";

/*  -----------------------------------
 *  pomocne promenne
 *  -----------------------------------
 */
char str_tmp[MAX_STR_LEN];
unsigned int i;

/*  -----------------------------------
 *  prototypy pouzitych funkci
 *  -----------------------------------
 */
void delay(unsigned int n);

void btn_leds_off(unsigned char mask); 
void btn_leds_on(unsigned char mask); 
void btn_leds_init(unsigned int d);
unsigned int btn_state(); 

unsigned char* enum2str(enum EnumColorChan actChan); 
enum EnumColorChan prevChan(enum EnumColorChan actChan) ;
enum EnumColorChan nextChan(enum EnumColorChan actChan) ;

void spi_sendByte(byte b); 

void lcd_init(void); 
void lcd_sendCMD(byte cmd); 
void lcd_sendDAT(byte dat); 
void lcd_clear(void); 
void lcd_strDisplay(unsigned char s[MAX_STR_LEN], unsigned char line, unsigned char clr); 

void mcu_init(void);
void timer_init(void);




/* ========================================================================= */
/* ========================================================================= */




/*  -----------------------------------
 *  hlavni programova smycka            =======================================
 *  -----------------------------------
 */
void main(void) 
{                              /* inicializace:                              */
  mcu_init();                  /*                MCU                         */
  btn_leds_init(20000);        /*                tlacitek/KBI+LED            */

  timer_init();                /*                TPM1                        */
                               /*     (!!! doplnte telo teto funkce !!!)     */

  lcd_init();                  /*                LCD                         */
  EnableInterrupts;            /*                povoleni preruseni          */
  
  for(;;);                     
}

/*  -----------------------------------
 *  funkce pro nastaveni casovace TPM1  =======================================
 *  -----------------------------------
 */
void timer_init(void) 
{
  /*
   *  !!! doplnte telo teto funkce  !!!
   */

  /* 1) vypnuti casovace                                                     */

  /* 2) nulovani casovace                                                    */
  
  /* 3) nastaveni koncove hodnoty citani (cyklu citace)                      */
  /*    (nastavte na 250 - pojmenovana COL_LEV_MAX)                          */  
  
  /* 4) nastaveni kazdeho z kanalu 3(R), 4(G), 5(B) casovace do rezimu       */
  /*    "Edge-aligned PWM: high-true pulses, clear on compare", tj.:         */
  /*    - PWM se zarovnanim na hranu,                                        */
  /*    - uroven signalu na kanalu bude na zacatku periody casovace v log.1, */
  /*    - pri komparaci prejde uroven signalu na kanalu do log.0.            */
  /*                                                                         */ 
  /*   pozn.1 (pro bystre vyplyne z vyse uvedeneho):                         */ 
  /*   - do konfiguracniho registru kazdeho z kanalu ulozte tutez hodnotu    */
  /*   zajistujici vyse popsane chovani                                      */
  /*                                                                         */ 
  /*   pozn.2 (pro bystre vyplyne z vyse uvedeneho):                         */ 
  /*   - intenzita svitu prislusne R, G, B, slozky je urcena zvolenou        */
  /*     stridou PWM signalu generovaneho na prislusnem kanalu casovace      */
  
  /* 5) nastaveni hodnot pracovniho (Value) registru kazdeho z kanalu        */
  /*    3(R), 4(G), 5(B) casovace na hodnotu pro komparaci, tj. na:          */
  /*    - hodnotu valR pro kanal 3 (R)                                       */
  /*    - hodnotu valG pro kanal 4 (G)                                       */
  /*    - hodnotu valB pro kanal 5 (B)                                       */
  
  /* 6) zapnuti casovace                           */
  /*    casovac spustte s nasledujicimi parametry: */
  /*    - zdroj hodin: "Bus rate clock"            */
  /*    - preddeleni:   128                        */
  
  TPM1SC = 0; //vypnuti
  TPM1CNT = 0; //nulovani
  TPM1MOD = COL_LEV_MAX; //nastaveni koncove hodnoty
  TPM1C3SC = TPM1C4SC = TPM1C5SC = 0b10101000; //nastaveni jednotlivych kanalu
  TPM1C3V = valR; //value pro R
  TPM1C4V = valG; //value pro G
  TPM1C5V = valB; //value pro B
  TPM1SC = 0b10001111; //zapnuti casovace
  
}




/* ========================================================================= */
/* ========================================================================= */




/*  -----------------------------------
 *  pomocne funkce
 *  -----------------------------------
 */
void delay(unsigned int n) 
{
	unsigned int i,j;
	for (i=0;i<n;i++)
    for (j=0;j<1;j++){ asm nop; }
} 

/*  -----------------------------------
 *  funkce pro BTN/KBI, LED 
 *  -----------------------------------
 */

void btn_leds_off(unsigned char mask) 
{
  LED_PORTD &= (~mask); /* LEDs off */
}

void btn_leds_on(unsigned char mask) 
{
  LED_PORTD |= mask; /* LEDs on */
}

void btn_leds_init(unsigned int d)
{
  /* inicializace portu tlacitek/KBI */
  KBISC_KBIE = 1;             // povoleni preruseni od KBI 
  KBIPE |= 0b11000111;        // aktivace KBI vyvodu 7,6,x,x,x,2,1,0 (maska: 1100.0111)
  KBIES = 0;                  // nastaveni citlivosti KBI: sestupna hrana

  /* inicializace portu a stavu LED */
  LED_PORTDD = BTN_LED_ALL;   // PTD2-6 na OUT 
  btn_leds_on(BTN_LED_UP);    // test kazde LED
  delay(d);
  btn_leds_on(BTN_LED_RIGHT);
  delay(d);
  btn_leds_on(BTN_LED_DOWN);
  delay(d);
  btn_leds_on(BTN_LED_LEFT);
  delay(d);
  btn_leds_on(BTN_LED_OK);
  delay(d);
  btn_leds_off(BTN_LED_ALL);  // zhasnuti vsech LED
}

unsigned int btn_state() 
{
  unsigned char btn_state = 0;

  if(~BTN_PORT1D & BTN_UP) btn_state |= BTN_UP;
  if(~BTN_PORT1D & BTN_DOWN) btn_state |= BTN_DOWN;
  if(~BTN_PORT1D & BTN_LEFT) btn_state |= BTN_LEFT;
  if(~BTN_PORT1D & BTN_RIGHT) btn_state |= BTN_RIGHT;
  if(~BTN_PORT2D & 4) btn_state |= BTN_OK;

  return(btn_state);
}

interrupt VectorNumber_Vkeyboard void ISR_kbi() 
{
  unsigned int tmp_i= btn_state();        
  unsigned char val;
  
  btn_leds_on(tmp_i*4);       
  delay(5000);                                                  // odstraneni zakmitu
  if(tmp_i != btn_state()) tmp_i=0;        
  btn_leds_off(BTN_LED_ALL);  
  delay(10000);     
  
  if(tmp_i & BTN_UP)                                            // nahoru (RGB slozka++)
  {  
    switch(color_chan) 
    {
      case chanR: if(valR+COL_INC_STEP>COL_LEV_MAX){valR=0;} else valR+=COL_INC_STEP; break;
      case chanG: if(valG+COL_INC_STEP>COL_LEV_MAX){valG=0;} else valG+=COL_INC_STEP; break;
      case chanB: if(valB+COL_INC_STEP>COL_LEV_MAX){valB=0;} else valB+=COL_INC_STEP; break;
    }
  }
  if(tmp_i & BTN_DOWN)                                          // dolu (RGB slozka--)
  {  
    switch(color_chan) 
    {
      case chanR: if(valR-COL_INC_STEP<0){valR=COL_LEV_MAX;} else valR-=COL_INC_STEP; break;
      case chanG: if(valG-COL_INC_STEP<0){valG=COL_LEV_MAX;} else valG-=COL_INC_STEP; break;
      case chanB: if(valB-COL_INC_STEP<0){valB=COL_LEV_MAX;} else valB-=COL_INC_STEP; break;
    }
  }
  if(tmp_i & BTN_LEFT) { color_chan = prevChan(color_chan); }   // vlevo (volba RGB slozky)
  if(tmp_i & BTN_RIGHT) { color_chan = nextChan(color_chan); }  // vpravo (volba RGB slozky)
  if(tmp_i & BTN_OK)                                            // ok (aplikace RGB nastaveni)
  { 
    timer_init();
  } 
    
  if(tmp_i)                                                     // byl stisk -> LCD refresh
  {
    i = sprintf(str_tmp, "%s", enum2str(color_chan));
    lcd_strDisplay(str_tmp, 0, 1);

    switch(color_chan) 
    {
      case chanR: val=valR; break;
      case chanG: val=valG; break;
      case chanB: val=valB; break;
    }

    i = sprintf(str_tmp, "Hodnota: %d", val);
    lcd_strDisplay(str_tmp, 1, 0);

    delay(100);
  }

  KBISC_KBACK = 1;                                              // potvrzeni dokonceni ISR obsluhy KBI
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

  valR = 125;  valG = 0;  valB = 0;  /* posvitit cervenou (R) */
  timer_init();
  delay(50000);
  valR = 0;  valG = 125;  valB = 0;  /* posvitit zelenou (G) */
  timer_init();
  delay(50000);
  valR = 0;  valG = 0;  valB = 125;  /* posvitit modrou (B) */
  timer_init();
  delay(50000);
  delay(50000);
  delay(50000);

  lcd_sendCMD(0x02); delay(50);
  i = sprintf(str_tmp, "IMP lab.3 (PWM)");
  lcd_strDisplay(str_tmp, 0, 1);
  i = sprintf(str_tmp, "RGB podsviceni");
  lcd_strDisplay(str_tmp, 1, 0);

  valR = 125;  valG = 100;  valB = 50; /* posvitit sedomodrou */
  timer_init();

  k = 50000;  
  delay(k); delay(k); delay(k); delay(k);                              

  for(i=0;i<15;i++) 
  { 
    lcd_sendCMD(0x1C); delay(10000);
  }

  i = sprintf(str_tmp, "Prosim stisk");
  lcd_strDisplay(str_tmp, 0, 1);
  i = sprintf(str_tmp, "tlacitka ...");
  lcd_strDisplay(str_tmp, 1, 0);

  for(i=0;i<15;i++) 
  {
    lcd_sendCMD(0x18); delay(10000);
  }

  lcd_sendCMD(0x0F); delay(50);
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
 * funkce pro praci s RGB typy
 * ---------------------------- 
 */
unsigned char* enum2str(enum EnumColorChan actChan) 
{
  switch(actChan) 
  {
    case(chanR): return(str_R); break;
    case(chanG): return(str_G); break;
    case(chanB): return(str_B); break;
  }
}

enum EnumColorChan nextChan(enum EnumColorChan actChan) 
{
  switch(actChan) 
  {
    case(chanR): return(chanG); break;
    case(chanG): return(chanB); break;
    case(chanB): return(chanR); break;
  }
}

enum EnumColorChan prevChan(enum EnumColorChan actChan) 
{
  switch(actChan) 
  {
    case(chanR): return(chanB); break;
    case(chanG): return(chanR); break;
    case(chanB): return(chanG); break;
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

  PTAD |= (unsigned char)0x0C;                               
  PTADD |= (unsigned char)0x0C;                               
}
