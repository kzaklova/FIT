/**
 * Katerina Zaklova, xzaklo00
 * 75% zmen
 * 16/12/12
 **/
   
/*******************************************************************************
   main.c: LCD + keyboard demo
   Copyright (C) 2009 Brno University of Technology,
                      Faculty of Information Technology
   Author(s): Zdenek Vasicek <vasicek AT stud.fit.vutbr.cz>

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



#include <fitkitlib.h>
#include <keyboard/keyboard.h>
#include <lcd/display.h>
#include <stdio.h>
#include "string.h"

char last_ch = 0;
int last = 0;
enum {OPER = 1, NUM, BRL, BRR, END};

int number[100];
int oper[100];
int posn, poso;
int mul = 0;
int div = 0;  
int badd = 0;
int bsub = 0;
int bmul = 0;
int bdiv = 0;
int brack = 0;
int error = 0;

#define DISPL_RNG 32
#define ERR 1

char p = 0;
char LCD_arr[DISPL_RNG + 1];

/*******************************************************************************
 * Vypis uzivatelske napovedy (funkce se vola pri vykonavani prikazu "help")
*******************************************************************************/
void print_user_help(void)
{
}

void show_LCD(char new)
{
	/*term_send_str("Tisk :");
	term_send_char(new);
	term_send_char(':');
	term_send_char(LCD_arr[p]);
	term_send_crlf();
	term_send_crlf();*/


	if(p < DISPL_RNG)
	{
		if(LCD_arr[p] != 0)
			p++;
		LCD_arr[p] = new;
	}
	else
	{
		int i = 0;
		while(i < DISPL_RNG - 1)
		{
			LCD_arr[i] = LCD_arr[i + 1];
			i++;
		}

		LCD_arr[DISPL_RNG - 1] = new;
	}

	LCD_clear();
	LCD_write_string(LCD_arr);
}

void countResult() {
  char buf[7];
  int n = 0;
  int o = 0;
  posn--;
	poso--;
  
	while ((n < posn) && (o <= poso)) {
		if (oper[o] == '+')
			number[n + 1] = number[n] + number[n + 1];
		else if (oper[o] == '-')
			number[n + 1] = number[n] - number[n + 1];
		n++;
		o++;
	}
	term_send_crlf();
	term_send_crlf();
	term_send_str("Vysledek :");
	term_send_num(number[posn]);
	term_send_crlf();
	term_send_crlf();
  
  sprintf(buf, "%d", number[posn]);
  LCD_clear();
	LCD_write_string(buf);
}

