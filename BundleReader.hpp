#pragma once
#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <vector>
#include <array>
#include <filesystem>
#include <any>
#include <unordered_map>
#include <optional>
#include <guiddef.h>
#include <bitset>
#include <DirectXMath.h>
#include <dxgiformat.h>

#include "mbuff.hpp"
#include <d3d11.h>

namespace fmnext
{
	const uint32_t TAG_BUNDLE = 0x47727562; // 'Grub' - BundleTag

	// Textures (swatchbin)
	const uint32_t TAG_BLOB_TextureContentBlob = 0x54584342; // 'TXCB'

	// Models (modelbin)
	const uint32_t TAG_BLOB_Skeleton = 0x536B656C; // 'Skel'
	const uint32_t TAG_BLOB_Morph = 0x4D727068; // 'Mrph'
	const uint32_t TAG_BLOB_MaterialInstance = 0x4D617449; // 'MatI'
	const uint32_t TAG_BLOB_Mesh = 0x4D657368; // 'Mesh'
	const uint32_t TAG_BLOB_IndexBuffer = 0x496E6442; // 'IndB'
	const uint32_t TAG_BLOB_VertexLayout = 0x564C6179; // 'VLay'
	const uint32_t TAG_BLOB_InstancedVertexLayout = 0x494C6179; // 'ILay'
	const uint32_t TAG_BLOB_VertexBuffer = 0x56657242; // 'VerB'
	const uint32_t TAG_BLOB_MorphBuffer = 0x4D427566; // 'MBuf'
	const uint32_t TAG_BLOB_Skin = 0x536B696E; // 'Skin'
	const uint32_t TAG_BLOB_Model = 0x4D6F646C; // 'Modl'

	// Materials (materialbin)
	const uint32_t TAG_BLOB_MATI = 0x4D415449; // 'MATI'
	const uint32_t TAG_BLOB_MATL = 0x4D41544C; // 'MATL'
	const uint32_t TAG_BLOB_MaterialShaderParameter = 0x4D545052; // 'MTPR'

	// Shaders (shaderbin)
	const uint32_t TAG_BLOB_DefaultShaderParameter = 0x44465052; // 'DFPR'
	const uint32_t TAG_BLOB_LightScenario = 0x4C534345; // 'LSCE'
	const uint32_t TAG_BLOB_DebugLightScenario = 0x44424C53; // 'DBLS'
	const uint32_t TAG_BLOB_CBMP = 0x43424D50; // 'CBMP'
	const uint32_t TAG_BLOB_TXMP = 0x54584D50; // 'TXMP'
	const uint32_t TAG_BLOB_SPMP = 0x53504D50; // 'SPMP'
	const uint32_t TAG_BLOB_VARS = 0x56415253; // 'VARS' - Variables, Variants?
	const uint32_t TAG_BLOB_VERS = 0x56455253; // 'VERS'
	const uint32_t TAG_BLOB_TRGT = 0x54524754; // 'TRGT'

	// ManufacturerColors (bin)
	const uint32_t TAG_BLOB_ManufacturerColors = 0x4D4E434C; // 'MNCL'

	// Models (modelbin)
	const uint32_t TAG_METADATA_Name = 0x4E616D65; // 'Name'
	const uint32_t TAG_METADATA_TextureContentHeader = 0x54584348; // 'TXCH'
	const uint32_t TAG_METADATA_Identifier = 0x49642020; // 'Id  '
	const uint32_t TAG_METADATA_BBox = 0x42426F78; // 'BBox'
	const uint32_t TAG_METADATA_TextureReferences = 0x54526566; // 'TRef'
	const uint32_t TAG_METADATA_ACMR = 0x41434D52; // 'ACMR'

	// Materials (materialbin)
	const uint32_t TAG_METADATA_ATST = 0x41545354; // 'ATST' - Atlas?
	const uint32_t TAG_METADATA_BLEN = 0x424C454E; // 'BLEN'
	const uint32_t TAG_METADATA_VDCL = 0x5644434C; // 'VDCL' - VD Color?

	enum ShaderParameterType
	{
		ShaderParameter_Vector = 0, // Float4; Vector4ShaderParameter, Vector3ShaderParameter
		ShaderParameter_Color = 1, // Float4
		ShaderParameter_Float = 2, // Float; ScaleShaderParameter
		ShaderParameter_Bool = 3, // Boolean
		ShaderParameter_Int = 4, // Int; CarShaderParameter, CarDirtShaderParameter, TrackShaderParameter, CarWheelBlurShaderParameter
		ShaderParameter_Swizzle = 5, // Float4
		ShaderParameter_Texture2D = 6, // Texture
		ShaderParameter_Sampler = 7, // Sampler
		ShaderParameter_ColorGradient = 8, // Texture
		ShaderParameter_FunctionRange = 9, // Float4
		ShaderParameter_Vector2 = 11 // Float2
		//ShaderParameter_RenderTarget = ? // RenderTargetTexture
	};

