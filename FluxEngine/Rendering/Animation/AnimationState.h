#pragma once
struct Bone;
struct AnimationNode;
class AnimatedModel;
class Animation;

struct AnimationKeyState
{
	const Bone* pBone = nullptr;
	int KeyFrame = 0;
	const AnimationNode* pNode = nullptr;

	void GetFrameIndex(float time, int& index) const;
	void GetMatrix(const float time, Matrix& matrix);
};

class AnimationState
{
public:
	AnimationState(Animation* pAnimation, AnimatedModel* pModel);

	void AddTime(const float time);
	void SetTime(const float time);

	void Apply(std::vector<Matrix>& skinMatrices);

	Animation* GetAnimation() const { return m_pAnimation; }
	bool IsLooped() const { return m_Looped; }
	float GetDuration() const;

private:
	void CalculateAnimations(Bone* pBone, Matrix parentMatrix, std::vector<Matrix>& skinMatrices);
	
	float m_Time = 0.0f;
	bool m_IsDirty = true;
	bool m_Looped = true;
	Animation* m_pAnimation = nullptr;
	AnimatedModel* m_pModel = nullptr;
	Bone* m_pRootBone = nullptr;
	std::vector<AnimationKeyState> m_KeyStates;
};