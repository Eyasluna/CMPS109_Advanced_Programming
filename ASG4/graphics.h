// $Id: graphics.h,v 1.1 2015-07-16 16:47:51-07 - - $

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <memory>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "rgbcolor.h"
#include "shape.h"


class object {
   private:
      shared_ptr<shape> pshape;
      vertex center;
      rgbcolor color;
   public:
      // Default copiers, movers, dtor all OK.
      object(shared_ptr<shape>& pshape,vertex& center,rgbcolor& color)
      {
            this->pshape = pshape;
            this->center = center;
            this->color = color;
      }
      void draw() { pshape->draw (center, color); }
      void draw_border(rgbcolor border_colors,float border_wigth) 
      { 
            pshape->draw_border (center, border_colors, border_wigth);  
      }
      void move (GLfloat delta_x, GLfloat delta_y) {
         center.xpos += delta_x;
         center.ypos += delta_y;
      }
      vertex getcenter(){ return center; }
};

class mouse {
      friend class window;
   private:
      int xpos {0};
      int ypos {0};
      int entered {GLUT_LEFT};
      int left_state {GLUT_UP};
      int middle_state {GLUT_UP};
      int right_state {GLUT_UP};
   private:
      void set (int x, int y) { xpos = x; ypos = y; }
      void state (int button, int state);
      void draw();
};


class window {
      friend class mouse;
   private:
      static int width;         // in pixels
      static int height;        // in pixels
      static int moveby;            // in pixels
      
      static vector<object> objects;
      static size_t selected_obj;
      static mouse mus;
   private:
      static void close();
      static void entry (int mouse_entered);
      static void display();
      static void reshape (int width, int height);
      static void keyboard (GLubyte key, int, int);
      static void special (int key, int, int);
      static void motion (int x, int y);
      static void passivemotion (int x, int y);
      static void mousefn (int button, int state, int x, int y);
   public:
     static rgbcolor border_colors;
      static float border_wigth;
      static void push_back (const object& obj) {
                  objects.push_back (obj); }
      static void setwidth (int width_) { width = width_; }
      static void setheight (int height_) { height = height_; }
      static void setmoveby (int moveby_) { moveby = moveby_; }
       static int getwidth () { return width; }
       static int getheight () { return height; }
      static void select_object (int n) 
      {
            if(n >= 0 && n < objects.size())
            {
                  selected_obj = n;
            }

                  
      }
      static void move_selected_object (int mx, int my)
      {

            object tempobj = objects.at(selected_obj);
            vertex tempver = tempobj.getcenter();

            if(tempver.xpos + mx * moveby < 0)
                  tempver.xpos = width - tempver.xpos;
            else if(tempver.xpos + mx * moveby > width)
                  tempver.xpos = -tempver.xpos;
            else 
                  tempver.xpos = mx * moveby;

            if(tempver.ypos + my *moveby < 0)
                  tempver.ypos = height - tempver.ypos;
            else if(tempver.ypos + my *moveby > height)
                  tempver.ypos = -tempver.ypos;
            else 
                  tempver.ypos = my * moveby;

            objects.at(selected_obj).move(tempver.xpos, tempver.ypos);
            // objects.at(selected_obj).move(mx,my);
      }
      static void main();
};
#endif

