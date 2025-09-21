#pragma once
#include "UEContainer.h"
#include "Vector.h"
#include "Enums.h"

// Raw Data
struct FObjInfo
{
    // Vertex List
    // UV List
    // Normal List
    // Vertex Index List
    // UV Index List
    // Normal Index List
    // other...

    TArray<FVector> Positions;
    TArray<FVector2D> TexCoords;
    TArray<FVector> Normals;

    TArray<uint32> PositionIndices;
    TArray<uint32> TexCoordIndices;
    TArray<uint32> NormalIndices;

    // TODO: Material 관련 정보 추가 필요
    // Material List
    // Texture List
    // other...
    TArray<FWideString> MaterialNames; // == 강의 글의 meshMaterials
    TArray<uint32> GroupIndexStartArray; // i번째 Group이 사용하는 indices 범위: GroupIndexStartArray[i] ~ GroupIndexStartArray[i+1]
    TArray<uint32> GroupMaterialArray; // i번쩨 Group이 사용하는 Materials 인덱스 넘버

    FString ObjFileName;
};



struct FObjImporter
{
    // Obj Parsing (*.obj to FObjInfo)
    // Material Parsing (*.obj to MaterialInfo)
    // Convert the Raw data to Cooked data (FStaticMesh)
    // other...

    // TODO: Material 파싱도 필요
    // TODO: 변수이름 가독성 있게 재설정
public:
    static bool LoadObjModel(const FWideString& InFileName, FObjInfo* const OutObjInfo, TArray<FObjMaterialInfo>& const OutMaterialInfos, bool bIsRHCoordSys, bool bComputeNormals)
    {
        // mtl 파싱할 때 필요한 정보들. 이거를 함수 밖으로 보내줘야 할수도? obj 파싱하면서 저장.(아래 링크 기반) 나중에 형식 바뀔수도 있음
        // https://www.braynzarsoft.net/viewtutorial/q16390-22-loading-static-3d-models-obj-format
        //TArray<uint32> GroupIndexStartArray; // i번째 Group이 사용하는 indices 범위: GroupIndexStartArray[i] ~ GroupIndexStartArray[i+1]
        //TArray<uint32> GroupMaterialArray; // i번쩨 Group이 사용하는 Materials 인덱스 넘버
        //TArray<FObjMaterialInfo> MaterialInfos; // Material 정보들
        // TArray<ID3D11ShaderResourceView*> meshSRV; // MaterialInfos의 각 원소에 저장된 index로 여기서 참조.
        uint32 subsetCount = 0;

        FWideString MtlFileName;

        // Make sure we have a default if no tex coords or normals are defined
        bool bHasTexcoord = false;
        bool bHasNormal = false;

        FWideString MaterialNameTemp;
        uint32 VertexPositionIndexTemp;
        uint32 VertexTexIndexTemp;
        uint32 VertexNormalIndexTemp;

        WIDECHAR CheckChar;
        FWideString Face;
        uint32 VIndex = 0; // 현재 파싱중인 vertex의 넘버(start: 0. 중복 고려x)
        uint32 TriangleCount = 0; // 현재 face 한줄에 들어있는 triangle 개수
        uint32 TotalVerts = 0; // 현재까지 파싱된 vertex 개수(중복 제거 후)
        uint32 MeshTriangles = 0; // 현재까지 파싱된 Triangle 개수

        std::wifstream FileIn(InFileName.c_str());

        if (!FileIn)
        {
            UE_LOG("파일명 %s가 존재하지 않습니다!", InFileName.c_str());
            return false;
        }

        // obj 파싱 시작
        OutObjInfo->ObjFileName = FString(InFileName.begin(), InFileName.end()); // 아스키 코드라고 가정

        FWideString line;
        while (std::getline(FileIn, line))
        {
            if (line.empty()) continue;

            // 주석(#) 처리
            if (line[0] == L'#')   // wide literal
                continue;

            if (line.rfind(L"v ", 0) == 0) // 정점 좌표 (v x y z)
            {
                std::wstringstream wss(line.substr(2));
                float vx, vy, vz;
                wss >> vx >> vy >> vz;

                if (bIsRHCoordSys)
                    OutObjInfo->Positions.push_back(FVector(vx, -vy, vz));
                else
                    OutObjInfo->Positions.push_back(FVector(vx, vy, vz));
            }
            else if (line.rfind(L"vt ", 0) == 0) // 텍스처 좌표 (vt u v)
            {
                std::wstringstream wss(line.substr(3));
                float u, v;
                wss >> u >> v;

                if (bIsRHCoordSys)
                    OutObjInfo->TexCoords.push_back(FVector2D(u, 1.0f - v));
                else
                    OutObjInfo->TexCoords.push_back(FVector2D(u, v));

                bHasTexcoord = true;
            }
            else if (line.rfind(L"vn ", 0) == 0) // 법선 (vn x y z)
            {
                std::wstringstream wss(line.substr(3));
                float nx, ny, nz;
                wss >> nx >> ny >> nz;

                if (bIsRHCoordSys)
                    OutObjInfo->Normals.push_back(FVector(nx, -ny, nz));
                else
                    OutObjInfo->Normals.push_back(FVector(nx, ny, nz));

                bHasNormal = true;
            }
            //else if (line.rfind(L"g ", 0) == 0) // 그룹 (g groupName)
            //{
            //    GroupIndexStartArray.push_back(VIndex);
            //    subsetCount++;
            //}
            else if (line.rfind(L"f ", 0) == 0) // 면 (f v1/vt1/vn1 v2/vt2/vn2 ...)
            {
                Face = line.substr(2); // ex: "3/2/2 3/3/2 3/4/2 "

                if (Face.length() <= 0)
                {
                    continue;
                }

                // 1) TriangleCount 구하기
                std::wstringstream wss(Face);
                FWideString VertexDef; // ex: 3/2/2
                //uint32 VertexCount = 0;
                //while (wss >> VertexDef)
                //{
                //    VertexCount++;
                //}
                //TriangleCount = (VertexCount >= 3) ? VertexCount - 2 : 0;

                //wss.clear();              // 스트림 상태 플래그 초기화 (EOF, fail 등)
                //wss.seekg(0, std::ios::beg); // 읽기 위치를 맨 앞으로 이동

                // 2) 실제 Face 파싱
                TArray<FFaceVertex> LineFaceVertices;
                while (wss >> VertexDef)
                {
                    FFaceVertex FaceVertex = ParseVertexDef(VertexDef);
                    LineFaceVertices.push_back(FaceVertex);
                }

                //3) FaceVertices에 그대로 파싱된 걸로, 다시 트라이앵글에 맞춰 인덱스 배열들에 넣기
                for (uint32 i = 0; i < 3; ++i)
                {
                    OutObjInfo->PositionIndices.push_back(LineFaceVertices[i].PositionIndex);
                    OutObjInfo->TexCoordIndices.push_back(LineFaceVertices[i].TexCoordIndex);
                    OutObjInfo->NormalIndices.push_back(LineFaceVertices[i].NormalIndex);
                    ++VIndex;
                }
                ++MeshTriangles;

                for (uint32 i = 3; i < LineFaceVertices.size(); ++i)
                {
                    OutObjInfo->PositionIndices.push_back(LineFaceVertices[0].PositionIndex);
                    OutObjInfo->TexCoordIndices.push_back(LineFaceVertices[0].TexCoordIndex);
                    OutObjInfo->NormalIndices.push_back(LineFaceVertices[0].NormalIndex);
                    ++VIndex;

                    OutObjInfo->PositionIndices.push_back(LineFaceVertices[i-1].PositionIndex);
                    OutObjInfo->TexCoordIndices.push_back(LineFaceVertices[i-1].TexCoordIndex);
                    OutObjInfo->NormalIndices.push_back(LineFaceVertices[i-1].NormalIndex);
                    ++VIndex;

                    OutObjInfo->PositionIndices.push_back(LineFaceVertices[i].PositionIndex);
                    OutObjInfo->TexCoordIndices.push_back(LineFaceVertices[i].TexCoordIndex);
                    OutObjInfo->NormalIndices.push_back(LineFaceVertices[i].NormalIndex);
                    ++VIndex;

                    ++MeshTriangles;
                }
            }
            else if (line.rfind(L"mtllib ", 0) == 0)
            {
                MtlFileName = line.substr(7);
            }
            else if (line.rfind(L"usemtl ", 0) == 0)
            {
                MaterialNameTemp = line.substr(7);
                OutObjInfo->MaterialNames.push_back(MaterialNameTemp);

                // material 하나 당 group 하나라고 가정. 현재 단계에서는 usemtl로 group을 분리하는 게 편함.
                OutObjInfo->GroupIndexStartArray.push_back(VIndex);
                subsetCount++;
            }
            else
            {
                UE_LOG("파일명 %s를 파싱하는 중 다음과 같은 unknown symbol을 발견했습니다: %s", InFileName, line);
            }
        }

        
        // GroupIndexStartArray 마무리 작업
        if (subsetCount == 0) //Check to make sure there is at least one subset
        {
            OutObjInfo->GroupIndexStartArray.push_back(0);		//Start index for this subset
            subsetCount++;
        }
        OutObjInfo->GroupIndexStartArray.push_back(VIndex); //There won't be another index start after our last subset, so set it here

        //sometimes "g" is defined at the very top of the file, then again before the first group of faces.
        //This makes sure the first subset does not conatain "0" indices.
        if (OutObjInfo->GroupIndexStartArray[1] == 0)
        {
            OutObjInfo->GroupIndexStartArray.erase(OutObjInfo->GroupIndexStartArray.begin() + 1);
            subsetCount--;
        }

        // default 값 설정
        if (!bHasNormal)
        {
            OutObjInfo->Normals.push_back(FVector(0.0f, 0.0f, 0.0f));
        }
        if (!bHasTexcoord)
        {
            OutObjInfo->TexCoords.push_back(FVector2D(0.0f, 0.0f));
        }

        FileIn.close();

        // TODO: Normal 다시 계산

        // TODO: Material 관련 코드
        FileIn.open(MtlFileName.c_str());

        if (!FileIn)
        {
            UE_LOG("파일명 %s가 존재하지 않습니다!", InFileName.c_str());
            return false;
        }

        // obj 파싱 시작
        //OutObjInfo->ObjFileName = FString(InFileName.begin(), InFileName.end()); // 아스키 코드라고 가정

        OutMaterialInfos.reserve(OutObjInfo->MaterialNames.size());
        /*OutMaterialInfos->resize(OutObjInfo->MaterialNames.size());*/
        uint32 MatCount = OutMaterialInfos.size();
        FWideString line;
        while (std::getline(FileIn, line))
        {
            if (line.empty()) continue;

            // 주석(#) 처리
            if (line[0] == L'#')   // wide literal
                continue;

            if (line.rfind(L"Kd ", 0) == 0)
            {
                std::wstringstream wss(line.substr(3));
                float vx, vy, vz;
                wss >> vx >> vy >> vz;

                OutMaterialInfos[MatCount - 1].DiffuseColor = FVector(vx, vy, vz);
            }
            else if (line.rfind(L"Ka ", 0) == 0)
            {
                std::wstringstream wss(line.substr(3));
                float vx, vy, vz;
                wss >> vx >> vy >> vz;

                OutMaterialInfos[MatCount - 1].AmbientColor = FVector(vx, vy, vz);
            }
            else if (line.rfind(L"Tr ", 0) == 0)
            {
                std::wstringstream wss(line.substr(3));
                float value;
                wss >> value;

                OutMaterialInfos[MatCount - 1].Transparency = value;
            }
            else if (line.rfind(L"d ", 0) == 0)
            {
                std::wstringstream wss(line.substr(3));
                float value;
                wss >> value;

                OutMaterialInfos[MatCount - 1].Transparency = 1.0f - value;
            }
            else if (line.rfind(L"map_kd ", 0) == 0) // 면 (f v1/vt1/vn1 v2/vt2/vn2 ...)
            {
                 = line.substr(7); // ex: "3/2/2 3/3/2 3/4/2 "

                if (Face.length() <= 0)
                {
                    continue;
                }

                // 1) TriangleCount 구하기
                std::wstringstream wss(Face);
                FWideString VertexDef; // ex: 3/2/2
                //uint32 VertexCount = 0;
                //while (wss >> VertexDef)
                //{
                //    VertexCount++;
                //}
                //TriangleCount = (VertexCount >= 3) ? VertexCount - 2 : 0;

                //wss.clear();              // 스트림 상태 플래그 초기화 (EOF, fail 등)
                //wss.seekg(0, std::ios::beg); // 읽기 위치를 맨 앞으로 이동

                // 2) 실제 Face 파싱
                TArray<FFaceVertex> LineFaceVertices;
                while (wss >> VertexDef)
                {
                    FFaceVertex FaceVertex = ParseVertexDef(VertexDef);
                    LineFaceVertices.push_back(FaceVertex);
                }

                //3) FaceVertices에 그대로 파싱된 걸로, 다시 트라이앵글에 맞춰 인덱스 배열들에 넣기
                for (uint32 i = 0; i < 3; ++i)
                {
                    OutObjInfo->PositionIndices.push_back(LineFaceVertices[i].PositionIndex);
                    OutObjInfo->TexCoordIndices.push_back(LineFaceVertices[i].TexCoordIndex);
                    OutObjInfo->NormalIndices.push_back(LineFaceVertices[i].NormalIndex);
                    ++VIndex;
                }
                ++MeshTriangles;

                for (uint32 i = 3; i < LineFaceVertices.size(); ++i)
                {
                    OutObjInfo->PositionIndices.push_back(LineFaceVertices[0].PositionIndex);
                    OutObjInfo->TexCoordIndices.push_back(LineFaceVertices[0].TexCoordIndex);
                    OutObjInfo->NormalIndices.push_back(LineFaceVertices[0].NormalIndex);
                    ++VIndex;

                    OutObjInfo->PositionIndices.push_back(LineFaceVertices[i - 1].PositionIndex);
                    OutObjInfo->TexCoordIndices.push_back(LineFaceVertices[i - 1].TexCoordIndex);
                    OutObjInfo->NormalIndices.push_back(LineFaceVertices[i - 1].NormalIndex);
                    ++VIndex;

                    OutObjInfo->PositionIndices.push_back(LineFaceVertices[i].PositionIndex);
                    OutObjInfo->TexCoordIndices.push_back(LineFaceVertices[i].TexCoordIndex);
                    OutObjInfo->NormalIndices.push_back(LineFaceVertices[i].NormalIndex);
                    ++VIndex;

                    ++MeshTriangles;
                }
            }
            else if (line.rfind(L"mtllib ", 0) == 0)
            {
                MtlFileName = line.substr(7);
            }
            else if (line.rfind(L"usemtl ", 0) == 0)
            {
                MaterialNameTemp = line.substr(7);
                OutObjInfo->MaterialNames.push_back(MaterialNameTemp);
            }
            else
            {
                UE_LOG("파일명 %s를 파싱하는 중 다음과 같은 unknown symbol을 발견했습니다: %s", InFileName, line);
            }
        }
    }