	constexpr const char* NameHashToString(uint32_t hash) { // CRC-32
		switch (hash) {
			// float4 - ParameterConstantBuffer
		case 0xEA718FBE: // carpaint_standard
			return "UniqueBaseColorColorParam";

		case 0x0014A502:
			return "PaintColorGroupColorParam";

		case 0xC0CB2820:
			return "PaintColorColorParam";

		case 0xA415641F:
			return "PaintFlakeF0Vector4";

		case 0x3FA9F5C9:
			return "PaintFinishColorParam";

		case 0x1BB17DD2:
			return "radColour1ColorParam_pg_radiosity";

		case 0x43AFD4FA:
			return "radColour2ColorParam_pg_radiosity";

		case 0xC28AB1DD:
			return "radColour3ColorParam_pg_radiosity";

		case 0x003E4460: // carpaint_standard_glossmask
			return "PaintFinishBColorParam";

		case 0x63040D89: // badge_clc_diffusemetallerpch1masknorm
			return "DiffuseColorColorParam";

		case 0x1F9B6488:
			return "F0aVector4";

		case 0x9114636B:
			return "F0bVector4";

		case 0x23C8B47A:
			return "ClearCoatF0Vector4";

		case 0xF51639BE: // badge_ch1difnormglossao
			return "DiffuseColorGroupColorParam";

		case 0x0E29312A:
			return "ReflectionTintColorParam";

		case 0x57C321A6: // simplediffuse
			return "ColorColorParam";

		case 0x938926B0:
			return "F0Vector4";

		case 0x73A9E2DF: // clc_metalch2normalch1mask
			return "ColorGroupColorParam";

		case 0x0B379E68:
			return "CarbonClearCoatColorParam";

		case 0xC308C931:
			return "IlluminationColoColorParam";

		case 0x1F3EB7A9: // ch1diffnormglossalphaemissive
			return "DiffTintColorParam";

		case 0x9BCED46E: // m_ch2normglossalphaemissive
			return "IlluminationColorColorParam";

		case 0xEF5CCE09: // ch2diffnormglossalphaemissive
			return "DiffuseColorAColorParam";

		case 0x76BEA808:
			return "DiffuseColorBColorParam";

		case 0xC017A27E: // ch1ch2normlerpch1glossdiff
			return "ReflectionTintColorColorParam";

		case 0x8467AAA4: // car_window_interior
			return "g_CarUserColor0";

		case 0x1F30F777:
			return "GlassColor0ColorParam";

		case 0x108D7FE5:
			return "CrackF0Vector4";

			// float2
		case 0x1CE52912: // carpaint_standard
			return "OrangePeelTiling";

		case 0x8ED8D865:
			return "FlakeNormalTiling";

		case 0x46E92CB5: // badge_clc_diffusemetallerpch1masknorm
			return "CH1MaskTiling";

		case 0x730F2086:
			return "NormalTiling";

		case 0xC19C70CD: // badge_ch1difnormglossao
			return "DiffuseATiling";

		case 0x3EC91ED5:
			return "GlossTiling";

		case 0xCB4FD76E:
			return "RTintTiling";

		case 0x75D6E294:
			return "LocalAOTiling";

		case 0x519B26A1: // clc_metalch2normalch1mask
			return "CH2DiffuseTextureTiling";

		case 0x53372732:
			return "CH2RTintTiling";

		case 0xB879D9F0:
			return "LightMapTiling";

		case 0x7495F9AC: // ch1diffnormglossalphaemissive
			return "AlphaTiling";

		case 0x8591411B: // ch1ch2normlerpch1glossdiff
			return "CH1GlossDiffMaskTiling";

		case 0x114A45A9:
			return "CH2NormalTiling";

		case 0xAC802967:
			return "CH1NormalTiling";

		case 0x0222B0B9:
			return "CH1NormalMaskTiling";

		case 0x2F354D91:
			return "AOTiling";

		case 0x548284FC: // car_cf_embossed_interior
			return "CH1_CLCNormalMap0Tiling";

			// float
		case 0x74226C85: // carpaint_standard
			return "BaseCoatScalar_floatVal";

		case 0xA2896771:
			return "stepMinRimSideAngle_floatVal_surfaceFX_snow_surfaceFX";

		case 0x193593DA:
			return "stepMaxRimSideAngle_floatVal_surfaceFX_snow_surfaceFX";

		case 0x8CF21130:
			return "lerpMinRimSideAngle_floatVal_surfaceFX_snow_surfaceFX";

		case 0x374EE59B:
			return "lerpMaxRimSideAngle_floatVal_surfaceFX_snow_surfaceFX";

		case 0x8EDE2B71:
			return "stepMinRimMask_floatVal_surfaceFX_snow_surfaceFX";

		case 0xB893E9E2:
			return "stepMaxRimMask_floatVal_surfaceFX_snow_surfaceFX";

		case 0x86EF8FB1:
			return "FlakeAmount_floatVal";

		case 0x3F770DFC:
			return "CH1NormalHeightScale";

		case 0xA49CC530:
			return "OrangePeelHeightScale";

		case 0xC8C003A0:
			return "RainUVScale_floatVal_basewaterbeading_uvrot0";

		case 0x33E9FD2B:
			return "DirectionalFalloff_Offset_floatVal_basewaterbeading_uvrot0";

		case 0xB7BB947A:
			return "DirectionalFalloff_Tightness_floatVal_basewaterbeading_uvrot0";

		case 0x99CC69B1:
			return "FlakeGloss_floatVal";

		case 0x2A57C2B6:
			return "FlakeNormalIntensity_floatVal";

		case 0x18F91688:
			return "PaintType_floatVal";

		case 0x8F88A8DD:
			return "radEV1_floatVal_pg_radiosity";

		case 0xF96D91E0:
			return "radEV2_floatVal_pg_radiosity";

		case 0x621E7B34:
			return "radEV3_floatVal_pg_radiosity";

		case 0xD6B74780: // carpaint_standard_glossmask
			return "FlakeAmountB_floatVal";

		case 0xA75C82D2:
			return "RainUVScale_floatVal_basewaterbeading";

		case 0x2C6CEF66:
			return "DirectionalFalloff_Offset_floatVal_basewaterbeading";

		case 0xA9383667:
			return "DirectionalFalloff_Tightness_floatVal_basewaterbeading";

		case 0xD6A86466:
			return "FlakeGlossB_floatVal";

		case 0xBB022974: // badge_clc_diffusemetallerpch1masknorm
			return "NormalHeightScale";

		case 0x52E99DA3:
			return "GlossA_floatVal";

		case 0xB9DE26A0:
			return "GlossB_floatVal";

		case 0x7E88DE7D:
			return "ClearCoatGloss_floatVal";

		case 0x00CE7933: // badge_ch1difnormglossao
			return "NormalSpecPower";

		case 0x7A549CB2:
			return "DiffuseDarkeningMaxPorosity_floatVal_basewaterbeading_nonclearcoat";

		case 0x5FF94E67: // simplediffuse
			return "Gloss_floatVal";

		case 0x18385024: // clc_metalch2normalch1mask
			return "RainUVScale_floatVal_basewaterbeading_uvrot";

		case 0xE58ABE7B:
			return "DirectionalFalloff_Offset_floatVal_basewaterbeading_uvrot";

		case 0x09A23168:
			return "DirectionalFalloff_Tightness_floatVal_basewaterbeading_uvrot";

		case 0x2DB53178:
			return "FlakeGlossDmg_floatVal";

		case 0x6F45C5F1:
			return "EmissiveType_floatVal";

		case 0x1364DE85:
			return "pgModEV_floatVal";

		case 0xF9F19BA6:
			return "PartDamageThreshold_floatVal";

		case 0x495BB2FB: // ext_grille
			return "UVRotation_floatVal";

		case 0xB0B8947E: // m_ch2normglossalphaemissive
			return "uTile_floatVal";
		case 0xCCD9B1A5:
			return "vTile_floatVal";

		case 0xD5F1D09E: // ch1ch2normlerpch1glossdiff
			return "CH2NormalHeightScale";

		case 0xF94165FE: // car_cf_embossed_interior
			return "CH1_CLCNormalMap0HeightScale";

		case 0x8F731786: // car_cf_embossed_interior
			return "RainUVScale_floatVal_basewaterbeading_interior";

		case 0x6F3A994F:
			return "AllowUseColorBool";

			// int
		case 0x5393C778: // carpaint_standard
			return "radFunction1CarLightParam_pg_radiosity";

		case 0xE007EABB:
			return "radFunction2CarLightParam_pg_radiosity";

		case 0x8E8BF1FA:
			return "radFunction3CarLightParam_pg_radiosity";

		case 0x493073A0: // clc_metalch2normalch1mask
			return "LightFunctionCarLightParam";

			// bool
		case 0x9615CAAA: // carpaint_standard
			return "UniqueBaseTextureSwitchBool";

		case 0x78004A9C:
			return "ColorGroupSwitchBool";

		case 0xD047A271:
			return "UseDiffuseAlphaBool";

		case 0x7169AC81:
			return "CH1DiffuseTextureSwitchBool";

		case 0xE1D827FD:
			return "UniqueBaseColorSwitchBool";

		case 0xFF73057F:
			return "LiverySwitchBool";

		case 0x9A8DF740:
			return "DamageSwitchBool";

		case 0x61730CD3:
			return "ScrapeSwitchBool";

		case 0xF03E432F:
			return "FlakeMaskSwitchBool";

		case 0x76C4A90E:
			return "CH1NormalSwitchBool";

		case 0xE159D67B:
			return "CH1AlphaSwitchBool";

		case 0xE4AAD859: // carpaint_standard_glossmask
			return "FlakeGlossMaskBool";

		case 0x9554D18D:
			return "GlossMaskSwitchBool";

		case 0x05A401E7: // badge_clc_diffusemetallerpch1masknorm
			return "DiffuseTextureSwitchBool";

		case 0x08B2C17F:
			return "CH1MaskSwitchBool";

		case 0xCBB3D988:
			return "CH1OpacitySwitchBool";

		case 0x04F8F9FA: // badge_ch1difnormglossao
			return "CH1DiffColTextureSwitchBool";

		case 0x0F70E9CC:
			return "RTintMaskSwitchBool";

		case 0x71263EB7:
			return "CH1F0MaskSwitchBool";

		case 0x5A0DA36A:
			return "CH1GlossMaskSwitchBool";

		case 0xE876DDCC:
			return "CH1LocalAOSwitchBool";

		case 0xFE5BAD37:
			return "CH1LocalAOSwitch0Bool";

		case 0xA6BF15E8:
			return "CH1OpacityMaskSwitchBool";

		case 0x4E3A085E: // clc_metalch2normalch1mask
			return "RTintTextureSwitchBool";

		case 0x6C03F944:
			return "LocalAOSwitchBool";

		case 0x255EF28A:
			return "CH2NormalSwitchBool";

		case 0x07ACB91F:
			return "RGBMapSwitchBool";

		case 0x0BF3318B:
			return "EmissiveOnOffBool";

		case 0xE5BC49CB:
			return "AlphaSwitchBool";

		case 0x989B026F: // ext_grille
			return "MetalSwitchBool";
		case 0x7487EB77:
			return "FillSwitchBool";

		case 0x553D641D: // ch1diffnormglossalphaemissive
			return "CH1NormalMapSwitchBool";

		case 0x6F80D6E1: // m_ch2normglossalphaemissive
			return "CH2RTintMaskSwitchBool";

		case 0x22BC6533:
			return "CH2F0MaskSwitchBool";

		case 0xFA9429D7:
			return "CH2NormalMapSwitchBool";

		case 0xF5A4EEA0:
			return "CH2GlossMaskSwitchBool";

		case 0x02F000AE:
			return "CH2LocalAOSwitchBool";

		case 0x2B935F14:
			return "CH2DirectAOSwitchBool";

		case 0x9FC7B8A8:
			return "CH2OpacityMaskSwitchBool";

		case 0x6FA53B68: // ch2diffnormglossalphaemissive
			return "CH2DiffuseLERPSwitchBool";

		case 0x56DD9628: // ch1ch2normlerpch1glossdiff
			return "CH1DiffuseLERPSwitchBool";

		case 0x88C5D9BD:
			return "CH1DirectAOSwitchBool";

		case 0x48287560: // car_cf_embossed_interior
			return "ClearCoatNormalSwitch0Bool";

		case 0x22819153:
			return "DialRadiositySwitchBool_dialRadiosity";

			// SRV Texture
		case 0x66E53F62: // carpaint_standard
			return "CH1AlphaTextureTexture";

		case 0x10350BBC:
			return "CH1DiffuseTextureTexture";

		case 0x5BB7DA76:
			return "CH1NormalTexture";

		case 0xB59BE3AB:
			return "FlakeNormalTexture";

		case 0x8F186353:
			return "HighNoiseTextureTexture_surfaceFX_snow_surfaceFX";

		case 0x26169D54:
			return "LowNoiseTextureTexture_surfaceFX_snow_surfaceFX";

		case 0x8B653400:
			return "MudTileDirectionalLeftTexture_surfaceFX_opus_surfaceFX";

		case 0x154A5458:
			return "MudTileNonDirectionalTexture_surfaceFX_opus_surfaceFX";

		case 0x5F37B059:
			return "NormalMapWithIntensityTexture";

		case 0xD7D2D75D:
			return "NormalMapWithIntensityTexture_surfaceFX_opus_surfaceFX";

		case 0x8382A669:
			return "NormalMapWithIntensityTexture_surfaceFX_snow_surfaceFX";

		case 0x7B683AC5:
			return "OrangePeelTexture";

		case 0x99EE46B6:
			return "radTextureTexture_pg_radiosity";

		case 0x9D396DAD:
			return "SnowDiffuseTextureTexture_surfaceFX_snow_surfaceFX";

		case 0xCA80B33B:
			return "TexLowFrequencyNoiseTexture_surfaceFX_opus_surfaceFX";

		case 0x16F462B7:
			return "TextureTexture";

		case 0x05EECB86:
			return "TextureWetnessEdgeTexture_surfaceFX_opus_surfaceFX";

		case 0x51BEBAB2:
			return "TextureWetnessEdgeTexture_surfaceFX_snow_surfaceFX";

		case 0x3380008B: // badge_clc_diffusemetallerpch1masknorm
			return "CH1MaskTexture";

		case 0x6DD98CD9:
			return "DiffuseATexture";

		case 0x8C658791:
			return "NormalTexture";

		case 0x7E4A41E1: // badge_ch1difnormglossao
			return "GlossTexture";

		case 0x7FDA2F1B:
			return "LocalAOTexture";

		case 0x220CAD2C:
			return "RTintTexture";

		case 0x57D9D49E: // clc_metalch2normalch1mask
			return "AlphaTexture";

		case 0x294DA6FC:
			return "CH2DiffuseTextureTexture";

		case 0x4049C803:
			return "CH2RTintTexture";

		case 0x35C82561:
			return "LightMapTexture";

		case 0x64C94C50:
			return "SplatterYTexture";

		case 0x043E9751: // ch2diffnormglossalphaemissive
			return "GlossVariationMapTexture";

		case 0x0FEA383B: // ch1ch2normlerpch1glossdiff
			return "AOTexture";

		case 0x022DF609:
			return "CH1GlossDiffMaskTexture";

		case 0x3A72873C:
			return "CH1NormalMaskTexture";

		case 0x27D6FFAD:
			return "CH2NormalTexture";

		case 0x3C929217: // car_cf_embossed_interior
			return "CH1_CLCNormalMap0Texture";

		case 0xA27F63E2: // car_window_interior
			return "ShatterMapTexture";

			// Sampler
		case 0xF01369AD: // carpaint_standard
			return "SamplerStatesLiverySampler";

		case 0x41359340:
			return "SamplerStatesSampler_pg_radiosity";

		case 0xB74664B2:
			return "SamplerStatesSampler_surfaceFX";

		case 0xEEACBDFB: // badge_clc_diffusemetallerpch1masknorm
			return "pgSamplerStatesSampler";

		case 0xF1163FB5: // badge_ch1difnormglossao
			return "SamplerStatesSampler_basewaterbeading_nonclearcoat";

		case 0xA8B882A2: // ch2diffnormglossalphaemissive
			return "SamplerStatesSampler";

		case 0x80846FBD: // car_cf_embossed_interior
			return "SamplerStatesSampler_dialRadiosity";
		}

		return "null";
	}

	class BundleReader
	{
	public:
		BundleReader() = default;
		~BundleReader() = default;

		BundleReader(const std::string& filename)
		{
			p_stream = std::make_unique<std::ifstream>(filename, std::ios::binary | std::ios::in);
		}

		BundleReader(void* data, size_t size)
		{
			p_buffer = std::make_unique<mbuff>(reinterpret_cast<char*>(data), reinterpret_cast<char*>(data) + size);
			p_stream = std::make_unique<std::istream>(p_buffer.get(), false);
		}

		BundleReader(std::vector<char>& data)
		{
			p_buffer = std::make_unique<mbuff>(data);
			p_stream = std::make_unique<std::istream>(p_buffer.get(), false);
		}

		struct LODFlags {
			LODFlags() = default;

			LODFlags(uint16_t value) :
				LODS(((value >> 0) & 0x1)),
				LOD0(((value >> 1) & 0x1)),
				LOD1(((value >> 2) & 0x1)),
				LOD2(((value >> 3) & 0x1)),
				LOD3(((value >> 4) & 0x1)),
				LOD4(((value >> 5) & 0x1)),
				LOD5(((value >> 6) & 0x1)),
				flags(value) {
			}

