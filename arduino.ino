#include <WiFi.h>
#include <HTTPClient.h>
#include <string.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <time.h>

//gpios do meu arduino para output led
#define id1 13
#define id2 14
#define id3 27
#define id4 26
#define id5 25
#define id6 33
#define id7 32

#define iniciando 3000

//credenciais wifi
char ssid[] = "ssid";
char password[] = "pass";


//declaração de função
void recursiva(int idValue,int acesoValue,int delayValue);

//api
char server_address[] = "sua api";

//iniciando clientes
HTTPClient http;
WiFiClient wifi;

//variaveis de suporte
int indiceSuporte = 1;
bool orderDelays = false;
int repetir;
JsonArray delays;
JsonArray acesos;
JsonArray Ids;


//setup
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


  delay(iniciando);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  Serial.print(".");

  }


  digitalWrite(id1,LOW);
  digitalWrite(id2,LOW);
  digitalWrite(id3,LOW);
  digitalWrite(id4,LOW);
  digitalWrite(id5,LOW);
  digitalWrite(id6,LOW);
  digitalWrite(id7,LOW);


  //servidor para ligar dois leds ao mesmo tempo
  configTime(0, 0, "pool.ntp.org");

  delay(iniciando);

}

void loop() {

  http.begin(server_address);
  int httpCode = http.GET();
  
  //se a api rest estiver ok
  if(httpCode == HTTP_CODE_OK){

    String response = http.getString();

    //me de o json
    const size_t capacity = JSON_OBJECT_SIZE(4) + 30;
    DynamicJsonDocument doc(capacity);

    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.println("Failed to parse JSON");
      return;
    }
    //OBJETOS DO JSON
    repetir = doc["repetir"];
    delays = doc["delays"];
    acesos = doc["acesos"];
    Ids = doc["ids"];

    
    //prints para checar
    Serial.println("Repetir: " + String(repetir));

    Serial.println("Delays:");
    for (int i = 0; i < delays.size(); i++) {
      Serial.println(delays[i].as<String>());
    }

    Serial.println("Acesos:");
    for (int i = 0; i < acesos.size(); i++) {
      Serial.println(acesos[i].as<String>());
    }

    for (int i = 0; i < delays.size(); i++) {
      Serial.println(Ids[i].as<String>());
    }


    //loop repetições de loop
  for (int i = 0; i < repetir; i++) {
    indiceSuporte = 1;
    
    //loop sequencia de leds enviados pelo json na array
    for (int j = 0; j < acesos.size(); j++) {
        int acesoValue = acesos[j].as<int>();
        int delayValue = delays[j].as<int>();
        int idValue = Ids[j].as<int>();

      //se o delay for maior que o tempo aceso, apenas pisque os leds normalmente
    if(delayValue >= acesoValue){
      digitalWrite(idValue,HIGH);
      delay(acesoValue);
      digitalWrite(idValue, LOW);
      delay(delayValue); 

      //se o tempo aceso for maior que o delay, use o servidor npt para comparar os tempos e se o tempo acabar, desligue o led de forma independente, os delays ficam com a propria ide
      e os tempo aceso ficam por conta de servidor npt.
    }else if(acesoValue > delayValue){

      time_t tempoAtualSegundos = now();
      unsigned long ledAtivoMS = tempoAtualSegundos * 1000;
      int ledAtivo = idValue;
      digitalWrite(ledAtivo,HIGH);
      bool orderDelays = true;
      delay(delayValue);

      while(orderDelays == true){
        time_t tempoFinalSegundos = now();
        unsigned long ledAtivoMSfinal = tempoFinalSegundos * 1000;

        
// o intervalo de tempo do servidor é enviado pela api como o intervalo do led
        if(ledAtivoMSfinal - ledAtivoMS >= acesoValue){
          orderDelays = false;
          digitalWrite(ledAtivo,LOW);
          delay(delayValue);
        }

        //enquanto o led estiver aceso (while), pegue os outros indices usando uma variavel de suporte declarada no inicio indiceSuporte, que incrementa se for menor que a sequencia
        int otherAceso = acesos[indiceSuporte].as<int>();
        int otherDelay = delays[indiceSuporte].as<int>();
        int otherId = Ids[indiceSuporte].as<int>();


        if(indiceSuporte <= acesos.size()){
          indiceSuporte++;
        }
// se esse outro led for maior que o aceso siga normalmente
        if(otherDelay >= otherAceso){
          digitalWrite(otherId,HIGH);
          delay(otherAceso);
          digitalWrite(otherId,LOW);
          delay(otherDelay);
        }
//se nao use o conceito de função recursiva até o o delay ser maior que o tempo aceso
        else if(otherAceso > otherDelay){
          recursiva(otherId,otherAceso,otherDelay);
      
        }
    }
}

  }

}

  }
  else{
    Serial.println("erro http");
  }


  http.end();
  delay(1000000000);

}

void recursiva(int idValue, int acesoValue, int delayValue){

      time_t tempoAtualSegundos = now();
      unsigned long ledAtivoMS = tempoAtualSegundos * 1000;
      int ledAtivo = idValue;
      digitalWrite(ledAtivo,HIGH);
      bool orderDelays = true;
      delay(delayValue);

      while(orderDelays == true){
        time_t tempoFinalSegundos = now();
        unsigned long ledAtivoMSfinal = tempoFinalSegundos * 1000;

        if(ledAtivoMSfinal - ledAtivoMS >= acesoValue){
          orderDelays = false;
          digitalWrite(ledAtivo,LOW);
          delay(delayValue);
        }

        int otherAceso = acesos[indiceSuporte].as<int>();
        int otherDelay = delays[indiceSuporte].as<int>();
        int otherId = Ids[indiceSuporte].as<int>();

        if(indiceSuporte <= acesos.size()){
          indiceSuporte++;
        }

        if(otherDelay > otherAceso){
          digitalWrite(otherId,HIGH);
          delay(otherAceso);
          digitalWrite(otherId,LOW);
          delay(otherDelay);
        }

        else if(otherAceso > otherDelay){
          recursiva(otherId,otherAceso,otherDelay);
      
 
        }
}

}
