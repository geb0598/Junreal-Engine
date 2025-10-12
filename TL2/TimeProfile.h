﻿#pragma once
#include "UEContainer.h"
#include "winnt.h"
#include "profileapi.h"
#include "UI/GlobalConsole.h"


//#ifdef _DEVELOP //_DEVELOP 이 정의 되어 있을때만 측정
//#define TIME_PROFILE(Key)\
//FScopeCycleCounter Key##Counter(#Key); //현재 스코프 단위로 측정
//#else
//#define TIME_PROFILE(Key) //_DEVELOP 미정의시 빈칸
//#endif
//
//#ifdef _DEVELOP
//#define TIME_PROFILE_END(Key)\
//Key##Counter.Finish();
//#else
//#define TIME_PROFILE_END(Key)
//#endif


#define TIME_PROFILE(Key)\
FScopeCycleCounter Key##Counter(#Key); //현재 스코프 단위로 측정

#define TIME_PROFILE_END(Key)\
Key##Counter.Finish();


class FWindowsPlatformTime
{
public:
	static double GSecondsPerCycle; // 0
	static bool bInitialized; // false

	static void InitTiming()
	{
		if (!bInitialized)
		{
			bInitialized = true;

			double Frequency = (double)GetFrequency();
			if (Frequency <= 0.0)
			{
				Frequency = 1.0;
			}

			GSecondsPerCycle = 1.0 / Frequency;
		}
	}
	static double GetSecondsPerCycle()
	{
		if (!bInitialized)
		{
			InitTiming();
		}
		return (double)GSecondsPerCycle;
	}
	static uint64 GetFrequency()
	{
		LARGE_INTEGER Frequency;
		QueryPerformanceFrequency(&Frequency);
		return Frequency.QuadPart;
	}
	static double ToMilliseconds(uint64 CycleDiff)
	{
		double Ms = static_cast<double>(CycleDiff)
			* GetSecondsPerCycle()
			* 1000.0;

		return Ms;
	}

	static uint64 Cycles64()
	{
		LARGE_INTEGER CycleCount;
		QueryPerformanceCounter(&CycleCount);
		return (uint64)CycleCount.QuadPart;
	}
};

struct TStatId
{
	FString Key;
	TStatId() = default;
	TStatId(const FString& InKey) : Key(InKey) {}
};
struct FTimeProfile
{
	double Milliseconds;
	uint32 CallCount;

	const char* GetConstChar() const
	{
		static char buffer[64]; // static으로 해야 반환 가능
		sprintf_s(buffer, sizeof(buffer), " : %.3fms, Call : %d", Milliseconds, CallCount);
		return buffer;
	}

	const wchar_t* GetConstWChar_t() const
	{
		static wchar_t buffer[64];
		swprintf_s(buffer, _countof(buffer), L" : %.3fms, Call : %d", Milliseconds, CallCount);
		return buffer;
	}

	const char* GetConstCharWithKey(const FString& Key) const
	{
		static char buffer[64]; // static으로 해야 반환 가능
		sprintf_s(buffer, sizeof(buffer), "%s : %.3fms, Call : %d", Key.c_str(), Milliseconds, CallCount);
		return buffer;
	}

	const wchar_t* GetConstWChar_tWithKey(const FString& Key) const
	{
		static wchar_t buffer[64];

		swprintf_s(buffer, _countof(buffer), L"%s : %.3fms, Call : %d", std::wstring(Key.begin(), Key.end()).c_str(), Milliseconds, CallCount);
		return buffer;
	}
};

typedef FWindowsPlatformTime FPlatformTime;

class FScopeCycleCounter
{
public:
	FScopeCycleCounter(TStatId StatId)
		: StartCycles(FPlatformTime::Cycles64()) //생성 시 사이클 저장
		, UsedStatId(StatId) //키값 저장
	{
	}
	FScopeCycleCounter() : StartCycles(FPlatformTime::Cycles64()), UsedStatId()
	{
	}

	FScopeCycleCounter(const FString& Key) : StartCycles(FPlatformTime::Cycles64()), UsedStatId(TStatId(Key))
	{
	}

	~FScopeCycleCounter()
	{
		Finish(); //소멸 시 현재 사이클 구해서 현재 - 생성 사이클로 시간 측정
	}

	double Finish()
	{
		if (bIsFinish == true)
		{
			return 0;
		}
		bIsFinish = true;
		const uint64 EndCycles = FPlatformTime::Cycles64();
		const uint64 CycleDiff = EndCycles - StartCycles;

		double Milliseconds = FWindowsPlatformTime::ToMilliseconds(CycleDiff);
		if (UsedStatId.Key.empty() == false)
		{
			AddTimeProfile(UsedStatId, Milliseconds); //키 값이 있을경우 Map에 저장
		}
		return Milliseconds;
	}

	static void AddTimeProfile(const TStatId& Key, double InMilliseconds);
	static void TimeProfileInit();

	//이거 왜 안됨?
	//static const TMap<FString, FTimeProfile>& GetTimeProfiles();

	static const TArray<FString> GetTimeProfileKeys();
	static const TArray<FTimeProfile> GetTimeProfileValues();
	static const FTimeProfile& GetTimeProfile(const FString& Key);
private:
	bool bIsFinish = false;
	uint64 StartCycles;
	TStatId UsedStatId;

};

