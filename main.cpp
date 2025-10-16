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

#include "Player.h"
#include "Pendulum.h"

// 行列をベクトルに変換する関数
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

Vector3 Leap(const Vector3& v1, const Vector3& v2, float t);

void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2,
	const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, int division, uint32_t color);

typedef struct Segment {
	Vector3 origin;// 始点
	Vector3 diff;// 終点
}Segment;

/// <summary>
/// cotangent(余接)を求める関数
/// </summary>
/// <param name="theta">θ(シータ)</param>
/// <returns>cotangent</returns>
float Cotangent(float theta);

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

float LengthSq(const Vector3& v) {
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

// 線分(壁のエッジ)とカプセルの交差判定
bool CapsuleIntersectsSegment3D(
	const Vector3& capsuleStart, const Vector3& capsuleEnd, float radius,
	const Vector3& segStart, const Vector3& segEnd);

/// <summary>
/// アフィン行列作成関数
/// </summary>
/// <param name="scale">縮尺</param>
/// <param name="rotate">thetaを求めるための数値</param>
/// <param name="translate">三次元座標でのx,y,zの移動量</param>
/// <returns>アフィン行列</returns>
Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotate, Vector3 translate);

/// <summary>
/// 4x4逆行列を求める関数
/// </summary>
/// <param name="matrix4x4">逆行列を求めたい行列</param>
/// <returns>4x4逆行列</returns>
Matrix4x4 Inverse(Matrix4x4 matrix4x4);

/// <summary>
/// 4x4行列の積を求める関数
/// </summary>
/// <param name="matrix1">1つ目の行列</param>
/// <param name="matrix2">1つ目の行列</param>
/// <returns>4x4行列の積</returns>
Matrix4x4 Multiply(Matrix4x4 matrix1, Matrix4x4 matrix2);

/// <summary>
/// 投視投影行列作成関数
/// </summary>
/// <param name="fovY">縦の画角</param>
/// <param name="aspectRatio">アスペクト比</param>
/// <param name="nearClip">近平面への距離</param>
/// <param name="farClip">遠平面への距離</param>
/// <returns>投視投影行列</returns>
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

/// <summary>
/// ビューポート行列作成関数
/// </summary>
/// <param name="left">左側の座標</param>
/// <param name="top">上側の座標</param>
/// <param name="width">切り取るスクリーンの幅</param>
/// <param name="height">切り取るスクリーンの高さ</param>
/// <param name="minDepth">最小深度値</param>
/// <param name="maxDepth">最大深度値</param>
/// <returns></returns>
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

/// <summary>
/// マウスでカメラ操作
/// </summary>
/// <param name="cameraTranslate"></param>
/// <param name="cameraRotate"></param>
void UpdateCameraByMouse(Vector3& cameraTranslate, Vector3& cameraRotate);

