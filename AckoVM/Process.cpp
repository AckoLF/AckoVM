#include "Process.h"

Process::Process(ProcessId pid) {}

Process::~Process() {}

ProcessId Process::getProcessId() const { return ProcessId(); }

Status Process::createSegment(VirtualAddress startAddress, PageNum segmentSize,
                              AccessType flags) {
  return Status();
}

Status Process::loadSegment(VirtualAddress startAddress, PageNum segmentSize,
                            AccessType flags, void *content) {
  return Status();
}

Status Process::deleteSegment(VirtualAddress startAddress) { return Status(); }

Status Process::pageFault(VirtualAddress address) { return Status(); }

PhysicalAddress Process::getPhysicalAddress(VirtualAddress address) {
  return PhysicalAddress();
}
