#include "ConsoleEncoding.h"

#include <clocale>
#include <windows.h>

namespace ConsoleEncoding {
    void UseUtf8()
    {
        // MSVC는 /utf-8 옵션으로 소스의 한글 문자열을 UTF-8 바이트로 컴파일합니다.
        // 콘솔 코드페이지도 UTF-8로 맞춰야 실행 중 cout/cin 한글이 깨지지 않습니다.
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        setlocale(LC_ALL, ".UTF-8");
    }
}
