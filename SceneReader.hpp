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
#include <map>
#include <bitset>
#include <DirectXMath.h>
#include <guiddef.h>

#include "mbuff.hpp"

namespace fmnext
{
	enum Series {
		Auto = 0,
		Motorsport = 1,
		Horizon = 2
	};

	enum CCarParts_Enum
	{
		CCarParts_Engine = 0,
		CCarParts_Drivetrain = 1,
		CCarParts_CarBody = 2,
		CCarParts_Motor = 3,
		CCarParts_Brakes = 4,
		CCarParts_SpringDamper = 5,
		CCarParts_AntiSwayFront = 6,
		CCarParts_AntiSwayRear = 7,
		CCarParts_TireCompound = 8,
		CCarParts_RearWing = 9,
		CCarParts_RimSizeFront = 10,
		CCarParts_RimSizeRear = 11,
		CCarParts_Camshaft = 12,
		CCarParts_Valves = 13,
		CCarParts_Displacement = 14,
		CCarParts_PistonsCompression = 15,
		CCarParts_FuelSystem = 16,
		CCarParts_Ignition = 17,
		CCarParts_Exhaust = 18,
		CCarParts_Intake = 19,
		CCarParts_Flywheel = 20,
		CCarParts_Manifold = 21,
		CCarParts_RestrictorPlate = 22,
		CCarParts_OilCooling = 23,
		CCarParts_SingleTurbo = 24,
		CCarParts_TwinTurbo = 25,
		CCarParts_QuadTurbo = 26,
		CCarParts_SuperchargerCSC = 27,
		CCarParts_SuperchargerDSC = 28,
		CCarParts_Intercooler = 29,
		CCarParts_Clutch = 30,
		CCarParts_Transmission = 31,
		CCarParts_Driveline = 32,
		CCarParts_Differential = 33,
		CCarParts_FrontBumper = 34,
		CCarParts_RearBumper = 35,
		CCarParts_Hood = 36,
		CCarParts_SideSkirts = 37,
		CCarParts_TireWidthFront = 38,
		CCarParts_TireWidthRear = 39,
		CCarParts_WeightReduction = 40,
		CCarParts_ChassisStiffness = 41,
		CCarParts_Ballast = 42,
		CCarParts_MotorParts = 43,
		CCarParts_WheelStyle = 44,
		CCarParts_Aspiration = 45,
	}; // enum CCarParts::Enum

	class SceneReader
	{
	public:
		SceneReader() = default;
		~SceneReader() = default;

		SceneReader(std::string& fileName, Series series = Auto) : m_fileName(fileName), series(series)
		{
			m_stream = std::make_unique<std::ifstream>(m_fileName, std::ios::binary);
		}

		SceneReader(std::vector<char>& fileBuffer, Series series) : series(series) 
		{
			m_buffer = std::make_unique<mbuff>(fileBuffer);
			m_stream = std::make_unique<std::istream>(m_buffer.get(), false);
		}

		struct AABB
		{
			DirectX::XMVECTOR min;
			DirectX::XMVECTOR max;
		};

		struct DrawGroups
		{
			DrawGroups() = default;
			DrawGroups(uint8_t value) : Exterior(std::bitset<8>(value).test(0)),
				Cockpit(std::bitset<8>(value).test(1)),
				Shadow(std::bitset<8>(value).test(2)),
				Hood(std::bitset<8>(value).test(3)),
				WindshieldReflection(std::bitset<8>(value).test(4)),
				DriverlessCockpit(std::bitset<8>(value).test(5)),
				WindshieldReflectionDriverlessCockpit(std::bitset<8>(value).test(6)),
				ProxyLOD(std::bitset<8>(value).test(7)) {}

			bool Exterior;
			bool Cockpit;
			bool Shadow;
			bool Hood;
			bool WindshieldReflection;
			bool DriverlessCockpit;
			bool WindshieldReflectionDriverlessCockpit; // = 1, if (WindshieldReflection && DriverlessCockpit)
			bool ProxyLOD;
			//int __padding__ : 24; ignore

		}; // std::bitset<8>; enum CarScene::DrawGroup::Enum

		struct LODFlags
		{
			LODFlags() = default;
			LODFlags(uint16_t value) :
				LODS(std::bitset<8>(value).test(0)),
				LOD0(std::bitset<8>(value).test(1)),
				LOD1(std::bitset<8>(value).test(2)),
				LOD2(std::bitset<8>(value).test(3)),
				LOD3(std::bitset<8>(value).test(4)),
				LOD4(std::bitset<8>(value).test(5)),
				LOD5(std::bitset<8>(value).test(6)) {}

