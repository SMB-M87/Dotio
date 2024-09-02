# **Dotio**
The purple circle is the player character the red ones are NPC’s. When running the application
you’ll immediately be in a game, on the bottom left corner you’ll notice your stats. The red bar is
your health, the blue one represent your mana and the green one is your bullet velocity.
You can kill enemies in two di@erent manners by shooting at them which will drain your mana
bar or by hitting them. Your mana and health regenerates over time. Shooting an enemy down
will restore some health if any was lost and upgrade your bullet velocity upon 100kills. Hitting an
enemy will restore mana. Enemies patrol around and will only start attacking you when you’re in their line of sight,
meanwhile when activated they will chase you. Enemies also need to regenerate from time to
time and will not be able to shoot at you in that state. You may be noticing them flee or stand still
somewhere hiding from you, killing them will give your health and mana back. Watch out for
enemies with a slight pinkish tint, they’re in full kamikaze modus and will not stop until they blow
up on you, however killing them will grant you health and mana back!

# **Controls**
*Movement:* {Up}, {Down}, {Left}, {Right} or {W}, {S}, {A}, {D}
*Shoot:* {Left-Mouse}
*Zoom-In:* {V}
*Zoom-Out:* {Shift+V}
*Pause game:* {Spacebar}
*New game:* {Shift+Spacebar}
*Quit Application:* {Escape}

# **Debug**
*Enter debug mode:* {Shift+`}
*Enter or leave god mode:* {G}
*Change player position to god mode position:* {Shift+G}
*Show all path’s:* {1}
*Show wander path’s:* {2}
*Show pursue path’s:* {3}
*Show kamikaze path’s:* {4}
*Show recover path’s:* {5}
*Show hide intersection’s:* {6} 

# **Features**
This project was built by hand from the ground up without any external libraries in C++. We used
native Win32 API for OS manipulation. For handling graphics Direct2D from Win32 API was used.
A lot of labor went into the basic features that are taken for granted, like creating a window,
keyboard, mouse handling, collision, etc. While those features are an achievement in itself, this
chapter will highlight the features that are related to algorithms and artificial intelligence.

- **Seek/Flee/Wander**
Basic steering behavior where enemies follow the player/path if there are no
obstructions. Flee is used when the enemy needs to step back or place distance in
between them and the player. Wander is used on patrol.

- **Wall collision**
Player and enemies are not able to go through walls.

- **Line of sight**
Enemies should only shoot if the player is not behind a wall.

- **Predictive aiming**
The enemies adjust their aim to where the player would be dependent on the bullet’s
trajectory and the player’s velocity.

- **Segment-circle continuous collision detection**
A fast traveling projectile should not go through its target.

- **Collision detection between enemies**
Enemies should not go through each other. In our implementation the enemies can push
each other out of the way. This algorithm makes use of spatial partitioning.

- **Spatial partitioning**
For collision detection between enemies it would be very ine$icient to compare
distances between all enemies for all enemies. A map keeps track of which enemies are
on which cell.

- __A*__
Is used to find the shortest path to the enemy for chasing purposes, but is also recycled
to generate a neat looking path on wander and a tweaked variant(HideStar) of it is used to
find a close by intersecting cell to hide in while fleeing. 

- **State Driven Enemies(state.h/cpp):**
  - **Wander(blue):** By default the enemy patrol around in the world with no precise
  destination using the wander behavior while using A* to find a path to a close by
  generated position. While patrolling if the player gets in the line of sight of the
  enemy it will immediately enter the attack state.
  
  - **Attack(red):** The enemy start shooting arrows at the player. An enemy can shoot
  up to ten arrows in a row before being out of ammo and after each shot the
  enemy steps back anticipating finding a hide spot to recover at. If the player is
  not in the line of sight anymore it will enter the pursue state and if the enemy is
  out of ammo it will enter the recover state. However in this state whenever the
  fuzzy logic evaluation criteria’s are met the enemy will enter the kamikaze state.
  
  - **Pursue(orange):** The enemy will chase the player down using A* and seek
  behavior to follow its path. While chasing a countdown is ran, if by the end of the
  countdown the player did not got in the line of sight the enemy will fall back to
  the wander state. However if it got sight of the player the enemy enters back into
  the attack state.
  
  - **Recover(green):** The enemy will try to find a hiding spot where the player is not in
  the line of sight using HideStar and seek behavior to arrive at its hiding location.
  Only while the enemy is not in the line of sight will it be able to recover ammo.
  When it’s ammo is fully replenished the enemy will enter the pursue state. If the
  enemy is stuck and can’t go anywhere or can’t escape the pursue of the player it
  will enter the kamikaze state instead of just accepting it’s inevitable death.
  
  - **Kamikaze(pinkish):** This is a final state for the enemy, the only way out is dying
  on impact with the player or being killed by the player. The enemy will chase
  down the player the same way it does in the pursue state if it’s not in the line of
  sight, but when given the chance will also shoot and seek like in the attack state.

- **Fuzzy Logic:** To manage when a player enter the kamikaze state fuzzy logic is used. A
  combination of the distance between the enemy and the player, the fear level and the
  amount of ammo left determines if it’s time to go all in. You’ll find everything correlate to
  this feature in the fuzzy.h/cpp files:
  
  - **Triangle:** Correspond to a triangular fuzzy membership function. It takes three  
  points that stands for a triangular shape. If the given value falls outside the range
  a or c, the membership ratio is 0. If it equals b, it’s 1. Otherwise the ratio is
  calculated based on the linear interpolation between a and b or b and c.

  - **Trapezoid:** Represents a trapezoidal fuzzy membership function. It takes the
  given value and four points a, b, c and d that define the trapezoidal shape. If the
  given value falls outside the range of a and d the membership ratio is 0. If it’s
  within the range b and c, it’s 1. Otherwise the degree is calculated based on the
  linear interpolation between a and b or c and d.

  - **Distance:** This function evaluates the fuzzy distance ratio based on the input
  distance value and the membership functions described above. The result is
  either: Close, Average or Far.

  - **Fear:** Same concept as above but with the fear parameter. The outcome can be:
  Anxious, Neutral or Warrior.

  - Ammo: Outcomes: Full, Midway or Empty.

  - Evaluate: It all boils down to this function where the logic is used to evaluate if
  the enemy needs to change its state.
