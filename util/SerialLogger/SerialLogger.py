import serial
import csv

ser = serial.Serial()
millis = 0
port = input('COM Port [10]: ')  or 10
destination = input('Output File [./data.csv]: ') or './data.csv'
ser.port = int(port)
ser.open()
with open(destination, 'w', encoding='utf8', newline='') as f:
    writer = csv.writer(f)    
    while True:
        str = ser.readline()
        millis += 1
        print(str)
        writer.writerow([str,millis])
