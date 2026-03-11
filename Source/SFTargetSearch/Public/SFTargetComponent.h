// Copyright Strayfarer & Contributors. Released under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "SFTargetCategoryTags.h"
#include "Components/SceneComponent.h"
#include "SFTargetComponent.generated.h"

/**
 * A targetable point on the owning actor.
 * Target is (de)registered on (de)activation.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SFTARGETSEARCH_API USFTargetComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	USFTargetComponent();

	// - UActorComponent
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
	FGameplayTag TargetCategory = TAG_TargetCategory;
};
