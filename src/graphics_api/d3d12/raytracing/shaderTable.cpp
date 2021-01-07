#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/raytracing/shaderTable.h>
#include <agz-utils/graphics_api/d3d12/resourceManager.h>

AGZ_D3D12_RT_BEGIN

ShaderRecord::ShaderRecord(void *identifier, UINT identifierSize)
    : identifier(identifier),
      identifierSize(identifierSize),
      localRootArguments(nullptr),
      localRootArgumentsSize(0)
{
    
}

ShaderRecord::ShaderRecord(
    void *identifier,
    UINT  identifierSize,
    void *localRootArguments,
    UINT  localRootArgumentsSize)
    : identifier(identifier),
      identifierSize(identifierSize),
      localRootArguments(localRootArguments),
      localRootArgumentsSize(localRootArgumentsSize)
{
    
}

ShaderTable::ShaderTable()
    : recordSize_(0), mappedRecords_(nullptr)
{
    
}

ShaderTable::ShaderTable(
    ResourceManager &rscMgr,
    UINT             recordCount,
    UINT             recordSize)
    : mappedRecords_(nullptr)
{
    recordSize_ = upalign_to<UINT>(
        recordSize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);
    
    const UINT bufferSize = recordCount * recordSize_;
    buffer_ = rscMgr.createUploadBuffer(bufferSize);

    const D3D12_RANGE readRange{ 0, 0 };
    AGZ_D3D12_CHECK_HR(
        buffer_.getResource()->Map(
            0, &readRange, reinterpret_cast<void **>(&mappedRecords_)));
}

ShaderTable::ShaderTable(ShaderTable &&other) noexcept
    : ShaderTable()
{
    swap(other);
}

ShaderTable &ShaderTable::operator=(ShaderTable &&other) noexcept
{
    swap(other);
    return *this;
}

void ShaderTable::swap(ShaderTable &other) noexcept
{
    buffer_.swap(other.buffer_);
    std::swap(recordSize_, other.recordSize_);
    std::swap(mappedRecords_, other.mappedRecords_);
}

void ShaderTable::pushBack(const ShaderRecord &record)
{
    std::memcpy(mappedRecords_, record.identifier, record.identifierSize);
    if(record.localRootArguments)
    {
        std::memcpy(
            mappedRecords_ + record.identifierSize,
            record.localRootArguments,
            record.localRootArgumentsSize);
    }
    mappedRecords_ += recordSize_;
}

UINT ShaderTable::getShaderRecordSize() const noexcept
{
    return recordSize_;
}

Buffer &ShaderTable::getShaderTable() noexcept
{
    return buffer_;
}

D3D12_GPU_VIRTUAL_ADDRESS ShaderTable::getGPUVirtualAddress() const noexcept
{
    return buffer_.getGPUVirtualAddress();
}

UINT ShaderTable::getByteSize() const noexcept
{
    return static_cast<UINT>(buffer_.getByteSize());
}

AGZ_D3D12_RT_END

#endif // #ifdef AGZ_ENABLE_D3D12
