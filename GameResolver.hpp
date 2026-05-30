#pragma once
#include <string>
#include <array>
#include <regex>
#include <filesystem>

namespace fmnext
{
    class GameResolver
    {
    public:
        GameResolver() = default;
        ~GameResolver() = default;

        GameResolver(const std::string& path) : m_asset(path)
        {
            std::smatch match_result;
            std::regex_search(path, match_result, std::regex("media", std::regex_constants::icase));

            auto media = std::find(m_asset.begin(), m_asset.end(), match_result.str());

            if (media != std::end(m_asset))
            {
                for (auto it = std::begin(m_asset); it != media; ++it)
                {
                    m_base /= *it;
                }

                // Initialize base series
                for (auto it = std::next(media, 1); it != std::end(m_asset); ++it)
                {
                    if (*it == std::filesystem::path("pcfamily"))
                    {
                        m_series = 1;
                        m_media = std::next(it, 2)->stem().string();

                        break;
                    }
                    else
                    {
                        m_series = 0;
                        m_media = std::next(it, 1)->stem().string();

                        break;
                    }
                }

                if (m_series == 0)
                {
                    std::filesystem::path db(m_base);
                    db /= "media\\db\\gamedb.slt";

                    if (std::filesystem::exists(db) && !std::filesystem::exists(GetDatabase()))
                    {
                        m_series = 1;
                        gen2 = false;
                    }
                }
            }

            std::filesystem::path file_name = std::filesystem::path(path).filename();
            std::filesystem::path file_ext = file_name.extension();

            if (file_ext == ".zip")
            {
                m_buffered = true;
            }

            if (file_ext == ".carbin")
            {
                m_buffered = false;
            }
        }

        std::filesystem::path GetBase()
        {
            return m_base;
        }

        int GetSeries()
        {
            return m_series;
        }

        std::string GetSeriesName()
        {
            std::string result;

            switch (m_series)
            {
            case 0:
                result = "Horizon";
                break;
            case 1:
                result = "Motorsport";
                break;
            default:
                result = "Unknown";
                break;
            }

            return result;
        }

        std::string GetMediaName()
        {
            return m_media;
        }

        bool isBuffered()
        {
            return m_buffered;
        }

        static bool Contains(const std::filesystem::path& path, const std::string& media_name)
        {
            for (auto it = path.begin(); it != path.end(); ++it)
            {
                std::cmatch m;
                std::regex_match(it->string().c_str(), m, std::regex(media_name, std::regex_constants::icase));

                if (!m.empty())
                {
                    return true;
                }
            }

            return false;
        }

        static std::filesystem::path Remove(const std::filesystem::path& model_path, const std::string& model_name)
        {
            std::filesystem::path result;

            std::smatch match_result;
            std::string model_path_str(model_path.string());

            std::regex_search(model_path_str, match_result, std::regex(model_name, std::regex_constants::icase));

            for (auto it = std::next(std::find(model_path.begin(), model_path.end(), match_result.str()), 1); it != std::end(model_path); ++it)
            {
                result /= *it;
            }

            result.make_preferred();

            return result;
        }

        static std::string RemoveBase(const std::filesystem::path& model_path, const std::string& model_name)
        {
            std::filesystem::path result{};

            std::smatch match_result;
            std::string model_path_str(model_path.string());

            std::regex_search(model_path_str, match_result, std::regex(model_name, std::regex_constants::icase));

            for (auto it = std::next(std::find(model_path.begin(), model_path.end(), match_result.str()), 1); it != std::end(model_path); ++it)
            {
                result /= *it;
            }

            result.make_preferred();

            std::string final_result = result.string();
            std::replace(final_result.begin(), final_result.end(), '\\', '/');

            return final_result;
        }

