#pragma once
#include "preinclude.hpp"

#include <roapi.h>
#include <Windows.h>
#include <wrl/client.h>

#include "DirectXTex.h"
#include "DirectXTexXbox.h"

#include "xg.h"
#include "DDS.h"

namespace fmnext
{
	class TextureResolver
	{
	public:
		TextureResolver() = default;
		~TextureResolver() = default;

		TextureResolver(const BundleReader::BundleData& bundle) : m_bundle(bundle)
		{
			detectPlatform();
		}

		void SaveToDDSFile(const std::string& file)
		{
			std::wstring widestr = std::wstring(file.begin(), file.end());
			const wchar_t* szFile = widestr.c_str();

			DirectX::DDS_FLAGS ddsFlags = DirectX::DDS_FLAGS_FORCE_DX10_EXT | DirectX::DDS_FLAGS_FORCE_DX10_EXT_MISC2;

			HRESULT hres = DirectX::SaveToDDSFile(m_image->GetImages(), m_image->GetImageCount(), m_info, ddsFlags, szFile);
			if (FAILED(hres))
			{
				std::cout << "Failed SaveToDDSFile.\n";
			}
		}

		DirectX::Blob& SaveToDDSMemory()
		{
			DirectX::DDS_FLAGS ddsFlags = DirectX::DDS_FLAGS_FORCE_DX10_EXT | DirectX::DDS_FLAGS_FORCE_DX10_EXT_MISC2;

			HRESULT hres = DirectX::SaveToDDSMemory(m_image->GetImages(), m_image->GetImageCount(), m_info, ddsFlags, m_blob);
			if (FAILED(hres))
			{
				std::cout << "Failed SaveToDDSMemory.\n";
			}

			return m_blob;
		}

		void SaveToPNGFile(const std::string& file)
		{
			std::wstring widestr = std::wstring(file.begin(), file.end());
			const wchar_t* szFile = widestr.c_str();

			// Initialize COM (needed for WIC)
			HRESULT hres = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
			if (FAILED(hres))
			{
				std::cout << "Failed to initialize COM\n";
			}

			//Texture compressed
			if (DirectX::IsCompressed(m_info.format)) // info.format == DXGI_FORMAT::DXGI_FORMAT_BC7_UNORM
			{
				std::unique_ptr<DirectX::ScratchImage> tmpimage = std::make_unique<DirectX::ScratchImage>();
				if (!tmpimage)
				{
					std::cout << "ERROR: Memory allocation failed\n";
				}

				hres = DirectX::Decompress(m_image->GetImages(), m_image->GetImageCount(), m_image->GetMetadata(), DXGI_FORMAT_UNKNOWN, *tmpimage);
				if (FAILED(hres))
				{
					std::cout << "Failed to Decompress.\n";
				}

				//DirectX::WIC_FLAGS_NONE
				//hres = DirectX::SaveToWICFile(tmpimage->GetImages(), tmpimage->GetImageCount(), DirectX::WIC_FLAGS_FORCE_SRGB, GetWICCodec(DirectX::WIC_CODEC_PNG), szFile);
				hres = DirectX::SaveToWICFile(*tmpimage->GetImage(0, 0, 0), DirectX::WIC_FLAGS_FORCE_SRGB, GetWICCodec(DirectX::WIC_CODEC_PNG), szFile);

				if (FAILED(hres))
				{
					std::cout << "Failed SaveToWICFile.\n";
				}

			}

			if (!DirectX::IsCompressed(m_info.format))
			{
				//DirectX::WIC_FLAGS_NONE
				//hres = DirectX::SaveToWICFile(m_image->GetImages(), m_image->GetImageCount(), DirectX::WIC_FLAGS_FORCE_SRGB, GetWICCodec(DirectX::WIC_CODEC_PNG), szFile);
				hres = DirectX::SaveToWICFile(*m_image->GetImage(0, 0, 0), DirectX::WIC_FLAGS_FORCE_SRGB, GetWICCodec(DirectX::WIC_CODEC_PNG), szFile);

				if (FAILED(hres))
				{
					std::cout << "Failed SaveToWICFile.\n";
				}
			}

		}


