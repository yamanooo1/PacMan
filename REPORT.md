# Pac-Man Game Architecture - Design Rationale Report

**Eray Yaman Ayyildiz - s0243673 - 04/01/2026**  
**GitHub: https://github.com/[jouw-github]/PacMan**

---

## Overall Architecture

The project is split into three CMake modules: `logic`, `representation`, and `app`. I didn't know how to set up a multi-library CMake project before this, so that was something new.

The **logic layer** contains all game mechanics - ghost AI, scoring, collision detection. Here I made a controversial choice: the assignment specifies normalized [-1, 1] coordinates, but I used a grid system (0-21) instead. I actually tried [-1, 1] first for two days, but converting between map coordinates, normalized coordinates, and pixels was a nightmare. With grid coordinates, position (10, 5) in code matches position (10, 5) in the map file. The `Camera` class still transforms grid → pixels, so the game scales properly at any window size. I think this keeps the spirit of normalized coordinates (resolution independence) while being way more practical.

The **representation layer** handles SFML - rendering, sprites, sounds, UI. The logic library compiles without SFML (`logic/CMakeLists.txt` has no `find_package(SFML)`). CircleCI builds the logic library separately to verify this separation works.

## Design Patterns Implementation

### Model-View-Controller (MVC)

Models (`PacMan`, `Ghost`, `Coin`) contain game logic with no rendering code. Views (`PacManView`, `GhostView`) handle SFML sprites and respond to model changes via Observer pattern. Setting up this separation with namespaces (`logic::`, `representation::`) was good practice - I'd never structured a project this way before.

### Observer Pattern

Models notify views without knowing about them. `ConcreteFactory` does `model->attach(view)` during entity creation. When something happens, the model calls `notify(PACMAN_DIED)` and all observers update.

This is where I learned about smart pointers the hard way. I used `shared_ptr` everywhere initially and created circular references (model → view, view → model). Valgrind showed memory leaks that took forever to track down. The fix was `weak_ptr` in `Subject::observers` - weak pointers don't increase reference count, breaking the cycle. Now I know:
- `unique_ptr` for exclusive ownership (World owns models)
- `shared_ptr` for shared ownership (views in factory + observer lists)
- `weak_ptr` to break cycles (observers)

Also learned virtual destructors are crucial. Without `virtual ~EntityModel()`, deleting through base pointers wouldn't call derived destructors - more memory leaks.

### Abstract Factory Pattern

`AbstractFactory` in logic defines entity creation methods. `ConcreteFactory` in representation implements them, creating model+view pairs. This lets `World` create entities without knowing about views or SFML. Really useful for level transitions - just call factory methods to recreate everything.

Before this project I used `dynamic_cast` a lot to check entity types. This taught me proper polymorphism instead. Like `Collectable::onCollisionWithPacMan()` gets overridden by `Coin` and `Fruit` - World just calls the method and the right thing happens. Way better than casting everywhere. This helped me understand polymorphism more than last year's C++ exercises honestly.

### Singleton Pattern

`Stopwatch`, `Random`, and `SoundManager` use Meyers Singleton. `Stopwatch` ensures everything uses the same deltaTime, `Random` provides numbers for ghost AI, `SoundManager` handles audio. Pretty straightforward.

### State Pattern

States (Menu, Level, Paused, Victory) use a stack in `StateManager`. Press ESC → push Paused on top, resume → pop it off. Paused is "transparent" so the game stays visible underneath. One gotcha: can't delete a state while it's running, so state changes are deferred to `processPendingChanges()` at frame end. Learned that after some crashes.

## Key Challenges

### Ghost AI Debugging

Making ghost AI wasn't conceptually hard - Manhattan distance, pick direction closer to target. But debugging was awful. You watch ghosts move and sometimes they do something weird, but you don't know if it's a bug or just random behavior.

I added debug output that showed each ghost's position, target, and chosen direction. This revealed a dumb bug: AMBUSHER targets 4 cells ahead of PacMan. If PacMan's at the edge facing outward, target ends up at (-3, 10) or (25, 7) - outside the map. Ghosts acted weird near edges because they pathfound to invalid positions. Fix was just clamping: `targetX = std::max(0, std::min(mapWidth-1, targetX))`. Simple fix, but took forever to find without debug output.

Ghost AI was a good exercise in logical thinking though - Manhattan distance, filtering valid directions, detecting intersections. More interesting than I expected.

### Movement System Redesign

Initially I tried pixel-perfect collision between PacMan (0.8×0.8) and walls (1.0×1.0). Floating-point errors caused PacMan to clip into walls or get stuck in corridors. After fighting this too long, I completely changed approach:

1. Position in grid coordinates (10.5, 7.3)
2. Continuous movement (position += velocity × deltaTime)
3. Only turn at grid centers (within 0.1 tolerance of 0.5)
4. Stop if wall in next grid cell

Code checks ahead: `if (hasWallInGridCell(nextGridX, nextGridY)) stop()`. PacMan never enters wall tiles. Combined with grid-center turns, movement is smooth. Way simpler than pixel-perfect collision. Lesson: sometimes "simple" beats "accurate".

### Camera Frustrations

Started with [-1, 1] normalized coordinates like the spec says. Map file has (0-20), logic needed (-1 to 1), screen uses pixels (0-800). Every coordinate needed multiple transformations - constant mistakes. After two days I switched to grid coordinates. Camera just does `screenX = gridX × (windowWidth / gridWidth)`. Direct transformation, no confusion. Game still scales at different window sizes.

I know this isn't spec-compliant, but pragmatically it made code way easier. Camera still provides proper abstraction between logic and rendering.

## Other Details

**Collision:** AABB detection. Entity types identified by size (Coin 0.02×0.02, Fruit 0.03×0.03, Ghost 0.8×0.8) instead of dynamic_cast.

**Score:** Exponential decay for coins - fast collection (30→27→25 points), slow collection (30→20→15). Plus -10 points/second time penalty with fractional accumulator.

**Animations:** DeltaTime-based timers for frame-independent animation (PacMan mouth, ghost walking, death sequence).

## Lessons Learned

If I started over:

1. **Plan World class better** - It got bloated with too many responsibilities
2. **Write tests earlier** - Testing by playing made debugging painful
3. **Commit more often** - Hours between commits made undoing hard
4. **Better logging from start** - Would've helped with ghost AI

**Main takeaways:**
- Smart pointers need careful ownership design, not magic
- Polymorphism is powerful when used properly
- Grid coordinates can be simpler than normalized for some games
- Debugging AI needs good logging
- Memory leaks are sneaky - Valgrind saves lives

Overall fun project. Got to work with game logic, AI, graphics, state machines. Ghost AI debugging was frustrating but interesting. Learned a lot about software architecture and C++ I didn't know before.

---

**Word count: ~1250 words (~2 pages)**
