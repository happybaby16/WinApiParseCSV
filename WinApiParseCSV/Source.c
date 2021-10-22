#include "Header.h"
#define PATHDLL L"CSVParseDLL.dll"
#define PATHLIST L"Users.csv"

typedef U*(_cdecl* MyFunction)(LPWSTR, LPWSTR, LPDWORD);
typedef void (_cdecl* WriteToCSV)(U*, LPDWORD );

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    HINSTANCE MyDLL;
    if (!(MyDLL = LoadLibrary(PATHDLL))) return 1;
    MyFunction UserList = (MyFunction)GetProcAddress(MyDLL, "Search");
    DWORD count = 0;
    LPWSTR name = L"Иванов";
    U* users = UserList(PATH, name, &count);


    DWORD count_1 = 0;
    LPWSTR name_1 = L"Иванова";
    U* users_1 = UserList(PATH, name_1, &count_1);

    //Создание результатирующей структуры
    U* users_res = calloc(count+count_1, sizeof(U));
    DWORD count_res = count + count_1;

    for(int i=0;i<count;i++)
    {
        users_res[i] = users[i];
    }

    for (int i = 0; i < count_1; i++)
    {
        users_res[count+i] = users_1[i];
    }


    if (!(MyDLL = LoadLibrary(PATHDLL))) return 1;
    WriteToCSV write = (WriteToCSV)GetProcAddress(MyDLL, "WriteToCSV");
    write(users_res,&count_res);



    return 0;
}