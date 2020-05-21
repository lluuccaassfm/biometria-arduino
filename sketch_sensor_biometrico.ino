#include <LiquidCrystal.h>        //biblioteca do lcd
#include <Adafruit_Fingerprint.h> //biblioteca do sensor
#include <Servo.h>                //biblioteca do servo
#include <Thread.h>               //biblioteca de Thread


LiquidCrystal lcd(4,5,10,11,12,13); //variavel do lcd/portas de dados do lcd
SoftwareSerial sensor(2, 3);        //variavel do sensor/portas de dados do sensor
Servo servo;                        //variavel do servor
int botao = 7;
int botao_pressionado = 0;
boolean admin = false;
int pessoas =1; //numero de pessoas/digitais. Numero 0 reservado para o adminitrador. incremental
int pinLedGreen = 9;
int pinLedRed = 8;

uint8_t modoGravacaoID(uint8_t IDgravar); //declara a funcao de modogravacao

Thread Thread1 = Thread(); //Declaracao da Thread1 para executar verificacao de usuario e adiciona nova digital 
Thread ThreadVerificaPessoa = Thread();

Adafruit_Fingerprint dedo = Adafruit_Fingerprint(&sensor);
void setup() {
  pinMode(botao,INPUT);
  lcd.begin(16,2);    //o lcd tem duas filas e 16 colunas
  servo.attach(6);  //porta de dados do sservo motor
  pinMode(pinLedGreen, OUTPUT); //led verde
  pinMode(pinLedRed, OUTPUT); //led vermelho
  
  Serial.begin(9600);
  dedo.begin(57600);

  if(dedo.verifyPassword()){
    apresentaMensagem("Sensor biometrico encontrado!");
    Serial.println("Sensor biometrico encontrado!");
  } else {
    apresentaMensagem("Sensor biometrico não encontrado! Verifique a conexão e reinicie o sistema.");
    Serial.println("Sensor biometrico não encontrado! Verifique a conexão e reinicie o sistema");
    while(true) { 
      delay(1); 
      }
  }
  
  Thread1.onRun(threadDigital); 
  Thread1.setInterval(100); //Executa o processo threadDigital no intervalo de 1s

  ThreadVerificaPessoa.onRun(verificaPessoa);
  ThreadVerificaPessoa.setInterval(50);
}

void loop() {

  if(Thread1.shouldRun()) Thread1.run(); //Verifica se o Thread1 deve ser executado;
  if(ThreadVerificaPessoa.shouldRun()) ThreadVerificaPessoa.run(); 
}


void threadDigital(){ //Metodo sendo executado na Thread1

  //verificaPessoa(); //Método que verifica se a digital esta na memoria e se pertence a um administrador
  delay(50);
  botao_pressionado = digitalRead(botao);
  if(botao_pressionado == HIGH){                        //verifica se o botao foi pressionado
    apresentaMensagem("Aguardando id do Administrador!");
    Serial.println("Aguardando id do Administrador!"); 
    botao_pressionado = LOW;                            //delay para nao ficar repetindo a mensagem
    delay(2000);
    modoGravacaoID(pessoas);
  } else {
    
  }

}

void apresentaMensagem(String mensagem){
  lcd.blink();
  lcd.clear();
  lcd.setCursor(0, 0);
  char charBuf[50];
  mensagem.toCharArray(charBuf, 50);
  int x =0;
  
  for (int thisChar = 0; thisChar < mensagem.length(); thisChar++) {
    lcd.print(charBuf[thisChar]);
    if(x == 15){
      lcd.autoscroll();
    }
   delay(250);
   x++;
    if(thisChar == mensagem.length()-1){
     lcd.noAutoscroll();
     delay(500);
     lcd.clear();
    }
  }
}

void mensagemAguardandoDigital(){
  lcd.blink();
  lcd.setCursor(0, 0);
  lcd.print("Aguardando...");
  //delay(500);
  //lcd.clear();
}

int verificaPessoa() {
  uint8_t p = dedo.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = dedo.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = dedo.fingerFastSearch();
  if (p != FINGERPRINT_OK){
    return digitalFailed();
  }  

  if(dedo.fingerID == 0){  // Comparando digital colocada com a digital da memoria, A digital 0 resenvada para o administrador
    apresentaMensagem("Ola, Administrador.");
    Serial.println("Ola Administrador");
    admin = true; //Esta digital percente ao administrador
  }

   return digitalSucess();  
}

int digitalFailed(){
  apresentaMensagem("Digital nao encontrada!");
  Serial.print("Digital não encontrada!");
  digitalWrite(pinLedRed, HIGH);
  delay(1000);
  digitalWrite(pinLedRed, LOW);
  return -1;
}

