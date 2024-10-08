#include <Arduino.h>

// INIT DES LIBRAIRIES------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <HX711.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// INIT DES IO--------------------------------------------------------------------

#define Led_R 12
#define Led_O 11
#define Led_V 10
#define Led_B 9

#define Pin_Rs485 2

#define Matrix_FB 13

#define CLK_HX711_1 5
#define DATA_HX711_1 6
//#define HX711_1_FB 13

#define CLK_HX711_2 7
#define DATA_HX711_2 8
//#define HX711_2_FB A5

#define DHT_pin 4
#define typeDeDHT DHT22

// INIT DE MES PARAMETRES DE VARIABLES--------------------------------------------

// Balance
HX711 Balance_1;
HX711 Balance_2;

// DHT22
DHT dht(DHT_pin, typeDeDHT);

// LCD Ecran de TEST
LiquidCrystal_I2C lcd(0x27,16,2);

// Timer et délai
// unsigned long currentTime = 0;
// unsigned long previousTime_Send_Data = 0;
// unsigned long previousTime_Send_Data_Offline = 0;
// unsigned long previousTime_dht22 = 0;
// int Delay_Send_Data = 100;
// int Delay_Send_Data_Offline = 100;
// int Delay_dht22 = 2000;
// unsigned long previousTimer_Received_Data = 0;
// unsigned long Delay_Received_Data = 10000;
// unsigned long Timer_Received_Data = 0;
// unsigned long previousTime_blink_R = 0;
// unsigned long previousTime_blink_O = 0;
// unsigned long previousTime_blink_V = 0;
// unsigned long previousTime_blink_B = 0;
// int Delay_led = 500;

// Variables
String message;
String data;
String sub;
bool error_get_data;
int ID_Arduino_Soute = 1;
bool send_data_ok = 0;
bool ledstate_R = false;
bool ledstate_O = false;
bool ledstate_V = false;


//"1$0$0$Maes$Lupulus$1013$1014$16000$13600$250$330$"

// INIT DE MES TABLEAUX DE STRUCTURES---------------------------------------------

struct data_Received
{
  int ID_RS485;
  bool balance_ena_1;
  bool balance_ena_2;
  String Name_beer_1;
  String Name_beer_2;
  float Rho_beer_1;
  float Rho_beer_2;
  double Weight_fut_empty_1;
  double Weight_fut_empty_2;
  int Cl_beer_1;
  int Cl_beer_2;
};
typedef struct data_Received Data_Received;

struct data_Offline
{
  float Rho_beer = 1.013;
  double Weight_fut_empty = 6000;
  int Cl_beer = 250;
};
typedef struct data_Offline Data_Offline;

struct data_Send
{
  int ID_RS485 = 0;
  int Nb_beer_1;
  int Nb_beer_2;
  float Temp_Soute;
  int Hum_Soute;
};
typedef struct data_Send Data_Send;

struct data_Send_Offline
{
  int ID_RS485 = 2;
  bool balance_ena_1;
  bool balance_ena_2;
  String Name_1 = "Info";
  String Name_2 = "Externe";
  int Nb_beer_1;
  int Nb_beer_2;
};
typedef struct data_Send_Offline Data_Send_Offline;

struct alarms
{
  bool Com_RPI_Arduino_def = 0;
  bool Balance_1_def = 0;
  bool Balance_2_def = 0;
  bool Nb_balance_1_def = 0;
  bool Nb_balance_2_def = 0;
  bool Balance_1_Unplug = 0;
  bool Balance_2_Unplug = 0;
  bool B1_EndFut = 0;
  bool B2_EndFut = 0;
  bool B1_NoFut = 0;
  bool B2_NoFut = 0;
  bool Temperature_def = 0;
  bool Humidity_def = 0;
  bool Matrix_def = 0;
  int Alarms_code = 0;
};
typedef struct alarms Alarms;

struct delays
{
  unsigned long currentTime = 0;
  int Delay_led = 500;
  int Delay_Send_Data = 100;
  int Delay_Send_Data_Offline = 100;
  int Delay_dht22 = 2000;
  int Delay_balance = 2000;
  unsigned long previousTime_Send_Data = 0;
  unsigned long previousTime_Send_Data_Offline = 0;
  unsigned long previousTime_dht22 = 0;
  unsigned long previousTimer_Received_Data = 0;
  unsigned long previousTime_b1 = 0;
  unsigned long previousTime_b2 = 0;
  unsigned long previousTime_blink_R = 0;
  unsigned long previousTime_blink_O = 0;
  unsigned long previousTime_blink_V = 0;
  unsigned long previousTime_blink_B = 0;
  unsigned long Delay_Received_Data = 10000;
  unsigned long Timer_Received_Data = 0;
};
typedef struct delays Delays;

