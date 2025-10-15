#include <Novice.h>

#include <algorithm>
#include <Matrix4x4.h>
#include <Vector3.h>
#include <cmath>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdint.h>
#include <algorithm>

//ImGUI
#include <ImGui.h>
#include <ImGuiManager.h>
#include "DirectXCommon.h"
#include "WinApp.h"


const char kWindowTitle[] = "LC1C_14_タカムラシュン_タイトル";

static const int kRowHeight = 20;
static const int kColumnWidth = 150;

typedef struct Quaternion {
	float x;
	float y;
	float z;
	float w;
}Quaternion;

// Quaternionの積
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

// Quaternionのスカラー倍
Quaternion Scaler(const float& scale, const Quaternion& quaternion);

// Quaternionの加算
Quaternion Add(const Quaternion& q0, const Quaternion& q1);

// 単位Quaternionを返す
Quaternion IdentityQuaternion();

// 共役Quaternionを返す(虚部の符号を反転)
Quaternion Conjugate(const Quaternion& quaternion);

// Quaternionのノルムを返す
float Norm(const Quaternion& quaternion);

// 正規化したQuaternionを返す
Quaternion Normalize(const Quaternion& quaternion);

// 逆Quaternionを返す(これをかけると単位クオータニオンができる)
Quaternion Inverse(const Quaternion& quaternion);

// 球面線形補間
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

// Quaternionの内容を画面に表示する関数
void QuaternionScreenPrintf(int x, int y, const Quaternion& q, const char* label);

void VectorScreenPrintf(int numX, int numY, Vector3 vector);

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix);

Vector3 Add(const Vector3& v1, const Vector3& v2);

Vector3 Subtract(const Vector3& v1, const Vector3& v2);

Vector3 Multiply(const float& i, const Vector3& v1);

Matrix4x4 Add(Matrix4x4 matrix1, Matrix4x4 matrix2);

Matrix4x4 Subtract(Matrix4x4 matrix1, Matrix4x4 matrix2);

float Length(const Vector3& v);

Vector3 Normalize(const Vector3& v);

// 内積
float Dot(const Vector3& v1, const Vector3& v2);

float Dot(const Quaternion& q0, const Quaternion& q1);

// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2);

Matrix4x4 MakeRotateXMatrix(Vector3 rotate);

Matrix4x4 MakeRotateYMatrix(Vector3 rotate);

Matrix4x4 MakeRotateZMatrix(Vector3 rotate);

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);

Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

// オーバーロード
Vector3 operator+(const Vector3& v1, const Vector3& v2) { return Add(v1, v2); }
Vector3 operator-(const Vector3& v1, const Vector3& v2) { return Subtract(v1, v2); }
Vector3 operator*(const float& i, const Vector3& v1) { return Multiply(i, v1); }
Vector3 operator*(const Vector3& v1, const float& i) { return i * v1; }
Vector3 operator/(const Vector3& v1, const float& i) { return Multiply(1.0f / i, v1); }
Matrix4x4 operator+(Matrix4x4 matrix1, Matrix4x4 matrix2) { return Add(matrix1, matrix2); }
Matrix4x4 operator-(Matrix4x4 matrix1, Matrix4x4 matrix2) { return Subtract(matrix1, matrix2); }

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Quaternion rotation0 = MakeRotateAxisAngleQuaternion({ 0.71f,0.71f,0.0f }, 0.3f);
	Quaternion rotation1 = MakeRotateAxisAngleQuaternion({ 0.71f,0.0f,0.71f }, 3.141592f);

	Quaternion interpolate0 = Slerp(rotation0, rotation1, 0.0f);
	Quaternion interpolate1 = Slerp(rotation0, rotation1, 0.3f);
	Quaternion interpolate2 = Slerp(rotation0, rotation1, 0.5f);
	Quaternion interpolate3 = Slerp(rotation0, rotation1, 0.7f);
	Quaternion interpolate4 = Slerp(rotation0, rotation1, 1.0f);

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///



		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		QuaternionScreenPrintf(0, kRowHeight * 0, interpolate0, ":interpolate0");
		QuaternionScreenPrintf(0, kRowHeight * 1, interpolate1, ":interpolate1");
		QuaternionScreenPrintf(0, kRowHeight * 2, interpolate2, ":interpolate2");
		QuaternionScreenPrintf(0, kRowHeight * 3, interpolate3, ":interpolate3");
		QuaternionScreenPrintf(0, kRowHeight * 4, interpolate4, ":interpolate4");
		

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs)
{
	Quaternion result;

	// 虚部を取り出す
	Vector3 imaginaryLhs;
	Vector3 imaginaryRhs;
	Vector3 imaginaryResult;

	imaginaryLhs = { lhs.x,lhs.y,lhs.z };
	imaginaryRhs = { rhs.x,rhs.y,rhs.z };

	// 実部の計算
	result.w = lhs.w * rhs.w - Dot(imaginaryLhs, imaginaryRhs);

	// 虚部の計算(クオータニオンq,rの虚部のクロス積に(qの虚部にrの実部をスカラー倍したもの)と(rの虚部にqの実部をスカラー倍したもの)を足せばいい)
	imaginaryResult = { Cross(imaginaryLhs,imaginaryRhs) + (lhs.w * imaginaryRhs) + (rhs.w * imaginaryLhs) };

	result.x = imaginaryResult.x;
	result.y = imaginaryResult.y;
	result.z = imaginaryResult.z;

	return result;
}

