#include <iostream>

#include "part.h"

#pragma comment(lib, "part.lib")

using namespace std;

int main(void) {
  cout << "Hello, World!" << endl;

  auto p1 = new Partition("p1.ini");
  cout << p1->getNumOfClusters() << endl;

  return 0;
}
