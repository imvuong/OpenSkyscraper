# OpenSkyscraper: Plan for Missing Features

This document lists all features missing or incomplete relative to the original SimTower (1994) and proposes an implementation plan. Items are grouped by system and ordered by dependency where applicable.

---

## 1. Star Rating System (Complete Progression)

**Current state:** Only 0→1 is implemented (population ≥ 300). Rating 1→2 shows "Your tower needs security." but does not upgrade; ratings 2–5 and Tower are absent.

**Target behavior (SimTower):**

| Rating | Requirements |
|--------|--------------|
| 1 Star | Starting (or e.g. 300 population in some variants) |
| 2 Stars | 300 population |
| 3 Stars | 1,000 population + 1 Security Office |
| 4 Stars | 5,000 population + 2+ Hotel Suites + 1 favorable VIP visit + recycling & medical demands met |
| 5 Stars | 10,000 population + 1 Metro Station |
| Tower | 15,000 population + Cathedral at top + all above-ground levels developed |

**Tasks:**

1. **1.1** Define a single source of truth for rating thresholds (config or constants) so requirements can be tuned without code churn.
2. **1.2** Implement rating 1→2: when population ≥ 300, require at least one Security item before calling `setRating(2)`; remove the TODO in `Game::ratingMayIncrease()` and add the check.
3. **1.3** Implement 2→3: population ≥ 1,000 and at least one Security office (count `itemsByType["security"]` or equivalent).
4. **1.4** Implement 3→4: population ≥ 5,000, at least two Hotel Suite items, one "favorable VIP" flag (see §3), and recycling + medical availability (see 1.5).
5. **1.5** Add "demand" or "satisfaction" flags for recycling and medical: e.g. tower has Recycling and Medical Center, and optionally some simple satisfaction logic (can be stub at first).
6. **1.6** Implement 4→5: population ≥ 10,000 and at least one Metro (already unique; `metroStation != NULL`).
7. **1.7** Implement 5→Tower: population ≥ 15,000, one Cathedral built (see §2), and "all above-ground levels developed" (define rule: e.g. no empty floor slots between ground and top, or all floors built to a minimum width).
8. **1.8** Ensure UI shows all six states: stars 1–5 plus Tower (assets exist: `simtower/ui/time/rating` and `rating/tower`). Handle Tower in `TimeWindow` and any rating-dependent UI.

**Files:** `source/Game.cpp` (ratingMayIncrease, setRating), `source/TimeWindow.cpp` (rating display), optionally new `source/RatingRules.h/.cpp` or data file.

---

## 2. Facility Unlocking by Rating

**Current state:** All tools are shown and buildable from the start (`ToolboxWindow` displays every `itemFactory` prototype).

**Target behavior:** Facilities become available only when the tower reaches the required star level (SimTower-style):

- **1 Star:** Lobby, Skylobby, Floor, Stairs, Standard Elevator, Office, Fast Food, Condo  
- **2 Stars:** Service Elevator, Hotel Single, Security, Housekeeping  
- **3 Stars:** Escalator, Express Elevator, Restaurant, Shop, Cinema, Party Hall, Hotel Double, Hotel Suite, Medical Center, Recycling, Parking Space, Parking Ramp  
- **4 Stars:** Metro Station  
- **5 Stars:** Cathedral  

**Tasks:**

2. **2.1** Add a "minimum rating" (or "unlock at star") field to item prototypes (e.g. in `Prototype.h` and each `initPrototype`).
2. **2.2** In `ToolboxWindow::reload()` (or when building the toolbox), hide or disable buttons for prototypes whose `minRating > game->rating`. Optionally show locked tools grayed out with a tooltip "Unlocks at X stars".
2. **2.3** In `Game::handleEvent` (or wherever placement is validated), reject placement of a prototype whose rating requirement is not met (prevents save-game editing from bypassing).
2. **2.4** When loading a save, allow items that are above current rating (already built) but do not allow building new ones until rating is reached.

**Files:** `source/Item/Prototype.h`, all `source/Item/*.h` with `OT_ITEM_PROTOTYPE`, `source/Item/Factory.cpp`, `source/ToolboxWindow.cpp`, `source/Game.cpp` (placement validation).

---

## 3. Cathedral and "Tower" Goal

**Current state:** No cathedral facility; Tower status is not implemented.

**Tasks:**

3. **3.1** Add a new item type `Cathedral` (or reuse a generic "special" type). Art may need to be sourced from SimTower assets or a placeholder.
3. **3.2** Cathedral is buildable only at 5 stars (see §2), and only on the top floor (or top N floors) of the tower. Define "top" (e.g. highest floor that has any built content).
3. **3.3** Only one Cathedral per tower; building it contributes to the Tower condition (see 1.7).
3. **3.4** "All above-ground levels developed" for Tower: implement a rule (e.g. every floor from 1 to top has at least one floor tile or no gap), and call it from `ratingMayIncrease()` when at 5 stars and cathedral exists.

**Files:** New `source/Item/Cathedral.h`, `source/Item/Cathedral.cpp`, `source/Item/Factory.cpp`, `source/Game.cpp` (rating + placement rules), `SimTowerLoader.cpp` if new assets are needed.

---

## 4. Building and Elevator Limits

**Current state:** B9 and lobby-every-15 are enforced; standard elevator span is capped at 31 floors. No max above-ground floors; no elevator shaft/car limits.

**Tasks:**

