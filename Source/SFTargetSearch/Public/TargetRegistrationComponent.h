// Copyright Strayfarer & Contributors. Released under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TargetCategoryTags.h"
#include "Components/ActorComponent.h"
#include "TargetRegistrationComponent.generated.h"


namespace SF
{
	/**
	 * Register the actor and/or root scene component as target.
	 * (De)registration happens on (de)activation.
	 */
	UCLASS(ClassGroup="Target Search", meta=(BlueprintSpawnableComponent))
	class SFTARGETSEARCH_API UTargetRegistrationComponent : public UActorComponent
	{
		GENERATED_BODY()

	public:
		UTargetRegistrationComponent();

		// UActorComponent
		virtual void BeginPlay() override;
		virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
		virtual void Activate(bool bReset = false) override;
		virtual void Deactivate() override;
		// --

	protected:
		/**The target category of any target registered by this component. */
		UPROPERTY(EditDefaultsOnly, meta=(Categories="TargetCategory"))
		FGameplayTag TargetCategory = TAG_TargetCategory_Default;

		/** If true, register the owning actor as a target. */
		UPROPERTY(EditDefaultsOnly)
		bool bAutoRegisterOwnerActor = false;

		/** If true, register the owning actors root component as a target. */
		UPROPERTY(EditDefaultsOnly)
		bool bAutoRegisterOwnerRootComponent = false;
	};
}