struct datas
{
  double weight_1_zero;
  double weight_2_zero;
  double weight_1;
  double weight_2;
  int weight_tole = 7750;
};
typedef struct datas Datas;


// INIT DE MES STRUCTURE----------------------------------------------------------

Data_Received data_received;
Data_Send data_send;
Data_Send_Offline data_send_offline;
Data_Offline data_offline;
Alarms alarms_default;
Delays delays;
Datas datas;


// MES FONCTIONS------------------------------------------------------------------

// SETUP FONCTIONS-----

void setup_led()
{
  pinMode(Led_R, OUTPUT);
  pinMode(Led_O, OUTPUT);
  pinMode(Led_V, OUTPUT);
  pinMode(Led_B, OUTPUT);
  digitalWrite(Led_R, LOW);
  digitalWrite(Led_O, LOW);
  digitalWrite(Led_V, LOW);
  digitalWrite(Led_B, LOW);
}

void setup_com() // Initialisation de la communication RS485
{
  pinMode(Pin_Rs485, OUTPUT);
  digitalWrite(Pin_Rs485, LOW);

  Serial.begin(115200);
  Serial.println("Balance starting...");
  Serial.setTimeout(25);
  while (!Serial)
  {}
}

void setup_balance() // Initialisation des balances
{
  Balance_1.begin(DATA_HX711_1, CLK_HX711_1);
  Balance_1.set_scale(23.412162);
  Balance_1.tare(0);
  Balance_2.begin(DATA_HX711_2, CLK_HX711_2);
  Balance_2.set_scale(23.412162);
  Balance_2.tare(0);
}

void setup_DHT() // Initialisation du capteur de température & humidité
{
  dht.begin();
}

void setup_LCD_debug() // Initialisation du LCD pour les tests
{
  
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0,0);  
  lcd.print("Balance Info");
}

// LOOP FONCTIONS-----

void sensors() //2.0
{
  if ((delays.currentTime - delays.previousTime_dht22) >= delays.Delay_dht22) {
    data_send.Hum_Soute = dht.readHumidity();
    data_send.Temp_Soute = dht.readTemperature();
    delays.previousTime_dht22 = delays.currentTime;
    // Serial.print("Hum:");
    // Serial.print(data_send.Hum_Soute);
    // Serial.print("  Temp:");
    // Serial.println(data_send.Temp_Soute);
  }

  if(Balance_1.is_ready())
  {
    datas.weight_1_zero = -Balance_1.get_units(1);
    datas.weight_1 = datas.weight_1_zero - datas.weight_tole;
    delays.previousTime_b1 = delays.currentTime;
    // Serial.print("B1:");
    // Serial.print(datas.weight_1);
    // Serial.print("   ");
  }

  if(Balance_2.is_ready())
  {
    datas.weight_2_zero = -Balance_2.get_units(1);
    datas.weight_2 = datas.weight_2_zero - datas.weight_tole;
    delays.previousTime_b2 = delays.currentTime;
    // Serial.print("B2:");
    // Serial.print(datas.weight_2);
    // Serial.print("   ");
  }
}

void get_nb_beers_b1(bool Online) //2.0
{
  double weight = datas.weight_1;
  if (Online == HIGH)
  {
    weight -= data_received.Weight_fut_empty_1;
    weight /= data_received.Rho_beer_1;
    data_send.Nb_beer_1 = weight / data_received.Cl_beer_1;
    // Serial.print("Beer1:");
    // Serial.print(data_send.Nb_beer_1);
    // Serial.print("   ");
  }
  else
  {
    weight -= data_offline.Weight_fut_empty;
    weight /= data_offline.Rho_beer;
    data_send_offline.Nb_beer_1 = weight / data_offline.Cl_beer;
  }
}