		void SaveToPNGFileNew(const std::string& file)
		{
			std::wstring widestr = std::wstring(file.begin(), file.end());
			const wchar_t* szFile = widestr.c_str();

			// Initialize COM (needed for WIC)
			HRESULT hres = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
			if (FAILED(hres))
			{
				std::cout << "Failed to initialize COM\n";
			}

			//Texture compressed
			if (DirectX::IsCompressed(m_info.format)) // info.format == DXGI_FORMAT::DXGI_FORMAT_BC7_UNORM
			{
				std::unique_ptr<DirectX::ScratchImage> tmpimage = std::make_unique<DirectX::ScratchImage>();
				if (!tmpimage)
				{
					std::cout << "ERROR: Memory allocation failed\n";
				}

				DXGI_FORMAT format = (m_info.format == DXGI_FORMAT::DXGI_FORMAT_UNKNOWN) ? m_info.format : DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;

				switch (m_info.format)
				{
				case DXGI_FORMAT_BC4_SNORM:
				case DXGI_FORMAT_BC5_SNORM:
					format = (DirectX::BitsPerColor(m_info.format) > 8) ? DXGI_FORMAT_R16G16B16A16_SNORM : DXGI_FORMAT_R8G8B8A8_SNORM;
					break;
				case DXGI_FORMAT_BC6H_SF16:
				case DXGI_FORMAT_BC6H_UF16:
					format = DXGI_FORMAT_R32G32B32_FLOAT;
					break;
				case DXGI_FORMAT_BC4_UNORM:
					format = DXGI_FORMAT_R8_UNORM;
					break;
				case DXGI_FORMAT_BC7_UNORM:
					format = DXGI_FORMAT_R8G8B8A8_UNORM;
					break;
				default:
					format = (DirectX::BitsPerColor(m_info.format) > 8) ? DXGI_FORMAT_R16G16B16A16_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM;
					break;
				}

				hres = DirectX::Decompress(m_image->GetImages(), m_image->GetImageCount(), m_image->GetMetadata(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, *tmpimage);

				if (FAILED(hres))
				{
					std::cout << "Failed to Decompress.\n";
				}

				switch (format)
				{
				case DXGI_FORMAT_R8G8B8A8_UNORM:
				{
					std::unique_ptr<DirectX::ScratchImage> destImage = std::make_unique<DirectX::ScratchImage>();

					hres = DirectX::Convert(*tmpimage->GetImage(0, 0, 0), DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, *destImage);

					if (FAILED(hres))
					{
						std::cout << "Cannot convert... \n";
					}

					//DirectX::WIC_FLAGS_NONE
					hres = DirectX::SaveToWICFile(destImage->GetImages(), destImage->GetImageCount(), DirectX::WIC_FLAGS_FORCE_RGB, GetWICCodec(DirectX::WIC_CODEC_PNG), szFile);

					if (FAILED(hres))
					{
						std::cout << "Failed SaveToWICFile.\n";
					}

					break;
				}

				case DXGI_FORMAT_BC6H_TYPELESS:
				case DXGI_FORMAT_BC6H_UF16:
				case DXGI_FORMAT_BC6H_SF16:
				case DXGI_FORMAT_BC7_TYPELESS:
				case DXGI_FORMAT_BC7_UNORM:
				case DXGI_FORMAT_BC7_UNORM_SRGB:
				{
					std::unique_ptr<DirectX::ScratchImage> destImage = std::make_unique<DirectX::ScratchImage>();

					hres = DirectX::Convert(*tmpimage->GetImage(0, 0, 0), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, *destImage);

					if (FAILED(hres))
					{
						std::cout << "Cannot convert... \n";
					}

					//DirectX::WIC_FLAGS_NONE
					hres = DirectX::SaveToWICFile(destImage->GetImages(), destImage->GetImageCount(), DirectX::WIC_FLAGS_FORCE_SRGB, GetWICCodec(DirectX::WIC_CODEC_PNG), szFile);

					if (FAILED(hres))
					{
						std::cout << "Failed SaveToWICFile.\n";
					}

					break;
				}
				case DXGI_FORMAT_R8_UNORM:
				{
					std::unique_ptr<DirectX::ScratchImage> destImage = std::make_unique<DirectX::ScratchImage>();

					hres = DirectX::Convert(*tmpimage->GetImage(0, 0, 0), DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, *destImage);

					if (FAILED(hres))
					{
						std::cout << "Cannot convert... \n";
					}

					//DirectX::WIC_FLAGS_NONE
					hres = DirectX::SaveToWICFile(destImage->GetImages(), destImage->GetImageCount(), DirectX::WIC_FLAGS_FORCE_RGB, GetWICCodec(DirectX::WIC_CODEC_PNG), szFile);

					if (FAILED(hres))
					{
						std::cout << "Failed SaveToWICFile.\n";
					}

					break;
				}

				default:
				{
					//DirectX::WIC_FLAGS_NONE
					hres = DirectX::SaveToWICFile(tmpimage->GetImages(), tmpimage->GetImageCount(), DirectX::WIC_FLAGS_FORCE_SRGB, GetWICCodec(DirectX::WIC_CODEC_PNG), szFile);

					if (FAILED(hres))
					{
						std::cout << "Failed SaveToWICFile.\n";
					}
					break;
				}
				}

			}

			if (!DirectX::IsCompressed(m_info.format))
			{
				std::unique_ptr<DirectX::ScratchImage> destImage = std::make_unique<DirectX::ScratchImage>();

				hres = DirectX::Convert(*m_image->GetImage(0, 0, 0), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, *destImage);

				if (FAILED(hres))
				{
					std::cout << "Cannot convert... \n";
				}

				//DirectX::WIC_FLAGS_NONE
				hres = DirectX::SaveToWICFile(destImage->GetImages(), destImage->GetImageCount(), DirectX::WIC_FLAGS_FORCE_SRGB, GetWICCodec(DirectX::WIC_CODEC_PNG), szFile);

				if (FAILED(hres))
				{
					std::cout << "Failed SaveToWICFile.\n";
				}
			}

		}

		DirectX::Blob& SaveToPNGMemory()
		{
			// Initialize COM (needed for WIC)
			HRESULT hres = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
			if (FAILED(hres))
			{
				std::cout << "Failed to initialize COM\n";
			}

			//Texture compressed
			if (DirectX::IsCompressed(m_info.format)) // info.format == DXGI_FORMAT::DXGI_FORMAT_BC7_UNORM
			{
				std::unique_ptr<DirectX::ScratchImage> tmpimage = std::make_unique<DirectX::ScratchImage>();
				if (!tmpimage)
				{
					std::cout << "ERROR: Memory allocation failed\n";
				}

				hres = DirectX::Decompress(m_image->GetImages(), m_image->GetImageCount(), m_image->GetMetadata(), DXGI_FORMAT_UNKNOWN, *tmpimage);
				if (FAILED(hres))
				{
					std::cout << "Failed to Decompress.\n";
				}

				//DirectX::WIC_FLAGS_NONE
				//hres = DirectX::SaveToWICMemory(tmpimage->GetImages(), tmpimage->GetImageCount(), DirectX::WIC_FLAGS_FORCE_SRGB, GetWICCodec(DirectX::WIC_CODEC_PNG), m_blob);
				hres = DirectX::SaveToWICMemory(*tmpimage->GetImage(0, 0, 0), DirectX::WIC_FLAGS_FORCE_SRGB, GetWICCodec(DirectX::WIC_CODEC_PNG), m_blob);

				if (FAILED(hres))
				{
					std::cout << "Failed SaveToWICMemory.\n";
				}

			}

			if (!DirectX::IsCompressed(m_info.format))
			{
				//DirectX::WIC_FLAGS_NONE
				//hres = DirectX::SaveToWICMemory(m_image->GetImages(), m_image->GetImageCount(), DirectX::WIC_FLAGS_FORCE_SRGB, GetWICCodec(DirectX::WIC_CODEC_PNG), m_blob);
				hres = DirectX::SaveToWICMemory(*m_image->GetImage(0, 0, 0), DirectX::WIC_FLAGS_FORCE_SRGB, GetWICCodec(DirectX::WIC_CODEC_PNG), m_blob);

				if (FAILED(hres))
				{
					std::cout << "Failed SaveToWICMemory.\n";
				}
			}

			return m_blob;
		}



	private:
		fmnext::BundleReader::BundleData m_bundle;
		std::unique_ptr<DirectX::ScratchImage> m_image = nullptr;
		DirectX::Blob m_blob{};
		DirectX::TexMetadata m_info{};

		std::vector<char> m_buffer;

		enum TexturePlatform
		{
			PC = 0,
			XBOX = 2
		};

		void detectPlatform()
		{
			for (auto& texture : m_bundle.Textures)
			{
				switch (texture.version.major)
				{
				case TexturePlatform::PC:
				{
					//std::cout << "DirectX Platform: PC \n";
					EncodePC_DDS(m_bundle);

					break;
				}
				case TexturePlatform::XBOX:
				{
					//std::cout << "DirectX Platform: Xbox One \n";
					EncodeXBOX_DDS(m_bundle);

					break;
				}
				default:
					break;
				}
			}
		}

		bool HasTextures()
		{
			if (!m_bundle.Textures.empty())
			{
				return true;
			}

			return false;
		}

		void EncodePC_DDS(const fmnext::BundleReader::BundleData& bundle)
		{
			m_buffer.resize(bundle.Textures[0].data.size() + 0x94);

			DirectX::TexMetadata metadata{};
			metadata.width = bundle.Textures[0].header.width;
			metadata.height = bundle.Textures[0].header.height; // 1 = 1D textures
			metadata.depth = bundle.Textures[0].header.depth; // 1 = 1D / 2D textures
			metadata.arraySize = bundle.Textures[0].header.is_texture_cube.value() == 1 ? 6 : 1; //bundle.Textures[0].header.num_slices.has_value() ? 0 : 1; // cubemap = multiple of 6
			metadata.mipLevels = bundle.Textures[0].header.mip_levels;
			metadata.miscFlags = bundle.Textures[0].header.is_texture_cube.has_value() ? (bundle.Textures[0].header.is_texture_cube.value() == 0 ? 0 : DirectX::DDS_RESOURCE_MISC_TEXTURECUBE) : 0;
			metadata.miscFlags2 = 0;
			metadata.format = bundle.Textures[0].header.encoding;
			metadata.dimension = DirectX::TEX_DIMENSION::TEX_DIMENSION_TEXTURE2D;

			size_t output_size = 0;
			HRESULT hres = EncodeDDSHeader(metadata, DirectX::DDS_FLAGS_FORCE_DX10_EXT_MISC2, reinterpret_cast<uint8_t*>(m_buffer.data()), 0x94, output_size);
			if (FAILED(hres))
			{
				std::cout << "Failed to EncodeDDSHeader. \n";
			}

			std::memcpy(m_buffer.data() + 0x94, bundle.Textures[0].data.data(), bundle.Textures[0].data.size());

			m_image = std::make_unique<DirectX::ScratchImage>();

			hres = DirectX::LoadFromDDSMemory(reinterpret_cast<uint8_t*>(m_buffer.data()), m_buffer.size(), DirectX::DDS_FLAGS_FORCE_DX10_EXT_MISC2, &m_info, *m_image);
			if (FAILED(hres))
			{
				std::cout << "Failed to LoadFromDDSMemory.\n";
			}
		}


		// Internal use only
		uint64_t calcBaseAlignment(const fmnext::BundleReader::BundleData& bundle)
		{
			if (!bundle.Textures[0].header.tile_mode.has_value())
			{
				return 32768;
			}

			XG_TILE_MODE mode = static_cast<XG_TILE_MODE>(bundle.Textures[0].header.tile_mode.value());

			switch (mode)
			{
			case XG_TILE_MODE::XG_TILE_MODE_1D_THIN:
			case XG_TILE_MODE::XG_TILE_MODE_2D_THIN:
			{
				XG_TEXTURE2D_DESC desc{};

				desc.Width = static_cast<UINT>(bundle.Textures[0].header.width);
				desc.Height = static_cast<UINT>(bundle.Textures[0].header.height);
				desc.MipLevels = static_cast<UINT>(bundle.Textures[0].header.mip_levels);
				desc.ArraySize = static_cast<UINT>(bundle.Textures[0].header.depth);
				desc.Format = static_cast<XG_FORMAT>(bundle.Textures[0].header.encoding);
				desc.SampleDesc.Count = 1;
				desc.Usage = XG_USAGE_DEFAULT;
				desc.BindFlags = XG_BIND_SHADER_RESOURCE;
				desc.MiscFlags = 0;
#if defined(_GAMING_XBOX_SCARLETT) || defined(_USE_SCARLETT)
				desc.SwizzleMode = bundle.Textures[0].header.tile_mode.value();
#else
				desc.TileMode = (XG_TILE_MODE)bundle.Textures[0].header.tile_mode.value();
#endif

				XG_RESOURCE_LAYOUT layout{};
				Microsoft::WRL::ComPtr<XGTextureAddressComputer> computer;

				HRESULT hres = XGCreateTexture2DComputer(&desc, computer.ReleaseAndGetAddressOf());
				if (FAILED(hres))
				{
					std::cout << "ERROR: Failed to create XGCreateTexture2DComputer." << "\n";
				}

				hres = computer->GetResourceLayout(&layout);
				if (FAILED(hres))
				{
					std::cout << "ERROR: Failed to get GetResourceLayout." << "\n";
				}

				return layout.BaseAlignmentBytes;

				break;
			}
			}

			return 32768;
		}

		void EncodeXBOX_DDS(const fmnext::BundleReader::BundleData& bundle)
		{
			size_t hdr_size = sizeof(uint32_t) + 124 + sizeof(Xbox::DDS_HEADER_XBOX);

			DirectX::TexMetadata metadata{};
			metadata.width = bundle.Textures[0].header.width;
			metadata.height = bundle.Textures[0].header.height; // 1 = 1D textures
			metadata.depth = bundle.Textures[0].header.depth; // 1 = 1D / 2D textures
			metadata.arraySize = bundle.Textures[0].header.is_texture_cube.value() == 1 ? 6 : 1; // cubemap = multiple of 6
			metadata.mipLevels = bundle.Textures[0].header.mip_levels;
			metadata.miscFlags = bundle.Textures[0].header.is_texture_cube.has_value() ? (bundle.Textures[0].header.is_texture_cube.value() == 0 ? 0 : DirectX::DDS_RESOURCE_MISC_TEXTURECUBE) : 0;
			metadata.miscFlags2 = 0;
			metadata.format = bundle.Textures[0].header.encoding;
			metadata.dimension = DirectX::TEX_DIMENSION::TEX_DIMENSION_TEXTURE2D;

			m_buffer.resize(hdr_size + bundle.Textures[0].data.size());

			size_t output_size = 0;
			HRESULT hres = EncodeDDSHeader(metadata, DirectX::DDS_FLAGS_NONE, reinterpret_cast<uint8_t*>(m_buffer.data()), hdr_size, output_size);
			if (FAILED(hres))
			{
				std::cout << "Failed to EncodeDDSHeader. \n";
			}

			memcpy(m_buffer.data() + hdr_size, bundle.Textures[0].data.data(), bundle.Textures[0].data.size());

			DirectX::DDS_HEADER* encoded_header = (DirectX::DDS_HEADER*)(m_buffer.data() + sizeof(uint32_t));
			encoded_header->ddspf.flags = DDS_FOURCC;
			encoded_header->ddspf.fourCC = MAKEFOURCC('X', 'B', 'O', 'X'); //0x584F4258

			Xbox::DDS_HEADER_XBOX* encoded_xbox_header = (Xbox::DDS_HEADER_XBOX*)(m_buffer.data() + sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER));
			encoded_xbox_header->dxgiFormat = metadata.format;
			encoded_xbox_header->resourceDimension = metadata.dimension;
			encoded_xbox_header->miscFlag = metadata.miscFlags; // & ~4
			encoded_xbox_header->arraySize = metadata.IsCubemap() ? 1 : static_cast<uint32_t>(metadata.arraySize);
			encoded_xbox_header->miscFlags2 = metadata.miscFlags2;
			encoded_xbox_header->tileMode = bundle.Textures[0].header.tile_mode.value();
			encoded_xbox_header->baseAlignment = static_cast<uint32_t>(calcBaseAlignment(bundle));
			encoded_xbox_header->dataSize = static_cast<uint32_t>(bundle.Textures[0].data.size());
			encoded_xbox_header->xdkVer = _XDK_VER;

			Xbox::XboxImage xbox{};

			hres = Xbox::LoadFromDDSMemory(reinterpret_cast<uint8_t*>(m_buffer.data()), m_buffer.size(), &m_info, xbox);
			if (FAILED(hres))
			{
				std::cout << "Xbox::LoadFromDDSMemory Failed. \n";
			}

			m_image = std::make_unique<DirectX::ScratchImage>();

			hres = Xbox::Detile(xbox, *m_image);
			if (FAILED(hres))
			{
				std::cout << "Xbox::Detile Failed. \n";
			}

			/*
			DirectX::TexMetadata info{};
			hres = Xbox::SaveToDDSFile(xbox_image, szFile);

			auto img = image->GetImage(0, 0, 0);
			assert(img);
			const size_t nimg = image->GetImageCount();

			DDS_FLAGS ddsFlags = DDS_FLAGS_FORCE_DX10_EXT | DDS_FLAGS_FORCE_DX10_EXT_MISC2; // can be DDS_FLAGS_NONE
			//ddsFlags |= DDS_FLAGS_FORCE_DX9_LEGACY; // fail 
			hres = SaveToDDSFile(img, nimg, info, ddsFlags, szFile);
			if (FAILED(hres))
			{
				std::cout << "Failed to write dds file! \n";
			}
			*/
		}
	};




}