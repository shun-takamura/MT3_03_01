#pragma once
#include <Vector3.h>
#include <cmath>
#include <Novice.h>// キー入力に使う

class Pendulum{

	bool isCut_ = false;
	Vector3 anchorPos_ = { 0.0f,0.0f,0.0f };
	float length_ = 0.8f;
	float angle_ = 0.0f;
	float radius_ = 0.05f;
	float angularVelocity_ = 0.0f;
	float angularAcceleration_ = 0.0f;
	float angularVelocityMax_ = 30.0f;
	unsigned int color_ = 0xFFFFFFFF;
	int strokeCount_ = 0;
	float kickStrength_ = 1.5f;
	//Vector3 anchorAtCut;// 切断時のアンカー位置（基準）

	// 打球位置の履歴管理
	Vector3 lastAnchorPos_ = anchorPos_;
	Vector3 prevAnchorPos_ = anchorPos_;

	// 減衰係数（小さいほど長く揺れる、大きいほどすぐ止まる）
	float damping_ = 0.25f;

public:

	void Initialize();

	void Update(const char* keys, const char* preKeys, float deltaTime);

	void Reset();

	//ゲッターロボ
	const Vector3& GetAnchor() const { return anchorPos_; }
	const Vector3& GetLastAnchor() const { return lastAnchorPos_; }
	const Vector3& GetPrevAnchor() const { return prevAnchorPos_; }
	const int& GetStrokeCount() const { return strokeCount_; }
	float GetLength() const { return length_; }
	float GetAngle() const { return angle_; }
	int GetIsCut() const { return isCut_; }

	// セッター
	void SetStrokeCount(int strokeCount) { strokeCount_ = strokeCount; }
	void SetAnchorPos(Vector3 anchorPos) { anchorPos_ = anchorPos; }
	void SetLastAnchorPos(Vector3 lastAnchorPos) { lastAnchorPos_ = lastAnchorPos; }
	void SetPrevAnchorPos(Vector3 prevAnchorPos) { prevAnchorPos_ = prevAnchorPos; }
	void SetIsCut(bool isCut) { isCut_ = isCut; }
};

