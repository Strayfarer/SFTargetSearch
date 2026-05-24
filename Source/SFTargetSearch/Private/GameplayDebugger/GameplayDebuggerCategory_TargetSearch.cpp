// Copyright Strayfarer & Contributors. Released under the MIT license.

#if WITH_GAMEPLAY_DEBUGGER

#include "GameplayDebuggerCategory_TargetSearch.h"

#include "Engine/Canvas.h"
#include "TargetQueryDataAsset.h"
#include "TargetSearchSubsystem.h"

SF::FGameplayDebuggerCategory_TargetSearch::FGameplayDebuggerCategory_TargetSearch()
{
	bShowCategoryName = true;
	bShowOnlyWithDebugActor = false;

	BindKeyPress(
		EKeys::Q.GetFName(),
		FGameplayDebuggerInputModifier::Alt,
		this,
		&FGameplayDebuggerCategory_TargetSearch::Input_PreviousInstigator,
		EGameplayDebuggerInputMode::Local);
	BindKeyPress(
		EKeys::E.GetFName(),
		FGameplayDebuggerInputModifier::Alt,
		this,
		&FGameplayDebuggerCategory_TargetSearch::Input_NextInstigator,
		EGameplayDebuggerInputMode::Local);

	BindKeyPress(
		EKeys::A.GetFName(),
		FGameplayDebuggerInputModifier::Alt,
		this,
		&FGameplayDebuggerCategory_TargetSearch::Input_PreviousQuery,
		EGameplayDebuggerInputMode::Local);
	BindKeyPress(
		EKeys::D.GetFName(),
		FGameplayDebuggerInputModifier::Alt,
		this,
		&FGameplayDebuggerCategory_TargetSearch::Input_NextQuery,
		EGameplayDebuggerInputMode::Local);
	
	BindKeyPress(
		EKeys::W.GetFName(),
		FGameplayDebuggerInputModifier::Alt,
		this,
		&FGameplayDebuggerCategory_TargetSearch::Input_LastInstigatorAndQuery,
		EGameplayDebuggerInputMode::Local);
	
	BindKeyPress(
		EKeys::X.GetFName(),
		FGameplayDebuggerInputModifier::Alt,
		this,
		&FGameplayDebuggerCategory_TargetSearch::Input_ToggleShowConditionalDebugTrace,
		EGameplayDebuggerInputMode::Local);
}

void SF::FGameplayDebuggerCategory_TargetSearch::DrawData(APlayerController* OwnerPC,
	FGameplayDebuggerCanvasContext& CanvasContext)
{
	FGameplayDebuggerCategory::DrawData(OwnerPC, CanvasContext);
	CachedOwnerPc = OwnerPC;

	const UTargetSearchSubsystem* TargetSearchSubsystem = FindTargetSearchSubsystem();
	if (!TargetSearchSubsystem)
	{
		CanvasContext.PrintAt(10.f, 90.f, "No target search subsystem available for this map!");
		return;
	}

	if (!SelectedInstigator.IsValid() || !SelectedQuery.IsValid())
	{
		Input_LastInstigatorAndQuery();
	}

	DrawControls(CanvasContext);
	DrawSelectableInstigators(CanvasContext);
	DrawSelectableQueries(CanvasContext);
	
	if (!SelectedInstigator.IsValid() || !SelectedQuery.IsValid())
		return;

	// retrieve cached query result
	const FTargetQueryResult* QueryResult = TargetSearchSubsystem->FindQueryResultCache(SelectedInstigator.Get(), SelectedQuery.Get());
	if (!QueryResult)
	{
		CanvasContext.PrintAt(10.f, 230.f, "Failed to find cached query result for instigator x query combination.");
		return;
	}
	
	// draw predicate visualizations
	SelectedQuery->GetSearchCondition()->ForThisAndEachChildDo([this, &CanvasContext](UConditional* Predicate)
	{
		Predicate->VisualizeWithGameplayDebugger(*this, CanvasContext);
	});

	// draw target visualizations
	TArray<FTargetCandidateQueryResult> CandidateQueryResults = QueryResult->GetCandidateResults();
	Algo::Sort(CandidateQueryResults, [](const FTargetCandidateQueryResult& A, const FTargetCandidateQueryResult& B)
	{
		if (A.GetAssessment().GetBinaryAnswer() && !B.GetAssessment().GetBinaryAnswer())
			return false;
		
		if (!A.GetAssessment().GetBinaryAnswer() && B.GetAssessment().GetBinaryAnswer())
			return true;
		
		return A.GetAssessment().GetFuzzyAnswer() < B.GetAssessment().GetFuzzyAnswer();
	});
	for (const FTargetCandidateQueryResult& ResultEntry : CandidateQueryResults)
	{
		const AActor* CandidateAsActor = Cast<AActor>(ResultEntry.GetCandidate());
		const USceneComponent* CandidateAsSceneComponent = Cast<USceneComponent>(ResultEntry.GetCandidate());
		if (!CandidateAsActor && !CandidateAsSceneComponent)
			continue;
		
		const FVector Pos3D = CandidateAsActor
			? CandidateAsActor->GetActorLocation()
			: CandidateAsSceneComponent->GetComponentLocation();
		const FVector2D Pos2D = CanvasContext.ProjectLocation(Pos3D);

		// draw summary box to the left
		const FColor Color = ResultEntry.IsBest()
			? FColor::Green
			: ResultEntry.GetAssessment().GetBinaryAnswer()
				? FColor::Yellow
				: FColor(128);
		const FString SanitizedScore = FString::SanitizeFloat(FMath::TruncToFloat(ResultEntry.GetAssessment().GetFuzzyAnswer() * 100) / 100, 2);
		CanvasContext.Canvas->K2_DrawBox(FVector2D(Pos2D.X - 50.f, Pos2D.Y), FVector2D(41.f, 21.f), 1.f, Color);
		CanvasContext.PrintAt(Pos2D.X - 41.f, Pos2D.Y + 3.f, Color, SanitizedScore);

		if (bShowConditionalDebugTrace)
		{
			// draw debug trace to the right
			CanvasContext.PrintAt(Pos2D.X, Pos2D.Y, ResultEntry.GetDebugTrace().ToString());
		}
	}
}