        std::unordered_map<std::string, std::filesystem::path> GetThumbnail(const std::filesystem::path& input_path)
        {
            std::unordered_map<std::string, std::filesystem::path> result{};
            std::filesystem::path path(m_base);

            std::smatch match_result;
            std::string model_path_str(input_path.string());

            std::regex_search(model_path_str, match_result, std::regex("media", std::regex_constants::icase));

            switch (m_series)
            {
            case 0:
            {
                path /= std::filesystem::path("media");

                for (auto it = std::next(std::find(input_path.begin(), input_path.end(), match_result.str()), 1); it != std::end(input_path); ++it)
                {
                    path /= *it;
                }

                path.make_preferred();

                result.try_emplace(input_path.filename().replace_extension().string() + "_Big.swatchbin", std::filesystem::path(path.parent_path().string() + "HiRes.zip"));
                result.try_emplace(input_path.filename().replace_extension().string() + "_Small.swatchbin", path.parent_path().replace_extension(".zip"));

                break;
            }
            case 1:
            {
                if (gen2)
                {
                    path /= std::filesystem::path("media\\pcfamily"); // Motorsport V2
                }
                else {
                    path /= std::filesystem::path("media"); // Motorsport V1
                }
                
                for (auto it = std::next(std::find(input_path.begin(), input_path.end(), match_result.str()), 1); it != std::end(input_path); ++it)
                {
                    path /= *it;
                }

                path.make_preferred();

                result.try_emplace(input_path.filename().replace_extension().string() + "_big.swatchbin", path.parent_path().replace_extension(".zip"));
                result.try_emplace(input_path.filename().replace_extension().string() + "_small.swatchbin", path.parent_path().replace_extension(".zip"));

                break;
            }
            }

            return result;
        }

        std::filesystem::path GetDatabase()
        {
            std::filesystem::path result(m_base);

            switch (m_series)
            {
            case 0:
            {
                result /= std::filesystem::path("media\\stripped\\gamedbRC.slt");
                break;
            }
            case 1:
            {
                if (gen2)
                {
                    result /= std::filesystem::path("media\\base\\db\\gamedb.slt"); // Motorsport V2
                }
                else {
                    result /= std::filesystem::path("media\\db\\gamedb.slt"); // Motorsport V1
                }
                break;
            }
            }

            result.make_preferred();

            return result;
        }

        static std::filesystem::path Remove(const std::filesystem::path& item_path)
        {
            std::filesystem::path result;
            std::array<std::string, 2> types{ "textures", "materials" };

            std::smatch match_result;
            std::string model_path_str(item_path.string());

            for (auto& type : types)
            {
                if (std::regex_search(model_path_str, match_result, std::regex(type, std::regex_constants::icase)))
                    break;
            }

            for (auto it = std::next(std::find(item_path.begin(), item_path.end(), match_result.str()), 1); it != std::end(item_path); ++it)
            {
                result /= *it;
            }

            return result;
        }

        std::filesystem::path GetPrimaryMaterialsLibrary()
        {
            std::filesystem::path result(m_base);
            switch (m_series)
            {
            case 0:
                result /= std::filesystem::path("media\\cars\\_library\\Materials.zip");
                break;
            case 1:
                if (gen2)
                {
                    result /= std::filesystem::path("media\\base\\cars\\_library\\materials.zip"); // Motorsport V2
                } 
                else {
                    result /= std::filesystem::path("media\\cars\\_library\\materials.zip"); // Motorsport V1
                }
                break;
            }

            result.make_preferred();

            return result;
        }

        std::filesystem::path GetPrimaryTexturesLibrary()
        {
            std::filesystem::path result(m_base);
            switch (m_series)
            {
            case 0:
                result /= std::filesystem::path("media\\cars\\_library\\Textures.zip");
                break;
            case 1:
                if (gen2)
                {
                    result /= std::filesystem::path("media\\pcfamily\\cars\\_library\\textures.zip"); // Motorsport V2
                } 
                else {
                    result /= std::filesystem::path("media\\cars\\_library\\textures.zip"); // Motorsport V1
                }
                break;
            }

            result.make_preferred();

            return result;
        }

