// Copyright Strayfarer & Contributors. Released under the MIT license.

#if WITH_AUTOMATION_WORKER

#include "TargetComponent.h"
#include "TargetSearchSubsystem.h"
#include "Misc/AutomationTest.h"
#include "Mocks/TargetSearch_MockActor.h"
#include "TestWorld/AutomationTestWorld.h"

BEGIN_DEFINE_SPEC(FTargetComponentSpec, "SF.TargetSearch.TargetComponent", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
	TSharedPtr<WeekendUtils::FScopedAutomationTestWorld> TestWorld;
	TObjectPtr<AActor> Actor;
	TObjectPtr<SF::UTargetComponent> Sut = nullptr;
	TObjectPtr<SF::UTargetSearchSubsystem> TargetSearchSubsystem = nullptr;
END_DEFINE_SPEC(FTargetComponentSpec)

void FTargetComponentSpec::Define()
{
	BeforeEach([this]
	{
		TestWorld = MakeShared<WeekendUtils::FScopedAutomationTestWorld>("TestWorld");
		TestWorld->InitializeGame();
		
		TargetSearchSubsystem = NewObject<SF::UTargetSearchSubsystem>(TestWorld->AsPtr());
	});
	AfterEach([this]
	{
		TestWorld.Reset();
	});
	Describe("spawning an actor with target component", [this]
	{
		It("should register the target component", [this]
		{
			Actor = TestWorld->AsRef().SpawnActorDeferred<ATargetSearchMockActor>(ATargetSearchMockActor::StaticClass(), FTransform::Identity);
			Sut = Cast<SF::UTargetComponent>(Actor->AddComponentByClass(SF::UTargetComponent::StaticClass(), false, FTransform::Identity, true));
		
			Sut->SetTargetCategory(TAG_TargetCategory);
		
			Actor->FinishAddComponent(Sut, false, FTransform::Identity);
			Actor->FinishSpawning(FTransform::Identity);
			
			TestTrue("IsTargetRegistered", TargetSearchSubsystem->IsTargetRegistered(Sut, TAG_TargetCategory));
		});
	});
	Describe("spawning an actor with target component that is then destroyed", [this]
	{
		It("should deregister the target component", [this]
		{
			Actor = TestWorld->AsRef().SpawnActorDeferred<ATargetSearchMockActor>(ATargetSearchMockActor::StaticClass(), FTransform::Identity);
			Sut = Cast<SF::UTargetComponent>(Actor->AddComponentByClass(SF::UTargetComponent::StaticClass(), false, FTransform::Identity, true));
		
			Sut->SetTargetCategory(TAG_TargetCategory);
		
			Actor->FinishAddComponent(Sut, false, FTransform::Identity);
			Actor->FinishSpawning(FTransform::Identity);
			
			Actor->Destroy();
			
			TestFalse("IsTargetRegistered", TargetSearchSubsystem->IsTargetRegistered(Sut, TAG_TargetCategory));
		});
	});
}

#endif WITH_AUTOMATION_WORKER