			bool LODS;
			bool LOD0;
			bool LOD1;
			bool LOD2;
			bool LOD3;
			bool LOD4;
			bool LOD5;
		};

		struct AOMapInfo
		{
			uint16_t version;
			std::string path;
			GUID guid;
		}; // CarScene::ICarRenderModel::AOMapInfo

		struct CarRenderModel11
		{
			uint16_t version;
			std::string path;
			DirectX::XMMATRIX transform;
			LODFlags levels_of_detail;
			std::string bone_name;
			uint16_t bone_id;
			DrawGroups draw_groups;
			std::optional<std::string> ao_swatchbin_path;
			std::map<std::string, std::vector<unsigned char>> material_overrides;
			std::map<std::string, int> material_indexes;
			bool is_droppable;
			std::optional<float> drop_value;
			std::optional<uint32_t> drop_part_id;
			float break_amount;
			std::vector<AOMapInfo> AOMapInfo; //CarScene::ICarRenderModel::AOMapInfo
			bool is_interior_windshield;
			std::string type;
			GUID guid_v13;
			GUID guid_v14;
			std::vector<GUID> guids;
			std::optional<bool> is_interior;
			std::optional<bool> is_left_side_window;
			std::optional<bool> is_right_side_window;
			std::optional<bool> is_nascar_wiper;
			std::optional<bool> is_icense_plate;
			std::optional<uint8_t> id;
		};

		struct UpgradablePart
		{
			struct SharedCarModel
			{
				std::list<long> upgrade_ids;
				std::shared_ptr<CarRenderModel11> model;

			}; // CarScene::UpgradablePart::SharedCarModel

			struct Upgrade
			{
				uint16_t version;
				uint8_t level; // unsigned char; possible name: Level
				bool is_stock; // bool IsStock;
				int32_t id; // long; possible name: Id
				int32_t car_body_id; // long; possible name: CarBodyId
				bool parent_is_stock; // bool ParentIsStock;
				std::vector<std::shared_ptr<CarRenderModel11>> models;
				AABB bounds;

			}; // CarScene::UpgradablePart::Upgrade

			uint16_t version;
			CCarParts_Enum type;
			std::list<Upgrade> upgrade_models;
			std::list<SharedCarModel> shared_models;

		}; // CarScene::UpgradablePart

		struct Part
		{
			uint16_t version;
			CCarParts_Enum type;
			std::list<std::shared_ptr<CarRenderModel11>> models;
			AABB bounds;
		}; // CarScene::Part


		struct Scene
		{
			uint16_t version;
			GUID build_guid;
			uint32_t ordinal;
			uint32_t CarBodyID;
			std::string media_name;
			std::string skeleton_modelbin_path;
			bool was_strict_build;
			LODFlags levels_of_detail;
			std::vector<Part> non_upgradable_parts;
			std::vector<UpgradablePart> upgradable_parts;
		};

