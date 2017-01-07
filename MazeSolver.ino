#include <QueueList.h>


QueueList<char> Q;
int l1=5;
int l2=6;

int r1=10;
int r2=9;

int a[6];
int last_proportional=0;
int integral=0;
int flag=0;//used in Shortest Path function
int mode;//to select the shortest path or not
int hasStopReached;//to check if the bot has reached the destination once
#define THRESHOLD 560
#define LENGTH 50

char select_turn(unsigned char found_left,unsigned char found_right,unsigned char found_st); //AI INITIALIZAION
int set_motors(int a,int b);
void turn(char dir);
void PID();
void ShortestPath();

int right=0;
int left=0;

void setup() {

  pinMode(l1,OUTPUT);   // Left motor output
  pinMode(l2,OUTPUT);
  
  pinMode(r1,OUTPUT);  //Right motor output
  pinMode(r2,OUTPUT);

  mode = 0;//not to use shortest path
  Serial.begin(9600);
}

void loop() 
{
  PID();   
 
  unsigned char found_left=0;
  unsigned char found_right=0;   
  unsigned char found_st=0;

  readline();
  if(a[0]==LOW)
    found_left=1;
 
  if(a[1]==HIGH&&(a[2]==LOW||a[3]==LOW)&&a[4]==HIGH)
  found_st=1;
     
  if(a[5]==LOW)
    found_right=1;
 

  unsigned char dir;
  Serial.print(" ");
  Serial.print(found_left);
  Serial.print(" ");
  Serial.print(found_right);
  Serial.print(" ");
  Serial.print(found_st);
  Serial.print(" ");
  dir=select_turn(found_left, found_right, found_st);
  turn(dir);
}

int set_motors(int l,int r) // Motor setup
{
  Serial.print(l);
  Serial.print(" ");
  Serial.println(r);
 
if(l>0&&r>0)
{
  analogWrite(l1,0);
  analogWrite(l2,l);
  analogWrite(r1,0);
  analogWrite(r2,r);
}
else if(l<0&&r>0)
{
  analogWrite(l1,-l);
  analogWrite(l2,0);
  analogWrite(r1,0);
  analogWrite(r2,r);  
}
else if(l>0&&r<0)
{
  analogWrite(l1,0);
  analogWrite(l2,l);
  analogWrite(r1,-r);
  analogWrite(r2,0);
}
else if(l==0&&r==0)
{
  analogWrite(l1,l);
  analogWrite(l2,l);
  analogWrite(r1,r);
  analogWrite(r2,r);
}
}

int readline()
{
 a[0]=digitalRead(4);
 Serial.print(a[0]);
 Serial.print(" ");
 for(int i=4;i>=0;i--) 
 {   if(analogRead(A0+i)<=THRESHOLD)
    a[5-i]=LOW;
    else
    a[5-i]=HIGH;
    Serial.print(a[5-i]);
    Serial.print("  ");
} 

 int v;
 v=(5000*a[0]+4000*a[1]+3000*a[2]+2000*a[3]+1000*a[4]+0*a[5])/(a[0]+a[1]+a[2]+a[3]+a[4]+a[5]);
 
 return v;
}

int STOP()
{
  analogWrite(l1,255);
  analogWrite(l2,255);
  analogWrite(r1,255);
  analogWrite(r2,255);
  hasStopReached=1;
  ShortestPath();
}

void turn(char dir)  
{
  set_motors(0,0);
  delay(5);
  switch(dir)
  {
    case 'L':
      while(1){
      readline();
      set_motors(-50,50);
      if((a[2]==LOW&&a[3]==LOW))
      break;
      }
      break;
    case 'R':
      while(1){
      readline();
      set_motors(50,-50);
      if((a[2]==LOW&&a[3]==LOW))
      break;
      }
      break;
    case 'B':
      while(1){
      readline();
      set_motors(50,-50);
      if((a[2]==LOW&&a[3]==LOW))
      break;
      }
      break;
    case 'S':
    break;
  }
}

void PID()
{ 
  int i;       
  int power_difference=0;
  float Kp,Ki,Kd;
  unsigned int position;
  int derivative,proportional;
  while(1)
  { 
  position=readline();
  
  proportional=((int)position-2500);
  derivative = proportional-last_proportional;
  integral = integral+proportional;
  last_proportional=proportional;
  
  Kp=0.1;
  Ki=0.0001;
  Kd=1.0;

  power_difference= proportional*Kp + integral*Ki + derivative*Kd;
  const int max=120;
  if(power_difference>max)
   power_difference=max;
  if(power_difference<-max)
   power_difference= (-1*max);
   
  if(power_difference<0)
  {
    //left
   set_motors(max+power_difference,max);
  }
  else
  {
   set_motors(max,max-power_difference); 
  }
  readline();
  if(a[0]==HIGH&&a[1]==HIGH&&a[2]==HIGH&&a[3]==HIGH&&a[4]==HIGH&&a[5]==HIGH)
   STOP();
  else if(a[0]==LOW||a[5]==LOW)
    return;
  }  
}

char select_turn(unsigned char found_left,unsigned char found_right,unsigned char found_st) // AI Function
{
 char turn;
 if(hasStopReached==0){
 if(found_left==1){
   Q.push('L');
   return 'L';
 }
 else if(found_st==1){
  Q.push('S');
  return 'S';
 } 
 else if(found_right==1){
  Q.push('R');
  return 'R';
 } 
 else{
  Q.push('B');
  return 'B';
 } 
 }
 else
 {
  turn = Q.pop();
  return turn;
 }
}

void ShortestPath(){
  int len = Q.count();
  char path[LENGTH];
  int i;
  if(len<3 || (len==3 && flag==0))
    return;
  for(i=0;i<len;i++){
    path[i] = Q.pop();
  }
  for(i=0;i<len;i++){
    
    if(path[i]=='B')
    {
      if(path[i-1]=='L' and path[i+1]=='L'){
        path[i-1]=path[i]='X';
        path[i+1]='S';
        i++;
        continue;
      }        
      else if(path[i-1]=='L' and path[i+1]=='R'){
        path[i-1]=path[i]='X';
        path[i+1]='B';
        i++;
        continue;
      }        
      else if(path[i-1]=='L' and path[i+1]=='S'){
        path[i-1]=path[i]='X';
        path[i+1]='R';
        i++;
        continue;
      }        
      else if(path[i-1]=='R' and path[i+1]=='L'){
        path[i-1]=path[i]='X';
        path[i+1]='B';
        i++;
        continue;
      }        
      else if(path[i-1]=='S' and path[i+1]=='L'){
         path[i-1]=path[i]='X';
         path[i+1]='R';
         i++;
         continue;      
      }       
      else if(path[i-1]=='S' and path[i+1]=='S'){
        path[i-1]=path[i]='X';
        path[i+1]='B';
        i++;
        continue;
      }        
    }
  }
  for(i=0;i<len;i++)
    if(path[i]=='L' || path[i]=='S' || path[i]=='R' || path[i]=='B'){
       if(path[i] == 'B')
          flag=1;
       Q.push(path[i]);
    }
  ShortestPath();
}



