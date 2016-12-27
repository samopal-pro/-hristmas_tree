// use the cRGB struct hsv method
#define USE_HSV

#include "WS2812.h"

//#define MIC_ENABLE 1

cRGB value;
#define LEDCount   8
#define outputPin  2
#define MAX_EFFECT 4
WS2812 LED(LEDCount); 

// Число пикселей в луче
uint8_t num_pixel  = LEDCount;
// Корстанты для эффектов
uint8_t TM = 40, DX = 2;
//Переменные для работы с микрофоном
// Средний уровень сигнала за цикл измерений
uint32_t  mic_avg  = 0;
// Счетчик измерений уровня (количество в цикле)
uint16_t  mic_cnt  = 0;
// Предыдущее значение уровня
uint16_t  mic_prev = 0;
// Изменение уровня сигнала (Разница между текущим и предыдущим)
int  mic_diff = 0;
// Максимальная разница за цикл измерений
int  max_diff = 0;
// Количество срабатывания в цикле
uint16_t mic_trig  = 0;
// Порог срабатывания
uint16_t mic_threshold = 450;
// Время последнего срабатывания
uint32_t mic_ms    = 0;

// Переменные для эффкта радуги
int step1 = 16;
int step2 = 2;
uint8_t color = 0;
// Номер эффекта
uint8_t Effect = 1;
// Переменные управления эффектами
uint32_t tm = 100;
uint8_t n_loop = 0;
uint8_t count_loop = 0;
uint8_t max_count  = 16;
// Флаг автоматической смены эффекта
bool flag_auto = true;
// Флаг мигиния при смене режима
bool flag_blink = false;

uint32_t ms, ms0=0, ms1=0, ms2=0; 

  
void setup() {
  Serial.begin(115200);
  Serial.println("Debug start...");
  LED.setOutput(outputPin);
//  analogReference(INTERNAL);
  
}



void loop() {
   ms = millis();
#ifdef MIC_ENABLE
   if( ms0 == 0 || ms0 > ms  || ms - ms0 >1 ){
       ms0 = ms;
       ReadMic();
   }

   
   
     if( ms1 == 0 || ms1 > ms || (ms - ms1)>200 ){
       ms1 = ms;
       if( mic_cnt > 0 )mic_avg /= mic_cnt;
/*       
       if( max_diff > 300 ){
       Serial.print("Mic: avg=");
       Serial.print(mic_avg);
       Serial.print(",cnt=");
       Serial.print(mic_cnt);
       Serial.print(",max=");
       Serial.print(max_diff);
       Serial.print(",trig=");
       Serial.println(mic_trig);
       }
*/      
      if( max_diff > mic_threshold ){
         if( flag_auto == false && mic_ms>1000 ){
            Effect++;
            if( Effect > MAX_EFFECT )Effect = 1;
         }
         Serial.print("Level ");
         Serial.print(max_diff);
         Serial.print(" ms ");
         Serial.println(ms-mic_ms);
         if( (ms-mic_ms) > 200 & (ms-mic_ms) < 500 ){
             if( flag_auto == true )flag_auto = false;
             else flag_auto = true;
             flag_blink = true;
             n_loop = 0;
             Serial.print("Auto is ");
             Serial.println((int)flag_auto);
         }
//         else {
            mic_ms = ms;
 //        }
      }
// Обнуляем статистику
       mic_avg  = 0;
       mic_cnt  = 0;
       max_diff = 0;
       mic_trig = 0;
   }
   if((  ms2 == 0 || ms2 > ms || (ms - ms2)>150 )&&flag_blink == true){
       ms2 = ms;
       value.r = 0;
       value.g = 0;
       value.b = 0;
       if( n_loop == 1 || n_loop == 3 ){
           if( flag_auto )value.g = 128;
           else value.r = 128;
       }
       for( int i=0; i<num_pixel; i++)LED.set_crgb_at(i, value);
       LED.sync();
       n_loop++;
       if( n_loop > 7 ){
           n_loop = 0;
           flag_blink = false;
       }

   }
#endif


   if((  ms2 == 0 || ms2 > ms || (ms - ms2)>tm )&&flag_blink == false ){
      ms2 = ms;
      
// Цикл эффекта закончился 
      if( n_loop == 0 ){
         count_loop++;
         if( count_loop >= max_count ){
             count_loop = 0;
             if( flag_auto == true ){
                Effect++;
                if( Effect > MAX_EFFECT )Effect = 1;
             }
         }
      }
// Выбор эффекта
      switch( Effect ){
         case 1 : //Эффект ракеты
            LoopMeteor(count_loop%6,0);
            max_count = 16;
            tm = 40;
            break;
         case 2 : //Эффект метеора
            LoopMeteor(count_loop%6,1);
            max_count = 16;
            tm = 40;
            break;
         case 3 : //Эффект радуги
            for( int i=0; i< num_pixel; i++ ){
               Wheel(color + i*step1  );
               LED.set_crgb_at(num_pixel-i-1, value); 
               }
             LED.sync();
             color += step2;
             n_loop++;
             if( n_loop >=64 )n_loop = 0;
             max_count = 16;
             tm = 10;
             break;    
         case 4 : // Эффект зигзаг           
             LoopMeteor2(count_loop%6,count_loop%2);
             max_count = 12;
             tm = 60;
             break;
      }
    }

      
}

/**
 * Эффект пламени
 */