Quaternion Scaler(const float& scale, const Quaternion& quaternion)
{
	Quaternion result;

	result = { quaternion.x * scale,quaternion.y * scale, quaternion.z * scale, quaternion.w * scale };

	return result;
}

Quaternion Add(const Quaternion& q0, const Quaternion& q1)
{
	Quaternion result;

	result = { q0.x + q1.x,q0.y + q1.y, q0.z + q1.z, q0.w + q1.w };

	return result;
}

Quaternion IdentityQuaternion()
{
	Quaternion result;

	result.x = 0.0f;
	result.y = 0.0f;
	result.z = 0.0f;
	result.w = 1.0f;

	return result;
}

Quaternion Conjugate(const Quaternion& quaternion)
{
	Quaternion result;

	result = { quaternion.x * -1.0f,quaternion.y * -1.0f,quaternion.z * -1.0f,quaternion.w };

	return result;
}

float Norm(const Quaternion& quaternion)
{
	float result;

	result = sqrtf(powf(quaternion.x, 2.0f) + powf(quaternion.y, 2.0f) + powf(quaternion.z, 2.0f) + powf(quaternion.w, 2.0f));

	return result;
}

Quaternion Normalize(const Quaternion& quaternion)
{
	Quaternion result;

	result = { (quaternion.x / Norm(quaternion)),(quaternion.y / Norm(quaternion)), (quaternion.z / Norm(quaternion)), (quaternion.w / Norm(quaternion)) };

	return result;
}

Quaternion Inverse(const Quaternion& quaternion)
{
	Quaternion result;

	// 共役を求める
	Quaternion conj = Conjugate(quaternion);

	// ノルムを求める
	float norm = Norm(quaternion);

	// 共役をノルムの2乗で割る
	result = {
		conj.x / powf(norm,2.0f),
		conj.y / powf(norm,2.0f),
		conj.z / powf(norm,2.0f),
		conj.w / powf(norm,2.0f)
	};

	return result;
}

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t)
{
	Quaternion result;
	Quaternion copyQ0;

	copyQ0 = q0;

	float dot = Dot(q0, q1);

	if (dot < 0) {

		copyQ0 = { -q0.x,-q0.y, -q0.z, -q0.w };

		dot = -dot;
	}

	// なす角を求める
	float theta = std::acos(dot);

	// thetaとsinを使って補間係数scale0,scale1を求める
	float sinTheta = sinf(theta);

	float scale0 = sinf((1.0f - t) * theta) / sinTheta;
	float scale1 = sinf(t * theta) / sinf(theta);

	// それぞれの補間係数を利用して補間後のQuaternionを求める
	result = Add(Scaler(scale0, copyQ0), Scaler(scale1, q1));

	return result;
}

