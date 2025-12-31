#pragma once
double GetTime()
{
    static LARGE_INTEGER frequency;
    static LARGE_INTEGER start;

    static bool initialized = false;
    if (!initialized)
    {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start);
        initialized = true;
    }

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    return double(now.QuadPart - start.QuadPart) / double(frequency.QuadPart);
}
