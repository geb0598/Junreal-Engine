﻿#pragma once
#include <immintrin.h> // SSE, AVX, AVX2, FMA 등 모든 내장 함수 포함

#include <cmath>        // ← 추가 (std::sin, std::cos, std::atan2, std::copysign 등)
#include <algorithm>
#include <string>
#include <limits>

#include "UEContainer.h"


// 혹시 다른 헤더에서 새어 들어온 매크로 방지
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif
constexpr float KINDA_SMALL_NUMBER = 1e-6f;

// ─────────────────────────────
// Constants & Helpers
// ─────────────────────────────
constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = 6.2831853071795864769f;
constexpr float HALF_PI = 1.5707963267948966192f;

inline float DegreeToRadian(float Degree) { return Degree * (PI / 180.0f); }
inline float RadianToDegree(float Radian) { return Radian * (180.0f / PI); }
// FMath 네임스페이스 대체
namespace FMath {
    template<typename T>
    static T Max(T A, T B) { return std::max(A, B); }
    template<typename T>
    static T Min(T A, T B) { return std::min(A, B); }
    template<typename T>
    static T Clamp(T Value, T Min, T Max) {
        return Value < Min ? Min : (Value > Max ? Max : Value);
    }
    template<typename T>
    static T Lerp(T A, T B, float Alpha) {
        return A + (B - A) * Alpha;
    }
}
// 각도를 -180 ~ 180 범위로 정규화 (모듈러 연산)
inline float NormalizeAngleDeg(float angleDeg)
{
    // fmod로 -360 ~ 360 범위로 만든 후, -180 ~ 180 범위로 조정
    angleDeg = std::fmod(angleDeg, 360.0f);
    if (angleDeg > 180.0f)
        angleDeg -= 360.0f;
    else if (angleDeg < -180.0f)
        angleDeg += 360.0f;
    return angleDeg;
}

// ─────────────────────────────
// Forward Declarations
// ─────────────────────────────
struct FVector;
struct FVector4;
struct FQuat;
struct FMatrix;
struct FTransform;

// ─────────────────────────────
// FVector (2D Vector)
// ─────────────────────────────

//// Add this global operator== for FVector2D and FVector4 to fix E0349 errors
//inline bool operator==(const FVector2D& A, const FVector2D& B)
//{
//    return std::fabs(A.X - B.X) < KINDA_SMALL_NUMBER &&
//        std::fabs(A.Y - B.Y) < KINDA_SMALL_NUMBER;
//}
//
//inline bool operator!=(const FVector2D& A, const FVector2D& B)
//{
//    return !(A == B);
//}
//
//inline bool operator==(const FVector4& A, const FVector4& B)
//{
//    return std::fabs(A.X - B.X) < KINDA_SMALL_NUMBER &&
//        std::fabs(A.Y - B.Y) < KINDA_SMALL_NUMBER &&
//        std::fabs(A.Z - B.Z) < KINDA_SMALL_NUMBER &&
//        std::fabs(A.W - B.W) < KINDA_SMALL_NUMBER;
//}
//
//inline bool operator!=(const FVector4& A, const FVector4& B)
//{
//    return !(A == B);
//}

struct FVector2D
{
    float X, Y;

    FVector2D(float InX = 0.0f, float InY = 0.0f) : X(InX), Y(InY)
    {
    }

    bool operator==(const FVector2D& V) const
    {
        return std::fabs(X - V.X) < KINDA_SMALL_NUMBER &&
            std::fabs(Y - V.Y) < KINDA_SMALL_NUMBER;
    }
    bool operator!=(const FVector2D& V) const { return !(*this == V); }

    FVector2D operator-(const FVector2D& Other) const
    {
        return FVector2D(X - Other.X, Y - Other.Y);
    }

    FVector2D operator+(const FVector2D& Other) const
    {
        return FVector2D(X + Other.X, Y + Other.Y);
    }

    FVector2D operator*(float Scalar) const
    {
        return FVector2D(X * Scalar, Y * Scalar);
    }

    float Length() const
    {
        return std::sqrt(X * X + Y * Y);
    }

    FVector2D GetNormalized() const
    {
        float Len = Length();
        if (Len > 0.0001f)
            return FVector2D(X / Len, Y / Len);
        return FVector2D(0.0f, 0.0f);
    }
};


// ─────────────────────────────
// FVector (3D Vector)
// ─────────────────────────────
struct FVector
{
    float X, Y, Z;

    FVector(float InX = 0, float InY = 0, float InZ = 0)
        : X(InX), Y(InY), Z(InZ)
    {
    }

    // 기본 연산
    FVector operator+(const FVector& V) const { return FVector(X + V.X, Y + V.Y, Z + V.Z); }
    FVector operator-(const FVector& V) const { return FVector(X - V.X, Y - V.Y, Z - V.Z); }
    FVector operator*(const FVector& V) const { return FVector(X * V.X, Y * V.Y, Z * V.Z); }
    FVector operator*(float S)        const { return FVector(X * S, Y * S, Z * S); }
    FVector operator/(float S)        const { return FVector(X / S, Y / S, Z / S); }
    FVector operator+(float S)        const { return FVector(X + S, Y + S, Z + S); }
    FVector operator-(float S)        const { return FVector(X - S, Y - S, Z - S); }
    FVector operator-()               const { return FVector(-X, -Y, -Z); }

    FVector& operator+=(const FVector& V) { X += V.X; Y += V.Y; Z += V.Z; return *this; }
    FVector& operator-=(const FVector& V) { X -= V.X; Y -= V.Y; Z -= V.Z; return *this; }
    FVector& operator*=(float S) { X *= S; Y *= S; Z *= S; return *this; }
    FVector& operator/=(float S) { X /= S; Y /= S; Z /= S; return *this; }
    FVector& operator+=(float S) { X += S; Y += S; Z += S; return *this; }
    FVector& operator-=(float S) { X -= S; Y -= S; Z -= S; return *this; }

    bool operator==(const FVector& V) const
    {
        return std::fabs(X - V.X) < KINDA_SMALL_NUMBER &&
            std::fabs(Y - V.Y) < KINDA_SMALL_NUMBER &&
            std::fabs(Z - V.Z) < KINDA_SMALL_NUMBER;
    }
    bool operator!=(const FVector& V) const { return !(*this == V); }