void QuaternionScreenPrintf(int x, int y, const Quaternion& q, const char* label) {
	Novice::ScreenPrintf(x, y, "%s", label);
	Novice::ScreenPrintf(x + kColumnWidth, y, "x: %2.2f", q.x);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "y: %2.2f", q.y);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "z: %2.2f", q.z);
	Novice::ScreenPrintf(x + kColumnWidth * 4, y, "w: %2.2f", q.w);
}

void VectorScreenPrintf(int numX, int numY, Vector3 vector)
{
	Novice::ScreenPrintf(numX, numY, "%.02f", vector.x);
	Novice::ScreenPrintf(numX + kColumnWidth, numY, "%.02f", vector.y);
	Novice::ScreenPrintf(numX + kColumnWidth * 2, numY, "%.02f", vector.z);
}

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix)
{
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(
				x + column * kColumnWidth, y + row * kRowHeight, "%6.3f", matrix.m[row][column]);
		}
	}
}

Vector3 Add(const Vector3& v1, const Vector3& v2)
{
	Vector3 result;

	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;

	return result;
}

Vector3 Subtract(const Vector3& v1, const Vector3& v2)
{
	Vector3 result;

	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;

	return result;
}

Vector3 Multiply(const float& i, const Vector3& v1)
{
	Vector3 result;

	result.x = v1.x * i;
	result.y = v1.y * i;
	result.z = v1.z * i;

	return result;
}

Matrix4x4 Add(Matrix4x4 matrix1, Matrix4x4 matrix2)
{
	Matrix4x4 result;

	result.m[0][0] = matrix1.m[0][0] + matrix2.m[0][0];
	result.m[0][1] = matrix1.m[0][1] + matrix2.m[0][1];
	result.m[0][2] = matrix1.m[0][2] + matrix2.m[0][2];
	result.m[0][3] = matrix1.m[0][3] + matrix2.m[0][3];

	result.m[1][0] = matrix1.m[1][0] + matrix2.m[1][0];
	result.m[1][1] = matrix1.m[1][1] + matrix2.m[1][1];
	result.m[1][2] = matrix1.m[1][2] + matrix2.m[1][2];
	result.m[1][3] = matrix1.m[1][3] + matrix2.m[1][3];

	result.m[2][0] = matrix1.m[2][0] + matrix2.m[2][0];
	result.m[2][1] = matrix1.m[2][1] + matrix2.m[2][1];
	result.m[2][2] = matrix1.m[2][2] + matrix2.m[2][2];
	result.m[2][3] = matrix1.m[2][3] + matrix2.m[2][3];

	result.m[3][0] = matrix1.m[3][0] + matrix2.m[3][0];
	result.m[3][1] = matrix1.m[3][1] + matrix2.m[3][1];
	result.m[3][2] = matrix1.m[3][2] + matrix2.m[3][2];
	result.m[3][3] = matrix1.m[3][3] + matrix2.m[3][3];

	return result;
}

Matrix4x4 Subtract(Matrix4x4 matrix1, Matrix4x4 matrix2)
{
	Matrix4x4 result;

	result.m[0][0] = matrix1.m[0][0] - matrix2.m[0][0];
	result.m[0][1] = matrix1.m[0][1] - matrix2.m[0][1];
	result.m[0][2] = matrix1.m[0][2] - matrix2.m[0][2];
	result.m[0][3] = matrix1.m[0][3] - matrix2.m[0][3];

	result.m[1][0] = matrix1.m[1][0] - matrix2.m[1][0];
	result.m[1][1] = matrix1.m[1][1] - matrix2.m[1][1];
	result.m[1][2] = matrix1.m[1][2] - matrix2.m[1][2];
	result.m[1][3] = matrix1.m[1][3] - matrix2.m[1][3];

	result.m[2][0] = matrix1.m[2][0] - matrix2.m[2][0];
	result.m[2][1] = matrix1.m[2][1] - matrix2.m[2][1];
	result.m[2][2] = matrix1.m[2][2] - matrix2.m[2][2];
	result.m[2][3] = matrix1.m[2][3] - matrix2.m[2][3];

	result.m[3][0] = matrix1.m[3][0] - matrix2.m[3][0];
	result.m[3][1] = matrix1.m[3][1] - matrix2.m[3][1];
	result.m[3][2] = matrix1.m[3][2] - matrix2.m[3][2];
	result.m[3][3] = matrix1.m[3][3] - matrix2.m[3][3];

	return result;
}

