#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <windows.h>
#include <psapi.h>
#include <shlobj.h>
#include <sstream>
#include <iomanip>
#include <format> 
#include <mutex>
#include <random>
#include <queue>

using namespace std;

// ====================== 전역 변수, 상수 선언 ======================
string GetEnv(const char* name);
const string& GetCsvDir();
template<typename T>
int MaxArray() {
    const size_t MaxByteLength = 0x7FFFFFC7;
    return static_cast<int>(MaxByteLength / sizeof(T));
}
// 유틸 함수: 랜덤 배열 생성
int* GenerateRandomArray(int length) {
    int* data = new int[length];
    mt19937 rng(42);
    uniform_int_distribution<int> dist(0, INT32_MAX);
    for (int i = 0; i < length; ++i)
        data[i] = dist(rng);
    return data;
}
const int depth = 45;
const int PrimeStressRange = (INT_MAX / 20) - 2;
const int MandelbrotWidth_Single = 2000;
const int MandelbrotHeight_Single = 2000;
const int MandelbrotMaxIterations_Single = 5000;
const int MandelbrotWidth_Parallel = 4000;
const int MandelbrotHeight_Parallel = 4000;
const int MandelbrotMaxIterations_Parallel = 10000;

// ====================== 함수 선언 ======================
int PromptRepeatCount();
void RunCycle();
void Run(
    void (*action)(const char* fileName, const char* title, const char* (*workFunc)()),
    const char* fileName,
    const char* title,
    const char* (*workFunc)());

void PauseAndCollectGC();
void MeasureAndLog(const char* fileName, const char* title, const char* (*workFunc)());

void WriteCSV(const string& fileName, const string& title, const string& result, double durationMs);
void CreateCSVDir(const string& dirName);

double FibonacciRecursive(int n);
bool IsPrime(int number);
int* MergeTwoArrays(int* a, int sizeA, int* b, int sizeB);
int* MergeSortedChunks(vector<int*>& chunks);
void CalculateMandelbrot(int width, int height, int maxIterations, bool parallel);

// ====================== 메모리 테스트 선언 ======================
const char* SingleArrayStressCPU();
const char* ParallelArrayStressCPU();
const char* SingleRecursiveStressCPU();
const char* ParallelRecursiveStressCPU();
const char* SinglePrimeStressCPU();
const char* ParallelPrimeStressCPU();
const char* SingleSortStressCPU();
const char* ParallelSortStressCPU();
const char* SingleMandelbrotStressCPU();
const char* ParallelMandelbrotStressCPU();
const char* StressMemory();

// ====================== main ======================
int main() {
    cout << "============================================" << endl;
    cout << "       CPU & Memory Stress Test Tool        " << endl;
    cout << "============================================" << endl;

    int repeat = PromptRepeatCount();
    cout << "결과 파일 경로 설정 : " << GetCsvDir() << endl << endl;
    for (int i = 0; i < repeat; ++i) {
        cout << "===== [테스트 반복] " << (i + 1) << "/" << repeat << " =====" << endl;
        RunCycle();
    }
    cout << "[완료] 모든 스트레스 테스트가 종료되었습니다." << endl;
    return 0;
}

// ====================== 전역함수 정의 ======================
string GetEnv(const char* name) {
    char* buffer = nullptr;
    size_t size = 0;
    if (_dupenv_s(&buffer, &size, name) == 0 && buffer != nullptr) {
        string result(buffer);
        free(buffer);
        return result;
    }
    return "";
}

const string& GetCsvDir() {
    static string fullPath;
    if (!fullPath.empty())
        return fullPath;

    char desktopPath[MAX_PATH];
    string baseDir;
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath))) {
        baseDir = format("{}\\StressTestResult", desktopPath);
    }
    else {
        baseDir = format("{}\\Desktop\\StressTestResult", GetEnv("USERPROFILE"));
    }

    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    tm localTime;
    localtime_s(&localTime, &t);

    stringstream ss;
    ss << put_time(&localTime, "%Y%m%d_%H%M%S");

    fullPath = format("{}\\{}", baseDir, ss.str());
    filesystem::create_directories(fullPath);
    return fullPath;
}

int PromptRepeatCount() {
    int num = 0;
    while (true) {
        cout << "스트레스 테스트 반복 횟수 설정 (1~10) : ";
        cin >> num;
        if (num >= 1 && num <= 10)
            return num;
        cout << "[오류] 잘못된 입력입니다. 1~10 사이 숫자를 입력하세요.\n" << endl;
    }
}

