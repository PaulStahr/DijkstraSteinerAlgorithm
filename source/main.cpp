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
