#/usr/bin/env python
import gi
gi.require_version('GBon', '1.0')
from gi.repository import GBon
from gi.repository import GLib

import sys
import time
import threading

class Recorder(threading.Thread):
    isRequestedStop = False
    
    def __init__(self, b25, fd):
        super(Recorder, self).__init__()
        self.b25 = b25
        self.fd = fd

    def run(self):
        while not self.isRequestedStop:
            retval = driver.get_ts_stream(b25)
            if (retval.status):
                print("%d, %d" % (retval.size, retval.remain))
                buf = retval.get_buf()
                fd.write(buf)
            if retval.remain == 0:
                time.sleep(0.01)
                
    def stop(self):
        print("stop")
        self.isRequestedStop = True

if len(sys.argv) < 6:
    print("Usage: %s bondriver spaceNo ch time outfile" % sys.argv[0])
    exit(1)

bondriver = sys.argv[1]
spaceNo = int(sys.argv[2])
ch = int(sys.argv[3])
t = float(sys.argv[4])
outfile = sys.argv[5]


driver = GBon.Driver.new()

try:
    print("load module")
    #driver.load_module('/usr/lib64/libGL.so.1')
    #driver.load_module('./BonDriver_LinuxPT.so'):
    #driver.load_module('/home/BonDriver/BonDriver_Proxy-T.so')
    #driver.load_module('/home/BonDriver/BonDriver_Proxy-S.so')
    driver.load_module(bondriver)
    
    print("create driver")
    driver.create_driver()

    print("open tuner")
    driver.open_tuner()
    
    print("set channel2 %d, %d" % (spaceNo, ch))
    driver.set_channel2(spaceNo, ch)

    sl = driver.get_signal_level()
    print("signal level: %f" % sl)
    
    b25 = None
    if GBon.B25.is_enabled():
        b25 = GBon.B25.new()
        print("b25 startup")
        b25.startup(4, False, False)
        
    fd = open(outfile, 'wb')

    th = Recorder(b25, fd)
    th.start()

    time.sleep(t)
    th.stop()
    th.join()
except GLib.GError as e:
    print("%s: %d: %s" % (e.domain , e.code , e.message))
    driver.close_tuner();
    driver.release();
    driver.close_module();
    exit(-e.code)
except KeyboardInterrupt:
    th.stop()
    th.join()
    pass
print("close tuner")
driver.close_tuner()
print("release driver")
driver.release()
print("close module")
driver.close_module()
    
