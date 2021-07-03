// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <immintrin.h>
#include <intrin.h>
#include <fstream>
extern "C" void asm_pg_KiErrata361Present();
extern "C" bool asm_pg_single_step();
/*
*  https://revers.engineering/
*  https://revers.engineering/detecting-hypervisor-presence-on-windows-10/
*  https://key08.com/
*/
void write_to_file(std::string file_name, std::string file_context) {
    std::ofstream out_file(file_name);
    out_file << file_context;
    out_file.close();
}
struct _cpuid
{
    UINT data[4];
};
void check_by_time()
{
    unsigned __int64 old_priority = SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    unsigned __int64 rdtsc_first = __rdtsc();
    Sleep(2000);
    unsigned __int64 rdtsc_time_delta = __rdtsc() - rdtsc_first;
    DWORD64 result_time = 0;
    int cpuid_data[4] = { 0 };
    for (std::size_t count = 0; count < 0x6694; count++)
    {
        auto rdtsc_iter_time = __rdtsc();
        
        __cpuid((int*)cpuid_data, 0);

        result_time += __rdtsc() - rdtsc_iter_time;
    }
    unsigned __int64 _time = 10000000 * result_time / rdtsc_time_delta / 0x65;
    if (_time > 400 || _time < 10)
        printf("rdtsc detected !\n"),
        write_to_file("rdtsc.txt", "rdtsc detected!");
    else
        printf("rdtsc pass! \n");
    printf("time: %d \n", _time);
    SetThreadPriority(GetCurrentThread(), old_priority);
}

void check_by_invalid_cpuid()
{
    unsigned int invalid_leaf = 0x13371337;
    unsigned int valid_leaf = 0x40000000;

    auto fn_check = [](_cpuid a, _cpuid b)->bool {
        return a.data[0] != b.data[0] || a.data[1] != b.data[1] || a.data[2] != b.data[2] || a.data[3] != b.data[3];
    };
    _cpuid cpuid_first = { 0 };
    _cpuid cpuid_sec = { 0 };

    __cpuid((int*)&cpuid_first, invalid_leaf);
    __cpuid((int*)&cpuid_sec, valid_leaf);

    if (fn_check(cpuid_first, cpuid_sec))
    {
        printf("CPUID detected \n");
        write_to_file("cpuid.txt", "cpuid detected!");
        return;
    }
    printf("CPUID pass \n");
}
void check_by_pg() {
    printf("pg detecting... \n");
    write_to_file("start_pg_detect.txt", "start pg detect");
    asm_pg_KiErrata361Present();
    printf("pg detec pass \n");
    write_to_file("pg_detect_pass.txt", "pg detect pass");
}
void check_by_single_step() {

    write_to_file("start_single_detect.txt", "start_single_detect");
    if (asm_pg_single_step()) {
        printf("single_step pass \n");
    }
    else {
        printf("single_step detected \n");
        write_to_file("single_step.txt", "single_step detected");
    }
    write_to_file("single_detect_end.txt", "single_detect_end");
}
LONG CALLBACK veh_hanlde(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
    return ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_CONTINUE_SEARCH;
}
int main()
{
    AddVectoredExceptionHandler(true, veh_hanlde);
    check_by_pg();
    check_by_time();
    check_by_invalid_cpuid();
    check_by_single_step();
    system("pause");
    return 0;
}
