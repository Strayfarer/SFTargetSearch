// Copyright Strayfarer & Contributors. Released under the MIT license.


#include "SFTargetSearchSubsystem.h"

#include "SFConditional.h"
#include "SFTargetQueryDataAsset.h"
#include "SFTargetSearchLog.h"
#include "SFTargetSearchSettings.h"

USFTargetSearchSubsystem* USFTargetSearchSubsystem::Get(const UObject& WorldContext)
{
	checkf(WorldContext.GetWorld(), TEXT("%hs expects world context to have valid world!"), __FUNCTION__)
	return WorldContext.GetWorld()->GetSubsystem<USFTargetSearchSubsystem>();
}

bool USFTargetSearchSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer) 
		&& Outer->HasAnyFlags(RF_WasLoaded) // do not create subsystem for the dummy world that gets initialized on engine startup
		&& GetDefault<USFTargetSearchSettings>()->ShouldSystemStartInWorld(*Outer->GetWorld());
}

FSFTargetQueryResult USFTargetSearchSubsystem::RunQuery(UObject* Instigator, USFTargetQueryDataAsset* Query)
{
	if (!IsValid(Query))
	{
		UE_LOG(LogSFTargetSearch, Error, TEXT("%hs: called with invalid query!"), __FUNCTION__)
		return FSFTargetQueryResult();
	}
	
	LastInstigator = Instigator;
	LastQuery = Query;
	
	// prepare relevant candidates, early return if there are none
	
	TArray<UObject*> RelevantCandidates;
	if (!TryRetrieveRelevantCandidates(Instigator, Query, OUT RelevantCandidates))
	{
		UE_LOG(LogSFTargetSearch, VeryVerbose, TEXT("%hs: no registered targets for category %s for query %s, returning no best candidate."), 
			__FUNCTION__, *Query->GetTargetCategory().ToString(), *Query->GetName())
		return { .Instigator = Instigator, .Query = Query, .bHasBestCandidate = false};
	}

	// test each relevant candidate against the search condition and find the best
	
	FSFTargetQueryResult Result;
	Result.Instigator = Instigator;
	Result.Query = Query;
	
	FSFTargetCandidateQueryResult* BestEntry = nullptr;
	float BestEntryAssessment = TNumericLimits<float>::Min();
	
	for (UObject* ObjectToTest : RelevantCandidates)
	{
		if (!IsValid(ObjectToTest))
		{
			UE_LOG(LogSFTargetSearch, Error, TEXT("%hs: There's a registered candidate which is not valid anymore! "
				"Make sure you deregister targets correctly."), __FUNCTION__)
			continue;
		}
		
		FSFTargetCandidateQueryResult& ResultEntry = Result.CandidateResults.AddDefaulted_GetRef();
		ResultEntry.Assessment = Query->GetSearchCondition()->Evaluate({ ObjectToTest, Instigator, &ResultEntry.DebugTrace });
		ResultEntry.Candidate = ObjectToTest;

		if (ResultEntry.Assessment.GetBinaryAnswer())
		{
			if (!Result.bHasBestCandidate || ResultEntry.Assessment.GetFuzzyAnswer() > BestEntryAssessment)
			{
				BestEntry = &ResultEntry;
				BestEntryAssessment = ResultEntry.Assessment.GetFuzzyAnswer();
			}
		}
	}

	if (BestEntry)
	{
		BestEntry->bIsBest = true;
		Result.BestCandidateResult = *BestEntry;
		Result.bHasBestCandidate = true;
	}

	CacheQueryResult(Result);
	return Result;
}

void USFTargetSearchSubsystem::RegisterTarget(UObject* PotentialTarget, const FGameplayTag& TargetCategory)
{
	FSFTargetArray* TargetsForCategory = RegisteredTargetsByCategory.Find(TargetCategory);
	if (!TargetsForCategory)
	{
		TargetsForCategory = &RegisteredTargetsByCategory.Add(TargetCategory, {});
	}
	TargetsForCategory->Targets.AddUnique(PotentialTarget);
}

void USFTargetSearchSubsystem::DeregisterTarget(UObject* PotentialTarget, const FGameplayTag& TargetCategory)
{
	if (FSFTargetArray* TargetsForQueryCategory = RegisteredTargetsByCategory.Find(TargetCategory))
	{
		TargetsForQueryCategory->Targets.Remove(PotentialTarget);
	}
}

bool USFTargetSearchSubsystem::IsTargetRegistered(UObject* Target, const FGameplayTag& TargetCategory)
{
	if (const FSFTargetArray* TargetsForQueryCategory = RegisteredTargetsByCategory.Find(TargetCategory))
	{
		return TargetsForQueryCategory->Targets.Contains(Target);
	}
	return false;
}

const FSFTargetQueryResult* USFTargetSearchSubsystem::FindQueryResultCache(UObject* Instigator, USFTargetQueryDataAsset* Query) const
{
	if (!IsValid(Instigator))
	{
		UE_LOG(LogSFTargetSearch, Error, TEXT("%hs expects valid instigator!"), __FUNCTION__)
		return nullptr;
	}

	const FSFQueryResultsCache* QueryResultCache = QueryResultByInstigatorCache.Find(Instigator);
	if (!QueryResultCache)
	{
		UE_LOG(LogSFTargetSearch, VeryVerbose, TEXT("%hs couldn't find query result cache for instigator %s!"), __FUNCTION__, *Instigator->GetName())
		return nullptr;
	}

	const FSFTargetQueryResult* QueryResult = QueryResultCache->Map.Find(Query);
	return QueryResult;
}

void USFTargetSearchSubsystem::CacheQueryResult(const FSFTargetQueryResult& Result)
{
	FSFQueryResultsCache* QueryResultCache = QueryResultByInstigatorCache.Find(Result.Instigator);
	if (!QueryResultCache)
	{
		QueryResultCache = &QueryResultByInstigatorCache.Add(Result.Instigator, {});
	}
	QueryResultCache->Map.Add(Result.Query, Result);
}

bool USFTargetSearchSubsystem::TryRetrieveRelevantCandidates(UObject* Instigator, const USFTargetQueryDataAsset* Query,
	TArray<UObject*>& OutCandidates)
{
	const FSFTargetArray* TargetsForQueryCategory = RegisteredTargetsByCategory.Find(Query->GetTargetCategory());
	if (!TargetsForQueryCategory)
		return false;
	
	OutCandidates = TargetsForQueryCategory->Targets;
	if (!Query->CanAcquireInstigatorAsTarget())
	{
		OutCandidates.Remove(Instigator);
	}
	
	return !OutCandidates.IsEmpty();
}
