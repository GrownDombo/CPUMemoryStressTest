# CPUMemoryStressTest

CPU와 메모리 자원에 강한 부하를 발생시켜, **단일 처리와 병렬 처리의 차이**, **알고리즘별 부하 특성**, **메모리 사용 패턴**을 비교할 수 있도록 만든 C++ 기반 콘솔 스트레스 테스트 도구입니다.

> **Why this project?**  
> 개발 및 테스트 환경에서 시스템 자원 사용량을 의도적으로 높여야 하는 상황이 있었고,  
> 단순 벤치마크보다 **직접 제어 가능한 부하 테스트 도구**가 필요했습니다.  
> 처음에는 C#으로 프로토타입을 만들었지만, 메모리 해제 타이밍과 실행 흐름을 더 명확하게 통제하기 위해 C++ 버전으로 다시 구현했습니다.

---

## Overview

이 프로젝트는 CPU 및 메모리에 강한 부하를 주는 여러 시나리오를 통해, 테스트 환경에서 시스템 반응과 처리 성능을 비교·기록할 수 있도록 구성한 도구입니다.

핵심 목적은 다음과 같습니다.

- 단일 처리와 병렬 처리의 성능 차이 비교
- 알고리즘별 부하 특성 확인
- 대량 메모리 할당과 접근 패턴 테스트
- 반복 실행 가능한 콘솔 기반 테스트 도구 구현
- CSV 기반 결과 기록 자동화

---

## Tech Stack

- **Language**: C++
- **IDE / Toolchain**: Visual Studio 2022
- **Platform**: Windows
- **API / Library**
  - WinAPI (`windows.h`, `psapi.h`, `shlobj.h`)
  - STL (`thread`, `chrono`, `vector`, `queue`, `mutex`, `fstream`)

---

## Test Scenarios

총 11개의 테스트를 순차적으로 실행하며, 일부 항목은 단일 처리와 병렬 처리를 비교할 수 있도록 구성했습니다.

| Category | Test | Description |
|---|---|---|
| CPU | Single Array Math | `pow`, `sin` 기반 대규모 배열 수학 연산 |
| CPU | Parallel Array Math | 동일한 배열 수학 연산을 멀티스레드로 동시 수행 |
| CPU | Single Recursive | 재귀 피보나치 계산 |
| CPU | Parallel Recursive | 재귀 피보나치 계산의 병렬 수행 |
| CPU | Single Prime | 대규모 범위의 소수 탐색 |
| CPU | Parallel Prime | 범위를 분할해 병렬 소수 탐색 |
| CPU | Single Sort | 대량 랜덤 배열 정렬 |
| CPU | Parallel Sort | 배열 분할 정렬 후 병합 |
| CPU | Single Mandelbrot | Mandelbrot 계산 |
| CPU | Parallel Mandelbrot | 작업 분담형 Mandelbrot 계산 |
| Memory | Memory Stress | 물리 메모리의 일정 비율까지 할당 및 접근 |

---

## Key Design Points

### 1. Unified execution flow
각 테스트는 공통 흐름으로 실행됩니다.

1. 테스트 시작
2. 작업 수행
3. 시간 측정
4. CSV 기록
5. 테스트 간 대기 및 메모리 정리

이 구조를 통해 테스트 로직과 측정/로깅 로직을 분리했고, 새로운 시나리오를 추가하기 쉽게 만들었습니다.

### 2. Single vs Parallel comparison
같은 작업을 단일 처리와 병렬 처리로 나누어 실행해, 멀티스레딩 적용 방식에 따른 차이를 비교할 수 있도록 했습니다.

- 동일 작업 반복형 병렬화
- 범위/데이터 분할형 병렬화

### 3. Stress-oriented workload selection
단순 반복 루프가 아니라, 부하 특성이 다른 작업들로 시나리오를 구성했습니다.

- 수학 연산
- 재귀 연산
- 소수 판별
- 정렬 및 병합
- Mandelbrot 계산
- 대량 메모리 할당

### 4. Result logging
모든 테스트 결과를 CSV로 남기도록 구성해, 반복 실행 시 결과 누적과 비교가 가능하도록 했습니다.

---

## Output

실행 결과는 데스크톱 하위의 실행 시각 기준 폴더에 저장됩니다.

```text
Desktop/StressTestResult/YYYYMMDD_HHMMSS/
```

각 테스트는 개별 CSV 파일로 기록되며, 다음 정보를 포함합니다.

- 테스트 이름
- 수행 시간(ms)
- 간단한 결과 정보

예시:

```text
"단일 처리 배열 정렬"
1532.41 ms, 결과 : 배열 길이 : 10000000
```

---

## Program Flow

```text
1. 반복 횟수 입력
2. 테스트 시나리오 순차 실행
3. 각 테스트마다 시간 측정 및 결과 수집
4. CSV 기록
5. 다음 테스트 전 대기 및 메모리 정리
```

---


## How to Run

### Build
1. Visual Studio 2022에서 솔루션을 엽니다.
2. 빌드 구성을 `Debug` 또는 `Release`로 선택합니다.
3. 콘솔 프로젝트를 빌드한 뒤 실행합니다.

### Run
1. 프로그램 실행 후 테스트 반복 횟수를 입력합니다.
2. 각 스트레스 테스트가 순차적으로 실행됩니다.
3. 실행 결과는 출력 폴더에 CSV 파일로 저장됩니다.

## Notes

- 일부 테스트는 CPU와 메모리를 강하게 사용하므로 실행 중 시스템이 일시적으로 느려질 수 있습니다.
- 메모리 스트레스 테스트는 실제 메모리 할당과 접근을 수행하므로, 여유 자원이 있는 환경에서 실행하는 것을 권장합니다.
- 실행 시간과 결과는 시스템 사양 및 백그라운드 프로세스 상태에 따라 달라질 수 있습니다.

