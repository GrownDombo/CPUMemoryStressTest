# CPUMemoryStressTest

Windows에서 CPU/Memory 부하 테스트를 실행하는 C++20 콘솔 도구입니다.

기본 실행은 사람이 쓰는 대화형 모드이고, 인자를 붙이면 AI나 스크립트가 쓰기 좋은 JSON 기반 CLI로 동작합니다. CSV 저장도 지원하지만, CLI에서는 명시적으로 요청했을 때만 파일을 만듭니다.

---

## 빠른 실행

```powershell
CPUMemoryStressTestCpp.exe list
CPUMemoryStressTestCpp.exe run memory --preset quick
CPUMemoryStressTestCpp.exe run all --preset quick --repeat 1
CPUMemoryStressTestCpp.exe shell
```

인자 없이 실행하면 기존 대화형 모드로 전체 테스트를 순서대로 실행합니다.

```powershell
CPUMemoryStressTestCpp.exe
```

---

## 주요 기능

| 기능 | 설명 |
|---|---|
| 선택 실행 | `run memory`, `run cpu.prime.parallel`처럼 원하는 테스트만 실행 |
| JSON 출력 | CLI 기본 출력은 stdout JSON |
| CSV 저장 | `--save-csv` 옵션을 줄 때만 CSV 저장 |
| Shell 모드 | `shell` 실행 후 `stress>` 프롬프트에서 반복 명령 |
| Preset | `quick`, `normal`, `heavy` 강도 선택, `extreme`은 shell/대화형에서만 허용 |
| 안전한 실패 처리 | 잘못된 인자는 error JSON과 exit code로 반환 |

---

## 명령어

```text
CPUMemoryStressTestCpp.exe list
CPUMemoryStressTestCpp.exe run <test-id|all> [--preset quick|normal|heavy] [--repeat 1..5] [--save-csv] [--csv-dir <path>]
CPUMemoryStressTestCpp.exe shell
CPUMemoryStressTestCpp.exe help
```

예시:

```powershell
CPUMemoryStressTestCpp.exe run cpu.prime.parallel --preset normal
CPUMemoryStressTestCpp.exe run memory --preset quick --save-csv
CPUMemoryStressTestCpp.exe run memory --preset quick --save-csv --csv-dir .\CliTestCsv
```

Shell 모드:

```text
stress> list
stress> run memory --preset quick
stress> run cpu.fibonacci.parallel --preset extreme
stress> exit
```

---

## Preset

| Preset | 용도 | 주의 |
|---|---|---|
| `quick` | 기능 확인, AI 호출 기본값 | 부담 적음 |
| `normal` | 일반적인 부하 확인 | 짧게 CPU/Memory 사용 |
| `heavy` | 강한 부하 확인 | 작업 저장 후 실행 권장 |
| `extreme` | 장시간 고부하 검증 | shell/대화형에서 확인 입력 후 실행 |

현재 설정:

| Preset | Workers | Array | Prime | Fibonacci | Mandelbrot | Memory |
|---|---:|---:|---:|---:|---|---|
| `quick` | 최대 2 | 100,000 | 10,000 | 30 | 200x160, 100 iter | 128 MB |
| `normal` | 최대 4 | 1,000,000 | 200,000 | 36 | 600x400, 500 iter | 512 MB |
| `heavy` | HW worker | 5,000,000 | 2,000,000 | 42 | 1200x800, 1500 iter | 2048 MB |
| `extreme` | HW worker | 50,000,000 | 100,000,000 | 45 | 2000x2000, 5000 iter | 물리 메모리 80% |

`extreme`은 일회성 CLI에서는 차단됩니다. `shell` 모드나 인자 없는 대화형 모드에서 확인 문구를 입력한 뒤 사용할 수 있습니다. `run all --preset extreme`은 전체 테스트를 모두 강한 설정으로 실행하므로 필요한 경우에만 사용하세요.

---

## 테스트 ID

| ID | 설명 |
|---|---|
| `cpu.array.single` | 단일 배열 수학 계산 |
| `cpu.array.parallel` | 병렬 배열 수학 계산 |
| `cpu.fibonacci.single` | 단일 재귀 피보나치 |
| `cpu.fibonacci.parallel` | 병렬 재귀 피보나치 |
| `cpu.prime.single` | 단일 소수 탐색 |
| `cpu.prime.parallel` | 병렬 소수 탐색 |
| `cpu.sort.single` | 단일 배열 정렬 |
| `cpu.sort.parallel` | 병렬 배열 정렬 및 병합 |
| `cpu.mandelbrot.single` | 단일 Mandelbrot 계산 |
| `cpu.mandelbrot.parallel` | 병렬 Mandelbrot 계산 |
| `memory` | 메모리 할당 및 페이지 접근 |