void parseInput(char c)
{
	char out = c;
	int tmp;
	switch(c)
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
    last = NUM;
			tmp = number[posn];
			number[posn] = tmp * 10 + (c - '0');
			if (oper[0] != 0 && mul == 0 && div == 0 && badd == 0 && bsub == 0 && bmul == 0 && bdiv == 0)
				poso++;
			break;

		case '+':
		case '-':
		
		case 'B':
		case '/':
		
		case 'C':
		case '(':
		
		case 'D':
		case ')':
		
		case '*':
		case 'A':
		case '#':
		case '=':
    
      if ((last == OPER) && (c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == '#'))
        error = ERR;
        
      if (c == 'A' || c == 'B' || c == 'C' || c == 'D')
        last = OPER;
      
			switch(c)
			{
				case 'A':
					out = '+';
					break;

				case 'B':
					out = '-';
					break;

				case 'C':
					out = '*';
					break;

				case 'D':
					out = '/';
					break;

				case '#':
					out = '=';
					break;
          
				case '*':
          if (last == 0 || last == OPER || last == BRL) 
					  out = '(';
          else if (last == NUM || last == BRR) 
					  out = ')';
          else
            out = '(';
					break;

				default:
					break;
			}
      
      if (out == '(') {
        last = BRL;
        brack++;
      }
      if (brack > 0) {
        if(last != BRL )
          posn++;
        if (bmul == 1) {
          posn--;
          number[posn - 1] = number[posn - 1] * number[posn];
          number[posn] = 0;
          bmul = 0;
        }
        if (bdiv == 1) {
          posn--;
          number[posn - 1] = number[posn - 1] / number[posn];
          number[posn] = 0;
          bdiv = 0;
        }
        if (badd == 1) {
          if (c != 'C' && c != 'D') {
            posn--;
            number[posn - 1] = number[posn - 1] + number[posn];
            number[posn] = 0;
            badd = 0;
          }
        }
        if (bsub == 1) {
          if (c != 'C' && c != 'D') {
            posn--;
            number[posn - 1] = number[posn - 1] - number[posn];
            number[posn] = 0;
            bsub = 0;
          }
        }
  			switch(c)
  			{
  				case 'A':
  					badd = 1;
  					break;
  
  				case 'B':
  					bsub = 1;
  					break;
  
  				case 'C':
  					bmul = 1;
  					break;
  
  				case 'D':
  					bdiv = 1;
  					break;
  
  				default:
  					break;
  			}
        if (out != ')'){
          show_LCD(out);
          return;
        }
      }
      if (out == ')') {
        if (brack <= 0)
          error = ERR;
        posn--;
        last = BRR;
        brack--;
      }
        
				if (mul == 1) {
					mul = 0;
					number[posn - 1] = number[posn - 1] * number[posn];
					number [posn] = 0;
				}
				else if (div == 1) {
					div = 0;
					number[posn - 1] = number[posn - 1] / number[posn];
          if (number[posn] == 0)
            error = ERR;
					number [posn] = 0;
				}
				else
          posn++;
          
        if (last == BRR)
          posn--;
          
				if (c == 'A') {
					oper[poso] = '+';
				}
				else if (c == 'B') {
					oper[poso] = '-';
				}
				else if (c == 'C')
					mul = 1;
				else if (c == 'D')
					div = 1;

     	if (out == '=') {
        if (brack > 0)
          error = ERR;
        if (last == BRR)
          posn++;
        last = END;
        show_LCD(out);
        if (mul == 1) {
          mul = 0;
          if (poso == 0)
            poso++;
          number[posn - 1] = number[posn - 1] * number[posn]; 
          number[posn] = 0;
          posn--; 
        }
        if (div == 1) {
          div = 0;
          if (poso == 0)
            poso++;
          if (number[posn] == 0)
            error = ERR;
          number[posn - 1] = number[posn - 1] / number[posn]; 
          number[posn] = 0;
          posn--; 
        }

        if (error == ERR) {
        	term_send_crlf();
	        term_send_str("Error");
	        term_send_crlf();
          LCD_clear();
	        LCD_write_string("Error");
          return;
        }
        countResult();
        return;
			}
			break;
	}
	
/*	term_send_crlf();
	term_send_crlf();
	term_send_str("Tisk :");
	term_send_char(out);
	term_send_crlf();
	term_send_crlf();  */

  show_LCD(out);
}

/*******************************************************************************
 * Obsluha klavesnice
*******************************************************************************/
int keyboard_idle()
{
	char ch;
	ch = key_decode(read_word_keyboard_4x4());

	if (ch != last_ch)
	{
		last_ch = ch;
		if (ch != 0)
		{
			parseInput(ch);
      if (last == END) {
        mul = 0;
        div = 0;  
        badd = 0;
        bsub = 0;
        bmul = 0;
        bdiv = 0;
        brack = 0;
        error = 0;
        posn = 0;
        poso = 0;
        p = 0;
        int i;
        for (i = 0; i < 100; i++) {
          number[i] = 0;
          oper[i] = 0;
          if (i < DISPL_RNG)
            LCD_arr[i] = 0;
        }
      }  
            
		}
	}
	return 0;
}

/*******************************************************************************
 * Dekodovani a vykonani uzivatelskych prikazu
*******************************************************************************/
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
	return CMD_UNKNOWN;
}

/*******************************************************************************
 * Inicializace periferii/komponent po naprogramovani FPGA
*******************************************************************************/
void fpga_initialized()
{
	LCD_init();
	LCD_clear();
}

/*******************************************************************************
 * Hlavni funkce
*******************************************************************************/
int main(void)
{
	poso = 0, posn = 0;
	number[posn] = 0;
  p = 0;

	LCD_arr[DISPL_RNG] = '\0';

	initialize_hardware();
	keyboard_init();

	while (1)
	{
		keyboard_idle();                   // obsluha klavesnice
		terminal_idle();                   // obsluha terminalu
	}
}