		static std::string PartsToString(CCarParts_Enum parts)
		{
			std::string result;

			switch (parts)
			{
			case CCarParts_Engine:
				result = "Engine";
				break;
			case CCarParts_Drivetrain:
				result = "Drivetrain";
				break;
			case CCarParts_CarBody:
				result = "CarBody";
				break;
			case CCarParts_Motor:
				result = "Motor";
				break;
			case CCarParts_Brakes:
				result = "Brakes";
				break;
			case CCarParts_SpringDamper:
				result = "SpringDamper";
				break;
			case CCarParts_AntiSwayFront:
				result = "AntiSwayFront";
				break;
			case CCarParts_AntiSwayRear:
				result = "AntiSwayRear";
				break;
			case CCarParts_TireCompound:
				result = "TireCompound";
				break;
			case CCarParts_RearWing:
				result = "RearWing";
				break;
			case CCarParts_RimSizeFront:
				result = "RimSizeFront";
				break;
			case CCarParts_RimSizeRear:
				result = "RimSizeRear";
				break;
			case CCarParts_Camshaft:
				result = "Camshaft";
				break;
			case CCarParts_Valves:
				result = "Valves";
				break;
			case CCarParts_Displacement:
				result = "Displacement";
				break;
			case CCarParts_PistonsCompression:
				result = "PistonsCompression";
				break;
			case CCarParts_FuelSystem:
				result = "FuelSystem";
				break;
			case CCarParts_Ignition:
				result = "Ignition";
				break;
			case CCarParts_Exhaust:
				result = "Exhaust";
				break;
			case CCarParts_Intake:
				result = "Intake";
				break;
			case CCarParts_Flywheel:
				result = "Flywheel";
				break;
			case CCarParts_Manifold:
				result = "Manifold";
				break;
			case CCarParts_RestrictorPlate:
				result = "RestrictorPlate";
				break;
			case CCarParts_OilCooling:
				result = "OilCooling";
				break;
			case CCarParts_SingleTurbo:
				result = "SingleTurbo";
				break;
			case CCarParts_TwinTurbo:
				result = "TwinTurbo";
				break;
			case CCarParts_QuadTurbo:
				result = "QuadTurbo";
				break;
			case CCarParts_SuperchargerCSC:
				result = "SuperchargerCSC";
				break;
			case CCarParts_SuperchargerDSC:
				result = "SuperchargerDSC";
				break;
			case CCarParts_Intercooler:
				result = "Intercooler";
				break;
			case CCarParts_Clutch:
				result = "Clutch";
				break;
			case CCarParts_Transmission:
				result = "Transmission";
				break;
			case CCarParts_Driveline:
				result = "Driveline";
				break;
			case CCarParts_Differential:
				result = "Differential";
				break;
			case CCarParts_FrontBumper:
				result = "FrontBumper";
				break;
			case CCarParts_RearBumper:
				result = "RearBumper";
				break;
			case CCarParts_Hood:
				result = "Hood";
				break;
			case CCarParts_SideSkirts:
				result = "SideSkirts";
				break;
			case CCarParts_TireWidthFront:
				result = "TireWidthFront";
				break;
			case CCarParts_TireWidthRear:
				result = "TireWidthRear";
				break;
			case CCarParts_WeightReduction:
				result = "WeightReduction";
				break;
			case CCarParts_ChassisStiffness:
				result = "ChassisStiffness";
				break;
			case CCarParts_Ballast:
				result = "Ballast";
				break;
			case CCarParts_MotorParts:
				result = "MotorParts";
				break;
			case CCarParts_WheelStyle:
				result = "WheelStyle";
				break;
			case CCarParts_Aspiration:
				result = "Aspiration";
				break;
			default:
				break;
			}

			return result;
		}

		Scene scene;
		Series series;

		bool Init()
		{
			if (m_stream->fail()) {
				return false;
			}

			// 1 initial
			// 2 add levels_of_detail
			// 3 add build_guid
			// 4 add Part wrapper in non_upgradable_parts
			// 5 add is_motorsport_series - FH2, FH3, FH4; FM5, FM6, FM7
			//   *Horizon fork
			//   6 add unk_v6 - FH5
			// 6 add Ballast to CCarParts::Enum above WheelStyle
			// 7 ?
			// 8 ?
			// 9 ?
			// 10 ? - FM2023
			// 11 ? - FM2023 v1.587.4035.0

			scene.version = this->Read<uint16_t>(); //<comment = "FM5/6/7: 5, FM2023: 10-11; FH2/3/4: 5, FH5: 6">; // unsigned short; possible name: m_Version, Version

			if (series == Auto && (scene.version == 10 || scene.version == 11)) { // heuristic
				series = Motorsport;
				series_is_weak = true;
			}

			uint16_t max_version = series == Horizon ? 7 : 11;

			if (scene.version > max_version) {
				printf("Warning: Unsupported CarScene::Scene version. Found: %d. Max supported: %d\n", scene.version, max_version);

				return false;
			}

			if (scene.version < 1) {
				series_is_weak = false; ("Warning: Unsupported CarScene::Scene version. Found: %d. Min supported: 1\n", scene.version);

				return false;
			}

			//GUID build_guid; // used to match .carbin_debug
			if (scene.version >= 3) {
				scene.build_guid = this->ReadGUID();
			}

			if (scene.version >= 5) {
				scene.was_strict_build = this->Read<uint8_t>(); // bool WasStrictBuild;
			}

			scene.ordinal = this->Read<uint32_t>(); // unsigned long; possible name: m_Ordinal, carId
			scene.CarBodyID = GenerateCarBodyID(scene.ordinal); // eq = List_UpgradeCarBody.CarBodyID
			scene.media_name = this->ReadStringByIntType<uint32_t>(); // std::string
			scene.skeleton_modelbin_path = this->ReadStringByIntType<uint32_t>(); // std::string

			//LODFlags levels_of_detail;
			if (scene.version >= 2) {
				scene.levels_of_detail = LODFlags(this->Read<uint16_t>()); // = 0x7E, if version < 2
			}

			uint32_t non_upgradable_parts_length = this->Read<uint32_t>();

			if (scene.version >= 4) {
				for (size_t i = 0; i < non_upgradable_parts_length; ++i) {
					if (series == Motorsport && scene.version >= 6) {
						//CCarParts_Enum type : 8; // unsigned char; possible name: m_PartEnum, partEnum, partId
						CCarParts_Enum type = (CCarParts_Enum)this->Read<uint8_t>();
					} 
					else {
						//CCarParts_EnumV1 type_v1 : 8; // unsigned char
						//local CCarParts_Enum type = CCarParts_EnumV1ToLatest(type_v1);
						uint8_t type_v1 = this->Read<uint8_t>();
						CCarParts_Enum type = CCarParts_EnumV1ToLatest(type_v1);
					}

					Part part;
					if (DeserializePart(&part)) {
						scene.non_upgradable_parts.push_back(part);
					}
				}
			} 
			else {
				for (size_t i = 0; i < non_upgradable_parts_length; ++i) {
					Part part;
					if (DeserializePart(&part)) {
						scene.non_upgradable_parts.push_back(part);
					}
				}
			}

			uint32_t upgradable_parts_length = this->Read<uint32_t>(); // unsigned long

			if (upgradable_parts_length > 0) {
				for (size_t i = 0; i < upgradable_parts_length; ++i) {
					UpgradablePart upgradablePart;
					if (DeserializeUpgradablePart(&upgradablePart)) {
						scene.upgradable_parts.push_back(upgradablePart);
					}
				}
			}

			if (series == Horizon && scene.version >= 6) {
				bool unk_v6 = this->Read<uint8_t>(); // bool; = true, if version < 6
			}

			return true;
		}

