#pragma once

#include <map>
#include <string>

#include <agz/utility/d3d11/D3D/Shader/ObjectBinding.h>

AGZ_D3D11_BEGIN

template<ShaderStage STAGE>
class ConstantBufferSlot
{
    UINT slot_;
    ComPtr<ID3D11Buffer> buffer_;

public:

    explicit ConstantBufferSlot(UINT slot = 0) noexcept : slot_(slot) { }

    ConstantBufferSlot(const ConstantBufferSlot &copyFrom) = default;

    ConstantBufferSlot(ConstantBufferSlot &&moveFrom) noexcept = default;

    ConstantBufferSlot &operator=(const ConstantBufferSlot &copyFrom) = default;

    ConstantBufferSlot &operator=(ConstantBufferSlot &&moveFrom) noexcept = default;

    ~ConstantBufferSlot() = default;

    void SetBuffer(ID3D11Buffer *buffer) noexcept
    {
        buffer_ = buffer;
    }

    void SetBuffer(ComPtr<ID3D11Buffer> buffer) noexcept
    {
        buffer_ = buffer;
    }

    UINT GetSlot() const noexcept
    {
        return slot_;
    }

    void Bind() const
    {
        ObjectBinding::BindConstantBuffer<STAGE>(slot_, buffer_.Get());
    }

    void Unbind() const
    {
        ObjectBinding::BindConstantBuffer<STAGE>(slot_, nullptr);
    }
};

template<ShaderStage STAGE>
class ConstantBufferSlotManager
{
public:

    struct Record
    {
        ConstantBufferSlot<STAGE> bufferSlot;

        Record()  = default;
        ~Record() = default;

        Record(const Record &)            = default;
        Record &operator=(const Record &) = default;

        Record(Record &&)            noexcept = default;
        Record &operator=(Record &&) noexcept = default;
    };

    using RecordTable = std::map<std::string, Record, std::less<>>;

    ConstantBufferSlotManager() = default;
    ~ConstantBufferSlotManager() = default;

    ConstantBufferSlotManager(const ConstantBufferSlotManager &)            = default;
    ConstantBufferSlotManager &operator=(const ConstantBufferSlotManager &) = default;

    ConstantBufferSlotManager(ConstantBufferSlotManager &&)            noexcept = default;
    ConstantBufferSlotManager &operator=(ConstantBufferSlotManager &&) noexcept = default;

    explicit ConstantBufferSlotManager(RecordTable table) noexcept
        : table_(std::move(table))
    {
        
    }

    bool Add(std::string name, UINT slot)
    {
        if(table_.find(name) != table_.end())
        {
            return false;
        }
        table_.insert(std::make_pair(
            std::move(name), Record{ ConstantBufferSlot<STAGE>(slot) }));
        return true;
    }

    ConstantBufferSlot<STAGE> *Get(const std::string &name)
    {
        auto it = table_.find(name);
        return it != table_.end() ? &it->second.bufferSlot : nullptr;
    }

    const ConstantBufferSlot<STAGE> *Get(const std::string &name) const
    {
        auto it = table_.find(name);
        return it != table_.end() ? &it->second.bufferSlot : nullptr;
    }

    void Bind() const
    {
        for(auto &it : table_)
        {
            it.second.bufferSlot.Bind();
        }
    }

    void Unbind() const
    {
        for(auto &it : table_)
        {
            it.second.bufferSlot.Unbind();
        }
    }

    template<typename NameIt>
    ConstantBufferSlotManager<STAGE> ExtractPartialManager(NameIt begin, NameIt end)
    {
        RecordTable partialTable;
        for(auto it = begin; it != end; ++it)
        {
            auto key = std::string_view(*it);
            auto itInTable = table_.find(key);
            if(itInTable == table_.end())
            {
                continue;
            }
            partialTable.insert(std::make_pair(std::string(key), std::move(itInTable->second)));
            table_.erase(itInTable);
        }
        return ConstantBufferSlotManager<STAGE>(std::move(partialTable));
    }

private:

    RecordTable table_;
};

AGZ_D3D11_END
