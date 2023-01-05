#!/usr/bin/python3

from time import sleep
from threading import Event, Thread
from epevermodbus.driver import EpeverChargeController
from gps3.agps3threaded import AGPS3mechanism


controller = EpeverChargeController("/dev/ttyUSB1", 1)

agps_thread = AGPS3mechanism()  # Instantiate AGPS3 Mechanisms
agps_thread.stream_data()  # From localhost (), or other hosts, by example, (host='gps.ddns.net')
agps_thread.run_thread()  # Throttle time to sleep after an empty lookup, default 0.2 second, default daemon=True

sleep(1)

print('timestamp, latitude, longitude, Vbatt, Abatt, SOC')

stopped = Event()

def loop():
    while not stopped.wait(interval): # the first call is in `interval` secs
        print(agps_thread.data_stream.time + ',', end='')
        print(str(agps_thread.data_stream.lat) + ',', end='')
        print(str(agps_thread.data_stream.lon) + ',', end='')
#       print(agps_thread.data_stream.speed
#       print(agps_thread.data_stream.track)
        print(str(controller.get_battery_voltage()) + ',', end='')
        print(str(controller.get_battery_current()) + ',', end='')
        print(str(controller.get_battery_state_of_charge()))

Thread(target=loop).start()
