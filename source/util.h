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

#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include <string>
#include <vector>
#include <sstream>

typedef uint64_t BITSET;    //The maximum number of terminals allowed in this tool is 64
typedef int32_t COOR;
typedef uint32_t DISTANCE_T;

struct neighbour{
   size_t _vertex;
   DISTANCE_T _distance;
};

struct vertex{
   std::vector<COOR> _coords;
   std::vector<neighbour> _neighbours;
   uint8_t _terminal_number;
   bool _is_excluded;
};

struct steiner_instance{
   std::vector<vertex> _vertices;
   std::vector<size_t> _sizes;
   std::vector<size_t> _terminals;
   std::vector<COOR > _terminal_coords;
};

template <typename T>
std::string print_vec(std::vector<T> data);


template< class T >
T multiply(std::vector<T> const & vec);

void update_neighbours(steiner_instance & instance);

size_t calculate_index(std::vector<size_t> const & sizes, std::vector<size_t> const & indices);

void sizes_to_steps(std::vector<size_t> const & sizes, std::vector<size_t> & steps);
template <typename T>
std::string print_vec(std::vector<T> data)
{
   std::stringstream ss;
   for (size_t i = 0; i < data.size(); ++i)
   {
      ss << data[i] << ' ';
   }
   return ss.str();
}

template< class T >
T multiply(std::vector<T> const & vec)
{
   T erg = 1;
   for (size_t i = 0; i< vec.size(); ++i)
   {
      erg *= vec[i];
   }
   return erg;
}


#endif
