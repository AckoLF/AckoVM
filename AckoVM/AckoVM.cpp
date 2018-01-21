#include <iostream>

#include "part.h"
#include "vm_declarations.h"

#include "ProcessTest.h"
#include "System.h"
#include "SystemTest.h"

#define VM_SPACE_SIZE (10000)
#define PMT_SPACE_SIZE (3000)
#define N_PROCESS (2)
#define PERIODIC_JOB_COST (1)

using namespace std;

PhysicalAddress alignPointer(PhysicalAddress address) {
  uint64_t addr = reinterpret_cast<uint64_t>(address);

  addr += PAGE_SIZE;
  addr = addr / PAGE_SIZE * PAGE_SIZE;

  return reinterpret_cast<PhysicalAddress>(addr);
}

int main(void) {
  auto partition = new Partition("p1.ini");
  cout << partition->getNumOfClusters() << endl;

  uint64_t size = (VM_SPACE_SIZE + 2) * PAGE_SIZE;
  PhysicalAddress vmSpace = (PhysicalAddress) new char[size];

  cout << "vmSpace: " << vmSpace << endl;
  PhysicalAddress alignedVmSpace = alignPointer(vmSpace);
  cout << "alignedVmSpace: " << alignedVmSpace << endl;

  size = (PMT_SPACE_SIZE + 2) * PAGE_SIZE;
  PhysicalAddress pmtSpace = (PhysicalAddress) new char[size];

  cout << "pmtSpace: " << pmtSpace << endl;
  PhysicalAddress alignedPmtSpace = alignPointer(pmtSpace);
  cout << "alignedPmtSpace: " << alignedPmtSpace << endl;

  // cout << "Testing whether KernelSystem is singleton or not" << endl;

  // System system1(alignedVmSpace, VM_SPACE_SIZE, alignedPmtSpace,
  // PMT_SPACE_SIZE,
  //               partition);
  // System system2(alignedVmSpace, VM_SPACE_SIZE, alignedPmtSpace,
  // PMT_SPACE_SIZE,
  //               partition);
  // System system3(alignedVmSpace, VM_SPACE_SIZE, alignedPmtSpace,
  // PMT_SPACE_SIZE,
  //               partition);

  // auto process1 = system1.createProcess();
  // auto process2 = system2.createProcess();
  // auto process3 = system3.createProcess();

  // cout << "proccess1 pid: " << process1->getProcessId() << endl;
  // cout << "proccess2 pid: " << process2->getProcessId() << endl;
  // cout << "proccess3 pid: " << process3->getProcessId() << endl;

  // auto virtualAddress = reinterpret_cast<uint64_t>(alignedVmSpace);
  // auto segment =
  //    process1->createSegment(virtualAddress, 5, AccessType::READ_WRITE);
  // cout << "segment: " << segment << endl;

  // for (int i = 0; i < 10; i++) {
  //  auto currentAddress = virtualAddress + i * 10;
  //  auto physicalAddress = process1->getPhysicalAddress(currentAddress);
  //  cout << "currentAddress: " << currentAddress << endl;
  //  cout << "physicalAddress: " << reinterpret_cast<uint64_t>(physicalAddress)
  //       << endl;
  //}

  System system(alignedVmSpace, VM_SPACE_SIZE, alignedPmtSpace, PMT_SPACE_SIZE,
                partition);
  SystemTest systemTest(system, alignedVmSpace, VM_SPACE_SIZE);
  ProcessTest* process[N_PROCESS];
  std::thread* threads[N_PROCESS];

  std::mutex globalMutex;

  for (int i = 0; i < N_PROCESS; i++) {
    process[i] = new ProcessTest(system, systemTest);
  }

  for (int i = 0; i < N_PROCESS; i++) {
    std::cout << "Create process " << i << std::endl;
    threads[i] = new std::thread(&ProcessTest::run, process[i]);
  }

  for (int i = 0; i < N_PROCESS; i++) {
    threads[i]->join();
    delete threads[i];
    delete process[i];
  }

  delete[] vmSpace;
  delete[] pmtSpace;

  std::cout << "Test finished\n";

  int debug;
  cin >> debug;
  cout << debug << endl;

  return 0;
}
