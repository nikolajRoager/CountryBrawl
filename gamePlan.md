Plan for the game
=========

Short summary, before the MVP
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

Expanded game 
======
With the MVP done, it is time to think about the expanded game, there are a couple of categories we can work on:

* Tactical Combat
* Strategic warfare
* Buildings
* Economy
* Rebalancing

Tactical combat
------

* police fire
* Defenders advantage (higher fire-rate, more health maybe) (can be boosted by buildings)
* Drones and Grenades
* Artillery (Are we still doing this?!)
* Tactical missiles (from buildings)
* Ammo system (Requires supply lines)
* Airplanes

Buildings
--------
Buildings can be upgraded along different paths:

* supply hub (for supply line mechanic)
* Missile base
* Factory
* airbase

On the other hand, maybe supply hubs and factories should not be mutually exclusive, 

Anti-air and fortification can freely be added to all bases

Strategic warfare
------------
Missiles and airplanes can target and disable cities, it takes time for cities to be repaired

Supply lines
------------
Factories produce stuff:
* bullets
* grenades
* drones
* cruise missiles
* SAM

Factories have a small capacity for storing stuff

Supply hubs absorb stuff from nearby factories (possibly automatically, possibly using trucks/trains)

Supply hubs have a large capacity for storing stuff

All cities consume stuff on behalf of soldiers associated with them

Soldiers create demand for stuff in their associated cities, demand is met by nearest supply hub which can satisfy demand

Demand may be met from further away supply hubs in the network

Supply networks recalculate every time a city is captured

Come to think about it, we may have supply network follow city network, or have set range... city network is better

We need a new city editor, or to upgrade the old one, to place down supply hubs

Rebalancing
------

* Welfare spending (max money)
* Recruitment should be a lot slower
* Police