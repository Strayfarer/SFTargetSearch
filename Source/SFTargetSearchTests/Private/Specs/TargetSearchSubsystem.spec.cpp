// Copyright Strayfarer & Contributors. Released under the MIT license.

#if WITH_AUTOMATION_WORKER

#include "TargetSearchSubsystem.h"
#include "Misc/AutomationTest.h"
#include "Mocks/TargetSearch_MockInstigatorObject.h"
#include "Mocks/TargetSearch_MockConditional.h"
#include "Mocks/TargetSearch_MockTargetObject.h"
#include "Mocks/TargetSearch_MockTargetQuery.h"

BEGIN_DEFINE_SPEC(FTargetSearchSubsystemSpec, "SF.TargetSearch.TargetSearchSubsystem", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
	TObjectPtr<UTargetSearch_MockInstigator> Instigator;
	TObjectPtr<UTargetSearch_MockConditional> Conditional;
	TObjectPtr<UTargetSearch_MockTargetQuery> Query;
	TObjectPtr<UTargetSearch_MockTarget> Target;
	TObjectPtr<SF::UTargetSearchSubsystem> TargetSearchSubsystem = nullptr;
END_DEFINE_SPEC(FTargetSearchSubsystemSpec)

void FTargetSearchSubsystemSpec::Define()
{
	BeforeEach([this]
	{
		TargetSearchSubsystem = NewObject<SF::UTargetSearchSubsystem>();
		Instigator = NewObject<UTargetSearch_MockInstigator>();
		Query = NewObject<UTargetSearch_MockTargetQuery>();
		Target = NewObject<UTargetSearch_MockTarget>();
		Conditional = NewObject<UTargetSearch_MockConditional>();
	});
	Describe("running a query", [this]
	{
		Describe("without instigator", [this]
		{
			It("should yield a valid result saying no target was found", [this]
			{
				Query->SetSearchCondition(Conditional);
				const SF::FTargetQueryResult Result = TargetSearchSubsystem->RunQuery(nullptr, Query);
				TestTrue("Result.IsValid()", Result.IsValid());
				TestTrue("Result.GetInstigator() == nullptr", Result.GetInstigator() == nullptr);
				TestEqual("Result.GetQuery()", Result.GetQuery(), Cast<SF::UTargetQueryDataAsset>(Query));
				TestFalse("Result.HasBestCandidate()", Result.HasBestCandidate());
				TestTrue("Result.GetCandidateResults().IsEmpty()", Result.GetCandidateResults().IsEmpty());
			});
		});
		Describe("without query", [this]
		{
			It("should yield an invalid result", [this]
			{
				Query->SetSearchCondition(Conditional);
				AddExpectedError("SF::UTargetSearchSubsystem::RunQuery called with invalid query!");
				TestFalse("Result.IsValid()", TargetSearchSubsystem->RunQuery(Instigator, nullptr).IsValid());
			});
		});
		Describe("without query that has an invalid search condition", [this]
		{
			It("should yield an invalid result", [this]
			{
				AddExpectedError("SF::UTargetSearchSubsystem::RunQuery called with query with invalid search condition!");
				TestFalse("Result.IsValid()", TargetSearchSubsystem->RunQuery(Instigator, Query).IsValid());
			});
		});
		Describe("without registering any target", [this]
		{
			It("should yield a valid result saying no target was found", [this]
			{
				Query->SetSearchCondition(Conditional);
				const SF::FTargetQueryResult Result = TargetSearchSubsystem->RunQuery(Instigator, Query);
				TestTrue("Result.IsValid()", Result.IsValid());
				TestEqual("Result.GetInstigator()", Result.GetInstigator(), Cast<UObject>(Instigator));
				TestEqual("Result.GetQuery()", Result.GetQuery(), Cast<SF::UTargetQueryDataAsset>(Query));
				TestFalse("Result.HasBestCandidate()", Result.HasBestCandidate());
				TestTrue("Result.GetCandidateResults().IsEmpty()", Result.GetCandidateResults().IsEmpty());
			});
		});
		Describe("without registering any target matching the target type", [this]
		{
			It("should yield a valid result saying no target was found", [this]
			{
				Query->SetSearchCondition(Conditional);
				Query->SetTargetCategory(TAG_TargetCategory_Test);
				TargetSearchSubsystem->RegisterTarget(Target, TAG_TargetCategory_Default);
				const SF::FTargetQueryResult Result = TargetSearchSubsystem->RunQuery(Instigator, Query);
				TestTrue("Result.IsValid()", Result.IsValid());
				TestEqual("Result.GetInstigator()", Result.GetInstigator(), Cast<UObject>(Instigator));
				TestEqual("Result.GetQuery()", Result.GetQuery(), Cast<SF::UTargetQueryDataAsset>(Query));
				TestFalse("Result.HasBestCandidate()", Result.HasBestCandidate());
				TestTrue("Result.GetCandidateResults().IsEmpty()", Result.GetCandidateResults().IsEmpty());
			});
		});
		Describe("with registering a target matching the target type", [this]
		{
			It("should yield a valid result saying that target was found", [this]
			{
				Query->SetSearchCondition(Conditional);
				Query->SetTargetCategory(TAG_TargetCategory_Test);
				TargetSearchSubsystem->RegisterTarget(Target, TAG_TargetCategory_Test);
				const SF::FTargetQueryResult Result = TargetSearchSubsystem->RunQuery(Instigator, Query);
				TestTrue("Result.IsValid()", Result.IsValid());
				TestEqual("Result.GetInstigator()", Result.GetInstigator(), Cast<UObject>(Instigator));
				TestEqual("Result.GetQuery()", Result.GetQuery(), Cast<SF::UTargetQueryDataAsset>(Query));
				TestTrue("Result.HasBestCandidate()", Result.HasBestCandidate());
				TestFalse("Result.GetCandidateResults().IsEmpty()", Result.GetCandidateResults().IsEmpty());
				TestEqual("Result.GetBestCandidateResult()", Result.GetBestCandidateResult().GetCandidate(), Cast<UObject>(Target));
				TestTrue("Result.GetBestCandidateResult()", Result.GetBestCandidateResult().IsBest());
				TestTrue("Result.GetBestCandidateResult().GetAssessment().GetBinaryAnswer()", Result.GetBestCandidateResult().GetAssessment().GetBinaryAnswer());
				TestEqual("Result.GetBestCandidateResult().GetAssessment().GetFuzzyAnswer()", Result.GetBestCandidateResult().GetAssessment().GetFuzzyAnswer(), 1.f);
			});
		});
	});
	Describe("registering a target", [this]
	{
		Describe("not at all", [this]
		{
			It("should yield the target is not registered", [this]
			{
				TestFalse("IsTargetRegistered()", TargetSearchSubsystem->IsTargetRegistered(Target, TAG_TargetCategory_Test));
			});
		});
		Describe("with null target", [this]
		{
			It("should yield the target is not registered and log an error", [this]
			{
				AddExpectedError("SF::UTargetSearchSubsystem::RegisterTarget called with invalid PotentialTarget parameter!");
				TargetSearchSubsystem->RegisterTarget(nullptr, TAG_TargetCategory_Test);
				TestFalse("IsTargetRegistered()", TargetSearchSubsystem->IsTargetRegistered(Target, TAG_TargetCategory_Test));
			});
		});
		Describe("with invalid target category", [this]
		{
			It("should yield the target is not registered and log an error", [this]
			{
				AddExpectedError("SF::UTargetSearchSubsystem::RegisterTarget called with TargetCategory parameter not being a sub-tag of 'TargetCategory'!");
				TargetSearchSubsystem->RegisterTarget(Target, TAG_TargetCategory);
				AddExpectedError("SF::UTargetSearchSubsystem::IsTargetRegistered called with TargetCategory parameter not being a sub-tag of 'TargetCategory'!");
				TestFalse("IsTargetRegistered()", TargetSearchSubsystem->IsTargetRegistered(Target, TAG_TargetCategory));
			});
		});
		Describe("with valid valid target and category", [this]
		{
			It("should yield the target is registered under the category used during registration", [this]
			{
				TargetSearchSubsystem->RegisterTarget(Target, TAG_TargetCategory_Test);
				TestTrue("IsTargetRegistered()", TargetSearchSubsystem->IsTargetRegistered(Target, TAG_TargetCategory_Test));
			});
			It("should yield the target is not registered under any other category", [this]
			{
				TargetSearchSubsystem->RegisterTarget(Target, TAG_TargetCategory_Test);
				TestFalse("IsTargetRegistered()", TargetSearchSubsystem->IsTargetRegistered(Target, TAG_TargetCategory_Default));
			});
		});
	});
}

#endif WITH_AUTOMATION_WORKER