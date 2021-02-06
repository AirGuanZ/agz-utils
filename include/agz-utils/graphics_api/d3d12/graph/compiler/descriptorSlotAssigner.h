#pragma once

#include <map>

#include <agz-utils/graphics_api/d3d12/graph/compiler/descriptorDecleration.h>

AGZ_D3D12_GRAPH_BEGIN

class DescriptorSlotAssigner
{
public:

    struct DescriptorKey
    {
        int             thread = 0;
        DescriptorItem *item   = nullptr;

        bool operator<(const DescriptorKey &rhs) const;
    };

    struct DescriptorRangeKey
    {
        int              thread = 0;
        DescriptorTable *table  = nullptr;

        bool operator<(const DescriptorRangeKey &rhs) const;
    };

    struct DescriptorSlotRecord
    {
        int             slot = 0;
        DescriptorItem *item = nullptr;
    };

    struct DescriptorRangeSlotRecord
    {
        int              slot = 0;
        DescriptorTable *table = nullptr;
    };

    DescriptorSlotAssigner() = default;

    explicit DescriptorSlotAssigner(int threadCount);

    int allocateDescriptorSlot(const DescriptorKey &key);

    int allocateDescriptorRangeSlot(const DescriptorRangeKey &key);

    int getThreadCount() const;

    const std::vector<DescriptorSlotRecord> &
        getDescriptorSlotRecords(int threadIndex);

    const std::vector<DescriptorRangeSlotRecord> &
        getDescriptorRangeSlotRecords(int threadIndex);

private:

    struct PerThread
    {
        std::vector<DescriptorSlotRecord>      descriptorSlots;
        std::vector<DescriptorRangeSlotRecord> descriptorRangeSlots;
    };

    std::map<DescriptorKey, int>      descriptorSlots_;
    std::map<DescriptorRangeKey, int> descriptorRangeSlots_;

    std::vector<PerThread> threads_;
};

AGZ_D3D12_GRAPH_END
