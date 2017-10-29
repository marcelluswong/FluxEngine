#include "stdafx.h"
#include "FreeCamera.h"
#include "Camera.h"
#include "Core/Components/Transform.h"
#include "Audio/AudioListener.h"

FreeCamera::FreeCamera(InputEngine* pInput, Graphics* pGraphics) : 
	m_pInput(pInput), m_pGraphics(pGraphics)
{
}


FreeCamera::~FreeCamera()
{
}

void FreeCamera::Initialize()
{
	m_pCamera = new Camera(m_pInput, m_pGraphics);
	AddComponent(m_pCamera);
	AudioListener* pAudioListener = new AudioListener();
	AddComponent(pAudioListener);
}

void FreeCamera::Update()
{
	if(m_UseMouseAndKeyboard)
		KeyboardMouse();
	//if (m_UseController)
	//	Controller();
}

void FreeCamera::KeyboardMouse()
{
	float dt = GameTimer::DeltaTime();

	//Moving
	XMFLOAT3 moveDirection(0, 0, 0);
	moveDirection.x += m_pInput->IsKeyboardKeyDown('A') ? -1 : 0;
	moveDirection.x += m_pInput->IsKeyboardKeyDown('D') ? 1 : 0;
	moveDirection.z += m_pInput->IsKeyboardKeyDown('S') ? -1 : 0;
	moveDirection.z += m_pInput->IsKeyboardKeyDown('W') ? 1 : 0;
	moveDirection.y += m_pInput->IsKeyboardKeyDown('Q') ? -1 : 0;
	moveDirection.y += m_pInput->IsKeyboardKeyDown('E') ? 1 : 0;

	XMVECTOR xmMove = XMLoadFloat3(&moveDirection);
	
	float moveSpeed = m_MoveSpeed;
	if ( m_pInput->IsKeyboardKeyDown(VK_LSHIFT))
		moveSpeed *= m_ShiftMultiplier;

	xmMove *= dt * moveSpeed;
	GetTransform()->Translate(xmMove, Space::SELF);

	//Rotation
	if ( m_pInput->IsMouseButtonDown(VK_RBUTTON))
	{
		XMFLOAT2 mouseMove =  m_pInput->GetMouseMovement();
		GetTransform()->Rotate(XMFLOAT3(mouseMove.y * dt * m_RotationSpeed, 0.0f, 0.0f), Space::SELF);
		GetTransform()->Rotate(XMFLOAT3(0.0f, mouseMove.x * dt * m_RotationSpeed, 0.0f), Space::WORLD);
	}
}

void FreeCamera::Controller()
{
	float dt = GameTimer::DeltaTime();

	XMFLOAT2 leftStick =  m_pInput->GetThumbstickPosition();
	XMFLOAT2 rightStick =  m_pInput->GetThumbstickPosition(false);
	bool lb =  m_pInput->IsGamepadButtonDown(XINPUT_GAMEPAD_LEFT_SHOULDER);
	bool rb =  m_pInput->IsGamepadButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER);
	bool leftStickPress =  m_pInput->IsGamepadButtonDown(XINPUT_GAMEPAD_LEFT_THUMB);

	//Moving
	XMFLOAT3 moveDirection(0, 0, 0);
	moveDirection.x = leftStick.x;
	moveDirection.z = leftStick.y;
	moveDirection.y += lb ? -1 : 0;
	moveDirection.y += rb ? 1 : 0;

	XMVECTOR xmMove = XMLoadFloat3(&moveDirection);

	float moveSpeed = m_MoveSpeed;
	if (leftStickPress)
		moveSpeed *= m_ShiftMultiplier;

	xmMove *= dt * moveSpeed;
	GetTransform()->Translate(xmMove, Space::SELF);

	//Rotation
	GetTransform()->Rotate(XMFLOAT3(-rightStick.y * dt * m_RotationSpeed*m_GamepadSensitivity, 0.0f, 0.0f), Space::SELF);
	GetTransform()->Rotate(XMFLOAT3(0.0f, rightStick.x * dt * m_RotationSpeed*m_GamepadSensitivity, 0.0f), Space::WORLD);
}
