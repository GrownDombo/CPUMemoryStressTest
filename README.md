# CPUMemoryStressTest

Windows 환경에서 CPU와 메모리에 의도적인 부하를 주고, 단일 처리와 병렬 처리의 차이를 CSV로 기록하는 C++ 콘솔 기반 스트레스 테스트 도구입니다.

이 프로젝트는 단순 벤치마크 숫자를 얻는 것보다, **부하 시나리오를 직접 설계하고 실행 흐름을 구조화하며 리소스 사용 패턴을 관찰할 수 있는 도구를 구현하는 것**에 초점을 두었습니다. 초기 아이디어는 C#으로 검증했고, 메모리 할당과 실행 흐름을 더 명확하게 제어하기 위해 C++ 버전을 중심 구현으로 발전시켰습니다.

---

## Portfolio Highlights

- **C++20 기반 콘솔 애플리케이션 설계**
  - Visual Studio 2022, MSBuild, WinAPI, STL 기반 구현
  - Windows 콘솔/CSV 한글 인코딩 처리 및 CRLF 줄바꿈 정책 적용

- **Strategy + Runner 패턴 리팩터링**
  - 각 스트레스 테스트를 `IStressTest` 인터페이스 구현체로 분리
  - 측정, CSV 기록, 테스트 간 대기, 메모리 정리는 `StressTestRunner`에서 공통 처리
  - 새 테스트 추가 시 Runner를 수정하지 않고 테스트 클래스 등록만으로 확장 가능

- **CPU/Memory 부하 시나리오 직접 구현**
  - 배열 수학 연산, 재귀 피보나치, 소수 탐색, 정렬/병합, Mandelbrot 계산, 메모리 할당 테스트
  - 단일 처리와 병렬 처리 방식을 나눠 실행하고 결과를 비교 가능하게 기록

- **안전성과 유지보수성 개선**
  - `new/delete` 중심 구현을 `std::vector`, `std::unique_ptr` 기반으로 개선
  - `_msize()` 의존 제거
  - `std::thread::hardware_concurrency()`가 0을 반환해도 최소 1개 worker 보장
  - Mandelbrot 병렬 처리를 “열마다 스레드 생성” 방식에서 제한된 worker 기반 분할 처리로 개선

---

## Tech Stack

| Area | Details |
|---|---|
| Language | C++20 |
| IDE / Build | Visual Studio 2022, MSBuild |
| Platform | Windows |
| Core APIs | WinAPI (`windows.h`, `psapi.h`, `shlobj.h`) |
| STL | `thread`, `chrono`, `vector`, `memory`, `mutex`, `fstream`, `format` |
| Output | Timestamp 기반 폴더 + 테스트별 CSV |

---

## Architecture

```text
CPUMemoryStressTestCpp/
  CPUMemoryStressTestCpp/
    main.cpp
    Interfaces/
      IStressTest.h
    Runner/
      StressTestRunner.h
      StressTestRunner.cpp
    Logging/
      CsvResultWriter.h
      CsvResultWriter.cpp
    Workloads/
      ArrayMathStressTest.h/.cpp
      FibonacciStressTest.h/.cpp
      PrimeStressTest.h/.cpp
      SortStressTest.h/.cpp
      MandelbrotStressTest.h/.cpp
      MemoryStressTest.h/.cpp
    Utils/
      ConsoleEncoding.h/.cpp
      Environment.h/.cpp
      StressAlgorithms.h/.cpp
```

### Design Intent

`IStressTest`는 각 테스트가 반드시 제공해야 하는 공통 계약입니다.

- `FileName()`은 CSV 파일명을 제공합니다.
- `Title()`은 콘솔과 CSV 제목으로 사용할 표시 이름을 제공합니다.
- `Run()`은 실제 부하 작업만 수행하고 결과 요약 문자열을 반환합니다.

`StressTestRunner`는 테스트의 구체 타입을 모른 채 `IStressTest` 인터페이스만 사용합니다. 따라서 실행 시간 측정, CSV 기록, 콘솔 출력, 테스트 간 대기 같은 공통 흐름은 한 곳에 모이고, 각 테스트 클래스는 자신의 알고리즘에만 집중합니다.

이 구조는 포트폴리오 관점에서 다음 역량을 보여주기 위해 적용했습니다.

