#ifndef INSTANCE_IO_H
#define INSTANCE_IO_H

#include <fstream>
#include "util.h"

void read_instance(std::ifstream & stream, steiner_instance & instance, size_t dim);

void print_instance(steiner_instance const & instance);

#endif
