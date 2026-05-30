#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <regex>
#include "BundleReader.hpp"
#include "SceneReader.hpp"

namespace fmnext
{
    struct DataBaseRecords
    {
        std::string MediaName;
        int CarId;
        int CarBodyID;
        std::string TireModelName;

        bool fallback;

        int FrontTireWidthMM;
        int FrontTireAspect;
        int FrontWheelDiameterIN;
        int RearTireWidthMM;
        int RearTireAspect;
        int RearWheelDiameterIN;

        std::string Thumbnail;

        float ModelWheelbase;
        float ModelFrontTrackOuter;
        float ModelRearTrackOuter;
        float ModelFrontStockRideHeight;
        float ModelRearStockRideHeight;

        float BottomCenterWheelbasePosX;
        float BottomCenterWheelbasePosY;
        float BottomCenterWheelbasePosZ;
    };

    enum GeometryType
    {
        TRIANGLES = 0,
        QUADS = 1
    };

    struct PartLocation
    {
        enum Side
        {
            LEFT,
            RIGHT
        };

        enum End
        {
            FRONT,
            REAR,
            MID
        };

        End end;
        Side side;

        PartLocation() = default;

        PartLocation(End end, Side side) : end(end), side(side) {}
    };

    struct Mesh
    {
        Mesh() = default;
        Mesh(std::string name, DirectX::XMMATRIX matrix, std::vector<uint32_t> indices, std::vector<DirectX::XMFLOAT3> vertices, std::vector<DirectX::XMFLOAT3> normals, std::vector<std::vector<DirectX::XMFLOAT2>> uvs, int16_t material_index) :
            name(name), matrix(matrix), indices(indices), vertices(vertices), normals(normals), uvs(uvs), material_index(material_index) {
        }

        //Mesh(const Mesh& other) : name(other.name), matrix(other.matrix), indices(other.indices), vertices(other.vertices), normals(other.normals), uvs(other.uvs), material_index(other.material_index)
        //{ let compiler declares }

        //Mesh(Mesh&& other) noexcept : name(other.name), matrix(other.matrix), indices(other.indices), vertices(other.vertices), normals(other.normals), uvs(other.uvs), material_index(other.material_index)
        //{ let compiler declares }

        //Mesh& operator=(const Mesh& other) = default;
        //Mesh& operator=(Mesh&& other) noexcept = default;

        std::string name{};
        DirectX::XMMATRIX matrix{};
        std::vector<uint32_t> indices{};
        std::vector<DirectX::XMFLOAT3> vertices{};
        std::vector<DirectX::XMFLOAT3> normals{};
        std::vector<std::vector<DirectX::XMFLOAT2>> uvs{};
        int16_t material_index{};
    };

}