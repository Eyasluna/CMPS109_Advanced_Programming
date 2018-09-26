// $Id: shape.cpp,v 1.1 2015-07-16 16:47:51-07 - - $

#include <typeinfo>
#include <unordered_map>
#include <math.h>
using namespace std;

#include "shape.h"
#include "util.h"

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font, const string& textdata):
      glut_bitmap_font(glut_bitmap_font), textdata(textdata) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


polygon::polygon (vertex_list vertices): vertices(vertices) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
            polygon({}) {
   vertex_list ver_list;

   vertex vertex1 { width/2, height/2 };
   ver_list.push_back(vertex1);
   vertex vertex2 { -width/2, height/2 };
   ver_list.push_back(vertex2);
   vertex vertex3 { -width/2, -height/2 };
   ver_list.push_back(vertex3);
   vertex vertex4 { width/2, -height/2 };
   ver_list.push_back(vertex4);
   vertices = ver_list;

   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

diamond::diamond (GLfloat width, GLfloat height): polygon({}) {
   vertex_list ver_list;

   vertex vertex1 { 0, height/2 };
   ver_list.push_back(vertex1);
   vertex vertex2 { -width/2, 0 };
   ver_list.push_back(vertex2);
   vertex vertex3 { 0, -height/2 };
   ver_list.push_back(vertex3);
   vertex vertex4 { width/2, 0 };
   ver_list.push_back(vertex4);
   vertices = ver_list;

   DEBUGF ('c', this << "(" << width << "," << height << ")");
}




triangle::triangle (vertex_list vertices): polygon(vertices) {
   DEBUGF ('c', this);
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

equilateral::equilateral (GLfloat length): triangle ({}) {
   DEBUGF ('c', this);

   vertex_list ver_list;
   vertex vertex1 { 0, static_cast<GLfloat>(sqrt(3.0) / 3 * length) };
   ver_list.push_back(vertex1);
   vertex vertex2 { -length/2,  static_cast<GLfloat>(-sqrt(3.0) / 3 * length) };
   ver_list.push_back(vertex2);
   vertex vertex3 { length/2,  static_cast<GLfloat>(-sqrt(3.0) / 3 * length) };
   ver_list.push_back(vertex3);
   vertices = ver_list;
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   void* font = glut_bitmap_font;
      glLoadIdentity();
      glColor3ubv (color.ubvec);
      glRasterPos2i (center.xpos, center.ypos);
      auto ubytes11 = reinterpret_cast<const GLubyte*>
                    (textdata.c_str());
      glutBitmapString (font, ubytes11);
}

void text::draw_border (const vertex& center, const rgbcolor& color, const float width) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
    float PI = 3.1415926;
    int n = (dimension.xpos + dimension.ypos) * 10;
    glColor3ubv (color.ubvec);
    glLoadIdentity();
    glTranslatef(center.xpos, center.ypos, 0);
    glBegin(GL_POLYGON);
    for(int i = 0; i < n; i++)  
        glVertex2f(dimension.xpos * cos(2 * PI / n * i), dimension.ypos * sin(2 * PI / n * i));  
    glEnd();  
    glFlush();
    glutSwapBuffers();
}

void ellipse::draw_border (const vertex& center, const rgbcolor& color, const float width) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   float PI = 3.1415926;
    int n = (dimension.xpos + dimension.ypos) * 10;
    glColor3ubv (color.ubvec);
    glLineWidth(width);
    glLoadIdentity();
    glTranslatef(center.xpos, center.ypos, 0);
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < n; i++)  
        glVertex2f(dimension.xpos * cos(2 * PI / n * i), dimension.ypos * sin(2 * PI / n * i));  
    glEnd();  
    glFlush();
    glutSwapBuffers();
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glColor3ubv (color.ubvec);
   glLoadIdentity();
   glTranslatef(center.xpos, center.ypos, 0);
   glBegin(GL_POLYGON);
   for(unsigned int i = 0; i < vertices.size(); i++)
      glVertex2f(vertices.at(i).xpos,vertices.at(i).ypos);
   glEnd();
   glFlush();
   glutSwapBuffers();
}

void polygon::draw_border (const vertex& center, const rgbcolor& color, const float width) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glColor3ubv (color.ubvec);
   glLineWidth(width);
   glLoadIdentity();
   glTranslatef(center.xpos, center.ypos, 0);
   glBegin(GL_LINE_LOOP);
   for(unsigned int i = 0; i < vertices.size(); i++)
      glVertex2f(vertices.at(i).xpos,vertices.at(i).ypos);
   glEnd();
   glFlush();
   glutSwapBuffers();
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

