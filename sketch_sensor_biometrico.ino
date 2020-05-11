#include <LiquidCrystal.h>        //biblioteca do lcd
#include <Adafruit_Fingerprint.h> //biblioteca do sensor
#include <Servo.h>                //biblioteca do servo

LiquidCrystal lcd(4,5,10,11,12,13); //variavel do lcd/portas de dados do lcd
SoftwareSerial sensor(2, 3);        //variavel do sensor/portas de dados do sensor
Servo servo;                        //variavel do servor
int botao = 7;
int botao_pressionado = 0;

uint8_t modoGravacaoID(uint8_t IDgravar); //declara a funcao de modogravacao

Adafruit_Fingerprint dedo = Adafruit_Fingerprint(&sensor);
void setup() {
  pinMode(botao,INPUT);
  lcd.begin(16,2);    //o lcd tem duas filas e 16 colunas
  servo.attach(8);  //porta de dados do servo motor
  
  Serial.begin(9600);
  dedo.begin(57600);

  if(dedo.verifyPassword()){
    Serial.println("Sensor biometrico encontrado!");
  } else {
    Serial.println("Sensor biometrico não encontrado! Verifique a conexão e reinicie o sistema");
    while(true) { 
      delay(1); 
      }
  }
}

void loop() {
  botao_pressionado = digitalRead(botao);
  if(botao_pressionado == HIGH){                        //verifica se o botao foi pressionado
    Serial.println("Aguardando id do Administrador!"); 
    botao_pressionado = LOW;                            //delay para nao ficar repetindo a mensagem
    delay(2000);
    modoGravacaoID(1);
  } else {
    
  }
}

uint8_t modoGravacaoID(uint8_t IDgravar) {

  int p = -1;
  Serial.print("Esperando uma leitura válida para gravar #"); Serial.println(IDgravar);
  delay(2000);
  while (p != FINGERPRINT_OK) {
    p = dedo.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura concluída");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      delay(200);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro comunicação");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Erro de leitura");
      break;
    default:
      Serial.println("Erro desconhecido");
      break;
    }
  }

  // OK successo!

  p = dedo.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Leitura suja");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Não foi possível encontrar propriedade da digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Não foi possível encontrar propriedade da digital");
      return p;
    default:
      Serial.println("Erro desconhecido");
      return p;
  }
  
  Serial.println("Remova o dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = dedo.getImage();
  }
  Serial.print("ID "); Serial.println(IDgravar);
  p = -1;
  Serial.println("Coloque o Mesmo dedo novamente");
  while (p != FINGERPRINT_OK) {
    p = dedo.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura concluída");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      delay(200);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Erro de Leitura");
      break;
    default:
      Serial.println("Erro desconhecido");
      break;
    }
  }

  // OK successo!

  p = dedo.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Leitura suja");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Não foi possível encontrar as propriedades da digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Não foi possível encontrar as propriedades da digital");
      return p;
    default:
      Serial.println("Erro desconhecido");
      return p;
  }
  
  // OK convertido!
  Serial.print("Criando modelo para #");  Serial.println(IDgravar);
  
  p = dedo.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("As digitais batem!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erro de comunicação");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("As digitais não batem");
    return p;
  } else {
    Serial.println("Erro desconhecido");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(IDgravar);
  p = dedo.storeModel(IDgravar);
  if (p == FINGERPRINT_OK) {
    Serial.println("Armazenado!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erro de comunicação");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Não foi possível gravar neste local da memória");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Erro durante escrita na memória flash");
    return p;
  } else {
    Serial.println("Erro desconhecido");
    return p;
  }   
}
