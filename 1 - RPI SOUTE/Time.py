import time

time_1 = 0
previousTime_1 = time.time() + 2.5
delay_1 = 5

time_2 = 0
previousTime_2 = time.time()
delay_2 = 5.0

try:
    while True:
        currentTime = time.time()
        # print(currentTime)
        time_1 = currentTime - previousTime_1
        time_2 = currentTime - previousTime_2

        if time_1 >= delay_1:
            previousTime_1 = currentTime
            print(" 1 ")

        if time_2 >= delay_2:
            previousTime_2 = currentTime
            print(" 2 ")

except KeyboardInterrupt :
    print ("Close Serial Communication")
