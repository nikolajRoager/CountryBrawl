Plan for the game
=========

Short summary
--------------

A real-time strategy game, inspired by "capture the base" game's like Solarmax, "Logistics games" like Creeper World, and war games with front-line like HoI

Every country has a number of cities, cities make stuff like money and different types of ammunition (with a timer), including bullets, drones, grenades, shells, SAMs and SSMs

Countries also have soldiers, soldiers consume stuff like ammunition, soldiers can be different types like infantry, artillery, and more

Soldiers stand around on the front-line, or the support line, which is behind the front-line, depending on their type.

I haven't yet decided if the front-line and support line should be two lines, or just one

Soldiers automatically shoot at other soldiers which are in range

Soldiers can be recruited at cities

Soldiers may have upkeep cost

Soldiers can "fast-travel" along the network of neighbouring cities

Soldiers can be ordered to attack (abandon their front-line and move towards another city)

If the attackers kill all defenders, and move to the frontline around the city, the city is captured

Soldiers consume ammunition, creating a "demand" for ammunition at their city. Demand is met by cities which have the required ammo in storage.

The created ammo then travels along the network of cities to the city where demand is.

The game need to keep track of how many disconnected "networks" there are, keep in mind some country may have a disconnected exclave

Maybe some cities can act as supply depots

There may be different types of cities: some producing a bit of everything, and some specialised cities producing just one thing really well

Cities have an "owner": who owns the city, and a "core": who rightfully owns the city. If owned by the core owner, the city has its full output, otherwise the city has reduced output.

There is a minimal "diplomacy system", it might be as simple as "declare war", "send gift", "ask for lend lease" (can requisition ammo from that nations factories, must be bordering), "ask for military access", "ask for ceasefire"

Enemy countries are controlled by an AI opponent, responsible for diplomacy and warfare decisions.

Key features, in order of implementation
-----

* A map for us to play on (and an editor to create it)
  * Including cities with owners and cores at game start
  * Including neighbours for all cities
  * Possibly including front-lines, unless that is auto-generated ... you know what even if it is auto-generated, we should probably pre-generate it as well
* Minimum viable game
  * Has menu screen, which allows selection of player countries
  * Can create soldiers and move soldiers around, has the requisite UI to do that
  * Soldiers go to their post on the front-line
  * Soldiers shoot at each other
  * AI "balances" its own borders