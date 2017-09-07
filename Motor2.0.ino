//Set PIN
int PUL = 4;
int DIR = 5;
int Switch1 = 8; //竖直方向限位开关
int Switch2 = 9; //水平方向限位开关
int Switch3 = 10;//滑轨限位开关
int relayPin = 11; //控制灯箱继电器
//Set constant
int vertical = 1;
int horizontal = 0;
String command = "";
int LeftFlag = 0;
int RightFlag = 0;
int UpFlag = 0;
int DownFlag = 0;
int FBStepCount = 0;

int x0 = 1;
int y0 = 1;

void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(Switch1, INPUT);
  pinMode(Switch2, INPUT);
  pinMode(Switch3, INPUT);
  pinMode(relayPin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  Init();
  while(1) {
    while(Serial.available()) {
      command += char(Serial.read());
      delay(10);  
    }
    if (command.length()>0) {
      if (command == "lighton") SetLight(1);
      else if (command == "lightoff") SetLight(0);
//      else if (command == "up") MoveUp();
//      else if (command == "down") MoveDown();
//      else if (command == "left") MoveLeft();
//      else if (command == "right") MoveRight();
      else if (command == "initial") InitialPosition();
      else if (command == "initial2") InitialForward();
      else if (command == "testUD") TestUD();
      else if (command == "auto") AutoMove(3,5);
      else if (command == "focus") Focus();
//      else if (command == "forward") MoveForward();
//      else if (command == "backward") MoveBack();
      else if (GetOrder(command)) command = "";
      else {
        Serial.println("WRONG ORDER");
        command = "";
      }
    }  
  }
}


/*************************************************************************/
//初始化函数
void Init() {
  digitalWrite(relayPin, LOW); //灯箱给5V
  digitalWrite(DIR, HIGH); //Dir forward  
}
void InitPosition() {
  x0 = 1;
  y0 = 1;
}

//灯箱继电器的控制
void SetLight(int state) {
  if(state == 1) {
    digitalWrite(relayPin, HIGH);
    Serial.println("LIGHT ON");
    command = "";
  }
  if(state == 0) {
    digitalWrite(relayPin, LOW);
    Serial.println("LIGHT OFF");
    command = "";  
  }  
}
int TestUD() {
  for (int i = 0;i<100;i++){
      MoveDown(7);
      delay(500);
      MoveUp(7);
      delay(500);
  }
}

//命令识别
int GetOrder(String command) {
  if(command.startsWith("up")) {
    String str = command.substring(command.indexOf(',')+1, command.length());
    int num = str.toInt();
    MoveUp(num);
    return 1;
  }
  else if (command.startsWith("down")) {
    String str = command.substring(command.indexOf(',')+1, command.length());
    int num = str.toInt();
    MoveDown(num);
    return 1;
  }
  else if (command.startsWith("left")) {
    String str = command.substring(command.indexOf(',')+1, command.length());
    int num = str.toInt();
    MoveLeft(num);
    return 1;
  }
  else if (command.startsWith("right")) {
    String str = command.substring(command.indexOf(',')+1, command.length());
    int num = str.toInt();
    MoveRight(num);
    return 1;
  }
  else if (command.startsWith("forward")) {
    String str = command.substring(command.indexOf(',')+1, command.length());
    int num = str.toInt();
    FBStepCount -= num;
    String count = String(FBStepCount);
    MoveForward(num);
    delay(500);
    Serial.println("num:"+count);
    return 1;
  }
  else if (command.startsWith("backward")) {
    String str = command.substring(command.indexOf(',')+1, command.length());
    int num = str.toInt();
    FBStepCount += num;
    String count = String(FBStepCount);
    MoveBack(num);
    delay(500);
    Serial.println("num:"+count);
    return 1;
  }
  else if (command.startsWith("auto")) {
    String str1 = command.substring(command.indexOf('o')+1, command.indexOf(','));
    String str2 = command.substring(command.indexOf(',')+1, command.length());
    int num1 = str1.toInt();
    Serial.println(num1);
    int num2 = str2.toInt();
    Serial.println(num2);
    AutoMove(num1,num2);
    return 1;
  }
  else return 0;  
}

//电机选择
void SetMotor(int type) {
  if(type == vertical) {
    PUL = 4;
    DIR = 5;
  }
  if(type == horizontal) {
    PUL = 6;
    DIR = 7;
  }
  if(type == 2) {
    PUL = 2;
    DIR = 3;
  }
}

//电机方向选择
void SetDir(int state) {
  if(state == 1) digitalWrite(DIR,HIGH);
  if(state == 0) digitalWrite(DIR,LOW);
}

//电机运动
void MotorMove(int s) {
   for(int i=0;i<(8*2*s);i++){
     digitalWrite(PUL, LOW);
     delayMicroseconds(650*(0.5*cos(((PI*i)/(8*2*s)))+1.5));
     digitalWrite(PUL, HIGH);
     //delayMicroseconds(1500/(sin(((PI*i)/(8*2*s)))+1));
     delayMicroseconds(650*(0.5*cos(((PI*i)/(8*2*s)))+1.5));
   }
   command = "";  
}
//前后电机运动
void MotorMove2(int s) {
   int i = 1;
   for(i = 1;i<=(8*s);i++){
     digitalWrite(PUL, LOW);
     delayMicroseconds(650*(0.5*cos(((2*PI*i)/(8*s)))+1.5));
     digitalWrite(PUL, HIGH);
     delayMicroseconds(650*(0.5*cos(((2*PI*i)/(8*s)))+1.5));
   } 
   command = "";  
}

