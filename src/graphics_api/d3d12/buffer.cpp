#ifdef AGZ_ENABLE_D3D12

#include <d3dx12.h>

#include <agz/utility/graphics_api/d3d12/buffer.h>
#include <agz/utility/graphics_api/d3d12/resourceManager.h>

AGZ_D3D12_BEGIN

Buffer::Buffer()
    : byteSize_(0), mappedData_(nullptr)
{
    
}

Buffer::Buffer(Buffer &&other) noexcept
    : Buffer()
{
    swap(other);
}

Buffer &Buffer::operator=(Buffer &&other) noexcept
{
    swap(other);
    return *this;
}

void Buffer::initializeDefault(
    ResourceManager      &rscMgr,
    size_t                byteSize,
    D3D12_RESOURCE_STATES initState)
{
    destroy();
    misc::fixed_scope_guard_t guard([&] { destroy(); });

    byteSize_ = byteSize;

    resource_ = rscMgr.create(
        D3D12_HEAP_TYPE_DEFAULT,
        CD3DX12_RESOURCE_DESC::Buffer(byteSize),
        initState);

    guard.dismiss();
}

void Buffer::initializeUpload(
    ResourceManager &rscMgr,
    size_t           byteSize)
{
    destroy();
    misc::fixed_scope_guard_t guard([&] { destroy(); });

    byteSize_ = byteSize;

    resource_ = rscMgr.create(
        D3D12_HEAP_TYPE_UPLOAD,
        CD3DX12_RESOURCE_DESC::Buffer(byteSize),
        D3D12_RESOURCE_STATE_GENERIC_READ);

    D3D12_RANGE readRange = { 0, 0 };
    AGZ_D3D12_CHECK_HR(
        resource_->resource->Map(0, &readRange, &mappedData_));

    guard.dismiss();
}

bool Buffer::isAvailable() const noexcept
{
    return resource_->resource != nullptr;
}

void Buffer::destroy()
{
    resource_.reset();

    byteSize_   = 0;
    mappedData_ = nullptr;
}

ID3D12Resource *Buffer::getResource() const noexcept
{
    return resource_->resource.Get();
}

size_t Buffer::getByteSize() const noexcept
{
    return byteSize_;
}

void Buffer::swap(Buffer &other) noexcept
{
    std::swap(resource_,   other.resource_);
    std::swap(byteSize_,   other.byteSize_);
    std::swap(mappedData_, other.mappedData_);
}

void Buffer::updateData(size_t offset, size_t bytes, const void *data)
{
    std::memcpy(static_cast<char *>(mappedData_) + offset, data, bytes);
}

AGZ_D3D12_END

#endif // #ifdef AGZ_ENABLE_D3D12
