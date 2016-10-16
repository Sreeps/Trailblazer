#include <StackHeader.h>


StackHeader SH;

#define mpin1cw 1
#define mpin1acw 2
#define mpin2cw 3
#define mpin2acw 4
#define IRM1 A0
#define IRM2 A1
#define IRL A2
#define IRR A3
#define IRA A4
#define patchLedL 5
#define patchLedR 6
#define binary 7
#define octal 8
#define speedpin 9
/*mpin->motorpins cw clockwise acw anticlockwise
 mpin1 = left motor
 mpin2 = right motor
 IRM1 and IRM2 middle IR sensors
 IRL left IR sensor
 IRR right IR sensor
 IRA sensor above the bot
 */
short systrack = 0;
short i = 0;
short leftturn = 1;
short rightturn = 0;
short turn = 0;
char exparr[100],post[100];
short count = 0;
short analog[5];
char sensorCurrent[5],sensorPrevious[5];

short precedence(char c) {
  switch (c) {
    case '+':
    case '-': return 3;
      break;
    case '*':
    case '/': return 5;
      break;
    default: return -1;
  }
}
//converting from infix to postfix
char* ItoP(char exparr[100]) {
  short i, count = 0;
  char val, c;
  for (i = 0; i < strlen(exparr); i++)
  {
    c = exparr[i];
    if (c == '(')
      SH.push(SH.s, c);
    else if (c == ')') {
      while (SH.s->top->val != '(') {
        val = SH.pop(SH.s);
        post[count++] = val;
      }
      val = SH.pop(SH.s);
    }
    else if (c == '+' || c == '-' || c == '*' || c == '/') {
      while (SH.isEmpty(SH.s) == 0 && precedence(c) < precedence(SH.s->top->val) && SH.s->top->val != '(') {
        val = SH.pop(SH.s);
        post[count++] = val;
      }
      SH.push(SH.s, c);
    }
    else if (isdigit(c))
      post[count++] = c;

  }
  while (SH.isEmpty(SH.s) == 0) {
    val = SH.pop(SH.s);
    post[count++] = val;
  }
  delete SH.s;

}
//Evaluating the postfix expession
int PostEval(char post[100]) {
  char c;
  short i;
  short a, b, c1;
  for (i = 0; i < strlen(post); i++)
  {
    c = post[i];
    if (isdigit(c))
      SH.push(SH.s, c);
    else if (c == '+' || c == '-' || c == '/' || c == '*') {
      a = SH.pop(SH.s) - 48;
      b = SH.pop(SH.s) - 48;
      switch (c) {
        case '+': c1 = a + b;
          break;
        case '-': c1 = b - a;
          break;
        case '*': c1 = a * b;
          break;
        case '/': c1 = b / a;
          break;
        default:
          ;
      }
      SH.push(SH.s, c1 + 48);
    }
  }
  return SH.pop(SH.s) - 48;
  delete SH.s;
}
//to convert sensor data to binary string
void sensorData(){
  analog[0]=analogRead(IRL);
  analog[1]=analogRead(IRM1);
  analog[2]=analogRead(IRA);
  analog[3]=analogRead(IRM2);
  analog[4]=analogRead(IRR);
  for(short i=0;i<5;++i){
    if(analog[i]>800)
      sensorCurrent[i]='1';//black line has been detected.
    else
      sensorCurrent[i]='0';//no black line detected.
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(mpin1cw, OUTPUT);
  pinMode(mpin1acw, OUTPUT);
  pinMode(mpin2cw, OUTPUT);
  pinMode(mpin2acw, OUTPUT);
  pinMode(IRM1, INPUT);
  pinMode(IRM2, INPUT);
  pinMode(IRL, INPUT);
  pinMode(IRR, INPUT);
  pinMode(IRA, INPUT);
  pinMode(patchLedL, OUTPUT);
  pinMode(patchLedR, OUTPUT);
  pinMode(binary, OUTPUT);
  pinMode(octal, OUTPUT);
  pinMode(speedpin, OUTPUT);
  digitalWrite(speedpin, LOW);
  digitalWrite(mpin1cw, LOW);
  digitalWrite(mpin1acw, LOW);
  digitalWrite(mpin2cw, LOW);
  digitalWrite(mpin2acw, LOW);
  digitalWrite(patchLedL,LOW);
  digitalWrite(patchLedR,LOW);
  digitalWrite(binary,LOW);
  digitalWrite(octal,HIGH);
  strcpy(sensorPrevious,"00000");
  strcpy(sensorCurrent,"00000");
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly
    sensorData();
    if(sensorCurrent[2]=='0'){
      digitalWrite(speedpin, LOW);
      ItoP(exparr);
      PostEval(post);
    }
    else{
      if(strcmp(sensorCurrent,"10000")==0){//for turning left
         leftturn = 1;
         rightturn = 0;
         turn = turn +1;
         if(turn%2 == 0){//changing number system
          digitalWrite(binary,HIGH);
          digitalWrite(octal,LOW);
         }
         else{
          digitalWrite(binary,LOW);
          digitalWrite(octal,HIGH);           
         }
         digitalWrite(speedpin, HIGH);
         digitalWrite(mpin1cw, LOW);
         digitalWrite(mpin1acw, HIGH);
         digitalWrite(mpin2cw, LOW);
         digitalWrite(mpin2acw, LOW);
      }
      else if(strcmp(sensorCurrent,"00001")==0){//for turning right
        rightturn = 1;
        leftturn = 0;
        turn = turn + 1;
        if(turn%2 == 0){//changing number system
          systrack = 0;
          digitalWrite(binary,HIGH);
          digitalWrite(octal,LOW);
         }
         else{
          systrack = 1;
          digitalWrite(binary,LOW);
          digitalWrite(octal,HIGH);           
         }
        digitalWrite(speedpin, HIGH);
        digitalWrite(mpin1cw, LOW);
        digitalWrite(mpin1acw, LOW);
        digitalWrite(mpin2cw, LOW);
        digitalWrite(mpin2acw, HIGH);
      }
      else if(strcmp(sensorCurrent,"01010")==0){//detecting patch in the front
        leftturn = rightturn = 0;
        digitalWrite(speedpin, HIGH);
        digitalWrite(mpin1cw, LOW);
        digitalWrite(mpin1acw, HIGH);
        digitalWrite(mpin2cw, LOW);
        digitalWrite(mpin2acw, HIGH);
      }
      else if(strcmp(sensorCurrent,"11010")==0){//detecting left patch
        leftturn = rightturn = 0;
        count = count+1;
        digitalWrite(speedpin, HIGH);
        digitalWrite(mpin1cw, LOW);
        digitalWrite(mpin1acw, HIGH);
        digitalWrite(mpin2cw, LOW);
        digitalWrite(mpin2acw, HIGH);
        digitalWrite(patchLedL, HIGH);
      }
      else if(strcmp(sensorCurrent,"01011")==0){//detecting right patch
        leftturn = rightturn = 0;
        count = count+1;
        digitalWrite(speedpin, HIGH);
        digitalWrite(mpin1cw, LOW);
        digitalWrite(mpin1acw, HIGH);
        digitalWrite(mpin2cw, LOW);
        digitalWrite(mpin2acw, HIGH);
        digitalWrite(patchLedR, HIGH);
    }  
    if(strcmp(sensorCurrent,sensorPrevious)!=0){//to prevent continuous reading
      if(count%2 == 0){//to detect operators
        if(leftturn == 1 && rightturn == 0){//checking previous turn
          if(sensorCurrent[0] == '1' && sensorCurrent[4] == '0')
             exparr[count] == '-';
          else if(sensorCurrent[0] == '0' && sensorCurrent[4] == '1')
            exparr[count] == '+' ;
        }
        else if(leftturn == 0 && rightturn == 1){//checking previous turn
          if(sensorCurrent[0] == '1' && sensorCurrent[4] == '0')
             exparr[count] == '/';
          else if(sensorCurrent[0] == '0' && sensorCurrent[4] == '1')
            exparr[count] == '*' ;
        }
      }
      else{//operands
        if(systrack == 0){//binary format
          if(sensorCurrent[0] == '1' && sensorCurrent[4] == '0')
             exparr[count] == '2';
          else if(sensorCurrent[0] == '0' && sensorCurrent[4] == '1')
            exparr[count] == '1' ;
        }
        else if(systrack == 1){//octal format
          if(sensorCurrent[0] == '1' && sensorCurrent[4] == '0')
             exparr[count] == '8';
          else if(sensorCurrent[0] == '0' && sensorCurrent[4] == '1')
            exparr[count] == '1' ;
        }
      }
    }
    strcpy(sensorPrevious,sensorCurrent);
    }
}