			uint16_t flags;
			bool LODS;
			bool LOD0;
			bool LOD1;
			bool LOD2;
			bool LOD3;
			bool LOD4;
			bool LOD5;
		};

		struct Version
		{
			uint8_t major;
			uint8_t minor;
		};

		struct BundleHeader {
			int32_t tag;
			Version version;
			uint16_t blob_count;
			uint32_t header_size;
			uint32_t total_size;
		};

		struct BundleBlobHeader {
			int32_t tag;
			Version version;
			uint16_t metadata_count;
			uint32_t metadata_offset;
			uint32_t data_offset;
			uint32_t data_size;

			BundleBlobHeader() = default;
			BundleBlobHeader(int32_t tag, Version version, uint16_t metadata_count, uint32_t metadata_offset, uint32_t data_offset, uint32_t data_size) : tag(tag), version(version), metadata_count(metadata_count), metadata_offset(metadata_offset), data_offset(data_offset), data_size(data_size)
			{
			}

			BundleBlobHeader(const BundleBlobHeader& other) = delete;
			BundleBlobHeader(BundleBlobHeader&& other) = default;
		};

		struct BundleBlobMetadata {
			uint32_t tag;
			uint16_t version : 4;
			uint16_t data_size : 12;
			uint16_t data_offset;
		};

		struct VertexBufferIndices {
			int32_t id; // match buffer metadata id; bufferType = 2
			uint32_t input_slot;
			uint32_t stride;
			uint32_t offset;
		};

		struct UVTransform
		{
			float v_translation;
			float v_scale;
			float u_translation;
			float u_scale;
		};

		struct RenderPass
		{
			bool opaque_and_transparent; // Opaque_And_Transparent
			uint16_t unk;
			bool decal_transparent; // DecalTransparent
			bool not_opaque; // !Opaque; = 1, if version < 1.7
			bool not_transparent; // !Transparent; = 1, if version < 1.7
			bool decal_opaque; // DecalOpaque
			//uint16_t __padding__;

			RenderPass() = default;
			RenderPass(uint16_t value) : opaque_and_transparent(std::bitset<8>(value)[0]),
				unk(std::bitset<8>(value)[1]),
				decal_transparent(std::bitset<8>(value)[2]),
				not_opaque(std::bitset<8>(value)[3]),
				not_transparent(std::bitset<8>(value)[4]),
				decal_opaque(std::bitset<8>(value)[5])
			{
			}

		}; // unsigned short

		struct VLFlags
		{
			bool TEXCOORD0 : 1;
			bool TEXCOORD1 : 1;
			bool TEXCOORD2 : 1;
			bool TEXCOORD3 : 1;
			bool TEXCOORD4 : 1;
			bool TANGENT0 : 1;
			bool TANGENT1 : 1;
			bool TANGENT2 : 1;
			bool TANGENT3 : 1;
			bool TANGENT4 : 1;
			bool COLOR0 : 1;
			//uint32_t __padding__ : 21; // unsigned long

			// TEXCOORD/TEXCOORD_XONLY: 0x1, 0x2, 0x4, 0x8, 0x10
			// TANGENT: 0x20, 0x40, 0x80, 0x100, 0x200
			// COLOR: 0x400
		};

		struct BlobMesh {
			Version version;
			std::vector<int16_t> material_ids;
			std::optional<int16_t> material_id;
			int16_t bone_index;
			int16_t levels_of_detail;
			int8_t lowest_level_of_detail;
			int8_t highest_level_of_detail;
			uint16_t render_pass;
			uint8_t skinning_elements_count;
			uint8_t morph_weights_count;
			int32_t index_buffer_id;
			int32_t start_index;
			int32_t base_vertex;
			uint32_t indexes_count;
			uint32_t faces_count;
			float average_cache_miss_ratio;
			uint32_t vertices_count;
			uint32_t vertex_layout_id;
			std::vector<VertexBufferIndices> vertex_buffer_ids;
			int32_t morph_data_buffer_id;
			int32_t skinning_data_buffer_id;
			std::vector<int32_t> constant_buffer_ids;
			std::array<UVTransform, 5> texcoord_transform;
			std::optional<DirectX::XMFLOAT4> position_scale; // DirectX::XMVECTOR; objectCBuffer.g_PosScale
			std::optional<DirectX::XMFLOAT4> position_translate; // DirectX::XMVECTOR; objectCBuffer.g_PosOffset
		};

		struct BlobDescBuffer {
			int32_t length;
			int32_t size;
			uint16_t stride;
			uint8_t num_elements;
			DXGI_FORMAT format;
			std::vector<char> Buffer;
		};

		enum ColorProfile {
			NoProfile = 0,
			sRGB = 1
		}; // enum ColorProfile::Enum

		struct parameter {
			uint32_t value_offset; // relative to TXCH data begin
			int32_t next_offset;
			uint32_t value;

			parameter() = default;
			parameter(uint32_t value_offset, int32_t next_offset, uint32_t value) : value_offset(value_offset), next_offset(next_offset), value(value) {}
		};

		struct subresource {
			uint32_t value_offset; // relative to TXCB data begin
			int32_t next_offset;

			subresource() = default;
			subresource(uint32_t value_offset, int32_t next_offset) : value_offset(value_offset), next_offset(next_offset) {}

		}; // count == mips

		struct TextureContent
		{
			GUID guid;
			uint16_t width;
			uint16_t height;
			uint16_t depth;
			uint8_t mip_levels;
			std::optional<uint8_t> gap1E;
			std::optional<uint32_t> tile_mode;
			DXGI_FORMAT encoding;
			DXGI_FORMAT transcoding;
			ColorProfile color_profile;
			std::optional<int16_t> num_slices;
			std::optional<uint32_t> is_texture_cube;
			std::optional<uint32_t> is_texture_3d;
			std::optional<uint32_t> pitch_or_linear_size_bit;
		};

		struct Bones
		{
			std::string name;
			int16_t parent_index;
			int16_t child_index;
			int16_t next_index;
			DirectX::XMMATRIX transform;
		};

		struct BlobSkeleton {
			std::vector<Bones> Bones;
		};

		struct BlobVertexLayoutElem
		{
			uint16_t semantic_name_index;
			uint16_t semantic_index;
			uint16_t input_slot;
			uint16_t input_slot_class;
			DXGI_FORMAT format;
			int32_t aligned_byte_offset;
			uint32_t instance_data_step_rate;
		};

		struct BlobVertexLayout
		{
			std::vector<std::string> semantic_names;
			std::vector<D3D11_INPUT_ELEMENT_DESC> elements;
			std::vector<int32_t> element_formats;
			std::optional<VLFlags> flags;
		};

		struct BlobModel
		{
			int16_t meshes_count = 0;
			int16_t buffers_count = 0;
			int16_t layouts_count = 0;
			int16_t materials_count = 0;
			bool has_levels_of_detail = false;
			int8_t lowest_level_of_detail;
			int8_t highest_level_of_detail;
			std::optional<LODFlags> levels_of_detail;
			uint8_t decompress_flags = 0;
		};

		struct AABB
		{
			DirectX::XMFLOAT3 min;
			DirectX::XMFLOAT3 max;
		};

		struct BlobMorphTable {
			std::vector<std::string> Data;
		};

		struct BlobTexture
		{
			Version version;
			TextureContent header;
			std::vector<char> data;
		};

		struct BlobManufacturerColor
		{
			Version version;
			std::optional<uint32_t> material_index_mask;
			std::optional<std::vector<std::string>> masks;
			DirectX::XMFLOAT3 preview_color;
			std::string path;
		};

		struct ShaderParameter
		{
			Version parameter_version;
			uint32_t hash;
			ShaderParameterType type;
			GUID guid;
			std::any value;
		};

		struct BlobShaderParameter
		{
			Version version;
			std::vector<ShaderParameter> shader_parameters;
		};

		struct Model
		{
			Version version;
			std::unordered_map<std::string, std::any> metadata;
			BlobModel model;
		};

		struct Mesh
		{
			Mesh() = default;
			Mesh(std::unordered_map<std::string, std::any>&& metadata, BlobMesh&& mesh) : metadata(std::move(metadata)), mesh(std::move(mesh)) {}
			//Mesh(const Mesh& other) : metadata(other.metadata), mesh(other.mesh) {}
			//Mesh(Mesh&& other) noexcept : metadata(other.metadata), mesh(other.mesh) {}

			//Mesh& operator=(const Mesh& other) = default;
			//Mesh& operator=(Mesh&& other) noexcept = default;

			std::unordered_map<std::string, std::any> metadata;
			BlobMesh mesh;
		};

		struct Material
		{
			Version version;
			std::unordered_map<std::string, std::any> metadata;
			std::vector<char> data;
		};

		/*
		std::vector<Model> m_Model;
		std::vector<Bones> m_Skeleton;
		std::vector<Mesh> m_Meshes;
		std::vector<std::string> m_MorphTables;
		std::unordered_map<int32_t, BlobDescBuffer> m_IndexBuffers;
		std::unordered_map<int32_t, BlobDescBuffer> m_SkinBuffers;
		std::unordered_map<int32_t, BlobDescBuffer> m_VertexBuffers;
		std::unordered_map<int32_t, BlobDescBuffer> m_MorphBuffers;
		std::unordered_map<int32_t, BlobVertexLayout> m_VertexLayouts;
		std::vector<std::vector<BlobManufacturerColor>> m_ManufacturerColors;
		std::vector<std::string> m_MaterialInstances;
		std::vector<Material> m_MaterialInstanceBundles;
		std::vector<ShaderParameter> m_ShaderParameters;
		std::vector<BlobTexture> m_Textures;
		*/

		struct BundleData
		{
			std::vector<Model> Model;
			std::vector<Bones> Skeleton;
			std::vector<Mesh> Meshes;
			std::vector<std::string> MorphTables;
			std::unordered_map<int32_t, BlobDescBuffer> IndexBuffers;
			std::unordered_map<int32_t, BlobDescBuffer> SkinBuffers;
			std::unordered_map<int32_t, BlobDescBuffer> VertexBuffers;
			std::unordered_map<int32_t, BlobDescBuffer> MorphBuffers;
			std::unordered_map<int32_t, BlobVertexLayout> VertexLayouts;
			std::vector<std::vector<BlobManufacturerColor>> ManufacturerColors;
			std::vector<std::string> MaterialInstances;
			std::vector<Material> MaterialInstanceBundles;
			std::vector<ShaderParameter> ShaderParameters;
			std::vector<BlobTexture> Textures;
		};

		BundleData bundle{};

