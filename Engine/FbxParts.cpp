#include "FbxParts.h"
#include "Fbx.h"
#include "Global.h"
#include "Direct3D.h"
#include "Camera.h"

//コンストラクタ
FbxParts::FbxParts():
	ppIndexBuffer_(nullptr), pMaterial_(nullptr), 
	pVertexBuffer_(nullptr), pConstantBuffer_(nullptr),
	pVertexData_(nullptr), ppIndexData_(nullptr)
{
}

//デストラクタ
FbxParts::~FbxParts()
{
	SAFE_DELETE_ARRAY(pBoneArray_);
	SAFE_DELETE_ARRAY(ppCluster_);

	if (pWeightArray_ != nullptr)
	{
		for (DWORD i = 0; i < vertexCount_; i++)
		{
			SAFE_DELETE_ARRAY(pWeightArray_[i].pBoneIndex);
			SAFE_DELETE_ARRAY(pWeightArray_[i].pBoneWeight);
		}
		SAFE_DELETE_ARRAY(pWeightArray_);
	}



	SAFE_DELETE_ARRAY(pVertexData_);
	for (DWORD i = 0; i < materialCount_; i++)
	{
		SAFE_RELEASE(ppIndexBuffer_[i]);
		SAFE_DELETE(ppIndexData_[i]);
		SAFE_DELETE(pMaterial_[i].pTexture);

	}
	SAFE_DELETE_ARRAY(ppIndexBuffer_);
	SAFE_DELETE_ARRAY(ppIndexData_);
	SAFE_DELETE_ARRAY(pMaterial_);

	SAFE_RELEASE(pVertexBuffer_);
	SAFE_RELEASE(pConstantBuffer_);
}

//FBXファイルから情報をロードして諸々準備する
HRESULT FbxParts::Init(FbxNode *pNode)
{
	//ノードからメッシュの情報を取得
	FbxMesh* mesh = pNode->GetMesh();

	//各情報の個数を取得
	vertexCount_ = mesh->GetControlPointsCount();			//頂点の数
	polygonCount_ = mesh->GetPolygonCount();				//ポリゴンの数
	polygonVertexCount_ = mesh->GetPolygonVertexCount();	//ポリゴン頂点インデックス数 


	InitVertex(mesh);		//頂点バッファ準備
	InitMaterial(pNode);	//マテリアル準備
	InitIndex(mesh);		//インデックスバッファ準備
	InitSkelton(mesh);		//骨の情報を準備
	IntConstantBuffer();	//コンスタントバッファ（シェーダーに情報を送るやつ）準備

	return E_NOTIMPL;
}


