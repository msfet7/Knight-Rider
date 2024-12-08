#include <Arduino.h>
#define PWM 6
int varDuty{}; //zmienna od aktualnego cyklu
int sign{1}; //zmienna znaku
int increment{1};
bool isPassed{false}; //czy było przerwanie
int dutyArr[8] = {0,0,0,0,0,0,0,0}; //przetrzymuje wartości wypełnień
int pins[8] = {0,8,4,12,2,10,6,14}; // wartośći wgrywane na PORTB €


int PWM_Gen(int percent){ //generacja sygnału PWM o damym procencie
  int duty = (255*percent)/100;
  return duty;
}
void setup() {
  pinMode(PWM,OUTPUT); //wyjście wspólnego sygnału
  pinMode(11,OUTPUT); //A
  pinMode(10,OUTPUT); //B
  pinMode(9,OUTPUT); //C
  pinMode(8,OUTPUT); //D
  //wyjścia do kontrolowania demuxa - PORTB
  //deklaracja pierwszego licznika - licznik do opóźnień
  TCCR1A &= B11111100; 
  TCCR1B &= B11100000;
  TCCR1B |= (1<<CS11)|(1<<CS10); //prescaler 256
  TCNT1 = 45000; //ustawienie tej wartości daje częstotliwość ok. 1Hz - przerwanie co sekundę
  TIMSK1 |= (1<<TOIE1);
  //deklaracja drugiego licznika - sygnał PWM
  TCCR0A &= B00111100; 
  TCCR0A |= (1<<WGM01) | (1<<WGM00);
  TCCR0A |= (1<<COM0A1) | (1<<COM0A0);
  TCCR0B &= B11100000;
  TCCR0B |= (1<<CS00);
}
ISR(TIMER1_OVF_vect){ //Przerwanie co sekunde
  TCNT1 = 45000; //ustawia licznik na odpowiednią częstotliwość
  varDuty += increment * sign; //inkrementuje var duty o increment pomnożony razy jeden lub -jeden
  //ruch światłą lewo i prawo
  if(varDuty > 7) {
    varDuty = 7 - increment;
    sign = -1;
  }
  if(varDuty < 0 ){
    varDuty = increment;
    sign = 1;
  } //warunki do zmiany kierunku
  isPassed = true;
}

void loop() {
dutyArr[varDuty] = 100; //ustawianie pierwszej diody w tablicy
for(int i = 0;i<=7;i++){ // ustawianie pozostałych diód w tablicy
  if(i == varDuty || dutyArr[i]== 0|| isPassed == false) continue;
  switch (dutyArr[i])
  {
  case 100:
    dutyArr[i] = 60;
    break;
  case 60:
    dutyArr[i] = 30;
    break;
  case 30:
    dutyArr[i] = 5;
    break;
  case 5:
    dutyArr[i] = 0;
    break;
  }
}
isPassed = false;
for(int i = 0; i<=7; i++){ //wystawianie wartości z tablicy na PORTB
  OCR0A=PWM_Gen(i[dutyArr]);
  PORTB = i[pins];
  delay(20);
}
}
