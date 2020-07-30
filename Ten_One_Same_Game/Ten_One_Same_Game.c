/*
 * Ten_One_Same_Game.c
 *
 * Created: 2020-07-30 오전 9:50:49
 *  Author: user
 */ 


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define BV(bit) (1 << bit)

void COMMAND(unsigned char byte);	// COMMAND 함수 선언
void DATA(unsigned char byte);		// DATA 함수 선언
void LCD_INIT(void);			// LCD_INIT 함수 선언
void DATASTR(const char* str);
void MOVE(int y, int x);

int cnt = 0;
int i = 0;
int start = 0;
const unsigned char SegAnode[10] = {
	0x40, // 0 01000000
	0xF9, // 1
	0x24, // 2
	0x30, // 3
	0x19, // 4
	0x12, // 5
	0x02, // 6
	0x58, // 7
	0x00, // 8
	0x10, // 9
};

int cnt_f = 0;
int pnt = 0;

void SegLoop(){
	if (cnt<10)
	{
		PORTG = BV(0);
		PORTD = SegAnode[i/10];
		_delay_ms(3);
		PORTD = SegAnode[i%10];
		PORTG = BV(1);
		_delay_ms(3);
	}
	else{
		PORTG = BV(0);
		PORTD = SegAnode[i/10];
		_delay_ms(1.5);
		PORTD = SegAnode[i%10];
		PORTG = BV(1);
		_delay_ms(1.5);
	}
	
}
void IsSuccess(){
	for(int k = 0; k<50; k++){
		if(i/10 == i%10 && ~PINB & BV(0)){
			//LED G on
			PORTC = BV(1);
			_delay_ms(10);
			PORTC = 0x00;
			cnt_f = 1;
			return;
		}
		else if((i/10 != i%10 && ~PINB & BV(0)) && start == 1){
			//LED L on
			COMMAND(0b00000001);
			DATASTR("   GAME  OVER");
			PORTC = BV(0);
			_delay_ms(5000);
			PORTC = 0x00;
			COMMAND(0b00000001);
			main();
			break;
		}
	}
	
}
void Start(){
	COMMAND(0b00000001);
	DATASTR("START!");
	_delay_ms(500);
	COMMAND(0b00000001);
	DATASTR("Your Score : ");
	DATA('0');
	MOVE(2,1);
	DATASTR("High Score : ");
	DATA('0' + pnt);
}
int main(){
	
	DDRD = 0xFF;
	DDRE = 0xFF;
	DDRG = 0xFF;
	DDRC = 0xFF;
	DDRB = 0x00;
	
	LCD_INIT();
	
	cnt = 0;
	PORTD = 0xff;
	i = 0;
	
	DATASTR("Your Score : ");
	DATA('0' + cnt);
	MOVE(2,1);
	DATASTR("High Score : ");
	if(pnt>=10) {
		DATA('0'+ pnt/10);
		DATA('0' + pnt%10);
	}
	else DATA('0' + pnt);
	while(1){
		if(~PINB & BV(0)){
			Start();
			start = 1;
			_delay_ms(500);
			for (; i<101; i++){
				if (i==100) i = 0;
				
				for(int j = 0; j<50; j++){
					SegLoop();
					IsSuccess();
					
				}
				if (cnt_f) {
					cnt++;
					MOVE(1,14);
					if(cnt>=10) {
						DATA('0'+ cnt/10);
						DATA('0' + cnt%10);
					}
					else DATA('0' + cnt);
					if(pnt<cnt) pnt = cnt;
					MOVE(2,14);
					if(cnt>=10) {
						DATA('0'+ pnt/10);
						DATA('0' + pnt%10);
					}
					else DATA('0' + pnt);
					cnt_f = 0;
					
				}
			}
			
			start = 1;
		}
		continue;
	}
}

void COMMAND(unsigned char byte)
{
	_delay_ms(2);

	PORTE=byte&0xf0;// 상위4비트 명령어 쓰기
	PORTE&=0b11111100;  // RS = 0, RW = 0, 명령어를 쓰도록 설정
	_delay_us(1);
	PORTE|=0b00000100; // E = 1, lcd 동작
	_delay_us(1);
	PORTE&=0b11111011;// E = 0

	PORTE=(byte<<4)&0xf0;// 하위4비트 명령어 쓰기
	PORTE&=0b11111100;// RS = 0, RW = 0, 명령어를 쓰도록 설정
	_delay_us(1);
	PORTE|=0b00000100; // E = 1, lcd 동작
	_delay_us(1);
	PORTE&=0b11111011;// E = 0
}
void DATA(unsigned char byte)
{
	_delay_ms(2);

	PORTE=byte&0xf0;// 상위4비트 명령어 쓰기
	PORTE |= 0b00000001;// RS = 1, 데이터 설정
	PORTE &= 0b11111101;// RW = 0, 쓰기 설정
	_delay_us(1);
	PORTE|=0b00000100; // E = 1, lcd 동작
	_delay_us(1);
	PORTE&=0b11111011;// E = 0

	PORTE=(byte<<4)&0xf0;// 하위4비트 데이터 쓰기
	PORTE |= 0b00000001;// RS = 1, 데이터 설정
	PORTE &= 0b11111101;// RW = 0, 쓰기 설정
	_delay_us(1);
	PORTE|=0b00000100; // E = 1, lcd 동작
	_delay_us(1);
	PORTE&=0b11111011;// E = 0
}
void LCD_INIT(void)
{
	_delay_ms(30);	// 전원 투입 후 30ms 이상 지연
	
	//Function set
	COMMAND(0b00101000);
	// 인터페이스(DL)=0(4bit), 라인(N)=1(2라인), 폰트(F)=0(5*8 dot)
	_delay_us(39);	// 39us 이상 지연

	//Display ON/OFF Control
	COMMAND(0b00001100);
	// 화면 표시(D)=1(on), 커서(C)=0(off), 블링크(B)=0(off)
	_delay_us(39);	// 39us 이상 지연

	//Clear Display
	COMMAND(0b00000001);
	// 화면을 클리어하고 , 커서가 홈위치인 0번지로 돌아감.
	_delay_ms(1.53);	// 1.53ms 이상 지연

	//Entry Mode Set
	COMMAND(0b00000110);
	// 커서방향(I/D)=1(address증가), 표시이동(S)=0(이동하지 않음)
}

void DATASTR(const char* str){
	
	for(int i =0; str[i]; i++){
		DATA(str[i]);
	}
}

void MOVE(int y, int x)     //커서 이동
{
	unsigned char data;
	if(y==1) 	data=0x80+x-1;           //1행
	else     	data=0xc0+x-1;            //2행
	COMMAND(data);
}
