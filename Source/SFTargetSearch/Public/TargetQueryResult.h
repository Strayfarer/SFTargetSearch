// Copyright Strayfarer & Contributors. Released under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "SFConditionalDebugTrace.h"
#include "SFConditionalAnswer.h"
#include "TargetQueryResult.generated.h"

namespace SF
{
	class UTargetQueryDataAsset;

	/**
	 * Result structure containing information on how a single target candidate
	 * fared in the evaluation.
	 */
	USTRUCT(BlueprintType)
	struct FTargetCandidateQueryResult
	{
		GENERATED_BODY()

		/** The candidate that was evaluated. */
		UPROPERTY(BlueprintReadOnly)
		TObjectPtr<UObject> Candidate = nullptr;

		/** The answer returned from evaluating with some query condition tree. */
		UPROPERTY(BlueprintReadOnly)
		FSFConditionalAnswer Assessment = Conditional::Answer::No();

		/** Whether this candidate yielded the best assessment. */
		UPROPERTY(BlueprintReadOnly)
		bool bIsBest = false;

		/** The debug trace resulting from evaluation the query condition tree. */
		UPROPERTY(BlueprintReadOnly)
		FSFConditionalDebugTrace DebugTrace = {};
	};

	/**
	 * Result structure containing all information on a target query evaluation,
	 * including which candidate is proposed as target.
	 */
	USTRUCT(BlueprintType)
	struct FTargetQueryResult
	{
		GENERATED_BODY()

		/** Instigator of the query. */
		UPROPERTY(BlueprintReadOnly)
		TObjectPtr<UObject> Instigator = nullptr;

		/** The query which was evaluated. */
		UPROPERTY(BlueprintReadOnly)
		TObjectPtr<UTargetQueryDataAsset> Query = nullptr;

		/** List of evaluation results per target candidate. */
		UPROPERTY(BlueprintReadOnly)
		TArray<FTargetCandidateQueryResult> CandidateResults;

		/** Whether there was any candidate for which the query evaluation yielded a true binary answer. */
		UPROPERTY(BlueprintReadOnly)
		bool bHasBestCandidate = false;

		/** If there was a best candidate (see bHasBestEntry), this is it. */
		UPROPERTY(BlueprintReadOnly)
		FTargetCandidateQueryResult BestCandidateResult;
	};
}