void get_nb_beers_b2(bool Online) //2.0
{
  double weight = datas.weight_2;
  if (Online == HIGH)
  {
    weight -= data_received.Weight_fut_empty_2;
    weight /= data_received.Rho_beer_2;
    data_send.Nb_beer_2 = weight / data_received.Cl_beer_2;
    // Serial.print("Beer2:");
    // Serial.print(data_send.Nb_beer_2);
    // Serial.print("   ");
  }
  else
  {
    weight -= data_offline.Weight_fut_empty;
    weight /= data_offline.Rho_beer;
    data_send_offline.Nb_beer_2 = weight / data_offline.Cl_beer;
  }
}

void alarms_b1() //2.0
{
  if (delays.currentTime - delays.Delay_balance > delays.previousTime_b1 )
  {
    alarms_default.Balance_1_Unplug = 1;
    // alarms_default.Balance_1_def = 0;
    alarms_default.B1_EndFut = 0;
    alarms_default.B1_NoFut = 0;
  }
  else
  {
    alarms_default.Balance_1_Unplug = 0;
    if (datas.weight_1 >= 70000)
    {alarms_default.Balance_1_def = true;    }
    else
    {alarms_default.Balance_1_def = false;}

    if (datas.weight_1 <= 5000 && datas.weight_1 >= 250 )
    {
      alarms_default.B1_EndFut = 1;
      alarms_default.B1_NoFut = 0;
    }
    else if (datas.weight_1 < 250)
    {
      alarms_default.B1_EndFut = 0;
      alarms_default.B1_NoFut = 1;
    }
    else
    {
      alarms_default.B1_EndFut = 0;
      alarms_default.B1_NoFut = 0;
    }
  } 

  // Serial.print(alarms_default.Balance_1_Unplug);
  // Serial.print(alarms_default.Balance_1_def);
  // Serial.print(alarms_default.B1_EndFut);
  // Serial.println(alarms_default.B1_NoFut);

}

void alarms_b2() //2.0
{
  if (delays.currentTime - delays.Delay_balance > delays.previousTime_b2 )
  { 
    alarms_default.Balance_2_Unplug = 1;
    // alarms_default.Balance_2_def = 0;
    alarms_default.B2_EndFut = 0;
    alarms_default.B2_NoFut = 0;
  }
  else
  {
    alarms_default.Balance_2_Unplug = 0;
    if (datas.weight_2 >= 70000)
    {alarms_default.Balance_2_def = true;    }
    else
    {alarms_default.Balance_2_def = false;}

    if (datas.weight_2 <= 5000 && datas.weight_2 >= 250 )
    {
      alarms_default.B2_EndFut = 1;
      alarms_default.B2_NoFut = 0;
    }
    else if (datas.weight_2 < 250)
    {
      alarms_default.B2_EndFut = 0;
      alarms_default.B2_NoFut = 1;
    }
    else
    {
      alarms_default.B2_EndFut = 0;
      alarms_default.B2_NoFut = 0;
    }
  } 

  // Serial.print(alarms_default.Balance_2_Unplug);
  // Serial.print(alarms_default.Balance_2_def);
  // Serial.print(alarms_default.B2_EndFut);
  // Serial.println(alarms_default.B2_NoFut);
}

