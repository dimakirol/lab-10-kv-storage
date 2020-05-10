// Copyright 2019 dimakirol <your_email>

#include <header.hpp>

#define KEY_SIZE 14
#define VALUE_SIZE 30

class RandomString {
public:

    RandomString() {
        alpha =
                "abcdefghijklmnopqrstuvwxyz"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "1234567890";
        auto now = static_cast<unsigned int>(time(nullptr));
        randomizer = static_cast<uint64_t>(rand_r(&now) % alpha.length());
    }

    std::string SetRandomValue (int ThreadID) {
        int64_t tmp_randomizer = randomizer;
        tmp_randomizer += ThreadID;
        std::string random_string = alpha;
        random_string.assign(alpha, 0, VALUE_SIZE);
        random_string.erase(tmp_randomizer % random_string.size(), 1);
        random_string = random_string + alpha[tmp_randomizer % alpha.size()];
        return random_string;
    }

    std::atomic_int64_t randomizer;
    std::string alpha;
};

struct Params{
	Params(){
		input = "";
		log_level = "";
		threads = default_threads;

		out = "dbhr-storage.db";
	}
    Params(std::string _input, std::string _log_level,
           uint32_t _threads){
        input = _input;
        log_level = _log_level;
        threads = _threads;

        if (input.find("/") != std::string::npos){
            out.assign(input, 0, input.rfind("/") + 1);
        }
        out += "dbhr-storage.db";
    }
    std::string input;
    std::string log_level;
    uint32_t threads;
    std::string out;
} typedef Params;

struct hash_this{
	hash_this(){
		cf_name = std::string("");
		key = std::string("");
        value = std::string("");
    }
	hash_this(std::string _cf_name, std::string _key, std::string _value){
        key = _key;
        value = _value;
    }
	std::string cf_name;
    std::string key;
    std::string value;
} typedef hash_this;

struct print_this{
	print_this(){
        key = std::string("");
        hash = nullptr;
    }
	print_this(std::string _cf_name, std::string _key, std::string* _hash){
		cf_name = _cf_name;
        key = _key;
        hash = _hash;
    }
	std::string cf_name;
    std::string key;
    std::string* hash;
} typedef print_this;

class BD_Hasher{
public:
    explicit BD_Hasher(Params &parameters){
        log_level = parameters.log_level;
        notes_in_work.store(0);
        threads = parameters.threads;
        out = parameters.out;

        download_finished = false;
        hashing_finished = false;

        processing_queue = new std::queue <hash_this>;
        output_queue = new std::queue <print_this>;
//        init();
    }
    ~BD_Hasher(){
        delete processing_queue;
        delete output_queue;
    }

private:
	void log_it(std::stringstream &ss){
    	if (log_level == "debug"){
    		BOOST_LOG_TRIVIAL(debug) << "kuku";
    	} else if (log_level == "info"){
		    BOOST_LOG_TRIVIAL(info) << ss.str();
    	} else if (log_level == "warning"){
		    BOOST_LOG_TRIVIAL(warning) << ss.str();
		} else if (log_level == "error"){
		    BOOST_LOG_TRIVIAL(error) << ss.str();
		} else if (log_level == "fatal"){
		    BOOST_LOG_TRIVIAL(fatal) << ss.str();
		} else if (log_level == "trace"){
		    BOOST_LOG_TRIVIAL(trace) << ss.str();
		}

    }
	static void hasher(ctpl::thread_pool *working_threads) {
    std::string str_before_hash;
    std::string str_after_hash;
    hash_this struct_before_hash;
     bool empty_queue = true;
    
        while (!finish_him.load()) {
            while (!safe_output.try_lock()){
                std::this_thread::sleep_for(std::chrono::milliseconds(
                        masha_sleeps_seconds));
            }
            empty_queue = output_queue->empty();
            safe_output.unlock();
        }    
        while (!empty_queue) {
            while (!safe_output.try_lock()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(
                        masha_sleeps_seconds));
            }
            std::struct prepare = output_queue->front();
            output_queue->pop();  
            safe_output.unlock();

            network_threads->push(std::bind(&BD_Hasher::hasher,
                this, working_threads));

            std::string str_before_hash = prepare.key + prepare.value;
            picosha2::hash256_hex_string(str_before_hash, str_after_hash);
            print_this (struct_before_hash.cf_name, struct_before_hash.key, *str_after_hash);
            
            empty_queue = output_queue->empty();
            
        }
}
	
