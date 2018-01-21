#include "KernelProcess.h"

KernelProcess::KernelProcess(ProcessId pid) { this->pid = pid; }

KernelProcess::~KernelProcess() {}

ProcessId KernelProcess::getProcessId() const { return pid; }

Status KernelProcess::createSegment(VirtualAddress startAddress,
                                    PageNum segmentSize, AccessType flags) {
  return Status();
}

Status KernelProcess::loadSegment(VirtualAddress startAddress,
                                  PageNum segmentSize, AccessType flags,
                                  void* content) {
  return Status();
}

Status KernelProcess::deleteSegment(VirtualAddress startAddress) {
  return Status();
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
    return nullptr;
  }
}
