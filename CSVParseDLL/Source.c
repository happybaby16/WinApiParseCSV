#include <Windows.h>
#include "../WinApiParseCSV/Header.h"


BOOL WINAPI DllMain(HINSTANCE hlnstDll, DWORD dwReason, LPVOID IpReserved)
{
	BOOL bAllWentWell = TRUE;
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	if (bAllWentWell)
		return TRUE;
	else
		return FALSE;
}

LPWSTR Reading(LPWSTR path)//������ ������ �� ����� � ���������� ������
{
    DWORD d = 0;
    HINSTANCE FileHandle = CreateFile(PATH, GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    LPWSTR ReadString = calloc(100500, sizeof(WCHAR));//�������� ������ ��� ������
    ReadFile(FileHandle, ReadString, 100500 * sizeof(WCHAR), &d, NULL);//������ �� �����
    CloseHandle(FileHandle);
    ReadString[d / 2] = '\n';//��� ��� ����, ��� ������ ����� ������� ���������� ����� �� �������� '\n'
    ReadString[d / 2 + 1] = '\0';//������ ������
    return ReadString;

}

DWORD LpwstrToDword(LPWSTR str)//������� �� ������ � �����
{
    DWORD dw = 0;
    for (size_t i = 0; i < wcslen(str); i++)//�� ���� �������� ������
    {
        dw += (str[i] - '0');//�������� �����
        dw *= 10;//����������� ������ ����� �� 1
    }
    return dw / 10;//����� �� 10, �.�. ������ ��� ��������� ������
}
DWORD CountUsers(LPWSTR str)//������� ������� ���������� ������� � �����
{
    DWORD count = 0;
    for (size_t i = 0; i < wcslen(str); i++)//���������� ���������� ����� � �����
    {
        if (str[i] == L'\n') count++;
    }
    return count;
}

__declspec(dllexport) U* CreateUserArray(LPWSTR path, LPDWORD n)//������� ������ � ������ ��������
{
    LPWSTR str = Reading(path);//�������� ������ �� �����
    DWORD count = CountUsers(str);//���������� ���������� �������
    U* users = calloc(count, sizeof(U));//�������� ������ ��� ������ ��������
    DWORD poz = 0, zap = 0, ind = 0;//������� ��� ��������
    LPWSTR strvozr = calloc(3, sizeof(TCHAR));//������ ��� �������� (���������)
    for (size_t i = 0; i < wcslen(str); i++)
    {
        if (str[i] == '\n')//���� ����� ������
        {
            strvozr = calloc(3, sizeof(TCHAR));
            zap++;//��������� � ���������� �������� �������
            poz = 0;
            ind = 0;
        }
        else
        {
            if (str[i] == ';')//���� �����������
            {
                poz++;//������ ���� ���������
                ind = 0;//������ ����� � ������ � ���
            }
            else
            {
                if (poz == 0) users[zap].f[ind] = str[i];
                if (poz == 1) users[zap].i[ind] = str[i];
                if (poz == 2) users[zap].o[ind] = str[i];
                if (poz == 3)
                {
                    if (str[i] == '\r')//������ ����� ��������� ������ '\r''\n'
                    {
                        strvozr[ind] = '\0';//��������� ������ � ���������
                        users[zap].vozr = LpwstrToDword(strvozr);//����������� �� � �����
                        free(strvozr);
                    }
                    else strvozr[ind] = str[i];//�������� ������, ���������� �������
                }
                ind++;//��������� � ��������� �����
            }
        }
    }
    *n = count;//���������� ����� �������
    return users;//���������� ��������� �� ������
}


__declspec(dllexport) U* Search(LPWSTR path, LPWSTR F, LPDWORD n)
{
   //������ ������ ��������� ��� ���������
    WCHAR search_name[30];
    for (DWORD i = 0; i < 15; i++)
    {
        search_name[i] = F[i];
        if (F[i] == '\0')
        {
            for (DWORD j = i; j < 15; j++)
            {
                search_name[j] = '\0';
            }
            break;
        }
    }


    DWORD success_count = 0;
    DWORD count = 0;
    U* users = CreateUserArray(path, &count);
    for (DWORD i = 0; i < count; i++)
    {
        if ((wcscmp(users[i].f, search_name))==0)
        {
            success_count++;
        }
    }
    

    U* success_users = calloc(success_count, sizeof(U));

    success_count = 0;
    for (DWORD i = 0; i < count; i++)
    {
        if ((wcscmp(users[i].f, search_name)) == 0)
        {
            success_users[success_count] = users[i];
            success_count++;
        }
    }


    *n = success_count;
    return success_users;
}




__declspec(dllexport) DOUBLE AvgAgeUsers(U* users, LPDWORD count)
{
    DOUBLE avg = 0;
    for (DWORD i = 0; i < *count; i++)
    {
        avg += users[i].vozr;
    }
    return avg / *count;
}




__declspec(dllexport) void WriteToCSV(U* users, LPDWORD count)
{

    for (int i = 0; i < *count; i++)
    {
        for (int j = 0;; j++)
        {
            if (users[i].f[j] == '\0')
            {
                users[i].f[j] = ';';
                break;
            }
        }
    }


    for (int i = 0; i < *count; i++)
    {
        for (int j = 0;; j++)
        {
            if (users[i].i[j] == '\0')
            {
                users[i].i[j] = ';';
                break;
            }
        }
    }

    for (int i = 0; i < *count; i++)
    {
        for (int j = 0;; j++)
        {
            if (users[i].o[j] == '\0')
            {
                users[i].o[j] = ';';
                break;
            }
        }
    }


    for (int i = 0; i < *count; i++)
    {
        WCHAR a[15];
        swprintf(a, 15, TEXT("%i"), users[i].vozr);
    }


    DWORD w = 0;
    HINSTANCE FileHandle = CreateFile(L"1.txt", GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    for (size_t i = 0; i < *count; i++)
    {
        WriteFile(FileHandle, users[i].f, wcslen(users[i].f) * sizeof(WCHAR), &w, NULL);
        WriteFile(FileHandle, users[i].i, wcslen(users[i].i) * sizeof(WCHAR), &w, NULL);
        WriteFile(FileHandle, users[i].o, wcslen(users[i].o) * sizeof(WCHAR), &w, NULL);
        WCHAR a[15];
        swprintf(a, 15, TEXT("%i"), users[i].vozr);
        for (int j = 0; j < 15; j++)
        {
            if (a[j] == '\0')
            {
                a[j] = '\n';
                j++;
                a[j] = '\0';
                j++;
                for (int k = j; k < 15; k++)
                {
                    a[k] = '\0';
                }
                break;
            }
        }
        WriteFile(FileHandle, a, wcslen(a) * sizeof(WCHAR), &w, NULL);
    }

    DOUBLE avg_age_users = AvgAgeUsers(users, count);

    WCHAR a[15];
    swprintf(a, 15, TEXT("%f"), avg_age_users);
    for (int j = 0; j < 15; j++)
    {
        if (a[j] == '\0')
        {
            a[j] = '\n';
            j++;
            a[j] = '\0';
            j++;
            for (int k = j; k < 15; k++)
            {
                a[k] = '\0';
            }
            break;
        }
    }


    WCHAR text[30] = L"������� �������: ";
    WriteFile(FileHandle, text, wcslen(text) * sizeof(WCHAR), &w, NULL);
    WriteFile(FileHandle, a, wcslen(a) * sizeof(WCHAR), &w, NULL);

    CloseHandle(FileHandle);

    
}