//    void init()
//    {
//	    boost::log::register_simple_formatter_factory
//			    <boost::log::trivial::severity_level, char>("Severity");
//	    logging::add_file_log
//			    (
//					    logging::keywords::file_name = "log_%N.log",
//					    logging::keywords::rotation_size = SIZE_FILE,
//					    logging::keywords::time_based_rotation =
//							    boost::log::sinks::file::rotation_at_time_point{0,
//							                                                    0, 0},
//					    logging::keywords::format =
//							    "[%TimeStamp%] [%Severity%] %Message%");
//
//	    add_console_log(
//			    std::cout,
//			    logging::keywords::format
//					    = "[%TimeStamp%] [%Severity%]: %Message%");
//	    logging::add_common_attributes();
//    }
    void downloading_notes(ctpl::thread_pool *network_threads){

    	//_---------------------------------------------CODE ROCKSDB_______________------------------

/*
// Created by kirill on 09.05.2020.
//
// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
#include <iostream>

#include <cstdio>
#include <string>
#include <vector>

#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>

		using namespace rocksdb;

		std::string kDBPath = "rocksdb_column_families_example";

		int main() {
			// open DB
//	Options options;
//	options.create_if_missing = true;
			DB* db;
			Status s;// = DB::Open(options, kDBPath, &db);
//	assert(s.ok());
//
//	// create column family
//	ColumnFamilyHandle* cf1;
//	s = db->CreateColumnFamily(ColumnFamilyOptions(), "cf_1", &cf1);
//	assert(s.ok());
//	ColumnFamilyHandle* cf2;
//	s = db->CreateColumnFamily(ColumnFamilyOptions(), "cf_2", &cf2);
//	assert(s.ok());
//
//	// close DB
//	s = db->DestroyColumnFamilyHandle(cf1);
//	s = db->DestroyColumnFamilyHandle(cf2);
//	assert(s.ok());
//	delete db;

			// open DB with two column families
			std::vector <std::string> cf_names;
			DB::ListColumnFamilies(DBOptions(), kDBPath, &cf_names);

			std::vector<ColumnFamilyDescriptor> column_families;
			std::cout << "Column families:" << std::endl;
			for (auto name : cf_names){
				std::cout << name << std::endl;
				column_families.push_back(ColumnFamilyDescriptor(
						name, ColumnFamilyOptions()));
			}
			std::cout << std::endl;
			std::vector<ColumnFamilyHandle*> handles;
			s = DB::Open(DBOptions(), kDBPath, column_families, &handles, &db);
			assert(s.ok());

			std::cout << handles.size() << " = size" << std::endl;

//	WriteBatch batch;
//	for (int i = 0;  i < handles.size(); ++i) {
//		for (int j = 0; j < 7; ++j) {
//			// atomic write
//			std::string key = "key" + std::to_string(j);
//			std::string value = "value" + std::to_string(j);
//			batch.Put(handles[i], Slice(key), Slice(value));
//		}
//	}
//	s = db->Write(WriteOptions(), &batch);
//	assert(s.ok());
//
			std::vector< Iterator* > iterators;
			s = db->NewIterators(ReadOptions(), handles, &iterators);

			auto iterator_column_names = cf_names.begin();
			for(auto it : iterators) {
				std::cout << *iterator_column_names << std::endl << std::endl;
				for (it->SeekToFirst(); it->Valid(); it->Next()) {
					std::cout << it->key().data() << ": "
					          << it->value().ToString()
					          << std::endl;
				}
				iterator_column_names++;
				delete it;
			}

			// close db
			for (auto handle : handles) {
				s = db->DestroyColumnFamilyHandle(handle);
				assert(s.ok());
			}
			delete db;

			return 0;
		}
*/
//-------------------------------------------------------------------------------------------------
















//        bool empty_queue = true;
//        while (empty_queue && !finish_him.load()) {
//            while (!safe_downloads.try_lock()) {
//                std::this_thread::sleep_for(std::chrono::milliseconds(
//                        kirill_sleeps_seconds));
//            }
//            empty_queue = download_queue->empty();
//            safe_downloads.unlock();
//        }
//        if (finish_him.load()) {
//            return;
//        }
//
//        while (!safe_downloads.try_lock()){
//            std::this_thread::sleep_for(std::chrono::milliseconds(
//                    kirill_sleeps_seconds));
//        }
//        download_this url_to_download = download_queue->front();
//        download_queue->pop();
//        safe_downloads.unlock();
//
//        network_threads->push(std::bind(&MyCrawler::downloading_pages,
//                                        this, network_threads));
//
//        std::mutex down_load;
//        std::string website("");
//        down_load.lock();
//        if (url_to_download.protocol){
//            website = get_https_page(url_to_download.url,
//                                     HTTPS_PORT, url_to_download.target);
//            if ((website == std::string(NOT_FOUND)) ||
//                (website == std::string("")) ||
//                (website.find(MOVED_PERMANENTLY) != std::string::npos)) {
//                website = get_https_page((WWW + url_to_download.url),
//                                         HTTPS_PORT, url_to_download.target);
//            }
//        } else {
//            website = get_http_page((WWW + url_to_download.url),
//                                    HTTP_PORT, url_to_download.target);
//        }
//        down_load.unlock();
//        parse_this site(url_to_download.url, url_to_download.target,
//                        website, url_to_download.current_depth,
//                        url_to_download.protocol);
//        while (!safe_processing.try_lock()) {
//            std::this_thread::sleep_for(std::chrono::milliseconds(
//                    kirill_sleeps_seconds));
//        }
//        processing_queue->push(site);
//        safe_processing.unlock();
    }
    void parsing_notes(ctpl::thread_pool *parsing_threads) {
//        bool empty_queue = true;
//        while (empty_queue && !finish_him.load()) {
//            while (!safe_processing.try_lock()) {
//                std::this_thread::sleep_for(std::chrono::milliseconds(
//                        dima_sleeps_seconds));
//            }
//            empty_queue = processing_queue->empty();
//            safe_processing.unlock();
//        }
//        if (finish_him.load()) {
//            return;
//        }
//
//        download_this download_package;
//        parse_this parse_package;
//
//        while (!safe_processing.try_lock()) {
//            std::this_thread::sleep_for(std::chrono::milliseconds(
//                    dima_sleeps_seconds));
//        }
//        parse_package = processing_queue->front();
//        processing_queue->pop();
//        safe_processing.unlock();
//
//        parsing_threads->push(std::bind(&MyCrawler::parsing_pages,
//                                        this, parsing_threads));
//
//        std::vector<std::string> img_references;
//        std::vector<std::string> href_references;
//        std::vector<std::string> paths_in_hrefs;
//        std::vector<bool> https_protocol;
//
//        GumboOutput* output = gumbo_parse(parse_package.website.c_str());
//        search_for_links(output->root, img_references, href_references);
//        true_site(parse_package.url, parse_package.protocol);
//        all_right_references(img_references, href_references,
//                             paths_in_hrefs, parse_package.url,
//                             parse_package.target);
//        about_https(https_protocol, href_references);
//
//        if (parse_package.current_depth) {
//            if (!href_references.empty()) {
//                bool first_one = true;
//                while (!href_references.empty()) {
//                    download_package.url = href_references[
//                            href_references.size() - odin];
//                    download_package.current_depth =
//                            parse_package.current_depth - odin;
//                    download_package.target = paths_in_hrefs[
//                            paths_in_hrefs.size() - odin];
//                    download_package.protocol = https_protocol[
//                            https_protocol.size() - odin];
//                    while (!safe_downloads.try_lock()) {
//                        std::this_thread::sleep_for(std::chrono::milliseconds(
//                                dima_sleeps_seconds));
//                    }
//                    download_queue->push(download_package);
//                    safe_downloads.unlock();
//                    href_references.pop_back();
//                    paths_in_hrefs.pop_back();
//                    https_protocol.pop_back();
//                    if (!first_one){
//                        sites_in_work.store(sites_in_work.load() + odin);
//                    } else {
//                        first_one = false;
//                    }
//                }
//            } else {
//                sites_in_work.store(sites_in_work.load() - odin);
//            }
//        } else {
//            sites_in_work.store(sites_in_work.load() - odin);
//        }
//        std::cout << "Sites in work " << sites_in_work.load()
//                  << ".............." << std::endl;
//
//        while (!img_references.empty()) {
//            while (!safe_output.try_lock()) {
//                std::this_thread::sleep_for(std::chrono::milliseconds(
//                        dima_sleeps_seconds));
//            }
//            output_queue->push(img_references[img_references.size() - odin]);
//            safe_output.unlock();
//            img_references.pop_back();
//        }
//        gumbo_destroy_output(&kGumboDefaultOptions, output);
//
//        if (!sites_in_work.load()) {
//            finish_him.store(true);
        }

    void writing_output(){
//        std::ofstream ostream;
//        ostream.open(out);
//        if (!ostream.is_open()){
//            std::cout << "The file " << out << " is not open" << std::endl;
//            throw std::logic_error("Output file is not opened!:(! ");
//        }
//        while (!finish_him.load()) {
//            while (!safe_output.try_lock()){
//                std::this_thread::sleep_for(std::chrono::milliseconds(
//                        masha_sleeps_seconds));
//            }
//            bool empty_queue = output_queue->empty();
//            safe_output.unlock();
//            while (!empty_queue) {
//                while (!safe_output.try_lock()) {
//                    std::this_thread::sleep_for(std::chrono::milliseconds(
//                            masha_sleeps_seconds));
//                }
//                std::string shit_to_write = output_queue->front();
//                ostream << shit_to_write << std::endl;
//                output_queue->pop();
//                empty_queue = output_queue->empty();
//                safe_output.unlock();
//            }
//        }
//        ostream.close();
    }

