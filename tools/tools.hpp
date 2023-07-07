#include <assert.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int BinaryBytes2String1(const unsigned char* pSrc, unsigned int nSrcLength, char* pDst);
int BinaryBytes2String2(const unsigned char* pSrc, unsigned int nSrcLength, char* pDst );
int BinaryBytes2String3(const unsigned char* pSrc, int nSrcLength, char* pDst);

vector<vector<double>> readPointCloud(string filePath);