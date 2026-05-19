// Copyright Strayfarer & Contributors. Released under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "ConditionalDebugTrace.h"
#include "ConditionalAnswer.h"
#include "TargetQueryResult.generated.h"

namespace SF
{
	class UTargetSearchSubsystem;
	class UTargetQueryDataAsset;

	/**
	 * Result structure containing information on how a single target candidate
	 * fared in the evaluation.
	 */
	USTRUCT(BlueprintType)
	struct FTargetCandidateQueryResult
	{
		GENERATED_BODY()

		friend UTargetSearchSubsystem;
		
		FORCEINLINE UObject* GetCandidate() const { return Candidate; }
		FORCEINLINE const FConditionalAnswer& GetAssessment() const { return Assessment; }
		FORCEINLINE bool IsBest() const { return bIsBest; }
		FORCEINLINE const FConditionalDebugTrace& GetDebugTrace() const { return DebugTrace; }
		
	private:
		/** The candidate that was evaluated. */
		UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
		TObjectPtr<UObject> Candidate = nullptr;

		/** The answer returned from evaluating with some query condition tree. */
		UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
		FConditionalAnswer Assessment = Conditional::Answer::No();

		/** Whether this candidate yielded the best assessment. */
		UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
		bool bIsBest = false;

		/** The debug trace resulting from evaluation the query condition tree. */
		UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
		FConditionalDebugTrace DebugTrace = {};
	};

	/**
	 * Result structure containing all information on a target query evaluation,
	 * including which candidate is proposed as target.
	 */
	USTRUCT(BlueprintType)
	struct FTargetQueryResult
	{
		GENERATED_BODY()
		
		friend UTargetSearchSubsystem;
		
		static FTargetQueryResult MakeWithNoCandidate(UObject* InInstigator, UTargetQueryDataAsset* InQuery)
		{
			FTargetQueryResult Result;
			Result.Instigator = InInstigator;
			Result.Query = InQuery;
			return Result;
		}
		
		FORCEINLINE UObject* GetInstigator() const { return Instigator; }
		FORCEINLINE UTargetQueryDataAsset* GetQuery() const { return Query; }
		FORCEINLINE const TArray<FTargetCandidateQueryResult>& GetCandidateResults() const { return CandidateResults; }
		FORCEINLINE bool HasBestCandidate() const { return bHasBestCandidate; }
		FORCEINLINE const FTargetCandidateQueryResult& GetBestCandidateResult() const { return BestCandidateResult; }
		
		FORCEINLINE bool IsValid() const { return !!Query; }
		
	private:
		/** Instigator of the query. */
		UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
		TObjectPtr<UObject> Instigator = nullptr;

		/** The query which was evaluated. */
		UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
		TObjectPtr<UTargetQueryDataAsset> Query = nullptr;

		/** List of evaluation results per target candidate. */
		UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
		TArray<FTargetCandidateQueryResult> CandidateResults;

		/** Whether there was any candidate for which the query evaluation yielded a true binary answer. */
		UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
		bool bHasBestCandidate = false;

		/** If there was a best candidate (see bHasBestEntry), this is it. */
		UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
		FTargetCandidateQueryResult BestCandidateResult;
	};
}
