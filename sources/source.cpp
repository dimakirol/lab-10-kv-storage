// Copyright 2019 dimakirol <your_email>

#include <header.hpp>


class RandomString {
public:
//
    RandomString() {
        alpha =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "1234567890";
        auto now = static_cast<unsigned int>(time(nullptr));
        randomizer = static_cast<uint64_t>(rand_r(&now) % alpha.length());
        random_string.assign(alpha, 0, value_size);
    }

    std::string SetRandomValue(int ThreadID) {
        randomizer += ThreadID;
        int64_t tmp_randomizer = randomizer;
        random_string.erase(tmp_randomizer % random_string.size(), 1);
        random_string = random_string + alpha[tmp_randomizer % alpha.size()];
        return random_string;
    }

    std::atomic_int64_t randomizer;
    std::string alpha;
    std::string random_string;
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
        cf_name = _cf_name;
        key = _key;
        value = _value;
    }
    std::string cf_name;
    std::string key;
    std::string value;
} typedef hash_this;

struct print_this{
    print_this(){
        cf_name = std::string("");
        key = std::string("");
        hash = std::string("");
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
        std::vector <std::string> cf_names_;
        DB::ListColumnFamilies(DBOptions(), name, &cf_names_);

        std::vector<ColumnFamilyDescriptor> column_families;

        for (auto name : cf_names_){
            column_families.push_back(ColumnFamilyDescriptor(
                                        name, ColumnFamilyOptions()));
        }
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
                    key = "key_" + cf_names_[i] + "_" + std::to_string(j);
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
//
    }
    void make_db(std::string name, std::vector <std::string> &cf_names_){
        Options options;
        options.create_if_missing = true;
        DB* db;
        Status s = DB::Open(options, name, &db);
        assert(s.ok());

        std::vector <ColumnFamilyHandle*> handles;
        ColumnFamilyHandle* cf;

        for (auto column_family : cf_names_) {
            if (column_family == "default")
            continue;
            s = db->CreateColumnFamily(ColumnFamilyOptions(),
                                       column_family, &cf);
            assert(s.ok());
            handles.push_back(cf);
        }

        for (auto handle : handles) {
            s = db->DestroyColumnFamilyHandle(handle);
            assert(s.ok());
        }
        delete db;
    }
    void log_it(){
        if (log_level == "debug"){
            BOOST_LOG_TRIVIAL(debug) << ss.str() << std::endl;
        } else if (log_level == "info"){
            BOOST_LOG_TRIVIAL(info) << ss.str() << std::endl;
        } else if (log_level == "warning"){
            BOOST_LOG_TRIVIAL(warning) << ss.str() << std::endl;
        } else if (log_level == "error"){
            BOOST_LOG_TRIVIAL(error) << ss.str() << std::endl;
        } else if (log_level == "fatal"){
            BOOST_LOG_TRIVIAL(fatal) << ss.str() << std::endl;
        } else if (log_level == "trace"){
            BOOST_LOG_TRIVIAL(trace) << ss.str() << std::endl;
        }
        ss.str("");
    }
    void downloading_notes(){
        std::vector <hash_this> please_hash_it;
        DB* db;
        Status s;
        DB::ListColumnFamilies(DBOptions(), source, &cf_names);
        cf_names_are_ready.store(true);

        std::vector<ColumnFamilyDescriptor> column_families;

        for (auto name : cf_names){
            column_families.push_back(ColumnFamilyDescriptor(
                                        name, ColumnFamilyOptions()));
        }
        std::vector<ColumnFamilyHandle*> handles;
        s = DB::Open(DBOptions(), source, column_families, &handles, &db);
        assert(s.ok());

        std::vector< Iterator* > iterators;
        s = db->NewIterators(ReadOptions(), handles, &iterators);
        assert(s.ok());

        auto iterator_column_names = cf_names.begin();
        uint32_t i = 0;
        for (auto it : iterators) {
            for (it->SeekToFirst(); it->Valid(); it->Next()) {
                please_hash_it.push_back(hash_this(*iterator_column_names,
                                                   it->key().data(),
                                                   it->value().ToString()));
            }
            i++;
            iterator_column_names++;
            delete it;
        }

        for (auto handle : handles) {
            s = db->DestroyColumnFamilyHandle(handle);
            assert(s.ok());
        }
        delete db;

        for (auto element : please_hash_it){
            while (!safe_processing.try_lock()){
                std::this_thread::sleep_for(std::chrono::milliseconds(
                                              kirill_sleeps_seconds));
            }
            processing_queue->push(element);
            safe_processing.unlock();
        }
        download_finished.store(true);
        ss << "All key-value paires successfully read!";
        log_it();
    }
    void parsing_notes(ctpl::thread_pool *parsing_threads) {
        std::string str_before_hash;
        std::string str_after_hash;
        hash_this struct_before_hash;
        bool empty_queue = true;

        while (!download_finished) {
            std::this_thread::yield();
        }

        while (!safe_processing.try_lock()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(
                                           masha_sleeps_seconds));
        }
        struct_before_hash = processing_queue->front();
        processing_queue->pop();
        empty_queue = processing_queue->empty();
        safe_processing.unlock();

