// Copyright 2019 dimakirol <your_email>

#include <header.hpp>

struct Params{
    Params(std::string _input, std::string _log_level;
           uint32_t _log_level){
        input = _input
        log_level = _log_level;
        threads = _log_level;

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
    _download_this(){
        key = std::string("");
        value = std::string("/");
    }
    _download_this(std::string _key, std::string _value){
        key = _key;
        value = _value;
    }
    std::string key;
    std::string value;
} typedef hash_this;

struct print_this{
    _parse_this(){
        key = std::string("");
        hash = nullptr;
    }
    _parse_this(std::string _key, std::string _hash){
        key = _key;
        hash = _hash;
    }
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

        processing_queue = new std::queue <parse_this>;
        output_queue = new std::queue <std::string>;
    }
    ~BD_Hasher(){
        delete processing_queue;
        delete output_queue;
    }

private:
    void init()
    {
        boost::log::register_simple_formatter_factory
                <boost::log::trivial::severity_level, char>("Severity");
        logging::add_file_log
                (
                        keywords::file_name = LOG_FILE_NAME,
                        keywords::rotation_size = ROTATION_SIZE_A *
                                                  ROTATION_SIZE_B * ROTATION_SIZE_H,
                        keywords::time_based_rotation =
                                sinks::file::rotation_at_time_point(0, 0, 0),
                        keywords::format =
                                "[%TimeStamp%][%ThreadID%][%Severiti%]: %Message%");
        logging::add_console_log
                (
                        std::cout,
                        logging::keywords::format =
                                "[%TimeStamp%] [%ThreadID%] [%Severity%]: %Message%");
        logging::add_common_attributes();
    }
    void downloading_notes(ctpl::thread_pool *network_threads){
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
        init();
        //log example : BOOST_LOG_TRIVIAL(log_level) <<  "ID: " << id;

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
//        }
    }

private:
    std::string log_level;
    uint32_t threads;
    std::string out;

    std::atomic_bool download_finished;
    std::atomic_bool hashing_finished;
    std::atomic_uint notes_in_work;

    std::queue <parse_this> * processing_queue;
    std::mutex safe_processing;

    std::queue <std::string> * output_queue;
    std::mutex safe_output;
};

Params parse_cmd(const po::variables_map& vm){
    Params cmd_params;
    if (vm.count("input-file"))
        cmd_params.input = vm["input-file"].as<std::string>();
    if (vm.count("log-level"))
        cmd_params.log_level = vm["log-level"].as<std::string>();
    if (vm.count("thread-count"))
        cmd_params.threads = vm["thread-count"].as<uint32_t>();
    if (vm.count("output"))
        cmd_params.out = vm["output"].as<std::string>();
    return cmd_params;
}
Params command_line_processor(int argc, char* argv[]){
    po::options_description desc("General options");
    std::string task_type;
    desc.add_options()
            ("help,h", "Show help")
            ("type,t", po::value<std::string>(&task_type),
             "Select task: hash");
    desc.add("input-file", -1);
    po::options_description parse_desc("Work options");
    parse_desc.add_options()
            ("log-level,l", po::value<std::string>()->default_value("error"),
                    "= \"info\"|\"warning\"|\"error\"\n"
                    "\t\t\t\t\t\t\t\t= default: \"error\"")
            ("thread-count,t", po::value<uint32_t>()->default_value(default_threads),
             "Input number of working threads"
             "\t\t\t\t\t\t\t\t= default: count of logical core")
            ("output,O", po::value<std::string>(), "Output parameters file");
    po::variables_map vm;
    try {
        po::parsed_options parsed = po::command_line_parser(argc,
                                                            argv).options(desc).allow_unregistered().run();
        po::store(parsed, vm);
        po::notify(vm);
        if (task_type == "hash") {
            desc.add(parse_desc);
            po::store(po::parse_command_line(argc, argv, desc), vm);
            Params cmd_params = parse_cmd(vm);
            return cmd_params;
        } else {
            std::cout << "Usage:\n" << "\n"
                      << "  dbhr [options] <path/to/input/storage.db>"
                      << std::endl;
            desc.add(parse_desc);
            std::cout << desc << std::endl;
        }
    } catch(std::exception& ex) {
        std::cout << desc << std::endl;
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
