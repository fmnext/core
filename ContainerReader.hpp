#pragma once
#include <Windows.h>
#include <iostream> 
#include <vector>
#include <string>
#include <regex>
#include <fstream>
#include <filesystem>
#include <chrono>

#include "zip.h"
#include "xcompress.h"

#pragma comment(lib, "zip.lib")
#pragma comment(lib, "xcompress64.lib")

namespace fmnext
{
	class ContainerReader
	{
	public:
		ContainerReader() = default;
		~ContainerReader() = default;

		ContainerReader(const std::string& file) : m_file(file), m_file_name(m_file.stem().string()) {
			int err;
			container = zip_open(file.c_str(), 0, &err);

			if (!container) {
				zip_error_t error;
				zip_error_init_with_code(&error, err);

				printf("\t%s: %s \n", zip_error_strerror(&error), file.c_str());

				//printf("\tERROR: Failed to open the zip container %s \n", file.c_str());
			}
		}

		std::vector<std::string> getMediaTireEntries()
		{
			std::vector<std::string> result{};

			for (int id = 0; id < zip_get_num_entries(container, ZIP_FL_UNCHANGED); ++id)
			{
				std::string entry_name(zip_get_name(container, id, ZIP_FL_UNCHANGED));
				std::string input_name(m_file_name.begin() + 5, m_file_name.end());

				std::smatch match_result{};
				if (std::regex_search(entry_name, match_result, std::regex(input_name + ".modelbin", std::regex_constants::icase)))
				{
					result.push_back(entry_name);

					continue;
				}
			}

			return result;
		}

		std::unordered_map<std::string, std::string> getMediaTextureEntries()
		{
			std::unordered_map<std::string, std::string> result{};
			std::filesystem::path media_name = std::filesystem::path(m_file).stem();

			for (int id = 0; id < zip_get_num_entries(container, ZIP_FL_UNCHANGED); ++id)
			{
				std::string entry_name(zip_get_name(container, id, ZIP_FL_UNCHANGED));

				std::smatch match_result{};
				std::regex_search(entry_name, match_result, std::regex(".swatchbin", std::regex_constants::icase));

				if (!match_result.empty())
				{
					result.try_emplace(entry_name, std::filesystem::path(entry_name).filename().string());

					continue;
				}
			}

			return result;
		}

		std::vector<std::string> getDirectoryEntries(const std::string& name)
		{
			std::vector<std::string> result;

			for (int id = 0; id < zip_get_num_entries(container, ZIP_FL_UNCHANGED); ++id)
			{
				std::string entry_name(zip_get_name(container, id, ZIP_FL_UNCHANGED));

				std::smatch match_result{};
				std::regex_search(entry_name, match_result, std::regex(name, std::regex_constants::icase));

				if (!match_result.empty())
				{
					result.push_back(entry_name);

					continue;
				}

				//break;
			}

			return result;
		}
		/*
		bool extractEntries(const std::filesystem::path& dest) {
			HRESULT hr = S_OK;

			if (!container) {
				printf("ERROR: Failed to open the zip container.\n");
				return false;
			}

			zip_int64_t entry_count = zip_get_num_entries(container, ZIP_FL_UNCHANGED);

			for (zip_int64_t id = 0; id < entry_count; ++id) {
				struct zip_stat fileInfo;
				zip_stat_init(&fileInfo);

				std::vector<char> blob{};

				if (zip_stat_index(container, id, 0, &fileInfo) == 0) {
					printf("File: %s Size: %d Compression Method: %d CRC: %X \n", fileInfo.name, static_cast<int>(fileInfo.size), static_cast<int>(fileInfo.comp_method), fileInfo.crc);

					std::filesystem::path file_dest(dest);
					file_dest /= std::filesystem::path(fileInfo.name);
					file_dest.make_preferred();

					if (!std::filesystem::exists(file_dest.parent_path()))
					{
						std::filesystem::create_directories(file_dest.parent_path());
					}

					zip_flags_t flags = (fileInfo.comp_method == 21) ? ZIP_FL_COMPRESSED : ZIP_FL_UNCHANGED;
					zip_file_t* file = zip_fopen_index(container, id, flags);

					blob.resize(fileInfo.size);

					if (fileInfo.comp_method == 21)
					{
						std::vector<char> tempBlob{};
						tempBlob.resize(fileInfo.comp_size);

						zip_fread(file, &tempBlob.front(), fileInfo.comp_size);

						XMEMDECOMPRESSION_CONTEXT DecompressionContext = 0;
						hr = XMemCreateDecompressionContext(XMEMCODEC_TYPE::XMEMCODEC_LZX, 0, 0, &DecompressionContext);

						zip_int64_t offset = zip_ftell(file);

						char hexData[2] = { 0x50, 0x4B };

						if (std::memcmp(tempBlob.data(), hexData, 2) == 0)
						{
							zip_fread(file, &tempBlob.front(), fileInfo.comp_size);

							continue;
						}

						char padding[4] = { 0xFF, 0xFF, 0xFF, 0xFF };

						if (std::memcmp(tempBlob.data(), padding, 4) == 0)
						{
							zip_fread(file, &tempBlob.front(), fileInfo.comp_size);

							continue;
						}


						try
						{
							hr = XMemDecompress(DecompressionContext, &blob.front(), &fileInfo.size, &tempBlob.front(), fileInfo.comp_size);
						}
						catch (const std::exception& ex)
						{
							XMemResetDecompressionContext(DecompressionContext);
							XMemDestroyDecompressionContext(DecompressionContext);

							printf("%s \n", ex.what());
						}


						try {

							if (std::memcmp(tempBlob.data(), hexData, 2) == 0)
							{
								zip_fread(file, &blob.front(), fileInfo.comp_size);
							}
							else
							{
								hr = XMemDecompress(DecompressionContext, &blob.front(), &fileInfo.size, &tempBlob.front(), fileInfo.comp_size);
							}

							char XCOMPRESS_MAGIC_IDENTIFIER_LZXTDECODE[4] = { 0x0F, 0xF5, 0x12, 0xED };
							char XCOMPRESS_MAGIC_IDENTIFIER_LZXNATIVE[4] = { 0x0F, 0xF5, 0x12, 0xEE };



							XCOMPRESS_FILE_HEADER* hdr = reinterpret_cast<XCOMPRESS_FILE_HEADER*>(tempBlob.data());

							int major_ver = XCOMPRESS_GET_FILE_VERSION_MAJOR(hdr->Version);
							int minor_ver = XCOMPRESS_GET_FILE_VERSION_MINOR(hdr->Version);

							if (std::memcmp(tempBlob.data(), XCOMPRESS_MAGIC_IDENTIFIER_LZXTDECODE, 4) == 0)
							{
								printf("ERROR\n");
							}

							if (std::memcmp(tempBlob.data(), XCOMPRESS_MAGIC_IDENTIFIER_LZXNATIVE, 4) == 0)
							{
								printf("ERROR\n");
							}

							if (std::memcmp(tempBlob.data(), XCOMPRESS_MAGIC_IDENTIFIER_LZXTDECODE, 4) == 0 || std::memcmp(&tempBlob.front(), XCOMPRESS_MAGIC_IDENTIFIER_LZXNATIVE, 4) == 0)
							{

							}




						}
						catch (const std::exception) {
							//XMemResetDecompressionContext(DecompressionContext);
							//XMemDestroyDecompressionContext(DecompressionContext);
						}

						XMemResetDecompressionContext(DecompressionContext);
						XMemDestroyDecompressionContext(DecompressionContext);

						if (FAILED(hr)) {
							printf("ERROR: xmem decompression failed (%08X)\n", static_cast<unsigned int>(hr));
						}

						tempBlob.clear();

						zip_fclose(file);
					}
					else {
						zip_fread(file, &blob.front(), fileInfo.size);
						zip_fclose(file);
					}

					if (true)
					{
						std::ofstream out_file(file_dest, std::ios::binary);
						out_file.write(blob.data(), blob.size());
						out_file.close();

						std::filesystem::last_write_time(file_dest, std::filesystem::last_write_time(m_file));
					}

					blob.clear();
				}
			}

			return true;
		}
		*/

