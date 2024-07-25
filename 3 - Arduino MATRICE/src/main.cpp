#include <Arduino.h>
#include <String.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include "Font_Data.h"
#include <LiquidCrystal_I2C.h>

//Com RS485
#define Pin_Rs485_1 5
#define Pin_Rs485_2 6

//Matrice de led
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

//Matrice de led
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define  MAX_DEVICES 16
#define MAX_ZONES 2
#define ZONE_SIZE (MAX_DEVICES/MAX_ZONES)
MD_Parola Matrix = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);


//ZONE AFFICHAGE
/*         
           --------------------
          |       ZONE 0       |
          |       ZONE 1       |
           --------------------

           --------------------
          |  ZONE 5    ZONE 4  |
          |  ZONE 3    ZONE 2  |
           --------------------
*/

// LCD Ecran de TEST
LiquidCrystal_I2C lcd(0x27,16,2);

//INIT DE MES PARAMETRES DE VARIABLES--------------------------------------------

String message;
String data;
String previous_data;
String sub;
String sub_check;
bool error_get_data;

int ID_Arduino_Matrix = 2;

unsigned long currentTime = 0;
unsigned long previousTime = 0;
unsigned long previousTime_2 = 0;
long Delay_scroll;

// exemple de trame recu : 2$1$0$Lux$Info$Maes:$Orval:$102$64$1$1$0$1$20$Bienvenue au cercle$0$

//INIT DE MES TABLEAUX DE STRUCTURES---------------------------------------------

struct data_R_matrix
{
  int ID_RS485 = 2;
  bool balance_ena_1;
  bool balance_ena_2;
  String Name_1 ;
  String Name_2 ;
  String Name_beer1;
  String Name_beer2 ;
  int Nb_beer_1;
  int Nb_beer_2;
  bool Option_Name_1;
  bool Option_Name_2;
  bool Text_const;
  bool Scroll;
  int Freq_text;
  String Text_to_print;
  bool Offline;
};
typedef struct data_R_matrix Data_R_matrix;

// INIT DE MES STRUCTURE----------------------------------------------------------

Data_R_matrix data_R_matrix;

// MES FONCTIONS------------------------------------------------------------------

bool get_data()
{
  
  if (Serial.available() > 0)
  {
    //delay(10);
    //message = Serial.readStringUntil('\n');
    message = Serial.readString(); //bonne fonction ne pas changer !!!
    data = message;
    //lcd.setCursor(0,1);  
    //lcd.print(message);
    //delay(10);
    //digitalWrite(Pin_Rs485, HIGH);
    //Serial.print("ArduinoReceived :");
    //Serial.println(message);
    //delay(10);
    //digitalWrite(Pin_Rs485, LOW);
    lcd.print(data);
    return 1;
  }
  else
  {
    return 0;  
  }  
}

bool parse_data()
{
  
  int startPos = 0;
  int foundPos = -1;
  int i = 0;
  error_get_data = 0;

  sub_check = data.substring(0,1);

  
    while ((foundPos = data.indexOf("$", startPos)) != -1)
  {
    sub = data.substring(startPos, foundPos);
    

    if (startPos == 0 && (sub.toInt() != ID_Arduino_Matrix))
    {
      break;
    }

    switch (i)
    {
    case 0:
      data_R_matrix.ID_RS485 = sub.toInt();
      break;
    case 1:
      if (sub == String("0"))
      {
        data_R_matrix.balance_ena_1 = 0;
      }
      else
      {
        data_R_matrix.balance_ena_1 = 1;
      }
      break;

    case 2:
      if (sub == String("0"))
      {
        data_R_matrix.balance_ena_2 = 0;
      }
      else
      {
        data_R_matrix.balance_ena_2 = 1;
      }
      break;

    case 3:
      data_R_matrix.Name_1 = sub;
      break;

    case 4:
      data_R_matrix.Name_2 = sub;
      break;

    case 5:
      data_R_matrix.Name_beer1 = sub;
      break;

    case 6:
      data_R_matrix.Name_beer2 = sub;
      break;

    case 7:
      data_R_matrix.Nb_beer_1 = sub.toInt();
      break;

    case 8:
      data_R_matrix.Nb_beer_2 = sub.toInt();
      break;

    case 9:
        if (sub == String("0"))
        {
          data_R_matrix.Option_Name_1 = 0;
        }
        else
        {
          data_R_matrix.Option_Name_1 = 1;
        }
        break;

    case 10:
        if (sub == String("0"))
        {
          data_R_matrix.Option_Name_2 = 0;
        }
        else
        {
          data_R_matrix.Option_Name_2 = 1;
        }
        break;

    case 11:
      if (sub == String("0"))
      {
        data_R_matrix.Text_const = 0;
      }
      else
      {
        data_R_matrix.Text_const = 1;
      }
      break;

    case 12:
      if (sub == String("0"))
      {
        data_R_matrix.Scroll = 0;
      }
      else
      {
        data_R_matrix.Scroll = 1;
      }
      break;

    case 13:
      data_R_matrix.Freq_text = sub.toInt();
      break;

    case 14:
        data_R_matrix.Text_to_print = sub;
        break;

    case 15:
      if (sub == String("0"))
      {
        data_R_matrix.Offline = 0;
      }
      else
      {
        data_R_matrix.Offline = 1;
      }
      break;

    default:
      error_get_data = 1;
    }

    //Serial.println(" Nouvelle donnée");
    previous_data = data;
    i++;
    startPos = foundPos + 1;
  }
  
  
  return error_get_data;
}

