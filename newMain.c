#include <avr/io.h>
#include <avr/interrupt.h>
#include<stdbool.h>
#include<util/delay.h>
const uint8_t key2digit[]  = {0x3F, 0x06, 0x5B, 0x4F,
0x66, 0x6D,0x7D, 0x07, 0x7F, 0x6F, 0b0100000, 0};
uint8_t angleInArr[] = {11, 11, 11, 11, 11};
int16_t angle = 0;
#define SS_IND(IND_N, BITS){
    PORTA = (1 << IND_N);
    _delay_us(5);
    PORTD = BITS;
    _delay_us(5);
    PORTA = 0;
    _delay_us(5);
}
#define SS_IND_NEW(IND_N, DIGIT){
    PORTA = (1 << IND_N);
    _delay_us(5);
    PORTD = key2digit[DIGIT];
    _delay_us(5);
    PORTA = 0;
    _delay_us(5);
}
bool STOP_FLAG = true;
void resetForNull();
void num2digitArr();
void showNumber();

int main(void){
    EIMSK = (1<<INT2)|(1<<INT1);
    EICRA = (1<<ISC21)|(1<<ISC11);
    // настройка портов выхода для семисегментника
    DDRA = (1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5); // это про номер индикаторов
    DDRC = 0xFF; // это про  светодиоды
    sei();
    resetForNull();
    while{
        if(!STOP_FLAG){
            cli();
            showNumber();
            STOP_FLAG = true;
            sei();
        }
    }

} 
void showNumber(){
    num2digitArr();
    bool isFirstDigit = false;
    uint8_t ind_n = 0;
    for(uint8_t i = 1; i <= 4; i++){
        ind_n = i + 1;
        if(!isFirstDigit){ //если еще не нашли первый элемент
            if(ssKeys[i]){ //и если текущий элемент не ноль
                isFirstDigit = true; // говорим что нашли первый элемент
                SS_IND_NEW(ind_n, angleInArr[i]);
                SS_IND_NEW(ind_n - 1, angleInArr[10]);
            }
            else{                               // и если текущий элемент ноль 
                SS_IND_NEW(ind_n, 11);        // гасим все светодиоды
            }
        }else{                                  // если уже нашли первый элемент
            SS_IND_NEW(ind_n, angleInArr[i]);    // просто выводим согласно hashMap
            // _delay_ms(10);                  
        }
        _delay_us(10);
        PORTA = 0;    
    }
}
void num2digitArr(){
    int16_t d = angle;
    if(d < 0){
        d = -d;
        angleInArr[0] = 10;
    }
    else{
        angleInArr[0] = 11;
    }
    uint8_t i = 4, a;
    while(i){
        // цикл по разрядам числа
        a = d%10;
        angleInArr[i--] = a;
        // выделяем очередной разряд
        d /= 10;
        // уменьшаем число в 10 раз
        // --i;
    }
}



void resetForNull(){
    for(uint8_t i = 1; i <= 4; i++){
        SS_IND_NEW(i, 11);
    }
    SS_IND_NEW(i, 0);
}

ISR (INT2_vect){
    resetForNull();
    angle = 0;
}

ISR (INT1_vect){
    if((PIND & (1 << 0)) != 0){
        EICRA = (1<<ISC11);
        if((PIND & (1 << 1)) != 0) 
            angle++;
        else 
            angle--;
    }
    else{
        EICRA = (1 << ISC11) | (1<<ISC10);
            if((PIND & (1 << 1)) != 0) 
                angle--;
            else 
                angle++;
    }
}