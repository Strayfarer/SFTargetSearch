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
	TObjectPtr<UTargetSearch_MockTarget> TargetA;
	TObjectPtr<UTargetSearch_MockTarget> TargetB;
	TObjectPtr<SF::UTargetSearchSubsystem> TargetSearchSubsystem = nullptr;
END_DEFINE_SPEC(FTargetSearchSubsystemSpec)

void FTargetSearchSubsystemSpec::Define()
{
	BeforeEach([this]
	{
		TargetSearchSubsystem = NewObject<SF::UTargetSearchSubsystem>();
		Instigator = NewObject<UTargetSearch_MockInstigator>();
		Query = NewObject<UTargetSearch_MockTargetQuery>();
		TargetA = NewObject<UTargetSearch_MockTarget>();
		TargetB = NewObject<UTargetSearch_MockTarget>();
		Conditional = NewObject<UTargetSearch_MockConditional>();
		Conditional->TargetA = TargetA;
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
				TargetSearchSubsystem->RegisterTarget(TargetA, TAG_TargetCategory_Default);
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
				TargetSearchSubsystem->RegisterTarget(TargetA, TAG_TargetCategory_Test);
				const SF::FTargetQueryResult Result = TargetSearchSubsystem->RunQuery(Instigator, Query);
				TestTrue("Result.IsValid()", Result.IsValid());
				TestEqual("Result.GetInstigator()", Result.GetInstigator(), Cast<UObject>(Instigator));
				TestEqual("Result.GetQuery()", Result.GetQuery(), Cast<SF::UTargetQueryDataAsset>(Query));
				TestTrue("Result.HasBestCandidate()", Result.HasBestCandidate());
				TestEqual("Result.GetCandidateResults().Num()", Result.GetCandidateResults().Num(), 1);
				TestEqual("Result.GetBestCandidateResult()", Result.GetBestCandidateResult().GetCandidate(), Cast<UObject>(TargetA));
				TestTrue("Result.GetBestCandidateResult().IsBest()", Result.GetBestCandidateResult().IsBest());
				TestTrue("Result.GetBestCandidateResult().GetAssessment().GetBinaryAnswer()", Result.GetBestCandidateResult().GetAssessment().GetBinaryAnswer());
				TestEqual("Result.GetBestCandidateResult().GetAssessment().GetFuzzyAnswer()", Result.GetBestCandidateResult().GetAssessment().GetFuzzyAnswer(), 1.f);
			});
		});
		Describe("with registering a target matching the target type but not evaluating to true binary assessment", [this]
		{
			It("should yield a valid result saying that target was found", [this]
			{
				Conditional->AnswerTargetA = SF::Conditional::Answer::No();
				Query->SetSearchCondition(Conditional);
				Query->SetTargetCategory(TAG_TargetCategory_Test);
				TargetSearchSubsystem->RegisterTarget(TargetA, TAG_TargetCategory_Test);
				const SF::FTargetQueryResult Result = TargetSearchSubsystem->RunQuery(Instigator, Query);
				TestTrue("Result.IsValid()", Result.IsValid());
				TestEqual("Result.GetInstigator()", Result.GetInstigator(), Cast<UObject>(Instigator));
				TestEqual("Result.GetQuery()", Result.GetQuery(), Cast<SF::UTargetQueryDataAsset>(Query));
				TestFalse("Result.HasBestCandidate()", Result.HasBestCandidate());
				TestEqual("Result.GetCandidateResults().Num()", Result.GetCandidateResults().Num(), 1);
			});
		});
		Describe("with registering two targets matching the target type", [this]
		{
			Describe("with target A yielding yes and target B yielding no answer", [this]
			{
				It("should yield a valid result saying that target A was found", [this]
				{
					Query->SetSearchCondition(Conditional);
					Query->SetTargetCategory(TAG_TargetCategory_Test);
					TargetSearchSubsystem->RegisterTarget(TargetA, TAG_TargetCategory_Test);
					TargetSearchSubsystem->RegisterTarget(TargetB, TAG_TargetCategory_Test);
					const SF::FTargetQueryResult Result = TargetSearchSubsystem->RunQuery(Instigator, Query);
					TestTrue("Result.IsValid()", Result.IsValid());
					TestEqual("Result.GetInstigator()", Result.GetInstigator(), Cast<UObject>(Instigator));
					TestEqual("Result.GetQuery()", Result.GetQuery(), Cast<SF::UTargetQueryDataAsset>(Query));
					TestTrue("Result.HasBestCandidate()", Result.HasBestCandidate());
					TestEqual("Result.GetCandidateResults().Num()", Result.GetCandidateResults().Num(), 2);
					TestEqual("Result.GetBestCandidateResult()", Result.GetBestCandidateResult().GetCandidate(), Cast<UObject>(TargetA));
					TestTrue("Result.GetBestCandidateResult().IsBest()", Result.GetBestCandidateResult().IsBest());
					TestTrue("Result.GetBestCandidateResult().GetAssessment().GetBinaryAnswer()", Result.GetBestCandidateResult().GetAssessment().GetBinaryAnswer());
					TestEqual("Result.GetBestCandidateResult().GetAssessment().GetFuzzyAnswer()", Result.GetBestCandidateResult().GetAssessment().GetFuzzyAnswer(), 1.f);
				});
			});
			Describe("with target A yielding [true, 1f] and target B yielding [true, 0f] answer", [this]
			{
				It("should yield a valid result saying that target A was found", [this]
				{
					Conditional->AnswerTargetA = SF::Conditional::Answer::Yes();
					Conditional->AnswerTargetB = SF::Conditional::Answer::Create(true, 0.f);
					Query->SetSearchCondition(Conditional);
					Query->SetTargetCategory(TAG_TargetCategory_Test);
					TargetSearchSubsystem->RegisterTarget(TargetA, TAG_TargetCategory_Test);
					TargetSearchSubsystem->RegisterTarget(TargetB, TAG_TargetCategory_Test);
					const SF::FTargetQueryResult Result = TargetSearchSubsystem->RunQuery(Instigator, Query);
					TestTrue("Result.IsValid()", Result.IsValid());
					TestEqual("Result.GetInstigator()", Result.GetInstigator(), Cast<UObject>(Instigator));
					TestEqual("Result.GetQuery()", Result.GetQuery(), Cast<SF::UTargetQueryDataAsset>(Query));
					TestTrue("Result.HasBestCandidate()", Result.HasBestCandidate());
					TestEqual("Result.GetCandidateResults().Num()", Result.GetCandidateResults().Num(), 2);
					TestEqual("Result.GetBestCandidateResult()", Result.GetBestCandidateResult().GetCandidate(), Cast<UObject>(TargetA));
					TestTrue("Result.GetBestCandidateResult().IsBest()", Result.GetBestCandidateResult().IsBest());
					TestTrue("Result.GetBestCandidateResult().GetAssessment().GetBinaryAnswer()", Result.GetBestCandidateResult().GetAssessment().GetBinaryAnswer());
					TestEqual("Result.GetBestCandidateResult().GetAssessment().GetFuzzyAnswer()", Result.GetBestCandidateResult().GetAssessment().GetFuzzyAnswer(), 1.f);
				});
			});
			Describe("with target A yielding [true, 0f] and target B yielding [false, 1f] answer", [this]
			{
				It("should yield a valid result saying that target A was found", [this]
				{
					Conditional->AnswerTargetA = SF::Conditional::Answer::Create(true, 0.f);
					Conditional->AnswerTargetB = SF::Conditional::Answer::Create(false, 1.f);
					Query->SetSearchCondition(Conditional);
					Query->SetTargetCategory(TAG_TargetCategory_Test);
					TargetSearchSubsystem->RegisterTarget(TargetA, TAG_TargetCategory_Test);
					TargetSearchSubsystem->RegisterTarget(TargetB, TAG_TargetCategory_Test);
					const SF::FTargetQueryResult Result = TargetSearchSubsystem->RunQuery(Instigator, Query);
					TestTrue("Result.IsValid()", Result.IsValid());
					TestEqual("Result.GetInstigator()", Result.GetInstigator(), Cast<UObject>(Instigator));
					TestEqual("Result.GetQuery()", Result.GetQuery(), Cast<SF::UTargetQueryDataAsset>(Query));
					TestTrue("Result.HasBestCandidate()", Result.HasBestCandidate());
					TestEqual("Result.GetCandidateResults().Num()", Result.GetCandidateResults().Num(), 2);
					TestEqual("Result.GetBestCandidateResult()", Result.GetBestCandidateResult().GetCandidate(), Cast<UObject>(TargetA));
					TestTrue("Result.GetBestCandidateResult().IsBest()", Result.GetBestCandidateResult().IsBest());
					TestTrue("Result.GetBestCandidateResult().GetAssessment().GetBinaryAnswer()", Result.GetBestCandidateResult().GetAssessment().GetBinaryAnswer());
					TestEqual("Result.GetBestCandidateResult().GetAssessment().GetFuzzyAnswer()", Result.GetBestCandidateResult().GetAssessment().GetFuzzyAnswer(), 0.f);
				});
			});
			Describe("with target A yielding [true, 1f] and target B yielding [true, 0,5f] answer", [this]
			{
				It("should yield a valid result saying that target A was found", [this]
				{
					Conditional->AnswerTargetA = SF::Conditional::Answer::Yes();
					Conditional->AnswerTargetB = SF::Conditional::Answer::Create(true, 0.5f);
					Query->SetSearchCondition(Conditional);
					Query->SetTargetCategory(TAG_TargetCategory_Test);
					TargetSearchSubsystem->RegisterTarget(TargetA, TAG_TargetCategory_Test);
					TargetSearchSubsystem->RegisterTarget(TargetB, TAG_TargetCategory_Test);
					const SF::FTargetQueryResult Result = TargetSearchSubsystem->RunQuery(Instigator, Query);
					TestTrue("Result.IsValid()", Result.IsValid());
					TestEqual("Result.GetInstigator()", Result.GetInstigator(), Cast<UObject>(Instigator));
					TestEqual("Result.GetQuery()", Result.GetQuery(), Cast<SF::UTargetQueryDataAsset>(Query));
					TestTrue("Result.HasBestCandidate()", Result.HasBestCandidate());
					TestEqual("Result.GetCandidateResults().Num()", Result.GetCandidateResults().Num(), 2);
					TestEqual("Result.GetBestCandidateResult()", Result.GetBestCandidateResult().GetCandidate(), Cast<UObject>(TargetA));
					TestTrue("Result.GetBestCandidateResult().IsBest()", Result.GetBestCandidateResult().IsBest());
					TestTrue("Result.GetBestCandidateResult().GetAssessment().GetBinaryAnswer()", Result.GetBestCandidateResult().GetAssessment().GetBinaryAnswer());
					TestEqual("Result.GetBestCandidateResult().GetAssessment().GetFuzzyAnswer()", Result.GetBestCandidateResult().GetAssessment().GetFuzzyAnswer(), 1.f);
				});
			});
			Describe("with target A yielding [true, 0,5f] and target B yielding [true, 0,7f] answer", [this]
			{
				It("should yield a valid result saying that target B was found", [this]
				{
					Conditional->AnswerTargetA = SF::Conditional::Answer::Create(true, 0.5f);
					Conditional->AnswerTargetB = SF::Conditional::Answer::Create(true, 0.7f);
					Query->SetSearchCondition(Conditional);
					Query->SetTargetCategory(TAG_TargetCategory_Test);
					TargetSearchSubsystem->RegisterTarget(TargetA, TAG_TargetCategory_Test);
					TargetSearchSubsystem->RegisterTarget(TargetB, TAG_TargetCategory_Test);
					const SF::FTargetQueryResult Result = TargetSearchSubsystem->RunQuery(Instigator, Query);
					TestTrue("Result.IsValid()", Result.IsValid());
					TestEqual("Result.GetInstigator()", Result.GetInstigator(), Cast<UObject>(Instigator));
					TestEqual("Result.GetQuery()", Result.GetQuery(), Cast<SF::UTargetQueryDataAsset>(Query));
					TestTrue("Result.HasBestCandidate()", Result.HasBestCandidate());
					TestEqual("Result.GetCandidateResults().Num()", Result.GetCandidateResults().Num(), 2);
					TestEqual("Result.GetBestCandidateResult()", Result.GetBestCandidateResult().GetCandidate(), Cast<UObject>(TargetB));
					TestTrue("Result.GetBestCandidateResult().IsBest()", Result.GetBestCandidateResult().IsBest());
					TestTrue("Result.GetBestCandidateResult().GetAssessment().GetBinaryAnswer()", Result.GetBestCandidateResult().GetAssessment().GetBinaryAnswer());
					TestEqual("Result.GetBestCandidateResult().GetAssessment().GetFuzzyAnswer()", Result.GetBestCandidateResult().GetAssessment().GetFuzzyAnswer(), 0.7f);
				});
			});
			Describe("with target A yielding [false, 0,5f] and target B yielding [false, 0,7f] answer", [this]
			{
				It("should yield a valid result saying no target was found", [this]
				{
					Conditional->AnswerTargetA = SF::Conditional::Answer::Create(false, 0.5f);
					Conditional->AnswerTargetB = SF::Conditional::Answer::Create(false, 0.7f);
					Query->SetSearchCondition(Conditional);
					Query->SetTargetCategory(TAG_TargetCategory_Test);
					TargetSearchSubsystem->RegisterTarget(TargetA, TAG_TargetCategory_Test);
					TargetSearchSubsystem->RegisterTarget(TargetB, TAG_TargetCategory_Test);
					const SF::FTargetQueryResult Result = TargetSearchSubsystem->RunQuery(Instigator, Query);
					TestTrue("Result.IsValid()", Result.IsValid());
					TestEqual("Result.GetInstigator()", Result.GetInstigator(), Cast<UObject>(Instigator));
					TestEqual("Result.GetQuery()", Result.GetQuery(), Cast<SF::UTargetQueryDataAsset>(Query));
					TestFalse("Result.HasBestCandidate()", Result.HasBestCandidate());
					TestEqual("Result.GetCandidateResults().Num()", Result.GetCandidateResults().Num(), 2);
				});
			});
		});
	});
	Describe("registering a target", [this]
	{
		Describe("not at all", [this]
		{
			It("should yield the target is not registered", [this]
			{
				TestFalse("IsTargetRegistered()", TargetSearchSubsystem->IsTargetRegistered(TargetA, TAG_TargetCategory_Test));
			});
		});
		Describe("with null target", [this]
		{
			It("should yield the target is not registered and log an error", [this]
			{
				AddExpectedError("SF::UTargetSearchSubsystem::RegisterTarget called with invalid PotentialTarget parameter!");
				TargetSearchSubsystem->RegisterTarget(nullptr, TAG_TargetCategory_Test);
				TestFalse("IsTargetRegistered()", TargetSearchSubsystem->IsTargetRegistered(TargetA, TAG_TargetCategory_Test));
			});
		});
		Describe("with invalid target category", [this]
		{
			It("should yield the target is not registered and log an error", [this]
			{
				AddExpectedError("SF::UTargetSearchSubsystem::RegisterTarget called with TargetCategory parameter not being a sub-tag of 'TargetCategory'!");
				TargetSearchSubsystem->RegisterTarget(TargetA, TAG_TargetCategory);
				AddExpectedError("SF::UTargetSearchSubsystem::IsTargetRegistered called with TargetCategory parameter not being a sub-tag of 'TargetCategory'!");
				TestFalse("IsTargetRegistered()", TargetSearchSubsystem->IsTargetRegistered(TargetA, TAG_TargetCategory));
			});
		});
		Describe("with valid valid target and category", [this]
		{
			It("should yield the target is registered under the category used during registration", [this]
			{
				TargetSearchSubsystem->RegisterTarget(TargetA, TAG_TargetCategory_Test);
				TestTrue("IsTargetRegistered()", TargetSearchSubsystem->IsTargetRegistered(TargetA, TAG_TargetCategory_Test));
			});
			It("should yield the target is not registered under any other category", [this]
			{
				TargetSearchSubsystem->RegisterTarget(TargetA, TAG_TargetCategory_Test);
				TestFalse("IsTargetRegistered()", TargetSearchSubsystem->IsTargetRegistered(TargetA, TAG_TargetCategory_Default));
			});
		});
	});
}

#endif WITH_AUTOMATION_WORKER