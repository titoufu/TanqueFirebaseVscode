#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#define LIGA LOW
#define DESLIGA HIGH
#define MOTOR 8
#define DRENO 9
#define AGUA 10
#define NIVEL_ALTO A3
#define NIVEL_MEDIO A4
#define NIVEL_BAIXO A5
#define BUZZER  6
int menuItem = 1;
int frame = 1;
int lastmenuItem = 1;
int page = 1;
String  menuItem1 = " PROGRAMA ";
String  menuItem2 = " NIVEL    ";
String menuItem3 =  " LIGAR    ";
String menuItem4 =  " LUZ: OFF ";
String menuItem5 =  " CONTRASTE";
String menuItem6 =  " RESET    ";

boolean backlight = true;
int contrast = 50;
int volume = 50;

String Programa[5] = { "50 min.", "40 min.", "30 min." , "10 min." , "ESVAZ."};
String Nivel[4] =    { " ALTO", "MEDIO", "BAIXO"};
String Estado[2] =   { " DESLIGAR ", "  LIGAR   "};
String PRG_RES[5] = {"50", "40", "30", "10" , "ES"};
String NIV_RES[3] = {"AL", "ME", "BA"};
String EST_RES[2] = {"DE", "LI"};
String ESTADO = "  TITO/2018 ";

boolean up = false;
boolean down = false;
boolean middle = false;
boolean buz = true;
ClickEncoder *encoder;
int16_t last, value;

