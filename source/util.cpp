#include <sstream>
#include <limits>
#include "util.h"

void update_neighbours(steiner_instance & instance)
{
   size_t dim = instance._sizes.size();
   size_t vertex_count = instance._vertices.size();
   std::vector<size_t> step;
   sizes_to_steps(instance._sizes, step);
   for (size_t i = 0; i < vertex_count; ++i)
   {
      vertex & current_vertex = instance._vertices[i];
      current_vertex._neighbours.clear();

      if (!current_vertex._is_excluded)
      {
         std::vector<COOR> const & current_coor = current_vertex._coords;
         for (size_t j = 0; j < 2 * dim; ++j)
         {
            const vertex* w = nullptr;
            DISTANCE_T dist = std::numeric_limits<DISTANCE_T>::max();
            size_t w_index = std::numeric_limits<size_t>::max();
            if (j < dim)
            {
               w_index = i + step[j];
               if (w_index >= vertex_count)
               {
                  continue;
               }
               w = &instance._vertices[w_index];
               if (w->_coords[j] <= current_coor[j] || w->_is_excluded)
               {
                  continue;
               }
               dist = w->_coords[j] - current_coor[j];
            }
            else
            {
               w_index = i - step[j - dim];
               if (w_index >= vertex_count)
               {
                  continue;
               }
               w = &instance._vertices[w_index];
               if (w->_coords[j - dim] >= current_coor[j - dim] || w->_is_excluded)
               {
                  continue;
               }
               dist = current_coor[j - dim] - w->_coords[j - dim];
            }
            current_vertex._neighbours.push_back({w_index, dist});
         }
      }
      current_vertex._neighbours.shrink_to_fit();
   }
}

size_t calculate_index(std::vector<size_t> const & sizes, std::vector<size_t> const & indices)
{
   size_t index = indices.back();
   for (size_t i = indices.size() - 1; i --> 0;)
   {
      index = index * sizes[i] + indices[i];
   }
   return index;
}

void sizes_to_steps(std::vector<size_t> const & sizes, std::vector<size_t> & steps)
{
   steps.clear();
   steps.reserve(sizes.size());
   steps.push_back(1);
   for (size_t i = 0; i < sizes.size() - 1; ++i)
   {
      steps.push_back(steps.back() * sizes[i]);
   }
}
