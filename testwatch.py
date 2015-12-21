#!/usr/bin/env python

from __future__ import print_function

import sys
import time

import gi
gi.require_version("Gst", "1.0")
from gi.repository import GLib, GObject, Gst
gi.require_version('GBon', '1.0')
from gi.repository import GBon

def exit(msg):
    print(msg, file=sys.stderr)
    sys.exit()


class TvPlayer(object):
    def __init__(self):
        self.fd = None
        self.mainloop = GObject.MainLoop()

        self.pipeline = Gst.ElementFactory.make("playbin", None)
        self.pipeline.set_property("uri", "appsrc://")
        self.pipeline.connect("source-setup", self.on_source_setup)

        self.bus = self.pipeline.get_bus()
        self.bus.add_signal_watch()
        #self.bus.connect("message", self.on_message)
        self.bus.connect("message::eos", self.on_eos)
        self.bus.connect("message::error", self.on_error)

    def exit(self, msg):
        self.stop()
        exit(msg)

    def stop(self):
        # Stop playback and exit mainloop
        self.pipeline.set_state(Gst.State.NULL)
        self.mainloop.quit()

    def play(self, driver, b25):
        self.driver = driver
        self.b25 = b25

        self.pipeline.set_state(Gst.State.PLAYING)
        self.mainloop.run()

    def on_source_setup(self, element, source):
        source.connect("need-data", self.on_source_need_data)
        source.connect("enough-data", self.on_source_enough_data)

    def on_source_need_data(self, source, length):
        while True:
            retval = self.driver.get_ts_stream(self.b25)
            if (retval.status):
                print("%d, %d" % (retval.size, retval.remain))
                data = retval.get_buf()
                if retval.size > 0:
                    buf = Gst.Buffer.new_wrapped(data)
                    source.emit("push-buffer", buf)
                    return
            if retval.remain == 0:
                time.sleep(0.01)
    def on_source_enough_data(self, source):
        print("enough-data")

    def on_message(self, bus, msg):
        t = msg.type
        print(t)

    def on_eos(self, bus, msg):
        self.stop()

    def on_error(self, bus, msg):
        error = msg.parse_error()[1]
        self.stop()

def main():
    if len(sys.argv) < 4:
        print("Usage: %s bondriver spaceNo ch" % sys.argv[0])
        exit(1)

    bondriver = sys.argv[1]
    spaceNo = int(sys.argv[2])
    ch = int(sys.argv[3])

    GObject.threads_init()
    Gst.init(None)

    driver = GBon.Driver.new()
    player = TvPlayer()

    try:
        print("load module")
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
        
        player.play(driver, b25)
    except GLib.GError as e:
        print("%s: %d: %s" % (e.domain , e.code , e.message))
        if (player):
            player.stop()
        driver.close_tuner();
        driver.release();
        driver.close_module();
        exit(-e.code)
    except KeyboardInterrupt:
        pass
    player.stop()
    print("close tuner")
    driver.close_tuner()
    print("release driver")
    driver.release()
    print("close module")
    driver.close_module()

if __name__ == "__main__":
    main()