		bool Init()
		{
			if (p_stream->fail()) {
				std::cout << "Failed to open stream \n";

				return false;
			}

			// 1.0 initial - FM5; FH2
			// 1.1 change blob_count type from 2-byte to 4-byte - FM6Apex, FM2023; FH3, FH5

			header.tag = this->Read<uint32_t>(); // m_Tag
			if (header.tag != TAG_BUNDLE) {
				printf("Error: File signature does not match.\n");

				return false;
			}

			header.version = this->Read<Version>(); // m_VersionMajor, m_VersionMinor

			if (!IsAtMostVersion(header.version, 1, 1)) {
				printf("Warning: Unsupported Bundle version. Found: %s. Max supported: 1.1\n", VersionToString(header.version).c_str());
			}

			if (!IsAtLeastVersion(header.version, 1, 0)) {
				printf("Warning: Unsupported Bundle version. Found: %s. Min supported: 1.0\n", VersionToString(header.version).c_str());
			}

			if (IsAtLeastVersion(header.version, 1, 1)) {
				uint16_t __padding__ = this->Read<uint16_t>();
			}
			else {
				header.blob_count = this->Read<uint16_t>();
			}

			header.header_size = this->Read<uint32_t>(); // m_HeaderSize; includes metadata
			header.total_size = this->Read<uint32_t>(); // m_TotalSize

			if (IsAtLeastVersion(header.version, 1, 1)) {
				header.blob_count = this->Read<uint32_t>();
			}

			std::streampos base_pos = p_stream->tellg();

			std::vector<BundleBlobHeader> blobHeaders;
			blobHeaders.reserve(header.blob_count);

			// Deserialize blob headers
			for (size_t i = 0; i < header.blob_count; ++i) {
				DeserializeBlobHeaders(blobHeaders);
			}

			// Deserialize BlobModel upfront of any other data;
			const auto& blobModelHeader = std::find_if(blobHeaders.begin(), blobHeaders.end(), [](auto& item) { return item.tag == TAG_BLOB_Model; });

			if (blobModelHeader != std::end(blobHeaders)) {
				std::unordered_map<std::string, std::any> metadata = DeserializeMetadata(*blobModelHeader);

				BlobModel blobModel;
				DeserializeBlob(*blobModelHeader, blobModel);

				bundle.Model.reserve(1);

				// reserve mesh count
				bundle.Meshes.reserve(blobModel.meshes_count);

				//m_IndexBuffers.reserve(m_Model.BuffersCount);
				//m_VertexBuffers.reserve(m_Model.BuffersCount);
				//m_MorphBuffers.reserve(m_Model.LayoutsCount);
				bundle.VertexLayouts.reserve(blobModel.layouts_count);

				bundle.Model.push_back({ blobModelHeader->version, std::move(metadata), std::move(blobModel) });
			}

			p_stream->seekg(base_pos, std::ios::beg);

			for (auto& blobHeader : blobHeaders) {
				std::unordered_map<std::string, std::any> metadata = DeserializeMetadata(blobHeader);

				switch (blobHeader.tag)
				{
				case TAG_BLOB_Skeleton:
				{
					DeserializeBlob(blobHeader, bundle.Skeleton);
					break;
				}
				case TAG_BLOB_MaterialInstance: // m_MaterialEntries
				{
					//std::shared_ptr<BundleReader> bundle_instance = nullptr;
					//DeserializeBlob(blobHeader, bundle_instance);

					std::vector<char> bundle_instance;
					DeserializeBlob(blobHeader, bundle_instance);

					bundle.MaterialInstanceBundles.push_back({ blobHeader.version, std::move(metadata), bundle_instance });

					break;
				}
				case TAG_BLOB_MATI:
				case TAG_BLOB_MATL:
				{
					DeserializeBlob(blobHeader, bundle.MaterialInstances);
					break;
				}
				case TAG_BLOB_DefaultShaderParameter:
				case TAG_BLOB_MaterialShaderParameter:
				{
					DeserializeBlob(blobHeader, bundle.ShaderParameters);
					break;
				}
				case TAG_BLOB_ManufacturerColors: // CarManufacturerColors::Load()
				{
					DeserializeBlob(blobHeader, bundle.ManufacturerColors);
					break;
				}
				case TAG_BLOB_Mesh:
				{
					//printf("BLOB %s ver. %i.%i (0x%04Xh)\n", GetTagAsString(blobHeader.tag).c_str(), blobHeader.version.major, blobHeader.version.minor, blobHeader.data_offset);
					//printf("BLOB: %s, Metadata: %i \n", GetTagAsString(blobHeader.tag).c_str(), int(metadata.size()));

					BlobMesh mesh;
					DeserializeBlob(blobHeader, &mesh);
					bundle.Meshes.emplace_back(std::move(metadata), std::move(mesh));

					break;
				}
				case TAG_BLOB_IndexBuffer:
				case TAG_BLOB_Skin:
				case TAG_BLOB_MorphBuffer:
				case TAG_BLOB_VertexBuffer:
				{
					//printf("BLOB %s ver. %i.%i (0x%04Xh)\n", GetTagAsString(blobHeader.tag).c_str(), blobHeader.version.major, blobHeader.version.minor, blobHeader.data_offset);

					BlobDescBuffer descBuffer;
					DeserializeBlob(blobHeader, &descBuffer);

					switch (blobHeader.tag)
					{
					case TAG_BLOB_IndexBuffer:
						bundle.IndexBuffers.emplace(std::any_cast<int32_t>(metadata["Id"]), std::move(descBuffer));
						break;
					case TAG_BLOB_Skin:
						bundle.SkinBuffers.emplace(std::any_cast<int32_t>(metadata["Id"]), std::move(descBuffer));
						break;
					case TAG_BLOB_MorphBuffer:
						bundle.MorphBuffers.emplace(std::any_cast<int32_t>(metadata["Id"]), std::move(descBuffer));
						break;
					case TAG_BLOB_VertexBuffer:
						bundle.VertexBuffers.emplace(std::any_cast<int32_t>(metadata["Id"]), std::move(descBuffer));
						break;
					}

					break;
				}
				case TAG_BLOB_VertexLayout:
				{
					//printf("BLOB %s ver. %i.%i (0x%04Xh)\n", GetTagAsString(blobHeader.tag).c_str(), blobHeader.version.major, blobHeader.version.minor, blobHeader.data_offset);
					//printf("BLOB: %s, Metadata: %i \n", GetTagAsString(blobHeader.tag).c_str(), int(metadata.size()));

					BlobVertexLayout descLayout;
					DeserializeBlob(blobHeader, descLayout);

					bundle.VertexLayouts.emplace(std::any_cast<int32_t>(metadata["Id"]), std::move(descLayout));

					break;
				}
				case TAG_BLOB_Morph:
				{
					//printf("BLOB %s ver. %i.%i (0x%04Xh)\n", GetTagAsString(blobHeader.tag).c_str(), blobHeader.version.major, blobHeader.version.minor, blobHeader.data_offset);
					//printf("BLOB: %s, Metadata: %i \n", GetTagAsString(blobHeader.tag).c_str(), int(metadata.size()));

					BlobMorphTable blobMorphTable;
					DeserializeBlob(blobHeader, blobMorphTable);

					bundle.MorphTables.reserve(blobMorphTable.Data.size());
					bundle.MorphTables.insert(bundle.MorphTables.begin(), blobMorphTable.Data.begin(), blobMorphTable.Data.end());

					break;
				}
				case TAG_BLOB_TextureContentBlob:
				{
					BlobTexture blobTexture;
					blobTexture.version = blobHeader.version;
					DeserializeBlob(blobHeader, blobTexture.data);
					blobTexture.header = std::move(std::any_cast<TextureContent>(metadata["TXCH"]));

					bundle.Textures.push_back({ std::move(blobTexture) });

					break;
				}
				case TAG_BLOB_Model: // ignored
				{
					continue;
				}
				default:
					//printf("BLOB: %s not implemented \n", GetTagAsString(blobHeader.tag).c_str());
					break;
				}

				//printf("MetaDataOffset: 0x%04Xh \n", blobHeader.MetaDataOffset); //or %#08x 
			}

			//printf("Current Offset: 0x%04Xh \n", static_cast<int32_t>(p_stream->tellg()));
			//std::cout << "Current pos: " << p_stream->tellg() << '\n';

			return true;
		}

	protected:
		BundleHeader header;

	private:
		std::unique_ptr<std::istream> p_stream = nullptr;
		std::unique_ptr<mbuff> p_buffer = nullptr;

		bool IsAtLeastVersion(const Version& version, uint8_t major, uint8_t minor) {
			return version.major > major || version.major == major && version.minor >= minor;
		}

		bool IsAtMostVersion(const Version& version, uint8_t major, uint8_t minor) {
			return version.major < major || version.major == major && version.minor <= minor;
		}

		bool IsVersion(const Version& version, uint8_t major, uint8_t minor) {
			return version.major == major && version.minor == minor;
		}

		std::string VersionToString(const Version& version) {
			std::string result{};
			result += std::to_string(version.major);
			result += ".";
			result += std::to_string(version.minor);

			return result;
		}

		std::string GetTagAsString(uint32_t Tag) {
			std::string result;
			for (int32_t i = 24; i >= 0; i -= 8) {
				char c = static_cast<char>(Tag >> i);
				if (c != 0x20) {
					result.push_back(c);
				}
			}

			return result;
		}

		template <typename T>
		T Read() {
			T value;
			p_stream->read(reinterpret_cast<char*>(&value), sizeof(T));

			return value;
		}

		GUID ReadGUID() {
			GUID guid;
			p_stream->read(reinterpret_cast<char*>(&guid), sizeof(GUID));

			return guid;
		}

		int32_t read7BitEncodedInteger() {
			char current;
			int32_t index = 0, result = 0;
			do {
				p_stream->read(reinterpret_cast<char*>(&current), sizeof(char));
				result |= (current & 127) << index;
				index += 7;
			} while ((current & 128) != 0);

			return result;
		}

		template <typename T>
		std::string ReadStringByType() {
			std::string value;
			T size;

			p_stream->read(reinterpret_cast<char*>(&size), sizeof(T));
			value.resize(size);

			p_stream->read(reinterpret_cast<char*>(value.data()), size);

			return value;
		}

		std::string ReadString(size_t size) {
			std::string value;
			value.resize(size);
			p_stream->read(reinterpret_cast<char*>(value.data()), size);

			return value;
		}