void RunCycle() {
    Run(MeasureAndLog, "SingleArrayMath", "[단일 처리] 배열 수학 계산 (Math.Pow, Math.Sin)", SingleArrayStressCPU);
    Run(MeasureAndLog, "ParallelArrayMath", "[병렬 처리] [같은 작업 동시 실행형] 배열 수학 계산 (Math.Pow, Math.Sin)", ParallelArrayStressCPU);
    Run(MeasureAndLog, "SingleRecursive", "[단일 처리] 재귀 피보나치", SingleRecursiveStressCPU);
    Run(MeasureAndLog, "ParallelRecursive", "[병렬 처리] [같은 작업 동시 실행형] 재귀함수 피보나치 계산", ParallelRecursiveStressCPU);
    Run(MeasureAndLog, "SinglePrime", "[단일 처리] 소수 찾기", SinglePrimeStressCPU);
    Run(MeasureAndLog, "ParallelPrime", "[병렬 처리] [작업 분담형] 소수 찾기", ParallelPrimeStressCPU);
    Run(MeasureAndLog, "SingleSort", "[단일 처리] 배열 정렬", SingleSortStressCPU);
    Run(MeasureAndLog, "ParallelSort", "[병렬 처리] [작업 분담형] 배열 정렬 및 병합", ParallelSortStressCPU);
    Run(MeasureAndLog, "SingleMandelbrot", "[단일 처리] Mandelbrot 계산", SingleMandelbrotStressCPU);
    Run(MeasureAndLog, "ParallelMandelbrot", "[병렬 처리] [작업 분담형] Mandelbrot 계산", ParallelMandelbrotStressCPU);
    Run(MeasureAndLog, "Memory", "[메모리 테스트]", StressMemory);
}

void Run(
    void (*action)(const char* fileName, const char* title, const char* (*workFunc)()),
    const char* fileName,
    const char* title,
    const char* (*workFunc)())
{
    action(fileName, title, workFunc);
    PauseAndCollectGC();
}

void PauseAndCollectGC() {
    cout << "[대기] 5초간 대기 중..." << endl;
    this_thread::sleep_for(chrono::seconds(5));
    HANDLE hProcess = GetCurrentProcess();
    SetProcessWorkingSetSize(hProcess, -1, -1);
    cout << "[완료] 대기 및 메모리 정리 완료" << endl << endl;
}

void MeasureAndLog(const char* fileName, const char* title, const char* (*workFunc)()) {
    cout << "[시작] " << title << endl;
    chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
    const char* result = workFunc();
    chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
    double elapsed = chrono::duration<double, milli>(end - start).count();

    WriteCSV(fileName, title, result, elapsed);
    cout << "[정보] " << result << endl;
    cout << "[결과] " << "소요 시간 : " << fixed << setprecision(2) << elapsed << " ms" << endl;
    cout << "[종료] " << title << endl << endl;
}

void WriteCSV(const string& fileName, const string& title, const string& result, double durationMs) {
    string dir = GetCsvDir();
    CreateCSVDir(dir);
    string fullPath = format("{}\\{}.csv", dir, fileName);

    ofstream file(fullPath, ios::app);
    if (file.tellp() == 0) {
        file << "\"" << title << "\"" << endl;
    }
    file << format("{:.2f} ms,결과 : {}\n", durationMs, result);
    file.close();
}

void CreateCSVDir(const string& dirName) {
    filesystem::create_directories(dirName);
}

// 스트레스 테스트 함수들
double FibonacciRecursive(int n) {
    if (n <= 1) return n;
    return FibonacciRecursive(n - 1) + FibonacciRecursive(n - 2);
}
bool IsPrime(int number) {
    if (number < 2) return false;
    if (number == 2) return true;
    if (number % 2 == 0) return false;
    int boundary = static_cast<int>(sqrt(number));
    for (int i = 3; i <= boundary; i += 2) {
        if (number % i == 0)
            return false;
    }
    return true;
}
void CalculateMandelbrot(int width, int height, int maxIterations, bool parallel)
{
    const double xmin = -2.1, xmax = 1.0;
    const double ymin = -1.3, ymax = 1.3;
    const double xstep = (xmax - xmin) / width;
    const double ystep = (ymax - ymin) / height;

    auto process = [&](int px) {
        for (int py = 0; py < height; py++) {
            double x = xmin + px * xstep;
            double y = ymin + py * ystep;
            double zx = 0, zy = 0;
            for (int i = 0; zx * zx + zy * zy <= 4.0 && i < maxIterations; i++) {
                double temp = zx * zx - zy * zy + x;
                zy = 2.0 * zx * zy + y;
                zx = temp;
            }
        }
        };

    if (parallel) {
        vector<thread> threads;
        for (int px = 0; px < width; ++px)
            threads.emplace_back(process, px);
        for (auto& t : threads)
            t.join();
    }
    else {
        for (int px = 0; px < width; ++px)
            process(px);
    }
}