void SF::FGameplayDebuggerCategory_TargetSearch::DrawControls(FGameplayDebuggerCanvasContext& CanvasContext)
{
	CanvasContext.PrintAt(10.f, 110.f, "{white}({cyan}ALT + Q/E{white})");
	CanvasContext.PrintAt(100.f, 110.f, "{white} Previous/Next Instigator");
	CanvasContext.PrintAt(10.f, 130.f, "{white}({cyan}ALT + A/D{white})");
	CanvasContext.PrintAt(100.f, 130.f, "{white} Previous/Next Query");
	CanvasContext.PrintAt(10.f, 150.f, "{white}({cyan}ALT + W{white})");
	CanvasContext.PrintAt(100.f, 150.f, "{white} Select Last Run Instigator+Query");
	CanvasContext.PrintAt(10.f, 170.f, "{white}({cyan}ALT + X{white})");
	CanvasContext.PrintAt(100.f, 170.f, "{white} Show Target Details");
}

void SF::FGameplayDebuggerCategory_TargetSearch::DrawSelectableInstigators(FGameplayDebuggerCanvasContext& CanvasContext) const
{
	CanvasContext.PrintAt(10.f, 210.f, "Instigator:");
	
	TArray<TObjectPtr<UObject>> Instigators = {};
	TryGetQueryCacheFromTargetService().GenerateKeyArray(Instigators);
	if (Instigators.IsEmpty())
	{
		CanvasContext.PrintAt(73.f, 210.f, "No instigators available.");
		return;
	}
	
	FString SelectableInstigatorsString = "";
	for (const UObject* SelectableInstigator : Instigators)
	{
		if (!IsValid(SelectableInstigator))
			continue;
		
		if (SelectableInstigator == SelectedInstigator)
		{
			SelectableInstigatorsString += TEXT("{green}") + SelectableInstigator->GetName() + TEXT(", ");
		}
		else
		{
			SelectableInstigatorsString += TEXT("{white}") + SelectableInstigator->GetName() + TEXT(", ");
		}
	}
	SelectableInstigatorsString.RemoveFromEnd(", ");
	CanvasContext.PrintAt(73.f, 210.f, SelectableInstigatorsString);
}

void SF::FGameplayDebuggerCategory_TargetSearch::DrawSelectableQueries(FGameplayDebuggerCanvasContext& CanvasContext) const
{
	CanvasContext.PrintAt(10.f, 230.f, "Query:");
	
	if (!SelectedInstigator.IsValid())
	{
		CanvasContext.PrintAt(73.f, 230.f, "No instigator selected.");
		return;
	}
	
	auto* ByQueryCache = TryGetQueryCacheFromTargetService().Find(SelectedInstigator.Get());
	if (!ByQueryCache)
	{
		CanvasContext.PrintAt(73.f, 230.f, "No queries known of.");
		return;
	}

	TArray<TObjectPtr<UTargetQueryDataAsset>> Queries = {};
	ByQueryCache->Map.GenerateKeyArray(Queries);
	if (Queries.IsEmpty())
	{
		CanvasContext.PrintAt(73.f, 230.f, "No queries known of.");
		return;
	}
	
	FString SelectableQueriesString = "";
	for (const UObject* SelectableQuery : Queries)
	{
		if (!IsValid(SelectableQuery))
			continue;
		
		if (SelectableQuery == SelectedQuery)
		{
			SelectableQueriesString += TEXT("{green}") + SelectableQuery->GetName() + TEXT(", ");
		}
		else
		{
			SelectableQueriesString += TEXT("{white}") + SelectableQuery->GetName() + TEXT(", ");
		}
	}
	SelectableQueriesString.RemoveFromEnd(", ");
	CanvasContext.PrintAt(73.f, 230.f, *SelectableQueriesString);
}