		DXGI_FORMAT TextureContentToTextureFormat(uint32_t encoding, uint32_t transcoding, ColorProfile color_profile) { // 142367780
			if (transcoding <= 1) {
				if (color_profile == sRGB) {
					switch (encoding) {
					case 0:
						return DXGI_FORMAT_BC1_UNORM_SRGB;
					case 1:
						return DXGI_FORMAT_BC2_UNORM_SRGB;
					case 2:
						return DXGI_FORMAT_BC3_UNORM_SRGB;
					case 3:
						return DXGI_FORMAT_BC4_UNORM;
					case 4:
						return DXGI_FORMAT_BC4_SNORM;
					case 5:
						return DXGI_FORMAT_BC5_UNORM;
					case 6:
						return DXGI_FORMAT_BC5_SNORM;
					case 9:
					case 22:
						return DXGI_FORMAT_BC7_UNORM_SRGB;
					case 13:
						return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
					}
				}
				else {
					switch (encoding) {
					case 0:
						return DXGI_FORMAT_BC1_UNORM;
					case 1:
						return DXGI_FORMAT_BC2_UNORM;
					case 2:
						return DXGI_FORMAT_BC3_UNORM;
					case 3:
						return DXGI_FORMAT_BC4_UNORM;
					case 4:
						return DXGI_FORMAT_BC4_SNORM;
					case 5:
						return DXGI_FORMAT_BC5_UNORM;
					case 6:
						return DXGI_FORMAT_BC5_SNORM;
					case 7:
						return DXGI_FORMAT_BC6H_UF16;
					case 8:
						return DXGI_FORMAT_BC6H_SF16;
					case 9:
					case 22:
						return DXGI_FORMAT_BC7_UNORM;
					case 10:
						return DXGI_FORMAT_R32G32B32A32_FLOAT;
					case 11:
						return DXGI_FORMAT_R16G16B16A16_UNORM;
					case 12:
						return DXGI_FORMAT_R16G16B16A16_FLOAT;
					case 13:
						return DXGI_FORMAT_R8G8B8A8_UNORM;
					case 14:
						return DXGI_FORMAT_B5G6R5_UNORM;
					case 15:
						return DXGI_FORMAT_B5G5R5A1_UNORM;
					case 19:
						return DXGI_FORMAT_R8_UNORM;
					case 20:
						return DXGI_FORMAT_A8_UNORM;
					case 21:
						return DXGI_FORMAT_R8G8_UNORM;
					}
				}
			}
			else {
				if (color_profile == sRGB) {
					switch (encoding) {
					case 2:
						return DXGI_FORMAT_BC1_UNORM_SRGB;
					case 3:
						return DXGI_FORMAT_BC2_UNORM_SRGB;
					case 4:
						return DXGI_FORMAT_BC3_UNORM_SRGB;
					case 5:
						return DXGI_FORMAT_BC4_UNORM;
					case 6:
						return DXGI_FORMAT_BC4_SNORM;
					case 7:
						return DXGI_FORMAT_BC5_UNORM;
					case 8:
						return DXGI_FORMAT_BC5_SNORM;
					case 11:
						return DXGI_FORMAT_BC7_UNORM_SRGB;
					}
				}
				else {
					switch (encoding) {
					case 2:
						return DXGI_FORMAT_BC1_UNORM;
					case 3:
						return DXGI_FORMAT_BC2_UNORM;
					case 4:
						return DXGI_FORMAT_BC3_UNORM;
					case 5:
						return DXGI_FORMAT_BC4_UNORM;
					case 6:
						return DXGI_FORMAT_BC4_SNORM;
					case 7:
						return DXGI_FORMAT_BC5_UNORM;
					case 8:
						return DXGI_FORMAT_BC5_SNORM;
					case 9:
						return DXGI_FORMAT_BC6H_UF16;
					case 10:
						return DXGI_FORMAT_BC6H_SF16;
					case 11:
						return DXGI_FORMAT_BC7_UNORM;
					}
				}
			}
			return DXGI_FORMAT_UNKNOWN;
		}

		void DeserializeBlobHeaders(std::vector<BundleBlobHeader>& blobHeaders) {
			int32_t tag = this->Read<int32_t>();
			Version version = this->Read<Version>();
			uint16_t metadata_count = this->Read<uint16_t>();
			uint32_t metadata_offset = this->Read<uint32_t>();
			uint32_t data_offset = this->Read<uint32_t>();
			uint32_t data_size_unused = this->Read<uint32_t>();
			uint32_t data_size = this->Read<uint32_t>();

			blobHeaders.emplace_back(tag, version, metadata_count, metadata_offset, data_offset, data_size);
		}

		std::any GetMetadata(BundleBlobHeader& blobHeader, BundleBlobMetadata& header) {
			std::any result;

			switch (header.tag) {
			case TAG_METADATA_Name:
			{
				// 0 initial - FM5; FH2, FH3, FH5
				if (header.version > 0) {
					printf("Warning: Unsupported 'Name' metadata version. Found: %d. Max supported: 0.\n", header.version);
				}
				if (header.data_size > 0) {
					std::string data = this->ReadString(header.data_size);
					result = std::make_any<std::string>(data);
				}
				else {
					result = std::make_any<std::string>("<none>");
				}

				break;
			}
			case TAG_METADATA_BBox:
			{
				// 0 initial - FM5; FH2, FH3, FH5
				AABB bbox{};
				p_stream->read(reinterpret_cast<char*>(&bbox.min), 3 * sizeof(float));
				p_stream->read(reinterpret_cast<char*>(&bbox.max), 3 * sizeof(float));

				result = std::make_any<AABB>(bbox);

				break;
			}
			case TAG_METADATA_Identifier:
			{
				// 0 initial - FM5; FH2, FH3, FH5
				int32_t Id;
				p_stream->read(reinterpret_cast<char*>(&Id), header.data_size);

				result = std::make_any<int32_t>(Id);

				break;
			}
			case TAG_METADATA_TextureReferences:
			{
				// 0 initial - FM5; FH2, FH3, FH5
				if (header.version > 0) {
					printf("Warning: Unsupported 'TRef' metadata version. Found: %d. Max supported: 0.\n", header.version);
				}

				uint32_t texture_path_hashes_length = this->Read<uint32_t>(); // unsigned long count;

				std::vector<unsigned long> texture_path_hashes;
				for (size_t i = 0; i < texture_path_hashes_length; ++i) {
					uint32_t texture_path_hash = this->Read<uint32_t>();
					texture_path_hashes.push_back(texture_path_hash);
				} //swatchbin path CRC-32 lowercase // std::vector<unsigned long>

				break;
			}
			case TAG_METADATA_ATST:
			{
				// 1 initial - FH2, FH3
				// 2 add unk_v2 - FH5

				if (header.version > 2) {
					printf("Warning: Unsupported 'ATST' metadata version. Found: %d. Max upported: 2.\n", header.version);
				}
				if (header.version < 1) {
					printf("Warning: Unsupported 'ATST' metadata version. Found: %d. Min supported: 2.\n", header.version);
				}

				bool unk = this->Read<uint8_t>(); // bool
				if (header.version >= 2) {
					bool unk_v2 = this->Read<uint8_t>(); // bool
				}

				break;
			}
			case TAG_METADATA_BLEN: // boolean enable?
			{
				// 1 initial - FH3, FH5
				if (header.version != 1) {
					printf("Warning: Unsupported 'BLEN' metadata version. Found: %d. Supported: 1.\n", header.version);
				}

				bool unk0 = this->Read<uint8_t>(); // bool
				bool unk1 = this->Read<uint8_t>(); // bool

				break;
			}
			case TAG_METADATA_ACMR:
			{
				if (header.version > 0) {
					printf("Warning: Unsupported 'ACMR' metadata version. Found: %d. Max supported: 0.\n", header.version);
				}
				float average_cache_miss_ratio = this->Read<float>();

				break;
			}
			case TAG_METADATA_TextureContentHeader:
			{
				TextureContent data{};

				if (IsVersion(blobHeader.version, 2, 0)) { // TextureContent2, Durango
					data.guid = ReadGUID(); // instead of "<unnamed>"
					data.width = this->Read<uint16_t>();
					data.height = this->Read<uint16_t>();
					data.depth = this->Read<uint16_t>();
					uint16_t width_unused = this->Read<uint16_t>();
					uint16_t height_unused = this->Read<uint16_t>();
					uint16_t depth_unused = this->Read<uint16_t>();
					data.mip_levels = this->Read<uint8_t>();
					uint8_t mip_levels_unused = this->Read<uint8_t>();
					uint8_t gap1E = this->Read<uint8_t>(); // base mip level?

					struct BitField_Struct
					{
						uint32_t tile_mode : 5; // x & 0x1F, 14236BD28; XG_TILE_MODE
						uint32_t encoding : 6; //< read = Str("%d - %s", this, DxgiFormatToString(TextureContentToTextureFormat(this, 0, 0))) >;
						uint32_t transcoding : 6;// < read = Str("%d - %s", this, DxgiFormatToString(TextureContentToTextureFormat(-1, this, 0))) >;
						uint32_t color_profile_unused : 3; // (x >> 17) & 0x7, 14236BD13
						uint32_t color_profile : 3;
						uint32_t unk1 : 2; // same as PC's dword2C?
						uint32_t is_texture_cube : 1; // (1 << 25), 14236CDAB; name: 142355508
						uint32_t is_texture_3d : 1; // (1 << 26), 1423535D6; name: 1423554FE
						uint32_t unk2 : 1; // (1 << 27), 14236C23C; same as TextureContent.byte1F & 0x2
						uint32_t pitch_or_linear_size_bit : 4;// < comment = Str("0x%08X", 1 << (this + 4)) >; // (1 >> 28), 14236BD33; PitchOrLinearSize = 1 << (value + 4); width * 16?
					};
					const auto bitfield = this->Read<BitField_Struct>();

					data.tile_mode = bitfield.tile_mode; // x & 0x1F, 14236BD28; XG_TILE_MODE
					data.encoding = TextureContentToTextureFormat(bitfield.encoding, 0, ColorProfile::NoProfile); //< read = Str("%d - %s", this, DxgiFormatToString(TextureContentToTextureFormat(this, 0, 0))) > ;
					data.transcoding = TextureContentToTextureFormat(-1, bitfield.transcoding, ColorProfile::NoProfile);// < read = Str("%d - %s", this, DxgiFormatToString(TextureContentToTextureFormat(-1, this, 0))) >;
					//uint8_t color_profile_unused; // (x >> 17) & 0x7, 14236BD13
					data.color_profile = ColorProfile(bitfield.color_profile);
					//uint32_t unk00; // same as PC's dword2C?
					data.is_texture_cube = bitfield.is_texture_cube; // (1 << 25), 14236CDAB; name: 142355508
					data.is_texture_3d = bitfield.is_texture_3d; // (1 << 26), 1423535D6; name: 1423554FE
					//uint32_t unk000000; // (1 << 27), 14236C23C; same as TextureContent.byte1F & 0x2
					data.pitch_or_linear_size_bit = bitfield.pitch_or_linear_size_bit; //< comment = Str("0x%08X", 1 << (this + 4)) >; // (1 >> 28), 14236BD33; PitchOrLinearSize = 1 << (value + 4); width * 16?
					//data <comment = Str("Format: %s", DxgiFormatToString(TextureContentToTextureFormat(encoding, transcoding, color_profile)))>; // Durango, Xbox One; struct TextureContent2	
				}
				else {
					int64_t p = int64_t(p_stream->tellg());

					uint32_t parameters_offset = this->Read<uint32_t>();
					uint32_t subresources_offset = this->Read<uint32_t>();

					data.guid = ReadGUID();
					data.width = this->Read<uint32_t>(); // m_Width; 14236D178
					data.height = this->Read<uint32_t>(); // m_Height
					data.depth = this->Read<uint32_t>(); // m_Depth
					data.num_slices = this->Read<int16_t>(); // m_NumSlices; & 0x3FFF; D3D12_RESOURCE_DESC.DepthOrArraySize
					data.mip_levels = this->Read<uint8_t>();

					struct BitField_Struct
					{
						uint8_t is_texture_cube : 1; // byte1F; 14236D1D7, 142355646
						uint8_t unk : 1;
						uint8_t __padding__ : 6; // & 0xFC
					};
					const auto bitfield = this->Read<BitField_Struct>(); // byte1F; 14236D1D7, 142355646

					data.is_texture_cube = bitfield.is_texture_cube; // byte1F; 14236D1D7, 142355646
					uint8_t unk000 = bitfield.unk;
					uint8_t __padding__ = bitfield.__padding__; // & 0xFC

					int32_t transcoding = DXGI_FORMAT(this->Read<int32_t>()); //<read = Str("%d - %s", this, DxgiFormatToString(TextureContentToTextureFormat(-1, this, 0))), comment = "TextureTranscoding::Enum">;
					data.transcoding = TextureContentToTextureFormat(-1, data.transcoding, ColorProfile::NoProfile);

					ColorProfile color_profile_unused = ColorProfile(this->Read<uint32_t>());
					data.color_profile = ColorProfile(this->Read<uint32_t>()); // ColorProfile::Enum
					uint32_t dword2C = this->Read<uint32_t>();
					uint32_t value_offset = this->Read<uint32_t>(); //<hidden = true>; // TODO: add OpusProfile addres
					int64_t p2 = int64_t(p_stream->tellg());
					p_stream->seekg(p + value_offset);

					int32_t encoding = DXGI_FORMAT(this->Read<int32_t>());// <read = Str("%d - %s", this, DxgiFormatToString(TextureContentToTextureFormat(this, 0, 0))), comment = "TextureEncoding::Enum">;
					data.encoding = TextureContentToTextureFormat(encoding, data.transcoding, data.color_profile);

					p_stream->seekg(p2);

					// Do not deserialize 
					{
						int next_offset_local = parameters_offset;

						//<comment = Str("Format: %s", DxgiFormatToString(TextureContentToTextureFormat(encoding, transcoding, color_profile)))>;
						while (next_offset_local != -1) {
							p_stream->seekg(p + next_offset_local);
							uint32_t value_offset = this->Read<uint32_t>(); // relative to TXCH data begin
							int32_t next_offset = this->Read<int32_t>();
							next_offset_local = next_offset;

							p2 = int64_t(p_stream->tellg());
							p_stream->seekg(p + value_offset);
							uint32_t value = this->Read<uint32_t>();
							p_stream->seekg(p2);

							// paramenter
							// [0] - TextureEncoding::Enum encoding; count == numSlices??
							// [1] - int PitchOrLinearSize; Pitch for uncompressed, LinearSize for BC compressed?
						}

						next_offset_local = subresources_offset;
						while (next_offset_local != -1) {
							p_stream->seekg(p + next_offset_local);

							uint32_t value_offset = this->Read<uint32_t>(); // relative to TXCB data begin
							int32_t next_offset = this->Read<int32_t>();
							next_offset_local = next_offset;

							//subresource
							// count == mips
						} // reader: 1423542CF
					}
				}

				result = std::make_any<TextureContent>(data);

				break;
			}
			default:
			{
				//printf("Metadata %s not implemented.", GetTagAsString(header.tag).c_str());
				break;
			}
			}

			return result;
		}

