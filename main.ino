#include <Servo.h>
#include <MsTimer2.h>
#include <LiquidCrystal.h>

#define interruptPin 3

LiquidCrystal lcd(4, 6, 10, 11, 12, 13);
Servo servo1;
Servo servo2;

//주차장 상태를 나타내는 변수
int state1;
int state2;

//요금을 받았는지를 나타내는 변수
int state3 =0;

//주차 상태를 나타내는 led
int led1 = 2;
int led2 = 1;

//주차가 되었는지 감지하는 센서 변수
int CDSValue1;
int CDSValue2;

//주차장 남은 자리를 나타내는 변수
int s;

//출차주의 알림을 위한 피에조 부저
int piezo = 8;

void setup(){
 
  servo1.attach(9);
  servo2.attach(5);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  lcd.begin(16,2);
  
  //주차정산 후 버튼을 누르면 발생하는 외부 인터럽트
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), abc ,FALLING);
  
  //실시간 주차장 조회를 위한 타임인터럽트
  MsTimer2::set(50,CDS);
  MsTimer2::start();

}

void loop(){
//주차자리에 있는조도센서로 읽어들인 아날로그 값
  CDSValue1 = analogRead(A0);
  CDSValue2 = analogRead(A1);

//입구에 있는 조도센서로 읽어들인 아날로그 값
int CDSValue3 = analogRead(A2);

//입구에 차가 감지되었을 때 차단기 열림, 단 주차장이 만원이면 열리지 않음
  if((CDSValue3<500)&&((state1)||(state2))){
    servo1.write(90);
    delay(2000);
  }
  else
    servo1.write(0);

  delay(10);

  //주차장 남은 여석 집계
  s = state1 + state2;

 //주차장 남은 여석 모니터에 표시
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LEFT LOT : ");
  lcd.print(s);
  delay(500);

//요금이 정산된 상태일 때 출구의 차단기가 열리고 출차 주의를 알리는 소리 발생
if(state3==1){
   servo2.write(90);
  delay(1000);
  servo2.write(0);
 tone(piezo,1);
 delay(500);
 noTone(piezo);
  state3=0;
}
else
state3=0;
}

/*  */
//외부 인터럽트 발생시 호출되는 함수
void abc(){
 state3 = 1;
}

//타임 인터럽트 발생시 호출되는 함수
void CDS(){
  //차가 조도 센서를 가려서 연결된 저항의 전압이 낮아지게 되면 주차가 되었음으로 인식하고 led꺼짐 및 상태변수 변화
  if(CDSValue1<800){
    state1=0;
    digitalWrite(led1,LOW);
  }
  else { //주차되었던 차량이 나가고 빛이 들어오면 주차장이 비었음으로 인식하고 led 켜짐 및 상태변수 변화
  state1=1; 
    digitalWrite(led1, HIGH);
  }
  //2번 자리도 같은 방법
  if(CDSValue2<800){
    state2=0;
    digitalWrite(led2,LOW);
  }
  else {
  state2=1;
    digitalWrite(led2,HIGH);
  }
}