int digitalSucess(){
  servo.write(0);
  digitalWrite(pinLedGreen, HIGH);
  delay(4000);
  servo.write(180);
  digitalWrite(pinLedGreen, LOW);
  apresentaMensagem("Ola, ID " + dedo.fingerID);
  Serial.print("Found ID #"); Serial.print(dedo.fingerID); 
  Serial.print(" with confidence of "); Serial.println(dedo.confidence);

  return dedo.fingerID; 
}

int analisaDedo(int p, int numDedo){
  p = dedo.image2Tz(numDedo);
  switch (p) {
    case FINGERPRINT_OK:
      apresentaMensagem("Leitura convertida");
      Serial.println("Leitura convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      apresentaMensagem("Leitura suja");
      Serial.println("Leitura suja");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      apresentaMensagem("Erro de comunicacao");
      Serial.println("Erro de comunicação");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      apresentaMensagem("Não foi possivel encontrar propriedade da digital");
      Serial.println("Não foi possível encontrar propriedade da digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      apresentaMensagem("Não foi possível encontrar propriedade da digital");
      Serial.println("Não foi possível encontrar propriedade da digital");
      return p;
    default:
      apresentaMensagem("Erro desconhecido");
      Serial.println("Erro desconhecido");
      return p;
  }
}

uint8_t modoGravacaoID(uint8_t IDgravar) {

  int p = -1;
  apresentaMensagem("Esperando uma leitura valida para gravar...");
  Serial.print("Esperando uma leitura válida para gravar #"); Serial.println(IDgravar);
  delay(2000);
  while (p != FINGERPRINT_OK) {
    p = dedo.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      apresentaMensagem("Leitura concluida!");
      Serial.println("Leitura concluída");
      break;
    case FINGERPRINT_NOFINGER:
      mensagemAguardandoDigital();
      Serial.println(".");
      delay(200);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      apresentaMensagem("Erro comunicacao");
      Serial.println("Erro comunicação");
      break;
    case FINGERPRINT_IMAGEFAIL:
      apresentaMensagem("Erro de leitura");
      Serial.println("Erro de leitura");
      break;
    default:
      apresentaMensagem("Erro desconhecido");
      Serial.println("Erro desconhecido");
      break;
    }
  }

  // OK successo!
   analisaDedo(p, 1);
  
  apresentaMensagem("Remova o dedo.");
  Serial.println("Remova o dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = dedo.getImage();
  }
  Serial.print("ID "); Serial.println(IDgravar);
  p = -1;
  apresentaMensagem("Coloque o mesmo dedo novamente.");
  Serial.println("Coloque o Mesmo dedo novamente");
  while (p != FINGERPRINT_OK) {
    p = dedo.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      apresentaMensagem("Leitura concluida");
      Serial.println("Leitura concluída");
      break;
    case FINGERPRINT_NOFINGER:
      mensagemAguardandoDigital();
      Serial.print(".");
      delay(200);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      apresentaMensagem("Erro de comunicacao");
      Serial.println("Erro de comunicação");
      break;
    case FINGERPRINT_IMAGEFAIL:
      apresentaMensagem("Erro de leitura");
      Serial.println("Erro de Leitura");
      break;
    default:
      apresentaMensagem("Erro desconhecido");
      Serial.println("Erro desconhecido");
      break;
    }
  }

  // OK successo!
  analisaDedo(p, 2);
  
  // OK convertido!
  Serial.print("Criando modelo para #");  Serial.println(IDgravar);
  
  p = dedo.createModel();
  if (p == FINGERPRINT_OK) {
    apresentaMensagem("As digitais batem!");
    Serial.println("As digitais batem!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    apresentaMensagem("Erro de comunicacao");
    Serial.println("Erro de comunicação");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    apresentaMensagem("As digitais nao batem");
    Serial.println("As digitais não batem");
    return p;
  } else {
    apresentaMensagem("Erro desconhecido");
    Serial.println("As digitais não batem");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(IDgravar);
  p = dedo.storeModel(IDgravar);
  if (p == FINGERPRINT_OK) {
    apresentaMensagem("Armazenado!");
    Serial.println("Armazenado!");
    pessoas++; //adiciona mais 1 no contador para armazenar proxima pessoa/digital
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    apresentaMensagem("Erro de comunicacao");
    Serial.println("Erro de comunicação");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    apresentaMensagem("Nao foi possivel gravar neste local da memoria");
    Serial.println("Não foi possível gravar neste local da memória");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    apresentaMensagem("Erro durante escrita na memoria flash");
    Serial.println("Erro durante escrita na memória flash");
    return p;
  } else {
    apresentaMensagem("Erro desconhecido");
    Serial.println("Erro desconhecido");
    return p;
  }   
}