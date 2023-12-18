#include "stdafx.h"
#include "AnimationPlayer.h"

#include "Core/Graphics/Drawable/ModelData.h"

namespace Kaka
{
	void AnimationPlayer::Init(AnimatedModelDataPtr* aModelData)
	{
		modelData = aModelData;
		currentAnimation.combinedTransforms.resize(modelData->skeleton->bones.size());
		currentAnimation.finalTransforms.resize(modelData->skeleton->bones.size());
		blendAnimation.combinedTransforms.resize(modelData->skeleton->bones.size());
		blendAnimation.finalTransforms.resize(modelData->skeleton->bones.size());
	}

	void AnimationPlayer::Animate(const float aDeltaTime)
	{
		const Skeleton* skeleton = modelData->skeleton;

		if (UpdateAnimation(aDeltaTime, currentAnimation))
		{
			isAnimationPlaying = true;

			modelData->combinedTransforms = currentAnimation.combinedTransforms;
			modelData->finalTransforms = currentAnimation.finalTransforms;

			if (isBlending)
			{
				if (UpdateAnimation(aDeltaTime, blendAnimation))
				{
					blendTimer += aDeltaTime;
					const float blendFactor = (std::min)(1.0f, blendTimer / blendTime);

					for (size_t i = 0; i < skeleton->bones.size(); i++)
					{
						{
							const DirectX::XMMATRIX currentFramePose = currentAnimation.finalTransforms[i];
							const DirectX::XMMATRIX blendFramePose = blendAnimation.finalTransforms[i];

							// Blended pose needs to be multiplication of decomposed matrices
							DirectX::XMVECTOR currentScale, currentRotation, currentTranslation;
							DirectX::XMVECTOR blendScale, blendRotation, blendTranslation;
							DirectX::XMMatrixDecompose(&currentScale, &currentRotation, &currentTranslation, currentFramePose);
							DirectX::XMMatrixDecompose(&blendScale, &blendRotation, &blendTranslation, blendFramePose);

							DirectX::XMMATRIX blendedPose = DirectX::XMMatrixAffineTransformation(
								DirectX::XMVectorLerp(currentScale, blendScale, blendFactor),
								DirectX::XMVectorZero(),
								DirectX::XMQuaternionSlerp(currentRotation, blendRotation, blendFactor),
								DirectX::XMVectorLerp(currentTranslation, blendTranslation, blendFactor)
							);

							modelData->finalTransforms[i] = blendedPose;
						}

						{
							const DirectX::XMMATRIX currentFramePose = currentAnimation.combinedTransforms[i];
							const DirectX::XMMATRIX blendFramePose = blendAnimation.combinedTransforms[i];

							// Blended pose needs to be multiplication of decomposed matrices
							DirectX::XMVECTOR currentScale, currentRotation, currentTranslation;
							DirectX::XMVECTOR blendScale, blendRotation, blendTranslation;
							DirectX::XMMatrixDecompose(&currentScale, &currentRotation, &currentTranslation, currentFramePose);
							DirectX::XMMatrixDecompose(&blendScale, &blendRotation, &blendTranslation, blendFramePose);

							DirectX::XMMATRIX blendedPose = DirectX::XMMatrixAffineTransformation(
								DirectX::XMVectorLerp(currentScale, blendScale, blendFactor),
								DirectX::XMVectorZero(),
								DirectX::XMQuaternionSlerp(currentRotation, blendRotation, blendFactor),
								DirectX::XMVectorLerp(currentTranslation, blendTranslation, blendFactor)
							);

							modelData->combinedTransforms[i] = blendedPose;
						}
					}

					// If blending is complete, reset variables
					if (blendFactor >= 1.0f)
					{
						isBlending = false;
						blendTimer = 0.0f;
						currentAnimation.clip = blendAnimation.clip;
						currentAnimation.time = blendAnimation.time;
						currentAnimation.speed = blendAnimation.speed;
						currentAnimation.isPlaying = blendAnimation.isPlaying;
						currentAnimation.isLooping = blendAnimation.isLooping;
						blendAnimation.clip = nullptr;
					}
				}
			}
		}
		else
		{
			// Show bind pose (T-pose)
			isAnimationPlaying = false;
			for (size_t i = 0; i < skeleton->bones.size(); i++)
			{
				// This works because the vertex is not moved in the vertex shader
				modelData->combinedTransforms[i] = DirectX::XMMatrixInverse(nullptr, modelData->skeleton->bones[i].bindPose);
				modelData->finalTransforms[i] = DirectX::XMMatrixIdentity();
			}
		}
	}

