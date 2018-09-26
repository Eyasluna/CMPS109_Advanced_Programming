// $Id: interp.cpp,v 1.2 2015-07-16 16:57:30-07 - - $

#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
   {"define"   , &interpreter::do_define   },
   {"draw"     , &interpreter::do_draw     },
   {"border"   , &interpreter::do_border   },
   {"moveby"   , &interpreter::do_moveby   },
};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
   {"text"       , &interpreter::make_text       },
   {"ellipse"    , &interpreter::make_ellipse    },
   {"circle"     , &interpreter::make_circle     },
   {"polygon"    , &interpreter::make_polygon    },
   {"rectangle"  , &interpreter::make_rectangle  },
   {"square"     , &interpreter::make_square     },
   {"diamond"    , &interpreter::make_diamond    },
   {"triangle"   , &interpreter::make_triangle   },
   {"equilateral", &interpreter::make_equilateral},
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.emplace (name, make_shape (++begin, end));
}


void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   rgbcolor color {begin[0]};
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};

   object obj(objmap.find(name)->second,where,color);
   window::push_back(obj);
//    itor->second->draw (where, color);
}

void interpreter::do_border (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   rgbcolor color {begin[0]};
   window::border_colors = color;
   window::border_wigth = from_string<float>(begin[1]);
}

void interpreter::do_moveby (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   window::setmoveby(from_string<int>(*begin));
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string font = *begin++;
   string data = *begin++;
   for(int i = 1; begin + i != end; i++)
   {
           data += " ";
           data += *(begin+i);
   }
   return make_shared<text> (fontcode.find(font)->second,data);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<ellipse> (from_string<GLfloat> (begin[0]), from_string<GLfloat> (begin[1]));
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<circle> (from_string<GLfloat> (begin[0]));
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list ver_list;
   for (int i = 0; begin + i != end; i += 2)
   {
        vertex tempver {from_string<GLfloat> (begin[i]),
                from_string<GLfloat> (begin[i+1])};
        ver_list.push_back(tempver);
   }
   return make_shared<polygon> (ver_list);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<rectangle> (from_string<GLfloat> (begin[0]), from_string<GLfloat> (begin[1]));
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<square> (from_string<GLfloat> (begin[0]));
}

shape_ptr interpreter::make_diamond (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<diamond> (from_string<GLfloat> (begin[0]), from_string<GLfloat> (begin[1]));
}

shape_ptr interpreter::make_triangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 6) throw runtime_error ("syntax error");
   vertex_list ver_list;
   for (int i = 0; begin + i != end; i += 2)
   {
        vertex tempver {from_string<GLfloat> (begin[i]),
                from_string<GLfloat> (begin[i+1])};
        ver_list.push_back(tempver);
   }
   return make_shared<triangle> (ver_list);
}

shape_ptr interpreter::make_equilateral (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<equilateral> (from_string<GLfloat> (begin[0]));
}