//頂点バッファ準備
void FbxParts::InitVertex(fbxsdk::FbxMesh * mesh)
{
	pVertexData_ = new VERTEX[vertexCount_];

	for (DWORD poly = 0; poly < polygonCount_; poly++)
	{
		//3頂点分
		for (int vertex = 0; vertex < 3; vertex++)
		{
			int index = mesh->GetPolygonVertex(poly, vertex);

			/////////////////////////頂点の位置/////////////////////////////////////
			FbxVector4 pos = mesh->GetControlPointAt(index);
			pVertexData_[index].position = XMFLOAT3((float)pos[0], (float)pos[1], (float)pos[2]);

			/////////////////////////頂点の法線/////////////////////////////////////
			FbxVector4 Normal;
			mesh->GetPolygonVertexNormal(poly, vertex, Normal);	//ｉ番目のポリゴンの、ｊ番目の頂点の法線をゲット
			pVertexData_[index].normal = XMFLOAT3((float)Normal[0], (float)Normal[1], (float)Normal[2]);

			///////////////////////////頂点のＵＶ/////////////////////////////////////
			FbxLayerElementUV * pUV = mesh->GetLayer(0)->GetUVs();
			int uvIndex = mesh->GetTextureUVIndex(poly, vertex, FbxLayerElement::eTextureDiffuse);
			FbxVector2  uv = pUV->GetDirectArray().GetAt(uvIndex);
			pVertexData_[index].uv = XMFLOAT3((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f);
		}
	}


	///////////////////////////頂点のＵＶ/////////////////////////////////////
	int m_dwNumUV = mesh->GetTextureUVCount();
	FbxLayerElementUV * pUV = mesh->GetLayer(0)->GetUVs();
	if (m_dwNumUV > 0 && pUV->GetMappingMode() == FbxLayerElement::eByControlPoint)
	{
		for (int k = 0; k < m_dwNumUV; k++)
		{
			FbxVector2 uv = pUV->GetDirectArray().GetAt(k);
			pVertexData_[k].uv = XMFLOAT3((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f);
		}
	}


	// 頂点データ用バッファの設定
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(VERTEX) * mesh->GetControlPointsCount();
	bd_vertex.Usage = D3D11_USAGE_DYNAMIC;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	data_vertex.pSysMem = pVertexData_;
	Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);


}

//マテリアル準備
void FbxParts::InitMaterial(fbxsdk::FbxNode * pNode)
{

	// マテリアルバッファの生成
	materialCount_ = pNode->GetMaterialCount();
	pMaterial_ = new MATERIAL[materialCount_];



	for (DWORD i = 0; i < materialCount_; i++)
	{
		ZeroMemory(&pMaterial_[i], sizeof(pMaterial_[i]));

		// フォンシェーディングを想定したマテリアルバッファの抽出
		FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);
		FbxSurfacePhong* pPhong = (FbxSurfacePhong*)pMaterial;

		// 環境光＆拡散反射光＆鏡面反射光の反射成分値を取得
		FbxDouble3  ambient = FbxDouble3(0, 0, 0);
		FbxDouble3  diffuse = FbxDouble3(0, 0, 0);
		FbxDouble3  specular = FbxDouble3(0, 0, 0);
		ambient = pPhong->Ambient;
		diffuse = pPhong->Diffuse;



		// 環境光＆拡散反射光＆鏡面反射光の反射成分値をマテリアルバッファにコピー
		pMaterial_[i].ambient = XMFLOAT4((float)ambient[0], (float)ambient[1], (float)ambient[2], 1.0f);
		pMaterial_[i].diffuse = XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f);
		pMaterial_[i].specular = XMFLOAT4(0, 0, 0, 0);
		pMaterial_[i].shininess = 0;


		if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			specular = pPhong->Specular;
			pMaterial_[i].specular = XMFLOAT4((float)specular[0], (float)specular[1], (float)specular[2], 1.0f);
			pMaterial_[i].shininess = (float)pPhong->Shininess;
		}


		InitTexture(pMaterial, i);

	}

}

//テクスチャ準備
void FbxParts::InitTexture(fbxsdk::FbxSurfaceMaterial * pMaterial, const DWORD &i)
{
	pMaterial_[i].pTexture = nullptr;


	// テクスチャー情報の取得
	FbxProperty  lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

	//テクスチャの数
	int fileTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();

	if (fileTextureCount > 0)
	{
		FbxFileTexture* texture = lProperty.GetSrcObject<FbxFileTexture>(0);

		//ファイル名+拡張だけにする
		char name[_MAX_FNAME];	//ファイル名
		char ext[_MAX_EXT];		//拡張子
		_splitpath_s(texture->GetRelativeFileName(), nullptr, 0, nullptr, 0, name, _MAX_FNAME, ext, _MAX_EXT);
		wsprintf(name, "%s%s", name, ext);



		pMaterial_[i].pTexture = new Texture;
		pMaterial_[i].pTexture->Load(name);


	}
}

//インデックスバッファ準備
void FbxParts::InitIndex(fbxsdk::FbxMesh * mesh)
{
	// マテリアルの数だけインデックスバッファーを作成
	ppIndexBuffer_ = new ID3D11Buffer*[materialCount_];
	ppIndexData_ = new DWORD*[materialCount_];

	

	int count = 0;

	// マテリアルから「ポリゴン平面」の情報を抽出する
	for (DWORD i = 0; i < materialCount_; i++)
	{
		count = 0;
		DWORD *pIndex = new DWORD[polygonCount_ * 3];
		ZeroMemory(&pIndex[i], sizeof(pIndex[i]));

		// ポリゴンを構成する三角形平面が、
		// 「頂点バッファ」内のどの頂点を利用しているかを調べる
		for (DWORD j = 0; j < polygonCount_; j++)
		{
			FbxLayerElementMaterial *   mtl = mesh->GetLayer(0)->GetMaterials();
			int mtlId = mtl->GetIndexArray().GetAt(j);
			if (mtlId == i)
			{
				for (DWORD k = 0; k < 3; k++)
				{
					pIndex[count + k] = mesh->GetPolygonVertex(j, k);
				}
				count += 3;
			}
		}

		// インデックスバッファを生成する
		D3D11_BUFFER_DESC   bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(int) * count;
		bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pIndex;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		if (FAILED(Direct3D::pDevice_->CreateBuffer(&bd, &InitData, &ppIndexBuffer_[i])))
		{
			//MessageBox(0, "インデックスバッファの生成に失敗", fbxFileName, MB_OK);
			//return FALSE;
		}
		pMaterial_[i].polygonCount = count / 3;
		ppIndexData_[i] = new DWORD[count];
		memcpy(ppIndexData_[i], pIndex, sizeof(DWORD) * count);
		SAFE_DELETE_ARRAY(pIndex);
	}

}

