#pragma once

#include "vm_declarations.h"

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

 private:
  ProcessId pid;
};