public:
    void i_like_to_hash_it_hash_it(){
    	std::stringstream ss;
    	ss << "ERROR MESSAGE";
	    log_it(ss);

        try {
            ctpl::thread_pool working_threads(threads);

            working_threads.push(std::bind(&BD_Hasher::downloading_notes,
                                           this, &working_threads));
            working_threads.push(std::bind(&BD_Hasher::parsing_notes,
                                           this, &working_threads));
            writing_output();
        } catch (std::logic_error const& e){
            std::cout << e.what();
        } catch (...){
            std::cout << "Unknown error! Ask those stupid coders:0";
        }
    }

private:
    std::string log_level;
    uint32_t threads;
    std::string out;

    std::atomic_bool download_finished;
    std::atomic_bool hashing_finished;
    std::atomic_uint notes_in_work;

    std::queue <hash_this> * processing_queue;
    std::mutex safe_processing;

    std::queue <print_this> * output_queue;
    std::mutex safe_output;
};

Params parse_cmd(const po::variables_map& vm){
    Params cmd_params;
    if (vm.count("log-level"))
        cmd_params.log_level = vm["log-level"].as<std::string>();
    if (vm.count("thread-count"))
        cmd_params.threads = vm["thread-count"].as<uint32_t>();
    if (vm.count("output"))
        cmd_params.out = vm["output"].as<std::string>();
	if (vm.count("input-file"))
		cmd_params.input = vm["input-file"].as<std::string>();
    return cmd_params;
}
Params command_line_processor(int argc, char* argv[]){
	po::options_description desc("General options");
    std::string task_type;
    desc.add_options()
            ("help,h", "Show help")
            ("type,t", po::value<std::string>(&task_type),
             "Select task: hash");

    po::options_description parse_desc("Work options");
    parse_desc.add_options()
            ("log-level,l", po::value<std::string>()->default_value("error"),
                    "= \"info\"|\"warning\"|\"error\"\n"
                    "\t= default: \"error\"")
            ("thread-count,t", po::value<uint32_t>()->default_value(default_threads),
             "=Input number of working threads\n"
             "\t= default: count of logical core")
		    ("input-file", po::value<std::string>()->required(), "input file")
            ("output,O", po::value<std::string>(), "Output parameters file");
	po::positional_options_description file_name;
	file_name.add("input-file", -1);
    po::variables_map vm;
    try {
        auto parsed = po::command_line_parser(argc,
         argv).options(desc).allow_unregistered().run();
        po::store(parsed, vm);
        po::notify(vm);
        if (task_type == "hash") {
	         desc.add(parse_desc);
	        parsed = po::command_line_parser(argc,
	                                              argv).options(desc).positional(file_name).allow_unregistered().run();
	        po::store(parsed, vm);
            po::notify(vm);
            Params cmd_params = parse_cmd(vm);
            return cmd_params;
        } else {
            std::cout << "Usage:" << "\n"
                      << "  dbhr [options] <path/to/input/storage.db>"
                      << std::endl;
            desc.add(parse_desc);
            std::cout << desc << std::endl;
	        exit(0);
        }
    } catch(std::exception& ex) {
        std::cout << ex.what() << std::endl;
        exit(-1);
    }
    return Params();
}

int main(int argc, char* argv[]){
    try {
        Params cmd_params = command_line_processor(argc, argv);
        BD_Hasher hasher(cmd_params);
        hasher.i_like_to_hash_it_hash_it();
    } catch(std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
    return 0;
}
