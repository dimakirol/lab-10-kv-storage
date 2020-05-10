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
	    randomizer += ThreadID;
        int64_t tmp_randomizer = randomizer;
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
	print_this(std::string _cf_name, std::string _key, std::string _hash){
		cf_name = _cf_name;
        key = _key;
        hash = _hash;
    }
	std::string cf_name;
    std::string key;
    std::string hash;
} typedef print_this;

class BD_Hasher{
public:
    explicit BD_Hasher(Params &parameters){
    	source = parameters.input;
        log_level = parameters.log_level;
        notes_in_work.store(0);
        threads = parameters.threads;
        out = parameters.out;

	    cf_names_are_ready.store(false);

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
	void feel_db(std::string name){
		Options options;
		options.create_if_missing = true;
		DB* db;
		Status s;
		std::vector <std::string> cf_names;
		DB::ListColumnFamilies(DBOptions(), name, &cf_names);

		std::vector<ColumnFamilyDescriptor> column_families;
//		std::cout << "Column families:" << std::endl;
		for (auto name : cf_names){
//			std::cout << name << std::endl;
			column_families.push_back(ColumnFamilyDescriptor(
					name, ColumnFamilyOptions()));
		}
//		std::cout << std::endl;
		std::vector<ColumnFamilyHandle*> handles;
		s = DB::Open(DBOptions(), name, column_families, &handles, &db);
		assert(s.ok());

		RandomString random_str;
		WriteBatch batch;
		for (uint32_t i = 0; i < column_families.size(); ++i){
			for (int j = 0; j < notes_number; ++j) {
				std::string key;
				if (!i){
					key = "key_default_" + std::to_string(j);
				} else {
					key = "key_" + cf_names[i] + "_" + std::to_string(j);
				}
				std::string value = random_str.SetRandomValue(j);
				batch.Put(handles[i], Slice(key), Slice(value));
			}
			s = db->Write(WriteOptions(), &batch);
			assert(s.ok());
		}
		for (auto handle : handles) {
			s = db->DestroyColumnFamilyHandle(handle);
			assert(s.ok());
		}
		delete db;

    }
	void make_db(std::string name, std::vector <std::string> cf_names){
    	//open DB
		Options options;
		options.create_if_missing = true;
		DB* db;
		Status s = DB::Open(options, name, &db);

		ColumnFamilyHandle* cf_handle;
//		uint32_t counter = 0;
		for (auto column_family : cf_names) {
			// create column family
			s = db->CreateColumnFamily(ColumnFamilyOptions(),
			                           column_family, &cf_handle);
			assert(s.ok());

			// close DB
			s = db->DestroyColumnFamilyHandle(cf_handle); //????????????????????????????????????????
			assert(s.ok());
		}
		delete db;
    }
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
    void downloading_notes(ctpl::thread_pool *network_threads){
		//open DB
	    Options options;
	    options.create_if_missing = true;
	    DB* db;
	    Status s;
		DB::ListColumnFamilies(DBOptions(), source, &cf_names);
	    cf_names_are_ready.store(true);

		std::vector<ColumnFamilyDescriptor> column_families;
		std::cout << "Column families:" << std::endl;
		for (auto name : cf_names){
			std::cout << name << std::endl;
			column_families.push_back(ColumnFamilyDescriptor(
					name, ColumnFamilyOptions()));
		}
		std::cout << std::endl;
		std::vector<ColumnFamilyHandle*> handles;
		s = DB::Open(DBOptions(), source, column_families, &handles, &db);
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
	    std::string str_before_hash;
	    std::string str_after_hash;
	    hash_this struct_before_hash;
	    bool empty_queue = true;

	    while (!download_finished.load() && !empty_queue) {
		    while (!safe_processing.try_lock()){
			    std::this_thread::sleep_for(std::chrono::milliseconds(
					    masha_sleeps_seconds));
		    }
		    empty_queue = processing_queue->empty();
		    safe_processing.unlock();
	    }
	    if (download_finished.load() && empty_queue){
		    hashing_finished.store(true);
	    }

	    while (!safe_processing.try_lock()) {
		    std::this_thread::sleep_for(std::chrono::milliseconds(
				    masha_sleeps_seconds));
	    }
	    struct_before_hash = processing_queue->front();
	    processing_queue->pop();
	    safe_processing.unlock();

	    parsing_threads->push(std::bind(&BD_Hasher::parsing_notes,
	                                    this, parsing_threads));

	    str_before_hash = struct_before_hash.key + struct_before_hash.value;
	    picosha2::hash256_hex_string(str_before_hash, str_after_hash);
	    print_this struct_after_hash(struct_before_hash.cf_name,
	                                 struct_before_hash.key, str_after_hash);

	    while (!safe_output.try_lock()) {
		    std::this_thread::sleep_for(std::chrono::milliseconds(
				    masha_sleeps_seconds));
	    }
	    output_queue->push(struct_after_hash);
	    safe_output.unlock();
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
        try {
        	std::vector <std::string> cf_names;
        	int count = 1;
        	for (auto cf_name : cf_names){
        		cf_name = "cf_" + std::to_string(count);
        	}
	        make_db(source, cf_names);
	        feel_db(source);
	        std::stringstream ss;
	        ss << "Database successfully created";
	        log_it(ss);

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
	std::string source;
    std::string log_level;
    uint32_t threads;
    std::string out;

	std::atomic_bool cf_names_are_ready;
	std::vector <std::string> cf_names;

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