	private:
		std::filesystem::path m_fileName;
		std::unique_ptr<mbuff> m_buffer = nullptr;
		std::unique_ptr<std::istream> m_stream = nullptr;

		bool series_is_weak = false;

		uint32_t GenerateCarBodyID(uint32_t Id)
		{
			uint32_t result = 1000;
			result = (Id * result);

			return result;
		}

		CCarParts_Enum CCarParts_EnumV1ToLatest(uint32_t value) {
			if (value >= 42) {
				value += 1;
			}
			return (CCarParts_Enum)value;
		}

		template <typename T>
		T Read() {
			T result;
			m_stream->read(reinterpret_cast<char*>(&result), sizeof(T));

			return result;
		}

		GUID ReadGUID() {
			GUID guid;
			m_stream->read(reinterpret_cast<char*>(&guid), sizeof(GUID));

			return guid;
		}

		template <typename T>
		std::string ReadStringByIntType() {
			T size;
			m_stream->read(reinterpret_cast<char*>(&size), sizeof(T));

			std::string result;
			result.resize(size);

			m_stream->read(reinterpret_cast<char*>(result.data()), size);

			return result;
		}

		bool DeserializeAOMapInfo(AOMapInfo* aoMapInfo) {

			// 1 initial
			// 2 remove unk_v1, add guid_v2
			// 3 add unk_v3 - FH2, FH3, FH4, FH5; FM5, FM6, FM7, FM2023
			aoMapInfo->version = Read<uint16_t>(); // <comment = "FM5/6/7/2023: 3; FH2/3/4/5: 3">; // unsigned short

			if (aoMapInfo->version > 3) {
				printf("Warning: Unsupported CarScene::ICarRenderModel::AOMapInfo version. Found: %d. Max supported: 3\n", aoMapInfo->version);

				return false;
			}

			if (aoMapInfo->version < 1) {
				printf("Warning: Unsupported CarScene::ICarRenderModel::AOMapInfo version. Found: %d. Min supported: 1\n", aoMapInfo->version);

				return false;
			}

			aoMapInfo->path = ReadStringByIntType<uint32_t>(); // std::string
			uint32_t unk0 = Read<uint32_t>(); // <format = hex>; // unsigned long
			int32_t unk1 = Read<int32_t>(); // long

			if (aoMapInfo->version >= 2) {
				aoMapInfo->guid = Read<GUID>(); // CGUID
			} 
			else {
				int16_t unk_v01 = Read<int16_t>(); // short; unused
				bool unk_v001 = Read<uint8_t>(); // bool; unused
			}

			bool unk = Read<uint8_t>(); // bool

			if (aoMapInfo->version >= 3) {
				char unk_v30 = Read<char>(); // char
				char unk_v31 = Read<char>(); // char
			}

			return true;

		} // struct CarScene::ICarRenderModel::AOMapInfo

