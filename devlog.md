Day 0 15-12-2025, workload 6 hours 
====

Plan
----

* We need to at least be able to add and display bases, with the right colour and maybe with a flag, Done

* Add option to select base by right-clicking (add highlight to base), Done

* Start working on the architecture of the UI, Done

* Load a selection of countries in Europe and North Africa, and the middle east, Done

* Try to add: uiTexture, uiTextline, uiLineList, maybe add uiButton, Done

Work
----
I am able to display bases, yet I am missing a way of placing them down, I have only loaded Denmark, Norway and Sweden

The UI system is working beautifully, I have both text, textures listLines and buttons

Issues
-----
If the program throws an exception while loading, some textures  (like ball in water, base, uiBackground, or the buttons) are not properly disposed off

day 1 16-12-2025, workload 6 hours
===============

plan
----
* Get the program working on Windows OS, Done, I also had to fixe the texture unloading issue

* Add the option to create bases with shift+click, Done

* Select bases with right-click and open up another menu (goes into edit base mode). Done

* Edit base income, ownership, and have option to delete base. Done

* Have option to name bases and provinces, that might be difficult, Done, that was easy.

* Consider how to get cores to work, maybe there should only be one core owner. Done, but didn't add option to change it

* Add a bunch of new countries and automatic country loading. Failed

work
---
I had some time to work, because I was waiting for a meeting call which, of course, never came.

I can now add bases, and edit some aspects of the bases.

day 2 17-12-2025, workload 9 hours, mainly adding countries
=============

plan
----
* Make base icon bigger, Done, (but I liked the smaller icons better)

* Add option to change core and owner from edit menu Done (but multiple cores are clearly needed Romania should core Moldova) (correction, single cores are more realistic)

* Add ability to view longitude/latitude of mouse. Done

* Add all countries and automatic country loading: Failed, I added most, but I am still missing 15 countries, and didn't get to loading


Work
----
I mainly added countries, the entire day, there are a lot of countries in Europe, seriously a lot

Issues
-----
Political issue: the game needs to support multiple CORE owners: example, Both Libyan governments need to core all of Libya

Correction, NO, I think single cores is more realistic. Instead there need to be unique events to gain cores

day 2.5 18-12-2025, workload 4 hours (half day)
=============

plan
----
* Make base icons smaller again, Done.

* Add the remaining countries, and automatic loading (ALMOST done)

* Add option to delete base (Failed)

* Add ability to save and load (Failed)

Work
----
I added almost all countries, and the option to load.
Loading is problematic due to SDL texture threading issues, I had a lot of crash bugs before I settled on sub-optimal single thread loading.

Issues
------
I need to multithread country loading, multithreading SDL texture loading is not possible, I need to seperate loading from finalization

day 3.5 19-12-2025, workload 9 hours
=============

plan
----
* Add the remaining countries (Done)

* Add ability to set neighbours (Done)

* Add ability to save and load (Done)

* Add display of core as well as owner (Failed, I completely forgot)

* Get a better font and fix issues rendering  (Done, Atkinson Hyperlegible must be the best)

* Start thinking about which cities to add (Oh my god, there are a lot of cities in Europe)

Work
----
Oh my god, there are a lot of cities in europe, like really a lot, I thought using 100000 population as a cut-of was a good idea, but there are hundreds of cities above that population.

I think there is nothing for it, I will just have to spend forever adding cities ... well, that is what I created this program for

Oh, I also got loading and saving working, and added the option to declare neighbours ... but I am going to only do that later, right now, I have hundreds of cities to add.

On a further thought, I won't commit to the 100000 limit everywhere, I will adjust it up or down for densely or sparsely populated regions

Issues
------
I need to multithread country loading, multithreading SDL texture loading is not possible, I need to separate loading from finalization

Fix Turkey missing ball

Fix France wrong colour

Only display city textures when on screen

day 4.5 20-12-2025, workload 9 hours
=============

Summary
-----
I forgot this log on the day, I basically spent the entire day adding cities in the Maghreb and middle east.

day 5.5 21-12-2025,  Workload 11 hours
=======

