#pragma once

struct FDebugLine
{
	FVector Start;
	FVector End;
	FVector4 Color;
};
class FDebugDrawManager
{
public:
	static FDebugDrawManager& GetInstance();

	/**
	 * @brief 디버그용 라인 하나를 추가합니다.
	 * @param Start 선의 시작점 월드 좌표
	 * @param End 선의 끝점 월드 좌표
	 * @param Color 선의 색상 (기본값: 흰색)
	 */
	void AddLine(const FVector & InStart, const FVector & InEnd, const FVector4 & InColor = {1.0f, 1.0f, 1.0f, 1.0f});
	/**
	 * @brief 디버그용 원을 이루는 라인들을 추가합니다.
	 * @param Center 원의 중심 월드 좌표
	 * @param UpAxis 원이 그려질 평면의 법선 벡터(Normal Vector)
	 * @param Radius 원의 반지름
	 * @param NumSegments 원을 표현하기 위해 사용할 선분의 개수(값이 높을수록 원이 부드러워짐)
	 * @param Color 원의 색상 (기본값: 흰색)
	 */
	void AddCircle(const FVector & InCenter, const FVector & InUpAxis, float InRadius, int InNumSegments, const FVector4 & InColor = {1.0f, 1.0f, 1.0f, 1.0f});
	/**
	 * @brief 3D 공간에 구(Sphere)를 이루는 라인들을 추가합니다. 3개의 축(X, Y, Z)에 원을 그려 구를 근사
	 * @param Center 구의 중심 월드 좌표
	 * @param Radius 구의 반지름
	 * @param NumSegments 구를 구성하는 각 원을 표현할 선분의 개수
	 * @param Color 구의 색상(기본값: 흰색)
	 */
	void AddSphere(const FVector& InCenter, float InRadius, int InNumSegments, const FVector4& InColor = { 1.0f, 1.0f, 1.0f, 1.0f });
	/**
	 * @brief 3D 공간에 원뿔(Cone)를 이루는 라인들을 추가합니다. 
	 * @param ApexPosition 원뿔의 꼭짓점 월드 좌표
	 * @param Direction 원뿔이 향하는 방향 벡터 (정규화될 필요 X)
	 * @param Height 원뿔의 꼭짓점부터 밑면 중심까지의 높이
	 * @param AngleInDegrees 원뿔의 꼭짓점에서 밑면 가장자리로 퍼지는 각도(절반각) (단위: Degree)
	 * @param NumSegments 원뿔의 밑면 원을 표현할 선분의 개수
	 * @param Color 원뿔의 색상입 (기본값: 흰색)
	 */
	void AddCone(const FVector & InApexPosition, const FVector & InDirection, float InHeight, float InAngleInDegrees, int InNumSegments, const FVector4 & InColor = {1.0f, 1.0f, 1.0f, 1.0f});
	/**
	 * @brief 3D 공간에 축 정렬된 상자(AABB)를 그립니다.
	 * @param Center 상자의 중심 월드 좌표
	 * @param Extents 상자 중심에서 각 면까지의 거리(반경)
	 * @param Color 상자의 색상 (기본값: 흰색)
	 */
	void AddBox(const FVector & InCenter, const FVector & InExtents, const FVector4 & InColor = {1.0f, 1.0f, 1.0f, 1.0f});
	
	/**
	* @brief 이번 프레임에 그릴 모든 라인 데이터 반환
	*/
	const TArray<FDebugLine>& GetLines() const;

	/**
	* @brief 저장된 디버그 라인을 비웁니다.
	*/
	void ClearLines();
	
private:
	FDebugDrawManager() = default;
	~FDebugDrawManager() = default;
	FDebugDrawManager(const FDebugDrawManager&) = delete;
	FDebugDrawManager& operator=(const FDebugDrawManager&) = delete;

	// 이번 프레임에 그릴 모든 디버그 라인 저장
	TArray<FDebugLine> DebugLines;

};