    // TODO: Material 파싱도 필요
    static void ConvertToStaticMesh(const FObjInfo& InObjInfo, FStaticMesh* const OutStaticMesh)
    {
        // TODO: indices도 강의자료에 f 파싱에서 하던 부분 여기서 해야됨. 2중 for문
        // TODO: ~Indices들도 지금 중복제거가 안되어있으니, 중복 제거 후 넣어야 함.
        OutStaticMesh->PathFileName = InObjInfo.ObjFileName;

        uint32 NumDuplicatedVertex = InObjInfo.PositionIndices.size();
        for (int CurIndex = 0; CurIndex < NumDuplicatedVertex; ++CurIndex)
        {
            bool bIsDuplicate = false;
            /*for (int PreIndex = 0; PreIndex < CurIndex; ++PreIndex)
            {
                if (InObjInfo.PositionIndices[CurIndex] == InObjInfo.PositionIndices[PreIndex] && InObjInfo.TexCoordIndices[CurIndex] == InObjInfo.TexCoordIndices[PreIndex])
                {
                    OutStaticMesh->Indices.push_back(PreIndex);
                    bIsDuplicate = true;
                    break;
                }
            }*/

            const uint64 VertexLen = OutStaticMesh->Vertices.size();
            for (uint32 VertexIndex = 0; VertexIndex < VertexLen; ++VertexIndex)
            {
                if (OutStaticMesh->Vertices[VertexIndex].pos == InObjInfo.Positions[InObjInfo.PositionIndices[CurIndex]] && OutStaticMesh->Vertices[VertexIndex].tex == InObjInfo.TexCoords[InObjInfo.TexCoordIndices[CurIndex]])
                {
                    OutStaticMesh->Indices.push_back(VertexIndex);
                    bIsDuplicate = true;
                    break;
                }
            }

            if (!bIsDuplicate)
            {
                FVector Pos = InObjInfo.Positions[InObjInfo.PositionIndices[CurIndex]];
                FVector Normal = InObjInfo.Normals[InObjInfo.NormalIndices[CurIndex]];
                FVector2D TexCoord = InObjInfo.TexCoords[InObjInfo.TexCoordIndices[CurIndex]];
                FVector4 Color = FVector4(1.0f, 1.0f, 1.0f, 1.0f); // default color
                FNormalVertex NormalVertex = FNormalVertex(Pos, Normal, Color, TexCoord);
                OutStaticMesh->Vertices.push_back(NormalVertex);

                OutStaticMesh->Indices.push_back(OutStaticMesh->Vertices.size() - 1);
            }
        }
    }

private:
    struct FFaceVertex
    {
        uint32 PositionIndex;
        uint32 TexCoordIndex;
        uint32 NormalIndex;
    };

