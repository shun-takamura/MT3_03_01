#include "Pendulum.h"

void Pendulum::Initialize()
{
}

void Pendulum::Update(const char* keys, const char* preKeys, float deltaTime)
{
	if (isCut_) return; // 切断後は処理しない

    // SPACEで加速
    if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
        angularVelocity_ += kickStrength_;
    }

    if (angularVelocity_ > angularVelocityMax_) angularVelocity_ = angularVelocityMax_;

    // 角加速度（減衰込み）
    angularAcceleration_ = (-9.8f / length_) * sinf(angle_) - damping_ * angularVelocity_;
    angularVelocity_ += angularAcceleration_ * deltaTime;
    angle_ += angularVelocity_ * deltaTime;
}

void Pendulum::Reset()
{
	// 初期角度・速度をリセット
	angle_ = 0.0f;
	angularVelocity_ = 0.0f;
	angularAcceleration_ = 0.0f;
	isCut_ = false;
}
