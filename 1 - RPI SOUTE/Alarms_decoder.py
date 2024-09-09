
code = input()

test = int(code)

code_b = f'{test:014b}'

print(code_b,"\n")

if code_b [13] == '1':
    print(" Matrice defaut ")
if code_b [12] == '1':
    print(" Humidity  ")
if code_b [11] == '1':
    print(" Temperature  ")
if code_b [10] == '1':
    print(" B2 NOFut  ")
if code_b [9] == '1':
    print(" B1 NOFut  ")
if code_b [8] == '1':
    print(" B2 ENDFut  ")
if code_b [7] == '1':
    print(" B1 ENDFut  ")
if code_b [6] == '1':
    print(" B2 Unplug  ")
if code_b [5] == '1':
    print(" B1 Unplug  ")
if code_b [4] == '1':
    print(" NB B2 Defaut  ")
if code_b [3] == '1':
    print(" NB B1 Defaut  ")
if code_b [2] == '1':
    print(" B2 Def  ")
if code_b [1] == '1':
    print(" B1 Def  ")
if code_b [0] == '1':
    print(" Communication Defaut  ")
print('\n')
