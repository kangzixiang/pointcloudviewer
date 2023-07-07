#include "tools.hpp"
#include <cstring>
#include <fstream>
#include <sstream>

int BinaryBytes2String1(const unsigned char* pSrc, unsigned int nSrcLength, char* pDst)
{
    if ( pSrc == 0 || pDst == 0 )
        return 0;

    char* p = pDst;

    for ( unsigned i = 0; i < nSrcLength; i++ )
    {
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

int BinaryBytes2String2(const unsigned char* pSrc, unsigned int nSrcLength, char* pDst )
{   
    if ( pSrc == 0 || pDst == 0 )
        return 0;

    const char tab[]="0123456789abcdef";   

    for(int i=0; i<nSrcLength; i++)   
    {    
        *pDst++ = tab[*pSrc >> 4];  
        *pDst++ = tab[*pSrc & 0x0f]; 
        pSrc++;   
    }   

    *pDst = '/0';   

    return nSrcLength * 2;   
}   

int BinaryBytes2String3(const unsigned char* pSrc, int nSrcLength, char* pDst)
{
    if ( pSrc == 0 || pDst == 0 )
        return 0;

    for (int i = 0; i < nSrcLength; i++)
    {
        sprintf(pDst, "%02x", *pSrc);
        pDst++;
        pDst++;
        pSrc++;
    }
    *pDst = '/0';

    return nSrcLength * 2;
}

vector<vector<double>> readPointCloud(string filePath)
{
    vector<vector<double>> retVec;
    string comment = "";
    string VERSION = "0.7";
    string FIELDS = "";
    string SIZE = "";
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
        if (line.find("PCD") != string::npos)
        {
            comment = line;
            continue;
        }
        else if (line.find("VERSION") != string::npos)
        {
            VERSION = line;
            continue;
        }
        else if (line.find("FIELDS") != string::npos)
        {
            FIELDS = line;
            continue;
        }
        else if (line.find("SIZE") != string::npos)
        {
            SIZE = line;
            continue;
        }
        else if (line.find("TYPE") != string::npos)
        {
            TYPE = line;
            continue;
        }
        else if (line.find("COUNT") != string::npos)
        {
            COUNT = line;
            continue;
        }
        else if (line.find("WIDTH") != string::npos)
        {
            WIDTH = line;
            continue;
        }
        else if (line.find("HEIGHT") != string::npos)
        {
            HEIGHT = line;
            continue;
        }
        else if (line.find("VIEWPOINT") != string::npos)
        {
            VIEWPOINT = line;
            continue;
        }
        else if (line.find("POINTS") != string::npos)
        {
            POINTS = line;
            continue;
        }
        else if (line.find("DATA") != string::npos)
        {
            DATA = line;
            continue;
        }
        std::stringstream ss(line);
        std::string str;
        vector<double> v;
        while (getline(ss, str, ' '))
        {
            try {
                double temp = std::stod(str);
                v.push_back(temp);
            }
            catch (...)
            {
                cout << str << endl;
            }
            
        }
        retVec.push_back(v);
    }
    ifs.close();
    return retVec;
}

int main(void)
{
    unsigned char test[3] = {0x23, 0xa4, 0xB7};
    char result[7];
    memset(result, 0, 7);

    BinaryBytes2String1(test, 3, result);
    cout<<result<<endl;

    BinaryBytes2String2(test, 3, result);
    cout<<result<<endl;

    BinaryBytes2String3(test, 3, result);
    cout<<result<<endl;

    // readPointCloud("../resources/model.pcd");
    return 0;
}