bool get_data() //1.0
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

    delays.Timer_Received_Data = delays.currentTime;
    return 1;
  }
  else
  {
    return 0;  
  }  
}
/*
void get_nb_beers() //1.0
{
  double weight_1;
  double weight_2;
  
  if (data_received.balance_ena_1 == true && Balance_1.is_ready())
  {
    Balance_1.set_scale(23.553215003040);
    //Serial.print("Scale B1:");
    //Serial.print(Balance_1.get_scale());
    weight_1 = -Balance_1.get_units(1);
    weight_1 = weight_1 - 7750;
    //Serial.print(" Poids B1:");
    //Serial.println(weight_1);
    //Serial.println(data_received.Weight_fut_empty_1);

    if (weight_1 < data_received.Weight_fut_empty_1 )
    {
      alarms_default.Balance_1_Unplug = 1;
      weight_1 -= data_received.Weight_fut_empty_1;
      weight_1 /= data_received.Rho_beer_1;
      data_send.Nb_beer_1 = weight_1 / data_received.Cl_beer_1;
    }
    else
    {
      alarms_default.Balance_1_Unplug = 0;
      if (weight_1 >= 70000)
      {
        alarms_default.Balance_1_def = true;
      }
      else
      {
        alarms_default.Balance_1_def = false;
      }

    //weight_1 = 30000.0; // Valeur test
    weight_1 -= data_received.Weight_fut_empty_1;
    weight_1 /= data_received.Rho_beer_1;
    data_send.Nb_beer_1 = weight_1 / data_received.Cl_beer_1;
    }

    if (data_send.Nb_beer_1 <= 8 && data_send.Nb_beer_1 > 1 )
    {
      alarms_default.B1_EndFut = 1;
      alarms_default.B1_NoFut = 0;
    }
    else if (data_send.Nb_beer_1 <= 1)
    {
      alarms_default.B1_EndFut = 0;
      alarms_default.B1_NoFut = 1;
    }
    else
    {
      alarms_default.B1_EndFut = 0;
      alarms_default.B1_NoFut = 0;
    }
  }
  
  if (data_received.balance_ena_2 == true && Balance_2.is_ready())
  {
    Balance_2.set_scale(23.553215003040);
    //Serial.print("Scale B2:");
    //Serial.print(Balance_2.get_scale());
    weight_2 = -Balance_2.get_units(1);
    weight_2 = weight_2 - 7750;

    if (weight_2 < data_received.Weight_fut_empty_2 )
    {
      alarms_default.Balance_2_Unplug = 1;
      weight_2 -= data_received.Weight_fut_empty_2;
      weight_2 /= data_received.Rho_beer_2;
      data_send.Nb_beer_2 = weight_2 / data_received.Cl_beer_2; 
    }
    else
    {
      alarms_default.Balance_2_Unplug = 0;
      if (weight_2 >= 70000)
      {
        alarms_default.Balance_2_def = true;
      }
      else
      {
        alarms_default.Balance_2_def = false;
      }

    //weight_2 = 30000.0; // Valeur test
    weight_2 -= data_received.Weight_fut_empty_2;
    weight_2 /= data_received.Rho_beer_2;
    data_send.Nb_beer_2 = weight_2 / data_received.Cl_beer_2; 
    }

    if (data_send.Nb_beer_2 <= 8 && data_send.Nb_beer_2 > 1 )
    {
      alarms_default.B2_EndFut = 1;
      alarms_default.B2_NoFut = 0;
    }
    else if (data_send.Nb_beer_2 <= 1)
    {
      alarms_default.B2_EndFut = 0;
      alarms_default.B2_NoFut = 1;
    }
    else
    {
      alarms_default.B2_EndFut = 0;
      alarms_default.B2_NoFut = 0;
    }
  }
}
*/
void nb_balance_offline(float weight,int balance)
{
  if (weight >= 0 && balance == 1)
    {data_send_offline.balance_ena_1 = 1;}
    else
    {data_send_offline.balance_ena_1 = 0;}

    if (weight >= 0 && balance == 2)
    {data_send_offline.balance_ena_2 = 1;}
    else
    {data_send_offline.balance_ena_1 = 0;}
}

