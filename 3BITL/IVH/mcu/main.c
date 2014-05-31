/*******************************************************************************
   main.c: main for blockram demo application
   Copyright (C) 2009 Brno University of Technology,
                      Faculty of Information Technology
   Author(s): Zdenek Vasicek <vasicek AT fit.vutbr.cz>

   LICENSE TERMS

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
   3. All advertising materials mentioning features or use of this software
      or firmware must display the following acknowledgement:

        This product includes software developed by the University of
        Technology, Faculty of Information Technology, Brno and its
        contributors.

   4. Neither the name of the Company nor the names of its contributors
      may be used to endorse or promote products derived from this
      software without specific prior written permission.

   This software or firmware is provided ``as is'', and any express or implied
   warranties, including, but not limited to, the implied warranties of
   merchantability and fitness for a particular purpose are disclaimed.
   In no event shall the company or contributors be liable for any
   direct, indirect, incidental, special, exemplary, or consequential
   damages (including, but not limited to, procurement of substitute
   goods or services; loss of use, data, or profits; or business
   interruption) however caused and on any theory of liability, whether
   in contract, strict liability, or tort (including negligence or
   otherwise) arising in any way out of the use of this software, even
   if advised of the possibility of such damage.

   $Id$

*******************************************************************************/
/*******************************************************************************
 * Tento zdrojovy kod byl pouzit jako kostra pro projekt Led cube (predmet IVH).
 * 
 * Autori: 	Katerina Zaklova (xzaklo00)
 * 			Mario Wanka (xwanka00)
 * 
*******************************************************************************/

#include<stdlib.h>
#include <fitkitlib.h>
unsigned long long field64bit = 0;

/*******************************************************************************
 * Vypis uzivatelske napovedy (funkce se vola pri vykonavani prikazu "help")
*******************************************************************************/
void print_user_help(void){
}

/*******************************************************************************
 * Dekodovani uzivatelskych prikazu a jejich vykonavani
*******************************************************************************/
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd){
  return (USER_COMMAND);
}
//Zapise zmenu 
void write_spi(){
	FPGA_SPI_RW_A8_DN(SPI_FPGA_ENABLE_WRITE,0x00,(unsigned char *)&field64bit,8);		//*FITKIT SVN repository*/mcu/libfitkit/fpga.c
}
//Rozsviti led na zadanych souradnicich
void led_on(char x, char y, char z){
	field64bit |= ((unsigned long long)1<<(z*16+x*4+y));
}
//Zhasne led na zadanych souradnicich
void led_off(char x, char y, char z){
	field64bit &= (~((unsigned long long)1<<(z*16+x*4+y)));
}
//Invertuje stav led na zadanych souradnicich
void led_flip(char x, char y, char z){
	field64bit ^= ((unsigned long long)1<<(z*16+x*4+y));
}
//Vrati kostku do pocatecniho stavu(vse vypnuto)
void cube_off(void){
	field64bit = 0;
  write_spi();
}

/*******************************************************************************
 * Inicializace periferii/komponent po naprogramovani FPGA
*******************************************************************************/
void fpga_initialized(){
}


/*******************************************************************************
 * Animation functions declaration
*******************************************************************************/
void showDevil(void);
void runTheFloors(void);
void showFire(int sec);
void showPaparazi(int sec);
void showAim(int x, int y, int z);

/*******************************************************************************
 * Hlavni funkce
*******************************************************************************/
//INLINE INT INIT_MCU(VOID)
int main(void)
{
	unsigned int counter = 0;
	initialize_hardware();
  cube_off();
  
  showPaparazi(2);
  cube_off();
  
  showDevil();
  delay_ms(2500);
  cube_off();
  
  showFire(4);
  cube_off();
  
  showAim(2,1,1);
  cube_off();
  
  showAim(0,2,2);
  cube_off();
  
  showAim(2,1,2);
  cube_off();
  
  showDevil();
  

  
  
	while (1){
		delay_ms(1);  //zpozdeni 1ms
		counter++;
		if (counter == 500) 
		{
			flip_led_d6(); //invertovat LED
			counter = 0;
		}
		terminal_idle();  // obsluha terminalu
	}

}


/******************************************************************************
 * Definition of animation functions
 *****************************************************************************/
//animates picture, that should lool like a head of devil with horns
void showDevil(void){
      led_on(0,1,0);
      led_on(0,2,0);
      led_on(1,1,0);
      led_on(1,2,0);
      led_on(2,1,0);
      led_on(2,2,0);
      led_on(3,1,0);
      led_on(3,2,0);
      led_on(1,1,1);
      led_on(2,1,1);
      led_on(1,2,1);
      led_on(2,2,1);
      led_on(1,1,2);
      led_on(2,1,2);
      led_on(1,2,2);
      led_on(2,2,2);
      led_on(0,3,2);
      led_on(3,3,2);
      led_on(0,3,3);
      led_on(3,3,3);
      
      write_spi();
}

