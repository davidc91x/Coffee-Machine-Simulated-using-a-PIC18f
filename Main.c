/* Main.c 
 * Processor: PIC18F45K22
 * Compiler:  MPLAB XC8
 */

#include <xc.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "config.h"
#include "GLCD.h"
#include "ascii.h"
#include "splash.h"
#define _XTAL_FREQ 8000000  
int overflows=0;
void configPIC() {
   ANSELA=0x00;
   ANSELB=0x00;        
   ANSELC=0x00;   
   ANSELD=0x00;                  
   ANSELE=ANSELE & 0xE;  
   TRISE=TRISE|0x3;
   ANSELE=ANSELE | 0x2;
   
   TRISA=TRISA|0x03;
   TRISD=0x00;
   TRISB=0x00;
   TRISC=0xFF;
   PORTD=0x00;
   PORTB=0x00;  
   TXSTA1=0x24;
   SPBRG1 = 16;
   BAUDCON=BAUDCON|0x08,
   TRISCbits.RC7 = 1;
   TRISCbits.RC6 = 1;
   PIE1bits.TXIE = 0;
   RCSTA1bits.SPEN = 1;
   RCSTA1 = 0x90;
   IPEN=1;
   GIEH=1;
   ADFM=1;
   ACQT2=0;
   ACQT1=1;
   ACQT0=0;
   ADCS2=0;
   ADCS1=0;
   ADCS0=1;
   NVCFG1=0;
   NVCFG0=0;
   PVCFG1=0;
   PVCFG0=0;
   CHS4=0;
   CHS3=0;
   CHS2=1;
   CHS1=1;
   CHS0=0;
   ADON=1;
   
   C3TSEL1=0;
   C3TSEL0=0;
   PR2=249;
   CCP3M3=1;
   CCP3M2=1;
   CCPR3L=0x00;
   DC3B1=0;
   DC3B0=0;   

   TMR2IF=0;
   T2CKPS1=1;
   T2CKPS0=0;
   
   TMR2IE=1;
   TMR2IP=0;
   IPEN=1;
   GIEH=1;
   PEIE=1;
   TMR2ON=1;
   
   T0CON=0x46;
   TMR0ON=0;
   TMR0IE=1;
   TMR0IF=0;
   
}

void writeTxt(byte page, byte y, char * s) {
   int i=0;
   while (*s!='\n' && *s!='\0') 
   {
      putchGLCD(page, y+i, *(s++));
      i++;
   };
}	
void interrupt low_priority tmr(){
   if (TMR2IE==1 && TMR2IF==1) {
      TRISE=TRISE & 0xE;
   }
   TMR2IF=0;
}

void interrupt tmr0(){
   if ( TMR0IE==1 && TMR0IF==1) {
      ++overflows;
   }
   TMR0IF=0;
}

void putc_usart1 (char xc){
   while (!TXSTA1bits.TRMT);
   TXREG1 = xc;
}

void puts_usart1 (unsigned char *cptr){
   while(*cptr) {
      putc_usart1 (*cptr++);
   }
}

const char * s0 ="Diposit buit\n";
const char * s1 ="Queda poca aigua\n";
const char * s2 ="Seleccionant tipus \n";
const char * s3 ="Seleccionant dosi\n";
const char * s4 ="Seleccionant temperatura\n";
const char * s8 ="Tipus= \n";
const char * s9 ="Dosi= \n";
const char * s10 ="Temperatura= \n";
const char * s5 ="L7 Projecte\n";
const char * s6 = "David Castro Paniello\n";
const char * s7 = "Nil Rof Bonet\n";
const char * s11 = "Molinet activat\n";
const char * s12 = "Bomba d'aigua activada\n";
const char * s13 = "Ventilador ences\n";
const char * s14 = "                      \n";
const char * s20 = " \n";
const char * s15 = "Cafe fet \n";
const char * missatge = "Disfruti del cafe\n";
const char * ul = ":\n";
const char * um = ";\n";
const char * ur = "<\n";
const char * bl = "]\n";
const char * bm = "[\n";
const char * br = "@\n";

int estado=1;