//骨の情報を準備
void FbxParts::InitSkelton(FbxMesh* pMesh)
{
	// デフォーマ情報（ボーンとモデルの関連付け）を取得
	FbxDeformer* pDeformer = pMesh->GetDeformer(0);

	if (pDeformer == nullptr)
	{
		// ボーン情報なし
		pSkinInfo_ = nullptr;
		numBone_ = 0;
		return;
	}

	// スキンメッシュ情報を取得
	pSkinInfo_ = static_cast<FbxSkin*>(pDeformer);

	// ボーン数を取得
	numBone_ = pSkinInfo_->GetClusterCount();

	if (numBone_ <= 0)
	{
		pSkinInfo_ = nullptr;
		numBone_ = 0;
		return;
	}

	// 頂点からポリゴンを逆引きする情報
	struct POLY_INDEX
	{
		int* polyIndex;
		int* vertexIndex;
		int numRef;
	};

	POLY_INDEX* polyTable = new POLY_INDEX[vertexCount_];

	for (DWORD i = 0; i < vertexCount_; i++)
	{
		polyTable[i].polyIndex =
			new int[polygonCount_ * 3];

		polyTable[i].vertexIndex =
			new int[polygonCount_ * 3];

		polyTable[i].numRef = 0;

		ZeroMemory(
			polyTable[i].polyIndex,
			sizeof(int) * polygonCount_ * 3
		);

		ZeroMemory(
			polyTable[i].vertexIndex,
			sizeof(int) * polygonCount_ * 3
		);

		// ポリゴン間で共有されている頂点を列挙
		for (DWORD polygonIndex = 0;
			polygonIndex < polygonCount_;
			polygonIndex++)
		{
			for (int polygonVertex = 0;
				polygonVertex < 3;
				polygonVertex++)
			{
				if (pMesh->GetPolygonVertex(
					polygonIndex,
					polygonVertex) ==
					static_cast<int>(i))
				{
					const int referenceIndex =
						polyTable[i].numRef;

					polyTable[i]
						.polyIndex[referenceIndex] =
						static_cast<int>(polygonIndex);

					polyTable[i]
						.vertexIndex[referenceIndex] =
						polygonVertex;

					polyTable[i].numRef++;
				}
			}
		}
	}

	// 各ボーンのクラスタを取得
	ppCluster_ = new FbxCluster * [numBone_];

	for (int boneIndex = 0;
		boneIndex < numBone_;
		boneIndex++)
	{
		ppCluster_[boneIndex] =
			pSkinInfo_->GetCluster(boneIndex);
	}

	// 頂点ごとのウェイト情報を準備
	pWeightArray_ = new FbxParts::Weight[vertexCount_];

	for (DWORD vertexIndex = 0;
		vertexIndex < vertexCount_;
		vertexIndex++)
	{
		pWeightArray_[vertexIndex].posOrigin =
			pVertexData_[vertexIndex].position;

		pWeightArray_[vertexIndex].normalOrigin =
			pVertexData_[vertexIndex].normal;

		pWeightArray_[vertexIndex].pBoneIndex =
			new int[numBone_];

		pWeightArray_[vertexIndex].pBoneWeight =
			new float[numBone_];

		for (int influence = 0;
			influence < numBone_;
			influence++)
		{
			pWeightArray_[vertexIndex]
				.pBoneIndex[influence] = -1;

			pWeightArray_[vertexIndex]
				.pBoneWeight[influence] = 0.0f;
		}
	}

	// ============================================================
	// 全ボーンのウェイトを登録
	// ============================================================
	for (int boneIndex = 0;
		boneIndex < numBone_;
		boneIndex++)
	{
		FbxCluster* cluster =
			ppCluster_[boneIndex];

		if (cluster == nullptr)
		{
			continue;
		}

		const int influenceVertexCount =
			cluster->GetControlPointIndicesCount();

		int* controlPointIndices =
			cluster->GetControlPointIndices();

		double* controlPointWeights =
			cluster->GetControlPointWeights();

		if (controlPointIndices == nullptr ||
			controlPointWeights == nullptr)
		{
			continue;
		}

		for (int influenceVertex = 0;
			influenceVertex < influenceVertexCount;
			influenceVertex++)
		{
			const int vertexIndex =
				controlPointIndices[influenceVertex];

			const float weight =
				static_cast<float>(
					controlPointWeights[influenceVertex]
					);

			// 不正な頂点番号を除外
			if (vertexIndex < 0 ||
				vertexIndex >=
				static_cast<int>(vertexCount_))
			{
				continue;
			}

			// 影響がほぼないウェイトは登録しない
			if (weight <= 0.000001f)
			{
				continue;
			}

			// 保存するのは最大4ボーン
			const int maxInfluence =
				(numBone_ < 4) ? numBone_ : 4;

			// ウェイトの大きい順に挿入
			for (int insertIndex = 0;
				insertIndex < maxInfluence;
				insertIndex++)
			{
				if (weight >
					pWeightArray_[vertexIndex]
					.pBoneWeight[insertIndex])
				{
					// 後ろへ1つずつ移動
					for (int shiftIndex =
						maxInfluence - 1;
						shiftIndex > insertIndex;
						shiftIndex--)
					{
						pWeightArray_[vertexIndex]
							.pBoneIndex[shiftIndex] =
							pWeightArray_[vertexIndex]
							.pBoneIndex[
								shiftIndex - 1
							];

						pWeightArray_[vertexIndex]
							.pBoneWeight[shiftIndex] =
							pWeightArray_[vertexIndex]
							.pBoneWeight[
								shiftIndex - 1
							];
					}

					pWeightArray_[vertexIndex]
						.pBoneIndex[insertIndex] =
						boneIndex;

					pWeightArray_[vertexIndex]
						.pBoneWeight[insertIndex] =
						weight;

					break;
				}
			}
		}
	}

	// ============================================================
	// 全ボーンの登録が完了してからウェイトを正規化
	// ============================================================
	const int maxInfluence =
		(numBone_ < 4) ? numBone_ : 4;

	for (DWORD vertexIndex = 0;
		vertexIndex < vertexCount_;
		vertexIndex++)
	{
		float totalWeight = 0.0f;

		for (int influence = 0;
			influence < maxInfluence;
			influence++)
		{
			const int boneIndex =
				pWeightArray_[vertexIndex]
				.pBoneIndex[influence];

			if (boneIndex < 0)
			{
				continue;
			}

			totalWeight +=
				pWeightArray_[vertexIndex]
				.pBoneWeight[influence];
		}

		if (totalWeight > 0.000001f)
		{
			for (int influence = 0;
				influence < maxInfluence;
				influence++)
			{
				const int boneIndex =
					pWeightArray_[vertexIndex]
					.pBoneIndex[influence];

				if (boneIndex < 0)
				{
					continue;
				}

				pWeightArray_[vertexIndex]
					.pBoneWeight[influence] /=
					totalWeight;
			}
		}
	}

	// ============================================================
	// ボーンのバインド姿勢を取得
	// ============================================================
	pBoneArray_ = new FbxParts::Bone[numBone_];

	for (int boneIndex = 0;
		boneIndex < numBone_;
		boneIndex++)
	{
		FbxAMatrix bindMatrix;

		ppCluster_[boneIndex]
			->GetTransformLinkMatrix(bindMatrix);

		XMFLOAT4X4 pose{};

		for (DWORD row = 0; row < 4; row++)
		{
			for (DWORD column = 0;
				column < 4;
				column++)
			{
				pose(row, column) =
					static_cast<float>(
						bindMatrix.Get(row, column)
						);
			}
		}

		pBoneArray_[boneIndex].bindPose =
			XMLoadFloat4x4(&pose);

		pBoneArray_[boneIndex].newPose =
			XMMatrixIdentity();

		pBoneArray_[boneIndex].diffPose =
			XMMatrixIdentity();
	}

	// 一時メモリを解放
	for (DWORD vertexIndex = 0;
		vertexIndex < vertexCount_;
		vertexIndex++)
	{
		SAFE_DELETE_ARRAY(
			polyTable[vertexIndex].polyIndex
		);

		SAFE_DELETE_ARRAY(
			polyTable[vertexIndex].vertexIndex
		);
	}

	SAFE_DELETE_ARRAY(polyTable);
}
//コンスタントバッファ（シェーダーに情報を送るやつ）準備
void FbxParts::IntConstantBuffer()
{
	// 定数バッファの作成(パラメータ受け渡し用)
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	Direct3D::pDevice_->CreateBuffer(&cb, NULL, &pConstantBuffer_);
}