		bool DeserializeCarRenderModel11(CarRenderModel11* carRenderModel) {
			// 1 initial
			// 2 add material_overrides
			// 3 add material_indexes
			// 4 ?
			// 5 change levels_of_detail type from uint to ushort
			// 6 add droppable
			// 7 ?
			// 8 add break_amount
			// 9 remove ao_swatchbin_path, add ao_map_info
			// 10 add is_interior_windshield
			// 11 unk_v11
			// 12 add std::string type field
			// 13 guid_v13
			// 14 add guid_v14, unk_v14 - FM5
			//   *Horizon fork
			//   15 add unk_v15 - FH2
			//   16 add guids - FH3, FH4
			//   17 add id
			//   18 add unk_v18, (add DriverlessCockpit, WindshieldReflectionDriverlessCockpit and ProxyLOD in CarScene::DrawGroup::Enum (may be v17)) - FH5
			// 15 add guids
			// 16 add unk_v16
			// 17 add unk_v17, (add DriverlessCockpit and WindshieldReflectionDriverlessCockpit in CarScene::DrawGroup::Enum (v15-v16) (FM7 only?)) - FM6, FM7
			// 18 add unk_v18
			// 19 add unk_v19
			// 20 add is_interior, ..., is_license_plate booleans
			// 21 change material_indexes value type from int to unsigned __int64 - FM2023

			carRenderModel->version = this->Read<uint16_t>(); //<comment = "FM5: 14, FM6/7: 17, FM2023: 21; FH2: 15, FH3/4: 16, FH5: 18">; // unsigned short
			if (series == Auto || series_is_weak) { // heuristic
				bool known = false;
				if (carRenderModel->version == 18) {
					series = Horizon;
					if (scene.version == 6) {
						known = true;
					}
				} else if (carRenderModel->version == 15 || carRenderModel->version == 16) {
					series = Horizon;
					if (scene.version == 5) {
						known = true;
					}
				} else if (carRenderModel->version == 21 && scene.version == 7) {
					series = Horizon;
					known = true;
				}
				else {
					series = Motorsport;
					if (carRenderModel->version == 21) {
						if (scene.version == 10 || scene.version == 11) {
							known = true;
						}
					} else if ((carRenderModel->version == 14 || carRenderModel->version == 17) && scene.version == 5) {
						known = true;
					}
				}

				if (!known) {
					printf("Warning: Unknown CarScene::Scene (v%d) and CarScene::CarRenderModel11 (v%d) version combination. \n", scene.version, carRenderModel->version);
				}

				/*
				if (series == Horizon) {
					printf("Assumed game series: Forza Horizon \n");
				} 
				else {
					printf("Assumed game series: Forza Motorsport \n");
				}
				*/
				series_is_weak = false;
			}

			uint16_t max_version = series == Horizon ? 21 : 21;

			if (carRenderModel->version > max_version) {
				printf("Warning: Unsupported CarScene::CarRenderModel11 version. Found: %d. Max supported: %d\n", carRenderModel->version, max_version);

				return false;
			}

			if (carRenderModel->version < 1) {
				printf("Warning: Unsupported CarScene::CarRenderModel11 version. Found: %d. Min supported: 1\n", carRenderModel->version);

				return false;
			}

			carRenderModel->path = this->ReadStringByIntType<uint32_t>(); // std::string
			carRenderModel->transform = this->Read<DirectX::XMMATRIX>(); // DirectX::XMMATRIX

			if (carRenderModel->version >= 5) {
				carRenderModel->levels_of_detail = LODFlags(this->Read<uint16_t>()); // unsigned short
			} 
			else {
				carRenderModel->levels_of_detail = LODFlags(this->Read<uint32_t>()); // unsigned long
			}

			carRenderModel->bone_name = this->ReadStringByIntType<uint32_t>(); // std::string
			carRenderModel->bone_id = this->Read<uint16_t>(); // short; refers to _skeleton.modelbin (not .gr2)
			bool unk = this->Read<uint8_t>(); // bool

			carRenderModel->draw_groups = DrawGroups(this->Read<uint32_t>()); // long DrawGroups; FH3: & 0x1F, FH5: & 0xFF; FM2023: & 0x7F

			if (carRenderModel->version < 9) {
				carRenderModel->ao_swatchbin_path = this->ReadStringByIntType<uint32_t>(); // std::string; was removed later on
			}

			// std::map<std::string, std::vector<unsigned char>> material_overrides
			if (carRenderModel->version >= 2) {
				uint32_t material_overrides_length = this->Read<uint32_t>(); // unsigned long
				if (material_overrides_length > 0) {
					for (size_t i = 0; i < material_overrides_length; ++i) {
						std::string key = this->ReadStringByIntType<uint32_t>();
						uint32_t value_length = this->Read<uint32_t>(); // unsigned long

						if (value_length > 0) {
							std::vector<unsigned char> value_data(value_length); // std::vector<unsigned char>; binary Grub Bundle
							m_stream->read(reinterpret_cast<char*>(value_data.data()), value_length);

							carRenderModel->material_overrides.emplace(key, value_data);
						}
					}
				}
			}

			// std::map<std::string, int> material_indexes
			if (carRenderModel->version >= 3) {
				uint32_t material_indexes_length = this->Read<uint32_t>(); // unsinged long

				for (size_t i = 0; i < material_indexes_length; ++i) {
					std::string key = this->ReadStringByIntType<uint32_t>(); // std::string
					int value;
					if (series == Motorsport && carRenderModel->version >= 21) {
						value = static_cast<int32_t>(this->Read<uint64_t>()); // unsigned __int64
					} else if (series == Horizon && carRenderModel->version >= 21) {
						value = static_cast<int32_t>(this->Read<uint64_t>()); // unsigned __int64
					}
					else {
						value = this->Read<int32_t>(); // int
					}

					carRenderModel->material_indexes.emplace(key, value);

				}

			}

			if (carRenderModel->version >= 6) {
				carRenderModel->is_droppable = this->Read<uint8_t>(); // bool Droppable;
				if (carRenderModel->is_droppable) {
					// struct CarScene::CarRenderModel11::DropPartData
					carRenderModel->drop_value = this->Read<float>(); // float DropValue;
					carRenderModel->drop_part_id = this->Read<uint32_t>(); // <format = hex>; // unsigned long DropPartID; FH2023: long; hash?
				}
			}

			if (carRenderModel->version >= 8) {
				carRenderModel->break_amount = this->Read<float>(); // float BreakAmount;
			}

			// std::vector<CarScene::ICarRenderModel::AOMapInfo> m_AOMapInfo;
			if (carRenderModel->version >= 9) {
				uint32_t ao_map_info_length = this->Read<uint32_t>(); // unsigned long count;

				for (size_t i = 0; i < ao_map_info_length; ++i) {
					AOMapInfo aoMapInfo;
					DeserializeAOMapInfo(&aoMapInfo);

					carRenderModel->AOMapInfo.push_back(aoMapInfo);
				}
			}

			if (carRenderModel->version >= 10) {
				carRenderModel->is_interior_windshield = this->Read<uint8_t>(); // bool IsInteriorWindshield;
			}

			if (carRenderModel->version >= 11) {
				bool unk_v11_0 = this->Read<uint8_t>(); // bool
				bool unk_v11_1 = this->Read<uint8_t>(); // bool
				bool unk_v11_2 = this->Read<uint32_t>(); // unsigned long
				bool unk_v11_3 = this->Read<uint32_t>(); // unsigned long
				bool unk_v11_4 = this->Read<uint32_t>(); // unsigned long
				bool unk_v11_5 = this->Read<uint32_t>(); // unsigned long
			}

			if (carRenderModel->version >= 12) {
				carRenderModel->type = this->ReadStringByIntType<uint32_t>(); // std::string
			}

			if (carRenderModel->version >= 13) {
				carRenderModel->guid_v13 = this->Read<GUID>(); // CGUID
			}

			if (carRenderModel->version >= 14) {
				carRenderModel->guid_v14 = this->Read<GUID>(); // CGUID
				uint32_t unk_v14 = this->Read<uint32_t>(); // unsigned long
			}

			if (series == Horizon && carRenderModel->version < 21) {
				int32_t unk_v15 = this->Read<int32_t>(); // long; = unk_v11[3], if version < 15
			}

			if (series == Horizon && carRenderModel->version >= 21) {
				int unk_val0 = this->Read<uint32_t>();
			}

			// std::vector<CGUID>; reference to other models?
			if (series == Motorsport && carRenderModel->version >= 15 || series == Horizon && carRenderModel->version >= 16) {
				uint32_t guids_length = this->Read<int32_t>(); // unsigned long
				if (guids_length > 0) {
					carRenderModel->guids.reserve(guids_length);
					for (size_t i = 0; i < guids_length; ++i) {
						GUID guid = this->Read<GUID>();
						carRenderModel->guids.emplace_back(guid);
					}
				}
			}			

			if (series == Motorsport) {
				if (carRenderModel->version >= 16) {
					uint32_t unk_v16 = this->Read<uint32_t>(); // unsigned long
				}

				if (carRenderModel->version >= 17) {
					uint8_t unk_v17 = this->Read<uint8_t>(); // unsigned char
				}

				if (carRenderModel->version >= 18) {
					std::string unk_v18 = this->ReadStringByIntType<uint32_t>(); // std::string
				}

				if (carRenderModel->version >= 19) {
					std::string unk_v19 = this->ReadStringByIntType<uint32_t>(); // std::string
				}

				if (carRenderModel->version >= 20) {
					carRenderModel->is_interior = this->Read<uint8_t>(); // bool IsInterior;
					carRenderModel->is_left_side_window = this->Read<uint32_t>(); // long IsLeftSideWindow;
					carRenderModel->is_right_side_window = this->Read<uint32_t>(); // long IsRightSideWindow;
					carRenderModel->is_nascar_wiper = this->Read<uint8_t>(); // bool IsNascarWiper;
					carRenderModel->is_icense_plate = this->Read<uint8_t>(); // bool IsLicensePlate;
				}
			}
			else if (series == Horizon && carRenderModel->version >= 21) {
				if (carRenderModel->version >= 17) {
					uint8_t unk_v17 = this->Read<uint8_t>(); // unsigned char
				}

				int unk_v16 = this->Read<uint32_t>();
				if (unk_v16 > 0) {
					int unk_v18_1 = this->Read<uint32_t>();
					std::string unk_v19 = this->ReadStringByIntType<uint32_t>(); // std::string
				}
			}
			else if (series == Horizon && carRenderModel->version < 21) {
				if (carRenderModel->version >= 17) {
					carRenderModel->id = this->Read<uint8_t>(); // unsigned char; model id for ordering? unique for all models in file, ascending for models in non_upgradable_parts
					//Printf("%s %d\n", type.data, id);
				}

				if (carRenderModel->version >= 18) {
					bool unk_v18 = this->Read<uint32_t>(); // unsigned long; = true, if version < 18
				}
			}

			return true;
		}

