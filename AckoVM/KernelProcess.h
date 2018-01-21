#pragma once

#include "vm_declarations.h"

#include <unordered_map>

class KernelProcess {
 public:
  KernelProcess(ProcessId pid);
  ~KernelProcess();
  ProcessId getProcessId() const;
  Status createSegment(VirtualAddress startAddress, PageNum segmentSize,
                       AccessType flags);
  Status loadSegment(VirtualAddress startAddress, PageNum segmentSize,
                     AccessType flags, void *content);
  Status deleteSegment(VirtualAddress startAddress);
  Status pageFault(VirtualAddress address);
  PhysicalAddress getPhysicalAddress(VirtualAddress address);
  AccessType getSegmentAccessPermissions(VirtualAddress address);

 private:
  ProcessId pid;
  std::unordered_map<VirtualAddress, PhysicalAddress>
      virtualAddressToPhysicalAddress;
  std::unordered_map<VirtualAddress, AccessType> segmentAccessPermissions;
};
