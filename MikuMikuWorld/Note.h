#pragma once
#include "NoteTypes.h"
#include <string>
#include <vector>
#include <stdexcept>

namespace MikuMikuWorld
{
	constexpr int MIN_NOTE_WIDTH = 1;
	constexpr int MAX_NOTE_WIDTH = 12;
	constexpr int MIN_LANE = 0;
	constexpr int MAX_LANE = 11;
	constexpr int NUM_LANES = 12;

	constexpr const char* SE_PERFECT = "perfect";
	constexpr const char* SE_FLICK = "flick";
	constexpr const char* SE_TICK = "tick";
	constexpr const char* SE_FRICTION = "friction";
	constexpr const char* SE_CONNECT = "connect";
	constexpr const char* SE_CRITICAL_TAP = "critical_tap";
	constexpr const char* SE_CRITICAL_FLICK = "critical_flick";
	constexpr const char* SE_CRITICAL_TICK = "critical_tick";
	constexpr const char* SE_CRITICAL_FRICTION = "critical_friction";
	constexpr const char* SE_CRITICAL_CONNECT = "critical_connect";

	constexpr const char* SE_NAMES[] = { SE_PERFECT,         SE_FLICK,         SE_TICK,
		                                 SE_FRICTION,        SE_CONNECT,       SE_CRITICAL_TAP,
		                                 SE_CRITICAL_FLICK,  SE_CRITICAL_TICK, SE_CRITICAL_FRICTION,
		                                 SE_CRITICAL_CONNECT };

	constexpr float flickArrowWidths[] = { 0.95f, 1.25f, 1.8f, 2.3f, 2.6f, 3.2f };

	constexpr float flickArrowHeights[] = { 1, 1.05f, 1.2f, 1.4f, 1.5f, 1.6f };

	enum class ZIndex : int32_t
	{
		HoldLine,
		Guide,
		HoldTick,
		Note,
		FrictionTick,
		zCount
	};

	struct NoteTextures
	{
		int notes;
		int holdPath;
		int touchLine;
		int ccNotes;
		int guideColors;
	};

	extern NoteTextures noteTextures;
	extern int nextID;

	class Note
	{
	  private:
		NoteType type;

	  public:
		int ID;
		int parentID;
		int tick;
		float lane;
		float width;
		bool critical{ false };
		bool friction{ false };
		FlickType flick{ FlickType::None };

		int layer{ 0 };

		explicit Note(NoteType _type);
		explicit Note(NoteType _type, int tick, float lane, float width);
		Note();

		constexpr NoteType getType() const { return type; }
		constexpr bool isHold() const
		{
			return type == NoteType::Hold || type == NoteType::HoldMid || type == NoteType::HoldEnd;
		}

		bool isFlick() const;
		bool hasEase() const;
		bool canFlick() const;
		bool canTrace() const;
	};

	struct HoldStep
	{
		int ID;
		HoldStepType type;
		EaseType ease;
	};

	class HoldNote
	{
	  public:
		HoldStep start;
		std::vector<HoldStep> steps;
		int end;

		HoldNoteType startType{};
		HoldNoteType endType{};

		FadeType fadeType{ FadeType::Out };
		GuideColor guideColor{ GuideColor::Green };

		constexpr bool isGuide() const
		{
			return startType == HoldNoteType::Guide || endType == HoldNoteType::Guide;
		}

		/**
		 * @brief Retrieve HoldStep according to given `index` within `[-1, steps.size()-1]`,
		 *        where -1 stands for the start step
		 * @throw `std::out_of_range` if `index` is invalid
		 * @warning Reference returned by this method can be invalidated by vector reallocation
		 */
		HoldStep& operator[](int index) {
			if (index < -1 || index >= (int)steps.size())
				throw std::out_of_range("Index out of range in HoldNote[]");
			return index == -1 ? start : steps[index];
		}
		/**
		 * @brief Retrieve HoldStep according to given `index` within `[-1, steps.size()-1]`,
		 *        where -1 stands for the start step
		 * @throw `std::out_of_range` if `index` is invalid
		 * @warning Reference returned by this method can be invalidated by vector reallocation
		 */
		const HoldStep& operator[](int index) const {
			if (index < -1 || index >= (int)steps.size())
				throw std::out_of_range("Index out of range in HoldNote[]");
			return index == -1 ? start : steps[index];
		}
		/**
		 * @brief Retrieve note ID according to given `index` within `[-1, steps.size()]`,
		 *        where -1 stands for the start note and `steps.size()` stands for the end note
		 * @throw `std::out_of_range` if `index` is invalid
		 */
		int id_at(int index) const {
			if (index < -1 || index > (int)steps.size())
				throw std::out_of_range("Index out of range in HoldNote::id_at");
			return (index == steps.size()) ? end : (index == -1 ? start.ID : steps[index].ID);
		}
	};

	struct Score;

	void resetNextID();

	void cycleFlick(Note& note);
	void cycleStepEase(HoldStep& note);
	void cycleStepType(HoldStep& note);
	void sortHoldSteps(const Score& score, HoldNote& note);
	int findHoldStep(const HoldNote& note, int stepID);

	int getFlickArrowSpriteIndex(const Note& note);
	int getNoteSpriteIndex(const Note& note);
	int getCcNoteSpriteIndex(const Note& note);
	int getFrictionSpriteIndex(const Note& note);
	std::string_view getNoteSE(const Note& note, const Score& score);
}
