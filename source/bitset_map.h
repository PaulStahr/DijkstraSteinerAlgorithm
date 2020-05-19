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

#ifndef BITSET_MAP_H
#define BITSET_MAP_H
//#define DEBUG

#include <exception>
#include <algorithm>
#include "util.h"

template <class Item>
class BitSetMap{
public:
   BitSetMap(BitSetMap<Item> const & btm_);

   BitSetMap(size_t num_bits_, size_t chunk_size_);

   Item* get_or_insert(BITSET key, Item & data);

   void insert_element(BITSET key, Item & data);

   Item* get_element(BITSET key);

   void delete_node_rek(void *node, size_t depth);

   ~BitSetMap();
private:
   size_t _total_bits;
   size_t _layer_count;
   size_t _layer_size;
   size_t _layer_bits;
   size_t _first_layer_size;
   size_t _first_layer_bits;
   size_t _bitmask;
   void *_root;
};

template <class Item>
BitSetMap<Item>::BitSetMap(BitSetMap<Item> const & btm_)
{
   _layer_bits = btm_._layer_bits;
   _total_bits = btm_._total_bits;
   _layer_count = btm_._layer_count;
   _layer_size = btm_._layer_size;
   _bitmask = btm_._bitmask;
   _first_layer_bits = btm_._first_layer_bits;
   _first_layer_size = btm_._first_layer_size;
   _root = new void*[_first_layer_size];
   std::fill((void**)_root, ((void**)_root) + _first_layer_size, nullptr);
}

template <class Item>
BitSetMap<Item>::BitSetMap(size_t num_bits_, size_t chunk_size_)
{
   _layer_bits = chunk_size_;
   _total_bits = num_bits_;
   _layer_count = (num_bits_ + _layer_bits - 1) / _layer_bits;
   _layer_size = 1 << _layer_bits;
   _bitmask = _layer_size - 1;
   _first_layer_bits = num_bits_ - (_layer_bits * (std::max(_layer_count, 1lu) - 1));
   _first_layer_size = 1 << _first_layer_bits;
   _root = new void*[_first_layer_size];
   std::fill((void**)_root, ((void**)_root) + _first_layer_size, nullptr);
}

template <class Item>
Item* BitSetMap<Item>::get_or_insert(BITSET key, Item & data)
{
   void **current_node = &_root;
   for (int i = (_layer_count - 1) * _layer_bits; i >= 0; i -= _layer_bits)
   {
      if (*current_node == nullptr)
      {
         *current_node = new void*[_layer_size];
         std::fill((void**)*current_node, (void**)(*current_node) + _layer_size, nullptr);
      }
      size_t index = (key >> i) & _bitmask;
      current_node = ((void**)(*current_node)) + index;
   }
   if (*current_node == nullptr)
   {
      *current_node = &data;
   }
   return (Item*)*current_node;
}

template <class Item>
void BitSetMap<Item>::insert_element(BITSET key, Item & data)
{
#ifdef DEBUG
   if ((key & ((1 << _total_bits) - 1)) != key)
   {
      throw std::runtime_error("Wrong key");
   }
#endif
   void **current_node = &_root;
   for (int i = (_layer_count - 1) * _layer_bits; i >= 0; i -= _layer_bits)
   {
      if (*current_node == nullptr)
      {
         *current_node = new void*[_layer_size];
         std::fill((void**)*current_node, (void**)(*current_node) + _layer_size, nullptr);
      }
      size_t index = (key >> i) & _bitmask;
      current_node = ((void**)(*current_node)) + index;
   }
   *current_node = &data;
}

template <class Item>
Item* BitSetMap<Item>::get_element(BITSET key)
{
   void *current_node = _root;
   for (int i = (_layer_count - 1) * _layer_bits; i >= 0; i -= _layer_bits)
   {
      size_t index = (key >> i) & _bitmask;
      current_node = ((void**)current_node)[index];
      if (current_node == nullptr)
      {
         return nullptr;
      }
   }
   return (Item*)current_node;
}

template <class Item>
void BitSetMap<Item>::delete_node_rek(void *node, size_t depth)
{
   if (depth < _layer_count)
   {
      for (size_t i = 0; i < (depth == 1 ? _first_layer_size : _layer_size); ++i)
      {
         if (((void **)node)[i] != nullptr)
         {
            delete_node_rek(((void **)node)[i], depth + 1);
            ((void **)node)[i] = nullptr;
         }
      }
   }
   delete [](void **)node;
   node = nullptr;
}

template <class Item>
BitSetMap<Item>::~BitSetMap()
{
   if (_root != nullptr)
   {
      delete_node_rek(_root, 1);
   }
   _root = nullptr;
}
#endif