		std::unordered_map<std::string, std::any> DeserializeMetadata(BundleBlobHeader& blobHeader) {
			std::unordered_map<std::string, std::any> result(blobHeader.metadata_count);

			std::streampos returnOffset = p_stream->tellg();

			if (blobHeader.metadata_count > 0) {
				p_stream->seekg(blobHeader.metadata_offset, std::ios::beg);

				for (size_t attr = 0; attr < blobHeader.metadata_count; ++attr) {
					std::streampos returnOffsetTag = p_stream->tellg();

					BundleBlobMetadata blobMetadata = this->Read<BundleBlobMetadata>();
					/*
					blobMetadata.tag = this->Read<uint32_t>();

					{ // possible name: Version
						struct Bitfield {
							uint16_t version : 4;
							uint16_t data_size : 12;
						};

						auto bitfield = this->Read<Bitfield>();

						blobMetadata.version = bitfield.version;
						blobMetadata.data_size = bitfield.data_size;
					}

					blobMetadata.data_offset = this->Read<uint16_t>();
					*/
					std::streampos returnOffsetData = p_stream->tellg();

					p_stream->seekg(returnOffsetTag, std::ios::beg);
					p_stream->seekg(blobMetadata.data_offset, std::ios::cur);

					result.emplace(GetTagAsString(blobMetadata.tag), GetMetadata(blobHeader, blobMetadata));

					p_stream->seekg(returnOffsetData, std::ios::beg);
				}

				p_stream->seekg(returnOffset, std::ios::beg);
			}

			return result;
		}

		void DeserializeBlob(const BundleBlobHeader& blobHeader, BlobMesh* blobMesh) {
			std::streampos returnOffset = p_stream->tellg();

			p_stream->seekg(blobHeader.data_offset, std::ios::beg);

			// 1.0 initial
			// 1.1 add unk__v1_1
			// 1.2 add skinning_elements_count, morph_weights_count
			// 1.3 add unk__v1_3
			// 1.4 add morph_data_buffer_id, skinning_data_buffer_id
			// 1.5 add texcoord_transform
			// 1.6 add average_cache_miss_ratio, vertices_count
			// 1.7 add not_opaque and not_transparent to render_pass bitfield - FM5; FH2
			// 1.8 add position_scale, position_translate - FM2023; FH3
			// 1.9 replace material_id with material_ids[4] - FH4, FH5

			blobMesh->version = blobHeader.version;

			if (!IsAtMostVersion(blobHeader.version, 1, 12)) {
				printf("Warning: Unsupported 'Mesh' blob version. Found: %s. Max supported: 1.12\n", VersionToString(blobHeader.version).c_str());
			}

			if (!IsAtLeastVersion(blobHeader.version, 1, 0)) {
				printf("Warning: Unsupported 'Mesh' blob version. Found: %s. Min supported: 1.0\n", VersionToString(blobHeader.version).c_str());
			}

			if (IsAtLeastVersion(blobHeader.version, 1, 9)) {
				for (size_t i = 0; i < 4; ++i) {
					int16_t material_id = this->Read<int16_t>(); // short
					blobMesh->material_ids.push_back(material_id);
				}

				blobMesh->material_id = std::make_optional(blobMesh->material_ids[1]); // short
				//Printf("%d\t%d\t%d\t%d\t%s\n", Unk_MaterialID_FH3, UnkV9_MaterialID, UnkV9[0], UnkV9[1], metadata[0].Name);
			}
			else
			{
				blobMesh->material_id = std::make_optional(this->Read<int16_t>()); // short
				//Printf("%s %d\n", metadata[0].Name, Unk_MaterialID_FH3);
			}

			blobMesh->bone_index = this->Read<int16_t>(); // short
			//Printf("%s %d\n", metadata[0].Name, BoneId);
			blobMesh->levels_of_detail = this->Read<int16_t>(); //LODFlags(); // unsigned short; don't render if == 0
			blobMesh->lowest_level_of_detail = this->Read<uint8_t>(); // char; don't render if sign bit is set (& 0x10 == 0); 14251B9BD
			blobMesh->highest_level_of_detail = this->Read<uint8_t>(); // char
			blobMesh->render_pass = this->Read<uint16_t>(); //RenderPass(); // unsigned short
			//Printf("%d\t%s\n", render_pass.value, metadata[0].Name);
			uint8_t char1BA = this->Read<uint8_t>(); // char

			if (IsAtLeastVersion(blobHeader.version, 1, 2)) {
				blobMesh->skinning_elements_count = this->Read<uint8_t>(); // unsigned char
				//blobMesh->morph_weights_count = this->Read<uint8_t>(); // unsigned char
				//Printf("%s %d\n", metadata[0].Name, UnkV2_WeightsCount);

				if (IsAtLeastVersion(blobHeader.version, 1, 12)) {
					blobMesh->morph_weights_count = this->Read<uint32_t>();
				}
				else {
					blobMesh->morph_weights_count = this->Read<uint8_t>(); // unsigned char
				}
			}

			if (IsAtLeastVersion(blobHeader.version, 1, 3)) {
				uint8_t unk__v1_3 = this->Read<uint8_t>(); // bool; objectCBuffer.g_DecompressFlags[1] ?
			}

			uint8_t bool1BE = this->Read<uint8_t>(); // bool; objectCBuffer.g_DecompressFlags[0] ?
			uint16_t dword1C4 = this->Read<uint16_t>(); // unsigned short
			blobMesh->index_buffer_id = this->Read<int32_t>(); // long; 'IndB'; metadata 'Id  ', buffer_type = 1
			uint32_t ulong1CC = this->Read<uint32_t>(); // unsigned long
			blobMesh->start_index = this->Read<int32_t>(); // long; StartIndexLocation
			blobMesh->base_vertex = this->Read<int32_t>(); // long; BaseVertexLocation; VBuf[buf_id][IndB[id]+VertIndexOffset], "Base vertex offset magnitude"
			blobMesh->indexes_count = this->Read<uint32_t>(); // unsigned long; IndexCountPerInstance
			blobMesh->faces_count = this->Read<uint32_t>(); // unsigned long

			if (IsAtLeastVersion(blobHeader.version, 1, 6)) {
				blobMesh->average_cache_miss_ratio = this->Read<float>(); // float; =vertices_count/faces_count
				blobMesh->vertices_count = this->Read<uint32_t>(); // unsigned long; sometimes zero. if average_cache_miss_ratio > 1?
			}

			if (IsAtLeastVersion(blobHeader.version, 1, 12)) {
				uint32_t unk_count_v1 = this->Read<uint32_t>();

				for (size_t i = 0; i < unk_count_v1; ++i) {
					int32_t unk_values_v1 = this->Read<int32_t>();
				}
			}

			blobMesh->vertex_layout_id = this->Read<uint32_t>(); // unsigned long; ordinal, not metadata 'Id  '
			uint32_t vertex_buffer_ids_length = this->Read<uint32_t>(); // unsigned long

			blobMesh->vertex_buffer_ids.resize(vertex_buffer_ids_length);

			for (size_t i = 0; i < vertex_buffer_ids_length; ++i) {
				int32_t id = this->Read<int32_t>(); // metadata 'Id  ', buffer_type = 2
				uint32_t input_slot = this->Read<uint32_t>();
				uint32_t stride = this->Read<uint32_t>();
				uint32_t offset = this->Read<uint32_t>();

				if (IsAtLeastVersion(blobHeader.version, 1, 12)) {
					uint32_t unk_value_v2 = this->Read<uint32_t>();
				}

				blobMesh->vertex_buffer_ids[i] = { id, input_slot, stride, offset };
			}

			if (IsAtLeastVersion(blobHeader.version, 1, 4)) {
				blobMesh->morph_data_buffer_id = this->Read<int32_t>(); // long; metadata 'Id  ', buffer_type = 4
				blobMesh->skinning_data_buffer_id = this->Read<int32_t>(); // long; metadata 'Id  ', buffer_type = 5
			}

			uint32_t constant_buffer_ids_length = this->Read<uint32_t>(); // unsigned long
			if (constant_buffer_ids_length > 0) {
				for (size_t i = 0; i < constant_buffer_ids_length; ++i) {
					int32_t constant_buffer_ids = this->Read<int32_t>(); // m_ConstantBufferIndices; 'ConB'; metadata 'Id  ', buffer_type = 3
				}
			}

			if (IsAtLeastVersion(blobHeader.version, 1, 1)) {
				uint32_t ulong1E0__v1_1 = this->Read<uint32_t>(); // unsigned long
			}

			if (IsAtLeastVersion(blobHeader.version, 1, 5)) {
				for (size_t i = 0; i < 5; ++i) {
					float v_translation = this->Read<float>();
					float v_scale = this->Read<float>();
					float u_translation = this->Read<float>();
					float u_scale = this->Read<float>();

					blobMesh->texcoord_transform[i] = { v_translation, v_scale, u_translation, u_scale };

				} // DirectX::XMVECTOR; union __m128 {{v translation, v scale} x2}; objectCBuffer.g_UVScale
			}

			if (IsAtLeastVersion(blobHeader.version, 1, 8)) {
				DirectX::XMFLOAT4 position_scale; // DirectX::XMVECTOR; objectCBuffer.g_PosScale
				DirectX::XMFLOAT4 position_translate; // DirectX::XMVECTOR; objectCBuffer.g_PosOffset

				p_stream->read(reinterpret_cast<char*>(&position_scale), 16);
				p_stream->read(reinterpret_cast<char*>(&position_translate), 16);

				blobMesh->position_scale = std::make_optional(position_scale);
				blobMesh->position_translate = std::make_optional(position_translate);
			}

			p_stream->seekg(returnOffset, std::ios::beg);
		}