void print_2balance()
{
  const char *Name1 = data_R_matrix.Name_1.c_str();
  const char *Name_beer1 = data_R_matrix.Name_beer1.c_str();
  String Nb_beer_str1 = String(data_R_matrix.Nb_beer_1);
  const char *Nb_beer1 = Nb_beer_str1.c_str();

  const char *Name2 = data_R_matrix.Name_2.c_str();
  const char *Name_beer2 = data_R_matrix.Name_beer2.c_str();
  String Nb_beer_str2 = String(data_R_matrix.Nb_beer_2);
  const char *Nb_beer2 = Nb_beer_str2.c_str();

  Matrix.setFont(0, NULL);
  Matrix.setFont(1, NULL);
  Matrix.setFont(2, NULL);
  Matrix.setFont(3, NULL);
  Matrix.setFont(4, NULL);
  Matrix.setFont(5, NULL);

  if (data_R_matrix.Option_Name_1 == 1)
  {
    Matrix.displayZoneText(5, Name1 , PA_CENTER, 30, 0 , PA_PRINT);
    Matrix.displayZoneText(3, Nb_beer1 , PA_CENTER, 30, 0, PA_PRINT);
  }
  else
  {
    Matrix.displayZoneText(5, Name_beer1 , PA_CENTER, 30, 0, PA_PRINT);
    Matrix.displayZoneText(3, Nb_beer1 , PA_CENTER, 30, 0, PA_PRINT);
  }

  if (data_R_matrix.Option_Name_2 == 1)
  {
    Matrix.displayZoneText(4, Name2 , PA_CENTER, 30, 0 , PA_PRINT);
    Matrix.displayZoneText(2, Nb_beer2 , PA_CENTER, 30, 0, PA_PRINT);
  }
  else
  {
    Matrix.displayZoneText(4, Name_beer2 , PA_CENTER, 30, 0, PA_PRINT);
    Matrix.displayZoneText(2, Nb_beer2 , PA_CENTER, 30, 0, PA_PRINT);
  }
  
  Matrix.synchZoneStart();
  while (!Matrix.getZoneStatus(2) || !Matrix.getZoneStatus(3))
  Matrix.displayAnimate();

}

void print_balance1()
{
  const char *Name = data_R_matrix.Name_1.c_str();
  const char *Name_beer = data_R_matrix.Name_beer1.c_str();
  String Nb_beer_str = String(data_R_matrix.Nb_beer_1);
  const char *Nb_beer = Nb_beer_str.c_str();

  /*Serial.print(data_R_matrix.Nb_beer_1);
  Serial.print("  ");
  Serial.print(String(data_R_matrix.Nb_beer_1));
  Serial.print("  ");
  Serial.println(Nb_beer);*/

  Matrix.setFont(0, NULL);
  Matrix.setFont(1, NULL);
  Matrix.setFont(2, NULL);
  Matrix.setFont(3, NULL);
  Matrix.setFont(4, NULL);
  Matrix.setFont(5, NULL);
  Matrix.displayZoneText(1, Name , PA_CENTER, 30, 0 , PA_PRINT);
  Matrix.displayZoneText(3, Name_beer , PA_CENTER, 30, 0, PA_PRINT);
  Matrix.displayZoneText(2, Nb_beer , PA_CENTER, 30, 0, PA_PRINT);
  Matrix.synchZoneStart();
  while (!Matrix.getZoneStatus(2) || !Matrix.getZoneStatus(3))
  Matrix.displayAnimate();
}

void print_balance2()
{
  const char *Name = data_R_matrix.Name_2.c_str();
  const char *Name_beer = data_R_matrix.Name_beer2.c_str();
  String Nb_beer_str = String(data_R_matrix.Nb_beer_2);
  const char *Nb_beer = Nb_beer_str.c_str();

  Matrix.setFont(0, NULL);
  Matrix.setFont(1, NULL);
  Matrix.setFont(2, NULL);
  Matrix.setFont(3, NULL);
  Matrix.setFont(4, NULL);
  Matrix.setFont(5, NULL);
  Matrix.displayZoneText(1, Name , PA_CENTER, 30, 0, PA_PRINT);
  Matrix.displayZoneText(3, Name_beer , PA_CENTER, 30, 0, PA_PRINT);
  Matrix.displayZoneText(2, Nb_beer , PA_CENTER, 30, 0, PA_PRINT);
  Matrix.synchZoneStart();
  while (!Matrix.getZoneStatus(2) || !Matrix.getZoneStatus(3))
  Matrix.displayAnimate();
}

