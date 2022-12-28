#include <mysql.h>
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdint.h>
#include <math.h>

#define Sample_rate 25
#define Config 26
#define Gyro_config 27
#define Acc_config 28
#define Interrupt 56
#define PWR_Managment 107

#define Acc_X 59
#define Acc_Y 61
#define Acc_Z 63
#define channel 0
#define INT_MPU 0

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;

char *server = "localhost";
char *user = "quangthanh";
char *password = "123456";
char *database = "mpu6050";

int mpu;

float roll,pitch,Ax,Ay,Az;

float sample;

int8_t dlfp, gypro, acc, interrupt;

float LSB_Sensitivity;

char cmd[200];

int16_t read_sensor(unsigned char sensor)
{   
    int16_t high, low, data;
    high= wiringPiI2CReadReg8(mpu, sensor);
    low= wiringPiI2CReadReg8(mpu, sensor+1);
    data=(high<<8)|low;
    return data;
}

void Init_6050(float sample_rate, int8_t dlfp, int8_t gypro_cf, int8_t acc_cf, int8_t interrupt)
{ 
    wiringPiI2CWriteReg8(mpu, Sample_rate, sample_rate); 
    wiringPiI2CWriteReg8(mpu, Config, dlfp); 
    wiringPiI2CWriteReg8(mpu, Gyro_config, gypro_cf); 
    wiringPiI2CWriteReg8(mpu, Acc_config, acc_cf); 
    wiringPiI2CWriteReg8(mpu, Interrupt, interrupt); 
    wiringPiI2CWriteReg8(mpu, PWR_Managment, 0x01);
}

void ReadAcc(void)
{
    Ax= (float)read_sensor(Acc_X)/LSB_Sensitivity;
    Ay= (float)read_sensor(Acc_Y)/LSB_Sensitivity;
    Az= (float)read_sensor(Acc_Z)/LSB_Sensitivity;

    roll=atan2(Ay,sqrt(pow(Ax,2)+pow(Az,2)))*180/M_PI;
    pitch=atan2(Ax,sqrt(pow(Ay,2)+pow(Az,2)))*180/M_PI;
     
    char sql[200];
    sprintf(sql, "insert into data(rollmode,pitchmode) values(%.2f, %.2f)", roll,pitch);
    // send SQL query
    mysql_query(conn, sql);
}

void InterruptMPU()
{
}

int main(int argc, char* argv[]) {
    //setup wiringPI, ISR
    wiringPiSetup();
    pinMode(INT_MPU, INPUT);
    wiringPiISR(INT_MPU, INT_EDGE_RISING, &InterruptMPU);

    //setup I2C
    mpu=wiringPiI2CSetup(0x68);

    char msg[200];
    while(1) {
        conn = mysql_init(NULL);
        mysql_real_connect(conn,server,user,password,database,0,NULL,0); 

        char sql[200];
        sprintf(sql, "select * from dataMPU");
        mysql_query(conn,sql);
        res = mysql_store_result(conn); 
        row = mysql_fetch_row(res); //row[0]-> sample_rate; row[1]->dlpf; row[2]->acc; row[3]->gypro; row[4]->int_interrupt, row[5]->isUpdate;
        
        if (atoi(row[5])==1)
        {
            if (atoi(row[1])==0|atoi(row[1])==7) sample=(float)8000/atoi(row[0]) - 1;
            else sample=(float) 1000/atoi(row[0]) - 1;

            dlfp = atoi(row[1]);
            int_interrupt = atoi(row[4]);

            switch (atoi(row[2]))
            {
                case 1:
                    LSB_Sensitivity=8192.0;
                    acc=8;
                    break;
                case 2:
                    LSB_Sensitivity=4096.0;
                    acc=16;
                    break;
                case 3:
                    LSB_Sensitivity=2048.0;
                    acc=24;
                    break;
                default:
                    LSB_Sensitivity=16384.0;
                    acc=0;
                    break;
            }
            switch (atoi(row[3]))
            {
                case 1:
                    gypro=8;
                    break;
                case 2:
                    gyro=16;
                    break;
                case 3:
                    gypro=24;
                    break;
                default:
                    gypro=0;
                    break;
            }
        }

        Init_6050(sample,dlfp,gypro,acc,interrupt);

        ReadAcc();
        delay(1000);
    }

}