//描画
void FbxParts::Draw(Transform& transform)
{
	//今から描画する頂点情報をシェーダに伝える
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Direct3D::pContext_->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	//使用するコンスタントバッファをシェーダに伝える
	Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_);
	Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_);


	//シェーダーのコンスタントバッファーに各種データを渡す
	for (DWORD i = 0; i < materialCount_; i++)
	{
		// インデックスバッファーをセット
		UINT    stride = sizeof(int);
		UINT    offset = 0;
		Direct3D::pContext_->IASetIndexBuffer(ppIndexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);


		// パラメータの受け渡し
		D3D11_MAPPED_SUBRESOURCE pdata;
		CONSTANT_BUFFER cb;
		cb.worldVewProj =	XMMatrixTranspose(transform.GetWorldMatrix() * Camera::GetViewMatrix() * Camera::GetProjectionMatrix());						// リソースへ送る値をセット
		cb.world =		XMMatrixTranspose(transform.GetWorldMatrix());
		cb.normalTrans =	XMMatrixTranspose(transform.matRotate_ * XMMatrixInverse(nullptr, transform.matScale_));
		cb.ambient = pMaterial_[i].ambient;
		cb.diffuse = pMaterial_[i].diffuse;
		cb.speculer = pMaterial_[i].specular;
		cb.shininess = pMaterial_[i].shininess;
		cb.cameraPosition = XMFLOAT4(Camera::GetPosition().x, Camera::GetPosition().y, Camera::GetPosition().z, 0);
		cb.lightDirection = XMFLOAT4(1, -1, 1, 0);
		cb.isTexture = pMaterial_[i].pTexture != nullptr;


		Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのリソースアクセスを一時止める
		memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));		// リソースへ値を送る



		// テクスチャをシェーダーに設定

		if (cb.isTexture)
		{
			ID3D11SamplerState*			pSampler = pMaterial_[i].pTexture->GetSampler();
			Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);

			ID3D11ShaderResourceView*	pSRV = pMaterial_[i].pTexture->GetSRV();
			Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);
		}
		Direct3D::pContext_->Unmap(pConstantBuffer_, 0);									// GPUからのリソースアクセスを再開

		 //ポリゴンメッシュを描画する
		Direct3D::pContext_->DrawIndexed(pMaterial_[i].polygonCount * 3, 0, 0);
	}

}

