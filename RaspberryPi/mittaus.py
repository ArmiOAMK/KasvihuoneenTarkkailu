import serial
import time
import datetime
from pymongo import MongoClient

# Configuration
serial_port          = '/dev/ttyACM0'
mongodb_host         = 'localhost'
mongodb_db           = 'mittaus_db'

# Connect to Serial Port for communication
ser = serial.Serial(serial_port, 9600)

# Connect to MongoDB
client = MongoClient(mongodb_host, 27017)
db = client[mongodb_db]
collection = db['mittaukset']

# Setup a loop to send Temperature values at fixed intervals in seconds
fixed_interval = 3
while 1:
    try:
        # Obtained from Arduino
        temp_string = ser.readline().rstrip()
        parsed_strings = temp_string.split(",")
        temperature = float(parsed_strings[0])
        humidity = float(parsed_strings[1])

        # If we received a measurement, print it and send it to MongoDB.
        if temperature:

            doc_id = collection.insert_one({ 'temperature': temperature,
                                             'humidity': humidity,
                                             'datetime': datetime.datetime.now()}).inserted_id
            print str(doc_id) + ': ' + str(temperature) + ',' + str(humidity)
    except serial.SerialTimeoutException:
        print('Error! Could not read the Temperature Value from unit')
    except ValueError:
        print('Error! Could not convert temperature to float')
    finally:
        time.sleep(fixed_interval)