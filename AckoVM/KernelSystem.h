#pragma once

#include "part.h"
#include "vm_declarations.h"

#include "Process.h"

class KernelSystem {
 public:
  KernelSystem(PhysicalAddress processVMSpace, PageNum processVMSpaceSize,
               PhysicalAddress pmtSpace, PageNum pmtSpaceSize,
               Partition* partition);
  ~KernelSystem();
  Process* createProcess();

  Time periodicJob();
  // Hardware job
  Status access(ProcessId pid, VirtualAddress address, AccessType type);
};
