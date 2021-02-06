#ifdef AGZ_ENABLE_D3D12

#include <agz-utils/graphics_api/d3d12/graph/compiler/descriptorSlotAssigner.h>

AGZ_D3D12_GRAPH_BEGIN

bool DescriptorSlotAssigner::DescriptorKey::operator<(
    const DescriptorKey &rhs) const
{
    return std::tie(thread, *item) < std::tie(rhs.thread, *rhs.item);
}

bool DescriptorSlotAssigner::DescriptorRangeKey::operator<(
    const DescriptorRangeKey &rhs) const
{
    return std::tie(thread, *table) < std::tie(rhs.thread, *rhs.table);
}

DescriptorSlotAssigner::DescriptorSlotAssigner(int threadCount)
    : threads_(threadCount)
{
    
}

int DescriptorSlotAssigner::allocateDescriptorSlot(
    const DescriptorKey &key)
{
    const auto it = descriptorSlots_.find(key);
    if(it != descriptorSlots_.end())
        return it->second;

    const int newSlot = static_cast<int>(descriptorSlots_.size());
    descriptorSlots_.insert({ key, newSlot });
    threads_[key.thread].descriptorSlots.push_back({ newSlot, key.item });

    return newSlot;
}

int DescriptorSlotAssigner::allocateDescriptorRangeSlot(
    const DescriptorRangeKey &key)
{
    const auto it = descriptorRangeSlots_.find(key);
    if(it != descriptorRangeSlots_.end())
        return it->second;

    const int newSlot = static_cast<int>(descriptorRangeSlots_.size());
    descriptorRangeSlots_.insert({ key, newSlot });
    threads_[key.thread].descriptorRangeSlots.push_back({ newSlot, key.table });

    return newSlot;
}

int DescriptorSlotAssigner::getThreadCount() const
{
    return static_cast<int>(threads_.size());
}

const std::vector<DescriptorSlotAssigner::DescriptorSlotRecord> &
    DescriptorSlotAssigner::getDescriptorSlotRecords(int threadIndex)
{
    return threads_[threadIndex].descriptorSlots;
}

const std::vector<DescriptorSlotAssigner::DescriptorRangeSlotRecord> &
    DescriptorSlotAssigner::getDescriptorRangeSlotRecords(int threadIndex)
{
    return threads_[threadIndex].descriptorRangeSlots;
}

AGZ_D3D12_GRAPH_END

#endif // #ifdef AGZ_ENABLE_D3D12
