#include "LastHitAssistView.h"
#define PI 3.14159265358979323846f

const char* LastHitAssistView::GetName() {
	return "Last Hit Assist";
}

void LastHitAssistView::OnSaveSettings(ConfigSet& configs) {
	BaseView::OnSaveSettings(configs);

	configs.Set<bool>("showSmiteLastHit", showSmiteLastHit);
	configs.Set<bool>("showMinionLastHit", showMinionLastHit);
}

void LastHitAssistView::OnLoadSettings(ConfigSet& configs) {
	BaseView::OnLoadSettings(configs);

	showSmiteLastHit = configs.Get<bool>("showSmiteLastHit", false);
	showMinionLastHit = configs.Get<bool>("showMinionLastHit", true);
}

void LastHitAssistView::DrawSettings(LeagueMemoryReader& reader, UI& ui) {
	ImGui::Checkbox("Show when to smite##showSmiteLastHit", &showSmiteLastHit);
	ImGui::Checkbox("Show when to hit minions##showMinionLastHit", &showMinionLastHit);
}

void LastHitAssistView::DrawWorldSpaceOverlay(LeagueMemoryReader& reader, ImDrawList* overlayCanvas, UI& uis) {
	
	if (showMinionLastHit) {
		for (auto it = reader.minions.begin(); it != reader.minions.end(); ++it) {
			GameObject* minion = *it;

			// Skip dead/invisible/allied creeps
			if (minion->team == reader.localChampion->team || !minion->isVisible || minion->health <= 0.f)
				continue;

			if (League::EffectiveHP(minion->health, minion->armour) - reader.localChampion->GetBasicAttackDamage() <= 0.f) {
				reader.renderer.DrawCircleAt(overlayCanvas, minion->position, 50.f, false, 15, Colors::Cyan);
			}
		}
	}
	if (showSmiteLastHit) {

		Spell* smite = reader.localChampion->GetSummonerSpell(SummonerSpellType::SMITE);
		if (smite != nullptr) {
			for (auto it = reader.jungle.begin(); it != reader.jungle.end(); ++it) {
				GameObject* mob = *it;

				// Skip plants and dead mobs
				if (mob->health <= 1.f || !mob->isVisible)
					continue;

				if (mob->health - smite->damage <= 0.f) {
					reader.renderer.DrawCircleAt(overlayCanvas, mob->position, 50.f, false, 15, Colors::Yellow);
				}
			}
		}
	}

}