plan
-----
Add the bases for the following very ambitious list of countries: DONE, that took like forever
* France (Done)
* Germany (Done)
* Belgium (Done)
* Netherlands (Done)
* Luxembourg (that one should be fast) (Done)
* Italy (Done)
* Austria (Done)
* Slovenia (Done)
* Czechia (Done)
* Slovakia (Done)
* Hungary (Done)
* Croatia (Done)
* Serbia (Done)
* Bosnia and Herzegovina (Done)
* Montenegro (Done)
* Kosovo (Done)
* Albania (Done)
* North Macedonia (Done)

Fix missing countryball for Türkiye (Done)

Fix wrong colour for Bosnia and hzg., kosovo, austria, serbia (should be white or dark blue) (Done)

Add flags to the bases themself (Done, I don't know if it is better)

Work
----
I had a lot of time to work, now when the holiday has begun, so I worked a lot

Issues
------
I need to multithread country loading, multithreading SDL texture loading is not possible, I need to separate loading from finalization

Only display city textures when on screen

day 6.5 22-12-2025,  Workload 12 hours
=======

Plan
----

Add the flag and countryball for andorra

Fix Colour of Finland, and maybe norway or Estonia

Add the bases for the following very ambitious list of countries:  DONE
* Poland Done
* Lithuania Done
* Latvia Done
* Estonia Done
* Romania Done
* Bulgaria Done
* Ukraine Done
* Turkey Done
* Iraq Done
* Georgia Done
* Norway Done
* Sweden Done
* Finland Done
* Andorra Done
* Belarus Done
* Russia DONE DONE DONE DONE DONE ... DONE

Work
-----
I added cities all day, it was terrible, but it is done, I am actually truly fully done with adding cities.

Issues
------
I need to multithread country loading, multithreading SDL texture loading is not possible, I need to separate loading from finalization

Only display city textures when on screen

day 7.5 23-12-2025, workload 8 hours
=======

Plan
----
Rename bases to cities (Done)

Allow zooming further in, without scaling up the city texture (Done)

record a video on: the project, the progress (Done and uploaded)

Prepare thorough plan for the game (for the video) (Done)

Start working on neighbours and maybe implement borders (Done I guess, at least I tried, and failed)

work
-----
I recorded and uploaded a video on my progress, that took most of the time.

I also did a lot of thinking about how to handle the front-lines, and I got something working and it is worthless garbage which need to be completely replaced

But well, I didn't know that it would be bad before I tried it, so the work still counts

oh, and I went through he map and added all neighbours

day 8 24-12-2025, workload 4 hours
=======

Note
----
I forgot to write the devlog for this day, I worked on the main menu for the finished game because I felt like I needed to do something else

day 9 25-12-2025, workload 12 hours
=======

Note
----
I forgot to write the devlog for this day, I worked on the main menu for the finished game because I felt like I needed to do something else

In particular I worked on the select country menu, which involved writing a lot of description texts for each country.

day 10 26-12-2025
=====

plan
----
Finish writing descriptions for every country. Done

Display all relevant information for selected countries. Done

Open up the game, load tiles asynchronously and display the map FAILED, AND HUGE ISSUE FOUND

Work
----
While working on the asynchronous loading of the tiles, I had an unexplainable undignified crash (where the program crashed without throwing an exception)

This led me down a rabit-hole, which culminated with me realising that `IMG_Load` is not thread safe!!!

If the three exclamation marks doesn't give it away, this is a huge issue, because I have used multithreaded image loading quite liberally
I need to completely revamp the texture loading infrastructure, but at least I know what is wrong, and what needs to be done

issues
--------
IMG_Load is not thread safe, replace it with another png loading function

Update, some AI agent insist that it IS thread safe, some say it is not. I am going with NOT to be on the safe side


day 10.5 27-12-2025 workload 4 hours
=============
Note
---
I was visiting my grandparents, and didn't have a lot of time to work.

I also did a lot of reading up on multithreading and the likes ... I don't think that should be counted as part of this project per-se so I think my work fits under the 4 hour mark

Whatever I am counting this as a half day.

plan
--------
Get running on Windows Done

Load tilemap with STB FAILED ... I had a lot of problems and eventually gave up and went back to SDL ... I don't think this is a long term solution

Load cities and countries and just display the damned map DONE

Replace std::async with a proper thread-pool FAILED, this is a huge undertaking

issues
--------
IMG_Load is not thread safe, replace it with another png loading function
At the very least use STB when loading map data
Get a working thread-pool system

day 11.5 28-12-2025 Workload 6 hours
=============
Note
----
I am somewhat doubtful as to what to do today, I have essentially four things I could do:

* Add a music management system, and add some music to the menu
* Make a threadpool loading system
* STB image loading
* have countryballs stand around bases at the frontlines

I will get the trusty old random number generator, and got dammit, I will be working on STB image loading

I won't have a lot of time to work today anyway. (Ok, I did only have 6 hours to work, I should really have held it under the 4 hour mark)

plan
-----
apparently the plan is to Get STB image loading working (DONE, the program now fails safely). What on earth do I do now, I thought that was going to take longer

Add countryballs, so that countryballs stand around the cities, or along the front-lines. DONE I guess


Issues
-----
Get a working thread-pool system

Balls jiggle

Front-lines should be proper locations, start facing each other then pull back if they are in water. Artillery lines are missing

Balls miss weapons

watermap and movementPenalties should use STB

day 12.5 29-12-2025 Workload 10 hours
=============
Note
-----
Once again, I don't know what I shall work on, so lets make a list, actually lets just get the issue list of issues and randomly sort it:


* Balls miss weapons
* watermap and movementPenalties should use STB
* Get a working thread-pool system
* Front-lines should be proper locations, start facing each other then pull back if they are in water. Artillery lines are missing
* Balls jiggle

Alright, that is my plan for tomorrow (yep I am writing this the day before):

Plan
-------

Add weapons to balls, use the weapons that country actually uses 
OH MY GOD EVERYONE HAS THEIR OWN MODEL GUN, THIS WILL TAKE LIKE FOREVER. Done, it took like forever

Weapons should point the right way. Done

watermap and movementPenalties should use STB. Done

Make a working thread-pool system (in another project). Done

Implement thread pool loading of tiles and watermap/movementPenalties in this project. Done

Implement early bail on exceptions. Done

See if you can fix the jigglyness. Done

Make front-line start 25 px from each other, pulled back in event of water. Partly Done, I miss the pulled back out of water part

WOW, I got everything I planned done... wow just wow.

day 13.5 30-12-2025, workload 10 hours
=============
Note
----
Once again, I have no idea what to do (since I amazingly finished all open issues yesterday)

Well, lets try to come up with a few things, and shuffle them


* URGENT: test the thread-pool on windows
* Framerate counter
* Center on selected nation on startup
* Ability to move soldiers betwixt cities
  * Requires ability to click on and select cities, we need to indicate the selection to the player visually, maybe with a sound
* More work on front-lines, pull out of water, clamp to the cities themself
* Cities should indicate how many soldiers they have, should be able to handle soldiers of different allegiances
* Multiple types of soldiers (artillery and infantry)
* Projectiles and death

I already shuffled it, so I guess that is my plan.

But when it comes to the movement of soldiers, I need to think about what style of controls I would like.

I think left-click to select (shift+left click to select multiple) and right click to move makes sense, it is just like the Paradox games which I like

Plan
----

URGENT: test the thread-pool on windows Done (That was hard, Windows is way stricter with the C++ standard than Linux)

Framerate counter. Done, and it is fine

Center on selected nation on startup. Done

Add ability to select cities by left-clicking them Done

Add selection marker to cities when mouse-over Done

Add selection marker to cities when selected Done 

Add ability to select multiple cities by shift+left-clicking Done

Add box to draw over multiple cities by holding down left Done

Add highlight of neighbouring paths for selected cities, for now only highlight path to adjacent cities. Done

Add ability to move soldiers, by right-clicking an adjacent city. Done

Move Should prioritize soldiers along the relevant front. Done

Pull front-lines back from water. Done

Clamp front-lines to cities Done

Add artillery line Done

Record a video Done

Cities should indicate how many soldiers they have, should be able to handle soldiers of different allegiances  Done

Multiple types of soldiers (artillery and infantry) Failed (Out of time)

Projectiles and death Failed (Out of time)


Issues
------
Add soldier shouldn't "reshuffle" soldiers, instead it should add to the front which needs the most and only shuffle that

Issue, it is possible to "Outflank" cities by rapidly switching attack vectors, this should not be possible, or we need to find a way to defend against it

day 14.5 31-12-2025 workload 6 hours
=============
Note
-----
Again, I don't know what I need to do today, and this day, I probably won't have a lot of time to work, and I don't have the luxury of writing this note the day before.
  
Anyway, let us write down a list of things I would like to see added:

* Multiple types of soldiers (artillery and infantry)
* Projectiles and death
* Base capturing
* UI for the game

I kind of feel like adding combat first

In retrospect, this was another day where I really should have accepted that I didn't get to work so much and made this a half day, instead this will count as a full day even though it was only 6 hours

Plan
----
Have soldiers shoot at enemies if in range DONE (doing so was easy, doing so without killing performance was hard, doing so and fixing the bugs was extremely hard)

Add bullet sounds for shots on screen and close in enough  Failed, I simply forgot

Have soldiers who get shot die Done

Kill dead soldiers (remove them from bases, have them despawn maybe) Done

Fix reshuffling (I imagine removing dead soldiers will trigger it) Failed I was out of time, and the capture condition seemed more pressing

Add condition to capture base (I think when base is outnumbered, and at least one attacking soldier is on post around it) Done

Have defeated soldiers retreat from captured bases Failed (out of time)


issues
--------------
Add or kill soldier shouldn't "reshuffle" soldiers, instead it should add to the front which needs the most and only shuffle that

Issue, it is possible to "Outflank" cities by rapidly switching attack vectors, this should not be possible, or we need to find a way to defend against it

For now, everything is based on dt, we should add a form of "time-warp" (Requires somewhat of a UI)

We need a diplomacy system, right now everyone shoots at everyone, and everyone has a hostile border with everyone.

We need a "tension" diplomatic matrix

We need a "military access" diplomatic matrix

We need to highlight all neighbours of cities


day 15.5 1-1-2026, workload 10 hours
=====

Note
----
Today will hopefully be a good day with a lot of work

There are a lot of loose ends for me to try to close, let us make a list

* The shots need to be better looking, maybe add some particle effects
* We need sound effects for the shots, while we are at it, music
* We need to fix the reshuffling issue
* we need fast-transportation betwixt friendly cities with visual "trains"
* Visualize the neighbouring cities of the selected city ... or just the hovered city ... or all of them
* Add the beginnings of the UI, we need a top or bottom bar with economy, timewarp, and date-and-time, and maybe a "balance fronts" and "auto-advance" menu menus ... think about what menus we need

I do feel like starting on the transportation system, I feel that might be a whole-day-and-then-some project

An extended think about trains
-----------
This is such a delicate issue, that some thinking may be required before I can start this.

The end goal is that a group of soldiers, receiving an order to redeploy to some friendly or neutral city far away:
* Calculates if there is a path from their city to the target city
* Then goes to their own city
* Then (only when all soldiers have arrived by the city) they despawn from the map, and a train spawns.
* The train drives (much faster than soldiers can walk) along the path so calculated
* When the train arrives at the destination, the soldiers get out (respawn on the map) and are now assigned to that base
* If tha path becomes invalid, the train stops wherever and the soldiers get out, assigned to the last location they were at
* Trains can be destroyed, killing their occupants

For a start, I will only consider immediate neighbours, so I don't need pathfinding (realistically, taking the trains is likely slower in that case, and may not be done in the final game)

I think I will need some kind of "ticket" or "journey" class to keep track of the entire journey.
The ticket should be managed by the game, and it should "own" the soldiers on the ticket, the same way a base "owns" the soldiers around it
I don't think the soldiers need a reference to their ticket, but they need some variable to tell that they are on a ticket

wait, I just had a horrible thought ... HOW ON EARTH AM I GOING TO IMPLEMENT SAVING AND LOADING IN THIS GAME ... oh GOD, that is a worry, for another day

Plan
------
Transportation of soldiers to friendly bases with trains (Done, that took like forever)

issues
--------------
Train should "rotate" to point the right way

trains and their passengers shouldn't be immortal

We need higher resolution train

Add or kill soldier shouldn't "reshuffle" soldiers, instead it should add to the front which needs the most and only shuffle that

Issue, it is possible to "Outflank" cities by rapidly switching attack vectors, this should not be possible, or we need to find a way to defend against it

For now, everything is based on dt, we should add a form of "time-warp" (Requires somewhat of a UI)

We need a diplomacy system, right now everyone shoots at everyone, and everyone has a hostile border with everyone.

We need a "tension" diplomatic matrix

We need a "military access" diplomatic matrix

We need to highlight all neighbours of cities

Visualize the neighbouring cities of the selected city ... or just the hovered city ... or all of them

Implement the bonuses for the nations

day 16.5 2-1-2026 Workload 9 hours
=====

Note
----
Yesterday saw the trains brought to a state where they appear to be working, at the cost of the entire day of work.

I do feel like picking another topic for today, and making the whole day about that

I think todays theme should be UI, let us add a UI system to the game, and let us add relevant things for the UI to display, like a time system

Thinking about UI
-------
What do I think should be in the UI, well

The UI will be made up of a top-bar, with elements, elements may open a menu, menus may have a search bar (search for countries) and a scroll bar (scroll through countries), all elements have a mouse-over text block


* Date and time + timewarp
  * Requires that we think about pause
* Income and treasury + breakdown
  * Requires that we add it
* Army size
  * Requires that we calculate it, and add army cap
* diplomacy menu
  * Requires diplomacy engine (yeah, we are unlikely to actually add this today)
* auto-reinforce switch
* balance front-lines button
* music menu + mute button
* Reminder of who we are at war with
* Maybe a terminal with logs of events

The big problem is that each thing I want to add to the UI, requires that I add it in game, it has now been 6 hours, and I only added the calender, because I had to add timekeeping, which took time.

plan
----
Implement a "gametime" variable which keeps track of non-paused millis since game start Done

Space to pause gametime Done

Make UI top-bar class which automatically fills the screen Done 

make (virtual) UI top-bar element Done

make (derived) UI clock class, which converts gametime to year, month, day, hour, minute (using Chrono) Done

Add number of cores and occupied cities to nation Done

Add ui to show that Done

Add army cap to nations Done

Add ui containers to that Done

Add mouse-over text to relevant things Failed (out of time)

issues
--------------
Train path is still shown after cities are deselected/no longer hovered over

Train should "rotate" to point the right way

trains and their passengers shouldn't be immortal

We need higher resolution train

defeated soldiers should retreat or surrender

Add or kill soldier, and train arrivals shouldn't "reshuffle" soldiers, instead it should add to the front which needs the most and only shuffle that

Issue, it is possible to "Outflank" cities by rapidly switching attack vectors, this should not be possible, or we need to find a way to defend against it

We need a diplomacy system, right now everyone shoots at everyone, and everyone has a hostile border with everyone.

We need a "tension" diplomatic matrix

We need a "military access" diplomatic matrix

We need to highlight all neighbours of cities

Visualize the neighbouring cities of the selected city ... or just the hovered city ... or all of them

Implement the bonuses for the nations

day 17.5 3-1-2026 Workload 7 hours
=====

plan
--
Add UI mouse-over text (Mouse over text must have multiple lines, must be green, red, or white, must have a black background) Done, that was exceedingly difficult and took hours

Think about how money should be spent, write a note here with your thoughts, think about upkeep cost for units, recruitment cost, equipment cost Done

Think about how recruitment should work, keep in mind there will be different soldier types, write a note here with your thoughts

Add income to nations Done

Add income to UI with proper mouse-over text Done

Add auto-recruitment Failed, (out of time)

Thinking about money
-------
Money is the sinews of war, and must be represented as such.

Money is collected every month from cities

Money is spent each  time equipment or soldiers are produced, if money is not available the production/recruitment pauses

Soldiers have upkeep cost, which is deducted directly from revenues, there is currently no penalty to not paying your soldiers

For the minimum viable product, there is no equipment production, and the cost and upkeep cost of soldiers should be scaled up accordingly

Thinking about recruitment
-------
This is much more of a difficult matter

Recruitment is supposed to happen in cities, in randomly chosen core cities, whenever the army cap so permits.

So I need a reliable and fast way of picking a random core city which is not currently already recruiting a soldier

Recruitment should not happen if it would cause the army cap to be exceeded, it should count ongoing recruitments to the cap, so that if we have 3 soldiers before the cap, only 3 recruitments start

Nations need a "recruiting" soldier counter, which counts how many soldiers are currently undergoing recruitment, it is automatically incremented when recruitment start, and decremented if recruitment finishes or is interrupted

Recruitment has a timer, which builds up over 10 seconds (or whatever the time is going to be), it is visualized by a bar filling up

When the timer runs out, a new soldier spawn, auto-assigned to the city

Recruitment can be interrupted by the city where the recruitment is ongoing being captured

issues
--------------
The number renderer needs to be able to render decimal numbers
Same goes with the mouseover text

We should switch everything over to using in-game-dt, not real world dt (except camera movement)

Train should "rotate" to point the right way

trains and their passengers shouldn't be immortal

We need higher resolution train

defeated soldiers should retreat or surrender

Add or kill soldier, and train arrivals shouldn't "reshuffle" soldiers, instead it should add to the front which needs the most and only shuffle that

Issue, it is possible to "Outflank" cities by rapidly switching attack vectors, this should not be possible, or we need to find a way to defend against it

We need a diplomacy system, right now everyone shoots at everyone, and everyone has a hostile border with everyone.

We need a "tension" diplomatic matrix

We need a "military access" diplomatic matrix

We need to highlight all neighbours of cities

Visualize the neighbouring cities of the selected city ... or just the hovered city ... or all of them

Implement the bonuses for the nations

day 18.5 4-1-2026 
=====

plan
--
Get auto-recruitment working, this is probably a whole-day project Done, (it took almost the whole day)
* Recruitment should cost money
* There should be some kind of effect, showing that money has been spent, maybe a particle effect with the cost
* Recruitment should cancel when city is captured
* Recruitment should update every frame
* Recruitment should finish with the actual recruitment of a soldier

Make a recruitment menu button, where the player can choose to turn off recruitment, and in the future switch soldier type Done (it took surprisingly long)

Add an auto-balance armies button (Done)

Make an auto-balance armies function (this is going to take like forever, and take forever to run) Failed (But started, but out of time)

Make "AI" nations auto-balance every N days Failed (out of time)

add an auto-attack menu button Failed (out of time)

add auto attack Failed (out of time)

issues
--------------
Limit ingame timer to pass half a day when the game is lagging

Switch all time over from dt, to ingame timer

Train path is still shown after cities are deselected/no longer hovered over

Train should "rotate" to point the right way

trains and their passengers shouldn't be immortal

We need higher resolution train

defeated soldiers should retreat or surrender

Add or kill soldier, and train arrivals shouldn't "reshuffle" soldiers, instead it should add to the front which needs the most and only shuffle that

Issue, it is possible to "Outflank" cities by rapidly switching attack vectors, this should not be possible, or we need to find a way to defend against it

We need a diplomacy system, right now everyone shoots at everyone, and everyone has a hostile border with everyone.

We need a "tension" diplomatic matrix

We need a "military access" diplomatic matrix

We need to highlight all neighbours of cities

Visualize the neighbouring cities of the selected city ... or just the hovered city ... or all of them

Implement the bonuses for the nations

day 19.5 5-1-2026 workload 6 hours
=====
I won't have a lot of time to work today

plan
--

Get auto-balance front-lines to work (Done, that was surprisingly easy, but it lags A LOT, we need multithreading)

Implement country offensive stances (Done)

Have countries auto-attack based on their stance (Done)

Implement all implementable country bonuses (gonna take like forever) (Done)

Draw some countryball art, to use a thumbnail for the next video (Done)

Record a video (Done)


issues
--------------
Laggy autobalance

Buggy autobalance when autobalance triggers while autobalance trains are still driving does not count the countryballs already on the way to destinations

Limit ingame timer to pass half a day when the game is lagging

Switch all time over from dt, to ingame timer

Trains should stop if the city they try to drive to is no longer controlled

Train should "rotate" to point the right way

trains and their passengers shouldn't be immortal

We need higher resolution train

defeated soldiers should retreat or surrender

Add or kill soldier, and train arrivals shouldn't "reshuffle" soldiers, instead it should add to the front which needs the most and only shuffle that

Issue, it is possible to "Outflank" cities by rapidly switching attack vectors, this should not be possible, or we need to find a way to defend against it

We need a diplomacy system, right now everyone shoots at everyone, and everyone has a hostile border with everyone.

We need a "tension" diplomatic matrix

We need a "military access" diplomatic matrix

We need to highlight all neighbours of cities

Visualize the neighbouring cities of the selected city ... or just the hovered city ... or all of them

Implement the bonuses for the nations

day 21 6-1-2026 Workload less than 4 hours
=====

A very extended think about pathfinding
--------
Forget about multithreading pathfind, using Dijkstra's algorithm in a point-to-point manner is stupid

We need a completely new strategy for re-balancing the front, one which doesn't rely on finding the path from Moscow to Donetsk every 5 days

As I see it there are two options, one suggested by me, the other an AI generated suggestion.

My suggestion is a hub-and-spoke pathfinding system, where I pre-designate certain cities (maybe 100) as hubs, and pre-calculate the paths to the nearest hub, and betwixt hubs, and then piece together a path from that.

The AI suggestion is Dijkstra's algorithm once, with the front-lines as the "source", which essentially will generate paths from everywhere to the front-lines.

I must be honest, I like the AI suggestion bests, at least that sounds easiest to implement

plan
--
Implement a way of visualizing the way soldiers are going to move as a result of auto-balance front-lines Done

Implement Dijkstra's algorithm for filling the front-lines in its most basic form Done

Solve the trains-passing through enemy cities bug Done

Download some songs and make sure they are legal to use Done, a few only

Implement the music wrapper class, for use by the music manager Done

issues
--------------
Limit ingame timer to pass half a day when the game is lagging

Switch all time over from dt, to ingame timer

Train should "rotate" to point the right way

trains and their passengers shouldn't be immortal

We need higher resolution train

defeated soldiers should retreat or surrender

Add or kill soldier, and train arrivals shouldn't "reshuffle" soldiers, instead it should add to the front which needs the most and only shuffle that

Issue, it is possible to "Outflank" cities by rapidly switching attack vectors, this should not be possible, or we need to find a way to defend against it

We need a diplomacy system, right now everyone shoots at everyone, and everyone has a hostile border with everyone.

We need a "tension" diplomatic matrix

We need a "military access" diplomatic matrix

We need to highlight all neighbours of cities

Visualize the neighbouring cities of the selected city ... or just the hovered city ... or all of them

Implement the bonuses for the nations

day 22, 7-1-2026
=======

Music wishlist
--------------
Here are the songs I wish I could add in game, but only if public domain/Creative Commons recordings can be found

Warzawienko: No recordings found, not added, sadly, Whirlwinds of danger is great but not the right genre

British Grenadiers: We need an instrumental recording
Colonel Bogey march: Good recording: https://commons.wikimedia.org/wiki/File:Colonel_Bogey.ogg

Le Régiment de Sambre et Meuse: Is on Wikimedia

Thinking about music
---------
How the music should be integrated in the game is a difficult and delicate issue, which requires a lot of thinking

Let us start with how the finished product shall behave: It should very much be like the music manager in Paradox games.

* In the title screen, we will simply loop the title melodi.
* The player can click somewhere to mute music
* The player can open a settings menu, where they can modify the volume of the music and effects
* In the game, the music manager plays the music track until it finished, then it picks a random new track.
* The player can open a music manager window, which shows the progress of the current playing track, and all tracks, the player can click any tracks to play it from the beginning

Then how are we going to slowly build it...

* We need a music manager class, which is owned by the engine, and leased out to the scenes
* Before everything is loaded, the music manager loads all music from a json file
* The music manager has a `loopTitle()` function which sets whether or not the title loops
* The music manager `update()` function is called by the engine
* `update()` keeps a variable with the current playing song, it keeps track of whether a song is playing
* The graphical music manager ... is a completely different kettle of fish, which I likely won't get to today

Plan
------
Make music manager load music wrappers from JSon (Done)

Make the `loopTitle()` and `update()` functions Done

Make the title music loop in the title screen, then stop in the game Done

Make the random generator a global variable Done

Make music pick random tracks in game Done

Find a way of modifying the volume Done

Add a settings menu to modify the volume Done

Add a music button to the bottom bar (from the left), and the settings menu

Add the graphical music manager display, let us be clear, you won't have time to do this

Add functionalities to music manager, definitely out of scope for today

Add option to pause music

Legal/copyright issues
------
Check if non-derivatives clause prevents me from using the Preobrazhensky march in game