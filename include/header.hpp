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
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>

namespace Rocksdb = rocksdb;
using boost::log::v2_mt_posix::trivial::logger;
namespace po = boost::program_options;
namespace logging = boost::log;

static const uint32_t SIZE_FILE = 10*1024*1024;
static const uint32_t default_threads = std::thread::hardware_concurrency();
#endif // INCLUDE_HEADER_HPP_