4. **4.1** Enforce maximum **100 floors above ground** (floor index 1..100): when extending or placing, reject if resulting highest floor > 100.
4. **4.2** (Optional) Enforce **max 9 floors below ground** (e.g. floor indices -9..-1) if not already strict.
4. **4.3** Enforce **max 24 elevator shafts** (count items in `itemsByType["elevator"]`); reject new elevator placement when at limit.
4. **4.4** Enforce **max 8 cars per shaft**: in `Elevator::addCar` (or when loading), cap `cars.size()` at 8; expose in UI (e.g. elevator config or tooltip) if applicable.

**Files:** `source/Game.cpp` (placement/extend validation), `source/Item/Elevator/Elevator.cpp` and `.h` (car limit).

---

## 5. Random Events

**Current state:** None. Assets exist for some (e.g. `alerts/terrorist`, `rating/increased`, `rating/tower`).

### 5.1 Terrorist / Bomb Event

- **Trigger:** Random timer or probability per game-day/week (tune so it's rare but noticeable).
- **Flow:** Notification (use terrorist alert asset); player chooses: pay ransom (lose funds) or dispatch security. If security: run a "search" (e.g. timer or a few in-game hours); if not resolved in time, bomb "explodes" (e.g. damage a floor, reduce rating, or similar).
- **Tasks:** Event state machine, timer, UI dialog/message, security "search" logic, penalty/reward.

### 5.2 VIP Visit

- **Trigger:** Random "VIP will visit in X days" announcement.
- **Flow:** VIP arrives at a set time; track "satisfaction" based on: hotel suite quality, wait times (elevator stress?), maybe restaurant visit. If satisfied, set a "favorable VIP" flag used for 4-star upgrade (see 1.4).
- **Tasks:** VIP state (scheduled → arrived → left), satisfaction criteria, notification UI, and setting the flag for `ratingMayIncrease()`.

### 5.3 Underground Gold Discovery

- **Trigger:** Random chance when the tower has underground floors (e.g. below ground 0).
- **Flow:** One-time message "Workers discovered gold!" and add a lump sum to funds.
- **Tasks:** Random trigger, condition (has underground), message, `transferFunds`.

### 5.4 Santa (End of Fourth Quarter)

- **Trigger:** At end of Q4 each year (time/calendar check).
- **Flow:** Purely cosmetic: play a sound or show a short animation (Santa and reindeer); no gameplay effect (per SimTower).
- **Tasks:** Calendar check in `Time` or `Game::advance`, trigger animation/sound, optional sprite pass.

**Implementation order:** Add a small **EventManager** or **RandomEvents** module (e.g. `source/RandomEvents.h/.cpp`) that is ticked from `Game::advance` and owns event state and timers. Implement 5.3 and 5.4 first (simplest), then 5.2 (needed for rating), then 5.1.

**Files:** New `source/RandomEvents.h`, `source/RandomEvents.cpp`, `source/Game.cpp` (advance + event hooks), UI/dialogs as needed, `source/Time.cpp` if calendar helpers are centralized there.

---

## 6. Small Fixes and TODOs (High Value)

- **6.1** **Rating 1→2 security check:** Remove TODO in `Game::ratingMayIncrease()` and require at least one Security item before setting rating to 2 (covered in §1).
- **6.2** **Elevator cars:** "Constrain cars to stay within elevator bounds" in `Elevator.cpp` (TODO) to avoid cars outside shaft after resize or load.
- **6.3** **Cinema income:** Define and implement cinema income in `Item/Cinema.cpp` (TODO) so revenue is applied when shows complete.
- **6.4** **Cinema hall sprite:** Base hall index on number of guests (TODO in Cinema.cpp) for better visuals.

---

## 7. Optional / Polish

- **7.1** **Save format versioning:** Add a version attribute to the root `<tower>` element so future changes can migrate old saves.
- **7.2** **SimTower binary save compatibility:** Research original save format; optional "import SimTower save" for nostalgia (large effort).
- **7.3** **Stress and tenant vacating:** SimTower tenants vacate if stress is too high; in OpenSkyscraper, stress exists but vacating logic may be incomplete—verify and complete.
- **7.4** **Documentation:** Document rating rules and facility unlock table in-game (e.g. help screen or tooltips) and in user docs.

---

## Suggested Implementation Order

1. **Phase 1 – Rating and tools**
   - 1.1–1.3, 1.8: Complete rating 0→1→2→3 and UI.
   - 2.1–2.4: Facility unlocking by rating.

2. **Phase 2 – Limits and one new building**
   - 4.1–4.4: Floor and elevator limits.
   - §3: Cathedral and Tower condition (1.7, 1.8).

3. **Phase 3 – Rating 4 and 5**
   - 1.4–1.7: Full rating chain (VIP and demands).
   - §5.2: VIP event (needed for 4 stars).

4. **Phase 4 – Remaining events**
   - 5.1, 5.3, 5.4: Bomb, gold, Santa.

5. **Phase 5 – Cleanup**
   - §6 and §7: TODOs, cinema income, optional polish.

---

## Summary Table

| # | Feature | Priority | Depends on |
|---|--------|----------|------------|
| 1 | Full star rating (1–5 + Tower) | High | — |
| 2 | Facility unlock by rating | High | Rating values |
| 3 | Cathedral + Tower goal | High | Rating 5, limits |
| 4 | Building/elevator limits | Medium | — |
| 5 | Random events (bomb, VIP, gold, Santa) | Medium | Event system |
| 6 | TODOs (elevator bounds, cinema) | Low | — |
| 7 | Polish (versioning, stress, docs) | Low | — |

This plan can be used to create issues in a tracker or to assign work in phases.
