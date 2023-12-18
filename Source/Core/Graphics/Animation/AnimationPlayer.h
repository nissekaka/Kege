#pragma once

namespace Kaka
{
	// TODO Try using this to make a 1D blend tree
	struct Animation
	{
		AnimationClip* clip = nullptr;
		float time = 0.0f;
		float speed = 1.0f;
		bool isPlaying = false;
		bool isLooping = true;

		std::vector<DirectX::XMMATRIX> combinedTransforms = {};
		std::vector<DirectX::XMMATRIX> finalTransforms = {};
	};

	class AnimationPlayer
	{
		friend class Model;
	public:
		AnimationPlayer() = default;
		~AnimationPlayer() = default;

		void Init(AnimatedModelDataPtr* aModelData);
		void Animate(float aDeltaTime);
		bool UpdateAnimation(const float aDeltaTime, Animation& aOutAnimation);

		void PlayAnimation(const std::string& aAnimationName, bool aShouldLoop = true, float aSpeed = 1.0f);
		void PlayAnimationBlend(const std::string& aAnimationName, bool aShouldLoop = true, float aSpeed = 1.0f,
		                        float aBlendTime = 0.2f);
		void PauseAnimation();
		void ResumeAnimation();
		void SetAnimationShouldLoop(bool aShouldLoop);
		void SetAnimationSpeed(float aSpeed);
		void SetBlendTime(float aBlendTime);

	private:
		AnimatedModelDataPtr* modelData = nullptr;

		Animation currentAnimation;
		Animation blendAnimation;

		bool isAnimationPlaying = false;

		bool isBlending = false;
		float blendTimer = 0.0f;
		float blendTime = 0.2f;
	};
}