//ボーン有りのモデルを描画
void FbxParts::DrawSkinAnime(Transform& transform, FbxTime time)
{
	// 現在フレームのボーン行列を作成
	for (int i = 0; i < numBone_; i++)
	{
		FbxAnimEvaluator* evaluator =
			ppCluster_[i]->GetLink()->GetScene()->GetAnimationEvaluator();

		FbxMatrix currentMatrix =
			evaluator->GetNodeGlobalTransform(
				ppCluster_[i]->GetLink(),
				time
			);

		XMFLOAT4X4 pose{};

		for (DWORD row = 0; row < 4; row++)
		{
			for (DWORD column = 0; column < 4; column++)
			{
				pose(row, column) =
					static_cast<float>(
						currentMatrix.Get(row, column)
						);
			}
		}

		pBoneArray_[i].newPose = XMLoadFloat4x4(&pose);

		pBoneArray_[i].diffPose =
			XMMatrixInverse(
				nullptr,
				pBoneArray_[i].bindPose
			);

		pBoneArray_[i].diffPose *=
			pBoneArray_[i].newPose;
	}

	// 各頂点をボーンで変形
	for (DWORD vertexIndex = 0;
		vertexIndex < vertexCount_;
		vertexIndex++)
	{
		XMMATRIX skinMatrix = XMMatrixSet(
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f
		);
		float totalWeight = 0.0f;

		// 最大4ボーンだけ使用する
		for (int influence = 0;
			influence < 4 && influence < numBone_;
			influence++)
		{
			int boneIndex =
				pWeightArray_[vertexIndex]
				.pBoneIndex[influence];

			if (boneIndex < 0)
			{
				break;
			}

			float weight =
				pWeightArray_[vertexIndex]
				.pBoneWeight[influence];

			skinMatrix +=
				pBoneArray_[boneIndex].diffPose * weight;

			totalWeight += weight;
		}

		// ボーンが設定されていない頂点は元の位置を維持
		if (totalWeight > 0.000001f)
		{
			// 合計が1ではない場合にも形が縮まないようにする
			skinMatrix *= 1.0f / totalWeight;
		}
		else
		{
			// ボーンが割り当てられていない頂点は元の位置を使う
			skinMatrix = XMMatrixIdentity();
		}
		XMVECTOR position =
			XMLoadFloat3(
				&pWeightArray_[vertexIndex].posOrigin
			);

		XMVECTOR normal =
			XMLoadFloat3(
				&pWeightArray_[vertexIndex].normalOrigin
			);

		position =
			XMVector3TransformCoord(
				position,
				skinMatrix
			);

		// 法線には平行移動を適用しない
		normal =
			XMVector3TransformNormal(
				normal,
				skinMatrix
			);

		normal = XMVector3Normalize(normal);

		XMStoreFloat3(
			&pVertexData_[vertexIndex].position,
			position
		);

		XMStoreFloat3(
			&pVertexData_[vertexIndex].normal,
			normal
		);
	}

	D3D11_MAPPED_SUBRESOURCE mapped{};

	if (SUCCEEDED(
		Direct3D::pContext_->Map(
			pVertexBuffer_,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mapped
		)))
	{
		memcpy(
			mapped.pData,
			pVertexData_,
			sizeof(VERTEX) * vertexCount_
		);

		Direct3D::pContext_->Unmap(
			pVertexBuffer_,
			0
		);
	}

	Draw(transform);
}
void FbxParts::DrawMeshAnime(Transform& transform, FbxTime time, FbxScene * scene)
{
	//// その瞬間の自分の姿勢行列を得る
	//FbxAnimEvaluator *evaluator = scene->GetAnimationEvaluator();
	//FbxMatrix mCurrentOrentation = evaluator->GetNodeGlobalTransform(_pNode, time);

	//// Fbx形式の行列からDirectX形式の行列へのコピー（4×4の行列）
	//for (DWORD x = 0; x < 4; x++)
	//{
	//	for (DWORD y = 0; y < 4; y++)
	//	{
	//		_localMatrix(x, y) = (float)mCurrentOrentation.Get(x, y);
	//	}
	//}

	Draw(transform);
}