void SF::FGameplayDebuggerCategory_TargetSearch::Input_NextInstigator()
{
	auto& QueryResultsCache = TryGetQueryCacheFromTargetService();
	SelectedInstigatorIndex++;
	if (SelectedInstigatorIndex == QueryResultsCache.Num())
	{
		SelectedInstigatorIndex = 0;
	}
	SelectedQueryIndex = 0;
	UpdateSelectionFromIndices();
}

void SF::FGameplayDebuggerCategory_TargetSearch::Input_PreviousInstigator()
{
	auto& QueryResultsCache = TryGetQueryCacheFromTargetService();
	SelectedInstigatorIndex--;
	if (SelectedInstigatorIndex == -1)
	{
		SelectedInstigatorIndex = QueryResultsCache.Num() - 1;
	}
	SelectedQueryIndex = 0;
	UpdateSelectionFromIndices();
}

void SF::FGameplayDebuggerCategory_TargetSearch::Input_NextQuery()
{
	SelectedQueryIndex++;
	if (SelectedQueryIndex == GetCachedQueryResultNumForInstigator())
	{
		SelectedQueryIndex = 0;
	}
	UpdateSelectionFromIndices();
}

void SF::FGameplayDebuggerCategory_TargetSearch::Input_PreviousQuery()
{
	SelectedQueryIndex--;
	if (SelectedQueryIndex == -1)
	{
		SelectedQueryIndex = GetCachedQueryResultNumForInstigator() - 1;
	}
	UpdateSelectionFromIndices();
}

void SF::FGameplayDebuggerCategory_TargetSearch::Input_LastInstigatorAndQuery()
{
	if (const auto TargetSearchSubsystem = FindTargetSearchSubsystem())
	{
		SelectedInstigator = TargetSearchSubsystem->GetLastInstigator();
		SelectedQuery = TargetSearchSubsystem->GetLastQuery();
		
		if (!SelectedInstigator.IsValid())
			return;
			
		auto& ByInstigatorCache = TryGetQueryCacheFromTargetService();
	
		TArray<TObjectPtr<UObject>> Instigators = {};
		ByInstigatorCache.GenerateKeyArray(Instigators);
		SelectedInstigatorIndex = Instigators.IndexOfByKey(SelectedInstigator);
		
		if (!SelectedQuery.IsValid())
			return;

		const FQueryResultsCache* QueryResultsCache = ByInstigatorCache.Find(SelectedInstigator.Get());
		if (!QueryResultsCache)
			return;
		
		TArray<TObjectPtr<UTargetQueryDataAsset>> Queries = {};
		QueryResultsCache->Map.GenerateKeyArray(Queries);
		SelectedQueryIndex = Queries.IndexOfByKey(SelectedQuery);
	}
}

void SF::FGameplayDebuggerCategory_TargetSearch::Input_ToggleShowConditionalDebugTrace()
{
	bShowConditionalDebugTrace = !bShowConditionalDebugTrace;
}

const TMap<TObjectPtr<UObject>, SF::FQueryResultsCache>& SF::FGameplayDebuggerCategory_TargetSearch::TryGetQueryCacheFromTargetService() const
{
	if (const auto TargetSearchSubsystem = FindTargetSearchSubsystem())
	{
		return TargetSearchSubsystem->GetQueryResultByInstigatorCache();
	}
	static TMap<TObjectPtr<UObject>, FQueryResultsCache> EmptyMap;
	return EmptyMap;
}

uint32 SF::FGameplayDebuggerCategory_TargetSearch::GetCachedQueryResultNumForInstigator() const
{
	auto& ByInstigatorCache = TryGetQueryCacheFromTargetService();
	if (SelectedInstigatorIndex < 0 || ByInstigatorCache.Num()-1 < SelectedInstigatorIndex)
		return 0;
	
	TArray<TObjectPtr<UObject>> Instigators = {};
	ByInstigatorCache.GenerateKeyArray(Instigators);

	return ByInstigatorCache[Instigators[SelectedInstigatorIndex]].Map.Num();
}

void SF::FGameplayDebuggerCategory_TargetSearch::UpdateSelectionFromIndices()
{
	auto& ByInstigatorCache = TryGetQueryCacheFromTargetService();
	if (SelectedInstigatorIndex < 0 || ByInstigatorCache.Num()-1 < SelectedInstigatorIndex)
		return;
	
	TArray<TObjectPtr<UObject>> Instigators = {};
	ByInstigatorCache.GenerateKeyArray(Instigators);
	SelectedInstigator = Instigators[SelectedInstigatorIndex];
	
	auto& ByQueryCache = ByInstigatorCache[SelectedInstigator.Get()].Map;
	if (SelectedQueryIndex < 0 || ByQueryCache.Num()-1 < SelectedQueryIndex)
		return;
	
	TArray<TObjectPtr<UTargetQueryDataAsset>> Queries = {};
	ByQueryCache.GenerateKeyArray(Queries);
	SelectedQuery = Queries[SelectedQueryIndex];
}

SF::UTargetSearchSubsystem* SF::FGameplayDebuggerCategory_TargetSearch::FindTargetSearchSubsystem() const
{
	return CachedOwnerPc.IsValid() ? UTargetSearchSubsystem::Get(*CachedOwnerPc.Get()) : nullptr;
}

#endif
