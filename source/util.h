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