		bool DeserializeUpgrade(UpgradablePart::Upgrade* upgradePart) {
			// 1 initial
			// 2 add bounds
			// 3 remove models - FH2, FH3, FH4, FH5; FM5, FM6, FM7
			//   *Horizon fork
			// 4 ? - FM2023

			upgradePart->version = this->Read<uint16_t>(); // <comment = "FM5/6/7: 3, FM2023: 4; FH2/3/4/5: 3">; // unsigned short; always equal to CarScene::UpgradablePart?
			uint16_t max_version = series == Horizon ? 4 : 4;

			if (upgradePart->version > max_version) {
				printf("Warning: Unsupported CarScene::UpgradablePart::Upgrade version. Found: %d. Max supported: %d\n", upgradePart->version, max_version);

				return false;
			}

			if (upgradePart->version < 1) {
				printf("Warning: Unsupported CarScene::UpgradablePart::Upgrade version. Found: %d. Min supported: 1\n", upgradePart->version);

				return false;
			}

			upgradePart->level = this->Read<uint8_t>(); // unsigned char; possible name: Level
			upgradePart->is_stock = this->Read<uint8_t>(); // bool IsStock;
			upgradePart->id = this->Read<int32_t>(); // long; possible name: Id
			upgradePart->car_body_id = this->Read<int32_t>(); // long; possible name: CarBodyId
			upgradePart->parent_is_stock = this->Read<uint8_t>(); // bool ParentIsStock;

			if (upgradePart->version < 3) {
				uint32_t models_length = this->Read<uint32_t>(); // unsigned long
				//CarRenderModel11 models[models_length];

				for (size_t i = 0; i < models_length; ++i) {
					CarRenderModel11 carRenderModel;
					DeserializeCarRenderModel11(&carRenderModel);

					upgradePart->models.push_back(std::make_shared<CarRenderModel11>(carRenderModel));
				}
			}

			if (upgradePart->version >= 2) {
				upgradePart->bounds = this->Read<AABB>();
			}

			return true;

		} // CarScene::UpgradablePart::Upgrade

