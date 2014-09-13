import serial

ser = serial.Serial()
port = int(input('COM Port [3]: '))
sensorName = input('SensorName [pr]: ')
ser.port = int(port)
ser.open()
while True:
    print (ser.read().decode("utf-8"))
