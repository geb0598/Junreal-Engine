#pragma once
#include "Vector.h"

#define CBUFFER_TYPE_LIST(MACRO)\
MACRO(TestCB)					\
MACRO(TestCB2)					\

#define CBUFFER_INFO(TYPENAME, SLOTNUM, SETVS, SETPS)\
constexpr uint32 TYPENAME##SlotNum = SLOTNUM;\
constexpr bool TYPENAME##SetVS = SETVS;\
constexpr bool TYPENAME##SetPS = SETPS;\

CBUFFER_INFO(TestCB, 0, true, false)
CBUFFER_INFO(TestCB2, 1, true, false)


//Create 
//Release
//매크로를 통해 List에 추가하는 형태로 제작
//List를 통해 D3D11RHI
//Update
//Set vs,ps,hs,ds,gs

struct TestCB
{
	FVector4 TestData;
};
struct TestCB2
{
	FVector4 TestData;
};



