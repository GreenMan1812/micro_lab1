#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>
const uint8_t key2digit[] = {0x3F, 0x06, 0x5B, 0x4F,
                             0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0b01000000, 0};
uint8_t angleInArr[] = {11, 11, 11, 11, 0};
int16_t angle = 0;
void SS_IND_NEW(uint8_t IND_N, uint8_t DIGIT)
{    
    PORTC = key2digit[DIGIT];

    PORTA = (1 << IND_N);
    _delay_us(25);
    PORTA = 0;
}

void resetForNull();
void num2digitArr();
void showNumber();

int main(void)
{
    EIMSK = (1 << INT2) | (1 << INT1);
    EICRA = (1 << ISC11) | (1 << ISC11) | (1 << ISC21) | (1 << ISC20);
    // настройка портов выхода для семисегментника
    DDRA = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5); // это про номер индикаторов
    DDRC = 0xFF;                                                 // это про  светодиоды
    DDRD = 0;
    resetForNull();
    uint8_t cnt = 0;
    //sei();
    while (1)
    {
        // cnt+=1;
        // if(!STOP_FLAG){
        // cli();
        showNumber();
        // STOP_FLAG = true;
        // sei();
        // }
    }
}

void showNumber()
{
    num2digitArr();
    bool isFirstDigit = false;
    // uint8_t ind_n = 0;
    if(angle == 0){
        resetForNull();
    }
    else{
        for (uint8_t i = 1; i <= 4; i++)
        {
            SS_IND_NEW(i + 1, angleInArr[i]);
            uint8_t ind_n = i + 1;
            ind_n = i + 1;
            if (!isFirstDigit)
            { // если еще не нашли первый элемент
                if (angleInArr[i])
                {                        // и если текущий элемент не ноль
                    isFirstDigit = true; // говорим что нашли первый элемент
                    SS_IND_NEW(ind_n, angleInArr[i]);
                    SS_IND_NEW(ind_n - 1, angleInArr[0]);
                }
                else
                {                          // и если текущий элемент ноль
                    SS_IND_NEW(ind_n, 11); // гасим все светодиоды
                }
            }
            else
            {                                     // если уже нашли первый элемент
                SS_IND_NEW(ind_n, angleInArr[i]); // просто выводим согласно hashMap
            }
        }
    }
    _delay_ms(25);
}
void num2digitArr()
{
    int16_t d = angle;
    if (d < 0)
    {
        d = -d;
        angleInArr[0] = 10;
    }
    else
    {
        angleInArr[0] = 11;
    }
    uint8_t i = 4, a;
    while (i)
    {
        // цикл по разрядам числа
        a = d % 10;
        angleInArr[i] = a;
        // выделяем очередной разряд
        d /= 10;
        // уменьшаем число в 10 раз
        --i;
    }
}

void resetForNull()
{
    for (uint8_t i = 1; i <= 4; i++)
    {
        SS_IND_NEW(i, 11);
    }
    SS_IND_NEW(5, 0);
}

ISR(INT2_vect)
{
    resetForNull();
    angle = 0;
}

ISR(INT1_vect)
{
    if ((PIND & (1 << 1)) != 0)
    {
        EICRA |= (1 << ISC11);
        if ((PIND & (1 << 0)) != 0)
            angle++;
        else
            angle--;
    }
    else
    {
        EICRA |= (1 << ISC11) | (1 << ISC10);
        if ((PIND & (1 << 0)) != 0)
            angle--;
        else
            angle++;
    }
    if(angle > 100 || angle < -100) angle = 100;
    // STOP_FLAG = false;
}
