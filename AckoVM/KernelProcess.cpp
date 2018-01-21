#include "KernelProcess.h"
#include "KernelSystem.h"

#include <iostream>

KernelProcess::KernelProcess(ProcessId pid) { this->pid = pid; }

KernelProcess::~KernelProcess() {}

ProcessId KernelProcess::getProcessId() const { return pid; }

Status KernelProcess::createSegment(VirtualAddress startAddress,
                                    PageNum segmentSize, AccessType flags) {
  std::cout << "KernelProcess::createSegment() pid: " << pid
            << " startAddress : " << startAddress
            << " segmentSize: " << segmentSize << std::endl;

  if (startAddress % PAGE_SIZE) {
    std::cout << "KernelProcess::createSegment() startAddress: " << startAddress
              << " is not aligned to PAGE_SIZE: " << PAGE_SIZE << std::endl;
    return Status::TRAP;
  }

  bool overlapFound = false;
  for (int i = 0; i < segmentSize && !overlapFound; i++) {
    VirtualAddress currentAddress = startAddress + i * PAGE_SIZE;
    auto it = virtualAddressToPhysicalAddress.find(currentAddress);
    if (it != virtualAddressToPhysicalAddress.end()) {
      overlapFound = true;
    }
  }

  if (overlapFound) {
    std::cout
        << "KernelProcess::createSegment() overlapFound for startAddress: "
        << startAddress << " segmentSize: " << segmentSize
        << " refusing to createSegment" << std::endl;
    return Status::TRAP;
  }

  auto kernelSystem = KernelSystem::getInstance();
  auto freeSegments = &kernelSystem->freeSegments;
  std::cout << "freeSegments size: " << freeSegments->size() << std::endl;
  for (int i = 0; i < segmentSize; i++) {
    VirtualAddress currentAddress = startAddress + i * PAGE_SIZE;
    auto segmentPhysicalAddress = freeSegments->front();
    freeSegments->pop_front();
    virtualAddressToPhysicalAddress[currentAddress] = segmentPhysicalAddress;
    segmentAccessPermissions[currentAddress] = flags;
  }
  return Status::OK;
}

Status KernelProcess::loadSegment(VirtualAddress startAddress,
                                  PageNum segmentSize, AccessType flags,
                                  void* content) {
  std::cout << "KernelProcess::loadSegment() pid: " << pid
            << " startAddress : " << startAddress
            << " segmentSize: " << segmentSize << std::endl;

  // First create the segment
  auto createSegmentReturnCode =
      this->createSegment(startAddress, segmentSize, flags);
  if (createSegmentReturnCode != Status::OK) {
    std::cout << "KernelProcess::loadSegment() failed to create segment!"
              << std::endl;
    return Status::TRAP;
  }

  // Afterwards load the segment with content
  for (int i = 0; i < segmentSize; i++) {
    auto offset = i * PAGE_SIZE;
    VirtualAddress currentAddress = startAddress + offset;
    auto currentContent = reinterpret_cast<PhysicalAddress>(
        reinterpret_cast<uint64_t>(content) + offset);
    auto physicalAddress = virtualAddressToPhysicalAddress[currentAddress];
    memcpy(physicalAddress, currentContent, PAGE_SIZE);
  }

  return Status::OK;
}

Status KernelProcess::deleteSegment(VirtualAddress startAddress) {
  auto it = virtualAddressToPhysicalAddress.find(startAddress);
  if (it == virtualAddressToPhysicalAddress.end()) {
    std::cout << "KernelProcess::deleteSegment() startAddress: " << startAddress
              << " does not belong to any segment, refusing to deleteSegment"
              << std::endl;
    return Status::TRAP;
  }
  auto physicalAddress = it->second;
  virtualAddressToPhysicalAddress.erase(startAddress);
  segmentAccessPermissions.erase(startAddress);
  return Status::OK;
}

Status KernelProcess::pageFault(VirtualAddress address) {
  // TODO (acko): Implement pageFault, need to track which segments are stored
  // on the partition and generate pageFault when trying to access them. This
  // function should load the segment from the partition and put some other
  // segment on the partition.
  return Status::OK;
}

PhysicalAddress KernelProcess::getPhysicalAddress(VirtualAddress address) {
  VirtualAddress offset = address % PAGE_SIZE;
  VirtualAddress alignedVirtualAddress = address - offset;
  auto& it = virtualAddressToPhysicalAddress.find(alignedVirtualAddress);
  if (it != virtualAddressToPhysicalAddress.end()) {
    auto physicalAddress = reinterpret_cast<uint64_t>(it->second);
    return reinterpret_cast<PhysicalAddress>(physicalAddress + offset);
  }
  return 0;
}

AccessType KernelProcess::getSegmentAccessPermissions(VirtualAddress address) {
  return segmentAccessPermissions[address];
}