float Length(const Vector3& v)
{
	float length;

	length = sqrtf(powf(v.x, 2.0f) + powf(v.y, 2.0f) + powf(v.z, 2.0f));

	return length;
}

Vector3 Normalize(const Vector3& v)
{
	Vector3 normalizedV;

	normalizedV = { v.x / Length(v),v.y / Length(v),v.z / Length(v) };

	return normalizedV;
}

float Dot(const Vector3& v1, const Vector3& v2)
{
	float resoult;

	resoult = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

	return resoult;
}

float Dot(const Quaternion& q0, const Quaternion& q1)
{
	float result;

	result = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;

	return result;
}

Vector3 Cross(const Vector3& v1, const Vector3& v2)
{
	Vector3 result;

	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	return result;
}

Matrix4x4 MakeRotateXMatrix(Vector3 rotate)
{
	// Xの回転行列
	Matrix4x4 rotateMatrixX;
	rotateMatrixX.m[0][0] = 1.0f;
	rotateMatrixX.m[0][1] = 0.0f;
	rotateMatrixX.m[0][2] = 0.0f;
	rotateMatrixX.m[0][3] = 0.0f;

	rotateMatrixX.m[1][0] = 0.0f;
	rotateMatrixX.m[1][1] = cosf(rotate.x);
	rotateMatrixX.m[1][2] = sinf(rotate.x);
	rotateMatrixX.m[1][3] = 0.0f;

	rotateMatrixX.m[2][0] = 0.0f;
	rotateMatrixX.m[2][1] = -sinf(rotate.x);
	rotateMatrixX.m[2][2] = cosf(rotate.x);
	rotateMatrixX.m[2][3] = 0.0f;

	rotateMatrixX.m[3][0] = 0.0f;
	rotateMatrixX.m[3][1] = 0.0f;
	rotateMatrixX.m[3][2] = 0.0f;
	rotateMatrixX.m[3][3] = 1.0f;

	return rotateMatrixX;
}

Matrix4x4 MakeRotateYMatrix(Vector3 rotate)
{
	// Yの回転行列
	Matrix4x4 rotateMatrixY;
	rotateMatrixY.m[0][0] = cosf(rotate.y);
	rotateMatrixY.m[0][1] = 0.0f;
	rotateMatrixY.m[0][2] = -sinf(rotate.y);
	rotateMatrixY.m[0][3] = 0.0f;

	rotateMatrixY.m[1][0] = 0.0f;
	rotateMatrixY.m[1][1] = 1.0f;
	rotateMatrixY.m[1][2] = 0.0f;
	rotateMatrixY.m[1][3] = 0.0f;

	rotateMatrixY.m[2][0] = sinf(rotate.y);
	rotateMatrixY.m[2][1] = 0.0f;
	rotateMatrixY.m[2][2] = cosf(rotate.y);
	rotateMatrixY.m[2][3] = 0.0f;

	rotateMatrixY.m[3][0] = 0.0f;
	rotateMatrixY.m[3][1] = 0.0f;
	rotateMatrixY.m[3][2] = 0.0f;
	rotateMatrixY.m[3][3] = 1.0f;

	return rotateMatrixY;
}