        if (!empty_queue) {
            parsing_threads->push(std::bind(&BD_Hasher::parsing_notes,
                                            this, parsing_threads));
        }

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

        if (download_finished.load() && empty_queue){
            hashing_finished.store(true);
            ss << "Hashing is completed!";
            log_it();
        }
    }

    void writing_output() {
        if (!cf_names_are_ready) {
            std::this_thread::sleep_for(std::chrono::milliseconds(
                                            dima_sleeps_seconds));
        }
        make_db(out, cf_names);
        Options options;
        options.create_if_missing = true;
        DB* db;
        Status s;

        std::vector<ColumnFamilyDescriptor> column_families;
        for (auto name : cf_names){
            column_families.push_back(ColumnFamilyDescriptor(
                                       name, ColumnFamilyOptions()));
        }

        std::vector<ColumnFamilyHandle*> handles;
        s = DB::Open(DBOptions(), out, column_families, &handles, &db);
        assert(s.ok());

        print_this object_to_print;

        bool empty_queue = true;
        while (empty_queue) {
            while (!safe_output.try_lock()) {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(dima_sleeps_seconds));
            }
            empty_queue = output_queue->empty();
            safe_output.unlock();
        }

        while (!(hashing_finished && empty_queue)) {
            while (!safe_output.try_lock()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(
                                                dima_sleeps_seconds));
            }
            object_to_print = output_queue->front();
            output_queue->pop();
            safe_output.unlock();

            int i = 0;
            for (auto column_family_name : cf_names) {
                if (column_family_name == object_to_print.cf_name) {
                    s = db->Put(WriteOptions(), handles[i],
                     Slice(object_to_print.key), Slice(object_to_print.hash));
                    break;
                }
                ++i;
            }
            empty_queue = true;
            while (empty_queue) {
                empty_queue = output_queue->empty();
                std::cout << "Notes in process: " <<
                output_queue->size() << std::endl;
                if (empty_queue){
                    if (hashing_finished.load())
                    break;
                }
            }
        }

        for (auto handle : handles) {
            s = db->DestroyColumnFamilyHandle(handle);
            assert(s.ok());
        }
        delete db;
        ss << "End database done!!!!!!!!!!!!!!!!!!!!!!";
        log_it();
    }

public:
    void i_like_to_hash_it_hash_it(){
        try {
            ss << "Starting........";
            log_it();
            std::vector<std::string> cf_names_;
            for (int i = 0; i < cf_names_number; ++i) {
                cf_names_.push_back(std::string("cf_" + std::to_string(i)));
            }
            make_db(source, cf_names_);
            feel_db(source);
            ss << "Database successfully created";
            log_it();


            ctpl::thread_pool working_threads(threads);

            working_threads.push(std::bind(&BD_Hasher::downloading_notes,
                                           this));

            working_threads.push(std::bind(&BD_Hasher::parsing_notes,
                                           this, &working_threads));
            writing_output();

            print_rezult(source, out);
//
        } catch (std::logic_error const& e){
            std::cout << e.what() << " was an error!";
        } catch (...){
            std::cout << "Unknown error! Ask those stupid coders:0";
        }
//
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

    std::stringstream ss;
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
