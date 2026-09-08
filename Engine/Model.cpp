#include "Global.h"
#include "Model.h"

//3Dモデル（FBXファイル）を管理する
namespace Model
{
	//ロード済みのモデルデータ一覧
	std::vector<ModelData*>	_datas;

	//初期化
	void Initialize()
	{
		AllRelease();
	}

	//モデルをロード
	int Load(std::string fileName)
	{
			ModelData* pData = new ModelData;


			//開いたファイル一覧から同じファイル名のものが無いか探す
			bool isExist = false;
			for (int i = 0; i < _datas.size(); i++)
			{
				//すでに開いている場合
				if (_datas[i] != nullptr && _datas[i]->fileName == fileName)
				{
					pData->pModel = _datas[i]->pModel;
					isExist = true;
					break;
				}
			}

			//新たにファイルを開く
			if (isExist == false)
			{
				pData->pModel = new BRModel;
				if (!pData->pModel->Load(fileName))
				{
					//開けなかった
					SAFE_DELETE(pData->pModel);
					SAFE_DELETE(pData);
					return -1;
				}

				//無事開けた
				pData->fileName = fileName;
			}


			//使ってない番号が無いか探す
			for (int i = 0; i < _datas.size(); i++)
			{
				if (_datas[i] == nullptr)
				{
					_datas[i] = pData;
					return i;
				}
			}

			//新たに追加
			_datas.push_back(pData);
			return (int)_datas.size() - 1;
	}



	//描画
	void Draw(int handle)
	{
		if (handle < 0 || handle >= _datas.size() || _datas[handle] == nullptr)
		{
			return;
		}

		//アニメーションを進める
		_datas[handle]->nowFrame += _datas[handle]->animSpeed;

		//最後までアニメーションしたら戻す
		if (_datas[handle]->nowFrame > (float)_datas[handle]->endFrame)
			_datas[handle]->nowFrame = (float)_datas[handle]->startFrame;



		if (_datas[handle]->pModel)
		{
			_datas[handle]->pModel->Draw(_datas[handle]->transform);
		}
	}


	//任意のモデルを開放
	void Release(int handle)
	{
		if (handle < 0 || handle >= _datas.size() || _datas[handle] == nullptr)
		{
			return;
		}

		//同じモデルを他でも使っていないか
		bool isExist = false;
		for (int i = 0; i < _datas.size(); i++)
		{
			//すでに開いている場合
			if (_datas[i] != nullptr && i != handle && _datas[i]->pModel == _datas[handle]->pModel)
			{
				isExist = true;
				break;
			}
		}

		//使ってなければモデル解放
		if (isExist == false )
		{
			SAFE_DELETE(_datas[handle]->pModel);
		}


		SAFE_DELETE(_datas[handle]);
	}


	//全てのモデルを解放
	void AllRelease()
	{
		for (int i = 0; i < _datas.size(); i++)
		{
			if (_datas[i] != nullptr)
			{
				Release(i);
			}
		}
		_datas.clear();
	}


	//アニメーションのフレーム数をセット
	void SetAnimFrame(int handle, int startFrame, int endFrame, float animSpeed)
	{
		_datas[handle]->SetAnimFrame(startFrame, endFrame, animSpeed);
	}


	//現在のアニメーションのフレームを取得
	int GetAnimFrame(int handle)
	{
		return (int)_datas[handle]->nowFrame;
	}


	//任意のボーンの位置を取得
	XMFLOAT3 GetBonePosition(int handle, std::string boneName)
	{
		return XMFLOAT3(
			0.0f, 0.0f, 0.0f
		);
	}


	//ワールド行列を設定
	void SetTransform(int handle, Transform & transform)
	{
		if (handle < 0 || handle >= _datas.size())
		{
			return;
		}

		_datas[handle]->transform = transform;
	}


	//ワールド行列の取得
	XMMATRIX GetMatrix(int handle)
	{
		return _datas[handle]->transform.GetWorldMatrix();
	}


	//レイキャスト（レイを飛ばして当たり判定）
	void RayCast(
		int handle,
		RayCastData* data)
	{
		if (!data)
		{
			return;
		}

		data->hit = FALSE;
	}}