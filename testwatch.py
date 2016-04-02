#!/usr/bin/env python3
 
import sys, time, threading, gi
gi.require_version("Gtk", "3.0")
gi.require_version("Gst", "1.0")
gi.require_version("GdkX11", "3.0")
from gi.repository import GObject, GLib, Gio, Gtk, Gdk, Gst, GdkX11, GstMpegts
gi.require_version('GBon', '1.0')
from gi.repository import GBon

        
class TvGst():
    def __init__(self, useGL, driver, b25):
        self.useGL = useGL
        self.driver = driver
        self.b25 = b25
        self.audio = 2
        self.pipeline = None
        self.chcker = None
        self.init()

    def init(self):
        #self.pipeline = Gst.parse_launch("appsrc name=appsrc ! tsdemux name=demux demux. ! queue name=vqueue  ! mpegvideoparse ! mpeg2dec ! videoconvert ! gtksink name=gtksink demux. ! queue name=aqueue ! faad ! autoaudiosink ")
        if self.pipeline:
            self.pipeline.remove(self.demux)
        self.pipeline = Gst.Pipeline.new("pipeline");
        
        self.appsrc = Gst.ElementFactory.make("appsrc", "appsrc")
        self.appsrc.connect("need-data", self.on_source_need_data)
        self.appsrc.connect("enough-data", self.on_source_enough_data)

        self.demux = Gst.ElementFactory.make("tsdemux", "demux")
        self.demux.connect("pad-added", self.on_demux)
        print(self.demux.get_metadata('long-name'))

        self.vqueue = Gst.ElementFactory.make("queue", "vqueue")
        self.mpegvideoparse = Gst.ElementFactory.make("mpegvideoparse", "mpegvideoparse")
        self.mpeg2dec = Gst.ElementFactory.make("mpeg2dec", "mpeg2dec")
        self.videoconvert = Gst.ElementFactory.make("videoconvert", \
                                                    "videoconvert")
        if self.useGL:
            self.glsinkbin = Gst.ElementFactory.make("glsinkbin", "glsinkbin")
            self.videosink = Gst.ElementFactory.make("gtkglsink", "gtkglsink")
            self.glsinkbin.set_property("sink", self.videosink)
        else:
            self.videosink = Gst.ElementFactory.make("gtksink", "glsinkbin")

        self.aqueue = Gst.ElementFactory.make("queue", "aqueue")
        self.faad = Gst.ElementFactory.make("faad", "faad")
        self.input_selector = Gst.ElementFactory.make("input-selector", "input-selector")
        self.deinterleave = Gst.ElementFactory.make("deinterleave", "dintterleave")
        self.deinterleave.connect('pad-added', self.on_deinterleave)
        self.audiosink = Gst.ElementFactory.make("autoaudiosink", "audiosink")

        
        self.pipeline.add(self.appsrc)
        self.pipeline.add(self.demux)
        self.pipeline.add(self.vqueue)
        self.pipeline.add(self.mpegvideoparse)
        self.pipeline.add(self.mpeg2dec)
        if self.useGL:
            self.pipeline.add(self.glsinkbin)
        else:
            self.pipeline.add(self.videoconvert)
            self.pipeline.add(self.videosink)
        self.pipeline.add(self.aqueue)
        self.pipeline.add(self.faad)
        if self.audio != 2:
            self.pipeline.add(self.deinterleave)

        self.pipeline.add(self.audiosink)

        self.appsrc.link(self.demux)
        
        self.vqueue.link(self.mpegvideoparse)
        self.mpegvideoparse.link(self.mpeg2dec)
        if self.useGL:
            self.mpeg2dec.link(self.glsinkbin)
        else:
            self.mpeg2dec.link(self.videoconvert)
            self.videoconvert.link(self.videosink)
        
        self.aqueue.link(self.faad)
        if self.audio != 2:
            self.faad.link(self.deinterleave)
        else:
            self.faad.link(self.audiosink)

        
        self.bus = self.pipeline.get_bus()
        self.bus.add_signal_watch()

    def on_demux(self, demux, pad):
        print(pad.get_name())
        name_template = pad.get_property("template").name_template
        print(name_template)
        if name_template == "video_%04x":
            vqueue_pad = self.vqueue.get_static_pad("sink")
            print("before link")
            pad.link(vqueue_pad)
            print("after link")
        if name_template == "audio_%04x":
            aqueue_pad = self.aqueue.get_static_pad("sink")
            pad.link(aqueue_pad)

    def on_deinterleave(self, deinterleave, pad):
        print(pad.get_name())
        if (pad.get_name() == 'src_0' and self.audio == 0) or \
           (pad.get_name() == 'src_1' and self.audio == 1):
            pad.link(self.audiosink.get_static_pad("sink"))
            

    def on_source_need_data(self, source, length):
        print('need-data')
        for i in range(0, 200):
            retval = self.driver.get_ts_stream(self.b25)
            if retval.status:
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

    def set_state(self, state):
        self.pipeline.set_state(state)
        
        
