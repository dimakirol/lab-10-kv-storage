// Copyright 2019 dimakirol <your_email>

#ifndef INCLUDE_HEADER_HPP_
#define INCLUDE_HEADER_HPP_

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <ctpl.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/coroutine/attributes.hpp>
#include <boost/program_options.hpp>
#include <boost/thread/thread.hpp>

#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>

static const uint32_t default_threads = std::thread::hardware_concurrency();
#endif // INCLUDE_HEADER_HPP_
