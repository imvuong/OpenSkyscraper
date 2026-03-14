# Rating Rules and Facility Unlocks

This document describes how star rating works and when each facility becomes available, matching the in-game logic (SimTower-style).

## Star rating (1–5 stars + Tower)

Rating index in code: 0 = 1 star, 1 = 2 stars, 2 = 3 stars, 3 = 4 stars, 4 = 5 stars, 5 = Tower.

### Requirements to reach each rating

| From | To   | Requirements |
|------|------|-----------------------------------------------------------------------------|
| 1 ★  | 2 ★  | Population ≥ 300 **and** at least one **Security** office. |
| 2 ★  | 3 ★  | Population ≥ 1,000 **and** at least one **Security** office. |
| 3 ★  | 4 ★  | Population ≥ 5,000 **and** at least 2 **Hotel Suites** **and** **Recycling** **and** **Medical Center** **and** a favorable VIP visit. |
| 4 ★  | 5 ★  | Population ≥ 10,000 **and** a **Metro Station**. |
| 5 ★  | Tower| Population ≥ 15,000 **and** a **Cathedral** **and** all above-ground floors developed (no gaps). |

If you meet population but not the other requirements, the game shows a hint (e.g. "Your tower needs security.", "A favorable VIP visit is required for 4 stars.").

### Constants (from `RatingRules.h`)

- 2 stars: population ≥ 300, Security count ≥ 1  
- 3 stars: population ≥ 1,000, Security count ≥ 1  
- 4 stars: population ≥ 5,000, Hotel suites ≥ 2, plus Recycling, Medical Center, favorable VIP  
- 5 stars: population ≥ 10,000, Metro  
- Tower: population ≥ 15,000, Cathedral, fully developed above-ground floors  

---

## Facility unlock table

Facilities are locked in the toolbox until the tower reaches the required star level. Hovering a locked tool shows "Unlocks at N stars."

| Stars | Facilities |
|-------|------------|
| **1 ★** | Floor, Lobby, Stairs, Standard Elevator, Fast Food, Office, Condo, and other base items. |
| **2 ★** | Security, Housekeeping, Hotel Single, Service Elevator. |
| **3 ★** | Parking Ramp, Parking Space, Recycling, Medical Center, Hotel Suite, Hotel Double, Party Hall, Cinema, Shop, Restaurant, Express Elevator, Escalator. |
| **4 ★** | Metro Station. |
| **5 ★** | Cathedral. |

---

## In-game reference

- **Toolbox:** Locked tools show "Unlocks at N stars" on hover.  
- **Time window:** Current rating (stars) is displayed; the game shows short messages when you are close to the next rating but missing a requirement (e.g. security, VIP visit, Metro, or full development for Tower).