//向上运动
void MoveUp(int num) {
  SetMotor(vertical);
  SetDir(1);
  if (UpFlag == 0){
    MotorMove(num);
    Serial.println("UP");
    delay(500);
    if (digitalRead(Switch1) == 0) {
      Serial.println("OVERMOVED");
      UpFlag = 1;
    }
    else DownFlag = 0;
  }
  else {
    Serial.println("ERROR");
    command = "";
  }
}

//向下运动
void MoveDown(int num) {
  SetMotor(vertical);
  SetDir(0);
  if (DownFlag == 0) {
    MotorMove(num);
    Serial.println("DOWN");
    delay(500);
    if (digitalRead(Switch1) == 0) {
      Serial.println("OVERMOVED");
      DownFlag = 1;
    }
    else UpFlag = 0;
  }
  else {
    Serial.println("ERROR");
    command = "";              
  }
}

//向左运动
void MoveLeft(int num) {
  SetMotor(horizontal);
  SetDir(1);
  if (LeftFlag == 0) {
    MotorMove(num);
    Serial.println("LEFT");
    delay(500);
    if (digitalRead(Switch2) == 0) {
      Serial.println("OVERMOVED");
      LeftFlag = 1;
    }
    else RightFlag = 0;
  }
  else {
    Serial.println("ERROR");
    command = "";              
  }
}

//向右运动
void MoveRight(int num) {
  SetMotor(horizontal);
  SetDir(0);
  if (RightFlag == 0) {
    MotorMove(num);
    Serial.println("RIGHT");
    delay(500);
    if (digitalRead(Switch2) == 0) {
      Serial.println("OVERMOVED");
      RightFlag = 1;
    }
    else LeftFlag = 0;
  }
  else {
    Serial.println("ERROR");
    command = "";              
  }
}

//向前运动
void MoveForward(int num) {
  SetMotor(2);
  SetDir(0);
  if (digitalRead(Switch3) == 1) {
    Serial.println("ORIGIN");
  }
  MotorMove2(num);
  //Serial.println("FORWARD");
  command = "";
}

//向后运动
void MoveBack(int num) {
  SetMotor(2);
  SetDir(1);
  if (digitalRead(Switch3) == 1) {
    Serial.println("ORIGIN");
  }
  MotorMove2(num);
  //Serial.println("BACKWARD");
  command = "";
}

//找到中点
void InitialPosition() {
  int StepCount = 0;
  int CentralPosition = 0;
  for(int i = 0;i<2;i++) {
    StepCount = 0;
    CentralPosition = 0;
    SetMotor(i);
    //Go Up
    SetDir(1);
    while((digitalRead(Switch1) == 1)&(digitalRead(Switch2) == 1)) {  
      MotorMove(1);
      StepCount++;
      //delay(20);  
    }
    delay(300);
    //Go Down
    SetDir(0);
    MotorMove(StepCount);
    while((digitalRead(Switch1) == 1)&(digitalRead(Switch2) == 1)) {  
      MotorMove(1);
      StepCount++;
      //delay(20);  
    }
    delay(300);
    //Go to center
    CentralPosition = StepCount/2;
    SetDir(1);
    MotorMove(CentralPosition);
    delay(300);
  }
  delay(300);
  InitPosition();
  //MoveRight(1);
}

//找到起始点
void InitialForward() {
  int CentralPosition = 0;
  FBStepCount = 0;
  SetMotor(2);
  SetDir(0);
  while(digitalRead(Switch3) == 0) {  
    MotorMove2(1);
    FBStepCount++; 
//    if(digitalRead(Switch3) == 1)Serial.println("original");
  }
  delay(500);
  SetDir(1);
  MotorMove2(FBStepCount);
  String count = String(FBStepCount);
  Serial.println("num:"+count);
}
//找到对焦点
void Focus() {
  FBStepCount = 0;
  SetMotor(2);
  SetDir(0);
  while(digitalRead(Switch3) == 0) {  
    MotorMove2(1);
    FBStepCount++; 
  }
  delay(500);
  SetDir(1);
  MotorMove2(686);
  //String count = String(FBStepCount);
  Serial.println("num:686");
}

//自动拍照程序
void AutoMove(int num1,int num2) {
  for(int i=0;i<3;i++){
    int t =i - y0;
    if(t<0){
      MoveUp(num2*abs(t));
    }
    if(t>0){
      MoveDown(num2*abs(t));
    }
    y0 = i;
    delay(4000);
    for(int j=0;j<3;j++){
      int s =j - x0;
      if(s<0){
        MoveLeft(num1*abs(s));
      }
      if(s>0){
        MoveRight(num1*abs(s));
      }
      x0 = j;
      delay(4000);
    }
  }
}