void print_text()
{
  const char *Text = data_R_matrix.Text_to_print.c_str();
  if (data_R_matrix.Scroll == 1)
  {
    Matrix.setFont(0, BigFontBottom);
    Matrix.setFont(1, BigFontUp);
    Matrix.displayZoneText(0, Text, PA_CENTER, 30, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    Matrix.displayZoneText(1, Text, PA_CENTER, 30, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    Matrix.synchZoneStart();
    while (!Matrix.getZoneStatus(0) || !Matrix.getZoneStatus(1))
    Matrix.displayAnimate();
  }
  else
  {
    Matrix.setFont(0, BigFontBottom);
    Matrix.setFont(1, BigFontUp);
    Matrix.displayZoneText(0, Text, PA_CENTER, 30, 10, PA_PRINT);
    Matrix.displayZoneText(1, Text, PA_CENTER, 30, 10, PA_PRINT);
    Matrix.synchZoneStart();
    while (!Matrix.getZoneStatus(0) || !Matrix.getZoneStatus(1))
    Matrix.displayAnimate();
  }
}

void send_data()
{
  digitalWrite(Pin_Rs485_2, HIGH);
  digitalWrite(Pin_Rs485_1, HIGH);
  Serial.print("Get data: ");
  delay(10);
  digitalWrite(Pin_Rs485_2, LOW);
  digitalWrite(Pin_Rs485_1, LOW);
}


// SETUP ---------------------------------------------------------------------

void setup() {

  // Initialise la communication série à 9600 bauds
  Serial.begin(115200);
  Serial.println("Matrice Starting...");
  Serial.setTimeout(40);

  // Initialisation com RS485
  pinMode(Pin_Rs485_2, OUTPUT);
  digitalWrite(Pin_Rs485_2, LOW);
  pinMode(Pin_Rs485_1, OUTPUT);
  digitalWrite(Pin_Rs485_1, LOW);

  // Initialise la matrice Led
  Matrix.begin(6);
  Matrix.setInvert(false);
  Matrix.displayClear();
  Matrix.setZone(0, 0, ZONE_SIZE-1);
  Matrix.setZone(1, ZONE_SIZE, MAX_DEVICES-1);
  Matrix.setZone(2, 0, 3);
  Matrix.setZone(3, 4, 7);
  Matrix.setZone(4, 8, 11);
  Matrix.setZone(5, 12, 15);
  Matrix.setIntensity(1);

  Matrix.setFont(0, BigFontBottom);
  Matrix.setFont(1, BigFontUp);
  Matrix.displayZoneText(0, "Bienvenue au cercle info", PA_CENTER, 30, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  Matrix.displayZoneText(1, "Bienvenue au cercle info", PA_CENTER, 30, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  Matrix.synchZoneStart();
  while (!Matrix.getZoneStatus(0) || !Matrix.getZoneStatus(1))
  Matrix.displayAnimate();

  // Initialisation du LCD pour les tests
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0,0);  
  lcd.print("Balance Info");

}


// LOOP ----------------------------------------------------------------------


void loop() {

currentTime = millis();


/*Serial.print(get_data());
Serial.print("  Parse data: ");
Serial.println(parse_data());*/

get_data();
parse_data();


if (data_R_matrix.Text_const == 1)
{
  print_text();
}
else
{
  if (data_R_matrix.balance_ena_1 == 1 && data_R_matrix.balance_ena_2 == 1)
  {
    print_2balance();
  }
  else if (data_R_matrix.balance_ena_1 == 1 && data_R_matrix.balance_ena_2 == 0)
  {
    print_balance1();
  }
  else if (data_R_matrix.balance_ena_1 == 0 && data_R_matrix.balance_ena_2 == 1)
  {
    print_balance2();
  }
  /*else
  {
    Matrix.setFont(0, BigFontBottom);
    Matrix.setFont(1, BigFontUp);
    Matrix.displayZoneText(0, "NoData", PA_CENTER, 30, 0, PA_PRINT );
    Matrix.displayZoneText(1, "NoData", PA_CENTER, 30, 0, PA_PRINT );
    Matrix.synchZoneStart();
    while (!Matrix.getZoneStatus(0) || !Matrix.getZoneStatus(1))
    Matrix.displayAnimate();
  }*/

  Delay_scroll = data_R_matrix.Freq_text * 1000;
  if (Delay_scroll == 0)
  {
    Delay_scroll = -5;
  }
  
  if ((currentTime - previousTime) >= Delay_scroll && Delay_scroll != -5) 
  { 
    print_text();
    previousTime = currentTime;
  }

  /*if ((currentTime - previousTime_2) >= 100) 
  { 
    //send_data();
    previousTime_2 = currentTime;
  }*/
}
}