		void DeserializeSharedCarModel(UpgradablePart::SharedCarModel* sharedModel) {
			uint32_t upgrade_ids_length = this->Read<uint32_t>(); // unsigned long

			// std::list<long> upgrade_ids;
			for (size_t i = 0; i < upgrade_ids_length; ++i) {
				sharedModel->upgrade_ids.push_back(this->Read<int32_t>());
			}

			CarRenderModel11 model;
			DeserializeCarRenderModel11(&model);

			sharedModel->model = std::make_shared<CarRenderModel11>(model);

		}; // CarScene::UpgradablePart::SharedCarModel


		bool DeserializeUpgradablePart(UpgradablePart* part) {

			// 1 initial
			// 2 add bounds to CarScene::UpgradablePart::Upgrade?
			// 3 replace CarRenderModel11 with SharedCarModels - FM5, FM6, FM7; FH2, FH3, FH4, FH5
			//   *Horizon fork
			// 4 add Ballast to CarParts::Enum - FM2023

			part->version = this->Read<uint16_t>(); // <comment = "FM5/6/7: 3, FM2023: 4; FH2/3/4/5: 3">; // unsigned short
			uint16_t max_version = series == Horizon ? 4 : 4;

			if (part->version > max_version) {
				printf("Warning: Unsupported CarScene::UpgradablePart version. Found: %d. Max supported: %d\n", part->version, max_version);

				return false;
			}

			if (part->version < 1) {
				printf("Warning: Unsupported CarScene::UpgradablePart version. Found: %d. Min supported: 1\n", part->version);

				return false;
			}

			if (series == Motorsport && part->version >= 4) {
				//CCarParts_Enum type : 32; // unsigned long
				part->type = (CCarParts_Enum)this->Read<uint32_t>();
			} 
			else {
				//CCarParts_EnumV1 type_v1 : 32; // unsigned long
				uint32_t type_v1 = (CCarParts_Enum)this->Read<uint32_t>();
				part->type = CCarParts_EnumV1ToLatest(type_v1);
			}

			uint32_t upgrades_length = this->Read<uint32_t>(); // unsigned long

			// std::list<CarScene::UpgradablePart::Upgrade> upgrade_models

			for (size_t i = 0; i < upgrades_length; ++i) {
				UpgradablePart::Upgrade upgrade;
				DeserializeUpgrade(&upgrade);

				part->upgrade_models.push_back(upgrade);
			}

			// std::list<CarScene::UpgradablePart::SharedCarModel> shared_models
			if (part->version >= 3) {
				uint32_t shared_models_length = this->Read<uint32_t>(); // unsigned long

				for (size_t i = 0; i < shared_models_length; ++i) {
					UpgradablePart::SharedCarModel sharedModel;
					DeserializeSharedCarModel(&sharedModel);

					part->shared_models.push_back(sharedModel);
				}
			}

			return true;

		}  //CarScene::UpgradablePart