bool FbxParts::GetBonePosition(std::string boneName, XMFLOAT3 * position)
{
	for (int i = 0; i < numBone_; i++)
	{
		if (boneName == ppCluster_[i]->GetLink()->GetName())
		{
			FbxAMatrix  matrix;
			ppCluster_[i]->GetTransformLinkMatrix(matrix);

			position->x = (float)matrix[3][0];
			position->y = (float)matrix[3][1];
			position->z = (float)matrix[3][2];

			return true;
		}

	}

	return false;
}

void FbxParts::RayCast(RayCastData * data)
{
	data->hit = FALSE;

	//マテリアル毎
	for (DWORD i = 0; i < materialCount_; i++)
	{
		//そのマテリアルのポリゴン毎
		for (DWORD j = 0; j < pMaterial_[i].polygonCount; j++)
		{
			//3頂点
			XMFLOAT3 ver[3];
			ver[0] = pVertexData_[ppIndexData_[i][j * 3 + 0]].position;
			ver[1] = pVertexData_[ppIndexData_[i][j * 3 + 1]].position;
			ver[2] = pVertexData_[ppIndexData_[i][j * 3 + 2]].position;

			BOOL  hit = FALSE;
			float dist = 0.0f;

			hit = Direct3D::Intersect(data->start, data->dir, ver[0], ver[1], ver[2], &dist);


			if (hit && dist < data->dist)
			{
				data->hit = TRUE;
				data->dist = dist;
			}
		}
	}
}
