import serial

ser = serial.Serial()
port = input('COM Port [0]: ')
sensorName = input('SensorName [pr]: ')
sensorName = sensorName or 'photoresistor'
ser.port = port or 0
ser.open()
while True:
    print ser.read()