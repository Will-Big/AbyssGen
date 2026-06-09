# AbyssGen Portfolio Plan

## Final Goal

Build AbyssGen into a portfolio-ready Unreal Engine demo that clearly shows:

- Procedural dungeon generation
- Data-driven spawn system
- Special treasure ambush room
- Reusable StateTree monster AI
- Demo video and README presentation

The project should be presented as:

```text
AbyssGen
Procedural Dungeon Generation & AI Encounter System in Unreal Engine
```

## Working Assumption

Code implementation will be supported heavily by AI. The developer's main role is to:

- Decide architecture and feature scope
- Set up Unreal Editor assets and Blueprint values
- Test gameplay behavior
- Review generated code and understand the intent
- Prepare portfolio video and README explanations

## Day 1-2: Core Structure Stabilization

### Goal

Make the current dungeon, spawn, and monster base systems buildable and explainable.

### Tasks

- Review `DungeonGenerator` flow.
- Confirm `RoomBase`, `ExitPoint`, and overlap box rules.
- Review `SpawnPoint` structure:
  - `ContentType`
  - `Requirement`
  - `SpawnTag`
  - `SpawnChance`
- Confirm `DefaultSpawnTables` and `TaggedSpawnTables` editor setup.
- Confirm `AbyssMonsterBase` and `AbyssMonsterAIController` build successfully.
- Finish basic `BP_SlimeMonster` setup.

### AI Usage

- Ask AI for code review.
- Ask AI to fix compile errors.
- Ask AI to summarize class responsibilities.

### Done When

- Project builds successfully.
- Dungeon generation does not crash.
- `BP_SlimeMonster` can be placed in a level.
- Spawn system structure can be explained in README draft form.

## Day 3-4: Monster AI Completion

### Goal

Complete the minimum combat loop for Slime:

```text
Find player -> Chase player -> Attack player -> Die when HP reaches 0
```

### Tasks

- Finalize `ABP_Slime`.
  - Idle state
  - Run state
  - `DefaultSlot` for Montage playback
- Connect `AM_Slime_Attack01`.
- Create and configure `ST_SlimeMonster`.
  - `Find Player Target`
  - `Chase`
  - `Attack`
- Connect Chase movement behavior.
- Connect attack range condition.
- Check attack trace/debug behavior.

### AI Usage

- Ask AI to improve StateTree Task/Condition code.
- Ask AI to explain Move To or StateTree AI API.
- Ask AI to debug attack range or Montage issues.

### Done When

- Slime finds the player.
- Slime moves toward the player.
- Slime attacks when inside attack range.
- Attack Montage plays.
- Attack trace/damage is triggered.
- Death behavior works.

## Day 5-6: Special Treasure Ambush Room

### Goal

Create a special room where opening the central treasure spawns monsters from four corners.

### Tasks

- Create `BP_SpecialTreasureRoom`.
- Place central treasure/chest.
- Place four corner ambush spawn points.
- Design the chest-open event.
- Implement `TriggerAmbush`.
- Spawn random monsters through either:
  - `Enemy.Ambush` spawn tag/table
  - or a room-local monster class array
- Register the room in `SpecialRoomsToBeSpawned`.

### AI Usage

- Ask AI to design the event-spawn structure.
- Ask AI to implement chest-room communication.
- Ask AI to write random spawn logic.

### Done When

- Special room appears in generated dungeon.
- No ambush monsters exist before opening treasure.
- Opening treasure spawns monsters from four corners.
- Spawned monsters use StateTree AI and attack the player.

## Day 7-8: Portfolio Polish

### Goal

Make the project easy to understand visually and stable enough for a short demo video.

### Tasks

- Stabilize generated room flow.
- Clean up doors, closed exits, and spawn point placement.
- Add useful debug visualization:
  - Normal room
  - Special room
  - Enemy spawn point
  - Reward spawn point
  - Exit point
- Tune monster stats.
- Improve treasure room presentation:
  - Chest opening timing
  - Ambush timing
  - Optional VFX/SFX if available
- Prepare a short demo route.

### AI Usage

- Ask AI to add debug visualization code.
- Ask AI to generate architecture diagram text.
- Ask AI to propose demo recording script.

### Done When

- The game loop can be shown in a 1-2 minute video.
- Important systems are visually understandable.
- No major visible bugs block the demo.
- The special room feels like a portfolio-worthy encounter.

## Day 9-10: Video and README

### Goal

Package the project as a portfolio artifact.

### Tasks

- Record demo video.
- Recommended video flow:
  1. Dungeon generation
  2. Normal room exploration
  3. Spawn system/debug view
  4. Special treasure room entry
  5. Chest opening
  6. Ambush monster spawn
  7. StateTree AI chase/attack
- Write README.
- Capture code/architecture screenshots.
- Add GIF or video link.
- Clean up GitHub/project presentation.

### AI Usage

- Ask AI to draft README.
- Ask AI to polish technical explanation.
- Ask AI to write video captions or narration.
- Ask AI to write concise portfolio copy.

### Done When

- README is complete.
- Demo video is complete.
- Project purpose and technical highlights are clear.
- The project is ready to share as a portfolio link.

## Recommended README Structure

```text
# AbyssGen

## Overview
Procedural dungeon generation and AI encounter system built in Unreal Engine.

## Key Features
- Procedural room generation
- Data-driven spawn system
- Special treasure ambush room
- Reusable StateTree monster AI

## Technical Highlights
- Exit-based room expansion
- SpawnTag / SpawnTable design
- Optional / Guaranteed / Unique spawn rules
- Pawn-owned StateTree AI setup
- Event-driven special room encounter

## Architecture
- DungeonGenerator
- RoomBase
- SpawnPointComponent
- AbyssMonsterBase
- AbyssMonsterAIController
- StateTree Tasks / Conditions

## Demo
Video / GIF

## Development Notes
Problems solved, tradeoffs, future improvements
```

## Guiding Principles

- Do not add too many new features.
- Every feature should be visible in the demo video.
- Architecture clarity matters more than content volume.
- AI can write code, but the developer must understand and explain the design.
- End every 2-day milestone in a buildable state.

## Portfolio Pitch

```text
AbyssGen is a portfolio project focused on procedural dungeon generation,
data-driven encounter placement, special room events, and reusable StateTree-based monster AI in Unreal Engine.
```
