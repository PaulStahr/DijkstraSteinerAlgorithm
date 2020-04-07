#include <iostream>
#include <vector>
#include <algorithm>
#include <exception>
#include <limits>
#include <cstdint>
#include <iterator>
#include "heap.h"
#include "bitset_map.h"
#include "util.h"
#include "dijkstra_steiner.h"
#include "instance_io.h"

struct node_comparator_struct
{
    bool operator()(node const * a, node const * b) const
    {
       if (a->_lower_bound_steinerlength == b->_lower_bound_steinerlength)
       {
          return a->_v < b->_v;
       }
       return a->_lower_bound_steinerlength < b->_lower_bound_steinerlength;
    }
};

static node_comparator_struct node_comparator;

struct node_index_set_struct
{
   void operator()(node * n, size_t const index) const
   {
      n->_heap_index = index;
   }
};

static node_index_set_struct node_index_set;

void calculate_position(std::vector<size_t> const & sizes, std::vector<size_t> & indices, size_t index)
{
   indices.clear();
   indices.reserve(sizes.size());
   for (size_t i = 0; i < sizes.size(); ++i)
   {
      indices.push_back(index % sizes[i]);
      index /= sizes[i];
   }
}

void mark_excluded_vertices(std::vector<std::vector<size_t > > const & terminals, std::vector<size_t> const & sizes, std::vector<bool> & is_excluded)
{
   is_excluded.clear();
   size_t size = multiply(sizes);
   is_excluded.resize(size, false);
   size_t dim = sizes.size();
   switch(dim)
   {
      case 0:
         break;
      case 1:
         break;
      case 2:
      {
         std::vector<std::vector<size_t> > terminals_by_layer(sizes[1], std::vector<size_t>());
         std::vector<size_t> min_coords;
         std::vector<size_t> max_coords;
         min_coords.reserve(sizes[1]);
         max_coords.reserve(sizes[1]);

         for (size_t i = 0; i < terminals.size(); ++i)
         {
            terminals_by_layer[terminals[i][1]].push_back(terminals[i][0]);
         }
         for (size_t i = 0; i < sizes[1]; ++i)
         {
            if (terminals_by_layer[i].size() == 0)
            {
               throw std::runtime_error("empty layer");
               min_coords.push_back(std::numeric_limits<size_t>::max());
               max_coords.push_back(std::numeric_limits<size_t>::min());
            }
            else
            {
               auto result = std::minmax_element(terminals_by_layer[i].begin(), terminals_by_layer[i].end());
               min_coords.push_back(*result.first);
               max_coords.push_back(*result.second);
            }
         }

         std::vector<size_t> min_coords_fwd(sizes[1], min_coords[0]);
         std::vector<size_t> max_coords_fwd(sizes[1], max_coords[0]);
         std::vector<size_t> min_coords_bwd(sizes[1], min_coords[sizes[1] - 1]);
         std::vector<size_t> max_coords_bwd(sizes[1], max_coords[sizes[1] - 1]);
         for (size_t i = 1; i < sizes[1]; ++i)
         {
            min_coords_fwd[i] = std::min(min_coords_fwd[i - 1], min_coords[i]);
            max_coords_fwd[i] = std::max(max_coords_fwd[i - 1], max_coords[i]);
            min_coords_bwd[sizes[1] - i - 1] = std::min(min_coords_fwd[sizes[1] - i], min_coords[sizes[1] - i - 1]);
            max_coords_bwd[sizes[1] - i - 1] = std::max(max_coords_fwd[sizes[1] - i], max_coords[sizes[1] - i - 1]);
         }
         for (size_t i = 0; i < sizes[1]; ++i)
         {
            for (size_t j = 0; j < sizes[0]; ++j)
            {
               if (j < min_coords_fwd[i] || j > max_coords_fwd[i])
               {
                  is_excluded[i * sizes[0]+ j] = true;
               }
            }
         }
         for (size_t i = 1; i < sizes[1]; ++i)
         {
            for (size_t j = 0; j < sizes[0]; ++j)
            {
               if((j < min_coords_bwd[i] && max_coords_fwd[i - 1] < j)
                  ||(j > max_coords_bwd[i] && min_coords_fwd[i - 1] > j))
               {
                  is_excluded[i * sizes[0]+ j] = true;
               }
            }
         }
         break;
      }
      default:
      {
         /*Project to all 2d-surfaces*/
         std::vector<std::vector<size_t> > terminals_plane(terminals.size(), std::vector<size_t>(2, 0));
         for (size_t i = 1; i < dim; ++i)
         {
            for (size_t j = 0; j < i; ++j)
            {
               std::vector<size_t> sizes_plane = {sizes[i], sizes[j]};
               std::vector<bool> is_excluded_plane(sizes_plane[0] * sizes_plane[1], false);
               for (size_t k = 0; k < terminals.size(); ++k)
               {
                  terminals_plane[k] = {terminals[k][i], terminals[k][j]};
               }
               mark_excluded_vertices(terminals_plane, sizes_plane, is_excluded_plane);

               std::vector<size_t> indices(dim, 0);
               for (size_t index = 0; indices.back() < sizes.back(); ++index)
               {
                  if (is_excluded_plane[indices[i] + sizes_plane[0] * indices[j]])
                  {
                     is_excluded[index] = true;
                  }
                  /*Enumerate all combination of indices*/
                  size_t k;
                  for (k = 0; k < dim - 1 && indices[k] >= sizes[k] - 1; ++k)
                  {
                     indices[k] = 0;
                  }
                  ++indices[k];
               }
            }
         }
         break;
      }
   }
}