    // 인덱스 접근자 (0=X, 1=Y, 2=Z)
    float& operator[](int Index)
    {
        switch (Index)
        {
        case 0: return X;
        case 1: return Y;
        case 2: return Z;
        default: return X; // 안전한 기본값
        }
    }

    const float& operator[](int Index) const
    {
        switch (Index)
        {
        case 0: return X;
        case 1: return Y;
        case 2: return Z;
        default: return X; // 안전한 기본값
        }
    }
    
    FVector ComponentMin(const FVector& B)
    {
        return FVector(
            (X < B.X) ? X : B.X,
            (Y < B.Y) ? Y : B.Y,
            (Z < B.Z) ? Z : B.Z
        );
    }
    FVector ComponentMax(const FVector& B)
    {
        return FVector(
            (X > B.X) ? X : B.X,
            (Y > B.Y) ? Y : B.Y,
            (Z > B.Z) ? Z : B.Z
        );
    }
    // 크기
    float Size()         const { return std::sqrt(X * X + Y * Y + Z * Z); }
    float SizeSquared()  const { return X * X + Y * Y + Z * Z; }

    // 정규화
    FVector GetNormalized() const
    {
        float S = Size();
        return (S > KINDA_SMALL_NUMBER) ? (*this / S) : FVector(0, 0, 0);
    }
    void Normalize()
    {
        float S = Size();
        if (S > KINDA_SMALL_NUMBER) { X /= S; Y /= S; Z /= S; }
    }
    FVector GetSafeNormal() const { return GetNormalized(); }

    // 내적/외적
    float Dot(const FVector& V) const { return X * V.X + Y * V.Y + Z * V.Z; }
    static float   Dot(const FVector& A, const FVector& B) { return A.X * B.X + A.Y * B.Y + A.Z * B.Z; }
    static FVector Cross(const FVector& A, const FVector& B)
    {
        return FVector(
            A.Y * B.Z - A.Z * B.Y,
            A.Z * B.X - A.X * B.Z,
            A.X * B.Y - A.Y * B.X
        );
    }


    // 보조 유틸
    static FVector Lerp(const FVector& A, const FVector& B, float T)
    {
        return A + (B - A) * T;
    }
    static float Distance(const FVector& A, const FVector& B)
    {
        return (B - A).Size();
    }
    static float AngleBetween(const FVector& A, const FVector& B) // radians
    {
        float D = Dot(A, B) / (std::sqrt(A.SizeSquared() * B.SizeSquared()) + KINDA_SMALL_NUMBER);
        D = std::max(-1.0f, std::min(1.0f, D));
        return std::acos(D);
    }
    static FVector Project(const FVector& A, const FVector& OnNormal)
    {
        FVector N = OnNormal.GetNormalized();
        return N * Dot(A, N);
    }
    static FVector Reflect(const FVector& V, const FVector& Normal) // Normal normalized
    {
        return V - Normal * (2.0f * Dot(V, Normal));
    }
    static FVector Clamp(const FVector& V, float minLen, float maxLen)
    {
        float Length = V.Size();
        if (Length < KINDA_SMALL_NUMBER) return FVector(0, 0, 0);
        float Value = std::max(minLen, std::min(maxLen, Length));
        return V * (Value / Length);
    }

    static FVector One()
    {
        return FVector(1.f, 1.f, 1.f);
    }
};

// ─────────────────────────────
// FVector4 (4D Vector)
// ─────────────────────────────
struct FVector4
{
    float X, Y, Z, W;
    FVector4(float InX = 0, float InY = 0, float InZ = 0, float InW = 0)
        : X(InX), Y(InY), Z(InZ), W(InW)
    {
    }
    FVector4(const FVector& InVt3, float InW = 0)
        : X(InVt3.X), Y(InVt3.Y), Z(InVt3.Z), W(InW)
    {
    }

    FVector4 ComponentMin(const FVector4& B)
    {
        return FVector4(
            (X < B.X) ? X : B.X,
            (Y < B.Y) ? Y : B.Y,
            (Z < B.Z) ? Z : B.Z,
            (W < B.W) ? W : B.W
        );
    }
    FVector4 ComponentMax(const FVector4& B)
    {
        return FVector4(
            (X > B.X) ? X : B.X,
            (Y > B.Y) ? Y : B.Y,
            (Z > B.Z) ? Z : B.Z,
            (W > B.W) ? W : B.W
        );
    }
    FVector GetVt3()
    {
        if (W == 0)
        {
            return FVector(0, 0, 0);
        }
        else if (W == 1)
        {
            return FVector(X, Y, Z);
        }
        else
        {
            float RcpW = 1 / W;
            return FVector(X, Y, Z) * RcpW;
        }
    }
};

// ─────────────────────────────
// FQuat (Quaternion)
// ─────────────────────────────
struct FQuat
{
    float X, Y, Z, W;

    FQuat(float InX = 0, float InY = 0, float InZ = 0, float InW = 1)
        : X(InX), Y(InY), Z(InZ), W(InW)
    {
    }

    static FQuat Identity() { return FQuat(0, 0, 0, 1); }

    //Quat1 * Quat2 = Quat2로 회전 후 Quat1로 회전
    FQuat operator*(const FQuat& Q) const
    {
        return FQuat(
            W * Q.X + X * Q.W + Y * Q.Z - Z * Q.Y,
            W * Q.Y - X * Q.Z + Y * Q.W + Z * Q.X,
            W * Q.Z + X * Q.Y - Y * Q.X + Z * Q.W,
            W * Q.W - X * Q.X - Y * Q.Y - Z * Q.Z
        );
    }

    static float Dot(const FQuat& A, const FQuat& B)
    {
        return A.X * B.X + A.Y * B.Y + A.Z * B.Z + A.W * B.W;
    }

    float SizeSquared() const { return X * X + Y * Y + Z * Z + W * W; }
    float Size()        const { return std::sqrt(SizeSquared()); }

    void Normalize()
    {
        float S = Size();
        if (S > KINDA_SMALL_NUMBER) { X /= S; Y /= S; Z /= S; W /= S; }
        else { *this = Identity(); }
    }

    FQuat GetNormalized() const
    {
        FQuat Quat = *this; Quat.Normalize(); return Quat;
    }

