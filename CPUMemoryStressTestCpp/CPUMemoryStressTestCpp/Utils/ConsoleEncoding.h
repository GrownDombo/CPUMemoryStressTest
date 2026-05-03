#pragma once

namespace ConsoleEncoding {
    // Windows 콘솔 입력/출력 코드페이지를 UTF-8로 맞춥니다.
    // /utf-8로 컴파일된 한글 문자열이 실행 중 깨지지 않도록 main 시작 시 한 번 호출합니다.
    void UseUtf8();
}