class TvWindow(Gtk.Window):
    def __init__(self, ch, gst_widget):
        Gtk.Window.__init__(self)

        settings = Gtk.Settings.get_default()
        settings.props.gtk_application_prefer_dark_theme = True

        self.hbar = Gtk.HeaderBar()
        self.hbar.set_show_close_button(True)
        self.set_titlebar(self.hbar)

        fullbutton = Gtk.Button.new()
        image_full = Gtk.Image.new_from_icon_name("view-fullscreen-symbolic", \
                                                  Gtk.IconSize.MENU)
        fullbutton.set_image(image_full)
        fullbutton.connect("clicked", self.on_fullscreen_button_clicked)
        self.hbar.pack_end(fullbutton)

        
        self.vbox = Gtk.Box.new(Gtk.Orientation.VERTICAL, 0)

        self.hbox = Gtk.Box.new(Gtk.Orientation.HORIZONTAL, 0)
        adjust = Gtk.Adjustment.new(ch, 0, 100, 1, 10, 10)
        self.spin = Gtk.SpinButton.new(adjust, 1, 0)
        self.btn = Gtk.Button.new_with_label("Set Channel")
        self.pn_combo = Gtk.ComboBox.new();
        renderer = Gtk.CellRendererText()
        self.pn_combo.pack_start(renderer, True)
        self.pn_combo.add_attribute(renderer, 'text', 1)
        self.audiobtn = Gtk.Button.new_with_label("audio both")
        self.audiobtn0 = Gtk.Button.new_with_label("audio 0")
        self.audiobtn1 = Gtk.Button.new_with_label("audio 1")
        self.hbox.pack_start(self.spin, False, False, 0)
        self.hbox.pack_start(self.btn, False, False, 0)
        self.hbox.pack_start(self.pn_combo, False, False, 0)
        self.hbox.pack_start(self.audiobtn, False, False, 0)
        self.hbox.pack_start(self.audiobtn0, False, False, 0)
        self.hbox.pack_start(self.audiobtn1, False, False, 0)
        self.vbox.pack_start(self.hbox, False, True, 0)

        self.vbox.pack_start(gst_widget, True, True, 0)

        self.add(self.vbox)

        self.resize(480, 400)
        self.show_all()

    def on_fullscreen_button_clicked(self, button):
        print("fullscreen_button_clicked")
        print(button)
        self.fullscreen()

    def fullscreen(self):
        self.hbox.hide()
        Gtk.Window.fullscreen(self)
    def unfullscreen(self):
        self.hbox.show()
        Gtk.Window.unfullscreen(self)


