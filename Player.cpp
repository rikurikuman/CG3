#include "Player.h"
#include "RInput.h"
#include "TimeManager.h"
#include "RAudio.h"
#include "Quaternion.h"

Player::Player()
{
	obj = ModelObj(Model::Load("./Resources/Model/VicViper/", "VicViper.obj"));

	obj.transform.position = { 0, 0, 0 };
	obj.transform.scale = { 0.1f, 0.1f, 0.1f };
	obj.transform.UpdateMatrix();

	col.pos = { 0, 0, 0 };
	col.r = 0.5f;
}

void Player::Update()
{
	if (Util::debugBool && RInput::GetKey(DIK_S)) {
		startFlag = true;
	}

	if (!initFlag) {
		if (course != nullptr) {
			for (Course::CheckPoint cp : course->checkPoints) {
				if (cp.name == "goal") {
					transform.position = cp.col.pos;
					camera.viewProjection.eye = cp.col.pos;
					camera.viewProjection.target = camera.viewProjection.eye + (Vector3(0, 0, 1) * Matrix4::RotationY(transform.rotation.y));
					camera.viewProjection.UpdateMatrix();
				}
			}
		}
		initFlag = true;
	}

	if (startFlag && RInput::GetKey(DIK_LEFT)) {
		transform.rotation.y -= Util::AngleToRadian(2);
		transform.rotation.z += Util::AngleToRadian(3);
		if (transform.rotation.z >= Util::AngleToRadian(20)) {
			transform.rotation.z = Util::AngleToRadian(20);
		}
	}
	else if (startFlag && RInput::GetKey(DIK_RIGHT)) {
		transform.rotation.y += Util::AngleToRadian(2);
		transform.rotation.z -= Util::AngleToRadian(3);
		if (obj.transform.rotation.z <= Util::AngleToRadian(-20)) {
			transform.rotation.z = Util::AngleToRadian(-20);
		}
	}
	else {
		if (transform.rotation.z != 0) {
			if (transform.rotation.z > 0) {
				transform.rotation.z -= Util::AngleToRadian(2);
				if (transform.rotation.z < 0) {
					transform.rotation.z = 0;
				}
			}
			else {
				transform.rotation.z += Util::AngleToRadian(2);
				if (transform.rotation.z > 0) {
					transform.rotation.z = 0;
				}
			}
		}
	}

	if (RInput::GetKey(DIK_SPACE)) {
		obj.transform.position.y = -0.2f;

		speed -= 0.02f * speed;
		velocity -= velocity * 0.02f;
		if (speed <= 0) {
			speed = 0;
		}
		charge += TimeManager::deltaTime;

		if (!charging && charge >= 0.5f) {
			charging = true;
			RAudio::Play("ChargeMax", 1.0f, 1.0f);
		}
		if (!charging) {
			RAudio::Play("Charge", 3.0f, 2.0f * charge / 0.5f);
		}
	}
	else {
		obj.transform.position.y = 0.0f;

		Vector3 moveVec = { 0, 0, 1 };
		moveVec *= Matrix4::RotationY(transform.rotation.y);

		if (charge >= 0.5f) {
			speed = 0.4f;
			velocity += moveVec * 0.4f;
			RAudio::Play("Dash", 0.5f, 1.0f);
		}
		else {
			velocity = moveVec * velocity.Length();
		}
		charge = 0;
		charging = false;

		Vector3 result = (moveVec.GetNormalize() * 0.2f + velocity.GetNormalize() * 0.8f).Normalize();

		velocity = result * (velocity.Length() + 0.2f * TimeManager::deltaTime);
		speed += 0.2f * TimeManager::deltaTime;
	}

	if (speed > 0) {
		speed -= 0.02f * speed;
		velocity -= velocity * 0.02f;
		if (speed <= 0) {
			speed = 0;
		}
	}

	if (startFlag) {
		Vector3 moveVec = { 0, 0, 1 };
		moveVec *= Matrix4::RotationY(transform.rotation.y);

		//transform.position += moveVec * speed;
		transform.position += velocity;
		transform.position.x = Util::Clamp(transform.position.x, -50.0f, 50.0f);
		transform.position.z = Util::Clamp(transform.position.z, -50.0f, 50.0f);

		if (speed > 0.005f) {
			RAudio::Play("Engine", 1.0f, 0.1f + 0.9f * min(1.0f, speed * 4));
		}
	}
	else {
		speed = 0;
	}

	col.pos = transform.position;

	if (course != nullptr) {
		if (prevCheckPoints.empty()) {
			prevCheckPoints = "goal";
			for (Course::CheckPoint cp : course->checkPoints) {
				if (cp.name == "goal") {
					nextCheckPoints = cp.next;
				}
			}
		}

		for (ColPrimitive3D::Triangle tri : course->courseColMeshes) {
			Vector3 hoge;
			if (ColPrimitive3D::IsHit(col, tri, &hoge)) {
				Vector3 dis = hoge - col.pos;

				float push = col.r - dis.Length();

				Vector3 moveVec = { 0, 0, 1 };
				moveVec *= Matrix4::RotationY(transform.rotation.y);

				float power = abs(moveVec.Dot(tri.normal));

				speed -= speed * powf(power, 6.0f);
				velocity -= velocity * powf(power, 6.0f);

				col.pos += tri.normal * push * 1.01f;
			}
		}

		Course::CheckPoint prevCP = {};
		Course::CheckPoint nextCP = {};
		for (Course::CheckPoint cp : course->checkPoints) {
			if (cp.name == prevCheckPoints) {
				prevCP = cp;
			}
			if (cp.name == nextCheckPoints) {
				nextCP = cp;
			}
		}

		for (Course::CheckPoint cp : course->checkPoints) {
			if (ColPrimitive3D::IsHit(col, cp.col)) {
				prevCheckPoints = cp.name;

				if (cp.name == nextCP.name
					&& cp.name == prevCP.next) {
					nextCheckPoints = cp.next;

					if (cp.line) {
						lap++;
						RAudio::Play("Lap");
					}
				}
			}
		}
	}

	transform.position = col.pos;
	transform.UpdateMatrix();

	obj.transform.position = transform.position;
	obj.transform.rotation = transform.rotation;

	if (RInput::GetKey(DIK_SPACE)) {
		obj.transform.position.y = transform.position.y - 0.1f;
	}
	else {
		obj.transform.position.y = transform.position.y;
	}

	obj.transform.UpdateMatrix();

	Vector3 frontVec;

	if (RInput::GetKey(DIK_SPACE)) {
		frontVec = { 0, 0, 1 };
		frontVec *= Matrix4::RotationY(transform.rotation.y);
	}
	else {
		frontVec = velocity.GetNormalize();
		if (frontVec.LengthSq() == 0) {
			frontVec = { 0, 0, 1 };
			frontVec *= Matrix4::RotationY(transform.rotation.y);
		}
	}

	Vector3 targetPos = transform.position + frontVec * 3.0f;

	Vector3 cameraVec = -frontVec;
	cameraVec *= Quaternion::AngleAxis(Vector3(0, 1, 0).Cross(frontVec), Util::AngleToRadian(25));
	cameraVec *= 3.0f;

	Vector3 diffPos = (transform.position + cameraVec) - camera.viewProjection.eye;
	Vector3 diffTarget = targetPos - camera.viewProjection.target;

	camera.viewProjection.eye += diffPos / 5;
	camera.viewProjection.target += diffTarget / 5;
	camera.viewProjection.UpdateMatrix();

	obj.TransferBuffer(Camera::nowCamera->viewProjection);
}

void Player::Draw()
{
	obj.DrawCommands();
}
