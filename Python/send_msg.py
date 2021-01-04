import serial
import smtplib
import ssl

receiver = sender = 'your email'
password = 'your password'
port = 465
context = ssl.create_default_context()

ser = serial.Serial()
ser.baudrate = 115200
ser.port = 'port that you need'
ser.open()

while(1):
    line = ser.readline()
    temperature = line[0:2].decode('utf-8')
    humidity = line[3:5].decode('utf-8')
    print(f"GREENHOUSE EMERGENCY!!!! TEMPERATURE: {temperature}, HUMIDITY: {humidity}")
    message = f"""\
Subject: GREENHOUSE EMERGENCY
===================================

Our Sensors have measured changes in the environment.

====================================
    Temperature in the greenhouse now: {temperature}.
       Humidity in the greenhouse now: {humidity}.
====================================

Please, do not let our plants die.
Hope to see you soon in the greenhouse, master!

===================================
"""
    with smtplib.SMTP_SSL('smtp.gmail.com', port, context=context) as server:
        server.login(sender, password)
        server.sendmail(sender, receiver, message)

