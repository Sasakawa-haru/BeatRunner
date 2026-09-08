#include "Game/Scene/ClearScene.h"
#include "Game/Result/ResultData.h"

namespace
{
    // リザルト画面のレイアウト
    constexpr int RESULT_TITLE_X = 100;
    constexpr int RESULT_TITLE_Y = 80;

    constexpr int RESULT_LABEL_X = 100;
    constexpr int RESULT_VALUE_X = 300;

    constexpr int RESULT_START_Y = 140;
    constexpr int RESULT_LINE_HEIGHT = 50;

    enum class ResultRow
    {
        Score = 0,
        MaxCombo,
        Perfect,
        Great,
        Good,
        Normal,
        Miss,
        Hp
    };

    constexpr int GetResultY(ResultRow row)
    {
        return RESULT_START_Y
            + static_cast<int>(row) * RESULT_LINE_HEIGHT;
    }
}
ClearScene::ClearScene(GameObject* parent)
    : GameObject(parent, "ClearScene")
{
}

void ClearScene::Initialize()
{
    text_.Initialize();
}

void ClearScene::Update()
{
}

void ClearScene::Release()
{
}

void ClearScene::Draw()
{
    text_.Draw(
        RESULT_TITLE_X,
        RESULT_TITLE_Y,
        "RESULT"
    );

    text_.Draw(
        RESULT_LABEL_X,
        GetResultY(ResultRow::Score),
        "SCORE"
    );

    text_.Draw(
        RESULT_VALUE_X,
        GetResultY(ResultRow::Score),
        gResultData.score
    );

    text_.Draw(
        RESULT_LABEL_X,
        GetResultY(ResultRow::MaxCombo),
        "MAX COMBO"
    );

    text_.Draw(
        RESULT_VALUE_X,
        GetResultY(ResultRow::MaxCombo),
        gResultData.maxCombo
    );

    text_.Draw(
        RESULT_LABEL_X,
        GetResultY(ResultRow::Perfect),
        "PERFECT"
    );

    text_.Draw(
        RESULT_VALUE_X,
        GetResultY(ResultRow::Perfect),
        gResultData.perfect
    );

    text_.Draw(
        RESULT_LABEL_X,
        GetResultY(ResultRow::Great),
        "GREAT"
    );

    text_.Draw(
        RESULT_VALUE_X,
        GetResultY(ResultRow::Great),
        gResultData.great
    );

    text_.Draw(
        RESULT_LABEL_X,
        GetResultY(ResultRow::Good),
        "GOOD"
    );

    text_.Draw(
        RESULT_VALUE_X,
        GetResultY(ResultRow::Good),
        gResultData.good
    );

    text_.Draw(
        RESULT_LABEL_X,
        GetResultY(ResultRow::Normal),
        "NORMAL"
    );

    text_.Draw(
        RESULT_VALUE_X,
        GetResultY(ResultRow::Normal),
        gResultData.normal
    );

    text_.Draw(
        RESULT_LABEL_X,
        GetResultY(ResultRow::Miss),
        "MISS"
    );

    text_.Draw(
        RESULT_VALUE_X,
        GetResultY(ResultRow::Miss),
        gResultData.miss
    );

    text_.Draw(
        RESULT_LABEL_X,
        GetResultY(ResultRow::Hp),
        "HP"
    );

    text_.Draw(
        RESULT_VALUE_X,
        GetResultY(ResultRow::Hp),
        gResultData.hp
    );
}