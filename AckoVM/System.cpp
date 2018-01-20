#include "System.h"

System::System(PhysicalAddress processVMSpace, PageNum processVMSpaceSize,
               PhysicalAddress pmtSpace, PageNum pmtSpaceSize,
               Partition* partition) {}

System::~System() {}

Process* System::createProcess() { return nullptr; }

Time System::periodicJob() { return Time(); }

Status System::access(ProcessId pid, VirtualAddress address, AccessType type) {
  return Status();
}