void get_nb_beers_offline() // TODO intégrer le système d'alarme des deux balances pour cette fonction
{
  double weight_1;
  double weight_2;

  if (Balance_1.is_ready())
  {
    //Balance_1.set_scale(23.412162);
    //Serial.print("Scale B1 OFF:");
    //Serial.print(Balance_1.get_scale());
    weight_1 = -Balance_1.get_units(1);
    weight_1 = weight_1 - 7750;
    Serial.print(" Poids B1 OFF:");
    Serial.print(weight_1);

    if (weight_1 < data_received.Weight_fut_empty_1 )
    {
      alarms_default.Balance_1_Unplug = 1;
      weight_1 -= data_offline.Weight_fut_empty;
      weight_1 /= data_offline.Rho_beer;
      data_send_offline.Nb_beer_1 = weight_1 / data_offline.Cl_beer;
    }
    else
    {
      alarms_default.Balance_1_Unplug = 0;
      if (weight_1 >= 70000)
      {
        alarms_default.Balance_1_def = true;
      }
      else
      {
        alarms_default.Balance_1_def = false;
      }
      weight_1 -= data_offline.Weight_fut_empty;
      weight_1 /= data_offline.Rho_beer;
      data_send_offline.Nb_beer_1 = weight_1 / data_offline.Cl_beer;
    }
    
    if (data_send.Nb_beer_1 <= 8 && data_send.Nb_beer_1 > 1 )
      {
      alarms_default.B1_EndFut = 1;
      alarms_default.B1_NoFut = 0;
      }
    else if (data_send.Nb_beer_1 <= 1)
      {
      alarms_default.B1_EndFut = 0;
      alarms_default.B1_NoFut = 1;
      }
    else
      {
      alarms_default.B1_EndFut = 0;
      alarms_default.B1_NoFut = 0;
      }   
  }

  


  if (Balance_2.is_ready())
  {
    //Balance_2.set_scale(23.553215003040);
    //Serial.print("Scale B2:");
    //Serial.print(Balance_2.get_scale());
    weight_2 = -Balance_2.get_units(1);
    weight_2 = weight_2 - 7750;
    // Serial.print("  Poids B2 OFF:");
    // Serial.println(weight_2);

    if (weight_2 < data_received.Weight_fut_empty_2 )
    {
      alarms_default.Balance_2_Unplug = 1;
      weight_2 -= data_offline.Weight_fut_empty;
      weight_2 /= data_offline.Rho_beer;
      data_send_offline.Nb_beer_2 = weight_2 / data_offline.Cl_beer;
    }
    else
    {
      alarms_default.Balance_2_Unplug = 0;
      if (weight_2 >= 70000)
      {
        alarms_default.Balance_2_def = true;
      }
      else
      {
        alarms_default.Balance_2_def = false;
      }
      weight_2 -= data_offline.Weight_fut_empty;
      weight_2 /= data_offline.Rho_beer;
      data_send_offline.Nb_beer_2 = weight_2 / data_offline.Cl_beer;
    }

    if (data_send.Nb_beer_2 <= 8 && data_send.Nb_beer_2 > 1 )
      {
      alarms_default.B2_EndFut = 1;
      alarms_default.B2_NoFut = 0;
      }
    else if (data_send.Nb_beer_2 <= 1)
      {
      alarms_default.B2_EndFut = 0;
      alarms_default.B2_NoFut = 1;
      }
    else
      {
      alarms_default.B2_EndFut = 0;
      alarms_default.B2_NoFut = 0;
      } 
  }


  // Serial.print("                                                               Poids B1 OFF:");
  // Serial.print(weight_1);
  // Serial.print("  Poids B2 OFF:");
  // Serial.print(weight_2);
  // Serial.println("    ");
  nb_balance_offline(weight_1,1);
  nb_balance_offline(weight_2,2);
}

bool parse_data()
{
  
  int startPos = 0;
  int foundPos = -1;
  int i = 0;
  error_get_data = 0;

  while ((foundPos = data.indexOf("$", startPos)) != -1)
  {
    sub = data.substring(startPos, foundPos);
    

    if (startPos == 0 && (sub.toInt() != ID_Arduino_Soute))
    {
      send_data_ok = 0;
      break;
    }

    switch (i)
    {
    case 0:
      data_received.ID_RS485 = sub.toInt();
      break;
    case 1:
      if (sub == String("0"))
      {
        data_received.balance_ena_1 = 0;
      }
      else
      {
        data_received.balance_ena_1 = 1;
      }
      break;

    case 2:
      if (sub == String("0"))
      {
        data_received.balance_ena_2 = 0;
      }
      else
      {
        data_received.balance_ena_2 = 1;
      }
      break;

    case 3:
      data_received.Name_beer_1 = sub;
      break;

    case 4:
      data_received.Name_beer_2 = sub;
      break;

    case 5:
      data_received.Rho_beer_1 = sub.toFloat();
      data_received.Rho_beer_1 /= 1000;
      break;

    case 6:
      data_received.Rho_beer_2 = sub.toFloat();
      data_received.Rho_beer_2 /= 1000;
      break;

    case 7:
      data_received.Weight_fut_empty_1 = sub.toDouble();
      break;

    case 8:
      data_received.Weight_fut_empty_2 = sub.toDouble();
      break;

    case 9:
      data_received.Cl_beer_1 = sub.toInt();
      break;

    case 10:
      data_received.Cl_beer_2 = sub.toInt();
      break;

    default:
      error_get_data = 1;
    }

    i++;
    startPos = foundPos + 1;
  }
  return error_get_data;
}

bool send_data()
{
  String data_to_send = String(data_send.ID_RS485) + "$" 
                      + String(data_send.Nb_beer_1) + "$" 
                      + String(data_send.Nb_beer_2) + "$" 
                      + String(data_send.Temp_Soute) + "$"
                      + String(data_send.Hum_Soute) + "$"
                      + String(alarms_default.Alarms_code) + "$";
  digitalWrite(Led_B, HIGH);
  digitalWrite(Pin_Rs485, HIGH);
  delay(2);
  //Serial.print("Arduino: ");
  Serial.println(data_to_send);
  delay(8);
  digitalWrite(Pin_Rs485, LOW);
  digitalWrite(Led_B, LOW);
  return 1;
}