//one line walkthought the whole cube
void runTheFloors(void){
  char x, y, z;
  for(z=0; z<4; z++){
    for(y=0; y<4; y++){
      for(x=0; x<4; x++){
        led_on(x,y,z);
        write_spi();
        delay_ms(100);
      }
    }
  }
}

void setFireColumn(int x, int y, int hight){ //lights up given count of LEDs
  led_off(x,y,1);
  led_off(x,y,2);
  led_off(x,y,3);
  
  int i;
  if(hight<4){
    for(i=1; i<hight; i++)
      led_on(x,y,i);
  }
  else{
    hight-=3;
    for(i=1; i<hight; i++)
      led_on(x,y,i);
  }
}

void showFire(int sec){
  //floor 0 will always shine
  int i, j, k;
  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      led_on(i,j,0);
    }
  }
  write_spi();
  
  int level[16]={5,0,3,2,1,4,3,5,1,4,0,2,5,0,3,4};

  //while theres time left
  int time=0;
  while((time/1000)<sec){
    //"count" new value for each column
    k=0;
    for(i=0; i<4; i++){
      for(j=0; j<4; j++){
        setFireColumn(i, j, level[k]);
        k++;
      }
    }
    for(i=0; i<16; i++){
      level[i]++;
      if(level[i]>5)
        level[i]=0;
    }
    write_spi();
    time+=200;
    delay_ms(200);
  }
}

//photography storm-like animation, random flashes
void showPaparazi(int sec){
  int time=0;
  int x,y,z;
  while((time/1000)<sec){
    x=rand()%4;
    y=rand()%4;
    z=rand()%4;
    
    led_on(x,y,z);
    write_spi();
    
    delay_ms(30);
    
    led_off(x,y,z);
    write_spi();
    
    time+=50;
  }
}

//point aiming animation
void flashWall(int axis, int val){ //axis: 0-x; 1-y; 2-z
  int i, j;
  if(axis==0){ //x-wall (side)
    for(j=0; j<4; j++){
      for(i=0; i<4; i++){
        led_on(val, j, i);
      }
    }
  }
  else if(axis==1){ //y-wall (depth)
    for(j=0; j<4; j++){
      for(i=0; i<4; i++){
        led_on(i, val, j);
      }
    }
  }
  else{ //z-wall (hight)
    for(j=0; j<4; j++){
      for(i=0; i<4; i++){
        led_on(i, j, val);
      }
    } 
  }
  write_spi();
}

void showAim(int x, int y, int z){
  led_on(x,y,z);
  write_spi();
  delay_ms(500);
  
  //side wall
  int i=0, j=0;
  while(i<=x){
    flashWall(0, i);
    delay_ms(400);
    cube_off();
    led_on(x,y,z);
    write_spi();
    i++;
  }
  
  //setting first aim laser
  cube_off();
  led_on(x,y,z);
  for(j=0; j<7; j++){
    for(i=0; i<4; i++){
      if(i!=y)
        led_flip(x, i, z);
    }
    write_spi();
    delay_ms(150);
  }
  
  //depth wall
  i=0;
  while(i<=y){
    flashWall(1, i);
    delay_ms(400);
    cube_off();
    for(j=0; j<4; j++)
      led_on(x, j, z);
    write_spi();
    i++;
  }
  
  //setting second aim laser
  cube_off();
  led_on(x,y,z);
  for(j=0; j<7; j++){
    for(i=0; i<4; i++){
      if(i!=y)
        led_flip(x, i, z);
    }
    for(i=0; i<4; i++){
      if(i!=z)
        led_flip(x, y, i);
    }
    write_spi();
    delay_ms(150);
  }
  
  //hight wall
  i=0;
  while(i<=z){
    flashWall(2, i);
    delay_ms(400);
    cube_off();
    for(j=0; j<4; j++)
      led_on(x, y, j);
    write_spi();
    i++;
  }
  
  //setting third aim laser
  cube_off();
  led_on(x,y,z);
  for(j=0; j<7; j++){
    for(i=0; i<4; i++){
      if(i!=y)
        led_flip(x, i, z);
    }
    for(i=0; i<4; i++){
      if(i!=z)
        led_flip(x, y, i);
    }
    for(i=0; i<4; i++){
      if(i!=x)
        led_flip(i, y, z);
    }
    write_spi();
    delay_ms(150);
  }
  delay_ms(500);
}











