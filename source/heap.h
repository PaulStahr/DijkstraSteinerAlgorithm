#ifndef HEAP_H
#define HEAP_H
#include <iterator>

namespace heap{

template <class RandomAccessIterator, class Compare, class IndexSet>
void extract_min(
   RandomAccessIterator first,
   RandomAccessIterator last,
   Compare comp,
   IndexSet assignIndex);

template <class RandomAccessIterator, class Compare, class IndexSet>
void make_heap (
   RandomAccessIterator first,
   RandomAccessIterator last,
   Compare comp,
   IndexSet assignIndex);

template <class RandomAccessIterator, class Compare>
bool check_heap(
   RandomAccessIterator first,
   RandomAccessIterator last,
   Compare comp);

template <class RandomAccessIterator, class Compare, class IndexSet>
void heapify(
   RandomAccessIterator first,
   RandomAccessIterator last,
   Compare comp,
   IndexSet assignIndex,
   size_t index);

template <class RandomAccessIterator, class Compare, class IndexSet>
void shift_down(
   RandomAccessIterator first,
   RandomAccessIterator last,
   Compare comp,
   IndexSet assignIndex,
   size_t index);

template <class RandomAccessIterator, class Compare, class IndexSet>
void shift_up(
   RandomAccessIterator first,
   Compare comp,
   IndexSet assignIndex,
   size_t index);

template <class RandomAccessIterator, class Compare, class IndexSet>
void extract_min(
   RandomAccessIterator first,
   RandomAccessIterator last,
   Compare comp,
   IndexSet assignIndex)
{
   std::swap(*first, *(last - 1));
   shift_down(first, last - 1, comp, assignIndex, 0);
}

template <class RandomAccessIterator, class Compare, class IndexSet>
void make_heap (
   RandomAccessIterator first,
   RandomAccessIterator last,
   Compare comp,
   IndexSet assignIndex)
{
   size_t size = std::distance(first, last);
   if (size >= 2)
   {
      for (size_t i = size / 2 - 1; i --> 0;)
      {
         auto obj = *(first + i);
         size_t elem = i;
         while (true)
         {
            size_t next = elem * 2 + 1 + (elem * 2 + 2 < size && comp(*(first + elem * 2 + 2), *(first + elem * 2 + 1))); //comp(a,b) is a < b

            if (next >= size || !comp(*(first + next), obj)) //Why comp(*(first + next)), obj)
            {
               break;
            }
            *(first + elem) = *(first + next);
            elem = next;
         }
         *(first + elem) = obj;
      }
   }
   for (size_t i = 0; i < size; ++i)
   {
      assignIndex(*(first + i), i);
   }
}

template <class RandomAccessIterator, class Compare>
bool check_heap(
   RandomAccessIterator first,
   RandomAccessIterator last,
   Compare comp)
{
   size_t size = std::distance(first, last);
   for (size_t i = 1; i < size; ++i)
   {
      if (comp(*(first + i), *(first + (i - 1) / 2)))
      {
         return false;
      }
   }
   return true;
}

template <class RandomAccessIterator, class Compare, class IndexSet>
void heapify(
   RandomAccessIterator first,
   RandomAccessIterator last,
   Compare comp,
   IndexSet assignIndex,
   size_t index)
{
   shift_down(first, last, comp, assignIndex, index);
   shift_up(first, comp, assignIndex, index);
}

template <class RandomAccessIterator, class Compare, class IndexSet>
void shift_down(
   RandomAccessIterator first,
   RandomAccessIterator last,
   Compare comp,
   IndexSet assignIndex,
   size_t index)
{
   size_t size = std::distance(first, last);
   auto obj = first[index];
   while (true)
   {
      size_t next = index * 2 + 1 + (index * 2 + 2 < size && comp(*(first + index * 2 + 2), *(first + index * 2 + 1))); //comp(a,b) is a < b

      if (next >= size || !comp(first[next], obj))
      {
         break;
      }
      first[index] = first[next];
      assignIndex(first[index], index);
      index = next;
   }
   first[index] = obj;
   assignIndex(obj, index);
}

template <class RandomAccessIterator, class Compare, class IndexSet>
void shift_up(
   RandomAccessIterator const first,
   Compare comp,
   IndexSet assignIndex,
   size_t index)
{
   auto obj = *(first + index);
   while (index > 0)
   {
      size_t next = (index - 1) / 2;
      if (!comp(obj, *(first + next)))
      {
         break;
      }
      *(first + index) = *(first + next);
      assignIndex(*(first + index), index);
      index = next;
   }
   *(first + index) = obj;
   assignIndex(obj, index);
}
}
#endif