void DrawSphere(const Vector3& center, float radius, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

float Length(const Vector3& v);

// 内積
float Dot(const Vector3& v1, const Vector3& v2);

Vector3 Normalize(const Vector3& v);

Vector3 Add(const Vector3& v1, const Vector3& v2);

Vector3 Subtract(const Vector3& v1, const Vector3& v2);

Vector3 Multiply(const float& i, const Vector3& v1);

Matrix4x4 Add(Matrix4x4 matrix1, Matrix4x4 matrix2);

Matrix4x4 Subtract(Matrix4x4 matrix1, Matrix4x4 matrix2);

// X回転行列作成関数
Matrix4x4 MakeRotateXMatrix(Vector3 rotate);

// Y回転行列作成関数
Matrix4x4 MakeRotateYMatrix(Vector3 rotate);

// Z回転行列作成関数
Matrix4x4 MakeRotateZMatrix(Vector3 rotate);

// オーバーロード
Vector3 operator+(const Vector3& v1, const Vector3& v2) { return Add(v1, v2); }
Vector3 operator-(const Vector3& v1, const Vector3& v2) { return Subtract(v1, v2); }
Vector3 operator*(const float& i, const Vector3& v1) { return Multiply(i, v1); }
Vector3 operator*(const Vector3& v1, const float& i) { return i * v1; }
Vector3 operator/(const Vector3& v1, const float& i) { return Multiply(1.0f / i, v1); }
Matrix4x4 operator+(Matrix4x4 matrix1, Matrix4x4 matrix2) { return Add(matrix1, matrix2); }
Matrix4x4 operator-(Matrix4x4 matrix1, Matrix4x4 matrix2) { return Subtract(matrix1, matrix2); }
Matrix4x4 operator*(Matrix4x4 matrix1, Matrix4x4 matrix2) { return Multiply(matrix1, matrix2); }

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	float deltaTime = 1.0f / 60.0f;

	Vector3 cameraRotate = { 0.0f, 0.0f, 0.0f };
	Vector3 cameraTranslate = { 0.0f,0.0f, -10.0f };

	// カメラ初期化のところに追加
	Vector3 baseCameraTranslate = cameraTranslate;  // 現在のカメラ基準位置
	//Vector3 anchorAtCut;                            // 切断時のアンカー位置（基準）

	// 壁の位置の初期化
	const float wallXMin = -7.5f;
	const float wallXMax = 7.5f;
	const float wallYMin = -1.0f;
	const float wallYMax = 14.0f;

	Player* player = new Player();
	player->Initialize();
	Vector3 targetGoal;
	Vector3 cameraTarget = { 0.0f,0.0f,0.0f };

	float cameraLerpSpeed = 0.05f;         // 0.05〜0.2くらいで調整
	Vector3 cameraOffset = { 0.0f,0.0f,-10.0f };

	Vector3 bumperPos = { 0.0f, 5.0f, 0.0f }; // バンパー位置
	float bumperRadius = 0.5f;

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


		player->Update(keys, preKeys, deltaTime);

		if (!player->GetIsCut()) {
			// カメラの注視点をアンカーに設定
			targetGoal = player->GetAnchorPosition();

		} else {

			targetGoal = player->GetPosition();
			
		}


		// カメラの処理
		cameraTarget = Leap(cameraTarget, targetGoal, cameraLerpSpeed);
		cameraTranslate = cameraTarget + cameraOffset;

		UpdateCameraByMouse(cameraTranslate, cameraRotate);

		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, 1280, 720, 0.0f, 1.0f);

		baseCameraTranslate = cameraTranslate;

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// グリッドの描画
		DrawGrid(viewProjectionMatrix, viewportMatrix);

		// --- 壁の線を描画 ---
		Vector3 wallPoints[4] = {
			{ wallXMin, wallYMin, 0.0f },
			{ wallXMax, wallYMin, 0.0f },
			{ wallXMax, wallYMax, 0.0f },
			{ wallXMin, wallYMax, 0.0f }
		};

		for (int i = 0; i < 4; i++) {
			Vector3 p1 = Transform(Transform(wallPoints[i], viewProjectionMatrix), viewportMatrix);
			Vector3 p2 = Transform(Transform(wallPoints[(i + 1) % 4], viewProjectionMatrix), viewportMatrix);
			Novice::DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, 0x00FF00FF);
		}

		DrawSphere(player->GetPosition(), player->GetRadius(), viewProjectionMatrix, viewportMatrix, player->GetColor());

		DrawSphere(player->GetAnchorPosition(), player->GetRadius(), viewProjectionMatrix, viewportMatrix, 0xFFFFFFFF);

		DrawSphere(bumperPos, bumperRadius , viewProjectionMatrix, viewportMatrix, 0x00FF00FF);

		// ImGui
		ImGui::Begin("Debug Controller");

		// 表示のみ
		ImGui::Text("Current Values:");
		//ImGui::Text("Pendulum Velocity: %.2f", pendulum.angularVelocity);
		//ImGui::Text("Ball Position: X = %.2f, Y = %.2f", ball.position.x, ball.position.y);
		//ImGui::Text("Stroke Count: %d", strokeCount);
		ImGui::Separator();

		// 変数調整
		ImGui::Text("Editable Parameters:");
		//ImGui::InputFloat("AngularVelocityMax", &pendulum.angularVelocityMax, 1.0f, 50.0f);
		//ImGui::InputFloat("Length", &pendulum.length, 0.1f, 2.0f);
		//ImGui::InputFloat("KickStrength", &kickStrength, 1.0f, 5.0f);
		//ImGui::InputFloat("DecelerationRate", &ball.decelerationRate, 0.5f, 1.0f);
		ImGui::Separator();

		// リセットボタン
		/*if (ImGui::Button("Reset Position")) {
			ball.position = { 0.0f, 0.2f, 0.0f };
			pendulum.anchor = { 0.0f, 1.0f, 0.0f };
			ballVelocity = { 0.0f, 0.0f, 0.0f };
			pendulum.angle = 0.0f;
			pendulum.angularVelocity = 0.0f;
			pendulum.angularAcceleration = 0.0f;
			isCut = false;
			targetGoal = pendulum.anchor;
		}*/

		ImGui::End();

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

Vector3 Leap(const Vector3& v1, const Vector3& v2, float t)
{
	Vector3 result;
	result.x = v1.x + (v2.x - v1.x) * t;
	result.y = v1.y + (v2.y - v1.y) * t;
	result.z = v1.z + (v2.z - v1.z) * t;
	return result;
}

void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, int division, uint32_t color)
{
	for (int index = 0; index < division; ++index) {
		float t0 = static_cast<float>(index) / static_cast<float>(division);
		float t1 = static_cast<float>(index + 1.0f) / static_cast<float>(division);

		Vector3 b0 = Leap(Leap(controlPoint0, controlPoint1, t0), Leap(controlPoint1, controlPoint2, t0), t0);
		Vector3 b1 = Leap(Leap(controlPoint0, controlPoint1, t1), Leap(controlPoint1, controlPoint2, t1), t1);

		// 変換
		Segment transformedSegment = {
			Transform(Transform(b0,viewProjectionMatrix),viewportMatrix),
			Transform(Transform(b1,viewProjectionMatrix),viewportMatrix),
		};

		// 描画
		Novice::DrawLine(
			static_cast<int>(transformedSegment.origin.x),
			static_cast<int>(transformedSegment.origin.y),
			static_cast<int>(transformedSegment.diff.x),
			static_cast<int>(transformedSegment.diff.y),
			color
		);
	}
}

