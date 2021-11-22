#pragma once
#pragma warning(disable: 4996)
#include <windows.h>
#include<iostream>
#include <stdio.h>
#include<sstream>
#include<vector>
#include<fstream>
#include  <cctype>
using namespace std;

string drive;
LPCWSTR driveTemp;


struct BOOTSECTOR {
    char OEM_ID[9];
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
    int Cluster_RDET;       //Cluster bắt đầu RDET
    char Volume_Label[12];
    char FAT_Type[8];
};

struct VolumeBootRecord {
    char OEM_ID[9];
    int Byte_Per_Sector;
    int Sector_Per_Cluster;
    int SectorPerTrack;
    int NumberOfHead;
    int StartClusterOfMFT;
    int MFT_entry_size;
    int VolumeSize;
};

struct ThongTinTapTin {
    int clusterBatDau;
    int clusterKetThuc;
} typedef FileInFo;


int ReadSector(LPCWSTR  drive, int readPoint, BYTE* sector,int numberByteRead)
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

    if (!ReadFile(device, sector, numberByteRead, &bytesRead, NULL))
    {
        printf("ReadFile: %u\n", GetLastError());
    }
    /*else
    {
        printf("Doc sector thanh cong!\n");
    }*/

    return 0;
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
        else if (hexVal[i] >= 'a' && hexVal[i] <= 'f') {
            dec_val += (int(hexVal[i]) - 87) * base;
            base = base * 16;
        }

    }
    return dec_val;
}

vector<int> Folder_Cluster_List(int clusterBatDau,string*HFAT) { //Nhập vào cluster bắt đầu -> trả về danh sách cách cluster
    vector<int>listTemp;
    string stemp;
    int k = clusterBatDau;
    stemp = HFAT[(4 * k) + 3] + HFAT[(4 * k) + 2] + HFAT[(4 * k) + 1] + HFAT[4 * k];
    if (stemp == "0ffffff8" || stemp =="0fffffff")
        listTemp.push_back(k);
    else {
        while (stemp != "0ffffff8" && stemp!="0fffffff") {
            listTemp.push_back(k);
            k = convertHexToDec(stemp);
            stemp = HFAT[4 * k + 3] + HFAT[4 * k + 2] + HFAT[4 * k + 1] + HFAT[4 * k];
        }
        listTemp.push_back(k);
    }
    return listTemp;
}

void ReadFileInFolder(int sector,int bytePerSector,int stt) {   //Hàm đọc thông tin các tệp tin trong thư mục

                BYTE* RDETT = new BYTE[bytePerSector*8];
                string* HRDETT = new string[bytePerSector*8];
                string temp;
                ReadSector(driveTemp, sector*bytePerSector, RDETT, bytePerSector*8);


                for (int i = 0; i < bytePerSector*8; i++)
                {
                    HRDETT[i] = convertByteToHex(RDETT[i]);

                }

                vector<int> iindex20;
                for (int i = 0; i < bytePerSector/4; i++) {
                    if (HRDETT[32 * i + 11] == "20")
                        iindex20.push_back(i);

                }
                if (iindex20.size() > 0) {      //Nếu có trên một tệp tin trong thư much thì liệt kê các tệp tin đó
                    cout << "Danh sach tap tin:" << endl;

                    for (int m = 0; m < iindex20.size(); m++)
                    {
                        cout << "Tap tin thu " << stt << "-" << m + 1 << ": ";
                        int l = iindex20[m] - 1;
                        if (HRDETT[32 * l + 11] == "0f")
                        {
                            while (true) {
                                if (HRDETT[32 * l + 11] == "0f") {
                                    // Đọc 10 byte tại vị trí 1
                                    for (int j = 0; j < 10; j++)
                                    {
                                        cout << RDETT[l * 32 + 1 + j];
                                    }
                                    //Đọc 12 byte tại vị trí e = 14
                                    for (int j = 0; j < 12; j++)
                                    {
                                        cout << RDETT[l * 32 + 14 + j];
                                    }
                                    //Đọc 4 byte tại vị trí 1c = 28
                                    for (int j = 0; j < 4; j++)
                                    {
                                        cout << RDETT[l * 32 + 28 + j];
                                    }
                                }
                                else {
                                    break;
                                }
                                l--;
                                if (l < 0) break;

                            }

                        }
                        else {
                            l++;
                            for (int j = 0; j < 11; j++)
                            {
                                if (j == 7) {
                                    cout << ".";
                                    continue;
                                }


                                cout << RDETT[l * 32 + j];
                            }
                        }
                        cout << endl;
                        l = iindex20[m];
                        temp = HRDETT[32 * l + 31] + HRDETT[32 * l + 30] + HRDETT[32 * l + 29] + HRDETT[32 * l + 28];
                        int size = convertHexToDec(temp);

                        if (size >= 1024 * 1024)
                            cout << "    Kich co: " << size << " byte = " << size / 1024 / 1024 << " MB";
                        else
                            cout << "    Kich co: " << size << " byte = " << size / 1024 << " KB";
                        cout << endl;
                    }

                }
                else {
                    cout << "Thu muc rong" << endl;
                }
                delete[] RDETT;
                delete[] HRDETT;
}

