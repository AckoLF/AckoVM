#include "Process.h"

Process::Process(ProcessId pid) { this->pProcess = new KernelProcess(pid); }

Process::~Process() {}

ProcessId Process::getProcessId() const { return pProcess->getProcessId(); }

Status Process::createSegment(VirtualAddress startAddress, PageNum segmentSize,
                              AccessType flags) {
  return this->pProcess->createSegment(startAddress, segmentSize, flags);
}

Status Process::loadSegment(VirtualAddress startAddress, PageNum segmentSize,
                            AccessType flags, void* content) {
  return this->pProcess->loadSegment(startAddress, segmentSize, flags, content);
}

Status Process::deleteSegment(VirtualAddress startAddress) {
  return this->pProcess->deleteSegment(startAddress);
}

Status Process::pageFault(VirtualAddress address) {
  return this->pProcess->pageFault(address);
}

PhysicalAddress Process::getPhysicalAddress(VirtualAddress address) {
  return this->pProcess->getPhysicalAddress(address);
}

AccessType Process::getSegmentAccessPermissions(VirtualAddress address) {
  return pProcess->getSegmentAccessPermissions(address);
}

bool Process::isAddressInColdStorage(VirtualAddress address) {
  return pProcess->isAddressInColdStorage(address);
}

std::unordered_map<VirtualAddress, PhysicalAddress>* Process::getPmt() {
  return pProcess->getPmt();
}

std::unordered_map<VirtualAddress, ClusterNo>* Process::getColdStorage() {
  return pProcess->getColdStorage();
}