		bool findName(const std::string& name, std::vector<char>& blob) {
			HRESULT hr = S_OK;

			if (!container) {
				//printf("\tERROR: Failed to open the zip container %s. \n", name.c_str());
				return false;
			}

			zip_int64_t id = zip_name_locate(container, name.c_str(), ZIP_FL_NOCASE);

			if (id == -1) {
				//printf("\tERROR: File: %s not found.\n", name.c_str());
				return false;
			}

			struct zip_stat fileInfo;
			zip_stat_init(&fileInfo);

			if (zip_stat_index(container, id, 0, &fileInfo) == 0) {
				//std::cout << "File: " << fileInfo.name << ", Size: " << fileInfo.size << ", Compression Method: " << fileInfo.comp_method << ", CRC: " << fileInfo.crc << " \n";

				zip_flags_t flags = (fileInfo.comp_method == 21) ? ZIP_FL_COMPRESSED : ZIP_FL_UNCHANGED;
				zip_file_t* file = zip_fopen_index(container, id, flags);

				blob.resize(fileInfo.size);

				if (fileInfo.comp_method == 21)
				{
					std::unique_ptr<char[]> tempBlob(new char[fileInfo.comp_size]);
					zip_fread(file, tempBlob.get(), fileInfo.comp_size);

					XMEMDECOMPRESSION_CONTEXT DecompressionContext = 0;
					hr = XMemCreateDecompressionContext(XMEMCODEC_TYPE::XMEMCODEC_LZX, 0, 0, &DecompressionContext);

					try {
						hr = XMemDecompress(DecompressionContext, blob.data(), &fileInfo.size, tempBlob.get(), fileInfo.comp_size);
					}
					catch (const std::exception) {
						XMemResetDecompressionContext(DecompressionContext);
						XMemDestroyDecompressionContext(DecompressionContext);
					}

					XMemResetDecompressionContext(DecompressionContext);
					XMemDestroyDecompressionContext(DecompressionContext);

					if (FAILED(hr)) {
						printf("ERROR: xmem decompression failed (%08X)\n", static_cast<unsigned int>(hr));
					}

					zip_fclose(file);
				}
				else {
					zip_fread(file, blob.data(), fileInfo.size);
					zip_fclose(file);
				}
			}

			return true;
		}

		void release()
		{
			if (container != nullptr) {
				zip_close(container);
			}
		}


	private:
		zip* container = nullptr;
		std::filesystem::path m_file;
		std::string m_file_name;
	};

}
