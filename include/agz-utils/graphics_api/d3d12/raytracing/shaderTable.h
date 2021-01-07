#pragma once

#include <agz-utils/graphics_api/d3d12/raytracing/common.h>
#include <agz-utils/graphics_api/d3d12/buffer.h>

AGZ_D3D12_RT_BEGIN

class ShaderRecord
{
public:

    ShaderRecord(
        void *identifier, UINT identifierSize);

    ShaderRecord(
        void *identifier,         UINT identifierSize,
        void *localRootArguments, UINT localRootArgumentsSize);

    void *identifier;
    UINT identifierSize;

    void *localRootArguments;
    UINT localRootArgumentsSize;
};

class ShaderTable : public misc::uncopyable_t
{
public:

    ShaderTable();

    ShaderTable(ResourceManager &rscMgr, UINT recordCount, UINT recordSize);

    ShaderTable(ShaderTable &&other) noexcept;

    ShaderTable &operator=(ShaderTable &&other) noexcept;

    void swap(ShaderTable &other) noexcept;

    void pushBack(const ShaderRecord &record);

    UINT getShaderRecordSize() const noexcept;

    Buffer &getShaderTable() noexcept;

    D3D12_GPU_VIRTUAL_ADDRESS getGPUVirtualAddress() const noexcept;

    UINT getByteSize() const noexcept;

private:

    Buffer buffer_;

    UINT  recordSize_;
    char *mappedRecords_;
};

AGZ_D3D12_RT_END