    FQuat Conjugate() const { return FQuat(-X, -Y, -Z, W); }
    FQuat Inverse()   const
    {
        float SS = SizeSquared();
        if (SS <= KINDA_SMALL_NUMBER) return Identity();
        return Conjugate() * (1.0f / SS);
    }
    // 스칼라 곱
    FQuat operator*(float S) const { return FQuat(X * S, Y * S, Z * S, W * S); }

    // 벡터 회전
    FVector RotateVector(const FVector& V) const;

    // 오일러 → 쿼터니언 (Pitch=X, Yaw=Y, Roll=Z in degrees)
    // ZYX 순서 (Roll → Yaw → Pitch) - 로컬 축 회전
    static FQuat MakeFromEuler(const FVector& EulerRad)
    {
        float PitchDeg = DegreeToRadian(EulerRad.X);
        float YawDeg = DegreeToRadian(EulerRad.Y);
        float RollDeg = DegreeToRadian(EulerRad.Z);

        const float PX = PitchDeg * 0.5f; // Pitch about X
        const float PY = YawDeg * 0.5f; // Yaw   about Y
        const float PZ = RollDeg * 0.5f; // Roll  about Z

        const float CX = cosf(PX), SX = sinf(PX);
        const float CY = cosf(PY), SY = sinf(PY);
        const float CZ = cosf(PZ), SZ = sinf(PZ);

        // Order: Rx * Ry * Rz  (intrinsic X→Y→Z)
        // Mapping: X=Pitch, Y=Yaw, Z=Roll
        FQuat q;
        q.X = SX * CY * CZ + CX * SY * SZ;
        q.Y = CX * SY * CZ - SX * CY * SZ;
        q.Z = CX * CY * SZ + SX * SY * CZ;
        q.W = CX * CY * CZ - SX * SY * SZ;
        return q.GetNormalized(); // 필요하면 정규화
    }
    // Axis-Angle → Quaternion (axis normalized, angle in radians)
    static FQuat FromAxisAngle(const FVector& Axis, float Angle)
    {
        float Half = Angle * 0.5f;
        float S = std::sin(Half);
        FVector N = Axis.GetNormalized();
        return FQuat(N.X * S, N.Y * S, N.Z * S, std::cos(Half));
    }

    // 쿼터니언 → 오일러 (Pitch, Yaw, Roll) in degrees
    // ZYX 순서 (Tait-Bryan) - FromEuler와 일치 (로컬 축 회전)
    FVector ToEuler() const
    {
        // normalize (safety)
        float N = std::sqrt(X * X + Y * Y + Z * Z + W * W);
        if (N <= KINDA_SMALL_NUMBER) return FVector(0, 0, 0);
        float QX = X / N, QY = Y / N, QZ = Z / N, QW = W / N;

        // ZYX Tait-Bryan 각도 추출 (Roll → Yaw → Pitch)
        // Pitch (X축): atan2(2*(qw*qx - qy*qz), 1 - 2*(qx² + qz²))
        float SinrCosp = 2.0f * (QW * QX - QY * QZ);
        float CosrCosp = 1.0f - 2.0f * (QX * QX + QZ * QZ);
        float Pitch = std::atan2(SinrCosp, CosrCosp);

        // Yaw (Y축): asin(2*(qw*qy + qx*qz))
        float Sinp = 2.0f * (QW * QY + QX * QZ);
        Sinp = std::max(-1.0f, std::min(1.0f, Sinp)); // clamp to avoid NaN
        float Yaw = std::asin(Sinp);

        // Roll (Z축): atan2(2*(qw*qz - qx*qy), 1 - 2*(qy² + qz²))
        float SinyCosp = 2.0f * (QW * QZ - QX * QY);
        float CosyCosp = 1.0f - 2.0f * (QY * QY + QZ * QZ);
        float Roll = std::atan2(SinyCosp, CosyCosp);

        return FVector(RadianToDegree(Pitch), RadianToDegree(Yaw), RadianToDegree(Roll));
    }
    FVector GetForwardVector() const
{
    // 보통 게임엔진(Z-Up, Forward = +X) 기준
    return RotateVector(FVector(1, 0, 0));
}

FVector GetRightVector() const
{
    return RotateVector(FVector(0, 1, 0));
}

FVector GetUpVector() const
{
    return RotateVector(FVector(0, 0, 1));
}

    // Slerp
    static FQuat Slerp(const FQuat& A, const FQuat& B, float T)
    {
        float CosTheta = Dot(A, B);
        FQuat End = B;

        // 가장 짧은 호
        if (CosTheta < 0.0f) { End = FQuat(-B.X, -B.Y, -B.Z, -B.W); CosTheta = -CosTheta; }

        // 근접하면 Nlerp
        const float SLERP_EPS = 1e-3f;
        if (CosTheta > 1.0f - SLERP_EPS)
        {
            FQuat r = FQuat(
                A.X + (End.X - A.X) * T,
                A.Y + (End.Y - A.Y) * T,
                A.Z + (End.Z - A.Z) * T,
                A.W + (End.W - A.W) * T
            );
            r.Normalize();
            return r;
        }

        float Theta = std::acos(CosTheta);
        float SinTheta = std::sin(Theta);
        float W1 = std::sin((1.0f - T) * Theta) / SinTheta;
        float W2 = std::sin(T * Theta) / SinTheta;

        FQuat Quat = A * W1 + End * W2;
        Quat.Normalize();
        return Quat;
    }

    // 보조: 선형 보간 후 정규화
    static FQuat Nlerp(const FQuat& A, const FQuat& B, float T)
    {
        FQuat End = (Dot(A, B) < 0.0f) ? FQuat(-B.X, -B.Y, -B.Z, -B.W) : B;
        FQuat Quat(A.X + (End.X - A.X) * T,
                A.Y + (End.Y - A.Y) * T,
                A.Z + (End.Z - A.Z) * T,
                A.W + (End.W - A.W) * T);
        Quat.Normalize();
        return Quat;
    }

    // 선언: 행렬 변환
    FMatrix ToMatrix() const;

private:
    // 보조 연산 (내부용)
    FQuat operator+(const FQuat& Quat) const { return FQuat(X + Quat.X, Y + Quat.Y, Z + Quat.Z, W + Quat.W); }
};

