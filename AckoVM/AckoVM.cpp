#include <cassert>
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

  cout << "Test creating overlapping segments and deleting segments" << endl;
  auto p = system.createProcess();
  auto createSegmentReturnCode =
      p->createSegment(0, 10, AccessType::READ_WRITE);
  assert(createSegmentReturnCode == Status::OK);
  createSegmentReturnCode = p->createSegment(1024, 10, AccessType::READ_WRITE);
  assert(createSegmentReturnCode == Status::TRAP);
  auto deleteSegmentReturnCode = p->deleteSegment(1024 * 10);
  assert(deleteSegmentReturnCode == Status::TRAP);
  deleteSegmentReturnCode = p->deleteSegment(1024);
  assert(deleteSegmentReturnCode == Status::OK);
  createSegmentReturnCode = p->createSegment(1024, 1, AccessType::READ_WRITE);
  assert(createSegmentReturnCode == Status::OK);

  cout << "Test finished" << endl;

  int debug;
  cin >> debug;
  cout << debug << endl;

  return 0;
}
