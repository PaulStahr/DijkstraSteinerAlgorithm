/*******************************************************************************
 * Copyright (c) 2019 Paul Stahr
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include "dijkstra_steiner.h"

#include "application_window.h"

bool pollEvents()
{
  SDL_Event event;

  while( SDL_PollEvent(&event) ) // Nächste Nachricht holen
  {
    switch(event.type)
    {
      case SDL_QUIT:
        return false; // Programm sollte beendet werden
      default:
        break;
    }
  }

  return true;
}

void draw_tube(float r, float g, float b)
{
   size_t resolution = 64;
   glBegin(GL_TRIANGLE_STRIP);
   double mult = 2 * M_PI / resolution;
   for (size_t i = 0; i <= resolution; ++i)
   { 
        glColor3f(r * (sin(i * mult) * 0.5 + 0.5), g * (sin(i * mult) * 0.5 + 0.5), b * (sin(i * mult) * 0.5 + 0.5));
        glVertex3f(sin(i * mult), cos(i * mult), -1);
        glVertex3f(sin(i *mult), cos(i * mult), 1);
   }
   glEnd();
}

void draw_sphere(float r, float g, float b)
{
   size_t resolution = 16;
   double mult = M_PI / resolution;
   for (size_t i = 0; i <= resolution; ++i)
   { 
      glBegin(GL_TRIANGLE_STRIP);
      for (size_t j = 0; j <= resolution * 2; ++j)
      {
         glColor3f(r, g * (cos(j * mult) * 0.5 + 0.5), b * ( sin(i * mult) * 0.5 + 0.5 ));
         glVertex3f(sin(j * mult) * sin(i * mult), cos(j * mult) * sin(i * mult), cos(i * mult));
         glVertex3f(sin(j * mult) * sin((i + 1) * mult), cos(j * mult) * sin((i + 1) * mult), cos((i + 1) * mult));
      }
      glEnd();
   }
}

template <typename T>
struct vertex_coordinate_3d{
   T x;
   T y;
   T z;
};

template <typename T>
struct tube_coordinate_3d{
   vertex_coordinate_3d<T> midpoint;
   T length;
   uint8_t direction;
};

int main(int , char **)
{
    if( SDL_Init( SDL_INIT_VIDEO ) != 0 )
    {
        std::cout << "Can't load sld " << SDL_GetError() << ')' << std::endl;
        return 1;
    }

    GL_UTIL::GlWindow game_window( 1366, 768, 32 );
    double aspect = static_cast<double>(game_window.get_height()) / game_window.get_width();
    game_window.setCaption( "Steinertrees" );
    game_window.setFullscreen(false);
    game_window.create();

    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glEnable( GL_DEPTH_TEST );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    double clipping_size = 160;
    glFrustum( -clipping_size, clipping_size, -clipping_size * aspect, clipping_size * aspect, 200, 10000000 );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0,0,-800000);
    glRotatef(60,1,1,0);
    std::vector<std::vector<COOR> > terminal_coordinates;
    std::vector<std::vector<COOR> > terminal_speed;
    for (size_t i = 0; i < 8; ++i)
    {
        std::vector<COOR> coor = {rand() % 10, rand() % 10, rand() % 10};
        terminal_coordinates.push_back(coor);
        terminal_speed.push_back({0,0,0});
    }
    
    double terminal_radius = 16000;
    double steiner_radius = 10000;
    double eckpoint_radius = 5000;
    double connection_radius = 5000;

    std::vector<std::vector<COOR> > steinerpoint_coordinates;
    std::vector<std::pair<size_t, size_t> > edges;

    dijkstra_steiner_settings settings;
    settings._small_memory_mode = false;  //deletes object when possible, less memory use but higher runtime
    settings._maximum_heap_width = 20;    //lower values can result in less memory consumption but increase runtime
    settings._edge_as_steinerpoint = false;

    while(true)
    {
        if( !pollEvents() ) break;

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glRotatef(0.01,0,0,1);

        
        for (size_t i = 0; i < terminal_coordinates.size(); ++i)
        {
            terminal_speed[i][0] += (rand() % 101) - 50 - terminal_coordinates[i][0] / 30000 - terminal_speed[i][0] / 1500;
            terminal_speed[i][1] += (rand() % 101) - 50 - terminal_coordinates[i][1] / 30000 - terminal_speed[i][1] / 1500;
            terminal_speed[i][2] += (rand() % 101) - 50 - terminal_coordinates[i][2] / 30000 - terminal_speed[i][2] / 1500;
            terminal_coordinates[i][0] += terminal_speed[i][0]; 
            terminal_coordinates[i][1] += terminal_speed[i][1];
            terminal_coordinates[i][2] += terminal_speed[i][2];
        }
            
        calculate_steinertree(*boundingbox_lower_bound, terminal_coordinates, settings, steinerpoint_coordinates, edges);
        
        for (size_t i = 0; i < terminal_coordinates.size(); ++i)
        {
            glPushMatrix();
            std::vector<int> const & coor = terminal_coordinates[i];
            glTranslatef(coor[0], coor[1], coor[2]);
            glScalef(terminal_radius, terminal_radius, terminal_radius);
            draw_sphere(0, 1, 1);
            glPopMatrix();
        }
        for (size_t i = 0; i < steinerpoint_coordinates.size(); ++i)
        {
            glPushMatrix();
            std::vector<int> const & coor = steinerpoint_coordinates[i];
            glTranslatef(coor[0], coor[1], coor[2]);
            glScalef(steiner_radius, steiner_radius, steiner_radius);
            draw_sphere(1, 1, 0);
            glPopMatrix();
        }
        for (size_t i = 0; i < edges.size(); ++i)
        {
            std::vector<COOR> const & c0 = edges[i].first < terminal_coordinates.size() ? terminal_coordinates[edges[i].first] : steinerpoint_coordinates[edges[i].first - terminal_coordinates.size()];
            std::vector<COOR> const & c1 = edges[i].second < terminal_coordinates.size() ? terminal_coordinates[edges[i].second] : steinerpoint_coordinates[edges[i].second - terminal_coordinates.size()];
            
            bool draw_connection = false;
            for (size_t j = 0; j < 3; ++j)
            {
                if (c0[j] != c1[j])
                {
                    if (draw_connection)
                    {
                        glPushMatrix();
                        glTranslatef(j < 1 ? c0[0] : c1[0], j < 2 ? c0[1] : c1[1], j < 3 ? c0[2] : c1[2]);
                        glScalef(eckpoint_radius, eckpoint_radius, eckpoint_radius);
                        draw_sphere(1,1,1);
                        glPopMatrix();
                    }
                    draw_connection = true;
                    glPushMatrix();
                    double posx = ((j > 0 ? c1[0] : c0[0]) + c1[0]) * 0.5;
                    double posy = ((j > 1 ? c1[1] : c0[1]) + (j > 0 ? c1[1] : c0[1])) * 0.5;
                    double posz = (c0[2] + (j > 1 ? c1[2] : c0[2])) * 0.5;
                    glTranslatef(posx, posy, posz);
                    if (j == 0)
                    {
                        glRotatef(90, 0, 1, 0);
                    }
                    else if (j == 1)
                    {
                        glRotatef(90, 1, 0, 0);
                    }
                    glScalef(connection_radius, connection_radius, 0.5 * std::abs(c0[j] - c1[j]));
                    draw_tube(1, 1, 1);
                    glPopMatrix();
                }
            }
        }
        steinerpoint_coordinates.clear();
        edges.clear();
        SDL_GL_SwapBuffers();
    }

    SDL_Quit();
    return 0;
}
