#include <stdio.h>
#include <string.h>
#include <io.h>   // library baraye estefade az tavabe e micro cotroller
#include <lcd.h> //library e lazem baraye kar krdan ba tavabe e lcd
#include <delay.h>

#asm     //dastorate lazam baraye marefi port haye lcd
    .equ __lcd_port=0x12 //adrese porte micro cotroller ke lcd be oon vasl shode 
#endasm

int game[3][3], win[8], player, count, state, i, j, k;
unsigned char row[4] = {0xFE, 0xFD, 0xFB, 0xF7};  //baraye oon satr 1 va baghie 0 
char text[2];
unsigned char num[2] = {0x06, 0x5B};  // halati ke 7 segment 1 ya 2 ro neshun bde -> 0x06: halate 1     0x5B: halate 2


void keypad() //OK
{
    while(1)
    {
        for(i=0; i<4; i++)
        {   
            j = 4;
            PORTC = row[i];
            DDRC = 0x0F;
            if(PINC.4 == 0)  //PINC -> vorodi haye micro (sotoon ha)
                j = 0;
            if(PINC.5 == 0)
                j = 1;
            if(PINC.6 == 0)
                j = 2;
            if(PINC.7 == 0)
                j = 3;  
            if(!(j==4))
            {
                while(PINC.4==0);   //test baraye inke kilidi age entekhab mishe vel beshe va bazikon oon klid ro negah nadare 
                while(PINC.5==0);
                while(PINC.6==0);
                while(PINC.7==0);
                return;
            }  
            delay_ms(5);            
        }
    }
}

void init() //OK
{
    player = 0;  //0: bazikone aval(X)    1: bazikone dovom(O)
    count = 0;   // tedad khune haye por shode az board
    state = 0;   //0: bazi edame dare     1: X borde     -1: Y borde    2: bazi mosavi shode va hame ye khune ha por shode  
    strcpy(text, "");
    for(i=0; i<3; i++)
        for(j=0; j<3; j++)
            game[i][j]=0;
    for(i=0; i<8; i++)
        win[i] = 0; 
    i = 0;
    j = 0;    
    lcd_clear();
    lcd_puts("Press ON/C to start game:)");    //pm e avalie rooye lcd 
    do  
    {
        keypad();
        k = i*4 + j;
    }while(k != 12); // check kardane inke hatman klide ON/C zade bshe ta bazi shoroo bshe, ON/C klide andise 12 e keypad hastesh
    lcd_clear();
    for(i = 3; i>0; i--) //shomareshe makoos
    {
       sprintf(text, "%d",i); 
       lcd_puts(text);
       delay_ms(100);
       lcd_clear();
    }
    PORTA = num[player];       
}

void showBoard()
{
    lcd_clear();
    for(i=0; i<3; i++)
        for(j=0; j<3; j++)
        {
            lcd_gotoxy(j, i);
            if(game[i][j] == 1)
                lcd_putchar('X');
            else if(game[i][j] == -1)
                lcd_putchar('O');
            else
                lcd_putchar('-');        
        }
}


void winnerCheck()
{
    for(i=0; i<8; i++)
        win[i] = 0;
    for(i=0; i<3; i++)
        for(j=0; j<3; j++)
        {
            win[i] += game[i][j];
            win[i+3] += game[j][i];
            if(i == j)
                win[6] += game[i][i];
            if(i + j == 2)
                win[7] += game[i][j];    
        }    
}

void main(void)
{
     PORTC = 0xFF; //maghadire avalie ye keypad ke be porte C micro val hastan mishe 8 ta 1 ke be hexa decimal mishe FF
     DDRC = 0x0F; //4 meghdare por arzeshe micro vorodi hastan va meghadare 0 mishan va 4 meghdare kam arzesh mishan khorojie micro ke mishan 1111
     PORTB = 0x00;   //halate khamoshe 7segment
     DDRA = 0xFF;    //chon porte A bayad khoroji bashe bayad 8ta 1 befrestm ke mishe FF
     lcd_init(20); // tanzimate avalie baraye lcd ke lcd ra pak mikonad va vorodie tabe tedate sotoon ha mibashad ke inja lcd ke darim 20*4 hastesh
     init();  
     showBoard();
     while(state == 0)
     {
        keypad();
        if(i != 3 && j != 3)
        {
            if(game[i][j] == 0)
            {
                if(player == 0)
                    game[i][j] = 1;
                else
                    game[i][j] = -1;
                player = !player;   //avaz kardane bazikon 
                PORTA = num[player]; 
                count++;        
            }
        }
        k = i*4 + j;
        if(k==13)
            init();
        showBoard();
        winnerCheck();
        for(i=0; i<8; i++)
            if(win[i] == 3)
                state = 1;
            else if(win[i] == -3)
                state = -1;
        if(count == 9 && state == 0)
            state = 2;                
     }
     lcd_gotoxy(9,3);
     if(state == 1)
        lcd_puts("X win!");
     else if(state == -1)
        lcd_puts("O win!");
     else if(state == 2)
        lcd_puts("Draw");
     while(1)
     {
        keypad();
        k = i*4 + j;
        if(k==13)
            init();
     }         
}