void mark_excluded_vertices(std::vector<size_t> const & terminals, std::vector<size_t> const & sizes, std::vector<bool> & is_excluded)
{
   std::vector<std::vector<size_t > > terminal_positions(terminals.size(), std::vector<size_t>());
   for (size_t i = 0; i < terminals.size(); ++i)
   {
      calculate_position(sizes, terminal_positions[i], terminals[i]);
   }
   mark_excluded_vertices(terminal_positions, sizes, is_excluded);
}

/*stores the pairs of vertex, terminals for quick access*/
void mark_excluded_vertices(steiner_instance & instance)
{
   std::vector<bool> is_excluded;
   mark_excluded_vertices(instance._terminals, instance._sizes, is_excluded);
   for (size_t i = 0; i < instance._vertices.size(); ++i)
   {
      if (is_excluded[i])
      {
         instance._vertices[i]._is_excluded = true;
      }
   }
   update_neighbours(instance);
}

/*void get_edges(
   size_t vertex,
   std::vector<std::vector<size_t> > const & adjactend_vertices,
   std::vector<bool> const & is_terminal,
   std::vector<std::pair<size_t, size_t> > & edges,
   std::vector<bool> & visited)
{
   if (visited[vertex])
   {
      return;
   }
   size_t unvisited_neighbours = 0;
   size_t unvisited_neighbour = 0;
   size_t last_unvisited_neighbour = 0;
   for (size_t i = 0; i < adjactend_vertices[vertex].size(); ++i)
   {
      if (!visited[adjactend_vertices[vertex][i]])
      {
         ++unvisited_neighbours;
         unvisited_neighbour = adjactend_vertices[vertex][i];
      }
   }
   if (unvisited_neighbours == 1)
   {
      if (last_unvisited_neighbour + unvisited_neighbour == vertex * 2)
      {
         last_unvisited_neighbour = vertex;
         vertex = unvisited_neighbour;
      }
      else
      {
         edges.push_back(std::pair<size_t, size_t>());
      }
   }
   else
   {
   }
}*/