		void DeserializeBlob(const BundleBlobHeader& blobHeader, std::vector<Bones>& blobSkel) {
			std::streampos returnOffset = p_stream->tellg();

			p_stream->seekg(blobHeader.data_offset, std::ios::beg);

			uint16_t bones_length = this->Read<uint16_t>();

			blobSkel.reserve(bones_length);

			for (size_t i = 0; i < bones_length; ++i) {
				std::string name = this->ReadStringByType<uint32_t>();
				int16_t parent_index = this->Read<int16_t>(); // Left-child right-sibling representation
				int16_t child_index = this->Read<int16_t>();
				int16_t next_index = this->Read<int16_t>();

				DirectX::XMMATRIX transform
				{
					this->Read<float>(), this->Read<float>(), this->Read<float>(), this->Read<float>(),
					this->Read<float>(), this->Read<float>(), this->Read<float>(), this->Read<float>(),
					this->Read<float>(), this->Read<float>(), this->Read<float>(), this->Read<float>(),
					this->Read<float>(), this->Read<float>(), this->Read<float>(), this->Read<float>()
				};

				blobSkel.push_back({ name, parent_index, child_index, next_index, transform });
			}

			if (IsAtLeastVersion(blobHeader.version, 1, 0)) { // FH3 OpusProfile 142AE00B5

				uint32_t unk__v1_0_length = this->Read<uint32_t>(); // unsigned long

				if (unk__v1_0_length > 0) {
					std::vector<uint32_t> UnkV1(unk__v1_0_length);  // void *
					p_stream->read(reinterpret_cast<char*>(UnkV1.data()), sizeof(uint32_t));
				}
			}

			p_stream->seekg(returnOffset, std::ios::beg);
		}

		void DeserializeBlob(const BundleBlobHeader& blobHeader, BlobMorphTable& blobMorphTable) {
			std::streampos returnOffset = p_stream->tellg();

			p_stream->seekg(blobHeader.data_offset, std::ios::beg);

			uint16_t strings_length = this->Read<uint16_t>();

			blobMorphTable.Data.reserve(strings_length);

			for (size_t i = 0; i < strings_length; ++i) {
				//std::string strings = this->ReadStringByType<uint32_t>();
				blobMorphTable.Data.emplace_back(this->ReadStringByType<uint32_t>());
			}

			p_stream->seekg(returnOffset, std::ios::beg);
		}

		void DeserializeBlob(const BundleBlobHeader& blobHeader, BlobVertexLayout& blobLayout) {
			std::streampos returnOffset = p_stream->tellg();
			p_stream->seekg(blobHeader.data_offset, std::ios::beg);

			auto SemanticLPCSTR = [&](const std::string& semantic) {

				if (semantic == "POSITION")
				{
					return "POSITION";
				}

				if (semantic == "NORMAL")
				{
					return "NORMAL";
				}

				if (semantic == "TEXCOORD")
				{
					return "TEXCOORD";
				}

				if (semantic == "TANGENT")
				{
					return "TANGENT";
				}

				if (semantic == "COLOR")
				{
					return "COLOR";
				}

				return "INVALID";
				};

			uint16_t semantic_names_length = this->Read<uint16_t>();

			blobLayout.semantic_names.reserve(semantic_names_length);
			for (size_t elem = 0; elem < semantic_names_length; ++elem) {
				std::string semantic_name = this->ReadStringByType<uint32_t>();

				blobLayout.semantic_names.emplace_back(semantic_name);
			}

			uint16_t num_elements = this->Read<uint16_t>();

			blobLayout.elements.reserve(num_elements);
			for (size_t elem = 0; elem < num_elements; ++elem) {
				uint16_t semantic_name_index = this->Read<uint16_t>();
				uint16_t semantic_index = this->Read<uint16_t>();
				uint16_t input_slot = this->Read<uint16_t>();
				uint16_t input_slot_class = this->Read<uint16_t>();
				DXGI_FORMAT format = DXGI_FORMAT(this->Read<uint32_t>());
				int32_t aligned_byte_offset = this->Read<int32_t>();
				uint32_t instance_data_step_rate = this->Read<uint32_t>();

				blobLayout.elements.push_back({ SemanticLPCSTR(blobLayout.semantic_names[semantic_name_index]), semantic_index, format, static_cast<uint32_t>(input_slot), D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, instance_data_step_rate });
			}

			blobLayout.element_formats.reserve(num_elements);
			for (size_t elem = 0; elem < num_elements; ++elem) {
				int32_t PackedFormat = this->Read<int32_t>(); // FH3 OpusProfile 142ADCF74
				blobLayout.element_formats.emplace_back(PackedFormat);
			}

			if (IsAtLeastVersion(header.version, 1, 1)) {

				blobLayout.flags = this->Read<VLFlags>();

				// TEXCOORD/TEXCOORD_XONLY: 0x1, 0x2, 0x4, 0x8, 0x10
				// TANGENT: 0x20, 0x40, 0x80, 0x100, 0x200
				// COLOR: 0x400
			}

			p_stream->seekg(returnOffset, std::ios::beg);
		}

		void DeserializeBlob(const BundleBlobHeader& blobHeader, BlobModel& blobModel) {

			std::streampos returnOffset = p_stream->tellg();

			p_stream->seekg(blobHeader.data_offset, std::ios::beg);

			// 1.0 initial
			// 1.1 add 'Mrph' blob; size: 0xE - FM5; FH2
			// 1.2 add DecompressFlags field; size: 0x10 - FM6Apex, FM2023; FH3, FH5
			// 1.3 add unk__v1_3 field; size: 0x12 - FH5 v1.573.834.0
			// 1.4 FH6

			if (!IsAtMostVersion(header.version, 1, 4)) {
				printf("Warning: Unsupported 'Modl' blob version. Found: %s. Max supported: 1.4\n", VersionToString(header.version).c_str());
			}

			if (!IsAtLeastVersion(header.version, 1, 0)) {
				printf("Warning: Unsupported 'Modl' blob version. Found: %s. Min supported: 1.0\n", VersionToString(header.version).c_str());
			}

			blobModel.meshes_count = this->Read<uint16_t>(); // 2-byte; CommonModel::Mesh
			blobModel.buffers_count = this->Read<uint16_t>(); // 2-byte; IndB, VerB, ConB, MBuf, Skin; CommonModel::ModelBuffer?
			blobModel.layouts_count = this->Read<uint16_t>(); // 2-byte; ILay, VLay; CommonModel::VertexLayout?
			blobModel.materials_count = this->Read<uint16_t>(); // 2-byte
			blobModel.has_levels_of_detail = this->Read<uint8_t>(); // 1-byte
			blobModel.lowest_level_of_detail = this->Read<uint8_t>(); // 1-byte
			blobModel.highest_level_of_detail = this->Read<uint8_t>(); // 1-byte
			uint8_t __pad1__ = this->Read<uint8_t>();
			blobModel.levels_of_detail = std::make_optional(LODFlags(this->Read<uint16_t>())); // 2-byte; FH3 OpusDev forza_x64_profile.exe: 142A333BF
			blobModel.decompress_flags = this->Read<int8_t>(); //Store normal.x in position.w
			uint8_t __pad2__ = this->Read<uint8_t>();

			p_stream->seekg(returnOffset, std::ios::beg);
		}

		void DeserializeBlob(const BundleBlobHeader& blobHeader, BlobDescBuffer* blobDescBuffer) {
			std::streampos returnOffset = p_stream->tellg();

			p_stream->seekg(blobHeader.data_offset, std::ios::beg);

			blobDescBuffer->length = this->Read<int32_t>();
			blobDescBuffer->size = this->Read<int32_t>();
			blobDescBuffer->stride = this->Read<uint16_t>();

			if (IsAtLeastVersion(blobHeader.version, 1, 0)) {
				blobDescBuffer->num_elements = this->Read<uint8_t>(); // NumElements
				uint8_t __padding__ = this->Read<uint8_t>();
				blobDescBuffer->format = DXGI_FORMAT(this->Read<int32_t>());
				//DXGI_FORMAT format; // Format
			}
			else {
				// num_elements = 1
				// format = DXGI_FORMAT_UNKNOWN (0)
				uint16_t __padding__ = this->Read<uint16_t>(); //uint8_t[2]
			}

			blobDescBuffer->Buffer.resize(blobDescBuffer->size);
			p_stream->read(reinterpret_cast<char*>(blobDescBuffer->Buffer.data()), blobDescBuffer->Buffer.size());

			p_stream->seekg(returnOffset, std::ios::beg);
		}

