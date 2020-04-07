#include <iostream>
#include <fstream>
#include <exception>

#include "dijkstra_steiner.h"
#include "instance_io.h"
#include "util.h"

int main(int argc, const char *argv[])
{
   if (argc != 2)
   {
      std::cout << "steinertree <file>" << std::endl;
      return 0;
   }

   steiner_instance instance;
   std::ifstream file;
   file.open (argv[1]);
   if (!file.good())
   {
      throw std::runtime_error("file does't exist");
   }
   read_instance(file, instance, 3);
   file.close();
   mark_excluded_vertices(instance);
   print_instance(instance);

   dijkstra_steiner_settings settings;
   settings._small_memory_mode = false;  //deletes object when possible, less memory use but higher runtime
   settings._maximum_heap_width = 20;    //lower values can result in less memory consumption but increase runtime

   std::vector<std::pair<size_t, size_t> > edges;
   std::cout << calculate_steinertree(*boundingbox_lower_bound, instance, settings, edges) << std::endl;
   return 0;
}
