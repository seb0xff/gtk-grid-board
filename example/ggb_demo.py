from os import path as p
import gi

TYPELIB_DIR_ABS = p.abspath(
    p.realpath(p.join(p.dirname(__file__), '..', 'build')))

gi.require_version('GIRepository', '2.0')
from gi.repository import GIRepository

repo = GIRepository.Repository.get_default()
repo.prepend_search_path(TYPELIB_DIR_ABS)
repo.prepend_library_path(TYPELIB_DIR_ABS)

gi.require_version('Gtk', '4.0')
gi.require_version('Gdk', '4.0')
gi.require_version('Adw', '1')
gi.require_version('Ggb', '0.1')

from gi.repository import Gtk, Gdk, Adw, Ggb


class MainWindow(Gtk.ApplicationWindow):

  def __init__(self, *args, **kwargs):
    super().__init__(*args, **kwargs)

    self.set_default_size(720, 720)
    self.set_resizable(False)
    self.set_title('GTK Grid Board Demo')
    css = """
    .background {
      background-color: rgb(36,0,36);
    }

    .view {
      background-color: rgba(36,0,36,0.7);
    }"""
    css_provider = Gtk.CssProvider()
    css_provider.load_from_data(css.encode())
    display = Gdk.Display.get_default()
    # Gtk.StyleContext.add_provider_for_display(
    #     display, css_provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)

    grid = Ggb.Grid(cols_num=10, rows_num=10, accent_guideline_repeat=5)
    # grid.set_size(10, 10)
    self.set_child(grid)


class MyApp(Adw.Application):

  def __init__(self, **kwargs):
    super().__init__(**kwargs)
    self.connect('activate', self.on_activate)

  def on_activate(self, app):
    self.win = MainWindow(application=app)
    self.win.present()


app = MyApp()
app.run()
