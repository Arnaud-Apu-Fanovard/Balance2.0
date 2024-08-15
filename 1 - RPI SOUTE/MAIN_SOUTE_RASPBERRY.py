
# INIT DES LIBRAIRIES------------------------------------------------------------

import serial
import time
import RPi.GPIO as GPIO

# INIT DES IO--------------------------------------------------------------------

Pin_RS485_RPI = 12   #pin RE & DE du module RS485 12 = GPIO 18
Pin_RS485_ARD = 18   #18 = GPIO 24
Pin_Led = 16         #16 = GPIO 23

# INIT DE MES TABLEAUX DE STRUCTURES---------------------------------------------

Data_Send_Soute = {"ID_RS485":1,
                   "balance_ena_1" :1,    
                   "balance_ena_2" :1,
                   "Name_beer_1" :"Maes",
                   "Name_beer_2" :"Lupulus",
                   "Rho_beer_1" : 1013.0,
                   "Rho_beer_2" : 1014.0,
                   "Weight_fut_empty_1" : 6000,
                   "Weight_fut_empty_2" : 6000,
                   "Cl_beer_1" : 250,
                   "Cl_beer_2" : 330}


Data_Send_Matrice = {"ID_RS485":2,
                     "balance_ena_1":1,
                     "balance_ena_2":0,
                     "Name_1": "Info",
                     "Name_2": "Lux",
                     "Name_beer1": "Lupulus",
                     "Name_beer2": "..",
                     "Nb_beer_1": 0,
                     "Nb_beer_2": 0,
                     "Option_Name_1":1,
                     "Option_Name_2":1,
                     "Text_const":0,
                     "Scroll":0,
                     "Freq_text":0,
                     "Text_to_print": "SUUUUUU",
                     "Offline":0}

Data_Received_Soute = {"Nb_beer_1":0,
                       "Nb_beer_2":0,
                       "Temp_Soute":20.0,
                       "Hum_Soute":50.0}

# GLOBAL VARIABLE -----------------------------------------------------------

currentTime = time.time()

send = 0
delay_1 = 0.1
time_1 = 0
previousTime_1 = time.time()




# SETUP ---------------------------------------------------------------------

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BOARD)
GPIO.setup(Pin_RS485_RPI,GPIO.OUT,initial=0)
GPIO.setup(Pin_RS485_ARD,GPIO.OUT,initial=0)
GPIO.setup(Pin_Led,GPIO.OUT,initial=0)

Com_RS485 = serial.Serial('/dev/ttyS0', 115200,timeout= 0.05)
time.sleep(0.5)
#Com_RS485.reset_input_buffer()
print("Communication Serial Ok")
GPIO.output(Pin_RS485_RPI, GPIO.LOW)
GPIO.output(Pin_Led, GPIO.LOW)

# MES FONCTIONS------------------------------------------------------------------

def send_data_soute():
    GPIO.output(Pin_RS485_RPI, GPIO.HIGH)
    GPIO.output(Pin_Led, GPIO.HIGH)

    message = ""
    print("Data send to Soute...")
    for data in Data_Send_Soute.values():
        data = str(data)
        data += "$"
        message += data
    end = "\n"
    message += end
    print(message)
    
    Com_RS485.write(message.encode('utf-8'))
    time.sleep(0.01)
    GPIO.output(Pin_RS485_RPI, GPIO.LOW)
    GPIO.output(Pin_Led, GPIO.LOW)

def send_data_matrice():
    GPIO.output(Pin_RS485_RPI, GPIO.HIGH)
    GPIO.output(Pin_Led, GPIO.HIGH)

    message2 = ""
    print("Data send to Matrix...")
    for data in Data_Send_Matrice.values():
        data = str(data)
        data += "$"
        message2 += data
    end = "\n"
    message2 += end
    print(message2)
    
    Com_RS485.write(message2.encode('utf-8'))
    time.sleep(0.01)
    GPIO.output(Pin_RS485_RPI, GPIO.LOW)
    GPIO.output(Pin_Led, GPIO.LOW)

def get_data():
    #while Com_RS485.in_waiting <= 0:
            #time.sleep(0.01)

    # if Com_RS485.in_waiting > 0:
    #     time.sleep(0.01)
    message_recu = Com_RS485.readline()
    print('message recu : ')
    print(message_recu,"\n")
    #print(type(message_recu))
    return message_recu

def parse_data(data):
    
    try:
        #print(data)
        data_parse = data.decode('utf-8')
        #print(type(data_parse))
        data_parse = data_parse.split('$')

        if int(data_parse[0]) == 0:
            Data_Received_Soute["Nb_beer_1"]= int(data_parse[1])
            Data_Received_Soute["Nb_beer_2"]= int(data_parse[2])
            Data_Received_Soute["Temp_Soute"]= float(data_parse[3])
            Data_Received_Soute["Hum_Soute"]= float(data_parse[4])
            return 1
        
    except:
        print ("..")

    
    """
    print(Data_Received_Soute["Nb_beer_1"])
    print(Data_Received_Soute["Nb_beer_2"])
    print(Data_Received_Soute["Temp_Soute"])
    print(Data_Received_Soute["Hum_Soute"])"""
    return

def data_received_from_soute_mapping():
    return

def data_send_to_soute_mapping():
    return

def data_send_to_matrix_mapping():
    Data_Send_Matrice["Nb_beer_1"] = Data_Received_Soute["Nb_beer_1"]
    Data_Send_Matrice["Nb_beer_2"] = Data_Received_Soute["Nb_beer_2"]
    return

def data_get_from_server():
    return

def data_post_to_server():
    return

# LOOP ----------------------------------------------------------------------

try:
    while True:
        
        currentTime = time.time()

        data_soute = get_data()
        receiv_OK = parse_data(data_soute)
        data_send_to_matrix_mapping()

        # send_data_soute()
        # time.sleep(0.05)
        # send_data_matrice()
        # time.sleep(0.05)


        if ( (currentTime - previousTime_1 ) >= delay_1):
            previousTime_1 = currentTime

            if send == 0:
                send_data_soute()
                send = 1  
            else:
                send_data_matrice()
                send = 0
           


except KeyboardInterrupt :
    print ("Close Serial Communication")
    Com_RS485.close()
    GPIO.cleanup()



