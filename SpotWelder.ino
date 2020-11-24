//硬件：Atmega168P, internal 8MHz;
//2020.10.8 增加5K可变电阻，可变电阻的最大值决定焊接的最长时间，比如阻值为5K时，最大焊接时间为20ms，则2.5K时为10ms，通过调整阻值来到调节焊接时间。
#define Buzzer 5  //蜂鸣器引脚
#define WelderDetect 13  //点焊笔检测引脚
#define Solder 12  //MOS驱动引脚
#define VoltTestPin  A1  //电压检测引脚
#define VoltTestPower  A2  
#define InitialSolderTimePin A3 //焊接时间检测引脚
#define MaxSolderTimePower A4

float MaxSolderTime=20.0;  //5.5V电压时最大焊接时间，可以自己修改
float InitialSolderTime;   //当前的焊接时间，通过读取可变电阻的阻值来决定
int SolderTime;    //转换成当前电压后的焊接时间          
int v;  //电压的模拟读数
int t;  //焊接时间的模拟读数
float volt;

void setup() {
  pinMode(Buzzer, OUTPUT);
  pinMode(Solder, OUTPUT);
  pinMode(VoltTestPower,OUTPUT);
  pinMode(MaxSolderTimePower,OUTPUT);
  analogReference(INTERNAL); //ATmega芯片内部参考电压，1.1V
  pinMode(WelderDetect,INPUT_PULLUP);  //利用内置的上拉电阻
  digitalWrite(Buzzer,HIGH);  //开机提示音
  delay(500);
  digitalWrite(Buzzer,LOW);
  delay(1000);
}

void loop() {
  //检测电源电压
  digitalWrite(VoltTestPower, HIGH);
  v=analogRead(VoltTestPin);
  delay(100);
  v=analogRead(VoltTestPin);
  digitalWrite(VoltTestPower, LOW);
  volt= v*0.00383;     //换算成当前电压，0.00383=(1.1/1023.0)/390*(390+1000);//R1=390K  R2=1000K
  
	//计算当前的最大焊接时间
	digitalWrite(MaxSolderTimePower, HIGH);
	t=analogRead(InitialSolderTimePin); //该读数会因为电容电压的变化而变化
	delay(100);
	t=analogRead(InitialSolderTimePin);
	digitalWrite(MaxSolderTimePower, LOW);
  InitialSolderTime=MaxSolderTime*t*5.5/volt/1023;  //将当前模拟读数映射到5.5V电压，以对应5.5V时的MaxSolderTime
  if(InitialSolderTime>MaxSolderTime){
      InitialSolderTime=MaxSolderTime;
  }
  //随电压变化自动调节焊接时间，Q=U2/R*t，输出的能量固定的情况下，焊接时间与电压的平方成反比
  SolderTime = int(InitialSolderTime*30.25/volt/volt);  //5.5*5.5=30.25
	

  if(v>910){         //v=910时，电源电压约为3.5V
    //监测低电平信号，并持续1秒时间  
    if(digitalRead(WelderDetect)==LOW) {
      delay(250);
      if(digitalRead(WelderDetect)==LOW){
        delay(250);
        if(digitalRead(WelderDetect)==LOW){
          delay(250);
          if(digitalRead(WelderDetect)==LOW){
              delay(250);
              if(digitalRead(WelderDetect)==LOW){
                  digitalWrite(Buzzer, HIGH);  //蜂鸣器提示
                  delay(200);
                  digitalWrite(Buzzer, LOW);
                  digitalWrite(Solder, HIGH);//预焊接
                  delay(3);
                  digitalWrite(Solder, LOW);
                  delay(2); 
                  digitalWrite(Solder, HIGH);//焊接
                  delay(SolderTime);
                  digitalWrite(Solder, LOW);
                  delay(2000);
              }
            }
          }
        }
     }
  } else {
    for (int i = 0; i < 4; i++)
    {
      digitalWrite(Buzzer, HIGH);
      delay(500);
      digitalWrite(Buzzer, LOW);
      delay(500);
    }
  }
}