// 스칼라 곱(전역)
inline FQuat operator*(float Scalar, const FQuat& Quat) { return FQuat(Quat.X * Scalar, Quat.Y * Scalar, Quat.Z * Scalar, Quat.W * Scalar); }

// ─────────────────────────────
// FMatrix (4x4 Matrix)
// (Row-major, Translation in M[row][3])
// ─────────────────────────────
struct alignas(16) FMatrix
{
    union // 같은 데이터를 다양한 방법으로 참조하기 위한 키워드
    {
        float M[4][4]{};
        float FlatM[16]; // 1차원 배열로 접근
        FVector4 Rows[4];  // FVector4 행으로 접근
    };

    FMatrix() = default;

    constexpr FMatrix(float M00, float M01, float M02, float M03,
                      float M10, float M11, float M12, float M13,
                      float M20, float M21, float M22, float M23,
                      float M30, float M31, float M32, float M33) noexcept
    {
        M[0][0] = M00; M[0][1] = M01; M[0][2] = M02; M[0][3] = M03;
        M[1][0] = M10; M[1][1] = M11; M[1][2] = M12; M[1][3] = M13;
        M[2][0] = M20; M[2][1] = M21; M[2][2] = M22; M[2][3] = M23;
        M[3][0] = M30; M[3][1] = M31; M[3][2] = M32; M[3][3] = M33;
    }

    static FMatrix Identity()
    {
        return FMatrix(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
    }

    static FMatrix Zero()
    {
        return FMatrix(
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        );
    }

    static FMatrix CreateScale(const FVector& Scale)
    {
        return FMatrix(
            Scale.X, 0, 0, 0,
            0, Scale.Y, 0, 0,
            0, 0, Scale.Z, 0,
            0, 0, 0, 1
        );
    }

    // 행렬 == 행렬
    bool operator==(const FMatrix& B) const noexcept
    {
        const FMatrix& A = *this;
        for (uint8 i = 0; i < 4; ++i)
        {
            for (uint8 j = 0; j < 4; ++j)
            {
                if (A.M[i][j] != B.M[i][j])
                    return false;
            }
        }
        return true;
    }

    // 행렬 != 행렬
    bool operator!=(const FMatrix& B) const noexcept
    {
        return !(*this == B);
    }

    FMatrix operator*(const FMatrix& B) const
    {
        const FMatrix& A = *this;
        FMatrix Result;

        // B의 각 행을 __m128 레지스터로 미리 로드
        __m128 BRow0 = _mm_load_ps(&B.Rows[0].X);
        __m128 BRow1 = _mm_load_ps(&B.Rows[1].X);
        __m128 BRow2 = _mm_load_ps(&B.Rows[2].X);
        __m128 BRow3 = _mm_load_ps(&B.Rows[3].X);

        // A의 각 행에 대해 반복
        for (int i = 0; i < 4; ++i)
        {
            // A의 i번째 행에서 X, Y, Z, W 값을 각각 모든 레인에 복사(Broadcast)
            __m128 ARow = _mm_load_ps(&A.Rows[i].X);
            __m128 AX = _mm_shuffle_ps(ARow, ARow, _MM_SHUFFLE(0, 0, 0, 0)); // A.X, A.X, A.X, A.X
            __m128 AY = _mm_shuffle_ps(ARow, ARow, _MM_SHUFFLE(1, 1, 1, 1)); // A.Y, A.Y, A.Y, A.Y
            __m128 AZ = _mm_shuffle_ps(ARow, ARow, _MM_SHUFFLE(2, 2, 2, 2)); // A.Z, A.Z, A.Z, A.Z
            __m128 AW = _mm_shuffle_ps(ARow, ARow, _MM_SHUFFLE(3, 3, 3, 3)); // A.W, A.W, A.W, A.W

            // FMA (Fused Multiply-Add)를 사용하여 곱셈과 덧셈을 한 번에 처리
            // ResultRow = (AX * BRow0) + (AY * BRow1) + (AZ * BRow2) + (AW * BRow3)
            __m128 ResultRow = _mm_mul_ps(AX, BRow0);
            ResultRow = _mm_fmadd_ps(AY, BRow1, ResultRow); // ResultRow = (AY * BRow1) + ResultRow
            ResultRow = _mm_fmadd_ps(AZ, BRow2, ResultRow); // ResultRow = (AZ * BRow2) + ResultRow
            ResultRow = _mm_fmadd_ps(AW, BRow3, ResultRow); // ResultRow = (AW * BRow3) + ResultRow

            // 계산된 행을 결과 행렬에 저장
            _mm_store_ps(&Result.Rows[i].X, ResultRow);
        }
        return Result;
    }

    // 전치
    FMatrix Transpose() const
    {
        FMatrix T;
        for (uint8 i = 0; i < 4; ++i)
        {
            for (uint8 j = 0; j < 4; ++j)
            {
                T.M[i][j] = M[j][i];
            }
        }
        return T;
    }

    // Affine 역행렬 (마지막 행 = [0,0,0,1] 가정)
    FMatrix InverseAffine() const
    {
        // 상단 3x3 역행렬
        float A00 = M[0][0], A01 = M[0][1], A02 = M[0][2];
        float A10 = M[1][0], A11 = M[1][1], A12 = M[1][2];
        float A20 = M[2][0], A21 = M[2][1], A22 = M[2][2];

        float Det = A00 * (A11 * A22 - A12 * A21) - A01 * (A10 * A22 - A12 * A20) + A02 * (A10 * A21 - A11 * A20);
        if (std::fabs(Det) < KINDA_SMALL_NUMBER) return Identity();
        float InvDet = 1.0f / Det;

        // Rinv = R^{-1}
        FMatrix InvMat = Identity();
        InvMat.M[0][0] = (A11 * A22 - A12 * A21) * InvDet;
        InvMat.M[0][1] = -(A01 * A22 - A02 * A21) * InvDet;
        InvMat.M[0][2] = (A01 * A12 - A02 * A11) * InvDet;

        InvMat.M[1][0] = -(A10 * A22 - A12 * A20) * InvDet;
        InvMat.M[1][1] = (A00 * A22 - A02 * A20) * InvDet;
        InvMat.M[1][2] = -(A00 * A12 - A02 * A10) * InvDet;

        InvMat.M[2][0] = (A10 * A21 - A11 * A20) * InvDet;
        InvMat.M[2][1] = -(A00 * A21 - A01 * A20) * InvDet;
        InvMat.M[2][2] = (A00 * A11 - A01 * A10) * InvDet;

        // t: last row
        const FVector T(M[3][0], M[3][1], M[3][2]);

        // invT = -t * Rinv  (t treated as row-vector)
        FVector invT(
            -(T.X * InvMat.M[0][0] + T.Y * InvMat.M[1][0] + T.Z * InvMat.M[2][0]),
            -(T.X * InvMat.M[0][1] + T.Y * InvMat.M[1][1] + T.Z * InvMat.M[2][1]),
            -(T.X * InvMat.M[0][2] + T.Y * InvMat.M[1][2] + T.Z * InvMat.M[2][2])
        );

        FMatrix Out = Identity();
        // place Rinv in upper-left
        Out.M[0][0] = InvMat.M[0][0]; Out.M[0][1] = InvMat.M[0][1]; Out.M[0][2] = InvMat.M[0][2];
        Out.M[1][0] = InvMat.M[1][0]; Out.M[1][1] = InvMat.M[1][1]; Out.M[1][2] = InvMat.M[1][2];
        Out.M[2][0] = InvMat.M[2][0]; Out.M[2][1] = InvMat.M[2][1]; Out.M[2][2] = InvMat.M[2][2];
        // translation in last row
        Out.M[3][0] = invT.X; Out.M[3][1] = invT.Y; Out.M[3][2] = invT.Z; Out.M[3][3] = 1.0f;
        return Out;
    }

    // 일반 역행렬 (4x4 full inverse)
    FMatrix Inverse() const
    {
        const float* m = &M[0][0];

        float inv[16];

        inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15]
               + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];

        inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15]
               - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];

        inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15]
               + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];

        inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14]
                - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];

        inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15]
               - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];

        inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15]
               + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];

        inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15]
               - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];

        inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14]
                + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];

        inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15]
               + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];

        inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15]
               - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];

        inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15]
                + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];

        inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14]
                - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];

        inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11]
               - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];

        inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11]
               + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];

        inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11]
                - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];

        inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10]
                + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

        float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

        if (std::fabs(det) < KINDA_SMALL_NUMBER)
            return Identity();

        det = 1.0f / det;

        FMatrix result;
        for (int i = 0; i < 16; i++)
            result.FlatM[i] = inv[i] * det;

        return result;
    }

    FVector4 TransformPosition(const FVector4& V) const
    {
        return FVector4(
            V.X * M[0][0] + V.Y * M[1][0] + V.Z * M[2][0] + V.W * M[3][0],
            V.X * M[0][1] + V.Y * M[1][1] + V.Z * M[2][1] + V.W * M[3][1],
            V.X * M[0][2] + V.Y * M[1][2] + V.Z * M[2][2] + V.W * M[3][2],
            V.X * M[0][3] + V.Y * M[1][3] + V.Z * M[2][3] + V.W * M[3][3]
        );
    }

    static FMatrix FromTRS(const FVector& T, const FQuat& R, const FVector& S)
    {
        FMatrix rot = R.ToMatrix();
        // scale 적용
        rot.M[0][0] *= S.X; rot.M[0][1] *= S.X; rot.M[0][2] *= S.X;
        rot.M[1][0] *= S.Y; rot.M[1][1] *= S.Y; rot.M[1][2] *= S.Y;
        rot.M[2][0] *= S.Z; rot.M[2][1] *= S.Z; rot.M[2][2] *= S.Z;
        // translation
        rot.M[3][0] = T.X;
        rot.M[3][1] = T.Y;
        rot.M[3][2] = T.Z;
        rot.M[3][3] = 1;
        return rot;
    }

    // View/Proj (L H)
    static FMatrix LookAtLH(const FVector& Eye, const FVector& At, const FVector& Up);
    static FMatrix PerspectiveFovLH(float FovY, float Aspect, float Zn, float Zf);
    static FMatrix OrthoLH(float Width, float Height, float Zn, float Zf);
    static FMatrix OffCenterOrthoLH(float Left, float Right, float Top, float Bottom, float Near, float Far);
};

//Without Last RC
inline FVector operator*(const FVector& V, const FMatrix& S)
{
    FVector Result;
    Result.X = V.X * S.M[0][0] + V.Y * S.M[1][0] + V.Z * S.M[2][0];
    Result.Y = V.X * S.M[0][1] + V.Y * S.M[1][1] + V.Z * S.M[2][1];
    Result.Z = V.X * S.M[0][2] + V.Y * S.M[1][2] + V.Z * S.M[2][2];
    return Result;
}

// ─────────────────────────────
// 전역 연산자들
// ─────────────────────────────

// FVector4 * FMatrix (row-vector: v' = v * M)
inline FVector4 operator*(const FVector4& V, const FMatrix& M)
{
    return FVector4(
        V.X * M.M[0][0] + V.Y * M.M[1][0] + V.Z * M.M[2][0] + V.W * M.M[3][0],
        V.X * M.M[0][1] + V.Y * M.M[1][1] + V.Z * M.M[2][1] + V.W * M.M[3][1],
        V.X * M.M[0][2] + V.Y * M.M[1][2] + V.Z * M.M[2][2] + V.W * M.M[3][2],
        V.X * M.M[0][3] + V.Y * M.M[1][3] + V.Z * M.M[2][3] + V.W * M.M[3][3]
    );
}
// ─────────────────────────────
// FTransform (position/rotation/scale)
// ─────────────────────────────
struct FTransform
{
    FVector Translation;
    FQuat   Rotation;
    FVector Scale3D;

    FTransform() : Rotation(0, 0, 0, 1), Translation(0, 0, 0), Scale3D(1, 1, 1) {}
    FTransform(const FVector& T, const FQuat& R, const FVector& S) : Rotation(R), Translation(T), Scale3D(S) {}

    FMatrix ToMatrixWithScaleLocalXYZ() const;

    // 자식로컬 : cl
    // 자식월드 : cw
    // 부모월드 : pw
    // 부모월드 inverse : pwi
    //Quat1 * Quat2 = Quat2로 회전 후 Quat1로 회전

