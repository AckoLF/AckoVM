#pragma once

#include "part.h"
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
                     AccessType flags, void* content);
  Status deleteSegment(VirtualAddress startAddress);
  Status pageFault(VirtualAddress address);
  PhysicalAddress getPhysicalAddress(VirtualAddress address);
  AccessType getSegmentAccessPermissions(VirtualAddress address);
  bool isAddressInColdStorage(VirtualAddress address);
  std::unordered_map<VirtualAddress, PhysicalAddress>* getPmt();
  std::unordered_map<VirtualAddress, ClusterNo>* getColdStorage();

 private:
  ProcessId pid;
  std::unordered_map<VirtualAddress, PhysicalAddress> pmt;
  std::unordered_map<VirtualAddress, ClusterNo> coldStorage;
  std::unordered_map<VirtualAddress, AccessType> segmentAccessPermissions;
};