		bool DeserializePart(Part* part) {
			// 1 initial
			// 2 add bounds - FH2, FH3, FH4, FH5; FM5, FM6, FM7
			//   *Horizon fork
			// 3 add Ballast to CarParts::Enum - FM2023

			part->version = this->Read<uint16_t>(); //<comment = "FM5/6/7: 2, FM2023: 3; FH2/3/4/5: 2">; // unsigned short
			uint16_t max_version = series == Horizon ? 3 : 3;

			if (part->version > max_version) {
				printf("Warning: Unsupported CarScene::Part version. Found: %d. Max supported: %d\n", part->version, max_version);

				return false;
			}

			if (part->version < 1) {
				printf("Warning: Unsupported CarScene::Part version. Found: %d. Min supported: 1\n", part->version);

				return false;
			}

			if (series == Motorsport && part->version >= 3) {
				//CCarParts_Enum type : 32; // unsigned long
				part->type = (CCarParts_Enum)this->Read<uint32_t>();
			} 
			else {
				//CCarParts_EnumV1 type : 32; // unsigned long
				uint32_t type_v1 = (CCarParts_Enum)this->Read<uint32_t>();
				part->type = CCarParts_EnumV1ToLatest(type_v1);
			}

			uint32_t models_length = this->Read<uint32_t>();

			// std::list<std::shared_ptr<CarScene::CarRenderModel11>> m_Models; ContainedModels

			for (size_t i = 0; i < models_length; ++i) {
				CarRenderModel11 carRenderModel;
				DeserializeCarRenderModel11(&carRenderModel);

				part->models.push_back(std::make_shared<CarRenderModel11>(carRenderModel));
			}

			if (part->version >= 2) {
				part->bounds = this->Read<AABB>();
			}

			return true;

		}  // CarScene::Part
	};

}