float Cotangent(float theta)
{
	float cotngent;

	cotngent = 1.0f / std::tanf(theta);

	return cotngent;
}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / static_cast<float>(kSubdivision);

	for (uint32_t i = 0; i <= kSubdivision; ++i) {
		float offset = -kGridHalfWidth + i * kGridEvery;

		// 色を決定（中央線だけ黒、それ以外は灰色）
		uint32_t color = (offset == 0.0f) ? 0x000000FF : 0xAAAAAAFF;

		// Z方向（X軸に平行）
		Vector3 start = { -kGridHalfWidth, 0.0f, offset };
		Vector3 end = { kGridHalfWidth, 0.0f, offset };
		start = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		end = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);
		Novice::DrawLine(static_cast<int>(start.x), static_cast<int>(start.y), static_cast<int>(end.x), static_cast<int>(end.y), color);

		// X方向（Z軸に平行）
		start = { offset, 0.0f, -kGridHalfWidth };
		end = { offset, 0.0f, kGridHalfWidth };
		start = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		end = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);
		Novice::DrawLine(static_cast<int>(start.x), static_cast<int>(start.y), static_cast<int>(end.x), static_cast<int>(end.y), color);
	}
}
bool CapsuleIntersectsSegment3D(const Vector3& capsuleStart, const Vector3& capsuleEnd, float radius, const Vector3& segStart, const Vector3& segEnd) {
	// カプセル軸方向
	Vector3 u = capsuleEnd - capsuleStart;
	Vector3 v = segEnd - segStart;
	Vector3 w = capsuleStart - segStart;

	float a = Dot(u, u);  // カプセル軸の長さ²
	float b = Dot(u, v);
	float c = Dot(v, v);
	float d = Dot(u, w);
	float e = Dot(v, w);

	float denom = a * c - b * b;
	float sc, sN, sD = denom;
	float tc, tN, tD = denom;

	// まず線分間の最近接点パラメータ s, t を求める（線分-線分距離の標準式）
	const float EPS = 1e-6f;
	if (denom < EPS) {
		sN = 0.0f;  // 平行時
		sD = 1.0f;
		tN = e;
		tD = c;
	} else {
		sN = (b * e - c * d);
		tN = (a * e - b * d);
		if (sN < 0.0f) {
			sN = 0.0f;
			tN = e;
			tD = c;
		} else if (sN > sD) {
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	tc = (fabsf(tN) < EPS ? 0.0f : tN / tD);
	tc = std::clamp(tc, 0.0f, 1.0f);
	sc = (fabsf(sN) < EPS ? 0.0f : sN / sD);
	sc = std::clamp(sc, 0.0f, 1.0f);

	// 線分間の最近接点を求める
	Vector3 p1 = capsuleStart + u * sc;
	Vector3 p2 = segStart + v * tc;

	// 距離が半径以下なら交差
	float distSq = LengthSq(p1 - p2);
	return distSq <= (radius * radius);
}
Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotate, Vector3 translate)
{
	//====================
	// 拡縮の行列の作成
	//====================
	Matrix4x4 scaleMatrix4x4;
	scaleMatrix4x4.m[0][0] = scale.x;
	scaleMatrix4x4.m[0][1] = 0.0f;
	scaleMatrix4x4.m[0][2] = 0.0f;
	scaleMatrix4x4.m[0][3] = 0.0f;

	scaleMatrix4x4.m[1][0] = 0.0f;
	scaleMatrix4x4.m[1][1] = scale.y;
	scaleMatrix4x4.m[1][2] = 0.0f;
	scaleMatrix4x4.m[1][3] = 0.0f;

	scaleMatrix4x4.m[2][0] = 0.0f;
	scaleMatrix4x4.m[2][1] = 0.0f;
	scaleMatrix4x4.m[2][2] = scale.z;
	scaleMatrix4x4.m[2][3] = 0.0f;

	scaleMatrix4x4.m[3][0] = 0.0f;
	scaleMatrix4x4.m[3][1] = 0.0f;
	scaleMatrix4x4.m[3][2] = 0.0f;
	scaleMatrix4x4.m[3][3] = 1.0f;

	//===================
	// 回転の行列の作成
	//===================
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

	// 回転行列の作成
	Matrix4x4 rotateMatrix4x4;

	rotateMatrix4x4 = Multiply(rotateMatrixX, Multiply(rotateMatrixY, rotateMatrixZ));

	//==================
	// 移動の行列の作成
	//==================
	Matrix4x4 translateMatrix4x4;
	translateMatrix4x4.m[0][0] = 1.0f;
	translateMatrix4x4.m[0][1] = 0.0f;
	translateMatrix4x4.m[0][2] = 0.0f;
	translateMatrix4x4.m[0][3] = 0.0f;

	translateMatrix4x4.m[1][0] = 0.0f;
	translateMatrix4x4.m[1][1] = 1.0f;
	translateMatrix4x4.m[1][2] = 0.0f;
	translateMatrix4x4.m[1][3] = 0.0f;

	translateMatrix4x4.m[2][0] = 0.0f;
	translateMatrix4x4.m[2][1] = 0.0f;
	translateMatrix4x4.m[2][2] = 1.0f;
	translateMatrix4x4.m[2][3] = 0.0f;

	translateMatrix4x4.m[3][0] = translate.x;
	translateMatrix4x4.m[3][1] = translate.y;
	translateMatrix4x4.m[3][2] = translate.z;
	translateMatrix4x4.m[3][3] = 1.0f;

	//====================
	// アフィン行列の作成
	//====================
	// 上で作った行列からアフィン行列を作る
	// アフィン行列の作成（スケール→回転→移動の順）
	Matrix4x4 affineMatrix4x4;
	affineMatrix4x4 = Multiply(scaleMatrix4x4, Multiply(rotateMatrix4x4, translateMatrix4x4));

	return  affineMatrix4x4;
}

Matrix4x4 Inverse(Matrix4x4 matrix4x4)
{
	// 行列式|A|を求める
	float bottom =
		(matrix4x4.m[0][0] * matrix4x4.m[1][1] * matrix4x4.m[2][2] * matrix4x4.m[3][3])
		+ (matrix4x4.m[0][0] * matrix4x4.m[1][2] * matrix4x4.m[2][3] * matrix4x4.m[3][1])
		+ (matrix4x4.m[0][0] * matrix4x4.m[1][3] * matrix4x4.m[2][1] * matrix4x4.m[3][2])
		- (matrix4x4.m[0][0] * matrix4x4.m[1][3] * matrix4x4.m[2][2] * matrix4x4.m[3][1])
		- (matrix4x4.m[0][0] * matrix4x4.m[1][2] * matrix4x4.m[2][1] * matrix4x4.m[3][3])
		- (matrix4x4.m[0][0] * matrix4x4.m[1][1] * matrix4x4.m[2][3] * matrix4x4.m[3][2])
		- (matrix4x4.m[0][1] * matrix4x4.m[1][0] * matrix4x4.m[2][2] * matrix4x4.m[3][3])
		- (matrix4x4.m[0][2] * matrix4x4.m[1][0] * matrix4x4.m[2][3] * matrix4x4.m[3][1])
		- (matrix4x4.m[0][3] * matrix4x4.m[1][0] * matrix4x4.m[2][1] * matrix4x4.m[3][2])
		+ (matrix4x4.m[0][3] * matrix4x4.m[1][0] * matrix4x4.m[2][2] * matrix4x4.m[3][1])
		+ (matrix4x4.m[0][2] * matrix4x4.m[1][0] * matrix4x4.m[2][1] * matrix4x4.m[3][3])
		+ (matrix4x4.m[0][1] * matrix4x4.m[1][0] * matrix4x4.m[2][3] * matrix4x4.m[3][2])
		+ (matrix4x4.m[0][1] * matrix4x4.m[1][2] * matrix4x4.m[2][0] * matrix4x4.m[3][3])
		+ (matrix4x4.m[0][2] * matrix4x4.m[1][3] * matrix4x4.m[2][0] * matrix4x4.m[3][1])
		+ (matrix4x4.m[0][3] * matrix4x4.m[1][1] * matrix4x4.m[2][0] * matrix4x4.m[3][2])
		- (matrix4x4.m[0][3] * matrix4x4.m[1][2] * matrix4x4.m[2][0] * matrix4x4.m[3][1])
		- (matrix4x4.m[0][2] * matrix4x4.m[1][1] * matrix4x4.m[2][0] * matrix4x4.m[3][3])
		- (matrix4x4.m[0][1] * matrix4x4.m[1][3] * matrix4x4.m[2][0] * matrix4x4.m[3][2])
		- (matrix4x4.m[0][1] * matrix4x4.m[1][2] * matrix4x4.m[2][3] * matrix4x4.m[3][0])
		- (matrix4x4.m[0][2] * matrix4x4.m[1][3] * matrix4x4.m[2][1] * matrix4x4.m[3][0])
		- (matrix4x4.m[0][3] * matrix4x4.m[1][1] * matrix4x4.m[2][2] * matrix4x4.m[3][0])
		+ (matrix4x4.m[0][3] * matrix4x4.m[1][2] * matrix4x4.m[2][1] * matrix4x4.m[3][0])
		+ (matrix4x4.m[0][2] * matrix4x4.m[1][1] * matrix4x4.m[2][3] * matrix4x4.m[3][0])
		+ (matrix4x4.m[0][1] * matrix4x4.m[1][3] * matrix4x4.m[2][2] * matrix4x4.m[3][0]);

	Matrix4x4 resoultMatrix;

	// 1行目
	resoultMatrix.m[0][0] = 1.0f / bottom * (
		(matrix4x4.m[1][1] * matrix4x4.m[2][2] * matrix4x4.m[3][3])
		+ (matrix4x4.m[1][2] * matrix4x4.m[2][3] * matrix4x4.m[3][1])
		+ (matrix4x4.m[1][3] * matrix4x4.m[2][1] * matrix4x4.m[3][2])
		- (matrix4x4.m[1][3] * matrix4x4.m[2][2] * matrix4x4.m[3][1])
		- (matrix4x4.m[1][2] * matrix4x4.m[2][1] * matrix4x4.m[3][3])
		- (matrix4x4.m[1][1] * matrix4x4.m[2][3] * matrix4x4.m[3][2]));

	resoultMatrix.m[0][1] = 1.0f / bottom * (
		-(matrix4x4.m[0][1] * matrix4x4.m[2][2] * matrix4x4.m[3][3])
		- (matrix4x4.m[0][2] * matrix4x4.m[2][3] * matrix4x4.m[3][1])
		- (matrix4x4.m[0][3] * matrix4x4.m[2][1] * matrix4x4.m[3][2])
		+ (matrix4x4.m[0][3] * matrix4x4.m[2][2] * matrix4x4.m[3][1])
		+ (matrix4x4.m[0][2] * matrix4x4.m[2][1] * matrix4x4.m[3][3])
		+ (matrix4x4.m[0][1] * matrix4x4.m[2][3] * matrix4x4.m[3][2]));

	resoultMatrix.m[0][2] = 1.0f / bottom * (
		(matrix4x4.m[0][1] * matrix4x4.m[1][2] * matrix4x4.m[3][3])
		+ (matrix4x4.m[0][2] * matrix4x4.m[1][3] * matrix4x4.m[3][1])
		+ (matrix4x4.m[0][3] * matrix4x4.m[1][1] * matrix4x4.m[3][2])
		- (matrix4x4.m[0][3] * matrix4x4.m[1][2] * matrix4x4.m[3][1])
		- (matrix4x4.m[0][2] * matrix4x4.m[1][1] * matrix4x4.m[3][3])
		- (matrix4x4.m[0][1] * matrix4x4.m[1][3] * matrix4x4.m[3][2]));

	resoultMatrix.m[0][3] = 1.0f / bottom * (
		-(matrix4x4.m[0][1] * matrix4x4.m[1][2] * matrix4x4.m[2][3])
		- (matrix4x4.m[0][2] * matrix4x4.m[1][3] * matrix4x4.m[2][1])
		- (matrix4x4.m[0][3] * matrix4x4.m[1][1] * matrix4x4.m[2][2])
		+ (matrix4x4.m[0][3] * matrix4x4.m[1][2] * matrix4x4.m[2][1])
		+ (matrix4x4.m[0][2] * matrix4x4.m[1][1] * matrix4x4.m[2][3])
		+ (matrix4x4.m[0][1] * matrix4x4.m[1][3] * matrix4x4.m[2][2]));

	// 2行目
	resoultMatrix.m[1][0] = 1.0f / bottom * (
		-(matrix4x4.m[1][0] * matrix4x4.m[2][2] * matrix4x4.m[3][3])
		- (matrix4x4.m[1][2] * matrix4x4.m[2][3] * matrix4x4.m[3][0])
		- (matrix4x4.m[1][3] * matrix4x4.m[2][0] * matrix4x4.m[3][2])
		+ (matrix4x4.m[1][3] * matrix4x4.m[2][2] * matrix4x4.m[3][0])
		+ (matrix4x4.m[1][2] * matrix4x4.m[2][0] * matrix4x4.m[3][3])
		+ (matrix4x4.m[1][0] * matrix4x4.m[2][3] * matrix4x4.m[3][2]));

	resoultMatrix.m[1][1] = 1.0f / bottom * (
		(matrix4x4.m[0][0] * matrix4x4.m[2][2] * matrix4x4.m[3][3])
		+ (matrix4x4.m[0][2] * matrix4x4.m[2][3] * matrix4x4.m[3][0])
		+ (matrix4x4.m[0][3] * matrix4x4.m[2][0] * matrix4x4.m[3][2])
		- (matrix4x4.m[0][3] * matrix4x4.m[2][2] * matrix4x4.m[3][0])
		- (matrix4x4.m[0][2] * matrix4x4.m[2][0] * matrix4x4.m[3][3])
		- (matrix4x4.m[0][0] * matrix4x4.m[2][3] * matrix4x4.m[3][2]));

	resoultMatrix.m[1][2] = 1.0f / bottom * (
		-(matrix4x4.m[0][0] * matrix4x4.m[1][2] * matrix4x4.m[3][3])
		- (matrix4x4.m[0][2] * matrix4x4.m[1][3] * matrix4x4.m[3][0])
		- (matrix4x4.m[0][3] * matrix4x4.m[1][0] * matrix4x4.m[3][2])
		+ (matrix4x4.m[0][3] * matrix4x4.m[1][2] * matrix4x4.m[3][0])
		+ (matrix4x4.m[0][2] * matrix4x4.m[1][0] * matrix4x4.m[3][3])
		+ (matrix4x4.m[0][0] * matrix4x4.m[1][3] * matrix4x4.m[3][2]));

	resoultMatrix.m[1][3] = 1.0f / bottom * (
		(matrix4x4.m[0][0] * matrix4x4.m[1][2] * matrix4x4.m[2][3])
		+ (matrix4x4.m[0][2] * matrix4x4.m[1][3] * matrix4x4.m[2][0])
		+ (matrix4x4.m[0][3] * matrix4x4.m[1][0] * matrix4x4.m[2][2])
		- (matrix4x4.m[0][3] * matrix4x4.m[1][2] * matrix4x4.m[2][0])
		- (matrix4x4.m[0][2] * matrix4x4.m[1][0] * matrix4x4.m[2][3])
		- (matrix4x4.m[0][0] * matrix4x4.m[1][3] * matrix4x4.m[2][2]));

	// 3行目
	resoultMatrix.m[2][0] = 1.0f / bottom * (
		(matrix4x4.m[1][0] * matrix4x4.m[2][1] * matrix4x4.m[3][3])
		+ (matrix4x4.m[1][1] * matrix4x4.m[2][3] * matrix4x4.m[3][0])
		+ (matrix4x4.m[1][3] * matrix4x4.m[2][0] * matrix4x4.m[3][1])
		- (matrix4x4.m[1][3] * matrix4x4.m[2][1] * matrix4x4.m[3][0])
		- (matrix4x4.m[1][1] * matrix4x4.m[2][0] * matrix4x4.m[3][3])
		- (matrix4x4.m[1][0] * matrix4x4.m[2][3] * matrix4x4.m[3][1]));

	resoultMatrix.m[2][1] = 1.0f / bottom * (
		-(matrix4x4.m[0][0] * matrix4x4.m[2][1] * matrix4x4.m[3][3])
		- (matrix4x4.m[0][1] * matrix4x4.m[2][3] * matrix4x4.m[3][0])
		- (matrix4x4.m[0][3] * matrix4x4.m[2][0] * matrix4x4.m[3][1])
		+ (matrix4x4.m[0][3] * matrix4x4.m[2][1] * matrix4x4.m[3][0])
		+ (matrix4x4.m[0][1] * matrix4x4.m[2][0] * matrix4x4.m[3][3])
		+ (matrix4x4.m[0][0] * matrix4x4.m[2][3] * matrix4x4.m[3][1]));

	resoultMatrix.m[2][2] = 1.0f / bottom * (
		(matrix4x4.m[0][0] * matrix4x4.m[1][1] * matrix4x4.m[3][3])
		+ (matrix4x4.m[0][1] * matrix4x4.m[1][3] * matrix4x4.m[3][0])
		+ (matrix4x4.m[0][3] * matrix4x4.m[1][0] * matrix4x4.m[3][1])
		- (matrix4x4.m[0][3] * matrix4x4.m[1][1] * matrix4x4.m[3][0])
		- (matrix4x4.m[0][1] * matrix4x4.m[1][0] * matrix4x4.m[3][3])
		- (matrix4x4.m[0][0] * matrix4x4.m[1][3] * matrix4x4.m[3][1]));

	resoultMatrix.m[2][3] = 1.0f / bottom * (
		-(matrix4x4.m[0][0] * matrix4x4.m[1][1] * matrix4x4.m[2][3])
		- (matrix4x4.m[0][1] * matrix4x4.m[1][3] * matrix4x4.m[2][0])
		- (matrix4x4.m[0][3] * matrix4x4.m[1][0] * matrix4x4.m[2][1])
		+ (matrix4x4.m[0][3] * matrix4x4.m[1][1] * matrix4x4.m[2][0])
		+ (matrix4x4.m[0][1] * matrix4x4.m[1][0] * matrix4x4.m[2][3])
		+ (matrix4x4.m[0][0] * matrix4x4.m[1][3] * matrix4x4.m[2][1]));

	// 4行目
	resoultMatrix.m[3][0] = 1.0f / bottom * (
		-(matrix4x4.m[1][0] * matrix4x4.m[2][1] * matrix4x4.m[3][2])
		- (matrix4x4.m[1][1] * matrix4x4.m[2][2] * matrix4x4.m[3][0])
		- (matrix4x4.m[1][2] * matrix4x4.m[2][0] * matrix4x4.m[3][1])
		+ (matrix4x4.m[1][2] * matrix4x4.m[2][1] * matrix4x4.m[3][0])
		+ (matrix4x4.m[1][1] * matrix4x4.m[2][0] * matrix4x4.m[3][2])
		+ (matrix4x4.m[1][0] * matrix4x4.m[2][2] * matrix4x4.m[3][1]));

	resoultMatrix.m[3][1] = 1.0f / bottom * (
		(matrix4x4.m[0][0] * matrix4x4.m[2][1] * matrix4x4.m[3][2])
		+ (matrix4x4.m[0][1] * matrix4x4.m[2][2] * matrix4x4.m[3][0])
		+ (matrix4x4.m[0][2] * matrix4x4.m[2][0] * matrix4x4.m[3][1])
		- (matrix4x4.m[0][2] * matrix4x4.m[2][1] * matrix4x4.m[3][0])
		- (matrix4x4.m[0][1] * matrix4x4.m[2][0] * matrix4x4.m[3][2])
		- (matrix4x4.m[0][0] * matrix4x4.m[2][2] * matrix4x4.m[3][1]));

	resoultMatrix.m[3][2] = 1.0f / bottom * (
		-(matrix4x4.m[0][0] * matrix4x4.m[1][1] * matrix4x4.m[3][2])
		- (matrix4x4.m[0][1] * matrix4x4.m[1][2] * matrix4x4.m[3][0])
		- (matrix4x4.m[0][2] * matrix4x4.m[1][0] * matrix4x4.m[3][1])
		+ (matrix4x4.m[0][2] * matrix4x4.m[1][1] * matrix4x4.m[3][0])
		+ (matrix4x4.m[0][1] * matrix4x4.m[1][0] * matrix4x4.m[3][2])
		+ (matrix4x4.m[0][0] * matrix4x4.m[1][2] * matrix4x4.m[3][1]));

	resoultMatrix.m[3][3] = 1.0f / bottom * (
		(matrix4x4.m[0][0] * matrix4x4.m[1][1] * matrix4x4.m[2][2])
		+ (matrix4x4.m[0][1] * matrix4x4.m[1][2] * matrix4x4.m[2][0])
		+ (matrix4x4.m[0][2] * matrix4x4.m[1][0] * matrix4x4.m[2][1])
		- (matrix4x4.m[0][2] * matrix4x4.m[1][1] * matrix4x4.m[2][0])
		- (matrix4x4.m[0][1] * matrix4x4.m[1][0] * matrix4x4.m[2][2])
		- (matrix4x4.m[0][0] * matrix4x4.m[1][2] * matrix4x4.m[2][1]));

	return resoultMatrix;
}

Matrix4x4 Multiply(Matrix4x4 matrix1, Matrix4x4 matrix2)
{
	Matrix4x4 resoultMatrix4x4;

	resoultMatrix4x4.m[0][0] = matrix1.m[0][0] * matrix2.m[0][0] + matrix1.m[0][1] * matrix2.m[1][0] + matrix1.m[0][2] * matrix2.m[2][0] + matrix1.m[0][3] * matrix2.m[3][0];
	resoultMatrix4x4.m[0][1] = matrix1.m[0][0] * matrix2.m[0][1] + matrix1.m[0][1] * matrix2.m[1][1] + matrix1.m[0][2] * matrix2.m[2][1] + matrix1.m[0][3] * matrix2.m[3][1];
	resoultMatrix4x4.m[0][2] = matrix1.m[0][0] * matrix2.m[0][2] + matrix1.m[0][1] * matrix2.m[1][2] + matrix1.m[0][2] * matrix2.m[2][2] + matrix1.m[0][3] * matrix2.m[3][2];
	resoultMatrix4x4.m[0][3] = matrix1.m[0][0] * matrix2.m[0][3] + matrix1.m[0][1] * matrix2.m[1][3] + matrix1.m[0][2] * matrix2.m[2][3] + matrix1.m[0][3] * matrix2.m[3][3];

	resoultMatrix4x4.m[1][0] = matrix1.m[1][0] * matrix2.m[0][0] + matrix1.m[1][1] * matrix2.m[1][0] + matrix1.m[1][2] * matrix2.m[2][0] + matrix1.m[1][3] * matrix2.m[3][0];
	resoultMatrix4x4.m[1][1] = matrix1.m[1][0] * matrix2.m[0][1] + matrix1.m[1][1] * matrix2.m[1][1] + matrix1.m[1][2] * matrix2.m[2][1] + matrix1.m[1][3] * matrix2.m[3][1];
	resoultMatrix4x4.m[1][2] = matrix1.m[1][0] * matrix2.m[0][2] + matrix1.m[1][1] * matrix2.m[1][2] + matrix1.m[1][2] * matrix2.m[2][2] + matrix1.m[1][3] * matrix2.m[3][2];
	resoultMatrix4x4.m[1][3] = matrix1.m[1][0] * matrix2.m[0][3] + matrix1.m[1][1] * matrix2.m[1][3] + matrix1.m[1][2] * matrix2.m[2][3] + matrix1.m[1][3] * matrix2.m[3][3];

	resoultMatrix4x4.m[2][0] = matrix1.m[2][0] * matrix2.m[0][0] + matrix1.m[2][1] * matrix2.m[1][0] + matrix1.m[2][2] * matrix2.m[2][0] + matrix1.m[2][3] * matrix2.m[3][0];
	resoultMatrix4x4.m[2][1] = matrix1.m[2][0] * matrix2.m[0][1] + matrix1.m[2][1] * matrix2.m[1][1] + matrix1.m[2][2] * matrix2.m[2][1] + matrix1.m[2][3] * matrix2.m[3][1];
	resoultMatrix4x4.m[2][2] = matrix1.m[2][0] * matrix2.m[0][2] + matrix1.m[2][1] * matrix2.m[1][2] + matrix1.m[2][2] * matrix2.m[2][2] + matrix1.m[2][3] * matrix2.m[3][2];
	resoultMatrix4x4.m[2][3] = matrix1.m[2][0] * matrix2.m[0][3] + matrix1.m[2][1] * matrix2.m[1][3] + matrix1.m[2][2] * matrix2.m[2][3] + matrix1.m[2][3] * matrix2.m[3][3];

	resoultMatrix4x4.m[3][0] = matrix1.m[3][0] * matrix2.m[0][0] + matrix1.m[3][1] * matrix2.m[1][0] + matrix1.m[3][2] * matrix2.m[2][0] + matrix1.m[3][3] * matrix2.m[3][0];
	resoultMatrix4x4.m[3][1] = matrix1.m[3][0] * matrix2.m[0][1] + matrix1.m[3][1] * matrix2.m[1][1] + matrix1.m[3][2] * matrix2.m[2][1] + matrix1.m[3][3] * matrix2.m[3][1];
	resoultMatrix4x4.m[3][2] = matrix1.m[3][0] * matrix2.m[0][2] + matrix1.m[3][1] * matrix2.m[1][2] + matrix1.m[3][2] * matrix2.m[2][2] + matrix1.m[3][3] * matrix2.m[3][2];
	resoultMatrix4x4.m[3][3] = matrix1.m[3][0] * matrix2.m[0][3] + matrix1.m[3][1] * matrix2.m[1][3] + matrix1.m[3][2] * matrix2.m[2][3] + matrix1.m[3][3] * matrix2.m[3][3];

	return resoultMatrix4x4;
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth)
{
	Matrix4x4 viewportMatrix4x4;

	viewportMatrix4x4.m[0][0] = width / 2.0f;
	viewportMatrix4x4.m[0][1] = 0.0f;
	viewportMatrix4x4.m[0][2] = 0.0f;
	viewportMatrix4x4.m[0][3] = 0.0f;

	viewportMatrix4x4.m[1][0] = 0.0f;
	viewportMatrix4x4.m[1][1] = -height / 2.0f;
	viewportMatrix4x4.m[1][2] = 0.0f;
	viewportMatrix4x4.m[1][3] = 0.0f;

	viewportMatrix4x4.m[2][0] = 0.0f;
	viewportMatrix4x4.m[2][1] = 0.0f;
	viewportMatrix4x4.m[2][2] = maxDepth - minDepth;
	viewportMatrix4x4.m[2][3] = 0.0f;

	viewportMatrix4x4.m[3][0] = left + width / 2.0f;
	viewportMatrix4x4.m[3][1] = top + height / 2.0f;
	viewportMatrix4x4.m[3][2] = minDepth;
	viewportMatrix4x4.m[3][3] = 1.0f;

	return viewportMatrix4x4;
}

void UpdateCameraByMouse(Vector3& cameraTranslate, Vector3& cameraRotate)
{
	// マウスでカメラ移動// マウス座標を取得
	// マウス座標取得
	int mouseX, mouseY;
	Novice::GetMousePosition(&mouseX, &mouseY);

	// 状態保持
	static int prevMouseX = mouseX;
	static int prevMouseY = mouseY;
	static bool wasRotating = false;
	static bool wasPanning = false;

	// 左ボタン回転
	bool isRotating = Novice::IsPressMouse(0); // 左ボタン
	bool allowRotate =
		isRotating && !ImGui::IsAnyItemActive() && !ImGui::IsAnyItemHovered();

	// 右ボタン平行移動
	bool isPanning = Novice::IsPressMouse(1); // 右ボタン
	bool allowPan =
		isPanning && !ImGui::IsAnyItemActive() && !ImGui::IsAnyItemHovered();

	// 視点回転（左ドラッグ）
	if (allowRotate) {
		if (!wasRotating) {
			prevMouseX = mouseX;
			prevMouseY = mouseY;
		}

		float dx = static_cast<float>(mouseX - prevMouseX);
		float dy = static_cast<float>(mouseY - prevMouseY);

		cameraRotate.y -= dx * 0.001f;
		cameraRotate.x -= dy * 0.001f;

		const float piOver2 = 3.141592f / 2.0f;
		cameraRotate.x = std::clamp(cameraRotate.x, -piOver2, piOver2);

		prevMouseX = mouseX;
		prevMouseY = mouseY;
	}

	// カメラ平行移動（右ドラッグ）
	if (allowPan) {
		if (!wasPanning) {
			prevMouseX = mouseX;
			prevMouseY = mouseY;
		}

		float dx = static_cast<float>(mouseX - prevMouseX);
		float dy = static_cast<float>(mouseY - prevMouseY);

		// カメラの向きに応じてX方向とY方向に動くように補正
		float speed = 0.01f;
		cameraTranslate.x -= dx * speed;
		cameraTranslate.y += dy * speed;

		prevMouseX = mouseX;
		prevMouseY = mouseY;
	}

	wasRotating = isRotating;
	wasPanning = isPanning;

	// ホイールズーム
	ImGuiIO& io = ImGui::GetIO();
	cameraTranslate.z += io.MouseWheel * 0.5f;
}

void DrawSphere(const Vector3& center, float radius, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 20; //分割数
	const float kLatEvery = static_cast<float>(M_PI) / static_cast<float>(kSubdivision); // 緯度分割1つ分の角度 θd
	const float kLonEvery = static_cast<float>(2.0f * M_PI) / static_cast<float>(kSubdivision); // 経度分割1つ分の角度 φd

	// 緯度の方向に分割 -π/2~π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -static_cast<float>(M_PI) / 2.0f + kLatEvery * latIndex; // θ

		// 経度の方向に分割 θ~2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = kLonEvery * lonIndex; // φ

			// 緯線
			Vector3 a = {
				center.x + radius * cosf(lat) * cosf(lon),
				center.y + radius * sinf(lat),
				center.z + radius * cosf(lat) * sinf(lon)
			};

			Vector3 b = {
				center.x + radius * cosf(lat + kLatEvery) * cosf(lon),
				center.y + radius * sinf(lat + kLatEvery),
				center.z + radius * cosf(lat + kLatEvery) * sinf(lon)
			};

			// 経線
			Vector3 c = {
				center.x + radius * cosf(lat) * cosf(lon + kLonEvery),
				center.y + radius * sinf(lat),
				center.z + radius * cosf(lat) * sinf(lon + kLonEvery)
			};

			a = Transform(Transform(a, viewProjectionMatrix), viewportMatrix);
			b = Transform(Transform(b, viewProjectionMatrix), viewportMatrix);
			c = Transform(Transform(c, viewProjectionMatrix), viewportMatrix);

			Novice::DrawLine((int)a.x, (int)a.y, (int)b.x, (int)b.y, color);
			Novice::DrawLine((int)a.x, (int)a.y, (int)c.x, (int)c.y, color);
		}
	}
}

