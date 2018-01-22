#pragma once

#include "KernelProcess.h"
#include "vm_declarations.h"

class KernelProcess;
class System;
class KernelSystem;

class Process {
 public:
  Process(ProcessId pid);
  ~Process();
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
  KernelProcess* pProcess;
  friend class System;
  friend class KernelSystem;
};