    //Other = 자식 로컬, this = 부모 월드
    FTransform GetWorldTransform(const FTransform& Other) const
    {
        FTransform Result;

        //Rcw = Rpw * Rcl
        //자식로컬 회전 후 부모월드 회전 = 자식월드 회전
        Result.Rotation = Rotation * Other.Rotation;
        Result.Rotation.Normalize();

        // 스케일 결합 (component-wise)
        Result.Scale3D = FVector(
            Scale3D.X * Other.Scale3D.X,
            Scale3D.Y * Other.Scale3D.Y,
            Scale3D.Z * Other.Scale3D.Z
        );


        // Tcw = Tcl * Spw * Rpw + Tpw
        FVector Scaled(Other.Translation * Scale3D);
        FVector Rotated = Rotation.RotateVector(Scaled);
        Result.Translation = Translation + Rotated;

        return Result;
    }

    //this = InverseParent, Other = ChildWorld
    FTransform GetRelativeTransform(const FTransform& Other) const
    {
        FTransform Result;
        
        //Rcl = Rcwi * Rcw
        //자식월드회전 후 부모역회전 = 자식로컬회전
        Result.Rotation = Rotation * Other.Rotation;
        Result.Rotation.Normalize();

        // 스케일 결합 (component-wise)
        Result.Scale3D = FVector(
            Scale3D.X * Other.Scale3D.X,
            Scale3D.Y * Other.Scale3D.Y,
            Scale3D.Z * Other.Scale3D.Z
        );

        //Tcw = Tcl * Spw * Rpw + Tpw
        //(Tcw - Tpw) * Rpwi * Spwi = Tcl (위 식을 전개한 식)
        //Tpwi = -Tpw * Rpwi * Spwi (FTransform 의 Inverse 에 이렇게 정의되어 있음)
        //Tcw * Rpwi * Spwi + Tpwi = Tcl
        FVector Rotated = Rotation.RotateVector(Other.Translation);
        FVector Scaled(Rotated * Scale3D);
        Result.Translation = Translation + Scaled;

        return Result;
    }
    // 역변환
    FTransform Inverse() const;

    // 유틸
    FVector TransformPosition(const FVector& P) const
    {
        // (R*S)*P + T
        FVector SP = FVector(P.X * Scale3D.X, P.Y * Scale3D.Y, P.Z * Scale3D.Z);
        FVector RP = Rotation.RotateVector(SP);
        return Translation + RP;
    }
    FVector TransformVector(const FVector& V) const
    {
        // R*(S*V) (translation 없음)
        FVector SV = FVector(V.X * Scale3D.X, V.Y * Scale3D.Y, V.Z * Scale3D.Z);
        return Rotation.RotateVector(SV);
    }

    static FTransform Lerp(const FTransform& A, const FTransform& B, float T)
    {
        FVector  TPosition = FVector::Lerp(A.Translation, B.Translation, T);
        FVector  TScale = FVector::Lerp(A.Scale3D, B.Scale3D, T);
        FQuat    TRotation = FQuat::Slerp(A.Rotation, B.Rotation, T);
        return FTransform(TPosition, TRotation, TScale);
    }
};

// ─────────────────────────────
// Inline 구현부
// ─────────────────────────────

// v' = v + 2 * cross(q.xyz, cross(q.xyz, v) + q.w * v)
inline FVector FQuat::RotateVector(const FVector& V) const
{
    float n = X * X + Y * Y + Z * Z + W * W;
    if (n <= KINDA_SMALL_NUMBER) return V;

    const FVector U(X, Y, Z);
    const FVector T(
        2.0f * (U.Y * V.Z - U.Z * V.Y),
        2.0f * (U.Z * V.X - U.X * V.Z),
        2.0f * (U.X * V.Y - U.Y * V.X)
    );
    return FVector(
        V.X + W * T.X + (U.Y * T.Z - U.Z * T.Y),
        V.Y + W * T.Y + (U.Z * T.X - U.X * T.Z),
        V.Z + W * T.Z + (U.X * T.Y - U.Y * T.X)
    );
}
inline FQuat MakeQuatFromAxisAngle(const FVector& Axis, float AngleRad)
{
    FVector NormAxis = Axis.GetSafeNormal();
    float half = AngleRad * 0.5f;
    float s = sinf(half);
    return FQuat(
        NormAxis.X * s,
        NormAxis.Y * s,
        NormAxis.Z * s,
        cosf(half)
    );
}