class TvController():
    isReplacingWidget = False
    
    def __init__(self, player, driver, b25, spaceNo, ch):
        self.player = player
        self.driver = driver
        self.b25 = b25
        self.spaceNo = spaceNo
        self.ch = ch
        self.pn = -1;
        #if isinstance(Gdk.Display.get_default(), GdkX11.X11Display):
        #    print("x11")
        #    self.gst = TvGst(False, self.driver, self.b25)
        #else:
        #    print("wayland")
        #    self.gst = TvGst(True, self.driver, self.b25)
        self.gst = TvGst(False, driver, b25)
        self.init_gst()
        self.window = TvWindow(ch, self.gst_widget)
        self.window.btn.connect("clicked", self.on_set_channel_button_clicked)
        self.window.pn_combo.connect("changed", self.on_program_number_changed)
        self.window.audiobtn.connect("clicked", self.on_audio_button_clicked)
        self.window.audiobtn0.connect("clicked", self.on_audio0_button_clicked)
        self.window.audiobtn1.connect("clicked", self.on_audio1_button_clicked)

    def init_gst(self):
        self.gst.init()
        if self.pn > 0:
            self.gst.demux.set_property("program_number", self.pn)
        self.gst.demux.connect("pad-removed", self.on_demux_pad_removed)
        self.gst.bus.connect("message", self.on_message)
        self.gst_widget = self.gst.videosink.get_property('widget');
        self.gst_widget.connect("button-press-event", \
                                       self.on_button_pressed)


    def play(self):
        self.gst.set_state(Gst.State.PLAYING)
    def stop(self):
        self.gst.set_state(Gst.State.NULL)

    def replace_widget(self):
        if self.isReplacingWidget:
            return
        self.isReplacingWidget = True
        self.window.vbox.remove(self.gst_widget)
        self.init_gst()
        self.window.vbox.pack_start(self.gst_widget, True, True, 0)
        self.gst_widget.show()
        self.isReplacingWidget = False;

    def on_set_channel_button_clicked(self, button):
        ch = self.window.spin.get_value_as_int()
        self.stop()
        #self.pn = -1
        self.replace_widget()
        
        try:
            self.gst.driver.set_channel2(self.spaceNo, ch)
        except GLib.GError as e:
            print("%s: %d: %s" % (e.domain , e.code , e.message))
            return
        #self.gst.pipeline.set_state(Gst.State.PAUSED)
        self.play()

    def restart_gst(self):
        self.stop()
        self.replace_widget()
        self.play()

    def on_demux_pad_removed(self, demux, old_pad):
        print("pad-removed")
        if self.isReplacingWidget:
            return
        GObject.idle_add(self.restart_gst)
        
    def on_program_number_changed(self, combo):
        pn = int(combo.get_model()[combo.get_active()][0])
        if pn == self.pn:
            return
        self.pn = pn
        print("**********************************")
        print(self.pn)
        self.stop()
        #self.replace_widget()
        self.gst.demux.set_property("program_number", self.pn)
        self.play()

    def add_program_number_combobox(self, ps):
        pn_store = Gtk.ListStore(int, str)
        selectedIndex = 0;
        index = 0;
        for p in ps:
            if p.program_number != 0:
                print(index)
                print(p.program_number)
                if p.program_number == self.pn:
                    print("%d %d %d" % (p.program_number, self.pn, index))
                    selectedIndex = index
                pn_store.append([int(p.program_number), str(p.program_number)])
                index += 1
        self.window.pn_combo.set_model(pn_store)
        self.window.pn_combo.set_active(selectedIndex)

    def on_message(self, bus, msg):
        if msg.type == Gst.MessageType.ELEMENT:
            sec = GstMpegts.message_parse_mpegts_section(msg)
            if sec.section_type == GstMpegts.SectionType.PAT:
                pats = sec.get_pat()
                for pat in pats:
                    print("program number: %d, network_or_program_map_PID: 0x%04x" % (pat.program_number, pat.network_or_program_map_PID))
                self.add_program_number_combobox(pats)

    def on_button_pressed(self, window, event):
        print("button pressed")
        print(window)
        print(dir(event))
        print(event.get_click_count())
        print(event.type)
        print(event.button)
        click_count = event.get_click_count()
        if (event.button == 1 and click_count[1] == 2):
            self.window.unfullscreen()

    def on_audio_button_clicked(self, button):
        self.gst.audio = 2
        self.restart_gst()

    def on_audio0_button_clicked(self, button):
        self.gst.audio = 0
        self.restart_gst()

    def on_audio1_button_clicked(self, button):
        self.gst.audio = 1
        self.restart_gst()
 

class TvPlayer(Gtk.Application):
    def __init__(self, driver, b25, spaceNo, ch):
        Gtk.Application.__init__(self, flags = Gio.ApplicationFlags.FLAGS_NONE)
        self.driver = driver
        self.b25 = b25
        self.spaceNo = spaceNo
        self.ch = ch
        self.connect("startup", self.on_startup)
        self.connect("activate", self.on_activate)

    def on_activate(self, player):
        print("activate")
        self.controller.play()
        
    def on_startup(self, player):
        print("startup")

        self.controller = TvController(self, self.driver, self.b25, \
                               self.spaceNo, self.ch)
        self.add_window(self.controller.window)

        self.cookie = Gtk.Application.inhibit(self, None,
                                         Gtk.ApplicationInhibitFlags.IDLE,
                                         "Playing TV");
def main():
    if len(sys.argv) < 4:
        print("Usage: %s bondriver spaceNo ch" % sys.argv[0])
        exit(1)

    bondriver = sys.argv[1]
    spaceNo = int(sys.argv[2])
    ch = int(sys.argv[3])
            
    cookie = None
    
    driver = GBon.Driver.new()
    b25 = None

    Gst.init()
    
    player = None
    
    try:
        if GBon.B25.is_enabled():
            b25 = GBon.B25.new()
            print("b25 startup")
            b25.startup(4, False, False)
            
        print("load module")
        driver.load_module(bondriver)
    
        print("create driver")
        driver.create_driver()

        print("open tuner")
        driver.open_tuner()
    
        #time.sleep(1)
        print("set channel2 %d, %d" % (spaceNo, ch))
        driver.set_channel2(spaceNo, ch)

        print(driver.get_tuner_name())
        print(driver.enum_tuning_space(spaceNo))
        print(driver.enum_channel_name(spaceNo, ch))

        sl = driver.get_signal_level()
        print("signal level: %f" % sl)

        player= TvPlayer(driver, b25, spaceNo, ch)
        player.run()
    except GLib.GError as e:
        print("%s: %d: %s" % (e.domain , e.code , e.message))
        if player:
            player.controller.stop()
        driver.close_tuner();
        driver.release();
        driver.close_module();
        exit(-e.code)
    except KeyboardInterrupt:
        print("KeyboardInterrup")
        pass
    if player:
        player.controller.stop()
    player = None
    b25 = None
    print("close tuner")
    driver.close_tuner()
    print("release driver")
    driver.release()
    print("close module")
    driver.close_module()
    driver = None

if __name__ == "__main__":
    main()
