#include "tools.hpp"
#include <cstring>
#include <fstream>
#include <sstream>

int BinaryBytes2String1(const unsigned char *pSrc, unsigned int nSrcLength,
                        char *pDst) {
  if (pSrc == 0 || pDst == 0)
    return 0;

  char *p = pDst;

  for (unsigned i = 0; i < nSrcLength; i++) {
    *p = (pSrc[i] >> 4) & 0x0f;
    *p += *p < 10 ? 0x30 : (0x61 - 10);
    p++;

    *p = pSrc[i] & 0x0f;
    *p += *p < 10 ? 0x30 : (0x61 - 10);
    p++;
  }

  *p = '/0';
  return nSrcLength * 2;
}

int BinaryBytes2String2(const unsigned char *pSrc, unsigned int nSrcLength,
                        char *pDst) {
  if (pSrc == 0 || pDst == 0)
    return 0;

  const char tab[] = "0123456789abcdef";

  for (int i = 0; i < nSrcLength; i++) {
    *pDst++ = tab[*pSrc >> 4];
    *pDst++ = tab[*pSrc & 0x0f];
    pSrc++;
  }

  *pDst = '/0';

  return nSrcLength * 2;
}

int BinaryBytes2String3(const unsigned char *pSrc, int nSrcLength, char *pDst) {
  if (pSrc == 0 || pDst == 0)
    return 0;

  for (int i = 0; i < nSrcLength; i++) {
    sprintf(pDst, "%02x", *pSrc);
    pDst++;
    pDst++;
    pSrc++;
  }
  *pDst = '/0';

  return nSrcLength * 2;
}

vector<vector<double>> readPointCloud(string filePath) {
  vector<vector<double>> retVec;
  string comment = "";
  string VERSION = "0.7";
  string FIELDS = "";
  vector<string> vecFields;
  string SIZE = "";
  vector<int> vecSieze;
  string TYPE = "";
  string COUNT = "";
  string WIDTH = "";
  string HEIGHT = "";
  string VIEWPOINT = "";
  string POINTS = "";
  string DATA;
  std::ifstream ifs(filePath);
  std::string line;
  while (getline(ifs, line)) {
    if (line.find("PCD") != string::npos) {
      comment = line;
      continue;
    } else if (line.find("VERSION") != string::npos) {
      VERSION = line;
      continue;
    } else if (line.find("FIELDS") != string::npos) {
      FIELDS = line;
      std::stringstream ss(line);
      std::string str;
      while (getline(ss, str, ' ')) {
        if (str == "FIELDS")
          continue;
        vecFields.push_back(str);
      }
      continue;
    } else if (line.find("SIZE") != string::npos) {
      SIZE = line;
      std::stringstream ss(line);
      std::string str;
      while (getline(ss, str, ' ')) {
        int nSize = atoi(str.c_str());
        if (0 == nSize)
          continue;
        vecSieze.push_back(nSize);
      }
      continue;
    } else if (line.find("TYPE") != string::npos) {
      TYPE = line;
      continue;
    } else if (line.find("COUNT") != string::npos) {
      COUNT = line;
      continue;
    } else if (line.find("WIDTH") != string::npos) {
      WIDTH = line;
      continue;
    } else if (line.find("HEIGHT") != string::npos) {
      HEIGHT = line;
      continue;
    } else if (line.find("VIEWPOINT") != string::npos) {
      VIEWPOINT = line;
      continue;
    } else if (line.find("POINTS") != string::npos) {
      POINTS = line;
      continue;
    } else if (line.find("DATA") != string::npos) {
      DATA = line;
      break;
    }
    break;
  }
  char c[4];
  float t = 0.0;
  for (int i = 0;; i++) {
    int nIndex = i % vecFields.size();
    if (ifs.read(c, vecSieze[nIndex])) {
      if (vecFields[nIndex] == "x") {
        vector<double> v;
        memcpy(&t, c, vecSieze[nIndex]);
        v.push_back(t);

        i++;
        nIndex = i % vecFields.size();
        if (ifs.read(c, vecSieze[nIndex])) {
          if (vecFields[nIndex] == "y") {
            memcpy(&t, c, vecSieze[nIndex]);
            v.push_back(t);

            i++;
            nIndex = i % vecFields.size();
            if (ifs.read(c, vecSieze[nIndex])) {
              if (vecFields[nIndex] == "z") {
                memcpy(&t, c, vecSieze[nIndex]);
                v.push_back(t);
                retVec.push_back(v);
              }
            } else {
              break;
            }
          }
        } else {
          break;
        }
      }
    } else {
      break;
    }
  }
  // while (ifs.read(c, 4))
  // {
  //     vector<double> v;
  //     memcpy(&t, c, 4);
  //     v.push_back(t);
  //     if (ifs.read(c, 4))
  //     {
  //         memcpy(&t, c, 4);
  //         v.push_back(t);
  //     }
  //     if (ifs.read(c, 4))
  //     {
  //         memcpy(&t, c, 4);
  //         v.push_back(t);
  //     }
  //     retVec.push_back(v);
  // }
  ifs.close();
  return retVec;
}
