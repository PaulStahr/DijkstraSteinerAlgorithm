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

#ifndef DIJKSTRA_STEINER_H
#define DIJKSTRA_STEINER_H

#include <vector>
#include "util.h"

struct light_node{
   DISTANCE_T _steinerlength;
   light_node *prev0, *prev1;
   size_t _v;
};

struct node : light_node{
   BITSET _terminal_key;
   DISTANCE_T _lower_bound_steinerlength;
   size_t _heap_index;
};

struct dijkstra_steiner_settings{
   bool _small_memory_mode;
   size_t _maximum_heap_width;
   bool _edge_as_steinerpoint;

   dijkstra_steiner_settings()
   {
      _small_memory_mode = false;
      _maximum_heap_width = 64;
      _edge_as_steinerpoint = true;
   }
};

DISTANCE_T calculate_steinertree(
   DISTANCE_T (*lower_bound)(BITSET terminal_key, size_t vertex, steiner_instance const &),
   std::vector<std::vector <COOR> > const & terminals,
   dijkstra_steiner_settings const & settings,
   std::vector<std::vector <COOR> > & steinerpoints,
   std::vector<std::pair<size_t, size_t> > & edges
);

void mark_excluded_vertices(steiner_instance & instance);

void mark_excluded_vertices(std::vector<size_t> const & terminals, std::vector<size_t> const & sizes, std::vector<bool> & is_excluded);

DISTANCE_T calculate_steinertree(
   DISTANCE_T (*lower_bound)(BITSET terminal_key, size_t vertex, steiner_instance const &),
   steiner_instance const & instance,
   dijkstra_steiner_settings const & settings, 
   std::vector<std::pair<size_t, size_t> > & edges);

DISTANCE_T zero_lower_bound(BITSET terminal_key, size_t vertex, steiner_instance const & instance);

DISTANCE_T boundingbox_lower_bound(BITSET terminal_key, size_t vertex, steiner_instance const & instance);

#endif
