/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include <cstdio>
#include "platform/mbed_thread.h"
#include "DHT.h"



DHT sensor(D8,DHT11);
DigitalOut led(LED1);
volatile bool releChange=false;
DigitalOut rele(D9);
//DigitalOut rele();
Thread threadAttention;
Thread threadReport;

volatile float value=0;
const int second=1000000;
volatile int nowTemperature=0;
float nowHumidity=0;
volatile int secCritics=0;
// int *tempMass;
// int *humidityMass;

int getTemperature(){
    
    return sensor.ReadTemperature(CELCIUS);
}
int getHumidity(){

    return sensor.ReadHumidity();
}


void write(int temperature, int humidity){//короткая запись данных (20_25)
    printf("%d_%d\n",temperature,humidity);
    fflush(stdout); //для вывода в потоке
}

void attention(){//предупрждение, если температрура выше нормы более минуты
    int tmax=25;
    int i=0;
    while(nowTemperature>tmax){
        releChange=false;
        wait_us(second);
        i++;
        secCritics++;
        if(i>5){
            printf("warning %d\n",nowTemperature); //реле переключить функция
            fflush(stdout); //для вывода в потоке
            rele=!rele;
        }
        
    }
    attention();
      
}

void report(){//данные за время отчёта, средняя температура(погуглить), время, сколько секунд температура превышало норму
        int days=3; //
        unsigned int secondsReport=20;
        int now=0;
        while(now<secondsReport){
            now++;
            value+=nowTemperature;
            wait_us(second);
            
        }
        value/=now; //среднее число
        printf("value_20_sec=%4.2f_time:%d\n",value,secCritics);
        secCritics=0;
        report();
}


//4 потока(или таймер) Main, attention (мигание, реле), report, выгрузка данных в виде записей
int main(){
    threadAttention.start(attention);
    threadReport.start(report);
    wait_us(second);
    while(true){
        sensor.readData();
        nowTemperature=getTemperature();
        nowHumidity=getHumidity();
        write(nowTemperature,nowHumidity);
        wait_us(second);
    }
    

    
}