bool send_data_offline()
{
  String data_to_send = String(data_send_offline.ID_RS485) + "$" 
                      + String(data_send_offline.balance_ena_1) + "$"
                      + String(data_send_offline.balance_ena_2) + "$"
                      + data_send_offline.Name_1 + "$"
                      + data_send_offline.Name_2 + "$Biere 1$Biere 2$"
                      + String(data_send_offline.Nb_beer_1) + "$" 
                      + String(data_send_offline.Nb_beer_2) + "$0$0$0$1$300$Bienvenu au cercle$1$" ;
  digitalWrite(Led_B, HIGH);
  digitalWrite(Pin_Rs485, HIGH);
  //delay(10);
  //Serial.print("Arduino: ");
  Serial.println(data_to_send);
  delay(10);
  digitalWrite(Pin_Rs485, LOW);
  digitalWrite(Led_B, LOW);
  return 1;
}

void blink_led(bool OnOff, int ledpin, bool ledstate, unsigned long previousTime_blink, int Delay)
{
  if (((delays.currentTime - previousTime_blink) >= Delay) && OnOff == true)
  {
    previousTime_blink = delays.currentTime;
    ledstate = !ledstate;
    digitalWrite(ledpin,ledstate);
  } 
  else if (OnOff == false)
  {
    digitalWrite(ledpin,LOW);
  } 
}

// void blink_led_Red(bool OnOff)
// {
//   if (((currentTime - previousTime_blink_R) >= Delay_led) && OnOff == true)
//   {
//     previousTime_blink_R = currentTime;
//     ledstate_R = !ledstate_R;
//     digitalWrite(Led_R,ledstate_R);
//     //Serial.print("LED:   ");
//     //Serial.println(ledstate_R);
//   } 
//   else if (OnOff == false)
//   {
//     digitalWrite(Led_R,LOW);
//   } 
// }

// void blink_led_Orange(bool OnOff)
// {
//   if (((currentTime - previousTime_blink_O) >= Delay_led) && OnOff == true)
//   {
//     previousTime_blink_O = currentTime;
//     ledstate_O = !ledstate_O;
//     digitalWrite(Led_O,ledstate_O);
//     //Serial.print("LED:   ");
//     //Serial.println(ledstate_O);
//   }
//   else if (OnOff == false)
//   {
//     digitalWrite(Led_O,LOW);
//   }
// }

// void blink_led_Green(bool OnOff)
// {
//   if (((currentTime - previousTime_blink_V) >= Delay_led) && OnOff == true)
//   {
//     previousTime_blink_V = currentTime;
//     ledstate_V = !ledstate_V;
//     digitalWrite(Led_V,ledstate_V);
//     //Serial.print("LED:   ");
//     //Serial.println(ledstate_V);
//   }
//   else if (OnOff == false)
//   {
//     digitalWrite(Led_V,LOW);
//   }
// }