int SelectedPrograma = 2;
int SelectedNivel = 1;
int SelectedEstado = 0;
boolean Fim_Molho = false;
boolean Fim_Bate = false;
int  Time_Molho = 0;
int   Time_Bate = 0;
int  Time_Dreno = 0;
boolean Drenado = false;
boolean P0[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
boolean P1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
boolean P2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
boolean P3[4] = {0, 0, 0, 0};


Adafruit_PCD8544 display = Adafruit_PCD8544( 5, 4, 3); //Download the latest Adafruit Library in order to use this constructor
void(* resetFunc) (void) = 0;//declare reset function at address 0;
void setup()
{
  Serial.begin(9600);
  pinMode(7, OUTPUT);
  pinMode(MOTOR, OUTPUT); digitalWrite(MOTOR, DESLIGA);
  pinMode(DRENO, OUTPUT); digitalWrite(DRENO, DESLIGA);
  pinMode(AGUA, OUTPUT); digitalWrite(AGUA, DESLIGA);
  pinMode(NIVEL_ALTO, INPUT_PULLUP);
  pinMode(NIVEL_MEDIO, INPUT_PULLUP);
  pinMode(NIVEL_BAIXO, INPUT_PULLUP);
  pinMode (BUZZER, OUTPUT);

  turnBacklightOn();

  encoder = new ClickEncoder(A1, A0, A2);
  encoder->setAccelerationEnabled(false);

  display.begin();
  display.clearDisplay();
  display.display();
  setContrast();

  Timer1.initialize(100);
  Timer1.attachInterrupt(timerIsr);
  last = encoder->getValue();
  page = 1;
}
void loop()
{
  drawMenu();
  readRotaryEncoder();
  ClickEncoder::Button b = encoder->getButton();
  if (b != ClickEncoder::Open) {
    switch (b) {
      case ClickEncoder::Clicked:
        middle = true;
        break;
    }
  }

  if (up && page == 1 ) {

    up = false;
    if (menuItem == 2 && frame == 2)
    {
      frame--;
    }

    if (menuItem == 4 && frame == 4)
    {
      frame--;
    }
    if (menuItem == 3 && frame == 3)
    {
      frame--;
    }
    lastmenuItem = menuItem;
    menuItem--;
    if (menuItem == 0)
    {
      menuItem = 1;
    }
  } else if (up && page == 2 && menuItem == 5 ) {
    up = false;
    contrast--;
    setContrast();
  }
  else if (up && page == 2 && menuItem == 2 ) {
    up = false;
    SelectedNivel--;
    if (SelectedNivel == -1)
    {
      SelectedNivel = 2;
    }
  }  else if (up && page == 2 && menuItem == 1 ) {
    up = false;
    SelectedPrograma--;
    if (SelectedPrograma == -1)
    {
      SelectedPrograma = 4;
    }
  }
  if (down && page == 1) //We have turned the Rotary Encoder Clockwise
  {
    down = false;
    if (menuItem == 3 && lastmenuItem == 2)
    {
      frame ++;
    } else  if (menuItem == 4 && lastmenuItem == 3)
    {
      frame ++;
    }
    else  if (menuItem == 5 && lastmenuItem == 4 && frame != 4)
    {
      frame ++;
    }
    lastmenuItem = menuItem;
    menuItem++;
    if (menuItem == 7)
    {
      menuItem--;
    }

  } else if (down && page == 2 && menuItem == 5) {
    down = false;
    contrast++;
    setContrast();
  }
  else if (down && page == 2 && menuItem == 2) {
    down = false;
    SelectedNivel++;
    if (SelectedNivel == 3)
    {
      SelectedNivel = 0;
    }
  }
  else if (down && page == 2 && menuItem == 1 ) {
    down = false;
    SelectedPrograma++;
    if (SelectedPrograma == 5)
    {
      SelectedPrograma = 0;
    }
  }
  if (middle) //Middle Button is Pressed
  {
    middle = false;

    if (page == 1 && menuItem == 4) // Backlight Control
    {
      if (backlight)
      {
        backlight = false;
        menuItem4 = " LUZ: ON" ;
        turnBacklightOff();
      }
      else
      {
        backlight = true;
        menuItem4 = " LUZ: OFF";
        turnBacklightOn();
      }
    }
    else if (page == 1 && menuItem == 6) // Reset
    {
      resetDefaults();
    }
    else if (page == 1) {
      page = 2;
    }
    else if (page == 2)
    {
      page = 1;
    }
  }
  if (page == 1) {
    digitalWrite(MOTOR, DESLIGA);
    digitalWrite(DRENO, DESLIGA);
    digitalWrite(AGUA, DESLIGA);
  }
  ///////////////////////////////////////////////////////
  if (page == 2 && menuItem == 3 )
  {
    SeMexe();
  }
  if (menuItem > 3)frame = 4; else frame = 1;
}

void drawMenu()
{

  if (page == 1)
  {
    // display.fillScreen(0);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(0, 0);
    display.print("MENU PRINCIPAL");
    display.drawFastHLine(0, 10, 83, BLACK);
    display.setCursor(60, 15); display.print("> " + PRG_RES[SelectedPrograma]);
    display.setCursor(60, 25); display.print("> " + NIV_RES[SelectedNivel]);
    if (menuItem == 1 && frame == 1)
    {
      displaymenuItem(menuItem1, 15, true);
      displaymenuItem(menuItem2, 25, false);
      displaymenuItem(menuItem3, 35, false);
    }
    else if (menuItem == 2 && frame == 1)
    {
      displaymenuItem(menuItem1, 15, false);
      displaymenuItem(menuItem2, 25, true);
      displaymenuItem(menuItem3, 35, false);
    }
    else if (menuItem == 3 && frame == 1)
    {
      displaymenuItem(menuItem1, 15, false);
      displaymenuItem(menuItem2, 25, false);
      displaymenuItem(menuItem3, 35, true);
    }
    else if (menuItem == 4 && frame == 2)
    {
      displaymenuItem(menuItem2, 15, false);
      displaymenuItem(menuItem3, 25, false);
      displaymenuItem(menuItem4, 35, true);
    }

    else if (menuItem == 3 && frame == 2)
    {
      displaymenuItem(menuItem2, 15, false);
      displaymenuItem(menuItem3, 25, true);
      displaymenuItem(menuItem4, 35, false);
    }
    else if (menuItem == 2 && frame == 2)
    {
      displaymenuItem(menuItem2, 15, true);
      displaymenuItem(menuItem3, 25, false);
      displaymenuItem(menuItem4, 35, false);
    }

    else if (menuItem == 5 && frame == 3)
    {
      displaymenuItem(menuItem3, 15, false);
      displaymenuItem(menuItem4, 25, false);
      displaymenuItem(menuItem5, 35, true);
    }

    else if (menuItem == 6 && frame == 4)
    {
      displaymenuItem(menuItem4, 15, false);
      displaymenuItem(menuItem5, 25, false);
      displaymenuItem(menuItem6, 35, true);
    }

    else if (menuItem == 5 && frame == 4)
    {
      displaymenuItem(menuItem4, 15, false);
      displaymenuItem(menuItem5, 25, true);
      displaymenuItem(menuItem6, 35, false);
    }
    else if (menuItem == 4 && frame == 4)
    {
      displaymenuItem(menuItem4, 15, true);
      displaymenuItem(menuItem5, 25, false);
      displaymenuItem(menuItem6, 35, false);
    }
    else if (menuItem == 3 && frame == 3)
    {
      displaymenuItem(menuItem3, 15, true);
      displaymenuItem(menuItem4, 25, false);
      displaymenuItem(menuItem5, 35, false);
    }
    else if (menuItem == 2 && frame == 2)
    {
      displaymenuItem(menuItem2, 15, true);
      displaymenuItem(menuItem3, 25, false);
      displaymenuItem(menuItem4, 35, false);
    }
    else if (menuItem == 4 && frame == 3)
    {
      displaymenuItem(menuItem3, 15, false);
      displaymenuItem(menuItem4, 25, true);
      displaymenuItem(menuItem5, 35, false);
    }
    display.display();
  }
  else if (page == 2 && menuItem == 1)
  {
    displayStringMenuPage(menuItem1, Programa[SelectedPrograma]);
  }
  else if (page == 2 && menuItem == 2)
  {
    displayStringMenuPage(menuItem2, Nivel[SelectedNivel]);
  }
  else if (page == 2 && menuItem == 3)
  {
    displayStringEstado( );
  }
  else if (page == 2 && menuItem == 5)
  {
    displayIntMenuPage(menuItem5, contrast);
  }
}

void resetDefaults()
{
  contrast = 50;
  volume = 50;
  SelectedPrograma = 0;
  setContrast();
  backlight = true;
  turnBacklightOn();
}
void setContrast()
{
  display.setContrast(contrast);
  display.display();
}

void turnBacklightOn()
{
  digitalWrite(7, HIGH);
}

void turnBacklightOff()
{
  digitalWrite(7, LOW);
}

void timerIsr() {
  encoder->service();
}

void displayIntMenuPage(String menuItem, int value)
{
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(15, 0);
  display.print(menuItem);
  display.drawFastHLine(0, 10, 83, BLACK);
  display.setCursor(5, 15);
  display.print("Valor:");
  display.setTextSize(2);
  display.setCursor(5, 25);
  display.print(value);
  display.setTextSize(2);
  display.display();
}

void displayStringMenuPage(String menuItem, String value)
{
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(15, 0);
  display.print(menuItem);
  display.drawFastHLine(0, 10, 83, BLACK);
  display.setCursor(1, 15);
  display.print(" ");
  display.setTextSize(2);
  display.setCursor(10, 28);
  display.print(value);
  display.display();
}
void displayStringEstado( )
{
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(0, 0);
  display.print(ESTADO);
  display.setCursor(70, 17); display.print(PRG_RES[SelectedPrograma]);
  display.setCursor(70, 35); display.print(NIV_RES[SelectedNivel]);

  display.drawFastHLine(0, 10, 83, BLACK);  display.drawFastHLine(60, 30, 83, BLACK);
  display.drawFastVLine(60, 10, 50, BLACK);
  display.setCursor(1, 20); display.print("PRESSIONE");
  display.setCursor(1, 30); display.print("   PARA");
  display.setCursor(1, 40); display.print(" RETORNAR");
  display.setTextSize(1);
  display.display();
}
void displayStringEstadoFim( )
{
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  //  display.setCursor(0, 0);
  //  display.print(ESTADO);
  //  display.setCursor(70, 17); display.print(PRG_RES[SelectedPrograma]);
  //  display.setCursor(70, 35); display.print(NIV_RES[SelectedNivel]);

  //  display.drawFastHLine(0, 10, 83, BLACK);  display.drawFastHLine(60, 30, 83, BLACK);
  //  display.drawFastVLine(60, 10, 50, BLACK);
  display.setTextSize(1);
  display.setCursor(1, 05); display.print("   DESLIGUE");
  display.setCursor(1, 20); display.print("     CHAVE ");
  display.setCursor(1, 35); display.print("     VERDE ");
  display.setTextSize(1);
  display.display();
}

void displaymenuItem(String item, int position, boolean selected)
{
  if (selected)
  {
    display.setTextColor(WHITE, BLACK);
  } else
  {
    display.setTextColor(BLACK, WHITE);
  }
  display.setCursor(0, position);
  display.print(item);
}

void readRotaryEncoder()
{
  value += encoder->getValue();

  if (value / 2 > last) {
    last = value / 2;
    down = true;
    delay(150);
  } else   if (value / 2 < last) {
    last = value / 2;
    up = true;
    delay(150);
  }
}
void SeMexe()
{
  boolean terminou = false;
  long int DT = 0;
  switch (SelectedPrograma) {
    case 0:                //  Muita Suja. (5342-3322-2422) => 50 miutos.
      if (!P0[0])  P0[0] = Bater(4);
      else if (!P0[1])  P0[1] = Molho(5);
      else if (!P0[2])  P0[2] = Bater(3);
      else if (!P0[3])  P0[3] = Molho(4);
      else if (!P0[4])  P0[4] = Bater(2);
      else if (!P0[5])  P0[5] = Molho(4);
      else if (!P0[6])  P0[6] = Bater(2);
      else if (!P0[7])  P0[7] = Drenar();
      else if (!P0[8])  P0[8] = Bater(3);
      else if (!P0[9])  P0[9] = Molho(4);
      else if (!P0[10]) P0[10] = Bater(2);
      else if (!P0[11]) P0[11] = Drenar();
      else if (!P0[12]) P0[12] = Bater(2);
      else if (!P0[13]) P0[13] = Molho(4);
      else if (!P0[14]) P0[14] = Bater(2);
      else if (!P0[15]) P0[15] = Drenar();
      else if (P0[15]) {

        ESTADO = "     FIM  "; terminou = true;
        displayStringEstado();
      }
      break;
    case 1:                //  Suja. (4542-3422)  => 40 miutos.
      if (!P1[0]) P1[0] = Bater(4);
      else if (!P1[1])  P1[1] = Molho(6);
      else if (!P1[2])  P1[2] = Bater(3);
      else if (!P1[3])  P1[3] = Drenar();
      else if (!P1[4])  P1[4] = Bater(3);
      else if (!P1[5])  P1[5] = Molho(5);
      else if (!P1[6])  P1[6] = Bater(2);
      else if (!P1[7])  P1[7] = Drenar();
      else if (P1[7]) {
        ESTADO = "     FIM  "; terminou = true;
        displayStringEstado();
      }
      break;

    case 2:                // Normal.(3532)   => 30 miutos.
      if (!P2[0]) P2[0] = Bater(2);
      else if (!P2[1])  P2[1] = Molho(4);
      else if (!P2[2])  P2[2] = Bater(2);
      else if (!P2[3])  P2[3] = Drenar();
      else if (!P2[4])  P2[4] = Bater(2);
      else if (!P2[5])  P2[5] = Molho(3);
      else if (!P2[6])  P2[6] = Bater(1);
      else if (!P2[7])  P2[7] = Drenar();
      else if (P2[7]) {
        ESTADO = "     FIM  "; terminou = true;
        displayStringEstado();
      }
      break;
    case 3:                // Delicada. (2322)    => 10 miutos.
      if (!P3[0]) P3[0] = Bater(2);
      else if (!P3[1])  P3[1] = Molho(2);
      else if (!P3[2])  P3[2] = Bater(1);
      else if (!P3[3])  P3[3] = Drenar();
      else if (P3[3]) {
        ESTADO = "     FIM  "; terminou = true;
        displayStringEstado();
      }
      break;
    case 4:                // Drenar.
      if (!Drenado) Drenado = Drenar();
      else {
        ESTADO = "     FIM  "; terminou = true;
        displayStringEstado();
      }
      break;
  }
  if (terminou && buz == true) {
    displayStringEstadoFim( );
    DT = millis() / 1000 + 3600 * 10;
    while (millis() / 1000 < DT) {
      buzina ();
    }
    display.clearDisplay();
    display.display();
    turnBacklightOn();
    buz = false;
    noTone(BUZZER);
  }
}
boolean Encher( )
{
  boolean vazio = false, flag = false;
  
  digitalWrite(DRENO, DESLIGA); digitalWrite(MOTOR, DESLIGA);
  
  if (SelectedNivel == 0) vazio = digitalRead(NIVEL_ALTO);
  else if (SelectedNivel == 1) vazio = digitalRead(NIVEL_MEDIO);
  else if (SelectedNivel == 2) vazio = digitalRead(NIVEL_BAIXO);
  
  switch (SelectedNivel)
  {
    case 0:
      if (digitalRead(NIVEL_ALTO)) {
        ESTADO = "ENCHENDO ... ";  displayStringEstado(); digitalWrite(AGUA, LIGA);
        vazio = true;
      }
      break;
    case 1:
      if (digitalRead(NIVEL_MEDIO)) {
        ESTADO = "ENCHENDO ...";  displayStringEstado(); digitalWrite(AGUA, LIGA);
        vazio = true;
      }
      break;
    case 2:
      if (digitalRead(NIVEL_BAIXO)) {
        ESTADO = "ENCHENDO ...";  displayStringEstado(); digitalWrite(AGUA, LIGA);
        vazio = true;
      }
      break;
  }
  if (!vazio)
  {
    digitalWrite(DRENO, DESLIGA); digitalWrite(MOTOR, DESLIGA); digitalWrite(AGUA, DESLIGA);
    ESTADO = "   NIVEL OK";  displayStringEstado(); delay(2000);
    flag = true;
  }
  return flag;
}
boolean Drenar()
{
  boolean flag = false,  vazio = true;
  int DT = 20;                              // tempo que fica ligado depois de atimgir o nível zero.
  digitalWrite(AGUA, DESLIGA); digitalWrite(MOTOR, DESLIGA);
  if (digitalRead(NIVEL_BAIXO) == 0 )
  {
    digitalWrite(DRENO, LIGA);// liga o dreno
    ESTADO = "DRENANDO!";   displayStringEstado();
    Time_Dreno = millis() / 1000 + DT;
    vazio = false;
  }
  if (digitalRead(NIVEL_BAIXO) == 1 && millis() / 1000 < Time_Dreno)
  {
    digitalWrite(DRENO, LIGA);// liga o dreno
    ESTADO = "DRENO:" + String((int)(Time_Dreno - millis() / 1000.)) + "[S]";
    displayStringEstado();
    vazio = false;
  }
  if (vazio)
  {
    Time_Dreno = millis() / 1000 + DT;
    while (millis() / 1000  < Time_Dreno )digitalWrite(DRENO, LIGA);  //
    ESTADO = "   VAZIO !!";
    digitalWrite(DRENO, DESLIGA); digitalWrite(MOTOR, DESLIGA); digitalWrite(AGUA, DESLIGA);
    displayStringEstado(); delay(2000);
    flag = true;
  }
  return flag;
}
boolean Molho(int DT)
{
  boolean flag = false, vazio;
  if (SelectedNivel == 0) vazio = digitalRead(NIVEL_ALTO);
  else if (SelectedNivel == 1) vazio = digitalRead(NIVEL_MEDIO);
  else if (SelectedNivel == 2) vazio = digitalRead(NIVEL_BAIXO);
  if (vazio) {
    Encher(); Time_Molho = 0; Fim_Molho = false;
  }
  else
  {
    digitalWrite(AGUA, DESLIGA);
    if (Time_Molho == 0)Time_Molho = millis() / 1000 + DT * 60;
  }
  if (millis() / 1000 < Time_Molho)
  {
    ESTADO = "MOLHO:" + String((int)(Time_Molho - millis() / 1000.)) + "[S]";
    displayStringEstado();
    Fim_Molho = true;
  }
  else if (Fim_Molho)
  {
    ESTADO = "FIM - MOLHO";
    digitalWrite(DRENO, DESLIGA); digitalWrite(MOTOR, DESLIGA); digitalWrite(AGUA, DESLIGA);
    displayStringEstado(); delay(2000);
    Time_Molho = 0  ;
    flag = true;
  }
  return flag;
}

boolean Bater(int DT)
{
  boolean flag = false, vazio = true;
  if (SelectedNivel == 0) vazio = digitalRead(NIVEL_ALTO);
  else if (SelectedNivel == 1) vazio = digitalRead(NIVEL_MEDIO);
  else if (SelectedNivel == 2) vazio = digitalRead(NIVEL_BAIXO);
  if (vazio) {
    Encher(); Time_Bate = 0; Fim_Bate = false;
  }
  else
  {
    digitalWrite(AGUA, DESLIGA);
    if (Time_Bate == 0)Time_Bate = millis() / 1000 + DT * 60;
  }
  if (millis() / 1000 < Time_Bate )
  {
    ESTADO = "BATENDO:" + String((int)(Time_Bate - millis() / 1000.)) + "[S]";
    displayStringEstado();
    digitalWrite(MOTOR, LIGA);
    Fim_Bate = true;
  }
  else if (Fim_Bate)
  {
    ESTADO = "FIM - BATER";
    digitalWrite(DRENO, DESLIGA); digitalWrite(MOTOR, DESLIGA); digitalWrite(AGUA, DESLIGA);
    displayStringEstado(); delay(2000);
    Time_Bate = 0  ;
    flag = true;
  }
  return flag;
}
void buzina () {
  tone(BUZZER, 200); turnBacklightOn();
  delay(200);
  tone(BUZZER, 10); turnBacklightOff();
  delay(200);
}