DISTANCE_T calculate_steinertree(
   DISTANCE_T (*lower_bound)(BITSET terminal_key, size_t vertex, steiner_instance const &),
   std::vector<std::vector <COOR> > const & terminals,
   dijkstra_steiner_settings const & settings,
   std::vector<std::vector <COOR> > & steinerpoints,
   std::vector<std::pair<size_t, size_t> > & edges)
{
   steinerpoints.clear();
   edges.clear();
   size_t dim = terminals[0].size();
   std::vector<std::vector<COOR> > coords;
   coords.reserve(3);
   for (size_t j = 0; j < dim; ++j)
   {
      coords.push_back(std::vector<COOR>());
      coords[j].reserve(terminals.size());
   }
   for (size_t i = 0; i < terminals.size(); ++i)
   {
      for (size_t j = 0; j < dim; ++j)
      {
         coords[j].push_back(terminals[i][j]);
      }
   }
   std::vector<size_t> sizes;
   sizes.reserve(dim);
   for (std::vector<COOR> & co : coords)
   {
      std::sort(co.begin(), co.end());
      co.erase(std::unique(co.begin(), co.end()), co.end());
      sizes.push_back(co.size());
   }
   std::vector<std::vector<size_t > > terminal_indizes;
   terminal_indizes.reserve(terminals.size());
   
   for (size_t i = 0; i < terminals.size(); ++i)
   {
      terminal_indizes.push_back(std::vector<size_t>());
      terminal_indizes[i].reserve(dim);
      for (size_t j = 0; j < dim; ++j)
      {
         std::vector<COOR>::iterator iter = std::find(coords[j].begin(), coords[j].end(), terminals[i][j]);
         terminal_indizes[i].push_back(std::distance(coords[j].begin(), iter));
      }
   }
   std::vector<bool> excluded;
   mark_excluded_vertices(terminal_indizes, sizes, excluded);
   steiner_instance instance;
   instance._sizes = sizes;
   instance._vertices.reserve(multiply(instance._sizes));

   std::vector<size_t> indices(dim, 0);
   size_t index = 0;
   while (indices.back() < instance._sizes.back())
   {
      instance._vertices.push_back(vertex());
      vertex & current_v = instance._vertices.back();
      current_v._coords.reserve(dim);
      for (size_t i = 0; i < dim; ++i)
      {
         current_v._coords.push_back(coords[i][indices[i]]);
      }
      current_v._is_excluded = excluded[index];
      current_v._terminal_number = std::numeric_limits<uint8_t>::max();

      /*Enumerate all combination of indices*/
      size_t i;
      for (i = 0; i < dim - 1 && indices[i] >= instance._sizes[i] - 1; ++i)
      {
         indices[i] = 0;
      }
      ++indices[i];
      ++index;
   }

   instance._terminals.reserve(terminals.size());
   instance._terminal_coords.reserve(terminals.size());
   for (size_t i = 0; i < terminals.size(); ++i)
   {
      size_t index = calculate_index(sizes, terminal_indizes[i]);
      instance._vertices[index]._terminal_number = i;
      instance._terminals.push_back(index);
      instance._terminal_coords.insert(instance._terminal_coords.end(), instance._vertices[index]._coords.begin(), instance._vertices[index]._coords.end());
   }
   mark_excluded_vertices(instance);
   update_neighbours(instance);
   std::vector<std::pair<size_t, size_t> > grid_edges;
   //print_instance(instance);
   DISTANCE_T length = calculate_steinertree(lower_bound, instance, settings, grid_edges);
   std::vector<std::vector<size_t> > adjactend_nodes(instance._vertices.size(), std::vector<size_t>());
   for (auto const & ge : grid_edges)
   {
      adjactend_nodes[ge.first].push_back(ge.second);
      adjactend_nodes[ge.second].push_back(ge.first);
   }
   std::vector<bool> visited(instance._vertices.size(), false);
   std::vector<size_t> vertex_kind(instance._vertices.size(), std::numeric_limits<size_t>::max());
   for (size_t i = 0; i < instance._vertices.size(); ++i)
   {
      if (instance._vertices[i]._terminal_number < instance._terminals.size())
      {
         vertex_kind[i] = instance._vertices[i]._terminal_number; /*save this node*/
      }
      else if (adjactend_nodes[i].size() != 0)
      {
         if (adjactend_nodes[i].size() != 2
            || (settings._edge_as_steinerpoint && adjactend_nodes[i][0] + adjactend_nodes[i][1] != i * 2))
         {
            vertex_kind[i] = instance._terminals.size() + steinerpoints.size(); /*save this node*/
            steinerpoints.push_back(instance._vertices[i]._coords);
         }
         else
         {
            vertex_kind[i] = std::numeric_limits<size_t>::max() - 1; /*node on trunk*/
         }
      }
   }
   for (size_t i = 0; i < instance._vertices.size(); ++i)
   {
      if (vertex_kind[i] < instance._terminals.size() + steinerpoints.size())
      {
         visited[i] = true;
         for (size_t j = 0; j < adjactend_nodes[i].size(); ++j)
         {
            size_t last_vertex = i;
            size_t current_vertex = adjactend_nodes[i][j];
            size_t next_vertex = std::numeric_limits<size_t>::max();
            if (visited[current_vertex])
            {
               continue;
            }
            while (vertex_kind[current_vertex] == std::numeric_limits<size_t>::max() - 1)
            {
               visited[current_vertex] = true;
               next_vertex = last_vertex ^ adjactend_nodes[current_vertex][1] ^ adjactend_nodes[current_vertex][0];
               last_vertex = current_vertex;
               current_vertex = next_vertex;
            }
            edges.push_back(std::pair<size_t, size_t>(vertex_kind[i], vertex_kind[current_vertex]));
         }
      }
   }
   return length;
}

