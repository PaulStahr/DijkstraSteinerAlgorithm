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

#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <numeric>

#include "instance_io.h"
#include "util.h"

//TODO: clear multiple terminals on same location
void read_instance(std::ifstream & stream, steiner_instance & instance, size_t dim){
   std::vector<std::vector<COOR> > coords;
   std::vector<std::vector<bool> > coord_used;

   coords.reserve(dim);
   coord_used.reserve(dim);
   for (size_t i = 0; i < dim; ++i)
   {
      std::string str;
      std::getline(stream, str);
      std::stringstream line(str);
      coords.push_back(std::vector<COOR>());
      for (size_t j = 0; !line.eof(); ++j)
      {
         COOR tmp = std::numeric_limits<COOR>::max();
         line >> tmp;
         if (tmp == std::numeric_limits<COOR>::max())
         {
            throw std::runtime_error("Can't read number");
         }
         coords[i].push_back(tmp);
      }
      std::sort(coords[i].begin(), coords[i].end());
      coord_used.push_back(std::vector<bool>(coords[i].size(), false));
   }

   std::vector<std::vector<COOR> > terminal_coords;
   std::string str;
   std::getline(stream, str);
   std::stringstream line(str);

   while (!line.eof())
   {
      terminal_coords.push_back(std::vector<COOR>());
      terminal_coords.back().reserve(dim);
      for (size_t j = 0; j < dim; ++j)
      {
         if (line.eof())
         {
            throw std::runtime_error("Number of terminals does't match");
         }
         COOR tmp = std::numeric_limits<COOR>::max();
         line >> tmp;
         if (tmp == std::numeric_limits<COOR>::max())
         {
            throw std::runtime_error("Can't read number");
         }
         std::vector<COOR>::iterator iter;
         iter = std::find(coords[j].begin(), coords[j].end(), tmp);
         if (iter == coords[j].end())
         {
            throw std::runtime_error("Coord doesn't exist");
         }
         coord_used[j][std::distance(coords[j].begin(), iter)] = true;
         terminal_coords.back().push_back(tmp);
      }
   }
   for (size_t i = 0; i < dim; ++i)
   {
      size_t write_index = 0;
      for (size_t j = 0; j < coord_used[i].size(); ++j)
      {
         if (coord_used[i][j])
         {
            coords[i][write_index++] = coords[i][j];
         }
      }
      coords[i].resize(write_index);
      instance._sizes.push_back(coords[i].size());
   }
   instance._vertices.clear();
   instance._vertices.reserve(std::accumulate(instance._sizes.begin(), instance._sizes.end(), size_t(1), std::multiplies<size_t>()));

   std::vector<size_t> indices(dim, 0);
   while (indices.back() < instance._sizes.back())
   {
      instance._vertices.push_back(vertex());
      vertex & current_v = instance._vertices.back();
      current_v._coords.reserve(dim);
      for (size_t i = 0; i < dim; ++i)
      {
         current_v._coords.push_back(coords[i][indices[i]]);
      }
      current_v._is_excluded = false;
      current_v._terminal_number = std::numeric_limits<unsigned char>::max();

      /*Enumerate all combination of indices*/
      size_t i;
      for (i = 0; i < dim - 1 && indices[i] >= instance._sizes[i] - 1; ++i)
      {
         indices[i] = 0;
      }
      ++indices[i];
   }

   instance._terminals.reserve(terminal_coords.size());
   instance._terminal_coords.reserve(terminal_coords.size());
   for (size_t i = 0; i < terminal_coords.size(); ++i)
   {
      size_t index = 0;
      for (size_t j = dim; j --> 0;)
      {
         std::vector<COOR>::iterator iter = std::find(coords[j].begin(), coords[j].end(), terminal_coords[i][j]);
         index = index * coords[j].size() + std::distance(coords[j].begin(), iter);
      }
      instance._vertices[index]._terminal_number = i;
      instance._terminals.push_back(index);
      instance._terminal_coords.insert(instance._terminal_coords.end(), instance._vertices[index]._coords.begin(), instance._vertices[index]._coords.end());
   }
}

void print_instance(steiner_instance const & instance)
{
   size_t dim = instance._sizes.size();
   std::cout<< "dimensions:";
   for (size_t i = 0; i < dim; ++i)
   {
      std::cout << " " << instance._sizes[i];
   }
   std::cout << std::endl;
   std::vector<size_t> indices(dim, 0); 
   for (size_t i = 0; i < dim; ++i)
   {
      std::cout << "coords of dim " << i << " :";
      for (size_t j = 0; j < instance._sizes[i]; ++j)
      {
         indices[i] = j;
         std::cout << " " << instance._vertices[calculate_index(instance._sizes, indices)]._coords[i];
      }
      indices[i] = 0;
      std::cout << std::endl;
   }
   for (size_t i = 0; i < instance._terminals.size(); ++i)
   {
      std::cout << "terminal " << i << ":" << print_vec(instance._vertices[instance._terminals[i]]._coords) << std::endl;
      for (size_t j = 0; j < dim; ++j)
      {
         if (instance._vertices[instance._terminals[i]]._coords[j] != instance._terminal_coords[i * dim + j])
         {
            std::cout << "Warning, malformed instance" << std::endl;
         }
      }
   }
   for (size_t i = 0; i < instance._vertices.size(); ++i)
   {
      std::cout << "v_coord:" << print_vec(instance._vertices[i]._coords) << " e:" << instance._vertices[i]._is_excluded << std::endl;
   }
}