// FQuat → Matrix
inline FMatrix FQuat::ToMatrix() const
{
    float XX = X * X, YY = Y * Y, ZZ = Z * Z;
    float XY = X * Y, XZ = X * Z, YZ = Y * Z;
    float WX = W * X, WY = W * Y, WZ = W * Z;

    return FMatrix(
        1.0f - 2.0f * (YY + ZZ), 2.0f * (XY - WZ), 2.0f * (XZ + WY), 0.0f,
        2.0f * (XY + WZ), 1.0f - 2.0f * (XX + ZZ), 2.0f * (YZ - WX), 0.0f,
        2.0f * (XZ - WY), 2.0f * (YZ + WX), 1.0f - 2.0f * (XX + YY), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

// Row-major + 행벡터(p' = p * M), Left-Handed: forward = +Z
inline FMatrix FMatrix::LookAtLH(const FVector& Eye, const FVector& At, const FVector& Up)
{
    // 1) forward(+Z)
    FVector ZAxis = (At - Eye);
    const float LenZ2 = ZAxis.X * ZAxis.X + ZAxis.Y * ZAxis.Y + ZAxis.Z * ZAxis.Z;
    if (LenZ2 < 1e-12f) // Eye == At
        return FMatrix::Identity();
    ZAxis = ZAxis / sqrt(LenZ2);

    // 2) right = Up × forward (LH에서 이 순서가 오른손쪽을 만듦)
    FVector XAxis = FVector::Cross(Up, ZAxis);
    float LenX2 = XAxis.X * XAxis.X + XAxis.Y * XAxis.Y + XAxis.Z * XAxis.Z;
    if (LenX2 < 1e-12f)
    { // up이 forward와 평행/반평행이면 임의 보정 벡터 사용
        const FVector Temp(0.0f, 1.0f, 0.0f);
        XAxis = FVector::Cross(Temp, ZAxis);
        LenX2 = XAxis.X * XAxis.X + XAxis.Y * XAxis.Y + XAxis.Z * XAxis.Z;
        if (LenX2 < 1e-12f)
        {
            const FVector Tmp2(1.0f, 0.0f, 0.0f);
            XAxis = FVector::Cross(Tmp2, ZAxis);
            LenX2 = XAxis.X * XAxis.X + XAxis.Y * XAxis.Y + XAxis.Z * XAxis.Z;
            if (LenX2 < 1e-12f) return FMatrix::Identity();
        }
    }
    XAxis = XAxis / sqrt(LenX2);

    // 3) Up = forward × right  (정규직교 보정)
    FVector YAxis = FVector::Cross(ZAxis, XAxis);
    const float LenY2 = YAxis.X * YAxis.X + YAxis.Y * YAxis.Y + YAxis.Z * YAxis.Z;
    if (LenY2 < 1e-12f) return FMatrix::Identity();
    YAxis = YAxis / sqrt(LenY2);

    // 4) 조립 (기저 벡터를 행에, 평행이동을 마지막 행에)
    FMatrix View = FMatrix::Identity();
    View.M[0][0] = XAxis.X; View.M[0][1] = XAxis.Y; View.M[0][2] = XAxis.Z; View.M[0][3] = 0.0f;
    View.M[1][0] = YAxis.X; View.M[1][1] = YAxis.Y; View.M[1][2] = YAxis.Z; View.M[1][3] = 0.0f;
    View.M[2][0] = ZAxis.X; View.M[2][1] = ZAxis.Y; View.M[2][2] = ZAxis.Z; View.M[2][3] = 0.0f;

    // 마지막 행 = -Eye * R (행벡터 규약)
    View.M[3][0] = -(Eye.X * View.M[0][0] + Eye.Y * View.M[0][1] + Eye.Z * View.M[0][2]);
    View.M[3][1] = -(Eye.X * View.M[1][0] + Eye.Y * View.M[1][1] + Eye.Z * View.M[1][2]);
    View.M[3][2] = -(Eye.X * View.M[2][0] + Eye.Y * View.M[2][1] + Eye.Z * View.M[2][2]);
    View.M[3][3] = 1.0f;

    return View;
}


inline FMatrix FMatrix::PerspectiveFovLH(float FovY, float Aspect, float Zn, float Zf)
{
    float YScale = 1.0f / std::tan(FovY * 0.5f);
    float XScale = YScale / Aspect;

    FMatrix proj{};
    proj.M[0][0] = XScale;
    proj.M[1][1] = YScale;
    proj.M[2][2] = Zf / (Zf - Zn);
    proj.M[2][3] = 1.0f;
    proj.M[3][2] = (-Zn * Zf) / (Zf - Zn);
    proj.M[3][3] = 0.0f;
    return proj;
}

inline FMatrix FMatrix::OrthoLH(float Width, float Height, float Zn, float Zf)
{
    // 기본 방어: 0 또는 역Z 방지
    const float W = (Width != 0.0f) ? Width : 1e-6f;
    const float H = (Height != 0.0f) ? Height : 1e-6f;
    const float DZ = (Zf - Zn != 0.0f) ? (Zf - Zn) : 1e-6f;

    FMatrix m = FMatrix::Identity();
    m.M[0][0] = 2.0f / W;
    m.M[1][1] = 2.0f / H;
    m.M[2][2] = 1.0f / DZ;
    m.M[3][2] = -Zn / DZ;   // 행벡터 규약: 마지막 행에 배치
    // 나머지는 Identity()로 이미 [0,0,0,1]
    return m;
}

inline FMatrix FMatrix::OffCenterOrthoLH(float Left, float Right, float Top, float Bottom, float Near, float Far)
{
    FMatrix Result{ FMatrix::Identity() };
    //[Left, Right] -> [-1, 1]
    Result.M[0][0] = 2.0f / (Right - Left);
    Result.M[3][0] = -(Right + Left) / (Right - Left);
    
    //[Bottom, Top] -> [-1, 1]
    Result.M[1][1] = 2.0f / (Top - Bottom);
    Result.M[3][1] = -(Top + Bottom) / (Top - Bottom);

    //[Near, Far] -> [0, 1]
    Result.M[2][2] = 1.0f / (Far - Near);
    Result.M[3][2] = -Near / (Far - Near);
    Result.M[3][3] = 1.0f;

    return Result;
}

// FTransform → Matrix (row-vector convention; translation in last row)
// inline FMatrix FTransform::ToMatrixWithScale() const
// {
//     float x2 = Rotation.X + Rotation.X;
//     float y2 = Rotation.Y + Rotation.Y;
//     float z2 = Rotation.Z + Rotation.Z;
//
//     float xx = Rotation.X * x2; float yy = Rotation.Y * y2; float zz = Rotation.Z * z2;
//     float xy = Rotation.X * y2; float xz = Rotation.X * z2; float yz = Rotation.Y * z2;
//     float wx = Rotation.W * x2; float wy = Rotation.W * y2; float wz = Rotation.W * z2;
//
//     float sx = Scale3D.X;
//     float sy = Scale3D.Y;
//     float sz = Scale3D.Z;
//
//     // Rows store basis vectors scaled by S; translation in last row
//     return FMatrix(
//         (1.0f - (yy + zz)) * sx, (xy + wz) * sx, (xz - wy) * sx, 0.0f,
//         (xy - wz) * sy, (1.0f - (xx + zz)) * sy, (yz + wx) * sy, 0.0f,
//         (xz + wy) * sz, (yz - wx) * sz, (1.0f - (xx + yy)) * sz, 0.0f,
//         Translation.X, Translation.Y, Translation.Z, 1.0f
//     );
// }

// Q = Q2 * Q1인데 Q1먼저 적용 되는거로 하고 싶을 때 이거 사용
inline FQuat QuatMul(const FQuat& Q2, const FQuat& Q1)
{
    return {
        Q2.W * Q1.X + Q2.X * Q1.W + Q2.Y * Q1.Z - Q2.Z * Q1.Y,
        Q2.W * Q1.Y - Q2.X * Q1.Z + Q2.Y * Q1.W + Q2.Z * Q1.X,
        Q2.W * Q1.Z + Q2.X * Q1.Y - Q2.Y * Q1.X + Q2.Z * Q1.W,
        Q2.W * Q1.W - Q2.X * Q1.X - Q2.Y * Q1.Y - Q2.Z * Q1.Z
    };
}

inline FQuat QuatFromAxisAngle(const FVector& Axis, float AngleRadian)
{
    const float H = 0.5f * AngleRadian;
    const float S = std::sin(H);
    const float C = std::cos(H);
    return { Axis.X * S, Axis.Y * S, Axis.Z * S, C };
}

inline FQuat MakeQuatLocalXYZ(float RX, float RY, float RZ)
{
    const FQuat QX = QuatFromAxisAngle({ 1,0,0 }, RX);
    const FQuat QY = QuatFromAxisAngle({ 0,1,0 }, RY);
    const FQuat QZ = QuatFromAxisAngle({ 0,0,1 }, RZ);
    return QuatMul(QuatMul(QZ, QY), QX); // q = qz * qy * qx
}

inline FMatrix MakeRotationRowMajorFromQuat(const FQuat& Q)
{
    // 비정규 안전화
    const float N = Q.X * Q.X + Q.Y * Q.Y + Q.Z * Q.Z + Q.W * Q.W;
    if (N <= 1e-8f) return FMatrix::Identity();
    const float S = 2.0f / N;

    const float XX = Q.X * Q.X * S, YY = Q.Y * Q.Y * S, ZZ = Q.Z * Q.Z * S;
    const float XY = Q.X * Q.Y * S, XZ = Q.X * Q.Z * S, YZ = Q.Y * Q.Z * S;
    const float WX = Q.W * Q.X * S, WY = Q.W * Q.Y * S, WZ = Q.W * Q.Z * S;

    FMatrix M = FMatrix::Identity();
    // row-major + 행벡터용 회전 블록
    M.M[0][0] = 1.0f - (YY + ZZ); M.M[0][1] = XY + WZ;          M.M[0][2] = XZ - WY;            M.M[0][3] = 0.0f;
    M.M[1][0] = XY - WZ;          M.M[1][1] = 1.0f - (XX + ZZ); M.M[1][2] = YZ + WX;            M.M[1][3] = 0.0f;
    M.M[2][0] = XZ + WY;          M.M[2][1] = YZ - WX;          M.M[2][2] = 1.0f - (XX + YY);   M.M[2][3] = 0.0f;
    // 마지막 행은 호출부에서 채움(평행이동 등)
    return M;
}

// 최종: S * R(qXYZ) * T  (row-major + 행벡터 규약)
// row-major + 행벡터(p' = p * M) 규약
inline FMatrix FTransform::ToMatrixWithScaleLocalXYZ() const
{
    FMatrix YUpToZUp =
    {
         0,  1,  0, 0 ,
         0,  0,  1, 0 ,
         1, 0,  0, 0 ,
         0,  0,  0, 1 
    };
    // Rotation(FQuat)은 이미 로컬 XYZ 순서로 만들어져 있다고 가정
    FMatrix R = MakeRotationRowMajorFromQuat(Rotation);

    // 행별 스케일(S * R): 각 "행"에 스케일 적용
    R.M[0][0] *= Scale3D.X; R.M[0][1] *= Scale3D.X; R.M[0][2] *= Scale3D.X;
    R.M[1][0] *= Scale3D.Y; R.M[1][1] *= Scale3D.Y; R.M[1][2] *= Scale3D.Y;
    R.M[2][0] *= Scale3D.Z; R.M[2][1] *= Scale3D.Z; R.M[2][2] *= Scale3D.Z;

    // 동차좌표 마무리 + Translation(last row)
    R.M[0][3] = 0.0f; R.M[1][3] = 0.0f; R.M[2][3] = 0.0f;
    R.M[3][0] = Translation.X;
    R.M[3][1] = Translation.Y;
    R.M[3][2] = Translation.Z;
    R.M[3][3] = 1.0f;

    return YUpToZUp * R; // 결과 = S * R(q) * T
    //return R; // 결과 = S * R(q) * T
}


//// FTransform 합성 (this * Other)
//inline FTransform FTransform::operator*(const FTransform& Other) const
//{
//    FTransform Result;
//
//    // 회전 결합
//    Result.Rotation =  Other.Rotation*Rotation;
//    Result.Rotation.Normalize();
//
//    // 스케일 결합 (component-wise)
//    Result.Scale3D = FVector(
//        Scale3D.X * Other.Scale3D.X,
//        Scale3D.Y * Other.Scale3D.Y,
//        Scale3D.Z * Other.Scale3D.Z
//    );
//
//    // 위치 결합: R*(S*Other.T) + T
//    FVector Scaled(Other.Translation.X ,
//                   Other.Translation.Y ,
//                   Other.Translation.Z);
//    FVector Rotated = Rotation.RotateVector(Scaled);
//    Rotated.X *= Scale3D.X;
//    Rotated.Y *= Scale3D.Y;
//    Rotated.Z *= Scale3D.Z;
//    Result.Translation = Translation + Rotated;
//
//    return Result;
//}



// FTransform 역변환
inline FTransform FTransform::Inverse() const
{
    // InvScale
    FVector InvScale(
        (std::fabs(Scale3D.X) > KINDA_SMALL_NUMBER) ? 1.0f / Scale3D.X : 0.0f,
        (std::fabs(Scale3D.Y) > KINDA_SMALL_NUMBER) ? 1.0f / Scale3D.Y : 0.0f,
        (std::fabs(Scale3D.Z) > KINDA_SMALL_NUMBER) ? 1.0f / Scale3D.Z : 0.0f
    );

    // InvRot = conjugate (단위 가정)
    FQuat InvRot(-Rotation.X, -Rotation.Y, -Rotation.Z, Rotation.W);

    // InvTrans = -(InvRot * (InvScale * T))
    FVector Rotated = InvRot.RotateVector(Translation);
    FVector Scaled(Rotated.X * InvScale.X,
                   Rotated.Y * InvScale.Y,
                   Rotated.Z * InvScale.Z);
   
    FVector InvTrans(-Scaled.X, -Scaled.Y, -Scaled.Z);

    FTransform Out;
    Out.Rotation = InvRot;
    Out.Scale3D = InvScale;
    Out.Translation = InvTrans;
    return Out;
}