Matrix4x4 MakeRotateZMatrix(Vector3 rotate)
{
	// Zの回転行列
	Matrix4x4 rotateMatrixZ;
	rotateMatrixZ.m[0][0] = cosf(rotate.z);
	rotateMatrixZ.m[0][1] = sinf(rotate.z);
	rotateMatrixZ.m[0][2] = 0.0f;
	rotateMatrixZ.m[0][3] = 0.0f;

	rotateMatrixZ.m[1][0] = -sinf(rotate.z);
	rotateMatrixZ.m[1][1] = cosf(rotate.z);
	rotateMatrixZ.m[1][2] = 0.0f;
	rotateMatrixZ.m[1][3] = 0.0f;

	rotateMatrixZ.m[2][0] = 0.0f;
	rotateMatrixZ.m[2][1] = 0.0f;
	rotateMatrixZ.m[2][2] = 1.0f;
	rotateMatrixZ.m[2][3] = 0.0f;

	rotateMatrixZ.m[3][0] = 0.0f;
	rotateMatrixZ.m[3][1] = 0.0f;
	rotateMatrixZ.m[3][2] = 0.0f;
	rotateMatrixZ.m[3][3] = 1.0f;

	return rotateMatrixZ;
}

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle)
{
	Quaternion result;

	// ベクトルを正規化して単位ベクトルを取り出す
	Vector3 normalizedAxis = Normalize(axis);

	// θ/2を求める
	float halfAngle = angle / 2.0f;
	float halfSinTheta = sinf(halfAngle);
	float halfCosTheta = cosf(halfAngle);

	// Quaternionを生成
	result.x = normalizedAxis.x * halfSinTheta;
	result.y = normalizedAxis.y * halfSinTheta;
	result.z = normalizedAxis.z * halfSinTheta;
	result.w = halfCosTheta;

	return result;
}

Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion)
{
	Vector3 result;

	// ベクトルをQuaternionに拡張
	Quaternion quaternionV = { vector.x,vector.y,vector.z,0.0f };

	// 単位quaternionの逆は共役なのでquaternionの共役を求める
	Quaternion conj = Conjugate(quaternion);

	// 別にQuaternionの逆求める関数あるんだから逆でよくね？
	//Quaternion inverseQ = Inverse(quaternion);

	// QxVxQ^*の公式で回転させる
	Quaternion rotatedVector = Multiply(Multiply(quaternion, quaternionV), conj);

	result = { rotatedVector.x,rotatedVector.y,rotatedVector.z };

	return result;
}

Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion)
{
	Matrix4x4 result;

	result.m[0][0] = powf(quaternion.w, 2.0f) + powf(quaternion.x, 2.0f) - powf(quaternion.y, 2.0f) - powf(quaternion.z, 2.0f);
	result.m[0][1] = 2.0f * (quaternion.x * quaternion.y + quaternion.w * quaternion.z);
	result.m[0][2] = 2.0f * (quaternion.x * quaternion.z - quaternion.w * quaternion.y);
	result.m[0][3] = 0.0f;

	result.m[1][0] = 2.0f * (quaternion.x * quaternion.y - quaternion.w * quaternion.z);
	result.m[1][1] = powf(quaternion.w, 2.0f) - powf(quaternion.x, 2.0f) + powf(quaternion.y, 2.0f) - powf(quaternion.z, 2.0f);
	result.m[1][2] = 2.0f * (quaternion.y * quaternion.z + quaternion.w * quaternion.x);
	result.m[1][3] = 0.0f;

	result.m[2][0] = 2.0f * (quaternion.x * quaternion.z + quaternion.w * quaternion.y);
	result.m[2][1] = 2.0f * (quaternion.y * quaternion.z - quaternion.w * quaternion.x);
	result.m[2][2] = powf(quaternion.w, 2.0f) - powf(quaternion.x, 2.0f) - powf(quaternion.y, 2.0f) + powf(quaternion.z, 2.0f);
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix)
{
	Vector3 resultVector3;

	resultVector3.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	resultVector3.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	resultVector3.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];

	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];

	assert(w != 0.0f);

	resultVector3.x /= w;
	resultVector3.y /= w;
	resultVector3.z /= w;

	return resultVector3;
}
