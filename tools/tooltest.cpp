#include "tools.hpp"
#include <cstring>

int main(void) {
  unsigned char test[3] = {0x23, 0xa4, 0xB7};
  char result[7];
  memset(result, 0, 7);

  BinaryBytes2String1(test, 3, result);
  cout << result << endl;

  BinaryBytes2String2(test, 3, result);
  cout << result << endl;

  BinaryBytes2String3(test, 3, result);
  cout << result << endl;

  readPointCloud("../resources/model.pcd");
  return 0;
}