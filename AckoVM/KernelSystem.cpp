#include "KernelSystem.h"

KernelSystem::KernelSystem(PhysicalAddress processVMSpace,
                           PageNum processVMSpaceSize, PhysicalAddress pmtSpace,
                           PageNum pmtSpaceSize, Partition* partition) {}

KernelSystem::~KernelSystem() {}

Process* KernelSystem::createProcess() { return nullptr; }

Time KernelSystem::periodicJob() { return Time(); }

Status KernelSystem::access(ProcessId pid, VirtualAddress address,
                            AccessType type) {
  return Status();
}