---

## 출력

성공 예시:

```json
{
  "ok": true,
  "command": "run",
  "target": "memory",
  "preset": "quick",
  "results": [
    {
      "testId": "memory",
      "durationMs": 14.53,
      "success": true,
      "summary": "목표 메모리 사용량 : 128 MB, 실제 메모리 사용량 : 128 MB"
    }
  ]
}
```

실패 예시:

```json
{
  "ok": false,
  "error": {
    "code": "invalid_test_id",
    "message": "Unknown test id: cpu.foo"
  }
}
```

Exit code:

| Code | 의미 |
|---|---|
| `0` | 성공 |
| `2` | 잘못된 명령, 인자, test id, preset |
| `10` | 테스트 실행 중 실패 |

---

## 구조

```text
CPUMemoryStressTestCpp/
  CPUMemoryStressTestCpp/
    main.cpp                         - 프로그램 진입점, interactive/CLI/shell 분기
    Cli/
      CliCommand.h                   - CLI 명령 데이터(list/run/help/shell, 옵션 값)
      CliCommandExecutor.h/.cpp      - 파싱된 명령을 실제 실행 흐름으로 연결
      CliExitCodes.h                 - CLI exit code 상수
      CliParser.h/.cpp               - argv를 CliCommand로 변환하고 인자 검증
      InteractiveSession.h/.cpp      - 인자 없는 기존 대화형 전체 실행 모드
      ShellSession.h/.cpp            - stress> 프롬프트 기반 반복 명령 모드
    Interfaces/
      IStressTest.h                  - 모든 스트레스 테스트가 구현하는 Strategy 인터페이스
    Registry/
      TestRegistry.h/.cpp            - test-id 기반 테스트 등록, 목록 조회, 선택 실행 지원
    Results/
      PresetConfig.h                 - quick/normal/heavy/extreme 부하 크기 정의
      TestResult.h                   - 테스트 실행 결과(JSON/CSV 출력의 원천 데이터)
    Runner/
      StressTestRunner.h/.cpp        - 실행 시간 측정, 예외 처리, 메모리 정리 대기
    Output/
      JsonResultWriter.h/.cpp        - CLI stdout JSON 출력 writer
    Logging/
      CsvResultWriter.h/.cpp         - 선택적 CSV 결과 저장 writer
    Workloads/
      ArrayMathStressTest.h/.cpp     - sqrt/sin 배열 계산 부하
      FibonacciStressTest.h/.cpp     - 재귀 피보나치 CPU 부하
      PrimeStressTest.h/.cpp         - 소수 탐색 CPU 부하
      SortStressTest.h/.cpp          - 랜덤 배열 정렬/병합 부하
      MandelbrotStressTest.h/.cpp    - Mandelbrot 반복 계산 부하
      MemoryStressTest.h/.cpp        - 물리 메모리 할당과 페이지 접근 부하
    Utils/
      ConsoleEncoding.h/.cpp         - Windows 콘솔 UTF-8 입출력 설정
      Environment.h/.cpp             - 결과 폴더 생성과 환경 경로 조회
      StressAlgorithms.h/.cpp        - workload들이 공유하는 계산/보조 알고리즘
```

설계 흐름:

```text
CLI 입력 -> CliParser -> CliCommandExecutor -> TestRegistry -> IStressTest -> TestResult -> JsonResultWriter
```

---

## 사용한 패턴

| 패턴 | 적용 위치 | 이유 |
|---|---|---|
| Strategy | `IStressTest`, `Workloads/*` | 테스트 알고리즘을 공통 인터페이스 뒤로 숨기고 새 테스트를 쉽게 추가하기 위해 |
| Registry | `TestRegistry` | CLI의 test-id로 테스트를 찾고 `list` 명령을 제공하기 위해 |
| Writer/Adapter | `JsonResultWriter`, `CsvResultWriter` | 실행 로직과 출력 형식을 분리하기 위해 |
| Session | `InteractiveSession`, `ShellSession` | 대화형 모드와 shell 모드를 독립적인 실행 흐름으로 분리하기 위해 |

---

## 확인용 명령

```powershell
CPUMemoryStressTestCpp.exe list
CPUMemoryStressTestCpp.exe run memory --preset quick
CPUMemoryStressTestCpp.exe run all --preset quick --repeat 1
CPUMemoryStressTestCpp.exe run cpu.foo --preset quick
```

JSON 파싱 확인:

```powershell
$json = CPUMemoryStressTestCpp.exe run memory --preset quick
$json | ConvertFrom-Json
```

---

## Repository

```text
CPUMemoryStressTestCpp/       중심 구현 C++ 버전
CPUMemoryStressTestCSharp/    초기 프로토타입 C# 버전
README.md
LICENSE
.gitignore
```
