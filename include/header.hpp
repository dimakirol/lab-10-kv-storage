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

static const uint32_t cf_names_number = 7;
static const uint32_t value_size = 30;
static const uint32_t notes_number = rand()%17 + 3;
static const uint32_t masha_sleeps_seconds = rand()%4 + 3;
static const uint32_t kirill_sleeps_seconds = rand()%2 + 1;
static const uint32_t dima_sleeps_seconds = rand()%2 + 1;
static const uint32_t default_threads = std::thread::hardware_concurrency();



void print_rezult (const std::string &db_bef_name, const std::string &db_after_name) {
	DB* db_bef;
	DB* db_after;
	Status s;
	std::vector<std::string> _cf_names;
	DB::ListColumnFamilies(DBOptions(), db_bef_name, &_cf_names);

	std::vector<ColumnFamilyDescriptor> bcolumn_families;
	std::vector<ColumnFamilyDescriptor> acolumn_families;

	std::cout << "\n\n" << "Before:" << "\t\t\t\t\t\t"
	          << "After:" << "\n\n";

	std::cout << "Database : " << db_bef_name;
	std::cout << "\t\t\t\t" <<  "Database : "
	          << db_after_name << std::endl << std::endl;

	std::cout << "Column families:" << "\t\t\t\t";
	std::cout << "Column families:" << std::endl << std::endl;
	for (auto name : _cf_names){
		std::cout << name;
		bcolumn_families.push_back(ColumnFamilyDescriptor(
				name, ColumnFamilyOptions()));
		std::cout << "\t\t\t\t\t\t" <<  name << std::endl;
		acolumn_families.push_back(ColumnFamilyDescriptor(
				name, ColumnFamilyOptions()));
	}
	std::vector<ColumnFamilyHandle*> bhandles;
	s = DB::Open(DBOptions(), db_bef_name, bcolumn_families,
	             &bhandles, &db_bef);
	assert(s.ok());

	std::vector<ColumnFamilyHandle*> ahandles;
	s = DB::Open(DBOptions(), db_after_name, acolumn_families,
	             &ahandles, &db_after);
	assert(s.ok());


	std::cout << std::endl << std::endl << std::endl;

	std::vector<Iterator *> b_iterators;
	s = db_bef->NewIterators(ReadOptions(), bhandles, &b_iterators);
	assert(s.ok());

	std::vector<Iterator *> a_iterators;
	s = db_bef->NewIterators(ReadOptions(), ahandles, &a_iterators);
	assert(s.ok());

	auto iterator_column_names = _cf_names.begin();
	uint32_t i = 0;
	auto a_it = a_iterators.begin();
	for (auto b_it : b_iterators) {
		std::cout << std::endl << *iterator_column_names
		          << "\t\t\t\t\t\t" << *iterator_column_names << "\n\n";

		(*a_it)->SeekToFirst();
		for (b_it->SeekToFirst(); b_it->Valid(); b_it->Next()) {
			std::cout << b_it->key().data() << " : "
			          << b_it->value().ToString() << "\t";
			std::string pretty_kostyl = "";
			std::cout << (*a_it)->key().data() << " : "
			          << pretty_kostyl.assign((*a_it)->value().ToString(), 0, 46)
			          << "\n\t\t\t\t\t\t\t\t\t\t\t  "
			          << pretty_kostyl.assign((*a_it)->value().ToString(), 47,
			                                  std::string::npos)
			          << std::endl;

			(*a_it)->Next();
		}
		i++;
		iterator_column_names++;
		delete b_it;
		delete (*a_it);
		a_it++;
	}

	for (auto handle : bhandles) {
		s = db_bef->DestroyColumnFamilyHandle(handle);
		assert(s.ok());
	}
	for (auto handle : ahandles) {
		s = db_after->DestroyColumnFamilyHandle(handle);
		assert(s.ok());
	}
	delete db_bef;
	delete db_after;
}

#endif // INCLUDE_HEADER_HPP_
