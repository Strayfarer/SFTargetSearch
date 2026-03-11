// Copyright Strayfarer & Contributors. Released under the MIT license.

#if WITH_AUTOMATION_WORKER

#include "SFTargetComponent.h"
#include "SFTargetSearchSubsystem.h"
#include "Misc/AutomationTest.h"
#include "Mocks/SFTargetSearch_MockActor.h"
#include "TestWorld/AutomationTestWorld.h"

BEGIN_DEFINE_SPEC(FTargetComponentSpec, "SF.TargetSearch.TargetComponent", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
	TSharedPtr<WeekendUtils::FScopedAutomationTestWorld> TestWorld;
	TObjectPtr<AActor> Actor;
	TObjectPtr<USFTargetComponent> Sut = nullptr;
	TObjectPtr<USFTargetSearchSubsystem> TargetSearchSubsystem = nullptr;
END_DEFINE_SPEC(FTargetComponentSpec)

void FTargetComponentSpec::Define()
{
	BeforeEach([this]
	{
		TestWorld = MakeShared<WeekendUtils::FScopedAutomationTestWorld>("TestWorld");
		TestWorld->InitializeGame();
		
		TargetSearchSubsystem = NewObject<USFTargetSearchSubsystem>(TestWorld->AsPtr());
	});
	AfterEach([this]
	{
		TestWorld.Reset();
	});
	Describe("spawning an actor with target component", [this]
	{
		It("should register the target component", [this]
		{
			Actor = TestWorld->AsRef().SpawnActorDeferred<ASFTargetSearchMockActor>(ASFTargetSearchMockActor::StaticClass(), FTransform::Identity);
			Sut = Cast<USFTargetComponent>(Actor->AddComponentByClass(USFTargetComponent::StaticClass(), false, FTransform::Identity, true));
		
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
			Actor = TestWorld->AsRef().SpawnActorDeferred<ASFTargetSearchMockActor>(ASFTargetSearchMockActor::StaticClass(), FTransform::Identity);
			Sut = Cast<USFTargetComponent>(Actor->AddComponentByClass(USFTargetComponent::StaticClass(), false, FTransform::Identity, true));
		
			Sut->SetTargetCategory(TAG_TargetCategory);
		
			Actor->FinishAddComponent(Sut, false, FTransform::Identity);
			Actor->FinishSpawning(FTransform::Identity);
			
			Actor->Destroy();
			
			TestFalse("IsTargetRegistered", TargetSearchSubsystem->IsTargetRegistered(Sut, TAG_TargetCategory));
		});
	});
}

#endif WITH_AUTOMATION_WORKER