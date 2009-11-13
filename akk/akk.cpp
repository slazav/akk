#include <iostream>
#include <gtkmm.h>
#include <math.h>

#include <string>

const int frets   = 12;
const int strings = 6;
const double bull_size=0.4;
const double fing_size=0.9;

const char str_values[strings] = {4,9,2,7,11,4};

struct chord{
  gboolean data[12];
  char base;
  chord(){
    base=0;
    for (int i=0;i<12;i++) data[i]=false;
  }
  chord(char base_, char a0, char a1, char a2, char a3,
                    char a4, char a5, char a6, char a7,
                    char a8, char a9, char aa, char ab){
    base=base_;
    data[0]=a0; data[1]=a1; data[2]=a2;  data[3]=a3;
    data[4]=a4; data[5]=a5; data[6]=a6;  data[7]=a7;
    data[8]=a8; data[9]=a9; data[10]=aa; data[11]=ab;
  }
};

chord str2chord(std::string str){
    char base;
    if (str.size()<1) return chord();
    switch (str[0]){
	case 'C': base=0;  break;
	case 'D': base=2;  break;
	case 'E': base=4;  break;
	case 'F': base=5;  break;
	case 'G': base=7;  break;
	case 'A': base=9;  break;
	case 'B': base=10; break;
	case 'H': base=11; break;
	default: return chord();
    }
    str.erase(0,1);
    if (str.size()>0){ 
        if (str[0] == '#') {base++; str.erase(0,1);}
	if (str[0] == 'b') {base--; str.erase(0,1);}
    }
    while (base < 0)   base+=12;
    while (base >= 12) base-=12;

    if (str == "")      return chord(base, 1,0,0,0,1,0,0,1,0,0,0,0);
    if (str == "m")     return chord(base, 1,0,0,1,0,0,0,1,0,0,0,0);
    if (str == "sus4")  return chord(base, 1,0,0,0,0,1,0,1,0,0,0,0);
    if (str == "+5")    return chord(base, 1,0,0,0,1,0,0,0,1,0,0,0);
    if (str == "-5")    return chord(base, 1,0,0,0,1,0,1,0,0,0,0,0);
    if (str == "m+5")   return chord(base, 1,0,0,1,0,0,0,0,1,0,0,0);
    if (str == "m-5")   return chord(base, 1,0,0,1,0,0,1,0,0,0,0,0);

    if (str == "7")     return chord(base, 1,0,0,0,1,0,0,1,0,0,1,0);
    if (str == "m7")    return chord(base, 1,0,0,1,0,0,0,1,0,0,1,0);
    if (str == "maj")   return chord(base, 1,0,0,0,1,0,0,1,0,0,0,1);
    if (str == "maj7")  return chord(base, 1,0,0,0,1,0,0,1,0,0,0,1);
    if (str == "+7")    return chord(base, 1,0,0,0,1,0,0,1,0,0,0,1);
    if (str == "mmaj")  return chord(base, 1,0,0,1,0,0,0,1,0,0,0,1);
    if (str == "mmaj7") return chord(base, 1,0,0,1,0,0,0,1,0,0,0,1);
    if (str == "m+7")   return chord(base, 1,0,0,1,0,0,0,1,0,0,0,1);
    if (str == "-57")   return chord(base, 1,0,0,0,1,0,1,0,0,0,1,0);
    if (str == "m-57")  return chord(base, 1,0,0,1,0,0,1,0,0,0,1,0);
    if (str == "+57")   return chord(base, 1,0,0,0,1,0,0,0,1,0,1,0);
    if (str == "+")     return chord(base, 1,0,0,0,1,0,0,0,1,0,1,0);

    if (str == "dim")   return chord(base, 1,0,0,1,0,0,1,0,0,1,0,0);
    if (str == "o")     return chord(base, 1,0,0,1,0,0,1,0,0,1,0,0);
    if (str == "m-5-7") return chord(base, 1,0,0,1,0,0,1,0,0,1,0,0);

    if (str == "9")     return chord(base, 1,0,1,0,1,0,0,1,0,0,1,0);
    if (str == "m9")    return chord(base, 1,0,1,1,0,0,0,1,0,0,1,0);
    if (str == "add9")  return chord(base, 1,0,1,0,1,0,0,1,0,0,0,0);
    if (str == "-9")    return chord(base, 1,1,0,0,1,0,0,1,0,0,1,0);
    if (str == "m-9")   return chord(base, 1,1,0,1,0,0,0,1,0,0,1,0);
return chord();
}


gshort chord_colors[12*3] = {
  0x0000,0x0000,0x0000, // 1
  0x7FFF,0x7FFF,0x0000, // -9
  0x7FFF,0x7FFF,0x0000, // 9
  0x0000,0xFFFF,0x0000, // -3
  0x7FFF,0xFFFF,0x0000, // 3
  0x0000,0xFFFF,0x7FFF, // 4
  0xFFFF,0x7FFF,0x0000, // -5
  0xFFFF,0x0000,0x0000, //  5
  0xFFFF,0x0000,0x7FFF, // +5
  0x7FFF,0x0000,0xFFFF, // -7
  0x0000,0x0000,0xFFFF, //  7
  0x0000,0x7FFF,0xFFFF, // +7
};