/*
   int r = 255;
   int g = r-150;
   int b = 20;

//for(int x = 0; x <num_pixel; x++)
//{
int flicker = random(0,150);
int r1 = r-flicker;
int g1 = g-flicker;
int b1 = b-flicker;

int x = random(0-num_pixel-1);
if(g1<0) g1=0;
if(r1<0) r1=0;
if(b1<0) b1=0;
//value.r = random(0,255);
//value.g = random(0,255);
//value.b = random(0,255);
//value.r = r1;
//value.g = g1;
//value.b = b1;
Wheel(random(0-255));

LED.set_crgb_at(x, value);
//}
LED.sync();
delay(random(20,100));
*/

/*
 * Эффект метеора (или ракеты, зависит от направления)
 * @param pair - цвет 0-5
 * @param dir - направление эффекта (0 и 1)
 * 
 */
void LoopMeteor( uint8_t pair, uint8_t dir){
   if( n_loop < num_pixel ){
      int i1 = n_loop;
    
      for( int i2=0; i2<num_pixel; i2++ ){
          SetStarColor1(num_pixel-i2-1, 0, pair, dir);
      }

      SetStarColor1(num_pixel-i1,15, pair, dir);
      for( int i2=0; i2<i1; i2++ ){
          SetStarColor1(num_pixel-i2-1, 16-(i1-i2)*DX, pair, dir);
      }
     
      LED.sync();
   }
   else {
      int i1 = n_loop - num_pixel;
      for( int i2=0; i2<num_pixel; i2++ ){
         int c = 16-(i1-(i2-8))*2;
         if( c < 0 )c = 0;
         SetStarColor1(num_pixel-i2-1,c, pair, dir);
      }
      LED.sync();
   }
   n_loop++;
   if( n_loop >= num_pixel * 2 )n_loop = 0;
}

/*
 * Эффект Кортокого метеора без затухающего хвоста
 * @param pair - цвет 0-5
 * @param dir - направление эффекта (0 и 1)
 * 
 */
void LoopMeteor2( uint8_t pair, uint8_t dir){
   int i1 = n_loop;
    
   for( int i2=0; i2<num_pixel; i2++ ){
       SetStarColor1(num_pixel-i2-1, 0, pair, dir);
   }

   SetStarColor1(num_pixel-i1,15, pair, dir);
   for( int i2=0; i2<i1; i2++ ){
       SetStarColor1(num_pixel-i2-1, 16-(i1-i2)*DX, pair, dir);
   }
     
   LED.sync();
   n_loop++;
   if( n_loop >= num_pixel )n_loop = 0;
}


/**
 * Разгорание одного пиксела на каждом луче 
 * @param n - Номер пиксела в луче 
 * @param br - Ярклсть
 * @param pair - цвет 0-5
 * @param dir - направление эффекта (0 и 1)
 * 
 */
void SetStarColor1( uint8_t n, uint8_t br, uint8_t pair, uint8_t dir ){
   if( n > num_pixel )return;
   if( br > 15 )br=15;
   uint8_t r=0, g=0, b=0;
   if( br < 8 ){
      switch( pair ){
        case 0 :
        case 3 :
           r = br*32;
           break;
        case 1 :
        case 4 :
           g = br*32;
           break;
        case 2 :
        case 5 :
           b = br*32;
           break;
      }
   }
   else {
      switch( pair ){
         case 0 :
            r = 255;
            g = (br-8)*32;
            break;
         case 1 :
            g = 255;
            b = (br-8)*32;
            break;
         case 2 :
            b = 255;
            r = (br-8)*32;
            break;
         case 3 :
            r = 255;
            b = (br-8)*32;
            break;
         case 4 :
            g = 255;
            r = (br-8)*32;
            break;
         case 5 :
            b = 255;
            g = (br-8)*32;
            break;
      }
   }
   value.r = r;
   value.b = b;
   value.g = g;
   if( dir == 1 )LED.set_crgb_at(n, value);
   else LED.set_crgb_at(num_pixel-n-1, value);
 
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
void Wheel(byte WheelPos) {
  if(WheelPos < 85) {
     value.r = WheelPos * 3;
     value.g = 255 - WheelPos * 3;
     value.b = 0;
  } 
  else if(WheelPos < 170) {
     WheelPos -= 85;
     value.r = 255 - WheelPos * 3;
     value.g = 0;
     value.b = WheelPos * 3;
  } 
  else {
     WheelPos -= 170;
     value.r = 0;
     value.g = WheelPos * 3;
     value.b = 255 - WheelPos * 3;
  }
}

/**
 * Работа с микрофоном
 */
void ReadMic(){
// Устанавливаем режим компаратора A2 и A3 с усилением 20х  
  ADMUX = 0B10000111;
  ADCSRA |= (1 << ADSC);
//Ждем окончания измерения  
  while(ADCSRA & (1<<ADSC)); 
//  uint16_t adc = (ADCH << 8) | ADCL; 

  uint8_t low  = ADCL;
  uint8_t high = ADCH;
  uint16_t adc = (high << 8) | low; 

// Собираем статистику
// Среднее значение
  mic_avg += adc;
  mic_cnt++;
// Разница с предыдущим значением  
  mic_diff = adc - mic_prev;
// Максимальная разница  
  if( max_diff < mic_diff )max_diff = mic_diff; 
// Количество превышений порога срабатывания
  if( mic_diff > mic_threshold )mic_trig++;
// Запоминаем значение микрофона  
  mic_prev = adc;
}  

