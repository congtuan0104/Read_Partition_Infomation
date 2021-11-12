#include <windows.h>
#include<iostream>
#include <stdio.h>
#include<sstream>

using namespace std;
int ReadSector(LPCWSTR  drive, int readPoint, BYTE sector[512])
{
    int retCode = 0;
    DWORD bytesRead;
    HANDLE device = NULL;

    device = CreateFile(drive,    // Drive to open
        GENERIC_READ,           // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
        NULL,                   // Security Descriptor
        OPEN_EXISTING,          // How to create
        0,                      // File attributes
        NULL);                  // Handle to template

    if (device == INVALID_HANDLE_VALUE) // Open Error
    {
        printf("CreateFile: %u\n", GetLastError());
        return 1;
    }

    SetFilePointer(device, readPoint, NULL, FILE_BEGIN);//Set a Point to Read

    /*DWORD SetFilePointer(
        [in]                HANDLE hFile,
        [in]                LONG   lDistanceToMove,
        [in, out, optional] PLONG  lpDistanceToMoveHigh,
        [in]                DWORD  dwMoveMethod
    );*/

    if (!ReadFile(device, sector, 512, &bytesRead, NULL))
    {
        printf("ReadFile: %u\n", GetLastError());
    }
    else
    {
        printf("Success!\n");
    }
}
int main(int argc, char** argv)
{
    struct BOOTSECTOR {
        //char Jump_Code[3];
        //char OEM[8];
        //int bytes_per_sector;
        //char sectors_per_cluster;
        //int reserved;
        //char FAT_cnt;
        //int ROOT_size;
        //int total_sectors;
        //char media;
        //int FAT_size;
        //int sectors_per_track;
        //int head_cnt;
        //long hidden_sectors;
        //long total_sectors_long;
        ////char unknown[3];
        //long serial;
        //char volume[11];
        //char FAT_type[8];
        //char loader[448];
        //char Mark[2];
        char Jump_Code[3];
        char OEM_ID[8];
        char Byte_Per_Sector[4];
        char Sector_Per_Cluster;
        char Sector_In_Bootsector[2];
        char Number_Of_FAT;
        char Entry_Of_RDET[2];  //Don't care - 0
        char Sector_of_Vol[2];  //Don't care - 0
        char Device_Typec;
        char Sector_Of_FAT[2];  //Dont't care - 0
        char Sector_of_Track[2];
        char Number_Of_Dau_Doc[2];
        char Khoang_Cach_Tu_Mo_Ta_Vol_Den_Dau_Vol[4];
        char Volume_Size[4];
        char FAT_Size[4];
        char Bit_8_Bat[2];
        char FAT_Version[2];   //FAT32
        char Cluster_Bat_Dau_Cua_RDET[4];
        char Sector_Chua_Thong_Tin_Phu[2];
        char Sector_Chua_Ban_Luu_Cua_Boot_Sector[2];
        char Danh_Rieng[12];
        char Ki_Hieu_Vat_Ly;
        char Danh_Rieng_2;
        char Ki_Hieu_Nhan_Dien_HDH;
        char Serial_Number_Of_Volume[4];
        char Volume_Label[11];
        char FAT_Type[8];
        char Doan_Chuong_Trinh_Khoi_Tao[420];
        char Dau_Hieu_Ket_Thuc_Boot_Sector[2];  //AA 55
    };
    BYTE sector[512];

    struct Hexa {
        char h[2];
    };
    Hexa hexaSector[512];

    stringstream sstream;

    for (int i = 0; i < 512; i++) {
        sstream << hex << (sector[i] - 0);
        string temp = sstream.str();
        hexaSector[i].h[0] = temp[0];
        hexaSector[i].h[1] = temp[1];
    }
    //scanf("%x", hexArr[0]);
    BOOTSECTOR boot;
    ReadSector(L"\\\\.\\H:", 0, sector);
    for (int i = 0; i < 512; i++) {
        cout << hexaSector[i].h[0] << hexaSector[i].h[1] << " ";
        //cout<<sector[i];
        //printf("%02x ", sector[i]);
        //if ((i+1) % 16 == 0) cout << endl;
    }

    //Read data from bootsector bootsector

    //OS Version
    for (int i = 3; i < 11; i++) {
        boot.OEM_ID[i - 3] = sector[i];
    }

    //Số byte trên một sector
    for (int i = 11; i < 13; i++) {
        boot.Byte_Per_Sector[i - 11] = sector[i];
    }

    //00 02 -->02 00
    //cout << result << endl;
    //cout <<"OEM: "<< boot.OEM_ID << endl;
    //printf("%02x ", boot.Byte_Per_Sector[1]);
    //cout << "So byte tren moi sector la: " << boot.Byte_Per_Sector << endl;
    return 0;
}