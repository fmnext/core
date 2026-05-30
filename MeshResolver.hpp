#pragma once
#include "preinclude.hpp"
#include "DirectXMesh.h"

namespace fmnext
{
    class MeshResolver
    {
    public:
        MeshResolver() = default;
        ~MeshResolver() = default;

        /// <summary>
        /// Default Bundle mesh processor with relative indices and quad/triangles support.
        /// </summary>
        /// <param name="bundle"></param>
        /// <param name="requested_lod"></param>
        /// <param name="geometry_type"></param>
        /// <param name="p_records"></param>
        /// <param name="type"></param>
        /// <param name="direction"></param>
        MeshResolver(std::shared_ptr<BundleReader::BundleData> bundle, int32_t requested_lod, GeometryType geometry_type, const std::shared_ptr<DataBaseRecords>& p_records = nullptr, uint32_t type = 0, int8_t direction = -1) : m_bundle(bundle), m_requested_lod(requested_lod), m_geometry_type(geometry_type), m_records(p_records), type(type), direction(direction)
        {
            //uint8_t requested_lod = 0; // LODS, LOD0, LOD1, ...
            int requested_render_pass = 0xFFFF; // empty: 1 << 1, max : 1 << 5
            bool use_normals = true;
            bool hide_decal_transparent_pass = false;

            for (auto& entry : m_bundle->Meshes)
            {
                if (std::bitset<8>(entry.mesh.levels_of_detail)[m_requested_lod] == 0)
                {
                    continue;
                }

                if ((entry.mesh.render_pass bitand 0x10) == 0) // skip shadow
                {
                    continue;
                }

                if ((entry.mesh.render_pass bitand requested_render_pass) == 0)
                {
                    continue;
                }

                if (hide_decal_transparent_pass && (entry.mesh.render_pass bitand 0x4) != 0) // skip DecalTransparent
                {
                    continue;
                }

                if (type == 44 && (entry.mesh.render_pass bitand 0x4) != 0) // skip blur rim
                {
                    continue;
                }

                DirectX::XMMATRIX matrix{};

                for (auto bone = std::begin(m_bundle->Skeleton); bone != std::end(m_bundle->Skeleton); ++bone)
                {
                    if (std::distance(std::begin(m_bundle->Skeleton), bone) == entry.mesh.bone_index)
                    {
                        matrix = bone->transform;

                        if (bone->parent_index != -1)
                        {
                            auto current = std::next(std::begin(m_bundle->Skeleton), bone->parent_index);

                            while (current != std::end(m_bundle->Skeleton) && current->parent_index != -1)
                            {
                                matrix *= current->transform;
                                current = std::next(std::begin(m_bundle->Skeleton), current->parent_index);
                            }
                        }

                        continue;
                    }
                }

                // Externals
                std::vector<uint32_t> indices{};
                std::vector<uint32_t> draw_indices{};
                std::vector<DirectX::XMFLOAT3> vertices{};
                std::vector<DirectX::XMFLOAT3> normals{};
                std::vector<std::vector<DirectX::XMFLOAT2>> uvs(5);
                std::vector<std::vector<DirectX::XMVECTOR>> tangents(5);
                std::vector<std::vector<DirectX::XMVECTOR>> colors(5);

                // Internals
                std::vector<DirectX::XMFLOAT3> v1_vertices{};
                std::vector<DirectX::XMFLOAT3> v1_normals{};
                std::vector<DirectX::XMVECTOR> v2_vertices{};
                std::vector<DirectX::XMFLOAT2> v2_normals{};

                std::vector<std::vector<DirectX::XMFLOAT2>> v2_uvs(5);
                std::vector<std::vector<DirectX::XMVECTOR>> v2_tangents(5);
                std::vector<std::vector<DirectX::XMVECTOR>> v2_colors(5);
                std::vector<std::vector<DirectX::XMVECTOR>> v2_weights;

                std::vector<std::vector<DirectX::XMVECTOR>> v2_vweights{};
                std::vector<std::vector<DirectX::XMVECTOR>> v2_nweights{};

                auto& index_buffer = m_bundle->IndexBuffers[entry.mesh.index_buffer_id];
                if (!index_buffer.Buffer.empty())
                {
                    indices.resize(entry.mesh.indexes_count);

                    auto front = index_buffer.Buffer.data() + (entry.mesh.start_index * index_buffer.stride);
                    auto back = entry.mesh.indexes_count * index_buffer.stride;

                    std::memcpy(indices.data(), front, back);
                }

                uint32_t vertex_id_min = 0xffffffff;
                uint32_t vertex_id_max = 0x0;

                for (auto id = std::begin(indices); id != std::end(indices); ++id)
                {
                    if (vertex_id_max < *id)
                        vertex_id_max = *id;

                    if (vertex_id_min > *id)
                        vertex_id_min = *id;
                }

                for (auto id = std::begin(indices); id != std::end(indices); ++id)
                {
                    *id -= vertex_id_min;
                }

                for (auto& buff : entry.mesh.vertex_buffer_ids)
                {
                    int32_t primary_id = entry.mesh.vertex_buffer_ids[0].id;

                    auto& vertex_layout = m_bundle->VertexLayouts[entry.mesh.vertex_layout_id];
                    auto& vertex_buffer = m_bundle->VertexBuffers[buff.id];

                    auto reader = std::make_unique<DirectX::VBReader>();

                    HRESULT hr = reader->Initialize(vertex_layout.elements.data(), vertex_layout.elements.size());
                    if (FAILED(hr))
                    {
                        printf("ERROR: Failed setting up VB reader (%08X)\n", static_cast<unsigned int>(hr));
                    }

                    int tVerts = m_bundle->VertexBuffers[primary_id].length; //this assumes that VerB[-1]/VerB[0] contains all possible vertices
                    int nVerts = (buff.input_slot == 0) ? vertex_buffer.length : ((vertex_id_max + 1) - vertex_id_min); // entry.mesh.vertices_count;
                    int offset = (buff.input_slot == 0) ? 0 : buff.offset + (vertex_id_min + entry.mesh.base_vertex) * buff.stride;

                    hr = reader->AddStream(vertex_buffer.Buffer.data() + offset, nVerts, buff.input_slot, buff.stride);
                    if (FAILED(hr))
                    {
                        printf("ERROR: Failed setting up stream for VB reader (%08X)\n", static_cast<unsigned int>(hr));
                    }
                    else
                    {
                        // Position data (XMFLOAT3)
                        if (buff.input_slot == 0 && m_bundle->Model[0].model.decompress_flags == 0)
                        {
                            v1_vertices.resize(nVerts);

                            hr = reader->Read(v1_vertices.data(), "POSITION", 0, nVerts);
                            if (FAILED(hr))
                            {
                                //printf("ERROR: Failed reading VB POSITION (%08X)\n", static_cast<unsigned int>(hr));
                            }
                        }

                        // Position data (XMVECTOR)
                        if (buff.input_slot == 0 && m_bundle->Model[0].model.decompress_flags == 1)
                        {
                            v2_vertices.resize(tVerts);

                            hr = reader->Read(v2_vertices.data(), "POSITION", 0, nVerts);
                            if (FAILED(hr))
                            {
                                //printf("ERROR: Failed reading VB POSITION (%08X)\n", static_cast<unsigned int>(hr));
                            }
                        }

                        if (buff.input_slot == 1)
                        {
                            // Normals data (XMFLOAT3)
                            if (m_bundle->Model[0].model.decompress_flags == 0)
                            {
                                v1_normals.resize(tVerts);

                                hr = reader->Read(v1_normals.data() + vertex_id_min, "NORMAL", 0, nVerts);
                                if (FAILED(hr))
                                {
                                    //printf("ERROR: Failed reading VB NORMAL (%08X)\n", static_cast<unsigned int>(hr));
                                }
                            }

                            // Normals data (XMFLOAT2)
                            if (m_bundle->Model[0].model.decompress_flags == 1)
                            {
                                v2_normals.resize(tVerts);

                                hr = reader->Read(v2_normals.data() + vertex_id_min, "NORMAL", 0, nVerts);
                                if (FAILED(hr))
                                {
                                    //printf("ERROR: Failed reading VB NORMAL (%08X)\n", static_cast<unsigned int>(hr));
                                }
                            }

                            // Texcoord data (XMFLOAT2)
                            for (uint32_t i = 0; i < uvs.size(); ++i)
                            {
                                v2_uvs[i].resize(tVerts);

                                hr = reader->Read(v2_uvs[i].data() + vertex_id_min, "TEXCOORD", i, nVerts);
                                if (FAILED(hr))
                                {
                                    //printf("ERROR: Failed reading VB TEXCOORD_%i (%08X)\n", i, static_cast<unsigned int>(hr));
                                }
                            }

                            // Tangent data (XMVECTOR)
                            for (uint32_t i = 0; i < tangents.size(); ++i)
                            {
                                v2_tangents[i].resize(tVerts);

                                hr = reader->Read(v2_tangents[i].data() + vertex_id_min, "TANGENT", i, nVerts);
                                if (FAILED(hr))
                                {
                                    //printf("ERROR: Failed reading VB TANGENT_%i (%08X)\n", i, static_cast<unsigned int>(hr));
                                }
                            }

                            // Color data (XMVECTOR)
                            for (uint32_t i = 0; i < colors.size(); ++i)
                            {
                                v2_colors[i].resize(tVerts);

                                hr = reader->Read(v2_colors[i].data() + vertex_id_min, "COLOR", i, nVerts);
                                if (FAILED(hr))
                                {
                                    //printf("ERROR: Failed reading VB COLOR_%i (%08X)\n", i, static_cast<unsigned int>(hr));
                                }
                            }
                        }
                    }
                }

                auto& morph_buffer = m_bundle->MorphBuffers[entry.mesh.morph_data_buffer_id];
                if (!morph_buffer.Buffer.empty())
                {
                    auto reader = std::make_unique<DirectX::VBReader>();
                    auto inputLayout = generateMorphLayout(&morph_buffer);

                    HRESULT hr = reader->Initialize(inputLayout.data(), inputLayout.size());
                    if (FAILED(hr))
                    {
                        printf("ERROR: Failed setting up VB reader (%08X)\n", static_cast<unsigned int>(hr));
                    }

                    size_t nVerts = morph_buffer.length;

                    hr = reader->AddStream(morph_buffer.Buffer.data(), nVerts, 0, morph_buffer.stride);
                    if (FAILED(hr))
                    {
                        printf("ERROR: Failed setting up stream for VB reader (%08X)\n", static_cast<unsigned int>(hr));
                    }
                    else
                    {
                        v2_vweights.resize(entry.mesh.morph_weights_count);
                        v2_nweights.resize(entry.mesh.morph_weights_count);

                        for (int i = 0; i < entry.mesh.morph_weights_count; ++i)
                        {
                            // Weight data (XMVECTOR)
                            {
                                v2_vweights[i].resize(nVerts);

                                hr = reader->Read(v2_vweights[i].data(), "SV_Position_Weight", i, nVerts);
                                if (FAILED(hr))
                                {
                                    //printf("ERROR: Failed reading VB XMVECTOR (%08X)\n", static_cast<unsigned int>(hr));
                                }
                            }
                        }

                        for (int i = 0; i < entry.mesh.morph_weights_count; ++i)
                        {
                            // Weight data (XMVECTOR)
                            {
                                v2_nweights[i].resize(nVerts);

                                hr = reader->Read(v2_nweights[i].data(), "SV_Normal_Weight", i, nVerts);
                                if (FAILED(hr))
                                {
                                    //printf("ERROR: Failed reading VB XMVECTOR (%08X)\n", static_cast<unsigned int>(hr));
                                }
                            }
                        }
                    }
                }

                vertices.reserve(entry.mesh.vertices_count);
                normals.reserve(entry.mesh.vertices_count);

                for (auto it = v2_uvs.begin(); it != v2_uvs.end(); ++it)
                {
                    int64_t index = std::distance(v2_uvs.begin(), it);

                    if (!it->empty())
                    {
                        uvs[index].reserve(entry.mesh.vertices_count);
                    }
                }

                for (uint32_t index = 0, vertex_id = vertex_id_min; vertex_id < (vertex_id_max + 1); ++vertex_id, ++index)
                {
                    //int index = static_cast<uint32_t>(std::distance(indices.begin(), vertex_id));

                    if (m_bundle->Model[0].model.decompress_flags == 0)
                    {
                        vertices.emplace_back(v1_vertices[vertex_id]);
                        normals.emplace_back(v1_normals[vertex_id]);
                    }

                    if (m_bundle->Model[0].model.decompress_flags == 1)
                    {
                        DirectX::XMFLOAT3 positionVertex;
                        DirectX::XMStoreFloat3(&positionVertex, v2_vertices[vertex_id]);

                        if (entry.mesh.position_translate.has_value() && entry.mesh.position_scale.has_value())
                        {
                            float v_x = positionVertex.x * entry.mesh.position_scale.value().x + entry.mesh.position_translate.value().x;
                            float v_y = positionVertex.y * entry.mesh.position_scale.value().y + entry.mesh.position_translate.value().y;
                            float v_z = positionVertex.z * entry.mesh.position_scale.value().z + entry.mesh.position_translate.value().z;

                            vertices.emplace_back(v_x, v_y, v_z);
                        }

                        if (use_normals)
                        {
                            float n_x = DirectX::XMVectorGetW(v2_vertices[vertex_id]);
                            float n_y = v2_normals[vertex_id].x;
                            float n_z = v2_normals[vertex_id].y;

                            normals.emplace_back(n_x, n_y, n_z);
                        }
                    }

                    if (entry.mesh.morph_data_buffer_id != -1 && p_records != nullptr)
                    {
                        int TireWidthMM = -1;
                        int TireAspect = -1;
                        int WheelOriginalDiameterIN = -1;
                        int WheelDiameterIN = -1;

                        switch (direction)
                        {
                        case 0:
                            TireWidthMM = p_records->FrontTireWidthMM;
                            TireAspect = p_records->FrontTireAspect;
                            WheelOriginalDiameterIN = p_records->FrontWheelDiameterIN;
                            WheelDiameterIN = p_records->FrontWheelDiameterIN;
                            break;
                        case 1:
                            TireWidthMM = p_records->RearTireWidthMM;
                            TireAspect = p_records->RearTireAspect;
                            WheelOriginalDiameterIN = p_records->RearWheelDiameterIN;
                            WheelDiameterIN = p_records->RearWheelDiameterIN;
                            break;
                        case 2:
                            TireWidthMM = p_records->FrontTireWidthMM;
                            TireAspect = p_records->FrontTireAspect;
                            WheelOriginalDiameterIN = p_records->FrontWheelDiameterIN;
                            WheelDiameterIN = p_records->FrontWheelDiameterIN;
                            break;
                        default:
                            // unreacheable
                            break;
                        }

                        if (type == 44) // wheel
                        {
                            std::array<float, 2> weights{ static_cast<float>((WheelDiameterIN - static_cast<float>(10)) / 14), static_cast<float>((1 - TireWidthMM / static_cast<float>(1000)) / 0.9) };

                            for (size_t weid = 0; weid < entry.mesh.morph_weights_count; ++weid)
                            {
                                DirectX::XMFLOAT3 local_weights;
                                DirectX::XMStoreFloat3(&local_weights, v2_vweights[weid][vertex_id + entry.mesh.base_vertex]);

                                uint32_t weight_id = static_cast<uint32_t>(DirectX::XMVectorGetW(v2_vweights[weid][vertex_id + entry.mesh.base_vertex]));

                                vertices[index].x += local_weights.x * weights[weight_id];
                                vertices[index].y += local_weights.y * weights[weight_id];
                                vertices[index].z += local_weights.z * weights[weight_id];
                            }

                            {
                                vertices[index].x *= 1;
                            }
                        }

                        if (type == 8) // tire
                        {
                            std::array<float, 5> weights{ static_cast<float>((static_cast<float>(TireWidthMM) * TireAspect / 100 - 225 + WheelOriginalDiameterIN * 12.7) / 275), static_cast<float>((WheelDiameterIN - 10) / static_cast<float>(14)), 0, 0, 0 };

                            for (size_t weid = 0; weid < entry.mesh.morph_weights_count; ++weid)
                            {
                                DirectX::XMFLOAT3 local_weights;
                                DirectX::XMStoreFloat3(&local_weights, v2_vweights[weid][vertex_id + entry.mesh.base_vertex]);

                                uint32_t weight_id = static_cast<uint32_t>(DirectX::XMVectorGetW(v2_vweights[weid][vertex_id + entry.mesh.base_vertex]));

                                vertices[index].x += local_weights.x * weights[weight_id];
                                vertices[index].y += local_weights.y * weights[weight_id];
                                vertices[index].z += local_weights.z * weights[weight_id];
                            }

                            {
                                vertices[index].x *= TireWidthMM / static_cast<float>(1000);
                            }
                        }

                        if (type == 44) // wheel normals
                        {
                            std::array<float, 2> my_weight{ static_cast<float>((WheelDiameterIN - static_cast<float>(10)) / 14), static_cast<float>((1 - TireWidthMM / static_cast<float>(1000)) / 0.9) };

                            for (size_t weid = 0; weid < entry.mesh.morph_weights_count; ++weid) //entry.mesh.morph_weights_count
                            {
                                DirectX::XMFLOAT3 local_weights;
                                DirectX::XMStoreFloat3(&local_weights, v2_nweights[weid][vertex_id + entry.mesh.base_vertex]);

                                uint32_t weight_id = static_cast<uint32_t>(DirectX::XMVectorGetW(v2_nweights[weid][vertex_id + entry.mesh.base_vertex]));

                                normals[index].x += local_weights.x * my_weight[weight_id];
                                normals[index].y += local_weights.y * my_weight[weight_id];
                                normals[index].z += local_weights.z * my_weight[weight_id];
                            }

                            {
                                DirectX::XMStoreFloat3(&normals[index], DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&normals[index])));
                            }

                            {
                                normals[index].x /= 1;
                            }
                        }

                        if (type == 8) // tire normals
                        {
                            std::array<float, 5> my_weight{ static_cast<float>((static_cast<float>(TireWidthMM) * TireAspect / 100 - 225 + WheelOriginalDiameterIN * 12.7) / 275), static_cast<float>((WheelDiameterIN - 10) / static_cast<float>(14)), 0, 0, 0 };

                            for (size_t weid = 0; weid < entry.mesh.morph_weights_count; ++weid) //entry.mesh.morph_weights_count
                            {
                                DirectX::XMFLOAT3 local_weights;
                                DirectX::XMStoreFloat3(&local_weights, v2_nweights[weid][vertex_id + entry.mesh.base_vertex]);

                                uint32_t weight_id = static_cast<uint32_t>(DirectX::XMVectorGetW(v2_nweights[weid][vertex_id + entry.mesh.base_vertex]));

                                normals[index].x += local_weights.x * my_weight[weight_id];
                                normals[index].y += local_weights.y * my_weight[weight_id];
                                normals[index].z += local_weights.z * my_weight[weight_id];
                            }

                            {
                                DirectX::XMStoreFloat3(&normals[index], DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&normals[index])));
                            }

                            {
                                normals[index].x /= TireWidthMM / static_cast<float>(1000);
                            }
                        }
                    }