void alarms_and_defaults()
{ 

  if (digitalRead(Matrix_FB) == HIGH)
  {alarms_default.Matrix_def = 0;}
  else
  {alarms_default.Matrix_def = 1;}
  
  
  //Serial.print("                                                 Alarmes :       ");
  bool prob;
  //Serial.println(alarms_default.Balance_1_Unplug);
  //Serial.println(alarms_default.Balance_2_Unplug);
  if ((alarms_default.Balance_1_Unplug == HIGH && data_received.balance_ena_1 == HIGH) || (alarms_default.Balance_2_Unplug == HIGH && data_received.balance_ena_1 == HIGH) || alarms_default.Matrix_def == HIGH)
  {
    digitalWrite(Led_R, HIGH);
    prob = 1;
    //Serial.print("Rouge:");
    //Serial.print("ON  ");
  }
  else
  {
    digitalWrite(Led_R, LOW);
    prob = 0;
    //Serial.print("Rouge:");
    //Serial.print("OFF ");
  }
  
  
  if (alarms_default.Balance_1_def == HIGH || alarms_default.Balance_2_def == HIGH)
  {
    digitalWrite(Led_O, HIGH);
    //Serial.print("Orange:");
    //Serial.print("ON  ");
  }
  else if (alarms_default.B1_EndFut == HIGH || alarms_default.B2_EndFut == HIGH || alarms_default.B1_NoFut == HIGH || alarms_default.B2_NoFut == HIGH ||alarms_default.Humidity_def || alarms_default.Temperature_def )
  {
    //blink_led_Orange(HIGH);
    blink_led(HIGH,Led_O,ledstate_O,delays.previousTime_blink_O, delays.Delay_led);
    //Serial.print("Orange blink:");
    //Serial.print("ON  ");
  }
  else
  {
    digitalWrite(Led_O, LOW);
    //Serial.print("Orange:");
    //Serial.print("OFF ");
  }


  if (prob == HIGH)
  {
    digitalWrite(Led_V, LOW);
    //Serial.print("Vert:");
    //Serial.println("OFF ");
  }
  else if (prob == LOW && alarms_default.Com_RPI_Arduino_def == HIGH )
  {
    // blink_led_Green(HIGH);
    blink_led(HIGH,Led_V,ledstate_V,delays.previousTime_blink_V, delays.Delay_led);
    //Serial.print("Vert blink:");
    //Serial.println("ON  ");
  }
  else
  {
    digitalWrite(Led_V, HIGH);
    //Serial.print("Vert:");
    //Serial.println("ON  ");
  }

  alarms_default.Alarms_code =  (alarms_default.Com_RPI_Arduino_def << 13) | 
                                (alarms_default.Balance_1_def << 12) |
                                (alarms_default.Balance_2_def << 11) |
                                (alarms_default.Nb_balance_1_def << 10) |
                                (alarms_default.Nb_balance_2_def << 9) |
                                (alarms_default.Balance_1_Unplug << 8) |
                                (alarms_default.Balance_2_Unplug << 7) |
                                (alarms_default.B1_EndFut << 6) |
                                (alarms_default.B2_EndFut << 5) |
                                (alarms_default.B1_NoFut << 4) |
                                (alarms_default.B2_NoFut << 3) |
                                (alarms_default.Temperature_def << 2) |
                                (alarms_default.Humidity_def << 1) |
                                alarms_default.Matrix_def;
                                
}

// DEBUG FONCTIONS-----

void teleplot_data()
{
  Serial.print(">Temp:");
  Serial.println(data_send.Temp_Soute);
  Serial.print(">Hum:");
  Serial.println(data_send.Hum_Soute);
  Serial.print(">B1_weight:");
  Serial.println(datas.weight_1_zero);
  Serial.print(">B2_weight:");
  Serial.println(datas.weight_2_zero);
}

void valeurs_test()
{
    // Valeur test
  data_received.balance_ena_1 = true;
  data_received.balance_ena_2 = false;
  data_received.Rho_beer_1 = 1.013;
  data_received.Rho_beer_2 = 1.013;
  data_received.Weight_fut_empty_1 = 1000;
  data_received.Weight_fut_empty_2 = 1000;
  data_received.Cl_beer_1 = 250;
  data_received.Cl_beer_2 = 250;
}

void serial_debug()
{
  Serial.print("W1: ");
  Serial.print(datas.weight_1);
  Serial.print("  W2:");
  Serial.print(datas.weight_2);
  Serial.print(" || ");
  Serial.print("  B1:");
  Serial.print(data_send.Nb_beer_1);
  Serial.print("  B2:");
  Serial.print(data_send.Nb_beer_2);
  Serial.print(" ||  D1 ");
  Serial.print(alarms_default.Balance_1_Unplug);
  Serial.print(alarms_default.Balance_1_def);
  Serial.print(alarms_default.B1_EndFut);
  Serial.print(alarms_default.B1_NoFut);
  Serial.print(" ||  D2 ");
  Serial.print(alarms_default.Balance_2_Unplug);
  Serial.print(alarms_default.Balance_2_def);
  Serial.print(alarms_default.B2_EndFut);
  Serial.println(alarms_default.B2_NoFut);
}

// SETUP ---------------------------------------------------------------------

void setup()
{
  setup_led();
  setup_balance();
  setup_DHT();
  setup_com();
  // setup_LCD_debug();
  
  // Séquence de démarrage
  for (byte i = 0; i < 2; i++)
  {
    digitalWrite(Led_R, HIGH);
    delay(500);
    digitalWrite(Led_O, HIGH);
    delay(500);
    digitalWrite(Led_V, HIGH);
    delay(500);
    digitalWrite(Led_B, HIGH);
    delay(500);
    digitalWrite(Led_R, LOW);
    delay(500);
    digitalWrite(Led_O, LOW);
    delay(500);
    digitalWrite(Led_V, LOW);
    delay(500);
    digitalWrite(Led_B, LOW);
    delay(500);
  }

  Serial.println("...");
  
}

