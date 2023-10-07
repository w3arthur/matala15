#pragma once
#include <string>	//try to remove the string
#include <map>
#include <vector>
#include <string>
#include <variant>
#include <typeindex>
#include <winsock2.h> // TODO: remove it from here

#include <cstring>

namespace client
{
	//TODO: set sizes
	struct transfer
	{
		//bool is_available;
		std::string ip_address{};
		unsigned short int port{};
		std::string name{};
		std::string file_address{};
	};

	struct identyfier
	{
		bool is_available{};
		std::string name{};
		std::string id{};	//unique//ascii hex
		std::string private_key{};
	};





	// Request			all request codes
	//Request structs


	enum class request_code : uint16_t
	{
		request_error = 0,
		registration = 1025,
		sending_public_key = 1026,
		reconnect_request = 1027,
		file_transfer = 1028,
		crc_request = 1029,
		crc_wrorng_resending = 1030,
		crc_wrorng_forth_time_stop = 1031,
	};

#pragma pack(push, 1)
	template <class Request_Class>
	struct request_payload : Request_Class
	{
	};

	struct request1025 
	{
		void set_name(std::string name)
		{
			strncpy_s(this->name, name.c_str(), sizeof(this->name));
			this->name[name.size()] = '\0';
		}
		char name[255]{};
	};
	struct request1026 : request1025
	{
		//char name[255]{};
		char public_key[160]{};
	};
	struct request1027 : request1025
	{
		//char name[255]{}; 
	};
	struct request1028
	{
		uint32_t content_size{};
		std::vector<char> message_content{};
	};
	struct request1029 
	{
		char file_name[255]{}; 
	};
	struct request1030 : request1029
	{ 
		//char file_name[255]{};
	};
	struct request1031 : request1029
	{
		//char file_name[255]{};
	};



	const std::map<std::type_index, uint16_t> request_code_map = {
		{typeid(request1025), 1025}, //
		{typeid(request1026), 1026}, //request_code::sending_public_key
		{typeid(request1027), 1027}, //request_code::reconnect_request
		{typeid(request1028), 1028}, //request_code::file_transfer
		{typeid(request1029), 1029}, //request_code::crc_request
		{typeid(request1030), 1030}, //request_code::crc_wrorng_resending
		{typeid(request1031), 1031}, //request_code::crc_wrorng_forth_time_stop
	};


	// Define the updated header structure
	template <class Request_Class>
	struct request_header
	{
		//request_header()
		//{
		//	strncpy_s(this->client_id, config::client_id, sizeof(this->client_id));
		//}
		char client_id[16]{"4465345345"};
		uint8_t version{ config::client_version };
		uint16_t code{ request_code_map.at(typeid(Request_Class)) };
		uint32_t payload_size{};
	};



	template <class Request_Class>
	struct request
	{
		request_header<Request_Class> header{};
		request_payload<Request_Class> payload{};
	};
#pragma pack(pop)





	enum class response_code : uint16_t
	{
		response_error = 0,
		register_success = 2100,
		register_fail = 2101,
		public_key_received_sending_aes = 2102,
		file_received_successfully_with_crc = 2103,
		approval_message_receiving = 2104,	//response to request  1029 / 1031
		approval_reconnection_request_send_crypted_aes = 2105,
		denined_reconnection_request_client_should_register_again = 2106,	//the client not registered or without public key
		global_server_error = 2107,
	};




	// Response			all response codes
#pragma pack(push, 1)
	template <class Response_Class>
	struct response_payload : Response_Class 
	{
	};

	struct response2100 
	{
		char client_id[16]{}; 
	};
	struct response2101 
	{
	};
	struct response2102 : response2100
	{
		//char client_id[16]{};
		std::string aes_key{};
	};
	struct response2103 : response2100
	{
		//char client_id[16]{};
		unsigned int content_size{};	//after encryption
		char file_name[255]{};
		char cksum[4]{}; //crc
	};
	struct response2104 : response2100 
	{
		//char client_id[16]{}; 
	};
	struct response2105 : response2100
	{
		//char client_id[16]{}; 
	};
	struct response2106 : response2100
	{
		//char client_id[16]{}; 
	};
	struct response2107
	{
	};
	struct responseError 
	{
	};


	struct response_header
	{
		response_header() = default;
		response_header(const std::vector<char>& data)
		{
			std::memcpy(this, data.data(), sizeof(this));
			this->payload_size = ntohl(this->payload_size);
		}

		uint8_t version{};
		short int response_code_value{};//0
		uint32_t payload_size{};
		
	};
#pragma pack(pop)




	struct response
	{
		response() = default;
		response(const std::vector<char>& data) : header(data), payload()
		{
			payload.reserve(header.payload_size);
			set_response_code(header.response_code_value);
		}

		response_code get_response_code()
		{
			return m_response_code;
		}

		response_header header{};
		std::vector<char> payload{};
		//response_payload<Response_Class> payload{};

	private:
		void set_response_code(uint16_t response_code_value)
		{
			switch (ntohs(response_code_value))
			{
			case 2100: m_response_code = response_code::register_success; break;
			case 2101: m_response_code = response_code::register_fail; break;
			case 2102: m_response_code = response_code::public_key_received_sending_aes; break;
			case 2103: m_response_code = response_code::file_received_successfully_with_crc; break;
			case 2104: m_response_code = response_code::approval_message_receiving; break;
			case 2105: m_response_code = response_code::approval_reconnection_request_send_crypted_aes; break;
			case 2106: m_response_code = response_code::denined_reconnection_request_client_should_register_again; break;
			case 2107: m_response_code = response_code::global_server_error; break;
			default: response_code::response_error;
			}
		}

		response_code m_response_code{ response_code::response_error };
	};




}