float Length(const Vector3& v)
{
	float length;

	length = sqrtf(powf(v.x, 2.0f) + powf(v.y, 2.0f) + powf(v.z, 2.0f));

	return length;
}

float Dot(const Vector3& v1, const Vector3& v2)
{
	float resoult;

	resoult = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

	return resoult;
}

Vector3 Normalize(const Vector3& v)
{
	Vector3 normalizedV;

	normalizedV = { v.x / Length(v),v.y / Length(v),v.z / Length(v) };

	return normalizedV;
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

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
{
	Matrix4x4 perspectiveFovMatrix;

	perspectiveFovMatrix.m[0][0] = 1.0f / aspectRatio * Cotangent(fovY / 2.0f);
	perspectiveFovMatrix.m[0][1] = 0.0f;
	perspectiveFovMatrix.m[0][2] = 0.0f;
	perspectiveFovMatrix.m[0][3] = 0.0f;

	perspectiveFovMatrix.m[1][0] = 0.0f;
	perspectiveFovMatrix.m[1][1] = Cotangent(fovY / 2.0f);
	perspectiveFovMatrix.m[1][2] = 0.0f;
	perspectiveFovMatrix.m[1][3] = 0.0f;

	perspectiveFovMatrix.m[2][0] = 0.0f;
	perspectiveFovMatrix.m[2][1] = 0.0f;
	perspectiveFovMatrix.m[2][2] = farClip / (farClip - nearClip);
	perspectiveFovMatrix.m[2][3] = 1.0f;

	perspectiveFovMatrix.m[3][0] = 0.0f;
	perspectiveFovMatrix.m[3][1] = 0.0f;
	perspectiveFovMatrix.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	perspectiveFovMatrix.m[3][3] = 0.0f;

	return perspectiveFovMatrix;
}