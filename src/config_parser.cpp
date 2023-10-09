#include <config_parser.h>
#include <iostream>
#include <fstream>
#include <regex>

namespace validate {
	inline bool key_value_pair(const std::string &line)
	{
		std::regex 	pat { R"([a-zA-Z_]+ = \S+$)" };
		std::smatch res;

		return std::regex_match(line, res, pat);
	}

	inline bool section(const std::string &str)
	{ return ((str[0] == '[') && (str[str.size()-1] == ']')); }
	
	namespace value {
		inline bool host(const std::string &host)
		{

			return true;
		}

		inline bool service(const unsigned int service)
		{

			return true;
		}
	}
}

namespace parse {
	/**
	 * @return  position of value in "line" if "key" found in "line", 
	 * otherwise std::string::npos
	 */
	inline std::string value(const std::string &key, const std::string &line)
	{
		const short kv_dist = 3; // n chars between key and value
		std::string value;
		
		value.assign(line.substr(key.size()+kv_dist));

		return value;
	}

	namespace print_error {
		inline void key_not_found(const std::string &section_name, 
								  const std::string &key)
		{
			std::cerr << "In section: " << section_name << " " 
								   		<< key << " not found!\n"; 
		}

		inline void bad_value(const std::string &section_name,
								const std::string &key, const std::string &val)
		{
			std::cerr << "In section: " << section_name << " bad value " 
										<< val << " for key " << key << std::endl;
		}
	}
 
	struct balancer {
		config::balancer parsed_res;

		bool operator()(const std::vector<std::string> &section)
		{
			for (auto line : section){
				if (line.find(outer_host) != std::string::npos){
					std::string	ip_addr = value(outer_host, line);
					if (!validate::value::host(outer_host)){
						print_error::bad_value(section_name, outer_host, ip_addr);
						return false;
					}
					parsed_res.outer.host.assign(ip_addr);
				} else if (line.find(outer_service) != std::string::npos){
					std::string	str_port = value(outer_service, line);
					unsigned int port = atoi(str_port.c_str());

					if (!validate::value::service(port)){
						print_error::bad_value(section_name, outer_service, str_port);
						return false;
					}
					parsed_res.outer.service = port;	
				} else if (line.find(inner_host) != std::string::npos){
					std::string	ip_addr = value(inner_host, line);
					if (!validate::value::host(inner_host)){
						print_error::bad_value(section_name, inner_host, ip_addr);
						return false;
					}
					parsed_res.inner.host.assign(ip_addr);
				} else if (line.find(inner_service) != std::string::npos){
					std::string	str_port = value(inner_service, line);
					unsigned int port = atoi(str_port.c_str());

					if (!validate::value::service(port)){
						print_error::bad_value(section_name, inner_service, str_port);
						return false;
					}
					parsed_res.inner.service = port;	
				} else if (line.find(max_dg_load) != std::string::npos){
					std::string	s = value(max_dg_load, line);
					unsigned int n = atoi(s.c_str());

					if (n <= 0){
						print_error::bad_value(section_name, max_dg_load, s);
						return false;
					}
					parsed_res.max_dg_load = n;	
				}
			}

			if (parsed_res.outer.host.empty()){
				print_error::key_not_found(section_name, outer_host);
				return false;
			}
			if (!parsed_res.outer.service){
				print_error::key_not_found(section_name, outer_service);
				return false;
			}
			if (parsed_res.inner.host.empty()){
				print_error::key_not_found(section_name, inner_host);
				return false;
			}
			if (!parsed_res.inner.service){
				print_error::key_not_found(section_name, inner_service);
				return false;
			}
			if (!parsed_res.max_dg_load){
				print_error::key_not_found(section_name, max_dg_load);
				return false;
			}
			
			return true;
		}

		private:
			// values
			const std::string section_name{"balancer"};
			const std::string outer_host{"outer_host"};
			const std::string outer_service{"outer_service"};
			const std::string inner_host{"inner_host"};
			const std::string inner_service{"inner_service"};
			const std::string max_dg_load{"max_dg_load"};
	};
	
	struct server {
		config::net_addr parsed_res;

		bool operator()(const std::vector<std::string> &section)
		{
			for (auto line : section){
				if (line.find(host) != std::string::npos){
					std::string	ip_addr = value(host, line);
					if (!validate::value::host(host)){
						print_error::bad_value(section_name, host, ip_addr);
						return false;
					}
					parsed_res.host.assign(ip_addr);
				} else if (line.find(service) != std::string::npos){
					std::string	str_port = value(service, line);
					unsigned int port = atoi(str_port.c_str());

					if (!validate::value::service(port)){
						print_error::bad_value(section_name, service, str_port);
						return false;
					}
					parsed_res.service = port;	
				}
			}

			if (parsed_res.host.empty()){
				print_error::key_not_found(section_name, host);
				return false;
			}
			if (!parsed_res.service){
				print_error::key_not_found(section_name, service);
				return false;
			}
			
			return true;
		}

		private:
			// values
			const std::string section_name{"server"};
			const std::string host{"host"};
			const std::string service{"service"};
	};
}

static inline void perror_in_line(std::string &line, 
								  size_t lnum, const char* err)
{
	std::cerr << "Error (line " << lnum << "): " << '"' << line << '"' << ". " 
														   << err << std::endl;
}

bool config::parse(const std::filesystem::path &conf_path, 
			  	   struct config::main_task &parse_conf)
{
	std::ifstream conf_file;
	conf_file.open(conf_path, std::ios_base::in);

	if (!conf_file.is_open()) {
		std::cerr << "Failed to open " << conf_path.filename() << std::endl;
		return false;
	}

	std::string line;
	size_t line_cnt = 0;

	while(std::getline(conf_file, line)){
		line_cnt++;
		if (validate::section(line)){
			std::string section_name;
			std::vector<std::string> section_content;
			// remove square braces
			section_name.assign(line.substr(1, line.size()-2));

			while(std::getline(conf_file, line)){
				line_cnt++;

				if (line.empty()){
					break;
				}

				if (!validate::key_value_pair(line)){
					perror_in_line(line, line_cnt, 
									"Syntax error! Expected \"key = value\"");
					parse_conf = {};
					return false;
				}
				section_content.push_back(line);
			}

			if (section_name == "balancer"){
				parse::balancer parse{};
				if (!parse(section_content)){	
					return false;
				}
				parse_conf.nlb = parse.parsed_res;
			} else if (section_name == "server"){
				parse::server parse{};
				if (!parse(section_content)){	
					return false;
				}
				parse_conf.server_pool.push_back(parse.parsed_res);
			}
		} else if (!line.empty()){
			// ignore everything what stays out of section 
		} 
	}

	return true;
}
