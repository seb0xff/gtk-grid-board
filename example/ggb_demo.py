import gi

## Uncomment the following lines if you don't want to install the library
# from os import path as p
# TYPELIB_DIR_ABS = p.abspath(
#     p.realpath(p.join(p.dirname(__file__), '..', 'build')))

# gi.require_version('GIRepository', '2.0')
# from gi.repository import GIRepository

# repo = GIRepository.Repository.get_default()
# repo.prepend_search_path(TYPELIB_DIR_ABS)
# repo.prepend_library_path(TYPELIB_DIR_ABS)

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
      background-color: rgb(30,32,48); 
    }
    .grid-board {
      color: rgb(245,169,127);
    }
    .grid-board #guidelines {
      color: rgb(47,51,72);
    }
    """
    css_provider = Gtk.CssProvider()
    css_provider.load_from_data(css.encode())
    display = Gdk.Display.get_default()
    Gtk.StyleContext.add_provider_for_display(
        display, css_provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)

    grid = Ggb.Grid(cols_num=10, rows_num=10, cell_radius=10)
    grid.set_at(4, 4, True, False)
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
