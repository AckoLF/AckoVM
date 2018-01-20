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
  return PhysicalAddress();
}