// 두 배열 병합
int* MergeTwoArrays(int* a, int sizeA, int* b, int sizeB) {
    int* result = new int[sizeA + sizeB];
    int i = 0, j = 0, k = 0;
    while (i < sizeA && j < sizeB)
        result[k++] = (a[i] <= b[j]) ? a[i++] : b[j++];
    while (i < sizeA) result[k++] = a[i++];
    while (j < sizeB) result[k++] = b[j++];
    return result;
}

// 정렬된 청크 병합
int* MergeSortedChunks(vector<int*>& chunks) {
    queue<pair<int*, int>> q;
    for (int* chunk : chunks)
        q.emplace(chunk, _msize(chunk) / sizeof(int));  // _msize는 MSVC 전용 (chunk 크기 추정용)

    while (q.size() > 1) {
        pair<int*, int> a_pair = q.front(); q.pop();
        pair<int*, int> b_pair = q.front(); q.pop();

        int* a = a_pair.first;
        int sizeA = a_pair.second;
        int* b = b_pair.first;
        int sizeB = b_pair.second;

        int* merged = MergeTwoArrays(a, sizeA, b, sizeB);
        q.emplace(merged, sizeA + sizeB);
    }
    return q.front().first;
}

const char* SingleArrayStressCPU() {
    static string result;
    int length = MaxArray<double>();
    double* array = new double[length];
    for (int i = 0; i < length; ++i)
        array[i] = pow(i, 0.5) * sin(i);
    delete[] array;

    result = format("배열 길이 : {}", length);
    return result.c_str();
}
const char* ParallelArrayStressCPU() {
    static string result;
    int processorCount = thread::hardware_concurrency();
    int length = MaxArray<double>();

    vector<thread> threads;
    for (int i = 0; i < processorCount; ++i) {
        threads.emplace_back([length]() {
            double* array = new double[length];
            for (int j = 0; j < length; ++j)
                array[j] = pow(j, 0.5) * sin(j);
            delete[] array;
            });
    }
    for (thread& t : threads)
        t.join();

    result = format("배열 길이 : {}, 프로세서 개수 : {}", length, processorCount);
    return result.c_str();
}
const char* SingleRecursiveStressCPU() {
    static string result;

    double value = FibonacciRecursive(depth);
    result = format("피보나치 수열 깊이 : {}", depth);
    return result.c_str();
}
const char* ParallelRecursiveStressCPU() {
    static string result;
    int processorCount = thread::hardware_concurrency();
    vector<thread> threads(processorCount);
    vector<double> results(processorCount);

    for (int i = 0; i < processorCount; ++i) {
        threads[i] = thread([i, &results]() {
            results[i] = FibonacciRecursive(depth);
            });
    }

    for (thread& t : threads)
        t.join();

    result = format("피보나치 수열 깊이 : {}, 프로세서 개수 : {}", depth, processorCount);
    return result.c_str();
}
const char* SinglePrimeStressCPU() {
    static string result;
    int count = 0;
    for (int i = 2; i < PrimeStressRange; ++i) {
        if (IsPrime(i))
            count++;
    }

    result = format("소수점 검색 범위 : {}, 소수점 개수 : {}", PrimeStressRange, count);
    return result.c_str();
}
const char* ParallelPrimeStressCPU() {
    static string result;
    int processorCount = thread::hardware_concurrency();
    int totalCount = 0;
    mutex countMutex; // totalCount 더할때 다른스레드에서 혹시 건들이면 안되니까 Lock 거는거

    vector<thread> threads;
    int chunkSize = PrimeStressRange / processorCount;

    for (int i = 0; i < processorCount; ++i) {
        int start = 2 + i * chunkSize;
        int end = (i == processorCount - 1) ? PrimeStressRange : start + chunkSize;

        threads.emplace_back([start, end, &totalCount, &countMutex]() {
            int localCount = 0;
            for (int j = start; j < end; ++j)
                if (IsPrime(j)) localCount++;

            lock_guard<mutex> lock(countMutex);
            totalCount += localCount;
            });
    }

    for (thread& t : threads)
        t.join();

    result = format("소수점 검색 범위 : {}, 소수점 개수 : {}", PrimeStressRange, totalCount);
    return result.c_str();
}

