#include <iostream>

#include "part.h"
#include "vm_declarations.h"

#define VM_SPACE_SIZE (10000)
#define PMT_SPACE_SIZE (3000)

using namespace std;

PhysicalAddress alignPointer(PhysicalAddress address) {
  uint64_t addr = reinterpret_cast<uint64_t>(address);

  addr += PAGE_SIZE;
  addr = addr / PAGE_SIZE * PAGE_SIZE;

  return reinterpret_cast<PhysicalAddress>(addr);
}

int main(void) {
  cout << "Hello, World!" << endl;

  auto p1 = new Partition("p1.ini");
  cout << p1->getNumOfClusters() << endl;

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

  return 0;
}
