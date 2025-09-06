#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<stdbool.h>
const uint8_t segments[] = {0x3F, 0x06, 0x5B, 0x4F,
0x66, 0x6D,0x7D, 0x07, 0x7F, 0x6F, 0b0100000, 0};
// "0:0, 1:1, 2:2 ... 10 : -, 11: "\0""
int32_t angle = 0;
uint8_t digitKeys[] = {11, 11, 11, 11, 11};
void showNumber();
void digit();
void allForNull();
void clean();


int main(void){ 
    // Настройка кнопки на срез
    EIMSK = (1<<INT2)|(1<<INT1);
    EICRA = (1<<ISC21)|(1<<ISC11);
    // настройка портов выхода для семисегментника
    DDRA = (1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5); // это про номер индикаторов
    DDRC = 0xFF; // это про  светодиоды
    allForNull();
    while (1)
    {
        showNumber(); // Показываем номер
        _delay_ms(10);
        sei();
    }
}

void showNumber(){
    digit();
    bool isFirstDigit = false;
    for(uint8_t i = 1; i <= 4; i++){
        PORTA = (1 << (i+1));
        if(!isFirstDigit){ //если еще не нашли первый элемент
            if(digitKeys[i]){ //и если текущий элемент не ноль
                isFirstDigit = true; // говорим что нашли первый элемент
                PORTD = segments[digitKeys[i]]; // выводим его на экран
                _delay_ms(10);
                PORTA = 0; 
                _delay_ms(5);
                PORTA = (1 << (i));  
                PORTD = segments[11];
            }
            else{                               // и если текущий элемент ноль 
                PORTD = segments[11];            // гасим все светодиоды
            }
        }else{                                  // если уже нашли первый элемент
            PORTD = segments[digitKeys[i-1]];    // просто выводим согласно hashMap
            // _delay_ms(10);                  
        }
        _delay_ms(10);
        PORTA = 0;    
    }
}
void digit(){
    int16_t d = angle;
    if(d < 0){
        d = -d;
        digitKeys[0] = 10;
    }
    else{
        digitKeys[0] = 11;
    }
    uint8_t i = 4, a;
    while(i){
        // цикл по разрядам числа
        a = d%10;
        digitKeys[i--] = a;
        // выделяем очередной разряд
        d /= 10;
        // уменьшаем число в 10 раз
        // --i;
    }
}
void allForNull(){
    for(uint8_t i = 4; i >= 1; i--){
        PORTA = (1 << i);
        PORTD = 0;
        _delay_ms(10);
        PORTA = 0;
    }
    PORTA = (1 << 5);
    PORTD = segments[0];
}
void clean(){
     for(uint8_t i = 5; i >= 1; i--){
        PORTA = (1 << i);
        PORTD = 0;
        _delay_ms(10);
        PORTA = 0;
    }
}



ISR (INT2_vect){
    allForNull();
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