                    for (size_t i = 0; i < uvs.size() && !v2_uvs[i].empty(); ++i)
                    {
                        float u = (v2_uvs[i][vertex_id].x * entry.mesh.texcoord_transform[i].v_scale) + entry.mesh.texcoord_transform[i].v_translation;
                        float v = (v2_uvs[i][vertex_id].y * entry.mesh.texcoord_transform[i].u_scale) + entry.mesh.texcoord_transform[i].u_translation;

                        uvs[i].emplace_back(u, v);
                    }
                }

                v1_vertices.clear();
                v1_normals.clear();
                v2_vertices.clear();
                v2_normals.clear();
                v2_uvs.clear();
                v2_tangents.clear();
                v2_colors.clear();
                v2_weights.clear();

                m_meshes.emplace_back(std::any_cast<std::string>(entry.metadata["Name"]), matrix, (m_geometry_type == QUADS) ? convertToQuads(indices) : indices, vertices, normals, uvs, entry.mesh.material_id.value()); //"polySurface"
            }
        }

        std::vector<Mesh> GetMeshes() { return m_meshes; }


    private:
        std::shared_ptr<BundleReader::BundleData> m_bundle = nullptr;
        std::shared_ptr<DataBaseRecords> m_records = nullptr;
        uint32_t type{};
        int8_t direction{};
        int32_t m_requested_lod{};
        GeometryType m_geometry_type{};

        std::vector<Mesh> m_meshes{};

        std::vector<uint32_t> convertToQuads(const std::vector<uint32_t>& indices) {
            std::vector<uint32_t> result{};
            std::vector<bool> faces_used(indices.size(), false);

            for (auto i = indices.begin(); i != indices.end(); i += 3) {
                auto i0 = std::next(i, 0);
                auto i2 = std::next(i, 1);
                auto i1 = std::next(i, 2);

                size_t index = std::distance(indices.begin(), i);

                if (faces_used[index]) {
                    continue;
                }

                auto find_ref = [&](auto next) {
                    for (; next != indices.end(); next += 3) {
                        auto f0 = std::next(next, 0);
                        auto f2 = std::next(next, 1);
                        auto f1 = std::next(next, 2);

                        if (*i0 == *f2 && *i2 == *f0) {
                            size_t indice = std::distance(indices.begin(), next);

                            result.push_back(*i0);
                            result.push_back(*i1);
                            result.push_back(*i2);
                            result.push_back(*f1);

                            faces_used[indice] = true;

                            //printf("Face: %i, Indices: %i %i %i %i\n", index, *i0, *i1, *i2, *f1);

                            return true;
                        }
                    }

                    return false;
            };

                if (!find_ref(std::next(i, 3))) {
                    result.push_back(*i0);
                    result.push_back(*i1);
                    result.push_back(*i2);
                    result.push_back(0xffffffff);

                    //printf("Face: %i, Indices: %i %i %i \n", index, *i0, *i1, *i2);
                }

            }

            return result;
        }

        std::vector<D3D11_INPUT_ELEMENT_DESC> generateMorphLayout(BundleReader::BlobDescBuffer* blob)
        {
            std::vector<D3D11_INPUT_ELEMENT_DESC> result{};

            /*
            // weight_id (rim)
            //   0 - diameter
            //   1 - width
            // weight_id (tire)
            //   0 - outer diameter
            //   1 - inner diameter
            //   2:3 - tire deformation
            */

            if (blob) {
                result.reserve(blob->num_elements);

                for (uint32_t SemanticIndex = 0; SemanticIndex < static_cast<uint32_t>(blob->num_elements / 2); ++SemanticIndex)
                {
                    result.push_back({ "SV_Position_Weight", SemanticIndex, blob->format, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                }

                for (uint32_t SemanticIndex = 0; SemanticIndex < static_cast<uint32_t>(blob->num_elements / 2); ++SemanticIndex)
                {
                    result.push_back({ "SV_Normal_Weight", SemanticIndex, blob->format, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
                }
            }

            return result;
        }
    };

}