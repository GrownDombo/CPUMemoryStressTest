using System.Collections.Concurrent;
using System.Diagnostics;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Text;
using System.Runtime.InteropServices;

#nullable disable
internal class CPUStressTest
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
    private class MEMORYSTATUSEX
    {
        public uint dwLength;
        public uint dwMemoryLoad;
        public ulong ullTotalPhys;
        public ulong ullAvailPhys;
        public ulong ullTotalPageFile;
        public ulong ullAvailPageFile;
        public ulong ullTotalVirtual;
        public ulong ullAvailVirtual;
        public ulong ullAvailExtendedVirtual;
        public MEMORYSTATUSEX()
        {
            dwLength = (uint)Marshal.SizeOf(typeof(MEMORYSTATUSEX));
        }
    }
    [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    private static extern bool GlobalMemoryStatusEx([In, Out] MEMORYSTATUSEX lpBuffer);
    [DllImport("psapi.dll")]
    static extern int EmptyWorkingSet(IntPtr hProcess);


    private const int FibonacciDepth = 45;
    private const int PrimeStressRange = (int.MaxValue / 20) - 2;
    private const int MandelbrotWidth_Single = 2000;
    private const int MandelbrotHeight_Single = 2000;
    private const int MandelbrotMaxIterations_Single = 5000;
    private const int MandelbrotWidth_Parallel = 4000;
    private const int MandelbrotHeight_Parallel = 4000;
    private const int MandelbrotMaxIterations_Parallel = 10000;
    //private const int SortArrayLength = 2147483591; // 최대 배열 길이 (2GB - 1) 

    private static readonly string CsvDir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.Desktop), "StressTestResult");

    private static void Main(string[] args)
    {
        Console.OutputEncoding = Console.InputEncoding = Encoding.UTF8;
        Console.WriteLine("============================================");
        Console.WriteLine("        CPU & Memory Stress Test Tool       ");
        Console.WriteLine("============================================");
        Console.WriteLine();
        int repeat = PromptForRepeatCount();
        for (int i = 0; i < repeat; ++i)
        {
            Console.WriteLine($"\n===== [테스트 반복] {i + 1}/{repeat} =====");
            RunTests();
        }

        MoveCSV();
        Console.WriteLine("\n[완료] 모든 스트레스 테스트가 종료되었습니다.");
    }

    private static int PromptForRepeatCount()
    {
        while (true)
        {
            Console.Write("스트레스 테스트 반복 횟수 설정 (1~10) : ");
            if (int.TryParse(Console.ReadLine(), out int number) && number >= 1 && number <= 10)
                return number;
            Console.WriteLine("[오류] 잘못된 입력입니다. 1~10 사이 숫자를 입력하세요.\n");
        }
    }

    private static void RunTests()
    {
        Run(SingleArrayStressCPU);
        Run(ParallelArrayStressCPU);
        Run(SingleRecursiveStressCPU);
        Run(ParallelRecursiveStressCPU);
        Run(SinglePrimeStressCPU);
        Run(ParallelPrimeStressCPU);
        Run(SingleSortStressCPU);
        Run(ParallelSortStressCPU);
        Run(SingleMandelbrotStressCPU);
        Run(ParallelMandelbrotStressCPU);
        Run(StressMemory);
    }

    private static void Run(Action action)
    {
        action();
        PauseAndCollectGC();
    }

    private static void PauseAndCollectGC()
    {
        Console.WriteLine("\n[대기] 가비지 컬렉션 수행 및 5초간 휴식 중...");
        GC.Collect();
        GC.WaitForPendingFinalizers();
        GC.Collect(); // 두 번 호출은 LOH 포함 정리를 보장하기 위함
        Process p = Process.GetCurrentProcess();
        EmptyWorkingSet(p.Handle); // OS에 메모리 반납 단편화 그런거 공부해보자
        Thread.Sleep(5000);
        Console.WriteLine("[완료] 가비지 컬렉션 완료\n");
    }

    private static void MeasureAndLog(string fileName, string title, Func<string> work)
    {
        Console.WriteLine($"[시작] {title}");
        Stopwatch stopwatch = Stopwatch.StartNew();
        string result = work();
        stopwatch.Stop();
        WriteCSV(fileName, title, result, $"{stopwatch.Elapsed.TotalMilliseconds} ms");
        Console.WriteLine($"[종료] 소요 시간: {stopwatch.Elapsed.TotalMilliseconds:F2} ms (Result : {result})\n");
    }

    private static void WriteCSV(string fileName, string title, string sResult, string content)
    {
        Directory.CreateDirectory(CsvDir);
        string path = Path.Combine(CsvDir, $"{fileName}.csv");
        bool newFile = !File.Exists(path);
        using (StreamWriter writer = new StreamWriter(path, true, new UTF8Encoding(true)))
        {
            if (newFile)
                writer.WriteLine($"\"{title}\"");
            writer.WriteLine($"{content},결과 : {sResult}");
        }
    }
    private static void MoveCSV()
    {
        try
        {
            if (!Directory.Exists(CsvDir))
                return;

            string dest = Path.Combine(CsvDir, DateTime.Now.ToString("yyyyMMdd_HHmmss", CultureInfo.InvariantCulture));
            Directory.CreateDirectory(dest);

            foreach (string file in Directory.GetFiles(CsvDir, "*.csv"))
            {
                File.Move(file, Path.Combine(dest, Path.GetFileName(file)));
            }

            Console.WriteLine("[정보] CSV 결과 파일을 이동 완료했습니다.");
        }
        catch (Exception ex)
        {
            Console.WriteLine($"[오류] CSV 이동 실패 : {ex.Message}");
        }
    }

    private static void SingleArrayStressCPU() => MeasureAndLog("SingleArrayMath", "[단일 처리] 배열 수학 계산 (Math.Pow, Math.Sin)", () =>
    {
        int length = MaxArray<double>();
        double[] array = new double[length];
        for (int i = 0; i < length; ++i)
            array[i] = Math.Pow(i, 0.5) * Math.Sin(i);
        array = null;
        return $"배열 길이 : {length}";
    });

    private static void ParallelArrayStressCPU() => MeasureAndLog("ParallelArrayMath", "[병렬 처리] [같은 작업 동시 실행형] 배열 수학 계산 (Math.Pow, Math.Sin)", () =>
    {
        int processorCount = Environment.ProcessorCount;
        int length = MaxArray<double>();
        Parallel.For(0, processorCount, _ =>
        {
            double[] array = new double[length];
            for (int i = 0; i < length; ++i)
                array[i] = Math.Pow(i, 0.5) * Math.Sin(i);
            array = null;
        });
        return $"배열 길이 : {length}, 프로세서 개수 : {processorCount}";
    });

    private static void SingleRecursiveStressCPU() => MeasureAndLog("SingleRecursive", "[단일 처리] 재귀함수 피보나치 계산", () =>
    {
        double result = FibonacciRecursive(FibonacciDepth);
        return $"피보나치 수열 깊이 : {FibonacciDepth}";
    });

    private static void ParallelRecursiveStressCPU() => MeasureAndLog("ParallelRecursive", "[병렬 처리] [같은 작업 동시 실행형] 재귀함수 피보나치 계산", () =>
    {
        int processorCount = Environment.ProcessorCount;
        double[] results = new double[processorCount];
        Parallel.For(0, processorCount, i => results[i] = FibonacciRecursive(FibonacciDepth));
        results = null;
        return $"피보나치 수열 깊이 : {FibonacciDepth}, 프로세서 개수 : {processorCount}";
    });

    private static void SinglePrimeStressCPU() => MeasureAndLog("SinglePrime", "[단일 처리] 소수 찾기", () =>
    {
        int count = Enumerable.Range(2, PrimeStressRange).Count(IsPrime);
        return $"소수점 검색 범위 : {PrimeStressRange}, 소수점 개수 : {count}";
    });

    private static void ParallelPrimeStressCPU() => MeasureAndLog("ParallelPrime", "[병렬 처리] [작업 분담형] 소수 찾기", () =>
    {
        int count = 0;
        Parallel.ForEach(Partitioner.Create(2, PrimeStressRange), r =>
        {
            int localCount = 0;
            for (int i = r.Item1; i < r.Item2; i++)
                if (IsPrime(i))
                    localCount++;
            Interlocked.Add(ref count, localCount);
        });
        return $"소수점 검색 범위 : {PrimeStressRange}, 소수점 개수 : {count}";
    });

    private static void SingleSortStressCPU() => MeasureAndLog("SingleSort", "[단일 처리] 배열 정렬", () =>
    {
        int length = MaxArray<int>();
        int[] data = GenerateRandomArray(length);
        Array.Sort(data);
        data = null;
        return $"배열 길이 : {length}";
    });

    private static void ParallelSortStressCPU() => MeasureAndLog("ParallelSort", "[병렬 처리] [작업 분담형] 배열 정렬 및 병합", () =>
    {
        /*
         * [랜덤 데이터 생성]
         * [코어별로 나눠서 각 청크 정렬]
         * [정렬된 청크들끼리 MergeSortedChunks]
         * [완성된 정렬 배열]
         */
        int length = MaxArray<int>();
        int chunkSize = length / Environment.ProcessorCount;

        int[] data = GenerateRandomArray(length);
        List<int[]> sortedChunks = new List<int[]>();
        Parallel.For(0, Environment.ProcessorCount, i =>
        {
            int start = i * chunkSize;
            int end = (i == Environment.ProcessorCount - 1) ? length : (start + chunkSize);
            int[] chunk = new int[end - start];
            Array.Copy(data, start, chunk, 0, end - start);
            Array.Sort(chunk);
            lock (sortedChunks)
            {
                sortedChunks.Add(chunk);
            }
        });
        int[] merged = MergeSortedChunks(sortedChunks);
        data = null;
        sortedChunks = null;
        merged = null;
        return $"배열 길이 : {length}";
    });

    private static void SingleMandelbrotStressCPU() => MeasureAndLog("SingleMandelbrot", "[단일 처리] Mandelbrot 계산", () =>
    {
        CalculateMandelbrot(MandelbrotWidth_Single, MandelbrotHeight_Single, MandelbrotMaxIterations_Single, false);
        return $"{MandelbrotWidth_Single}x{MandelbrotHeight_Single}, Iterations: {MandelbrotMaxIterations_Single}";
    });

    private static void ParallelMandelbrotStressCPU() => MeasureAndLog("ParallelMandelbrot", "[병렬 처리] [작업 분담형] Mandelbrot 계산", () =>
    {
        CalculateMandelbrot(MandelbrotWidth_Parallel, MandelbrotHeight_Parallel, MandelbrotMaxIterations_Parallel, true);
        return $"{MandelbrotWidth_Parallel}x{MandelbrotHeight_Parallel}, Iterations: {MandelbrotMaxIterations_Parallel}";
    });

    private static void StressMemory() => MeasureAndLog("Memory", "[메모리 테스트]", () =>
    {
        Console.WriteLine("[시작] 메모리 스트레스 테스트");
        List<byte[]> allocations = new List<byte[]>();
        ulong totalPhysicalMemory;
        MEMORYSTATUSEX memStatus = new MEMORYSTATUSEX();
        if (GlobalMemoryStatusEx(memStatus))
            totalPhysicalMemory = memStatus.ullTotalPhys;
        else
        {
            Console.WriteLine("[경고] 메모리 정보를 가져오지 못했습니다. 기본 8GB 로 진행합니다.");
            totalPhysicalMemory = 8UL * 1024 * 1024 * 1024; // 기본 8GB
        }
        double targetMB = (totalPhysicalMemory / 1024.0 / 1024.0) * 0.8;
        Console.WriteLine($"[정보] 목표 메모리 사용량: {targetMB:F2} MB");
        try
        {
            while (GC.GetTotalMemory(false) / 1024.0 / 1024.0 < targetMB)
            {
                /* 메모리 할당만 하고 안 건드리면 시스템이 "게으른 할당(lazy allocation)"으로 RAM은 여유있는데 가상 메모리만 불어나고 실제 부하는 안 걸린다
                 * 컴퓨터 시스템의 메모리 관리 기본 단위는 4KB (1 페이지, 1 Page)
                 * 운영체제(OS)는 메모리를 "4KB 단위"로 관리
                 * 프로그램이 배열만 만들고 아무것도 안 쓰면, 운영체제는 "진짜 물리 RAM"을 할당하지 않고 가상 주소만 잡아놈
                 * 즉, new byte[1024 * 1024] 만 하면 실제 RAM을 쓰지 않을 수도 있어. (가짜 할당 Virtual Memory)
                 * 그런데 4KB마다 실제로 접근(Write) 하면 운영체제가 "아, 진짜 쓰는구나" 함
                 * 페이지 폴트(Page Fault) 를 일으키고
                 * 물리 메모리(RAM)를 강제로 연결(Commit) 해준다.
                 * 4KB마다 건드려야 진짜 물리 메모리를 꽉 채울 수 있다.
                 */
                byte[] block = new byte[1024 * 1024]; // 1MB
                for (int i = 0; i < block.Length; i += 4096) // 4KB마다 터치
                    block[i] = (byte)(i % 256);
                allocations.Add(block);
            }
        }
        catch (OutOfMemoryException)
        {
            Console.WriteLine("[메모리] 메모리 부족");
        }
        double usedMemory = GC.GetTotalMemory(false) / 1024.0 / 1024.0;
        Console.WriteLine("[종료] 메모리 스트레스 테스트 완료\n");
        allocations = null;
        return $"전체 메모리 : {totalPhysicalMemory}, 목표 메모리 사용량 : {targetMB}, 실제 메모리 사용량 : {usedMemory}";
    });

    private static double FibonacciRecursive(int n)
    {
        if (n <= 1)
            return n;
        return FibonacciRecursive(n - 1) + FibonacciRecursive(n - 2);
    }

    private static bool IsPrime(int number)
    {
        if (number < 2)
            return false;
        if (number == 2)
            return true;
        if (number % 2 == 0)
            return false;
        int boundary = (int)Math.Sqrt(number);
        for (int i = 3; i <= boundary; i += 2)
        {
            if (number % i == 0)
                return false;
        }
        return true;
    }

    private static int MaxArray<T>()
    {
        const int MaxByteLength = 0x7FFFFFC7;
        int elementSize = typeof(T).IsValueType ? Unsafe.SizeOf<T>() : IntPtr.Size;
        return MaxByteLength / elementSize;
    }

    private static int[] GenerateRandomArray(int length)
    {
        Random rand = new Random(42);
        return Enumerable.Range(0, length).Select(_ => rand.Next()).ToArray();
    }

    private static int[] MergeSortedChunks(List<int[]> chunks)
    {
        Queue<int[]> queue = new Queue<int[]>(chunks);
        while (queue.Count > 1)
        {
            int[] a = queue.Dequeue();
            int[] b = queue.Dequeue();
            queue.Enqueue(MergeTwoArrays(a, b));
        }
        return queue.Dequeue();
    }

    private static int[] MergeTwoArrays(int[] a, int[] b)
    {
        int[] result = new int[a.Length + b.Length];
        int i = 0, j = 0, k = 0;
        while (i < a.Length && j < b.Length)
        {
            result[k++] = (a[i] <= b[j]) ? a[i++] : b[j++];
        }
        while (i < a.Length)
            result[k++] = a[i++];
        while (j < b.Length)
            result[k++] = b[j++];
        return result;
    }

    private static void CalculateMandelbrot(int width, int height, int maxIterations, bool parallel)
    {
        double xmin = -2.1, xmax = 1.0, ymin = -1.3, ymax = 1.3;
        double xstep = (xmax - xmin) / width;
        double ystep = (ymax - ymin) / height;

        Action<int> process = px =>
        {
            for (int py = 0; py < height; py++)
            {
                double x = xmin + px * xstep;
                double y = ymin + py * ystep;
                double zx = 0, zy = 0;
                for (int i = 0; zx * zx + zy * zy <= 4.0 && i < maxIterations; i++)
                {
                    double temp = zx * zx - zy * zy + x;
                    zy = 2.0 * zx * zy + y;
                    zx = temp;
                }
            }
        };

        if (parallel)
            Parallel.For(0, width, process);
        else
            for (int px = 0; px < width; px++)
                process(px);
    }
}