// LOOP ----------------------------------------------------------------------

void loop()
{
  delays.currentTime = millis();

  //valeurs_test();

  send_data_ok = get_data();

  parse_data();
  sensors();

  if ((delays.currentTime - delays.Timer_Received_Data) >= delays.Delay_Received_Data) //Offline mode activation 
  {
    alarms_b1();
    alarms_b2();
    
    alarms_default.Com_RPI_Arduino_def = 1;
    delays.previousTimer_Received_Data = delays.currentTime;

    if (alarms_default.Balance_1_Unplug == HIGH)
    {data_send_offline.balance_ena_1 = 0;}
    else
    {data_send_offline.balance_ena_1 = 1;}
    
    if (alarms_default.Balance_2_Unplug == HIGH)
    {data_send_offline.balance_ena_2 = 0;}
    else
    {data_send_offline.balance_ena_2 = 1;}
    
    get_nb_beers_b1(0);
    get_nb_beers_b2(0);
    if ((delays.currentTime - delays.previousTime_Send_Data_Offline) >= delays.Delay_Send_Data_Offline)
      {
      send_data_offline();
      delays.previousTime_Send_Data_Offline = delays.currentTime;
      }  
  }
  else
  {
    if (data_received.balance_ena_1 == 1 && data_received.balance_ena_2 == 1)
    {
      alarms_b1();
      alarms_b2();
    }
    else if (data_received.balance_ena_1 == 1 && data_received.balance_ena_2 == 0)
    {alarms_b1();
    alarms_default.Balance_2_Unplug = 0;
    alarms_default.Balance_2_def = 0;
    alarms_default.B2_EndFut = 0;
    alarms_default.B2_NoFut = 0;}
    else if (data_received.balance_ena_2 == 1 && data_received.balance_ena_1 == 0)
    {alarms_b2();
    alarms_default.Balance_1_Unplug = 0;
    alarms_default.Balance_1_def = 0;
    alarms_default.B1_EndFut = 0;
    alarms_default.B1_NoFut = 0;}


    alarms_default.Com_RPI_Arduino_def = 0;
    if (data_received.balance_ena_1 == 1)
    {get_nb_beers_b1(1);}
    if (data_received.balance_ena_2 == 1)
    {get_nb_beers_b2(1);}
    if (((delays.currentTime - delays.previousTime_Send_Data) >= delays.Delay_Send_Data) && send_data_ok == HIGH )
      {send_data();
      delays.previousTime_Send_Data = delays.currentTime;}
  }
  alarms_and_defaults();

  //serial_debug();
  //teleplot_data();

//--------------------------------------------------------------

  // Valeur test pour la communication
  //data_send.Nb_beer_1 = 78;
  //data_send.Nb_beer_2 = 154;
  // delays.currentTime = millis();
  // time_send_data_ok = get_data();
  // if (time_send_data_ok == true)
  // {
  //   delays.Timer_Received_Data = delays.currentTime;
  //   time_send_data_ok = 0;
  // }
  // parse_data();
  // //valeurs_test();
  // if ((delays.currentTime - delays.Timer_Received_Data) >= delays.Delay_Received_Data) //Offline mode activation 
  // {
  //   alarms_default.Com_RPI_Arduino_def = 1;
  //   delays.previousTimer_Received_Data = delays.currentTime;
  //   get_nb_beers_offline();
  //   if ((delays.currentTime - delays.previousTime_Send_Data_Offline) >= delays.Delay_Send_Data_Offline)
  //     {
  //     send_data_offline();
  //     delays.previousTime_Send_Data_Offline = delays.currentTime;
  //     }  
  // }
  // else
  // {
  //   alarms_default.Com_RPI_Arduino_def = 0;
  //   get_nb_beers();
  //   if ((delays.currentTime - delays.previousTime_Send_Data) >= delays.Delay_Send_Data)
  //     {
  //     //Serial.println("data send");
  //     send_data();
  //     delays.previousTime_Send_Data = delays.currentTime;
  //     }
  // }
  // alarms_and_defaults();
} 


    


