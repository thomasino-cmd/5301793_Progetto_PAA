#include "EndGameWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "AWGameMode.h"
#include "Kismet/GameplayStatics.h"

void UEndGameWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind del click del bottone
    if (PlayAgainButton)
    {
        PlayAgainButton->OnClicked.AddDynamic(this, &UEndGameWidget::OnPlayAgainClicked);
    }
}

void UEndGameWidget::SetWinnerText(int32 WinningPlayerId)
{
    if (WinnerText)
    {
        FString WinnerString = FString::Printf(TEXT("Player %d Wins!"), WinningPlayerId + 1);
        WinnerText->SetText(FText::FromString(WinnerString));

        // Imposta il colore in base al giocatore vincitore
        FSlateColor TextColor;
        if (WinningPlayerId == 0) // Giocatore umano (Player 1)
        {
            TextColor = FSlateColor(FLinearColor(0.0f, 0.35f, 1.0f, 1.0f)); // Blu
        }
        else // AI (Player 2)
        {
            TextColor = FSlateColor(FLinearColor(1.0f, 0.2f, 0.2f, 1.0f)); // Rosso
        }

        WinnerText->SetColorAndOpacity(TextColor);
    }
}

void UEndGameWidget::OnPlayAgainClicked()
{
    // Ottieni la GameMode e chiama RestartGame
    if (AAWGameMode* GameMode = Cast<AAWGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        GameMode->Reset();
        GameMode->RestartGame();
    }

    // Rimuovi questo widget dalla vista
    RemoveFromParent();
}