void ReadChildrenFolder(int sector, int bytePerSector, int stt, string* HFAT ,BOOTSECTOR boot) {    //Hàm đọc thông tin thư mục con và các tệp tin con.
    BYTE* RDETT = new BYTE[bytePerSector * 8];
    string* HRDETT = new string[bytePerSector * 8];
    string temp;
    ReadSector(driveTemp, sector * bytePerSector, RDETT, bytePerSector * 8);

    for (int i = 0; i < bytePerSector * 8; i++)
    {
        HRDETT[i] = convertByteToHex(RDETT[i]);
    }

    vector<int> iindex10;
    for (int i = 0; i < bytePerSector / 4; i++) {
        if (HRDETT[32 * i + 11] == "10")
            iindex10.push_back(i);
    }
    
    if (iindex10.size() == 0)
        return;
    else {
        cout << endl << endl << "Danh sach thu muc:" << endl;
        for (int m = 0; m < iindex10.size(); m++)
        {
            cout << "Thu muc thu " << m + 1 << ": ";
            int l = iindex10[m] - 1;
            if (HRDETT[32 * l + 11] == "0f")
            {
                while (true) {
                    if (HRDETT[32 * l + 11] == "0f") {
                        // Đọc 10 byte tại vị trí 1
                        for (int j = 0; j < 10; j++)
                        {
                            cout << RDETT[l * 32 + 1 + j];
                        }
                        //Đọc 12 byte tại vị trí e = 14
                        for (int j = 0; j < 12; j++)
                        {
                            cout << RDETT[l * 32 + 14 + j];
                        }
                        //Đọc 4 byte tại vị trí 1c = 28
                        for (int j = 0; j < 4; j++)
                        {
                            cout << RDETT[l * 32 + 28 + j];
                        }

                    }
                    else {
                        break;
                    }
                    l--;
                    if (l < 0)
                        break;
                }

                l = iindex10[m];
                cout << endl;
                temp = HRDETT[32 * l + 21] + HRDETT[32 * l + 20];
                int ttemp = convertHexToDec(temp);
                temp = HRDETT[32 * l + 27] + HRDETT[32 * l + 26];
                ttemp = ttemp + convertHexToDec(temp);
                vector<int>folderListTemp = Folder_Cluster_List(ttemp, HFAT);
                /*cout << " + Chiem cac cluster: ";
                for (int i = 0; i < folderListTemp.size(); i++)
                    cout << folderListTemp[i] << "  ";*/

                    /*cout << "   --> Chiem cac sector: ";
                    for (int i = 0; i < folderListTemp.size(); i++) {
                        cout << "     " << boot.Sector_In_Bootsector + boot.Number_Of_FAT * boot.FAT_Size + (folderListTemp[i] - 2) * boot.Sector_Per_Cluster << "->";
                        cout << boot.Sector_In_Bootsector + boot.Number_Of_FAT * boot.FAT_Size + (folderListTemp[i] - 1) * boot.Sector_Per_Cluster - 1 << endl;
                    }*/



                int sectorTemp;
                for (int i = 0; i < folderListTemp.size(); i++) {
                    sectorTemp = boot.Sector_In_Bootsector + boot.Number_Of_FAT * boot.FAT_Size + (folderListTemp[i] - 2) * boot.Sector_Per_Cluster;
                    ReadFileInFolder(sectorTemp, boot.Byte_Per_Sector, m + 1);
                }

            }
            else {
                l++;
                for (int j = 0; j < 11; j++)
                {
                    cout << RDETT[l * 32 + j];
                }

                cout << endl;
                l = iindex10[m];
                cout << endl;
                temp = HRDETT[32 * l + 21] + HRDETT[32 * l + 20];
                int ttemp = convertHexToDec(temp);
                temp = HRDETT[32 * l + 27] + HRDETT[32 * l + 26];
                ttemp = ttemp + convertHexToDec(temp);
                vector<int>folderListTemp = Folder_Cluster_List(ttemp, HFAT);
                /* cout << " + Chiem cac cluster: ";
                 for (int i = 0; i < folderListTemp.size(); i++)
                     cout << folderListTemp[i] << "  ";*/

                     /*cout << "   --> Chiem cac sector: ";
                     for (int i = 0; i < folderListTemp.size(); i++) {
                         cout << "     " << boot.Sector_In_Bootsector + boot.Number_Of_FAT * boot.FAT_Size + (folderListTemp[i] - 2) * boot.Sector_Per_Cluster << "->";
                         cout << boot.Sector_In_Bootsector + boot.Number_Of_FAT * boot.FAT_Size + (folderListTemp[i] - 1) * boot.Sector_Per_Cluster - 1 << endl;
                     }*/



                int sectorTemp;
                for (int i = 0; i < folderListTemp.size(); i++) {
                    sectorTemp = boot.Sector_In_Bootsector + boot.Number_Of_FAT * boot.FAT_Size + (folderListTemp[i] - 2) * boot.Sector_Per_Cluster;
                    ReadFileInFolder(sectorTemp, boot.Byte_Per_Sector, m + 1);
                }

            }
            cout << endl;
        }
    }
    delete[]RDETT;
    delete[]HRDETT;
    return;
}

