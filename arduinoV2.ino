#include <WiFi.h>
#include <HTTPClient.h>
#include <string.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <time.h>
#include <secrets.h>


#define id1 13
#define id2 14
#define id3 27
#define id4 26
#define id5 25
#define id6 33
#define id7 32

char ssid[] =  "";
char password[] = "";
char server_address[] = "";

bool primeiraIteracao;
bool ativadoList;
bool primeiraIteracaoContinuar;
bool continuar;

HTTPClient http;
WiFiClient wifi;

int i = 0;
int j = 0;
int k = 0;
int l = 0;
int m = 0;
int atrasos;
int repetir;
int IdValue;
int acesoValue;
int delayValue;
int acesoList;
int delayList;
int ledList;

JsonArray delays;
JsonArray acesos;
JsonArray Ids;

unsigned long ledAtivoMSList;
unsigned long ledAtivoMSfinal;
unsigned long ledAtivoMS;

unsigned long *leds = (unsigned long*)malloc(30 * sizeof(unsigned long));
bool *ativou = (bool*)malloc(30 * sizeof(bool));

time_t tempoFinalSegundos;
time_t tempoAtivoSegundos;

void piscarLeds();
void checar();
void checarEmTempoReal();

void setup() {

  Serial.begin(9600);

  pinMode(id1,OUTPUT);
  pinMode(id2,OUTPUT);
  pinMode(id3,OUTPUT);
  pinMode(id4,OUTPUT);
  pinMode(id5,OUTPUT);
  pinMode(id6,OUTPUT);
  pinMode(id7,OUTPUT);

  digitalWrite(id1,HIGH);
  digitalWrite(id2,HIGH);
  digitalWrite(id3,HIGH);
  digitalWrite(id4,HIGH);
  digitalWrite(id5,HIGH);
  digitalWrite(id6,HIGH);
  digitalWrite(id7,HIGH);


  delay(1000);

  
  if (leds == NULL) {
    Serial.println("Erro na alocação de memória!");
    while (1);  
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");

  }
  configTime(0, 0, "pool.ntp.org");


  digitalWrite(id1,LOW);
  digitalWrite(id2,LOW);
  digitalWrite(id3,LOW);
  digitalWrite(id4,LOW);
  digitalWrite(id5,LOW);
  digitalWrite(id6,LOW);
  digitalWrite(id7,LOW);


  delay(1000);

}
void loop() {

  http.begin(server_address);
  int httpCode = http.GET();

  if(httpCode == HTTP_CODE_OK){

   String response = http.getString();

   const size_t capacity = JSON_OBJECT_SIZE(4) + 30;
   DynamicJsonDocument doc(capacity);

   DeserializationError error = deserializeJson(doc, response);

   if (error) {
    Serial.println("JSON FALHOU");
    return;
    }

    repetir = doc["repetir"];
    delays = doc["delays"];
    acesos = doc["acesos"];
    Ids = doc["ids"];

    Serial.println("Repetir: " + String(repetir));

    Serial.println("Delays:");
    for (int i = 0; i < delays.size(); i++) {
      Serial.println(delays[i].as<String>());
    }

    Serial.println("Acesos:");
    for (int i = 0; i < acesos.size(); i++) {
      Serial.println(acesos[i].as<String>());
    }
    Serial.println("Ids:");
    for (int i = 0; i < delays.size(); i++) {
      Serial.println(Ids[i].as<String>());
    }

    for(i = 0; i < repetir;i++){
    piscarLeds();
    delay(500);
    Serial.println("checando pela ultima vez");
    checar();
    }

    free(leds);
    free(ativou);
    http.end();
    delay(1800000);


  }else{
    Serial.println("erro http");
    delay(15000);
    }
}

void piscarLeds(){

  for(j = 0;j < acesos.size()*2;j++){

   if(j < acesos.size()){
   acesoValue = acesos[j].as<int>();
   delayValue = delays[j].as<int>();
   IdValue = Ids[j].as<int>();
   

   if(delayValue >= acesoValue){   
    digitalWrite(IdValue,HIGH);
    delay(acesoValue);
    digitalWrite(IdValue, LOW);
    delay(delayValue); 

   }
   else if(acesoValue > delayValue){
    checarEmTempoReal();

   }
   }
   else if(j > acesos.size()){
    checar();
    delay(delayValue);
  }
}
}

void checarEmTempoReal(){
 
 tempoAtivoSegundos = now();
 ledAtivoMS = tempoAtivoSegundos * 1000;
 
  
  ativou[j] = true;
  digitalWrite(IdValue,HIGH);
  delay(delayValue);
  leds[j] = ledAtivoMS;

  for(k=0;k<=j;k++){


    tempoFinalSegundos = now();
    ledAtivoMSfinal = tempoFinalSegundos * 1000;

     acesoList = acesos[k].as<int>();
     delayList = delays[k].as<int>();
     ledList = Ids[k].as<int>();
     ledAtivoMSList = leds[k];
     ativadoList = ativou[k];

      if(j >=1 && k < j && ledList == IdValue && ativou[j] == true){
        digitalWrite(ledList,LOW);
        ativou[k] = false;
      }
      if ((ledAtivoMSfinal - ledAtivoMSList >= acesoList)) {
       digitalWrite(ledList,LOW);
       ativou[j] = false;
       
      }
    delay(10);
    }

    

}
void checar(){
  tempoFinalSegundos = now();
  ledAtivoMSfinal = tempoFinalSegundos * 1000;

  for(l=0;l<acesos.size();l++){

    acesoList = acesos[l].as<int>();
    delayList = delays[l].as<int>();
    ledList = Ids[l].as<int>();
    ativadoList = ativou[l];
    ledAtivoMSList = leds[l];


    if(ledAtivoMSfinal - ledAtivoMSList >= acesoList) {
    ativou[l] = false;
    digitalWrite(ledList, LOW);
    }
  delay(10);
  }

}

  

      







  
