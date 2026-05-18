// Copyright Strayfarer & Contributors. Released under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "TargetCategoryTags.h"
#include "Components/SceneComponent.h"
#include "TargetComponent.generated.h"

namespace SF
{
	/**
	 * A targetable point on the owning actor.
	 * Target is (de)registered on begin/end play.
	 */
	UCLASS(ClassGroup="Target Search", meta=(BlueprintSpawnableComponent))
	class SFTARGETSEARCH_API UTargetComponent : public USceneComponent
	{
		GENERATED_BODY()

	public:
		UTargetComponent();

		// UActorComponent
		virtual void BeginPlay() override;
		virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
		// --

		FORCEINLINE void SetTargetCategory(const FGameplayTag& InTargetCategory)
		{
			TargetCategory = InTargetCategory;
		}

	protected:
		/** The target category to register this component as. */
		UPROPERTY(EditDefaultsOnly, meta=(Categories="TargetCategory"))
		FGameplayTag TargetCategory = TAG_TargetCategory_Default;
	};
}