		void DeserializeBlob(const BundleBlobHeader& blobHeader, std::vector<ShaderParameter>& blobShaderParameter)
		{
			std::streampos returnOffset = p_stream->tellg();
			p_stream->seekg(blobHeader.data_offset, std::ios::beg);

			// 1.0 initial
			// 2.0 - FM5; FH2, FH3
			// 2.1 change length type from ubyte to ushort - FH5

			if (!IsAtMostVersion(blobHeader.version, 2, 1)) {
				printf("Warning: Unsupported '%s' blob version. Found: %s. Max supported: 2.1\n", GetTagAsString(blobHeader.tag).c_str(), VersionToString(blobHeader.version).c_str());
			}

			if (!IsAtLeastVersion(blobHeader.version, 1, 0)) {
				printf("Warning: Unsupported '%s' blob version. Found: %s. Min supported: 1.0\n", GetTagAsString(blobHeader.tag).c_str(), VersionToString(blobHeader.version).c_str());
			}

			uint16_t length = 0;
			if (IsAtLeastVersion(blobHeader.version, 2, 1)) {
				length = this->Read<uint16_t>();
			}
			else {
				length = this->Read<uint8_t>();
			}

			if (length > 0) {
				if (IsAtLeastVersion(blobHeader.version, 2, 0)) {

					blobShaderParameter.reserve(length);

					for (size_t i = 0; i < length; ++i) {
						// 1.0 initial
						// 3.0 add guid__v3_0 - FH3
						// 3.1 add unk__v3_1 - FH5
						// 3.4 add unk__v3_4 - FH6

						Version parameter_version = this->Read<Version>(); // ReadData(1) x2
						if (!IsAtMostVersion(parameter_version, 3, 4)) {
							printf("Warning: Unsupported 'DFPR' blob parameter version. Found: %s. Max supported: 3.4\n", VersionToString(parameter_version).c_str());
						}
						if (!IsAtLeastVersion(parameter_version, 2, 0)) {
							printf("Warning: Unsupported 'DFPR' blob parameter version. Found: %s. Min supported: 2.0\n", VersionToString(parameter_version).c_str());
						}

						uint32_t hash = this->Read<uint32_t>();

						if (IsAtLeastVersion(parameter_version, 3, 1)) {
							bool has_unk__v3_1 = this->Read<uint8_t>(); // bool
							if (has_unk__v3_1) {
								uint32_t unk__v3_1 = this->Read<uint32_t>(); // unsigned long
							}
						}

						ShaderParameterType type = (ShaderParameterType)this->Read<uint8_t>(); // unsigned char
						GUID guid{};

						if (IsAtLeastVersion(parameter_version, 3, 0)) {
							guid = this->ReadGUID(); // ReadData(16); MTPR.guid__v3_0 == DFPR.guid__v3_0
						}

						std::any result{};

						switch (type)
						{
						case ShaderParameter_Vector:
						{
							DirectX::XMFLOAT4 value = this->Read<DirectX::XMFLOAT4>(); // ReadData(16)
							result = std::make_any<DirectX::XMFLOAT4>(value);

							break;
						}
						case ShaderParameter_Color:
						{
							DirectX::XMFLOAT4 value = this->Read<DirectX::XMFLOAT4>(); // calls ShaderParameter_Vector serialize
							result = std::make_any<DirectX::XMFLOAT4>(value);

							break;
						}
						case ShaderParameter_Float:
						{
							float value = this->Read<float>(); // ReadData(4)
							result = std::make_any<float>(value);

							break;
						}
						case ShaderParameter_Bool:
						{
							bool value = this->Read<uint32_t>(); // ReadData(4)
							result = std::make_any<bool>(value);

							break;
						}
						case ShaderParameter_Int: // ShaderParameterType::Int
						{
							int32_t value = this->Read<int32_t>(); // ReadData(4)
							result = std::make_any<int32_t>(value);

							break;
						}
						case ShaderParameter_Swizzle:
						{
							std::array<uint8_t, 16> value; // ReadData(16); Float4; calls ShaderParameter_Vector serialize
							p_stream->read(reinterpret_cast<char*>(value.data()), 16);

							result = std::make_any<std::array<uint8_t, 16>>(value);

							printf("SwizzleShaderParameter\n");

							break;
						}
						case ShaderParameter_Texture2D:
						{
							uint32_t path_len = this->read7BitEncodedInteger();
							std::string path = this->ReadString(path_len);

							if (IsAtLeastVersion(parameter_version, 2, 0)) {
								uint32_t path_hash = this->Read<uint32_t>();// unsigned_long
							}

							result = std::make_any<std::string>(path);

							break;
						}
						case ShaderParameter_Sampler:
						{
							uint8_t unk0[4]; // ReadData(4)
							uint8_t unk1[4]; // ReadData(4)

							p_stream->read(reinterpret_cast<char*>(&unk0), 4);
							p_stream->read(reinterpret_cast<char*>(&unk1), 4);

							if (IsAtLeastVersion(parameter_version, 1, 1)) {
								uint8_t unk__v1_1[4]; // ReadData(4); type?
								p_stream->read(reinterpret_cast<char*>(&unk__v1_1), 4);
							}

							break;
						}
						case ShaderParameter_ColorGradient:
						{
							uint32_t unk_length = this->Read<uint32_t>(); // ReadData(4)
							for (size_t i = 0; i < length; ++i)
							{
								uint32_t unk = this->Read<uint32_t>();
							} // _BYTE[unk_length * 32 / 8]; DXGI_FORMAT_R8G8B8A8_UNORM; Texture

							break;
						}
						case ShaderParameter_FunctionRange:
						{
							std::array<uint8_t, 16> value; // ReadData(16); Float4; calls ShaderParameter_Vector serialize
							p_stream->read(reinterpret_cast<char*>(value.data()), 16);

							result = std::make_any<std::array<uint8_t, 16>>(value);

							break;
						}
						case ShaderParameter_Vector2: // ShaderParameterType::Float2
						{
							DirectX::XMFLOAT2 value = this->Read<DirectX::XMFLOAT2>(); // ReadData(8)

							if (!IsAtLeastVersion(parameter_version, 2, 0)) { // removed since v2.0
								uint8_t unk__v1_0[8]; // ReadData(8)
								p_stream->read(reinterpret_cast<char*>(&unk__v1_0), 8);
							}

							result = std::make_any<DirectX::XMFLOAT2>(value);

							break;
						}
						case 0x0C: // unk FH6
						{
							// IsAtLeastVersion(parameter_version, 3, 4) !!
							std::array<uint8_t, 16> value; // ReadData(16);
							p_stream->read(reinterpret_cast<char*>(value.data()), 16);

							break;
						}
						default:
							printf("Warning: Unknown ShaderParameterType %02X .\n", static_cast<uint8_t>(type));

							break;
						}

						blobShaderParameter.push_back({ parameter_version, hash, type, guid, result });
					}

				}
				else {
					printf("Warning: 'DFPR' blob parameter version below v2.0 is not supported.\n");
				}

				if (header.tag == TAG_BLOB_MaterialShaderParameter && IsAtLeastVersion(blobHeader.version, 2, 0)) {
					uint32_t unk0 = this->Read<uint32_t>(); // unsigned long
					uint32_t unk1 = this->Read<uint32_t>(); // unsigned long
					uint32_t unk2 = this->Read<uint32_t>(); // unsigned long
				}
			}

			p_stream->seekg(returnOffset, std::ios::beg);
		}

		void DeserializeBlob(const BundleBlobHeader& blobHeader, std::vector<char>& buffer)
		{
			std::streampos returnOffset = p_stream->tellg();
			p_stream->seekg(blobHeader.data_offset, std::ios::beg);

			buffer.resize(blobHeader.data_size);
			p_stream->read(reinterpret_cast<char*>(buffer.data()), blobHeader.data_size);

			p_stream->seekg(returnOffset, std::ios::beg);
		}

		void DeserializeBlob(const BundleBlobHeader& blobHeader, std::vector<std::vector<BlobManufacturerColor>>& data) {
			std::streampos returnOffset = p_stream->tellg();
			p_stream->seekg(blobHeader.data_offset, std::ios::beg);

			// 1.0 initial
			// 1.1 change material_index_mask type from unsigned short to unsigned long
			// 2.0 update for FH6

			uint8_t manufacturer_colors_length = this->Read<uint8_t>(); // unsigned char; 142A24211

			//printf("manufacturer color\n");

			data.resize(manufacturer_colors_length);

			for (size_t i = 0; i < manufacturer_colors_length; ++i)
			{
				uint8_t materials_length = this->Read<uint8_t>(); // unsigned char

				for (size_t m = 0; m < materials_length; ++m) {

					uint32_t material_index_mask{};

					if (IsAtLeastVersion(header.version, 1, 1)) {
						material_index_mask = this->Read<uint32_t>(); // unsigned long
					}
					else {
						material_index_mask = this->Read<uint16_t>(); // unsigned short
					}

					std::vector<std::string> masks{};

					if (IsAtLeastVersion(blobHeader.version, 2, 0))
					{
						masks.reserve(material_index_mask);

						for (size_t mask_id = 0; mask_id < material_index_mask; ++mask_id)
						{
							int32_t path_len = this->read7BitEncodedInteger();
							//std::string path = this->ReadString(path_len);
							masks.emplace_back(this->ReadString(path_len));
						}
					}

					DirectX::XMFLOAT3 preview_color; // {R, G, B, 1}
					p_stream->read(reinterpret_cast<char*>(&preview_color), sizeof(DirectX::XMFLOAT3));

					int32_t path_len = this->read7BitEncodedInteger();
					std::string path = this->ReadString(path_len);

					data[i].push_back({ blobHeader.version, material_index_mask, std::make_optional(masks), preview_color, path });
				}

				if (IsAtLeastVersion(blobHeader.version, 2, 0))
				{
					uint8_t unk0 = this->Read<uint8_t>();
					DirectX::XMFLOAT3 preview_color_v1; // {R, G, B, 1}
					p_stream->read(reinterpret_cast<char*>(&preview_color_v1), sizeof(DirectX::XMFLOAT3));

					uint8_t unk1 = this->Read<uint8_t>();
					uint8_t unk2 = this->Read<uint8_t>();
					if (unk2 == 1) {
						DirectX::XMFLOAT3 preview_color_v2; // {R, G, B, 1}
						p_stream->read(reinterpret_cast<char*>(&preview_color_v2), sizeof(DirectX::XMFLOAT3));
					}
				}


			} // std::vector<std::vector<?>>

			p_stream->seekg(returnOffset, std::ios::beg);
		}

		void DeserializeBlob(const BundleBlobHeader& blobHeader, std::shared_ptr<BundleReader>& data)
		{
			std::streampos returnOffset = p_stream->tellg();
			p_stream->seekg(blobHeader.data_offset, std::ios::beg);

			std::vector<char> blob(blobHeader.data_size);
			p_stream->read(reinterpret_cast<char*>(blob.data()), blob.size());

			// 1.0 initial - FM5, FM2023; FH2, FH3, FH5
			if (!blob.empty())
			{
				auto material_instance_bundle = std::make_shared<BundleReader>(blob); // MaterialSystem::MaterialInstance::Load()
				if (material_instance_bundle->Init())
				{
					data = std::move(material_instance_bundle);
				}
			}

			p_stream->seekg(returnOffset, std::ios::beg);
		}

		void DeserializeBlob(const BundleBlobHeader& blobHeader, std::vector<std::string>& data)
		{
			switch (blobHeader.tag)
			{
			case TAG_BLOB_MATI:
			{
				std::streampos returnOffset = p_stream->tellg();
				p_stream->seekg(blobHeader.data_offset, std::ios::beg);

				// 1.0 initial - FM5, FM2023; FH2, FH3, FH5

				uint32_t path_len = this->read7BitEncodedInteger();
				std::string path = this->ReadString(path_len);

				// MaterialSystem::MaterialInstance::ReadMaterialInstanceBlob()

				data.emplace_back(path);

				p_stream->seekg(returnOffset, std::ios::beg);

				break;
			}
			case TAG_BLOB_MATL:
			{
				std::streampos returnOffset = p_stream->tellg();
				p_stream->seekg(blobHeader.data_offset, std::ios::beg);

				// 1.0 initial - FH3
				// 1.1
				// 1.2

				uint32_t path_len = this->read7BitEncodedInteger();
				std::string path = this->ReadString(path_len);

				if (IsAtLeastVersion(header.version, 1, 1)) {
					uint32_t path__v1_1_len = this->read7BitEncodedInteger();
					std::string path__v1_1 = this->ReadString(path__v1_1_len);
				}

				if (IsAtLeastVersion(header.version, 1, 2)) {
					uint32_t path__v1_2_len = this->read7BitEncodedInteger();
					std::string path__v1_2 = this->ReadString(path__v1_2_len);
				}

				// Material::ReadMaterialBlob()

				data.emplace_back(path);

				p_stream->seekg(returnOffset, std::ios::beg);

				break;
			}
			}
		}

	};
}
