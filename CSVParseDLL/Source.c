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

LPWSTR Reading(LPWSTR path)//читает список их файла и возвращает строку
{
    DWORD d = 0;
    HINSTANCE FileHandle = CreateFile(PATH, GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    LPWSTR ReadString = calloc(100500, sizeof(WCHAR));//выделяем память под строку
    ReadFile(FileHandle, ReadString, 100500 * sizeof(WCHAR), &d, NULL);//читаем из файла
    CloseHandle(FileHandle);
    ReadString[d / 2] = '\n';//это для того, что другой метод считает количество строк по символам '\n'
    ReadString[d / 2 + 1] = '\0';//закрыл строку
    return ReadString;

}

DWORD LpwstrToDword(LPWSTR str)//перевод из строки в число
{
    DWORD dw = 0;
    for (size_t i = 0; i < wcslen(str); i++)//по всем символам строки
    {
        dw += (str[i] - '0');//получаем цифру
        dw *= 10;//увеличиваем разряд числа на 1
    }
    return dw / 10;//делим на 10, т.к. лишний раз увеличили разряд
}
DWORD CountUsers(LPWSTR str)//функция считает количество записей в файле
{
    DWORD count = 0;
    for (size_t i = 0; i < wcslen(str); i++)//определяем количество строк в файле
    {
        if (str[i] == L'\n') count++;
    }
    return count;
}

__declspec(dllexport) U* CreateUserArray(LPWSTR path, LPDWORD n)//парсинг строки в массив структур
{
    LPWSTR str = Reading(path);//получаем строку из файла
    DWORD count = CountUsers(str);//определяем количество записей
    U* users = calloc(count, sizeof(U));//выделили память под массив структур
    DWORD poz = 0, zap = 0, ind = 0;//индексы для парсинга
    LPWSTR strvozr = calloc(3, sizeof(TCHAR));//строка для возраста (временная)
    for (size_t i = 0; i < wcslen(str); i++)
    {
        if (str[i] == '\n')//если новая строка
        {
            strvozr = calloc(3, sizeof(TCHAR));
            zap++;//переходим к следующему элементу массива
            poz = 0;
            ind = 0;
        }
        else
        {
            if (str[i] == ';')//если разделитель
            {
                poz++;//индекс поля структуры
                ind = 0;//индекс буквы в строке с ФИО
            }
            else
            {
                if (poz == 0) users[zap].f[ind] = str[i];
                if (poz == 1) users[zap].i[ind] = str[i];
                if (poz == 2) users[zap].o[ind] = str[i];
                if (poz == 3)
                {
                    if (str[i] == '\r')//символ перед возвратом строки '\r''\n'
                    {
                        strvozr[ind] = '\0';//закрываем строку с возрастом
                        users[zap].vozr = LpwstrToDword(strvozr);//преобразуем ее в число
                        free(strvozr);
                    }
                    else strvozr[ind] = str[i];//набираем строку, содержащую возраст
                }
                ind++;//переходим к следующей букве
            }
        }
    }
    *n = count;//возвращаем длину массива
    return users;//возвращаем указатель на массив
}


__declspec(dllexport) U* Search(LPWSTR path, LPWSTR F, LPDWORD n)
{
   //Делаем строку пригодной для сравнения
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


    WCHAR text[30] = L"Средний возраст: ";
    WriteFile(FileHandle, text, wcslen(text) * sizeof(WCHAR), &w, NULL);
    WriteFile(FileHandle, a, wcslen(a) * sizeof(WCHAR), &w, NULL);

    CloseHandle(FileHandle);

    
}