    //없는 건 0으로 넣음
    static FFaceVertex ParseVertexDef(const FWideString& InVertexDef)
    {
        FWideString vertPart;
        uint32 whichPart = 0;

        uint32 VertexPositionIndexTemp;
        uint32 VertexTexIndexTemp;
        uint32 VertexNormalIndexTemp;
        for (int j = 0; j < InVertexDef.length(); ++j)
        {
            if (InVertexDef[j] != '/')	//If there is no divider "/", add a char to our vertPart
                vertPart += InVertexDef[j];

            //If the current char is a divider "/", or its the last character in the string
            if (InVertexDef[j] == '/' || j == InVertexDef.length() - 1)
            {
                std::wistringstream wstringToInt(vertPart);	//Used to convert wstring to int

                if (whichPart == 0)	//If vPos
                {
                    wstringToInt >> VertexPositionIndexTemp;
                    VertexPositionIndexTemp -= 1;		//subtract one since c++ arrays start with 0, and obj start with 1

                    //Check to see if the vert pos was the only thing specified
                    if (j == InVertexDef.length() - 1)
                    {
                        VertexNormalIndexTemp = 0;
                        VertexTexIndexTemp = 0;
                    }
                }

                else if (whichPart == 1)	//If vTexCoord
                {
                    if (vertPart != L"")	//Check to see if there even is a tex coord
                    {
                        wstringToInt >> VertexTexIndexTemp;
                        VertexTexIndexTemp -= 1;	//subtract one since c++ arrays start with 0, and obj start with 1
                    }
                    else	//If there is no tex coord, make a default
                        VertexTexIndexTemp = 0;

                    //If the cur. char is the second to last in the string, then
                    //there must be no normal, so set a default normal
                    if (j == InVertexDef.length() - 1)
                        VertexNormalIndexTemp = 0;

                }
                else if (whichPart == 2)	//If vNorm
                {
                    wstringToInt >> VertexNormalIndexTemp;
                    VertexNormalIndexTemp -= 1;		//subtract one since c++ arrays start with 0, and obj start with 1
                }

                vertPart = L"";	//Get ready for next vertex part
                whichPart++;	//Move on to next vertex part					
            }
        }

        FFaceVertex Result = FFaceVertex(VertexPositionIndexTemp, VertexTexIndexTemp, VertexNormalIndexTemp);
        return Result;
    }

};

class UStaticMesh;

class FObjManager
{
private:
    static TMap<FString, FStaticMesh*> ObjStaticMeshMap;

public:
    static FStaticMesh* LoadObjStaticMeshAsset(const FString& PathFileName);

    static UStaticMesh* LoadObjStaticMesh(const FString& PathFileName);
};