void track_back(
   std::vector<std::pair<size_t, size_t> > & edges,
   light_node const & n)
{
   if (!n.prev0)
   {
   }
   else if (n.prev0 == n.prev1)
   {
      edges.push_back(std::pair<size_t, size_t>(n._v, n.prev0->_v));
      track_back(edges, *n.prev0);
   }
   else
   {
      track_back(edges, *n.prev0);
      track_back(edges, *n.prev1);
   }
}

DISTANCE_T calculate_steinertree(
   DISTANCE_T (*lower_bound)(BITSET terminal_key, size_t vertex, steiner_instance const &),
   steiner_instance const & instance,
   dijkstra_steiner_settings const & settings, 
   std::vector<std::pair<size_t, size_t> > & edges)
{
   bool small_memory_mode = settings._small_memory_mode;

   size_t num_vertices = instance._vertices.size();
   size_t num_terminals = instance._terminals.size();
   std::vector<vertex>::const_iterator vertices = instance._vertices.begin();
   std::vector<node*> node_heap;
   std::vector<std::vector <DISTANCE_T> > node_p(num_vertices * num_terminals);
   std::vector<std::vector <BITSET> > node_p_key(num_vertices * num_terminals);
   std::vector<std::vector <light_node*> > extracted_nodes(num_vertices * num_terminals);

   std::vector<BitSetMap<light_node> > node_tree;
   node_tree.reserve(num_vertices);
   node_heap.reserve(num_terminals);
   {
      BitSetMap<light_node> init_normal = BitSetMap<light_node>(num_terminals - 1, settings._maximum_heap_width);
      BitSetMap<light_node> init_empty = BitSetMap<light_node>(0, settings._maximum_heap_width);
      for (size_t i = 0; i < num_vertices; ++i)
      {
         bool excluded = vertices[i]._is_excluded;
         node_tree.push_back(excluded ? init_empty : init_normal);
      }
   }

   /*t will be last terminal*/
   for (size_t i = 0; i < num_terminals; ++i)
   {
      if (vertices[instance._terminals[i]]._is_excluded)
      {
         throw std::runtime_error("Terminal marked as excluded");
      }
   }
   for (size_t i = 0; i < num_terminals - 1; ++i)
   {
      node & n = *(new node());
      n._v = instance._terminals[i];
      n._terminal_key = 1u << i;
      n._lower_bound_steinerlength = lower_bound(n._terminal_key, n._v, instance);
      n._steinerlength = 0;
      node_heap.push_back(&n);
      node_tree[n._v].insert_element(n._terminal_key, n);
   }
   heap::make_heap(node_heap.begin(), node_heap.end(), node_comparator, node_index_set);
   BITSET last_terminal_key = 1u << (num_terminals - 1); /*2^(terminals size - 1) - 1 is all terminals without last*/
   DISTANCE_T current_steinerlength;
   light_node *current_node;
   while (true)
   {
      if (node_heap.empty())
      {
         throw std::runtime_error("Empty heap");
      }

      node & tmp = *node_heap.front();
      node_heap.front() = node_heap.back();
      heap::shift_down(node_heap.begin(), node_heap.end(), node_comparator, node_index_set, 0);
      node_heap.pop_back();

      size_t current_node_v = tmp._v;
      BITSET current_terminal_key = tmp._terminal_key;
      current_steinerlength = tmp._steinerlength;
      uint8_t current_terminal_count = __builtin_popcount(current_terminal_key);

      current_node = small_memory_mode ? new light_node(tmp) : &tmp;
      if (small_memory_mode)
      {
         node_tree[current_node_v].insert_element(current_terminal_key, *current_node);
         delete &tmp;
      }
      size_t offet = current_node_v * num_terminals;
      extracted_nodes[offet + current_terminal_count].push_back(current_node);
      node_p[offet + current_terminal_count].push_back(current_steinerlength);
      node_p_key[offet + current_terminal_count].push_back(current_terminal_key);

      if (current_node_v == instance._terminals[num_terminals - 1] && current_terminal_key == last_terminal_key - 1)
      {
         break;
      }

      std::vector<neighbour> const & neighbours = vertices[current_node_v]._neighbours;
      for (size_t i = 0; i < neighbours.size(); ++i)
      {
         neighbour const & current_neighbour = neighbours[i];
         DISTANCE_T neighbour_steinerlength = current_steinerlength + current_neighbour._distance;
         size_t w_index = current_neighbour._vertex;
         BITSET tmp_terminal_key = current_terminal_key | ((last_terminal_key - 1) & (1 << vertices[w_index]._terminal_number));

         node *n = (node*)node_tree[w_index].get_element(tmp_terminal_key);
         if (n == nullptr)
         {
            n = new node();
            n->_v = w_index;
            n->_terminal_key = tmp_terminal_key;
            n->_lower_bound_steinerlength = neighbour_steinerlength + lower_bound(tmp_terminal_key, w_index, instance);
            n->_heap_index = node_heap.size();
            node_heap.push_back(n);
            node_tree[w_index].insert_element(tmp_terminal_key, *n);
         }
         else if (n->_steinerlength > neighbour_steinerlength)
         {
            n->_lower_bound_steinerlength = neighbour_steinerlength + (n->_lower_bound_steinerlength - n->_steinerlength);
         }
         else
         {
            continue;
         }
         n->_steinerlength = neighbour_steinerlength;
         heap::shift_up(node_heap.begin(), node_comparator, node_index_set, n->_heap_index);
         n->prev0 = n->prev1 = current_node;
      }

      BitSetMap<light_node> & current_node_tree = node_tree[current_node_v];
      BITSET key = current_terminal_key | last_terminal_key;  //this implements I union t
      for (uint8_t j = 1; j < num_terminals - current_terminal_count; ++j) //first entry is the zero key, which we ignore
      {
         std::vector<DISTANCE_T>::const_iterator current_p = node_p[offet + j].begin();
         std::vector<BITSET> const & current_p_key = node_p_key[offet + j];
         std::vector<light_node* >::const_iterator current_extracted = extracted_nodes[offet + j].begin();
         for (size_t i = 0; i < current_p_key.size(); ++i)
         {
            BITSET p_terminal_key = current_p_key[i];
            if (!(p_terminal_key & key)) // this asks for whether the nodes coincide and if J is a subset of I union t complement
            {
               DISTANCE_T added_steinerlength = current_steinerlength + current_p[i];
               BITSET  union_terminal_key = current_terminal_key | p_terminal_key;
               node *k = (node*)current_node_tree.get_element(union_terminal_key);
               if (k == nullptr)
               {
                  k = new node();  //terminals of n and current_node are disjoint
                  k->_v = current_node_v;
                  k->_terminal_key = union_terminal_key;
                  k->_steinerlength = added_steinerlength;
                  k->_lower_bound_steinerlength = added_steinerlength + lower_bound(union_terminal_key, current_node_v, instance);
                  k->_heap_index = node_heap.size();
                  node_heap.push_back(k);
                  current_node_tree.insert_element(union_terminal_key, *k);
               }
               else if(k->_steinerlength > added_steinerlength)
               {
                  k->_lower_bound_steinerlength = (k->_lower_bound_steinerlength - k->_steinerlength) + added_steinerlength;
               }
               else
               {
                  continue;
               }
               heap::shift_up(node_heap.begin(), node_comparator, node_index_set, k->_heap_index);
               k->_steinerlength = added_steinerlength;
               k->prev0 = current_node;
               k->prev1 = current_extracted[i];
            }
         }
      }
   }

   track_back(edges, *current_node);
   for (node* node : node_heap)
   {
      delete node;
   }
   for (size_t i = 0; i < extracted_nodes.size(); ++i)
   {
      for (light_node *ln : extracted_nodes[i])
      {
         delete ln;
      }
   }
   return current_steinerlength;
}

DISTANCE_T zero_lower_bound(BITSET , size_t , steiner_instance const & ){
   return 0;
}

DISTANCE_T boundingbox_lower_bound(BITSET terminal_key, size_t vertex, steiner_instance const & instance)
{
   static std::vector<COOR> minmaxc;
   size_t dim = instance._sizes.size();

   minmaxc.clear();
   for (COOR cord : instance._vertices[vertex]._coords)
   {
       minmaxc.push_back(cord);
       minmaxc.push_back(cord);
   }
   terminal_key ^= (1 << instance._terminals.size()) - 1;
   for (auto coords = instance._terminal_coords.begin(); terminal_key != 0; terminal_key >>= 1)
   {
      if ((terminal_key & 1) != 0)
      {
         for (auto iter = minmaxc.begin(); iter != minmaxc.end(); ++iter, ++coords)
         {
            *iter = std::min(*iter, *coords);
            ++iter;
            *iter = std::max(*iter, *coords);
         }
      }
      else
      {
          coords += dim;
      }
   }

   DISTANCE_T length = 0;
   for (auto iter = minmaxc.begin(); iter != minmaxc.end(); iter += 2)
   {
      length += iter[1] - iter[0];
   }
   return length;
}