- 중복된 절차 코드에서 공통 실행 파이프라인을 분리하는 리팩터링 능력
- 인터페이스 기반 설계와 확장 가능한 객체 구성
- Windows 콘솔 도구에서 발생하는 인코딩, 출력, 파일 시스템 이슈 대응
- 저수준 리소스 사용 테스트에서 RAII 기반 안정성 개선

---

## Stress Scenarios

총 11개 시나리오를 순서대로 실행합니다.

| Category | Test | Purpose |
|---|---|---|
| CPU | Single Array Math | 대용량 배열에 `sqrt`, `sin` 계산을 수행 |
| CPU | Parallel Array Math | 동일한 배열 연산 작업을 여러 worker에서 병렬 반복 |
| CPU | Single Recursive | 재귀 피보나치 계산으로 CPU 부하 생성 |
| CPU | Parallel Recursive | 재귀 피보나치 계산을 worker별로 병렬 실행 |
| CPU | Single Prime | 지정 범위에서 소수 개수 계산 |
| CPU | Parallel Prime | 범위를 나눠 소수 개수를 병렬 계산 |
| CPU | Single Sort | 랜덤 배열 단일 정렬 |
| CPU | Parallel Sort | 배열을 청크로 나눠 정렬 후 병합 |
| CPU | Single Mandelbrot | Mandelbrot 영역 단일 계산 |
| CPU | Parallel Mandelbrot | x축 범위를 나눠 Mandelbrot 병렬 계산 |
| Memory | Memory Stress | 물리 메모리 기준 목표량까지 할당하고 페이지 접근 |

---

## Output

실행 결과는 바탕화면 아래 timestamp 폴더에 저장됩니다.

```text
Desktop/StressTestResult/YYYYMMDD_HHMMSS/
```

각 테스트는 별도 CSV 파일로 기록됩니다.

```text
SinglePrime.csv
ParallelPrime.csv
Memory.csv
...
```

CSV에는 테스트 제목, 실행 시간, 결과 요약이 기록됩니다. Windows/Excel에서 한글이 깨지지 않도록 새 CSV 파일에는 UTF-8 BOM을 기록합니다.

예시:

```text
"[단일 처리] 소수 찾기"
5321.42 ms,결과 : 소수점 검색 범위 : 107374180, 소수점 개수 : 6190114
```

---

## Build & Run

### Build

Visual Studio 2022에서 솔루션을 열어 빌드할 수 있습니다.

```text
CPUMemoryStressTestCpp/CPUMemoryStressTestCpp.sln
```

또는 MSBuild로 빌드할 수 있습니다.

```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" `
  .\CPUMemoryStressTestCpp\CPUMemoryStressTestCpp.sln `
  /p:Configuration=Release `
  /p:Platform=x64
```

### Run

1. 프로그램을 실행합니다.
2. 스트레스 테스트 반복 횟수 `1~10`을 입력합니다.
3. 등록된 11개 테스트가 순서대로 실행됩니다.
4. 결과 CSV가 `Desktop/StressTestResult/YYYYMMDD_HHMMSS/`에 저장됩니다.

> 일부 테스트는 CPU와 메모리를 강하게 사용합니다. 다른 작업을 저장한 뒤 실행하는 것을 권장합니다.

---

## Repository Structure

```text
CPUMemoryStressTestCpp/       중심 구현 C++ 버전
CPUMemoryStressTestCSharp/    초기 프로토타입 C# 버전
README.md                     프로젝트 소개와 실행 방법
LICENSE                       MIT License
.gitignore                    Visual Studio / 빌드 산출물 제외 설정
.gitattributes                Windows 줄바꿈 정책
```

---

## What I Focused On

이 프로젝트에서 특히 신경 쓴 부분은 다음과 같습니다.

- 테스트 로직과 실행/측정/기록 로직을 분리해 유지보수성을 높이는 것
- 병렬 처리 방식이 작업의 성격에 따라 달라질 수 있음을 코드 구조로 드러내는 것
- 메모리 부하 테스트처럼 위험할 수 있는 영역에서 RAII로 정리 책임을 명확히 하는 것
- 포트폴리오 프로젝트라도 실제 실행 환경에서 마주치는 인코딩, CSV, 줄바꿈 문제를 놓치지 않는 것

---

## Future Improvements

- 테스트 선택 실행 옵션 추가
- 반복 횟수, 메모리 목표 비율, Mandelbrot 해상도 같은 설정값 외부화
- 전체 실행 요약 CSV 추가
- 실행 중 CPU/메모리 사용량 샘플링 추가
- 단위 테스트 또는 작은 입력 기반 검증 모드 추가