	bool AnimationPlayer::UpdateAnimation(const float aDeltaTime, Animation& aOutAnimation)
	{
		const Skeleton* skeleton = modelData->skeleton;

		if (aOutAnimation.clip != nullptr)
		{
			// Apply animation transformation based on the current time
			if (aOutAnimation.isPlaying)
			{
				aOutAnimation.time += aDeltaTime * aOutAnimation.speed;

				if (aOutAnimation.isLooping)
				{
					if (aOutAnimation.time >= aOutAnimation.clip->duration)
					{
						aOutAnimation.time = 0.0f;
					}
				}
				else
				{
					if (aOutAnimation.time >= aOutAnimation.clip->duration)
					{
						aOutAnimation.time = aOutAnimation.clip->duration;
						return true;
					}
				}

				// Calculate the current frame and delta
				const float frameRate = 1.0f / aOutAnimation.clip->fps;
				const float result = aOutAnimation.time / frameRate;
				const size_t frame = (std::size_t)std::floor(result); // Current frame
				const float delta = result - static_cast<float>(frame); // Progress to the next frame

				// Interpolate between current and next frame
				for (size_t i = 0; i < skeleton->bones.size(); i++)
				{
					DirectX::XMMATRIX currentFramePose = aOutAnimation.clip->keyframes[frame].boneTransforms[i];
					DirectX::XMMATRIX nextFramePose = aOutAnimation.clip->keyframes[(frame + 1) % aOutAnimation.clip->keyframes.size()].boneTransforms[i];

					// Interpolate between current and next frame using delta
					DirectX::XMMATRIX blendedPose = currentFramePose + delta * (nextFramePose - currentFramePose);

					const int parentIndex = skeleton->bones[i].parentIndex;

					if (parentIndex >= 0)
					{
						// Accumulate relative transformation
						aOutAnimation.combinedTransforms[i] = blendedPose * aOutAnimation.combinedTransforms[parentIndex];
					}
					else
					{
						// Root bone, use absolute transformation
						aOutAnimation.combinedTransforms[i] = blendedPose;
					}

					aOutAnimation.finalTransforms[i] = skeleton->bones[i].bindPose * aOutAnimation.combinedTransforms[i];
				}
			}
			else
			{
				// Play paused frame i.e do nothing
			}
			return true;
		}
		aOutAnimation.time = 0.0f;

		return false;
	}

	void AnimationPlayer::PlayAnimation(const std::string& aAnimationName, const bool aShouldLoop, const float aSpeed)
	{
		if (this == nullptr)
		{
			return;
		}

		if (isBlending)
		{
			isBlending = false;
			currentAnimation.clip = modelData->animationClipMap[aAnimationName];
			currentAnimation.time = 0.0f;
			currentAnimation.speed = aSpeed;
			currentAnimation.isPlaying = true;
			currentAnimation.isLooping = aShouldLoop;
			return;
		}

		if (modelData->animationClipMap.empty())
		{
			return;
		}

		if (currentAnimation.clip != nullptr && currentAnimation.clip->name == aAnimationName)
		{
			return;
		}

		if (currentAnimation.clip != nullptr)
		{
			PlayAnimationBlend(aAnimationName, aShouldLoop, aSpeed, blendTime);
			return;
		}

		currentAnimation.clip = modelData->animationClipMap[aAnimationName];
		currentAnimation.time = 0.0f;
		currentAnimation.speed = aSpeed;
		currentAnimation.isPlaying = true;
		currentAnimation.isLooping = aShouldLoop;
	}

	void AnimationPlayer::PlayAnimationBlend(const std::string& aAnimationName, const bool aShouldLoop,
	                                         const float aSpeed, const float aBlendTime)
	{
		if (this == nullptr)
		{
			return;
		}
		if (modelData->animationClipMap.empty())
		{
			return;
		}

		if (currentAnimation.clip == nullptr)
		{
			PlayAnimation(aAnimationName, aShouldLoop, aSpeed);
			return;
		}

		blendAnimation.clip = modelData->animationClipMap[aAnimationName];
		isBlending = true;
		blendTime = aBlendTime;
		blendAnimation.time = 0.0f;
		blendAnimation.speed = aSpeed;
		blendAnimation.isPlaying = true;
		blendAnimation.isLooping = aShouldLoop;
	}

	void AnimationPlayer::PauseAnimation()
	{
		currentAnimation.isPlaying = false;
	}

	void AnimationPlayer::ResumeAnimation()
	{
		currentAnimation.isPlaying = true;
	}

	void AnimationPlayer::SetAnimationShouldLoop(const bool aShouldLoop)
	{
		currentAnimation.isLooping = aShouldLoop;
	}

	void AnimationPlayer::SetAnimationSpeed(const float aSpeed)
	{
		currentAnimation.speed = aSpeed;
	}

	void AnimationPlayer::SetBlendTime(const float aBlendTime)
	{
		blendTime = aBlendTime;
	}
}
