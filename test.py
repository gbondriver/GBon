#/usr/bin/env python
import gi
gi.require_version('GBon', '1.0')
from gi.repository import GBon
from gi.repository import GLib

import time

driver = GBon.Driver.new()
b25 = GBon.B25.new()

try:
    if b25.is_enabled():
        print("b25 startup")
        b25.startup(4, False, False)
        
    print("load module")
    #driver.load_module('/usr/lib64/libGL.so.1')
    #if not driver.load_module('./BonDriver_LinuxPT.so'):
    driver.load_module('/home/BonDriver/BonDriver_Proxy-T.so')
    #driver.load_module('/home/BonDriver/BonDriver_Proxy-S.so')
    
    print("create driver")
    driver.create_driver()

    print("open tuner")
    driver.open_tuner()
    
    print("set channel2")
    driver.set_channel2(0, 72)

    f = open("test.ts", 'wb')
    
    while True:
        if b25.is_enabled():
            retval = driver.get_ts_stream(b25)
        else:
            retval = driver.get_ts_stream()
        if (retval.status):
            print("%d, %d" % (retval.size, retval.remain))
            buf = retval.get_buf()
            #print(type(buf))
            #print(retval.remain)
            #print(buf)
            f.write(buf)
        if retval.remain == 0:
            time.sleep(0.01)
                
except GLib.GError as e:
    print("%s: %d: %s" % (e.domain , e.code , e.message))
    driver.close_tuner();
    driver.release();
    driver.close_module();
    exit(-e.code)

driver.close_tuner();
driver.release();
driver.close_module();