        std::filesystem::path GetMediaMaterialsLibrary()
        {
            std::filesystem::path result(m_base);
            switch (m_series)
            {
            case 0:
                result /= std::filesystem::path("media\\_library\\Materials.zip");
                break;
            case 1:
            {
                if (gen2)
                {
                    result /= std::filesystem::path("media\\base\\_library\\materials.zip"); // Motorsport V2
                }
                else {
                    result /= std::filesystem::path("media\\_library\\materials.zip"); // Motorsport V1
                }
                break;
            }
            }

            result.make_preferred();

            return result;
        }

        std::filesystem::path GetMediaTexturesLibrary()
        {
            std::filesystem::path result(m_base);
            switch (m_series)
            {
            case 0:
                result /= std::filesystem::path("media\\_library\\Textures.zip");
                break;
            case 1:
            {
                if (gen2)
                {
                    result /= std::filesystem::path("media\\pcfamily\\_library\\textures.zip"); // Motorsport V2
                }
                else {
                    result /= std::filesystem::path("media\\_library\\textures.zip"); // Motorsport V1
                }
                break;
            }
            }

            result.make_preferred();

            return result;
        }

        std::vector<std::filesystem::path> GetSecondaryMaterialsLibrary()
        {
            std::vector<std::filesystem::path> result;

            std::array<std::string, 15> material_entries{
                "materials_pri_32.zip",
                "materials_pri_33.zip",
                "materials_pri_34.zip",
                "materials_pri_35.zip",
                "materials_pri_36.zip",
                "materials_pri_37.zip",
                "materials_pri_38.zip",
                "materials_pri_39.zip",
                "materials_pri_40.zip",
                "materials_pri_41.zip",
                "materials_pri_42.zip",
                "materials_pri_43.zip",
                "materials_pri_44.zip",
                "materials_pri_45.zip",
                "materials_pri_301.zip"
            };

            std::filesystem::path library_base(m_base);
            library_base /= "media\\cars\\_library";

            for (auto& material_entry : material_entries)
            {
                std::filesystem::path library(library_base);
                library /= material_entry;

                library.make_preferred();

                if (std::filesystem::exists(library))
                {
                    result.push_back(library);
                }
            }

            return result;
        }

        std::vector<std::filesystem::path> GetSecondaryTexturesLibrary()
        {
            std::vector<std::filesystem::path> result;

            std::array<std::string, 15> texture_entries{
                "textures_pri_32.zip",
                "textures_pri_33.zip",
                "textures_pri_34.zip",
                "textures_pri_35.zip",
                "textures_pri_36.zip",
                "textures_pri_37.zip",
                "textures_pri_38.zip",
                "textures_pri_39.zip",
                "textures_pri_40.zip",
                "textures_pri_41.zip",
                "textures_pri_42.zip",
                "textures_pri_43.zip",
                "textures_pri_44.zip",
                "textures_pri_45.zip",
                "textures_pri_301.zip"
            };

            std::filesystem::path library_base(m_base);
            library_base /= "media\\cars\\_library";

            for (auto& texture_entry : texture_entries)
            {
                std::filesystem::path library(library_base);
                library /= texture_entry;

                library.make_preferred();

                if (std::filesystem::exists(library))
                {
                    result.push_back(library);
                }
            }

            return result;
        }

        std::filesystem::path GetSharedTires(const std::string& name)
        {
            std::filesystem::path result(m_base);

            switch (m_series)
            {
            case 0:
            {
                result /= std::filesystem::path("media\\cars\\_library\\scene\\tires");
                result /= std::filesystem::path("tire_" + name).replace_extension(".zip");
                break;
            }
            case 1:
            {
                if (gen2)
                {
                    result /= std::filesystem::path("media\\base\\cars\\_library\\scene\\tires");
                    result /= std::filesystem::path("tire_" + name).replace_extension(".zip");
                }
                else {
                    result /= std::filesystem::path("media\\cars\\_library\\scene\\tires");
                    result /= std::filesystem::path("tire_" + name).replace_extension(".zip");
                }
                break;
            }
            }

            result.make_preferred();

            return result;
        }


    private:
        bool m_buffered = false;
        int m_series = -1;
        std::string m_media{};
        std::filesystem::path m_asset{};
        std::filesystem::path m_base{};
        bool gen2 = true;
    };

}