int main(int argc, char** argv)
{
    
    BYTE *sector = new BYTE[512];
    string* HSector=new string[512]; //Mảng lưu thông tin sector dạng hexa
    string temp;  

    
    cout << "Nhap phan vung muon doc du lieu (C,D,E,F,...): ";
    cin >> drive;
    while ((drive.length()>1)||!((drive[0] >= 'A' && drive[0] <= 'Z') || (drive[0] >= 'a' && drive[0] <= 'z'))) {
        cout << "Phan vung khong hop le.Nhap lai: ";
        cin >> drive;
    }
    drive[0] = toupper(drive[0]);
    
    if (drive[0] == 'C')
        driveTemp = L"\\\\.\\C:";
    else if(drive[0] == 'D')
        driveTemp = L"\\\\.\\D:";
    else if (drive[0] == 'E')
        driveTemp = L"\\\\.\\E:";
    else if (drive[0] == 'F')
        driveTemp = L"\\\\.\\F:";
    else if (drive[0] == 'G')
        driveTemp = L"\\\\.\\G:";
    else if (drive[0] == 'H')
        driveTemp = L"\\\\.\\H:";
    else if (drive[0] == 'I')
        driveTemp = L"\\\\.\\I:";
    else if (drive[0] == 'J')
        driveTemp = L"\\\\.\\J:";
    else if (drive[0] == 'K')
        driveTemp = L"\\\\.\\K:";
    else {
        cout << "Chi co the doc tu o C den K" << endl;
        return -1;
    }
    

    ReadSector(driveTemp, 0, sector,512);

    //Chuyển mảng sector thành mảng hex lưu trong HSector
    for (int i = 0; i < 512; i++) {
        HSector[i] = convertByteToHex(sector[i]);
    }
    //cout << "     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" << endl;
    //int j = 0;
    //cout << "00" << " | ";
    //for (int i = 0; i < 512; i++) {
    //    cout << HSector[i] << " ";
    //    if ((i + 1) % 16 == 0)
    //    {
    //        j++;
    //        cout << endl;
    //        if (j < 10)
    //            cout << "0" << j << " | ";
    //        else
    //            cout << j << " | ";
    //    }
    //}
    cout << endl;
    
    temp = HSector[36] + HSector[37] + HSector[38] + HSector[39]; 
    //Kiểm tra 4 byte tại 0x24 Nếu có giá trị 80008000 thì đọc phân vùng NTFS

    if(temp=="80008000")
    {
        cout << "Doc thong tin phan vung NTFS" << endl;
        VolumeBootRecord VBR;

        //OS Version
        for (int i = 3; i < 11; i++) {
            VBR.OEM_ID[i - 3] = sector[i];
        }
        VBR.OEM_ID[8] = '\0';
        cout << "+ OEM/Name of management system: " << VBR.OEM_ID << endl;

        //Số byte trên một sector
        temp = HSector[12] + HSector[11];
        VBR.Byte_Per_Sector = convertHexToDec(temp);
        cout << "+ So byte tren moi sector la: 0x" << temp << " = " << VBR.Byte_Per_Sector << " byte" << endl;

        //Số sector trên một cluster
        temp = HSector[13];
        VBR.Sector_Per_Cluster = convertHexToDec(temp);
        cout << "+ So sector tren moi cluster la: 0x" << temp << " = " << VBR.Sector_Per_Cluster << " sector" << endl;

        //Số sector trên một track
        temp = HSector[25] + HSector[24];
        VBR.SectorPerTrack = convertHexToDec(temp);
        cout << "+ So sector tren mot track la: 0x" << temp << " = " << VBR.SectorPerTrack << " sector" << endl;

        //Số mặt đĩa
        temp = HSector[27] + HSector[26];
        VBR.NumberOfHead = convertHexToDec(temp);
        cout << "+ So head(side) la: 0x" << temp << " = " << VBR.NumberOfHead << endl;

        //Cluster bắt đầu của MFT
        for (int i = 54; i >= 48; i--)
            temp = temp + HSector[i];
        VBR.StartClusterOfMFT = convertHexToDec(temp);
        cout << "+ Cluster bat dau MFT la: 0x" << temp << " = " << VBR.StartClusterOfMFT << endl;

        //Kích thước mỗi bản ghi trong MFT
        temp = HSector[64];
        VBR.MFT_entry_size = convertHexToDec(temp);
        cout << "+ Kich thuoc moi ban ghi trong MFT la: 0x" << temp << " = " << VBR.MFT_entry_size<<" byte" << endl;

        //Tổng số sector của ổ đĩa
        for (int i = 47; i >= 40; i--)
            temp = temp + HSector[i];
        VBR.VolumeSize = convertHexToDec(temp);
        cout << "+ Tong so sector la: 0x" << temp << " = " << VBR.VolumeSize << endl;

        delete[] sector;
        delete[] HSector;
    }
        
    else {
        cout << "Doc thong tin phan vung FAT32" << endl;
        
        BOOTSECTOR boot;

        //Volume Label
        for (int i = 71; i <= 81; i++)
            boot.Volume_Label[i - 71] = sector[i];
        boot.Volume_Label[11] = '\0';
        cout << "+ Volume Label: " << boot.Volume_Label << endl;

        //OS Version
        for (int i = 3; i < 11; i++) {
            boot.OEM_ID[i - 3] = sector[i];
        }
        boot.OEM_ID[8] = '\0';
        cout << "+ OEM: " << boot.OEM_ID << endl;
       

        //Số byte trên một sector
        temp = HSector[12] + HSector[11];
        boot.Byte_Per_Sector = convertHexToDec(temp);
        cout << "+ So byte tren moi sector la: " << temp << "h  = " << boot.Byte_Per_Sector << " byte" << endl;

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

        //Tổng số sector
        temp = HSector[20] + HSector[19];
        if (temp == "0000")
            temp = HSector[35] + HSector[34] + HSector[33] + HSector[32];  //Đọc 4 byte từ offset 20h <-> 32d
        boot.Volume_Size = convertHexToDec(temp);
        cout << "+ Kich thuoc vung he thong la: " << temp << "h  = " << boot.Volume_Size << " byte = " << boot.Volume_Size / 1024 / 1024 << "MB" << endl;

        //Cluster bắt đầu RDET
        temp = HSector[47] + HSector[46] + HSector[45] + HSector[44];  //Đọc 4 byte từ offset 2Ch <-> 44d
        boot.Cluster_RDET = convertHexToDec(temp);
        cout << "+ Cluster bat dau RDET la: " << temp << "h  = " << boot.Cluster_RDET << endl;

        delete[] sector;
        delete[] HSector;
        //-------------------------------------------------------------------------------
        int DistanceToFAT1 = boot.Sector_In_Bootsector * boot.Byte_Per_Sector;  //Vị trí byte bắt đầu bảng FAT1
        int startByteOfRDET = (boot.Sector_In_Bootsector + boot.Number_Of_FAT * boot.FAT_Size) * boot.Byte_Per_Sector;    //Vị trí byte bắt đầu của bảng RDET
        int FATByteSize = boot.Byte_Per_Sector * boot.FAT_Size; //Kích thước bảng FAT tính theo byte
        int Byte_Per_Cluster = boot.Byte_Per_Sector * boot.Sector_Per_Cluster;

        BYTE* FAT1 = new BYTE[FATByteSize];
        int fatByteSize = boot.FAT_Size * boot.Byte_Per_Sector;

        ReadSector(driveTemp, DistanceToFAT1, FAT1, FATByteSize);
        cout << endl << endl << "Dang do bang FAT. Vui long doi....." << endl;
        string* HFAT1 = new string[FATByteSize]; //Mảng lưu thông tin FAT1 dạng Hex
        for (int i = 0; i < FATByteSize; i++) {
            HFAT1[i] = convertByteToHex(FAT1[i]);
        }
        delete[] FAT1;
        //freopen("FAT1.txt", "wt", stdout);
        //cout << "     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" << endl;
        //int j = 0;
        //cout << "00" << " | ";
        //for (int i = 0; i < FATByteSize; i++) {
        //    cout << HFAT1[i] << " ";
        //    if ((i + 1) % 16 == 0)
        //    {
        //        j++;
        //        cout << endl;
        //        if (j < 10)
        //            cout << "0" << j << " | ";
        //        else
        //            cout << j << " | ";
        //    }
        //}
        //fclose(stdout);

        cout << "Dang tim vi tri RDET. Vui long doi....." << endl;
        

        vector<int> RDET_cluster_list;
        int k = boot.Cluster_RDET;
        temp = HFAT1[(4 * k) + 3] + HFAT1[(4 * k) + 2] + HFAT1[(4 * k) + 1] + HFAT1[4 * k];
        if (temp == "0ffffff8" || temp=="0fffffff")
            RDET_cluster_list.push_back(k);
        else {
            while (temp != "0ffffff8" && temp != "0fffffff") {
                RDET_cluster_list.push_back(k);
                cout << k << " ";
                k = convertHexToDec(temp);
                temp = HFAT1[4 * k + 3] + HFAT1[4 * k + 2] + HFAT1[4 * k + 1] + HFAT1[4 * k];
            }
        }

        cout << "RDET nam tren cac cluster: ";
        for (int i = 0; i < RDET_cluster_list.size(); i++) {
            cout << RDET_cluster_list[i] << " ";
        }
        cout << "  --> Chiem cac sector: ";
        for (int i = 0; i < RDET_cluster_list.size(); i++) {
            cout << boot.Sector_In_Bootsector+RDET_cluster_list[i]*boot.Sector_Per_Cluster << "->";
            cout << boot.Sector_In_Bootsector + RDET_cluster_list[i] * boot.Sector_Per_Cluster + boot.Sector_Per_Cluster -1 << endl;
        }
        cout << endl << endl;

        int q = 0;
        BYTE* RDET = new BYTE[RDET_cluster_list.size() * 32 * 512];
        string* HRDET = new string[RDET_cluster_list.size() * 32 * 512];
        for (int i = 0; i < RDET_cluster_list.size(); i++) {
            BYTE* Rtemp = new BYTE[32 * 512];
            ReadSector(driveTemp, boot.Volume_Size + (RDET_cluster_list[i] - 1) * Byte_Per_Cluster, Rtemp, 32 * 512);
            for (q; q < (i + 1) * 32 * 512; q++) {
                RDET[q] = Rtemp[q - i * (32 * 512)];
            }
            delete[] Rtemp;
        }
        
        

        for (int i = 0; i < RDET_cluster_list.size()* 32 * 512; i++)
        {
            HRDET[i] = convertByteToHex(RDET[i]);
        }

        vector<int> index20;
        for (int i = 0; i < 512; i++) {
            if (HRDET[32 * i + 11] == "20")
                index20.push_back(i);

        }
        vector<int>index10;
        for (int i = 0; i < 512; i++) {
            if (HRDET[32 * i + 11] == "10")
                index10.push_back(i);

        }


        if (index20.size() > 0) {
            cout << endl << "Danh sach tap tin:" << endl;

            for (int m = 0; m < index20.size(); m++)
            {
                cout << "Tap tin thu " << m + 1 << ": ";
                int l = index20[m] - 1;
                if (HRDET[32 * l + 11] == "0f")
                {
                    while (true) {
                        if (HRDET[32 * l + 11] == "0f") {
                            // Đọc 10 byte tại vị trí 1
                            for (int j = 0; j < 10; j++)
                            {
                                cout << RDET[l * 32 + 1 + j];
                            }
                            //Đọc 12 byte tại vị trí e = 14
                            for (int j = 0; j < 12; j++)
                            {
                                cout << RDET[l * 32 + 14 + j];
                            }
                            //Đọc 4 byte tại vị trí 1c = 28
                            for (int j = 0; j < 4; j++)
                            {
                                cout << RDET[l * 32 + 28 + j];
                            }
                        }
                        else {
                            break;
                        }
                        l--;
                        if (l < 0) break;

                    }
                    l++;
                }
                else {
                    l++;
                    for (int j = 0; j < 11; j++)
                    {
                        if (j == 7) {
                            cout << ".";
                            continue;
                        }


                        cout << RDET[l * 32 + j];
                    }
                }
                l = index20[m];
                cout << endl;
                temp = HRDET[32 * l + 21] + HRDET[32 * l + 20];
                int ttemp = convertHexToDec(temp);
                temp = HRDET[32 * l + 27] + HRDET[32 * l + 26];
                ttemp = ttemp + convertHexToDec(temp);
                vector<int>folderListTemp = Folder_Cluster_List(ttemp, HFAT1);
                cout << " + Chiem cac cluster: ";
                for (int i = 0; i < folderListTemp.size(); i++)
                    cout << folderListTemp[i] << "  ";
                cout << endl;
                cout << "   --> Chiem cac sector: " << endl;
                for (int i = 0; i < folderListTemp.size(); i++) {
                    cout << "     " << boot.Sector_In_Bootsector + boot.Number_Of_FAT * boot.FAT_Size + (folderListTemp[i] - 2) * boot.Sector_Per_Cluster << "->";
                    cout << boot.Sector_In_Bootsector + boot.Number_Of_FAT * boot.FAT_Size + (folderListTemp[i] - 1) * boot.Sector_Per_Cluster - 1 << endl;
                }

                temp = HRDET[32 * l + 31] + HRDET[32 * l + 30] + HRDET[32 * l + 29] + HRDET[32 * l + 28];
                int size = convertHexToDec(temp);

                if (size >= 1024 * 1024)
                    cout << "    Kich co: " << size << " byte = " << size / 1024 / 1024 << " MB";
                else
                    cout << "    Kich co: " << size << " byte = " << size / 1024 << " KB";
                cout << endl;
            }
        }
        

        if(index10.size()>0){
            cout << endl << endl << "Danh sach thu muc:" << endl;
            for (int m = 0; m < index10.size(); m++)
            {
                cout << "Thu muc thu " << m + 1 << ": ";
                int l = index10[m] - 1;
                if (HRDET[32 * l + 11] == "0f")
                {
                    while (true) {
                        if (HRDET[32 * l + 11] == "0f") {
                            // Đọc 10 byte tại vị trí 1
                            for (int j = 0; j < 10; j++)
                            {
                                cout << RDET[l * 32 + 1 + j];
                            }
                            //Đọc 12 byte tại vị trí e = 14
                            for (int j = 0; j < 12; j++)
                            {
                                cout << RDET[l * 32 + 14 + j];
                            }
                            //Đọc 4 byte tại vị trí 1c = 28
                            for (int j = 0; j < 4; j++)
                            {
                                cout << RDET[l * 32 + 28 + j];
                            }

                        }
                        else {
                            break;
                        }
                        l--;
                        if (l < 0)
                            break;
                    }

                    l = index10[m];
                    cout << endl;
                    temp = HRDET[32 * l + 21] + HRDET[32 * l + 20];
                    int ttemp = convertHexToDec(temp);
                    temp = HRDET[32 * l + 27] + HRDET[32 * l + 26];
                    ttemp = ttemp + convertHexToDec(temp);
                    vector<int>folderListTemp = Folder_Cluster_List(ttemp, HFAT1);
                    cout << " + Chiem cac cluster: ";
                    for (int i = 0; i < folderListTemp.size(); i++)
                        cout << folderListTemp[i] << "  ";

                    cout << "   --> Chiem cac sector: ";
                    for (int i = 0; i < folderListTemp.size(); i++) {
                        cout << "     " << boot.Sector_In_Bootsector + boot.Number_Of_FAT * boot.FAT_Size + (folderListTemp[i] - 2) * boot.Sector_Per_Cluster << "->";
                        cout << boot.Sector_In_Bootsector + boot.Number_Of_FAT * boot.FAT_Size + (folderListTemp[i] - 1) * boot.Sector_Per_Cluster - 1 << endl;
                    }



                    int sectorTemp;
                    for (int i = 0; i < folderListTemp.size(); i++) {
                        sectorTemp = boot.Sector_In_Bootsector + boot.Number_Of_FAT * boot.FAT_Size + (folderListTemp[i] - 2) * boot.Sector_Per_Cluster;
                        ReadFileInFolder(sectorTemp, boot.Byte_Per_Sector, m + 1);
                        ReadChildrenFolder(sectorTemp, boot.Byte_Per_Sector, m + 1, HFAT1,boot);
                    }

                }
                else {
                    l++;
                    for (int j = 0; j < 11; j++)
                    {
                        cout << RDET[l * 32 + j];
                    }

                    cout << endl;
                    l = index10[m];
                    cout << endl;
                    temp = HRDET[32 * l + 21] + HRDET[32 * l + 20];
                    int ttemp = convertHexToDec(temp);
                    temp = HRDET[32 * l + 27] + HRDET[32 * l + 26];
                    ttemp = ttemp + convertHexToDec(temp);
                    vector<int>folderListTemp = Folder_Cluster_List(ttemp, HFAT1);
                    cout << " + Chiem cac cluster: ";
                    for (int i = 0; i < folderListTemp.size(); i++)
                        cout << folderListTemp[i] << "  ";

                    cout << "   --> Chiem cac sector: ";
                    for (int i = 0; i < folderListTemp.size(); i++) {
                        cout << "     " << boot.Sector_In_Bootsector + boot.Number_Of_FAT * boot.FAT_Size + (folderListTemp[i] - 2) * boot.Sector_Per_Cluster << "->";
                        cout << boot.Sector_In_Bootsector + boot.Number_Of_FAT * boot.FAT_Size + (folderListTemp[i] - 1) * boot.Sector_Per_Cluster - 1 << endl;
                    }



                    int sectorTemp;
                    for (int i = 0; i < folderListTemp.size(); i++) {
                        sectorTemp = boot.Sector_In_Bootsector + boot.Number_Of_FAT * boot.FAT_Size + (folderListTemp[i] - 2) * boot.Sector_Per_Cluster;
                        ReadFileInFolder(sectorTemp, boot.Byte_Per_Sector, m + 1);
                        ReadChildrenFolder(sectorTemp, boot.Byte_Per_Sector, m + 1, HFAT1, boot);
                    }

                }
                cout << endl;

            }
        }
        delete[] HFAT1;
        delete[] RDET;
        delete[] HRDET;
    }
        
    return 0;
}