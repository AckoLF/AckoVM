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
  auto kernelSystem = KernelSystem::getInstance();
  auto freeSegments = &kernelSystem->freeSegments;
  std::cout << "freeSegments: " << freeSegments->size() << std::endl;
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
  // and then load it with content
  // TODO (acko): Load the segment with content
  return Status::OK;
}

Status KernelProcess::deleteSegment(VirtualAddress startAddress) {
  return Status::OK;
}

Status KernelProcess::pageFault(VirtualAddress address) { return Status(); }

PhysicalAddress KernelProcess::getPhysicalAddress(VirtualAddress address) {
  VirtualAddress offset = address % PAGE_SIZE;
  VirtualAddress alignedVirtualAddress = address - offset;
  auto& it = virtualAddressToPhysicalAddress.find(alignedVirtualAddress);
  if (it != virtualAddressToPhysicalAddress.end()) {
    auto physicalAddress = reinterpret_cast<uint64_t>(it->second);
    return reinterpret_cast<PhysicalAddress>(physicalAddress + offset);
  } else {
    return 0;
  }
}
