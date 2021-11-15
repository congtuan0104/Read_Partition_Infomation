#include <windows.h>
#include<iostream>
#include <stdio.h>
#include<sstream>
#include<vector>
using namespace std;


vector<BYTE[512]> data;
//ReadSector(L"\\\\.\\H:", 0, sector);
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
    /*DWORD SetFilePointer(HANDLE hFile,LONG   lDistanceToMove,PLONG  lpDistanceToMoveHigh,DWORD  dwMoveMethod);*/

    if (!ReadFile(device, sector, 512, &bytesRead, NULL))
    {
        printf("ReadFile: %u\n", GetLastError());
    }
    else
    {
        printf("Doc sector thanh cong!\n");
    }
}

//Hàm chuyển 1 ký tự ASCII về chuỗi dạng hexa
string convertByteToHex(BYTE byte) {
    stringstream sstream;
    sstream << std::hex << (int)byte;
    string result = sstream.str();
    if (result.length() == 1)
        result = "0" + result;
    return result;
}


int convertHexToDec(string hexVal)
{
    int len = hexVal.size();
    int base = 1;
    int dec_val = 0;


    for (int i = len - 1; i >= 0; i--) {

        if (hexVal[i] >= '0' && hexVal[i] <= '9') {
            dec_val += (int(hexVal[i]) - 48) * base;
            base = base * 16;
        }

        else if (hexVal[i] >= 'A' && hexVal[i] <= 'F') {
            dec_val += (int(hexVal[i]) - 55) * base;
            base = base * 16;
        }
    }
    return dec_val;
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
        int Byte_Per_Sector;
        int Sector_Per_Cluster;
        int Sector_In_Bootsector;
        int Number_Of_FAT;
        char Entry_Of_RDET[2];  //Don't care - 0
        char Sector_of_Vol[2];  //Don't care - 0
        char Device_Typec;
        char Sector_Of_FAT[2];  //Dont't care - 0
        char Sector_of_Track[2];
        char Number_Of_Dau_Doc[2];
        char Khoang_Cach_Tu_Mo_Ta_Vol_Den_Dau_Vol[4];
        int Volume_Size;
        int FAT_Size;
        char Bit_8_Bat[2];
        char FAT_Version[2];   //FAT32
        int Cluster_RDET;       //Cluster bắt đầu RDET
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
    string HSector[512]; //Mảng lưu thông tin sector dạng hexa
    BOOTSECTOR boot;
    string temp;         //Lưu trữ chuỗi hexa của phần đang đọc thông tin
    
    //int count = 0;
    //while()
    ReadSector(L"\\\\.\\H:", 0, sector);
    //Chuyển mảng sector thành mảng hex lưu trong HSector
    for (int i = 0; i < 512; i++) {
        HSector[i] = convertByteToHex(sector[i]);

    }
    /*cout << "     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" << endl;
    int j = 0;
    cout << "00" << " | ";
    for (int i = 0; i < 512; i++) {
        cout << HSector[i] << " ";
        if ((i + 1) % 16 == 0)
        {
            j++;
            cout << endl;
            if (j < 10)
                cout << "0" << j << " | ";
            else
                cout << j << " | ";
        }
    }*/
    cout << endl;

    //OS Version
    for (int i = 3; i < 11; i++) {
            boot.OEM_ID[i - 3] = sector[i];
    }
    boot.OEM_ID[8] = {'\0'};
    cout << "+ OEM: " << boot.OEM_ID << endl;

    //Số byte trên một sector
    temp = HSector[12] + HSector[11];
    boot.Byte_Per_Sector = convertHexToDec(temp);
    cout << "+ So byte tren moi sector la: " <<temp <<"h  = "<<boot.Byte_Per_Sector<< " byte"<<endl;

    //Số sector trên một cluster
    temp = HSector[13];
    boot.Sector_Per_Cluster = convertHexToDec(temp);
    cout << "+ So sector tren moi cluster la: " << temp << "h  = " << boot.Sector_Per_Cluster << " sector" << endl;

    //Số sector trên vùng bootsector
    temp = HSector[15] + HSector[14];
    boot.Sector_In_Bootsector = convertHexToDec(temp);
    cout << "+ So sector tren vung bootsector la: " << temp << "h  = " << boot.Sector_In_Bootsector << " sector" << endl;

    //Số bảng FAT
    temp = HSector[16];
    boot.Number_Of_FAT = convertHexToDec(temp);
    cout << "+ So bang FAT la: " << temp << "h  = " << boot.Number_Of_FAT << endl;

    //Kích thước mỗi bảng FAT
    temp = HSector[39] + HSector[38] + HSector[37] + HSector[36];  //Đọc 4 byte từ offset 24h <-> 36d
    boot.FAT_Size = convertHexToDec(temp);
    cout << "+ Kich thuoc bang FAT la: " << temp << "h  = " << boot.FAT_Size << " sector" << endl;

    //Kích thước volume
    temp = HSector[20] + HSector[19];
    if(temp=="0000")
        temp = HSector[35]+ HSector[34]+ HSector[33]+ HSector[32];  //Đọc 4 byte từ offset 20h <-> 32d
    boot.Volume_Size = convertHexToDec(temp);
    cout << "+ Kich thuoc volume la: " << temp << "h  = " << boot.Volume_Size << " byte = " << boot.Volume_Size / 1024 / 1024 << "MB" << endl;

    //Cluster bắt đầu RDET
    temp = HSector[47] + HSector[46] + HSector[45] + HSector[44];  //Đọc 4 byte từ offset 2Ch <-> 44d
    boot.Cluster_RDET = convertHexToDec(temp);
    cout << "+ Cluster bat dau RDET la: " << temp << "h  = " << boot.Cluster_RDET << endl;




    return 0;
}