std::string chord_o;

gboolean draw_chord(Gtk::Widget *entry, GdkEventExpose *event, Gtk::Widget *drawing) {
//void draw_chord(Gtk::Widget *entry, const Glib::ustring& str1, Gtk::Widget *drawing) {

    std::string chord_n = ((Gtk::Entry *)entry)->get_text();
    if ((chord_n!="")&&(chord_n == chord_o)) return TRUE;
    chord c = str2chord(chord_n);


    int xm = drawing->get_allocation().get_width();
    int ym = drawing->get_allocation().get_height();

    double sum=0;
    for (int i=0; i<frets; i++) sum+=1.0/pow(2, i/12.0);
    double l0 = ym/(sum+2);

    int top_space  = (int)l0;
    int bot_space  = (int)l0;
    int side_space = xm/(strings+1);
    int string_space = xm/(strings+1);
    
    int r = (int)(l0/4);
    if (r > string_space/2) r = string_space/2;  
    int r1 = (int)(r*bull_size);


    Glib::RefPtr<Gdk::GC> gc = drawing->get_style()->get_fg_gc(drawing->get_state());

    drawing->get_window()->clear();
    gc->set_rgb_fg_color(Gdk::Color("black"));

    // strings
    for (int i=0; i<strings; i++){
      gc->set_line_attributes((strings-i+1)/2, Gdk::LINE_SOLID, Gdk::CAP_BUTT, Gdk::JOIN_MITER);
      drawing->get_window()->draw_line (gc, 
	side_space+string_space*i, top_space, 
	side_space+string_space*i, ym-bot_space);
    }
    sum=0;
    for (int i=0; i<=frets; i++){
      drawing->get_window()->draw_line (gc, 
	side_space,                          top_space+(int)(sum*l0), 
	side_space+string_space*(strings-1), top_space+(int)(sum*l0));
      if ((i==5)||(i==7)||(i==10)||(i==12)){
	int cx = side_space+string_space*(strings-1)/2;
	int cy = top_space+(int)((sum-0.5/pow(2, (i-1)/12.0))*l0);
        drawing->get_window()->draw_arc (gc, FALSE, cx-r1,cy-r1,2*r1,2*r1,0,64*360);
      }
      sum+=1.0/pow(2, i/12.0);
    }

    int r2 = (int)(r*fing_size);


//    for (int i=0;i<12;i++) std::cerr << (int)c.data[i] << " ";
//    std::cerr << "\n";

//    Glib::RefPtr<Gdk::Colormap> colormap = drawing->get_default_colormap();

    for (int s=0; s<strings; s++){
      sum=0;
      for (int f=0; f<=frets; f++){
        int cx = side_space+string_space*s;
        int cy = top_space+(int)((sum-0.5/pow(2, (f-1)/12.0))*l0);
        int st = (12+str_values[s]+f-c.base)%12;
        if (c.data[st]){
	  Gdk::Color col;
	  col.set_rgb(chord_colors[3*st], chord_colors[3*st+1], chord_colors[3*st+2]);
          gc->set_rgb_fg_color(col);
	  drawing->get_window()->draw_arc (gc, TRUE, cx-r,cy-r,2*r,2*r,0,64*360);
        }
        sum+=1.0/pow(2, f/12.0);
      }
    }
    gc->set_rgb_fg_color(Gdk::Color("black"));

    chord_o = chord_n;
    return TRUE;
}

gboolean redraw (Gtk::Widget *entry, GdkEventExpose *event, Gtk::Widget *drawing){
  chord_o="";
  draw_chord(entry, event, drawing);
  return TRUE;
}


gboolean
on_delete (GdkEventAny * e, Gtk::Window * win){
  win->hide();
  return TRUE;
}


main (int argc, char **argv){
  Gtk::Main app (argc, argv);
  // main window
  Gtk::Window win (Gtk::WINDOW_TOPLEVEL);

  win.set_default_size(0,480);

  win.signal_delete_event().connect 
    (sigc::bind (sigc::ptr_fun (&on_delete), &win));

  //drawing area
  Gtk::DrawingArea drawing;

  // text entry
  Gtk::Entry text_entry;
  text_entry.signal_expose_event().connect 
    (sigc::bind<0> (sigc::bind<2>(sigc::ptr_fun (&draw_chord), &drawing), &text_entry));

  drawing.signal_expose_event().connect 
    (sigc::bind<0> (sigc::bind<2>(sigc::ptr_fun (&redraw), &drawing), &text_entry));

  // vbox
  Gtk::VBox vbox;
  vbox.pack_start(text_entry,   false, false, 0);
  vbox.pack_start(drawing,  true, true, 0);

  win.add (vbox);
  win.show_all ();
  app.run (win);

  return 0;
}
