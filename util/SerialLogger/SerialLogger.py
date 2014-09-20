import serial
import csv

ser = serial.Serial()
port = input('COM Port [10]: ')  or 10
destination = input('Output File [./data.csv]: ') or './data.csv'
ser.port = int(port)
ser.open()
with open(destination, 'w', encoding='utf8') as f:
    writer = csv.writer(f)    
    while True:
        str = ser.readline().decode("utf-8")
        print(str)
        writer.writerow([str,])