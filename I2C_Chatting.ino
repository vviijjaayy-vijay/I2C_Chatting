#include <Wire.h>



int address = 64, recieve_followup = 0, cmdlength[10] = {5, 6, 1};
const int INPUTLIMIT = 100;
char writes[INPUTLIMIT], history[INPUTLIMIT], cmdlist[10][10] = {"login", "resend", "r", {0}};
const char ENDCHR = 3, CONTINUECHR = 147;

void login(char*);
void resend(char*);
void (*cmdactions[10])(char*) = {login, resend, resend};



void strcopy(char* origin, char* target) {
  for (int i = 0; !i || origin[i-1]; i++){
    target[i] = origin[i];
  }
}


void remove_line(char* str) {
  for (int i = 0; i < INPUTLIMIT; i++){
    if (!str[i]){
      str[i-1] = 0;
      break;
    }
  }
}


void i2c_messagewrite(char* message, int address) {
  Wire.beginTransmission(address);
  Wire.write(message);
  Wire.endTransmission();
}


void send_message(char* message, int address, int split_by = 32) {
  int idx = 0, iter;
  char message_chunk[split_by + 8] = {};
  // Serial.println((int)message[0]);

  Serial.print("[S#");
  Serial.print(address);
  Serial.print("] ");
  Serial.print(millis());
  Serial.print("ms > ");

  while (1){
    for (iter = idx; message[idx] && idx - iter < split_by-1; idx++){
      message_chunk[idx - iter] = message[idx];
      Serial.print(message[idx]);
    }

    message_chunk[idx - iter] = (message[idx])? CONTINUECHR: ENDCHR;
    i2c_messagewrite(message_chunk, address);
    if (!message[idx])  break;
  }
  Serial.println();
}


int i2c_messageread() {
  char last;

  while (Wire.available()){
    last = Wire.read();

    if (last == ENDCHR){
      Serial.println();
      return 0;
    }else if (last == CONTINUECHR){
      return 1;
    }else{
      Serial.print(last);
      // Serial.println((int)last);
    }
  }

  return -1;
}


void recieve_message(int howmuch){
  if (!recieve_followup){
    Serial.print("[R#");
    Serial.print(address);
    Serial.print("] ");
    Serial.print(millis());
    Serial.print("ms > ");
  }

  recieve_followup = i2c_messageread();
}


int find_command(char* cmd) {
  int cmdlen = 0, cmdidx = 0, match;
  char cmdinput[10];

  for (; cmd[cmdlen+1] != 0 && cmd[cmdlen+1] != 32; cmdlen++){
    cmdinput[cmdlen] = cmd[cmdlen+1];
  }

  while (cmdlist[cmdidx][0]){
    if (cmdlength[cmdidx] == cmdlen){
      match = 1;
      for (int i = 0; i < cmdlen; i++){
        if (cmdinput[i] != cmdlist[cmdidx][i]){
          match = 0;
          break;
        }
      }

      if (match)  return cmdidx;
    }
    cmdidx++;
  }

  return -1;
}


void command(char* cmd, int cmdidx) {
  if (cmdidx < 0){
    Serial.println("Invaild command");
    return;
  }

  int idx = 0, idxshift = cmdlength[cmdidx] +2;
  char params[INPUTLIMIT];
  for (; cmd[idx + idxshift -1]; idx++){
    params[idx] = cmd[idx + idxshift];
  }
  params[(idx) ?--idx :0] = 0;

  cmdactions[cmdidx](params);
}


void login(char* params) {
  if (!params[0]){
    Serial.println("Missing address parameter");
    return;
  }

  int new_address = 0;

  for (int i = 0; params[i]; i++){
    new_address *= 10;
    new_address += params[i] - 48;
  }

  if (new_address < 0 || new_address > 127){
    Serial.println("Invalid login address");
  }else{
    address = new_address;
    login_message();
  }
}


void login_message() {
  Serial.println("\n\n\n***************************");
  Serial.println("** I2C Chatting Platform **");

  Serial.print("** login on address ");
  Serial.print(address);
  for (int i = 0; i < 4 - (int)log10(address); i++)  Serial.print(" ");
  Serial.println("**");

  Serial.print("** message limit is ");
  Serial.print(INPUTLIMIT);
  for (int i = 0; i < 4 - (int)log10(INPUTLIMIT); i++)  Serial.print(" ");
  Serial.println("**");

  Serial.println("***************************");
}


void resend(char* params) {
  int repeat = 1;

  if (params[0]){
    for (int i = 0; params[i]; i++){
      repeat *= 10;
      repeat += params[i] - 48;
    }
  }

  for (int i = 0; i < repeat; i++)  send_message(history, address);
}



void setup() {
  // put your setup code here, to run once:
  // Wire.begin(address);
  Serial.begin(9600);
  login_message();
}


void loop() {
  // put your main code here, to run repeatedly:
  Serial.readString().toCharArray(writes, INPUTLIMIT);
  remove_line(writes);

  if (writes[0]){
    if (writes[0] == '/') command(writes, find_command(writes));
    else{
      strcopy(writes, history);
      send_message(writes, address);
    }
  }
  
  Wire.begin(address);
  Wire.onReceive(recieve_message);
}
