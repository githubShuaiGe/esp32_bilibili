#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

//管脚数据
int  control_temp_pin = 25;   
//wifi数据
const char* ssid     = "401";
const char* password = "46017001531";           //api.bilibili.com/x/web-interface/card?mid=429222273
const char* host = "api.bilibili.com";   


void setup() {
    Serial.begin(115200);
    delay(10);
    //管脚初始化
    pinMode(control_temp_pin, OUTPUT);  //上拉输入  
    //wifi初始化
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(2500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
  //获取http的josn数据
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {                   //第一步
    Serial.println("connection failed");
    return;
  }  
  
  /* We now create a URI for the request */
  String url = "/x/web-interface/card?mid=429222273";  
  
  /* This will send the request to the server */
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +            //第二步
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(1000);  

  /* Read all the lines of the reply from server and print them to Serial */
  String answer;
  while(client.available()){
    String line = client.readStringUntil('\r');                   //第三步
    answer += line;
  }
  
  /* Disconnect to the server */
  client.stop();                                                 //stop


  //对josn数据进行处理
  String jsonAnswer;
  int jsonIndex;
  for (int i = 0; i < answer.length(); i++) {
    if (answer[i] == '{') {
      jsonIndex = i;
      break;
    }
  }
  jsonAnswer = answer.substring(jsonIndex);
  Serial.println();
  Serial.println("JSON answer: ");
  Serial.println(jsonAnswer);

DynamicJsonDocument doc(3072);

DeserializationError error = deserializeJson(doc, jsonAnswer);
//DeserializationError error = deserializeJson(doc, json);

if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return;
}

int code = doc["code"]; // 0
const char* message = doc["message"]; // "0"
int ttl = doc["ttl"]; // 1

JsonObject data = doc["data"];

JsonObject data_card = data["card"];
int data_follower = data["follower"]; // 2

  Serial.print("bibi粉丝数:");
  Serial.println(data_follower);
  if(data_follower == 2)
  {
    digitalWrite(control_temp_pin,HIGH);
  }else{
    digitalWrite(control_temp_pin,LOW);
    }

  delay(100);
}
