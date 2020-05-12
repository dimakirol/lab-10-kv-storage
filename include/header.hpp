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
#include <picosha2.h>
#include <queue>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/coroutine/attributes.hpp>
#include <boost/program_options.hpp>
#include <boost/thread/thread.hpp>

#include <boost/log/trivial.hpp>

#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>

namespace Rocksdb = rocksdb;
namespace po = boost::program_options;
using namespace rocksdb;

static const uint32_t notes_number = rand()%17 + 3;
static const uint32_t masha_sleeps_seconds = rand()%4 + 3;
static const uint32_t kirill_sleeps_seconds = rand()%2 + 1;
static const uint32_t dima_sleeps_seconds = rand()%2 + 1;
static const uint32_t default_threads = std::thread::hardware_concurrency();
#endif // INCLUDE_HEADER_HPP_