int tipus=10;
int dosi=15;
int temperatura=60;
int sel=1;
int terror,agua;
double f1;

void enviardades(void){
   unsigned char *d[50];
   sprintf(d,"L7 Projecte\r\nDavid Castro Paniello\r\nNil Rof Bonet\r\n\0");
   puts_usart1(d);
}

void writebit(void){
   byte columna=0;
   byte pagina=0;
   for(int i=0;i<1024;++i) {
   
      writeByte(pagina, columna, bitmap[i]);
      ++columna;
      if(columna==128){
	 ++pagina;
	 columna=0;
      }
   }
}

void alarmab(void) {
   writeTxt(7,0,s1);
}

void alarma(void) {
   clearGLCD(0,7,0,127);
   writeTxt(7,0,s0);
   __delay_ms(2000);
}

unsigned char getc_usart1 (void){
//while (!PIR1bits.RCIF);
   if(!RCSTA1bits.FERR && !RCSTA1bits.OERR) return RCREG1; // RCIF clears automatically
   else if(RCSTA1bits.OERR) return '*';
   else return 'º';
}

void main(void){
   configPIC();
   GLCDinit();           //Inicialitzem la pantalla
   clearGLCD(0,7,0,127);   //Esborrem pantalla
   setStartLine(0);        //Definim linia d'inici
   writebit();
   __delay_ms(2000);
   clearGLCD(0,7,0,127);
   enviardades();
   writeTxt(0,2,s5);
   writeTxt(3,2,s6);
   writeTxt(5,6,s7);
   __delay_ms(2000);
   clearGLCD(0,7,0,127);
   
   int ant4 = PORTCbits.RC4;
   int ant3 = PORTCbits.RC3;
   int ant2 = PORTCbits.RC2;
   int ant1 = PORTCbits.RC1;
   int ant0 = PORTCbits.RC0;
   
   while (1){
      if(estado == 1){
         if(ant4 == 0 && PORTCbits.RC4 == 1){
            __delay_ms(10);
            if(PORTCbits.RC4 == 1){
               ant4 = 1;
               if(sel < 3) ++sel;
               else {
                  ++estado;
               }
            }
            else ant4 = 0;
         }
         else if (ant4 == 1 && PORTCbits.RC4 == 0){
            __delay_ms(10);
            if(PORTCbits.RC4 == 0) ant4 = 0;
            else ant4 = 1;
         }
         
         unsigned char d = getc_usart1();
         if(d == 's'){
            RCREG1 = ' ';
            if(sel < 3) ++sel;
            else {
               ++estado;
            }
         }

         if(sel == 1 && estado == 1) {
            writeTxt(0,0,s2);
            if(ant2 == 0 && PORTCbits.RC2 == 1){
               __delay_ms(10);
               if(PORTCbits.RC2 == 1){
                  if(tipus > 5) --tipus;
                  ant2 = 1;
               }
               else ant2 = 0;
            }
            else if (ant2 == 1 && PORTCbits.RC2 == 0){
               __delay_ms(10);
               if(PORTCbits.RC2 == 0) ant2 = 0;
               else ant2 = 1;
            }

            if(ant3 == 0 && PORTCbits.RC3 == 1){
               __delay_ms(10);
               if(PORTCbits.RC3 == 1){
                  if(tipus < 15) ++tipus;
                  ant3 = 1;
               }
               else ant3 = 0;
            }
            else if (ant3 == 1 && PORTCbits.RC3 == 0){
               __delay_ms(10);
               if(PORTCbits.RC2 == 0) ant3 = 0;
               else ant3 = 1;
            }

            unsigned char d = getc_usart1();
            if(d == 'a' && tipus > 5){
               --tipus;
               RCREG1 = ' ';
            }
            if(d == 'd' && tipus < 15){
               ++tipus;
               RCREG1 = ' ';
            }
         }
         else if(sel == 2 && estado == 1) {
            writeTxt(0,0,s3);
            if(ant0 == 0 && PORTCbits.RC0 == 1){
               __delay_ms(10);
               if(PORTCbits.RC0 == 1){
                  if(dosi < 20) ++dosi;
                  ant0 = 1;
               }
               else ant0 = 0;
            }
            else if (ant0 == 1 && PORTCbits.RC0 == 0){
               __delay_ms(10);
               if(PORTCbits.RC0 == 0) ant0 = 0;
               else ant0 = 1;
            }

            if(ant1 == 0 && PORTCbits.RC1 == 1){
               __delay_ms(10);
               if(PORTCbits.RC1 == 1){
                  if(dosi > 10) --dosi;
                  ant1 = 1;
               }
               else ant1 = 0;
            }
            else if (ant1 == 1 && PORTCbits.RC1 == 0){
               __delay_ms(10);
               if(PORTCbits.RC1 == 0) ant1 = 0;
               else ant1 = 1;
            }

            unsigned char d = getc_usart1();
            if(d == 'x' && dosi > 10){
               --dosi;
               RCREG1 = ' ';
            }
            if(d == 'w' && dosi < 20){
               ++dosi;
               RCREG1 = ' ';
            }
         }
         else if(estado == 1 && sel == 3){
            writeTxt(0,0,s4);
            if(ant0 == 0 && PORTCbits.RC0 == 1){
               __delay_ms(10);
               if(PORTCbits.RC0 == 1){
                  if(temperatura < 70) ++temperatura;
                  ant0 = 1;
               }
               else ant0 = 0;
            }
            else if (ant0 == 1 && PORTCbits.RC0 == 0){
               __delay_ms(10);
               if(PORTCbits.RC0 == 0) ant0 = 0;
               else ant0 = 1;
            }

            if(ant1 == 0 && PORTCbits.RC1 == 1){
               __delay_ms(10);
               if(PORTCbits.RC1 == 1){
                  if(temperatura > 50) --temperatura;
                  ant1 = 1;
               }
               else ant1 = 0;
            }
            else if (ant1 == 1 && PORTCbits.RC1 == 0){
               __delay_ms(10);
               if(PORTCbits.RC1 == 0) ant1 = 0;
               else ant1 = 1;
            }

            unsigned char d = getc_usart1();
            if(d == 'x' && temperatura > 50){
               --temperatura;
               RCREG1 = ' ';
            }
            if(d == 'w' && temperatura < 70){
               ++temperatura;
               RCREG1 = ' ';
            }
         }

         writeTxt(1,0,s8);
         writeNum(1,7,tipus);
         if(tipus < 10) writeTxt(1,8,s20);
         writeTxt(2,0,s9);
         writeNum(2,6,dosi);
         writeTxt(3,0,s10);
         writeNum(3,13,temperatura);
      }

      if(estado == 2){
         clearGLCD(0,7,0,127);
         LATA = LATA | 0x03;
         TMR0 = 0;
         overflows = 0;
         TMR0ON = 1;
         writeTxt(0,0,s11);
         writeTxt(1,0,s12);

         CHS4 = 0;
         CHS3 = 0;
         CHS2 = 1;
         CHS1 = 1;
         CHS0 = 0;
         GO = 1;
         while(GO == 1);
         
         int vout = 0;
         if(ADRESL & 0x01) vout = vout + 1;
         if(ADRESL & 0x02) vout = vout + 2;
         if(ADRESL & 0x04) vout = vout + 4;
         if(ADRESL & 0x08) vout = vout + 8;
         if(ADRESL & 0x10) vout = vout + 16;
         if(ADRESL & 0x20) vout = vout + 32;
         if(ADRESL & 0x40) vout = vout + 64;
         if(ADRESL & 0x80) vout = vout + 128;
         if(ADRESH & 0x01) vout = vout + 256;
         if(ADRESH & 0x02) vout = vout + 512;

         double vout2 = (vout * 5.0) / 1024.0;
         int temesur = 4050.0 / log(-(4700.0 - (4700.0 * 5.0 / vout2)) / 0.0059257) - 273.2;
         terror = temperatura - temesur;
         ++estado;
      }

      if(estado == 3){
         if(terror <= -5){
            CCPR3L = 0xFA;
            DC3B1 = 0;
            DC3B0 = 0;
         }
         else if(terror == -4){
            CCPR3L = 0xE5;
            DC3B1 = 0;
            DC3B0 = 0;
         }
         else if(terror == -3){
            CCPR3L = 0xC8;
            DC3B1 = 0;
            DC3B0 = 0;
         }
         else if(terror == -2){
            CCPR3L = 0xAF;
            DC3B1 = 0;
            DC3B0 = 0;
         }
         else if(terror == -1){
            CCPR3L = 0x96;
            DC3B1 = 0;
            DC3B0 = 0;
         }
         else if(terror == 0){
            CCPR3L = 0x7D;
            DC3B1 = 0;
            DC3B0 = 0;
         }
         else if(terror == 1){
            CCPR3L = 0x64;
            DC3B1 = 0;
            DC3B0 = 0;
         }
         else if(terror == 2){
            CCPR3L = 0x4B;
            DC3B1 = 0;
            DC3B0 = 0;
         }
         else if(terror == 3){
            CCPR3L = 0x32;
            DC3B1 = 0;
            DC3B0 = 0;
         }
         else if(terror == 4){
            CCPR3L = 0x19;
            DC3B1 = 0;
            DC3B0 = 0;
         }
         else if(terror >= 5){
            CCPR3L = 0x00;
            DC3B1 = 0;
            DC3B0 = 0;
         }

         ++estado;
         writeTxt(2,0,s13);
         __delay_ms(1000);
      }

      if(estado == 4){
         CHS4 = 0;
         CHS3 = 0;
         CHS2 = 0;
         CHS1 = 1;
         CHS0 = 1;
         GO = 1; 
         while(GO == 1);

         int vout = 0;
         if(ADRESL & 0x01) vout = vout + 1;
         if(ADRESL & 0x02) vout = vout + 2;
         if(ADRESL & 0x04) vout = vout + 4;
         if(ADRESL & 0x08) vout = vout + 8;
         if(ADRESL & 0x10) vout = vout + 16;
         if(ADRESL & 0x20) vout = vout + 32;
         if(ADRESL & 0x40) vout = vout + 64;
         if(ADRESL & 0x80) vout = vout + 128;
         if(ADRESH & 0x01) vout = vout + 256;
         if(ADRESH & 0x02) vout = vout + 512;

         f1 = (vout / 1024.0 * 5.0);
         agua = f1;
         if(f1 <= 1 && f1 != 0) alarmab();
         if(f1 == 0) alarma();
         ++estado;
      }

      if(estado == 5) {
         estado = 1;
         tipus = 10;
         dosi = 15;
         temperatura = 60;
         sel = 1;

         if(f1 != 0){
            int t = TMR0;
            float temps2 = (overflows * 0.016384 + t * 128 * (4.0 / (8 * 1000000.0))) * 1000;
            int temps = temps2;

            while(temps < (tipus * 1000)){
               int t = TMR0;
               temps2 = (overflows * 0.016384 + t * 128 * (4.0 / (8 * 1000000.0))) * 1000;
               temps = temps2;
            }

            writeTxt(0,0,s14);
            LATA = LATA & 0xFE;
            
            while(temps < (dosi * 1000)) {
               int t = TMR0;
               temps2 = (overflows * 0.016384 + t * 128 * (4.0 / (8 * 1000000.0))) * 1000;
               temps = temps2;
            }

            LATA = LATA & 0xFD;
            CCPR3L = 0x00;
            DC3B1 = 0;
            DC3B0 = 0;
            TMR0ON = 0;
            clearGLCD(0,7,0,127);
            writeTxt(2, 62, ul);
            writeTxt(2, 63, um);
            writeTxt(2, 64, ur);
            writeTxt(3, 62, bl);
            writeTxt(3, 63, bm);
            writeTxt(3, 64, br);
            writeTxt(6, 5, missatge);
            __delay_ms(2000);
         }

         CCPR3L = 0x00;
         LATA = LATA & 0xFE;
         LATA = LATA & 0xFD;
         DC3B1 = 0;
         DC3B0 = 0;
         clearGLCD(0,7,0,127);
      }
   }
}