const char* SingleSortStressCPU() {
    static string result;
    int length = min(MaxArray<int>(), 10000000);

    int* data = GenerateRandomArray(length);
    sort(data, data + length);
    delete[] data;
    result = format("배열 길이 : {}", length);
    return result.c_str();
}
const char* ParallelSortStressCPU() {
    static string result;
    int length = min(MaxArray<int>(), 10000000);
    int processorCount = thread::hardware_concurrency();
    int chunkSize = length / processorCount;

    int* data = GenerateRandomArray(length);
    vector<int*> sortedChunks;
    mutex mtx;// totalCount 더할때 다른스레드에서 혹시 건들이면 안되니까 Lock 거는거

    vector<thread> threads;
    for (int i = 0; i < processorCount; ++i) {
        threads.emplace_back([&, i]() {
            int start = i * chunkSize;
            int end = (i == processorCount - 1) ? length : (start + chunkSize);
            int* chunk = new int[end - start];
            copy(data + start, data + end, chunk);
            sort(chunk, chunk + (end - start));
            lock_guard<mutex> lock(mtx);
            sortedChunks.push_back(chunk);
            });
    }
    for (thread& t : threads)
        t.join();
    delete[] data;

    int* merged = MergeSortedChunks(sortedChunks);
    for (int* chunk : sortedChunks)
        delete[] chunk;
    delete[] merged;

    result = format("배열 길이 : {}", length);
    return result.c_str();
}

const char* SingleMandelbrotStressCPU() {
    static string result;
    CalculateMandelbrot(MandelbrotWidth_Single, MandelbrotHeight_Single, MandelbrotMaxIterations_Single, false);
    result = format("{}x{}, Iterations: {}", MandelbrotWidth_Single, MandelbrotHeight_Single, MandelbrotMaxIterations_Single);
    return result.c_str();
}

const char* ParallelMandelbrotStressCPU() {
    static string result;
    CalculateMandelbrot(MandelbrotWidth_Parallel, MandelbrotHeight_Parallel, MandelbrotMaxIterations_Parallel, true);
    result = format("{}x{}, Iterations: {}", MandelbrotWidth_Parallel, MandelbrotHeight_Parallel, MandelbrotMaxIterations_Parallel);
    return result.c_str();
}
const char* StressMemory() {
    static string result;
    cout << "[시작] 메모리 스트레스 테스트" << endl;

    // 1. 메모리 정보 조회
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(memStatus);
    ULONGLONG totalPhysicalMemory = 0;
    if (GlobalMemoryStatusEx(&memStatus)) {
        totalPhysicalMemory = memStatus.ullTotalPhys;
    }
    else {
        cerr << "[경고] 메모리 정보를 가져오지 못했습니다. 기본 8GB 로 진행합니다." << endl;
        totalPhysicalMemory = 8ULL * 1024 * 1024 * 1024; // 8GB fallback
    }

    // 2. 목표 메모리 사용량 (80%)
    double targetMB = (totalPhysicalMemory / 1024.0 / 1024.0) * 0.8;
    cout << format("[정보] 목표 메모리 사용량: {:.2f} MB", targetMB) << endl;

    // 3. 메모리 할당 및 실제 접근
    vector<char*> allocations;
    size_t totalAllocatedMB = 0;

    try {
        while (true) {
            char* block = new(nothrow) char[1024 * 1024]; // 1MB
            if (!block) throw bad_alloc();

            // 실제로 4KB마다 접근해서 페이지 커밋 유도
            for (int i = 0; i < 1024 * 1024; i += 4096)
                block[i] = static_cast<char>(i % 256);

            allocations.push_back(block);
            totalAllocatedMB++;

            if (totalAllocatedMB >= targetMB)
                break;
        }
    }
    catch (const bad_alloc&) {
        cout << "[메모리] 메모리 부족" << endl;
    }

    cout << "[종료] 메모리 스트레스 테스트 완료" << endl;

    result = format("전체 메모리 : {}, 목표 메모리 사용량 : {:.2f} MB, 실제 메모리 사용량 : {} MB", totalPhysicalMemory, targetMB, totalAllocatedMB);

    // 정리
    for (char* p : allocations)
        delete[] p;
    allocations.clear();

    return result.c_str();
}


