# 📘 KRAFTON TechLab Week03 – Unreal Engine Style 3D Editor & Rendering System
📌 프로젝트 개요

### 프로젝트명: Unreal Engine Style 3D Editor & Rendering System

#### 개발 기간: 1 week

#### 개발 환경: Visual Studio, DirectX 11, Windows 10/11

## 아키텍처: C++ 기반 Actor-Component 시스템
<img width="1911" height="1104" alt="image" src="https://github.com/user-attachments/assets/2aa029db-2fca-451d-aeaf-607d3256ccee" />


## 🚀 구현 완료 사항
### 🎨 1. Editor & Rendering System (눈에 보이는 세상)
#### 1.1 실시간 텍스트 렌더링 시스템

파일: TextBillboard.hlsl, Week02/UI/

기능

ASCII 문자 → 텍스처 아틀라스 베이킹

UV 좌표 조작을 통한 임의 문자열 생성

Billboard 효과 (항상 카메라를 향함)

월드 공간의 UObject UUID 실시간 표시

기술

ViewInverse 행렬을 이용한 카메라 정렬

Alpha Testing 기반 텍스트 윤곽 처리

#### 1.2 Batch Line 렌더링 시스템

파일: ShaderLine.hlsl

기능

모든 Line을 하나의 Vertex/Index Buffer로 관리

D3D11_PRIMITIVE_TOPOLOGY_LINELIST 기반

World Grid, Bounding Box 시각화

성능 최적화

단일 Draw Call 처리

동적 버퍼 업데이트 지원

#### 1.3 좌표계 변환 시스템

DirectX 좌표계 (Y-Up, Z-Depth) → UE 좌표계 (Z-Up, X-Depth) 변환

카메라, 트랜스폼, 렌더링 파이프라인 전반에 적용

#### 1.4 동적 윈도우 리사이징

RTV/DSV 해제 → SwapChain Resize → ImGui DisplaySize 갱신 → RTV/DSV 재생성

#### 1.5 View Mode 시스템

Lit / Unlit / Wireframe 모드 지원

#### 1.6 Show Flag 시스템

비트 플래그 기반 토글 기능

예: Primitive 표시, UUID 텍스트, Bounding Box, Grid 등

ImGui 연동으로 실시간 제어

⚙️ 2. Engine Core System (눈에 안 보이는 세상)
#### 2.1 FName 시스템

문자열 → Pool 관리, 인덱스로 비교

strcmp() 대신 정수 비교 (O(n) → O(1))

대소문자 구분 없는 비교 ("Test" == "test")

메모리 절약 + 성능 최적화

#### 2.2 UObject 시스템 & RTTI

구현: Object.h

특징

UUID, FName 기반 이름 관리

컴파일 타임 타입 정보 생성 (DECLARE_CLASS)

안전한 런타임 타입 검사 & 캐스팅

ObjectFactory와 연동된 메모리 관리

#### 2.3 메모리 관리 시스템

UObject 전용 Allocator / Deallocator

중앙 집중식 객체 관리

자동 정리로 메모리 누수 방지

#### 2.4 Selection Manager

선택된 Actor 관리

안전성 강화 (CleanupInvalidActors)

## 🏗 아키텍처 및 설계 패턴

싱글톤 패턴: UUIManager, USelectionManager, UInputManager

컴포넌트 시스템: Actor-Component 구조 (Camera, Cube, Gizmo 등)

팩토리 패턴: ObjectFactory + NewObject<T>() 템플릿

Observer 패턴: Show Flag 시스템과 UI/렌더링 간 느슨한 결합

## 🖥 렌더링 파이프라인

World Grid (Show Flag)

Primitive Geometry (Lit/Unlit/Wireframe)

Bounding Boxes (Show Flag)

Billboard Text (UUID) (Show Flag)

Gizmo (선택된 객체)

셰이더 구성

Primitive.hlsl: 기본 메시

ShaderLine.hlsl: 라인/그리드

TextBillboard.hlsl: 빌보드 텍스트

## ⚡ 안전성 및 성능 최적화

배치 렌더링: Draw Call 최소화

텍스처 아틀라스: 폰트 통합 관리

Show Flag: 불필요한 렌더링 제거

FName 시스템: 문자열 비교 최적화

## 🏆 기술적 성취

언리얼 스타일 아키텍처 구현

UObject 시스템, RTTI, FName 최적화

Actor-Component 패러다임 적용

고급 렌더링 기술

Billboard 텍스트

Batch Line 렌더링

멀티 셰이더 파이프라인 관리

실용적인 에디터 도구

Show Flag 실시간 제어

다중 View Mode 지원

동적 윈도우 리사이징
